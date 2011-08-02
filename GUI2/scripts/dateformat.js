// Simulates PHP's date function
//http://jacwright.com/projects/javascript/date_format/
Date.prototype.format=function(a){var b="";var c=Date.replaceChars;for(var d=0;d<a.length;d++){var e=a.charAt(d);if(d-1>=0&&a.charAt(d-1)=="\\"){b+=e}else if(c[e]){b+=c[e].call(this)}else if(e!="\\"){b+=e}}return b};Date.replaceChars={shortMonths:["Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"],longMonths:["January","February","March","April","May","June","July","August","September","October","November","December"],shortDays:["Sun","Mon","Tue","Wed","Thu","Fri","Sat"],longDays:["Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"],d:function(){return(this.getDate()<10?"0":"")+this.getDate()},D:function(){return Date.replaceChars.shortDays[this.getDay()]},j:function(){return this.getDate()},l:function(){return Date.replaceChars.longDays[this.getDay()]},N:function(){return this.getDay()+1},S:function(){return this.getDate()%10==1&&this.getDate()!=11?"st":this.getDate()%10==2&&this.getDate()!=12?"nd":this.getDate()%10==3&&this.getDate()!=13?"rd":"th"},w:function(){return this.getDay()},z:function(){var a=new Date(this.getFullYear(),0,1);return Math.ceil((this-a)/864e5)},W:function(){var a=new Date(this.getFullYear(),0,1);return Math.ceil(((this-a)/864e5+a.getDay()+1)/7)},F:function(){return Date.replaceChars.longMonths[this.getMonth()]},m:function(){return(this.getMonth()<9?"0":"")+(this.getMonth()+1)},M:function(){return Date.replaceChars.shortMonths[this.getMonth()]},n:function(){return this.getMonth()+1},t:function(){var a=new Date;return(new Date(a.getFullYear(),a.getMonth(),0)).getDate()},L:function(){var a=this.getFullYear();return a%400==0||a%100!=0&&a%4==0},o:function(){var a=new Date(this.valueOf());a.setDate(a.getDate()-(this.getDay()+6)%7+3);return a.getFullYear()},Y:function(){return this.getFullYear()},y:function(){return(""+this.getFullYear()).substr(2)},a:function(){return this.getHours()<12?"am":"pm"},A:function(){return this.getHours()<12?"AM":"PM"},B:function(){return Math.floor(((this.getUTCHours()+1)%24+this.getUTCMinutes()/60+this.getUTCSeconds()/3600)*1e3/24)},g:function(){return this.getHours()%12||12},G:function(){return this.getHours()},h:function(){return((this.getHours()%12||12)<10?"0":"")+(this.getHours()%12||12)},H:function(){return(this.getHours()<10?"0":"")+this.getHours()},i:function(){return(this.getMinutes()<10?"0":"")+this.getMinutes()},s:function(){return(this.getSeconds()<10?"0":"")+this.getSeconds()},u:function(){var a=this.getMilliseconds();return(a<10?"00":a<100?"0":"")+a},e:function(){return"Not Yet Supported"},I:function(){return"Not Yet Supported"},O:function(){return(-this.getTimezoneOffset()<0?"-":"+")+(Math.abs(this.getTimezoneOffset()/60)<10?"0":"")+Math.abs(this.getTimezoneOffset()/60)+"00"},P:function(){return(-this.getTimezoneOffset()<0?"-":"+")+(Math.abs(this.getTimezoneOffset()/60)<10?"0":"")+Math.abs(this.getTimezoneOffset()/60)+":00"},T:function(){var a=this.getMonth();this.setMonth(0);var b=this.toTimeString().replace(/^.+ \(?([^\)]+)\)?$/,"$1");this.setMonth(a);return b},Z:function(){return-this.getTimezoneOffset()*60},c:function(){return this.format("Y-m-d\\TH:i:sP")},r:function(){return this.toString()},U:function(){return this.getTime()/1e3}}