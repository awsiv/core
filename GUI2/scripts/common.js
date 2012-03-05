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

        unixTimeToJavascriptTime: function (unixTime) {
            return unixTime * 1000;
        }
    };
})();
