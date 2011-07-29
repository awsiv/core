// Minimal event-handling wrapper.
function stopEvent() {
    if (this.preventDefault) {
        this.preventDefault();
        this.stopPropagation();
    }
    else {
        this.returnValue = false;
        this.cancelBubble = true;
    }
}
function addStop(event) {
    if (!event.stop) event.stop = stopEvent;
    return event;
}
function connect(node, type, handler) {
    function wrapHandler(event) {
        handler(addStop(event || window.event));
    }
    if (typeof node.addEventListener == "function")
        node.addEventListener(type, wrapHandler, false);
    else
        node.attachEvent("on" + type, wrapHandler);
}

function forEach(arr, f) {
    for (var i = 0, e = arr.length; i < e; ++i) f(arr[i]);
}



startComplete= function(tab_id) {
    // We want a single cursor position.
    var editor=editors[tab_id];
    if(!editor) return;
    if (editor.somethingSelected()) return;
    var cur = editor.getCursor();
    var to = {
        'line':cur.line,
        'ch':cur.ch-1
    };

    var textBefore = editor.getRange(to,cur);

    var token = editor.getTokenAt(cur);

    var content = token.state.content;

    var text = textBefore;
    var added = false;
    if (/(?:^|[\s\)\}\"])$/.test(text)) {
        added = true;
    }

    var expected = [];
    if (!added)
        expected = token.state.expected;
    else {

        editor.replaceRange('x ', cur);
        var state = editor.getStateAfter(cur.line);
        expected = state.expected;
        var   temp =  editor.getCursor();
        var tempTo = temp;
        tempTo.ch = temp.ch;
        editor.replaceRange('',cur,tempTo);
    }

    var completions = getCompletions(expected);
    var match=[];



    for (var i = 0; i < completions.length; i++) {
        var str = completions[i];
        if (added === false ) {
            if (str.indexOf(content) != 0) continue;
        }
        match.push(str);
    }

    match.sort();


    if (!match.length) return;


    function insert(str) {
        if (added===true) str = textBefore + str;
        editor.replaceRange(str, {
            line: cur.line,
            ch: token.start
        }, {
            line: cur.line,
            ch: token.end
        });
        return;
    }
    // When there is only one completion, use it directly.
    if (match.length == 1) {
        insert(match[0]);
        return true;
    }

    // Build the select widget
    var complete = document.createElement("div");
    complete.className = "cm-completions";
    var sel = complete.appendChild(document.createElement("select"));
    sel.multiple = false;
    for (var i = 0; i < match.length; ++i) {
        var opt = sel.appendChild(document.createElement("option"));
        opt.appendChild(document.createTextNode(match[i]));
    }
    sel.firstChild.selected = true;
    sel.size = Math.min(10, match.length);
    var pos = editor.cursorCoords();
    complete.style.left = pos.x + "px";
    complete.style.top = pos.yBot + "px";
    document.body.appendChild(complete);
    // Hack to hide the scrollbar.
    if (completions.length <= 10)
        complete.style.width = (sel.clientWidth - 1) + "px";

    var done = false;
    function close() {
        if (done) return;
        done = true;
        complete.parentNode.removeChild(complete);
    }
    function pick() {
        var index = sel.selectedIndex;
        var val = sel.options[index].text; // should be for ie
        
        insert(val);
        
        setTimeout(function(){
            close();
            editor.focus();
        }, 50);
    }
    
    // for IE delay the bindings
    setTimeout(function(){
        sel.focus();
        
        connect(sel, "keydown", function(event) {
            var code = event.keyCode;                        
            // Enter and space
            if (code == 13 || code == 32) {
                event.stop();
                pick();
            }
            // Escape
            else if (code == 27) {
                event.stop();
                close();
                editor.focus();
            }
            else if (code != 38 && code != 40) {
                close();
                editor.focus();
                setTimeout(startComplete, 50);
            }
        });
        connect(sel, "dblclick", pick);
      
        connect(sel, "blur", function(e){
            close(e)
        });
                 
                        
                        
    }, 5);

  
    return true;

}

function getCompletions(expected) {
    if (expected == null)
        return [];
    else if (expected.pcre_range)
        return expected.pcre_range.indexOf("|") == -1 ? []
        : map(function(x){
            return "\"" + x + "\"";
        }, expected.pcre_range.split("|"));
    else
        return propertiesOf(expected);

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

