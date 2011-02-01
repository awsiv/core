// Completion

function initComplete(editor) {
  editor.win.addEventHandler(editor.win.document, "keydown", function(e) {
    if (e.keyCode == 32 && e.ctrlKey) {
      e.stop();
      var coords = editor.cursorCoords(), pos = editor.cursorPosition();
      if (!coords || !pos) return;
      completionUI(editor, getCompletions(editor, pos), {x: coords.x, y: coords.yBot});
    }
  });
  editor.win.addEventHandler(editor.win.document, "keypress", function(e) {
    if (e.code == 32 && e.ctrlKey) e.stop();
  });
}

function completionUI(editor, data, point) {
  function insert(str) {
    var before = data.line.slice(0, data.pos.character - data.lead.length),
        after = data.line.slice(data.pos.character),
        re = (str.charAt(0) == "\"" ? /^"(?:[^\"\\]|\\.)*"(.*)$/ : /^[a-zA-Z0-9_]+(.*)$/),
        match = after.match(re);
    if (match) after = match[1];
    editor.setLineContent(data.pos.line, before + str + after);
    editor.selectLines(data.pos.line, before.length + str.length);
  }

  var list = data.completions, len = list.length, done = false, cutoff = 30;
  if (!len) return;
  if (len == 1) return insert(list[0]);
  
  var div = document.createElement("DIV");
  var elt = div.appendChild(document.createElement("SELECT"));
  elt.size = Math.min(len, cutoff);
  div.style.position = "absolute";
  div.className = "completions";
  for (var i = 0; i < len; i++) {
    var opt = elt.appendChild(document.createElement("OPTION"));
    opt.appendChild(document.createTextNode(list[i]));
  }
  elt.firstChild.selected = true;
  div.style.left = point.x + "px";
  div.style.top = point.y + "px";
  document.body.appendChild(div);
  // Hack to hide the select's scrollbar in FF
  if (len <= cutoff) div.style.width = elt.clientWidth + "px";
  editor.win.addEventHandler(elt, "blur", close);
  elt.focus();
  // Opera sometimes seem to refuse to focus the element right away
  if (window.opera)
    setTimeout(function(){if (!done) elt.focus();}, 100);
  editor.win.addEventHandler(elt, "keydown", function(event) {
    if (event.keyCode == 13 || event.keyCode == 32) {event.stop(); pick();}
    else if (event.keyCode == 27) {event.stop(); close(); editor.focus();}
  });
  editor.win.addEventHandler(elt, "keypress", function(event) {
    if (event.keyCode == 13 || event.code == 32) event.stop();
    else if (event.keyCode == 27) event.stop();
  });
  editor.win.addEventHandler(elt, "dblclick", pick);

  function pick() {
    if (elt.selectedIndex == -1) return;
    insert(elt.options[elt.selectedIndex].firstChild.nodeValue);
    close();
  }
  function close() {
    if (done) return;
    done = true;
    document.body.removeChild(div);
  }
}

function getCompletions(editor, pos) {
  var br = pos.line, line = editor.lineContent(br), textBefore = line.substr(0, pos.character);
  var w = editor.win, e = editor.editor, added = false;
  if (br && !br.parserFromHere) {
    if (!e.highlightAtCursor()) return [];
    br = editor.cursorLine();
  }

  var text = textBefore;
  if (/(?:^|[\s\)\}\"])$/.test(text)) {text += "x"; added = true;}
  var stream = w.stringStream(w.singleStringIter(text)),
      parser = br ? br.parserFromHere(stream) : w.CfengineParser.make(stream),
      token = w.lastToken(parser);

  text = added ? token.content.slice(0, token.content.length - 1) : token.content;
  var match = [], len = text.length, compl = parser.completions(token.expected);
  if (compl.length != 0 && len != 0)
    for (var i = 0; i < compl.length; i++) {
      var str = compl[i];
      if (str.length >= len && str.slice(0, len) == text) match.push(str);
    }
  else
    match = compl;
  match.sort();
  return {completions: match, line: line, pos: pos, lead: text};
}

// Resizing

function initResize(editor) {
  var wrap = editor.wrapping;
  var marker = document.createElement("IMG");
  marker.src = "/images/resize.png";
  marker.style.position = "absolute";
  marker.style.right = marker.style.bottom = "0px";
  marker.style.cursor = "pointer";
  wrap.appendChild(marker);

  var connect = editor.win.addEventHandler;

  function computedStyle(elt) {
    if (elt.currentStyle) return elt.currentStyle;
    else if (window.getComputedStyle) return window.getComputedStyle(elt, null);
  }

  connect(marker, "mousedown", function(event) {
    event.stop();
    var startX = event.screenX, startY = event.screenY;
    var style = computedStyle(wrap);
    var startWidth = (style && parseInt(style["width"], 10)) || wrap.offsetWidth,
        startHeight = (style && parseInt(style["height"], 10)) || wrap.offsetHeight;
    var unreg = [];

    function stop(event) {
      event.stop();
      for (var i = 0; i < unreg.length; i++) unreg[i]();
    }
    function move(event) {
      wrap.style.width = (startWidth + event.screenX - startX) + "px";
      wrap.style.height = (startHeight + event.screenY - startY) + "px";
	  //added to resize the editor's parent node
	  wrap.parentNode.style.width=(startWidth + event.screenX - startX+30)+"px";
	  wrap.parentNode.style.height=(startHeight + event.screenY - startY+10) + "px";
    }
    function reg(node, ev, func) {
      unreg.push(connect(node, ev, func, true));
    }
    reg(document, "mouseup", stop);
    reg(editor.win.document, "mouseup", stop);
    reg(document, "mousemove", move);
    reg(editor.win.document, "mousemove", move);
  });
  // Stop IE from screwing up our events.
  connect(marker, "drag", function(event){event.stop();});
}

// Change tracking

(function() {
  var lastHist = [], firstClear, ed = null;

  window.initChangeTracker = function(editor) {
    ed = editor;
    for (var l = editor.firstLine(); l !== false; l = editor.nextLine(l)) {
      if (l) l.cfClear = editor.lineContent(l);
      else firstClear = editor.lineContent(l);
    }
  }

  window.contentChanged = function() {
    var hist = ed.editor.history.history;
    for (var common = Math.min(lastHist.length, hist.length) - 1; common >= 0; common--)
      if (hist[common] == lastHist[common]) break;
    while (lastHist.length > common + 1) // TODO try to revert marking
      lastHist.pop();
    while (hist.length > common + 1) {
      var elt = hist[++common];
      lastHist.push(elt);
      for (var i = 0; i < elt.length; i++) {
        var ch = elt[i], start = ch[0].from, end = ch[ch.length - 1].to;
        while (start !== false && start != end) {
          var clear = start ? start.cfClear : firstClear;
          if (clear != null && clear != ed.lineContent(start)) {
            if (start) start.cfClear = null;
            else firstClear = null;
          }
          try {start = ed.nextLine(start);}
          catch (e) {break;}
        }
      }
    }
  };

  window.styleLineNum = function(div, node, num) {
    div.style.backgroundColor =
      (typeof num == "number" && (node ? node.cfClear : firstClear) == null) ? "#eef" : "";
  };
})();
