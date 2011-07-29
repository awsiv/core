CodeMirror.defineMode("CFEngine", function(config, parserConfig) {
    var indentUnit = config.indentUnit;
        
    var cftype, content;
    function ret(tp, style, cont) {
        cftype = tp;
        content = cont;
        return style;
    }
  
    
    
    function chain(stream, state, f) {
        state.tokenize = f;
        return f(stream, state);
    }
  
    function cfTokenBase(stream, state) {
        var ch = stream.next();
        if (/[{}(),:;\.|&!]/.test(ch)) {
            if (ch == ":" && stream.peek() == ":") {
                stream.next();               
                return ret("cf-punc","cf-punc",ch+':');
            }
            
            return ret("cf-punc","cf-punc",ch);
        }
        else if (/[\-=]/.test(ch) && stream.peek() == ">") {
            stream.next();
            return  ret("cf-punc", "cf-punc",stream.current());
        }
        else if (ch == '"') {
            
            return chain(stream, state, cfString);
        }
        else if (ch == "#") {
            while (!stream.eol()) stream.next();
            return ret("comment", "cf-comment");
        }
        else if ((ch == "@" || ch == "$") && stream.peek() == "(") {
            stream.next();
            while (!stream.eol() && stream.next() != ")");
            return ret("cf-expand","cf-expand");
        }
        else if (/[a-zA-Z0-9_]/.test(ch)) {
            while (stream.match(/^[a-zA-Z0-9_]/));
            return ret("cf-name", "cf-name",stream.current());
        }
        else {
            return ret("cf-error", "cf-error");
        }
        
        
    }
    
    function cfString(source, state) {
        var escaped = false;
        while (!source.eol()) {
            source.eatSpace();
            var next = source.next();
            if (next == '"' && !escaped) {
                state.tokenize= cfTokenBase;
                break;
            }
            escaped = !escaped && next == "\\";
        }
        return ret("string", "cf-string",source.current());
    }
    
    
    
    //states
    
    /** modify the array  and the synatx utility */
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
  
    /* make * of bundle synatax apear on all bundle for checking */
    forEachIn(bundle_syntax, function(name, bundle) {
        var all = bundle["*"];
        if (all) {
            forEachIn(bundle, function(name1, cls) {
                forEachIn(all, function(name, prop){
                    if (!cls.hasOwnProperty(name)) cls[name] = prop;
                });
            });
        }
        forEachIn(bundle, function(name, cls) {
            if (name != "*") forEachIn(cls, explore);
        });
    });
    
    
    
    
    
    
    
    
    
    
    
    
    
    var empty = {};
    function lookupProp(parent, name) {
        
        //console.log('lookup property',parent,name);
        if (parent.hasOwnProperty(name)) {
            mark("cf-keyword");                
            return parent[name];
        }
        else {
           
            mark("cf-error");
            return empty;
        }
    }
    
    
    function enterContext(type) {
        cx.context = {
            type: type, 
            prev: cx.context, 
            base: cx.indented, 
            start: cx.column
        };
    
    }
    function leaveContext() {
        if (cx.context)  cx.context = cx.context.prev;
    }

  
    
    function pass() {
        push(arguments);
        cx.consume = false;
    }
    
    function errIf(check) {
          
        if (check)    mark("cf-error");
    }
    
    function name(st) {
      
        errIf(st != "cf-name");
        cont();
    
    }
        
        
        
    
    function testAhead(val) {
        try {
            
            
            var backupStream = cx.stream;
            // console.log(val);
            
            var _regex = new RegExp( '^[ ]*' + val + '(?!:)[ ,]{0,1}', "i");
            var testString = backupStream.match(_regex,false);
            // console.log( _regex);
            if (testString) {
                //console.log('matched ..')
                return true;
            } 
            // console.log('not matched ..');
            return false;
            
        }
        catch (e) {
            if (e != StopIteration) throw e;
        }
    }
    
    
    function maybeclassop(st, v) {
        if (v == "&" || v == "|" || v == ".") cont(classexpr);
        else pass();
    }
    
    function classexpr(st, v) {
        if (st == "cf-name") cont(maybeclassop);
        else if (v == "(") {
            enterContext("parens");
            cont(classexpr, endparens, maybeclassop);
        }
        else if (v == "!") cont(classexpr);
        else pass();
    }
    
    function expect(punc) {
        
        return function(st, v) {
            if (v == punc) cont(); else  errIf(true);
        };
    }
    
    function bundleItemProp(sec, first) {
        return function(st, v) {
            // console.log("in bundle item property",st,v,sec,first )
            
            if (v == ";") {
                leaveContext();
                cont();
            }
            else if (v == ",") cont(prop(sec), bundleItemProp(sec));
            else if (first) pass(prop(sec), bundleItemProp(sec));
        };
    }
    
    function prop(sec) {
        return function(st, v) {
            
           
            cx.expected = sec;
            //console.log ('prop funciton call with sec',sec);
            if (st == "cf-name")  {
                cont(expect("=>"), value(lookupType(sec, v)));
            }
        };
    }
    
    
    var unspecifiedType = {
        type: {}, 
        re: /.*/, 
        list: null
    };
    
    var valueAny = value(unspecifiedType);
    function maybecall(st, v) {
        if (v == "(") {
            enterContext("parens");
            cont(listOf(valueAny, ")"));
        }
        else pass();
    }
    
    function endparens(st, v) {
        if (v == ")") {
            leaveContext();
            cont();
        }
    }
    
    function listOf(elem, end) {
        
        
        function proceed(st, v) {
            //console.log('list of',v,elem);
            if (v == end) {
                leaveContext();
                cont();
            }
            else if (v == ",") cont(elem, proceed);
        }
        return function(st, v) {
            //console.log('main list of ',st,v)
            if (v == end) {
                leaveContext();
                cont();
            }
            else pass(elem, proceed);
        };
    }
    
    
    
    function typedArg(type) {
        return function(st, v) {
            // console.log(type,st,v);
            cx.expected = type.type; 
            if (v == "}") {
                pass();
            }
            if (st == "cf-expand") cont();
            else if (st == "cf-string") {
                errIf(!typeMatch(type, v));
                cont();
            }
            else if (type.type && type.type.datatype == "clist") {
                //  console.log('clist enter',st,v,type);
                if (st=='cf-punc')  {
                    pass();
                }
                pass(value(empty));
            }
        };
    }
    
    function typeMatch(type, str) {
        //  console.log('type match',type,str);
        // if (!type.re) type.re =  '';
        return /[$@]\(/.test(str) || type.re.test(str);
    }
    
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
  
    
    function lookupType(parent, name) {
        //console.log('parent --->',parent,name);
        var prop = lookupProp(parent, name);
        //console.log('prop',prop);
        return prop == empty ? unspecifiedType : interpretType(prop);
    }
    
    function value(type) {
        return function(st, v) {
            
            
            if (type.list === false) cx.expected = type.type;
            if (v == "{") {
                //    console.log('value --> ',type,v,st)
                errIf(type.list === false);
                enterContext("list");
                cont(listOf(typedArg(type), "}"));
            }
            else if (st == "cf-string") {
                //errIf(type.list === true || !typeMatch(type, v));
                cont();
            }
            else if (st == "cf-name") cont(maybecall);
        };
    }
    
    function promisee(st, v) {
        if (st == "cf-string") cont();
        else if (v == "{") {
            enterContext("list");
            cont(listOf(valueAny, "}"));
        }
    }
    
    function bundleItem(sec) {
        return function(st, v) {
            // console.log('in --bundel item ',st,v);
            
            if (v == "->") cont(promisee, bundleItemProp(sec, true));
            else pass(bundleItemProp(sec, true));
        };
    }
    
    function bundle1(btype) {
        function bundleTop(sec) {
            return function(st, v, token) {
                cx.expected = btype;
               
                if (v == "}") {
                    leaveContext();
                    cont();
                }
                else if (st == "cf-name" && testAhead(":")) {
                    // console.log("lookadeahd pass",st,v);
                    cont(function(){
                        cont();
                    }, bundleTop(lookupProp(btype, v)));
                
                }
                
                else if (st == "cf-string" && sec) {
                    // console.log('cf-string detected ',st,v);
                    enterContext("item");
                    cont(bundleItem(sec), bundleTop(sec));
                }
                else {
                    // console.log('class exp');
                    pass(classexpr, expect("::"), bundleTop(sec)); 
                }
            };
        }
     
        return function(st, v) {
         
            if (v == "{") {              
                enterContext("bundle"), cont(bundleTop(null));
            }
        };
    }
    
    function maybeargs(st, v) {
        if (v == "(") {
            enterContext("parens");
            cont(listOf(name, ")"));
        }
        else pass();
    }
    
    function bundle(st, v) {
        cx.expected = bundle_syntax;      
        if (st == "cf-name") cont(name, maybeargs,bundle1(lookupProp(bundle_syntax, v)));
    }
    
    function merge_options(obj1,obj2){
        var obj3 = {};
        for (attrname in obj1) {
            obj3[attrname] = obj1[attrname];
        }
        for (attrname in obj2) {
            obj3[attrname] = obj2[attrname];
        }
        return obj3;
    }
    
    /* body parsing */
    
    function body(st, v) {
        var e = merge_options(body_control_syntax,bodyTypes);
        cx.expected = e; 
        
        if (st == "cf-name") {
            var normal = lookupProp(bodyTypes, v), control = body_control_syntax[v] || empty;
            
            if (control != empty) mark("cf-keyword");
            cont(function (st, v) {
                 cx.expected = {'control':true}; 
                if (st == "cf-name") {
                    if (v == "control") {
                        errIf(control == empty);
                        cont(maybeargs, body1(control));
                    }
                    else {
                         
                        errIf(normal == empty);
                        cont(maybeargs, body1(normal));
                    }
                }
            });
        }
    }
    
    
    function body1(type, control) {
        function body2(st, v) {
            cx.expected = type;
           
            if (v == "}") {
                leaveContext();
                cont();
            }
            else if (st == "cf-name" && testAhead("=>"))  {
          
                cont(expect("=>"), value(lookupType(type, v)), expect(";"), body2);
            }
            else pass(classexpr, expect("::"), body2);
        }
        return function(st, v) {
            // console.log  
            if (v == "{") {
                enterContext("body");
                cont(body2);
            }
        }
    }
    
    
  
    function top(st, v) {
        cx.expected = {
            body: true, 
            bundle: true
        };
        
       
        if (st == "cf-name") {
            if (v == "body") {
                mark("cf-section");
                cont(body, top);
            }
            else if (v == "bundle") {
                mark("cf-section");               
                cont(bundle, top);
            } else pass();
           
        } 
        
        
        
    }
    
    function mark(style) {
        cx.marked = style;
    }
    function cont() {
        push(arguments);
        cx.consume = true;
        
    }
    
    function push(fs) {
        for (var i = fs.length - 1; i >= 0; i--) cx.cc.push(fs[i]);
    }
  
  
  
  
  
    var cx = {
        state: null, 
        column: 0,
        indented:0,
        marked: null, 
        cc: null,
        consume:null,
        stream: '',
        context:null,
        expected:[],
        content:''
    };
    
    function parseCf(state,style, type, content, stream) {       
        //consume = marked = expected = null;
        
        var cc = state.cc;
        // state.expected = cx.expected;
        // console.log(cc);
        
        // Communicate our context to the combinators.
        cx.state = state;
        cx.stream = stream;
        cx.marked = null;
        cx.cc = cc;
        //cx.expected = expected;
       
      
        
        var loopCounter = 0;
        while(true) {
            loopCounter++;
            currentStream = stream;
            var combinator = cc.length ? cc.pop():top;
            if (!combinator) return ret('error','cf-error');
            combinator(style, content);  
            if (cx.expected) state.expected = cx.expected;
            state.context = cx.context;
            
            if (cx.consume == null) {
                cx.cc.push(combinator);  
                return ret('error','cf-error');
            } else if (cx.consume) {                 
                if (cx.marked) {  
                    return ret(cx.marked,cx.marked);  
                } 
                return style;
            }
            if (loopCounter > 10 )  return ret('error','cf-error');
           
        }
        
        
        return ret('error','cf-error');
    }
    
    return {
       
        startState: function(basecolumn) {
            return {
                tokenize:cfTokenBase,
                cc: [],
                context: null,
                indented: 0,
                expected:[]
            };
        },
        
        token: function(stream, state) {
            
            if (stream.eatSpace()) return null;
             
            var style = state.tokenize(stream, state);
            state.content = content;
           
            if (style == "whitespace" || style == "cf-comment" ||style == "cf-error") return style;
           
            $return =   parseCf(state,style,cftype, content, stream);
            return $return;
        },
        
        indent: function(state, textAfter) {
           
           
           
        },
        electricChars: "}"
            
    };
});

CodeMirror.defineMIME("text", "CFEngine");
