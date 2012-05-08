/*
 * CFEngine Mission Common Utility functions
 */
var common = (function(){

    return {

        colours: {
            red: '#cc4358',
            yellow: '#f0d76a',
            green: '#779a62',
            blue: '#476e8c',
            black: '#000000',
            white: '#ffffff'
        },

        time: {
            // e.g. Jan 25th, 2008
            formatDate: function(javascriptTimestamp) {
                var date = new Date(javascriptTimestamp);
                return date.format('M dS Y');
            },

            // e.g. 22:42
            formatTimeOfDay: function(javascriptTimestamp) {
                var date = new Date(javascriptTimestamp);

                var hours = date.getHours() + "";
                var mins = date.getMinutes() + "";

                if (hours.length < 2) {
                    hours = "0" + hours;
                }

                if (mins.length < 2) {
                    mins = "0" + mins;
                }

                return hours + ":" + mins;
            },
            formatGMTOffset:function(javascriptTimestamp) {
                var date = new Date(javascriptTimestamp);
                return date.format('P');
            },

            format: function(javascriptTimestamp) {
                return common.time.formatDate(javascriptTimestamp) + ' ' +
                    common.time.formatTimeOfDay(javascriptTimestamp) + ' (GMT ' + common.time.formatGMTOffset(javascriptTimestamp) + ')';
            }
        },

        arrayWithValue: function(value, length) {
            var arr = new Array(length);
            for (var i = 0; i < arr.length; i++) {
                arr[i] = value;
            }
            return arr;
        },

        canonify: function(expression) {
            return expression.replace(/[^A-Za-z0-9]+/g, '_');
        },

        isValidClassExpression: function(expression) {
            return !/,/.test(expression);
        },

        randomUUID: function() {
            var S4 = function() {
                return (((1+Math.random())*0x10000)|0).toString(16).substring(1);
            };
            return (S4()+S4()+"-"+S4()+"-"+S4()+"-"+S4()+"-"+S4()+S4()+S4());
        },

        unixTimeToJavascriptTime: function(unixTime) {
            return unixTime * 1000;
        },

        javascriptTimeToUnixTime: function(javascriptTime) {
            return javascriptTime / 1000;
        }
    };
})();


var mediator = (function(){
    var subscribe = function(channel, fn){
        if (!mediator.channels[channel]) mediator.channels[channel] = [];
        mediator.channels[channel].push({ context: this, callback: fn });
        return this;
    },

    publish = function(channel){
        if (!mediator.channels[channel]) return false;
        var args = Array.prototype.slice.call(arguments, 1);
        for (var i = 0, l = mediator.channels[channel].length; i < l; i++) {
            var subscription = mediator.channels[channel][i];
            subscription.callback.apply(subscription.context, args);
        }
        return this;
    };

    return {
        channels: {},
        publish: publish,
        subscribe: subscribe,
        installTo: function(obj){
            obj.subscribe = subscribe;
            obj.publish = publish;
        }
    };

}());

var stringToArray=function(commaSeparatedString){
        var resultfromExplode=commaSeparatedString.split(',');

        if(resultfromExplode.length==1 && resultfromExplode[0]=="" ){
          return [];
        }else{
          return  resultfromExplode;
        }
};

