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

        randomUUID: function() {
            var S4 = function() {
                return (((1+Math.random())*0x10000)|0).toString(16).substring(1);
            };
            return (S4()+S4()+"-"+S4()+"-"+S4()+"-"+S4()+"-"+S4()+S4()+S4());
        },

        unixTimeToJavascriptTime: function (unixTime) {
            return unixTime * 1000;
        }
    };
})();
