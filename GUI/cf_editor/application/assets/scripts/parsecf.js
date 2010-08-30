var CfengineParser = Editor.Parser = (function() {
  var tokenizeCfengine = (function() {
    function normal(source, setState) {
      var ch = source.next();
      if (/[{}(),:;\.|&!]/.test(ch)) {
        if (ch == ":" && source.peek() == ":") source.next();
        return "cf-punc";
      }
      else if (/[\-=]/.test(ch) && source.peek() == ">") {
        source.next();
        return "cf-punc";
      }
      else if (ch == "\"") {
        setState(string);
        return null;
      }
      else if (ch == "#") {
        while (!source.endOfLine()) source.next();
        return "cf-comment";
      }
      else if ((ch == "@" || ch == "$") && source.peek() == "(") {
        source.next();
        while (!source.endOfLine() && source.next() != ")");
        return "cf-expand";
      }
      else if (/[a-zA-Z0-9_]/.test(ch)) {
        while (source.matches(/[a-zA-Z0-9_]/)) source.next();
        return "cf-name";
      }
      else {
        return "cf-error";
      }
    }

    function string(source, setState) {
      var escaped = false;
      while (!source.endOfLine()) {
        var next = source.next();
        if (next == "\"" && !escaped) {
          setState(normal);
          break;
        }
        escaped = !escaped && next == "\\";
      }
      return "cf-string";
    }

    return function(source, startState) {
      return tokenizer(source, startState || normal);
    };
  })();

  function forEachIn(obj, func) {
    for (var n in obj)
      if (obj.hasOwnProperty(n))
        func(n, obj[n]);
  }
  function propertiesOf(obj) {
    var result = [];
    for (var n in obj)
      if (obj.hasOwnProperty(n)) result.push(n);
    return result;
  }
  function map(func, arr) {
    var res = [], len = arr.length;
    for (var i = 0; i < len; i++)
      res.push(func(arr[i]));
    return res;
  }

  var bodyTypes = {};
  function explore(name, obj) {
    if (typeof obj == "object" && !obj.datatype) {
      bodyTypes[name] = obj;
      forEachIn(obj, explore);
    }
  }
  forEachIn(bundle_syntax, function(name, bundle) {
    var all = bundle["*"];
    if (all) {
      forEachIn(bundle, function(name1, cls) {
        forEachIn(all, function(name, prop){if (!cls.hasOwnProperty(name)) cls[name] = prop;});
      });
    }
    forEachIn(bundle, function(name, cls) {if (name != "*") forEachIn(cls, explore);});
  });

  function indentCfengine(context) {
    return function(text) {
      var c = context;
      if (/^\s*\}/.test(text)) {
        while (c && c.type == "item") c = c.prev;
        if (c) c = c.prev;
      }
      if (!c) return 0;
      else if (c.type == "list" || c.type == "parens") return c.start;
      else if (c.type == "bundle" && /^\s*\"/.test(text)) return c.base + 2 * indentUnit;
      else return c.base + indentUnit;
    };
  }

  var unspecifiedType = {type: {}, re: /.*/, list: null};
  function typeToRegexp(type) {
    var dt = type.datatype, re = type.pcre_range;
    if (dt == "int" || dt == "ilist")
      return re.charAt(0) == "-" ? /^\"-?\d+\"$/ : /^\"\d+\"$/;
    else if (dt == "real" || dt == "rlist")
      return /^\"[+\-]?\d*(?:.\d+)?(?:[eE]\d+)?\"$/;
    else if (!re)
      return /.*/;
    else
      return new RegExp("^\"(?:" + re + ")\"$");
  }
  parent.qq = typeToRegexp;
  function interpretType(type) {
    if (!type.pcre_range) return unspecifiedType;
    return {
      type: type, 
      re: type.re || (type.re = typeToRegexp(type)),
      list: (type.datatype && /list\)?$/.test(type.datatype) ? true : false)
    };
  }

  function parseCfengine(source) {
    var tokens = tokenizeCfengine(source), lookahead = [];
    var cc = [top], context = null;
    var column = 0, indented = 0, inString = false;
    var consume, marked, expected;
  
    var parser = {next: next, copy: copy, completions: completions};
    function completions(expected) {
      if (expected == null)
        return [];
      else if (expected.pcre_range)
        return expected.pcre_range.indexOf("|") == -1 ? []
               : map(function(x){return "\"" + x + "\"";}, expected.pcre_range.split("|"));
      else
        return propertiesOf(expected);
    }
    function next() {
      var token = lookahead.length ? lookahead.shift() : tokens.next();
      if (token.style == "whitespace" && column == 0)
        indented = token.value.length;
      column += token.value.length;
      if (token.content == "\n") {
        indented = column = 0;
        token.indentation = indentCfengine(context);
      }
      if (token.style == "whitespace" || token.style == "cf-comment" || token.style == "cf-error")
        return token;
      else if (token.style == "cf-string") {
        if (inString) return token;
        inString = true;
      }
      else inString = false;
      
      while(true) {
        consume = marked = expected = null;
        var f = cc.pop();
        f(token.style, token.content, token);
        if (expected) token.expected = expected;
        if (consume == null) {
          cc.push(f);
          token.style = "cf-error";
          return token;
        }
        else if (consume) {
          if (marked) token.style = marked;
          return token;
        }
      }
    }
    function copy() {
      var _context = context, _cc = cc.concat([]), _tokenState = tokens.state, _inString = inString;
      return function(input) {
        context = _context;
        cc = _cc.concat([]);
        column = indented = 0;
        inString = _inString;
        lookahead = [];
        tokens = tokenizeCfengine(input, _tokenState);
        return parser;
      };
    }

    function testAhead(val) {
      try {
        while (true) {
          var next = tokens.next();
          lookahead.push(next);
          if (next.style != "whitespace" && next.style != "cf-comment" && next.style != "cf-error")
            return next.content == val;
        }
      }
      catch (e) {if (e != StopIteration) throw e;}
    }

    function enterContext(type) {context = {type: type, prev: context, base: indented, start: column};}
    function leaveContext() {context = context.prev;}

    function push(fs) {for (var i = fs.length - 1; i >= 0; i--) cc.push(fs[i]);}
    function cont() {push(arguments); consume = true;}
    function pass() {push(arguments); consume = false;}
    function mark(style) {marked = style;}
    function errIf(check) {if (check) mark("cf-error");}

    function expect(punc) {
      return function(st, v) {if (v == punc) cont();};
    }

    function name(st) {errIf(st != "cf-name"); cont();}
    function string(st) {errIf(st != "cf-string"); cont();}
    function listOf(elem, end) {
      function proceed(st, v) {
        if (v == ",") cont(elem, proceed);
        else if (v == end) {leaveContext(); cont();}
      }
      return function(st, v) {
        if (v == end) {leaveContext(); cont();}
        else pass(elem, proceed);
      };
    }
    function typedArg(type) {
      return function(st, v) {
        expected = type.type;
        if (st == "cf-expand") cont();
        else if (st == "cf-string") {errIf(!typeMatch(type, v)); cont();}
        else if (type.type && type.type.datatype == "clist") pass(value(empty));
      };
    }

    var empty = {};
    function lookupProp(parent, name) {
      if (parent.hasOwnProperty(name)) {mark("cf-keyword"); return parent[name];}
      else {mark("cf-error"); return empty;}
    }
    function lookupType(parent, name) {
      var prop = lookupProp(parent, name);
      return prop == empty ? unspecifiedType : interpretType(prop);
    }
    function typeMatch(type, str) {
      return /[$@]\(/.test(str) || type.re.test(str);
    }

    function top(st, v) {
      expected = {body: true, bundle: true};
      if (st == "cf-name") {
        if (v == "body") {mark("cf-section"); cont(body, top);}
        else if (v == "bundle") {mark("cf-section"); cont(bundle, top);}
      }
    }
    function body(st, v) {
      expected = bodyTypes;
      if (st == "cf-name") {
        var normal = lookupProp(bodyTypes, v), control = body_control_syntax[v] || empty;
        if (control != empty) mark("cf-keyword");
        cont(function (st, v) {
          if (st == "cf-name") {
            if (v == "control") {errIf(control == empty); cont(maybeargs, body1(control));}
            else {errIf(normal == empty); cont(maybeargs, body1(normal));}
          }
        });
      }
    }
    function maybeargs(st, v) {
      if (v == "(") {enterContext("parens"); cont(listOf(name, ")"));}
      else pass();
    }
    function body1(type, control) {
      function body2(st, v) {
        expected = type;
        if (v == "}") {leaveContext(); cont();}
        else if (st == "cf-name" && testAhead("=>")) cont(expect("=>"), value(lookupType(type, v)), expect(";"), body2);
        else pass(classexpr, expect("::"), body2);
      }
      return function(st, v) {
        if (v == "{") {enterContext("body"); cont(body2);}
      }
    }
    function value(type) {
      return function(st, v) {
        if (type.list === false) expected = type.type;
        if (v == "{") {errIf(type.list === false); enterContext("list"); cont(listOf(typedArg(type), "}"));}
        else if (st == "cf-string") {errIf(type.list === true || !typeMatch(type, v)); cont();}
        else if (st == "cf-name") cont(maybecall);
      };
    }
    var valueAny = value(unspecifiedType);
    function maybecall(st, v) {
      if (v == "(") {enterContext("parens"); cont(listOf(valueAny, ")"));}
      else pass();
    }
    function bundle(st, v) {
      expected = bundle_syntax;
      if (st == "cf-name") cont(name, maybeargs, bundle1(lookupProp(bundle_syntax, v)));
    }
    function bundle1(btype) {
      function bundleTop(sec) {
        return function(st, v, token) {
          expected = btype;
          if (v == "}") {leaveContext(); cont();}
          else if (st == "cf-name" && testAhead(":")) cont(function(){cont();}, bundleTop(lookupProp(btype, v)));
          else if (st == "cf-string" && sec) {enterContext("item"); cont(bundleItem(sec), bundleTop(sec));}
          else pass(classexpr, expect("::"), bundleTop(sec));
        };
      }
      return function(st, v) {
        if (v == "{") {enterContext("bundle"), cont(bundleTop(null));}
      };
    }
    function classexpr(st, v) {
      if (st == "cf-name") cont(maybeclassop);
      else if (v == "(") {enterContext("parens"); cont(classexpr, endparens, maybeclassop);}
      else if (v == "!") cont(classexpr);
      else pass();
    }
    function endparens(st, v) {
      if (v == ")") {leaveContext(); cont();}
    }
    function maybeclassop(st, v) {
      if (v == "&" || v == "|" || v == ".") cont(classexpr);
      else pass();
    }
    function bundleItem(sec) {
      return function(st, v) {
        if (v == "->") cont(promisee, bundleItemProp(sec, true));
        else pass(bundleItemProp(sec, true));
      };
    }
    function promisee(st, v) {
      if (st == "cf-string") cont();
      else if (v == "{") {enterContext("list"); cont(listOf(valueAny, "}"));}
    }
    function bundleItemProp(sec, first) {
      return function(st, v) {
        if (v == ";") {leaveContext(); cont();}
        else if (v == ",") cont(prop(sec), bundleItemProp(sec));
        else if (first) pass(prop(sec), bundleItemProp(sec));
      };
    }
    function prop(sec) {
      return function(st, v) {
        expected = sec;
        if (st == "cf-name") cont(expect("=>"), value(lookupType(sec, v)));
      };
    }
      
    return parser;
  }

  return {make: parseCfengine, electricChars: "}"};
})();

// These have to be defined in the frame, since IE is weird about
// exceptions travelling between frames.
function lastToken(parser) {
  try {while (true) {var token = parser.next();}}
  catch (e) {if (e != StopIteration) throw e;}
  return token;
}
function singleStringIter(string) {
  var done = false;
  return {
    next: function() {
      if (done) throw StopIteration;
      else {done = true; return string;}
    }
  };
}
