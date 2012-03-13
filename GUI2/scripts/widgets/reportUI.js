(function ($) {
    $.widget('ui.reportUI', {
        options: {
            contextWidgetElement:'hclist',
            baseUrl:''
        },
        _context: {
            includes: [],
            excludes: []
        },       

        _create: function () {
            var $element = this.element;
            this.menuPane = $('<div>').attr('id','treeReportContentMenu');
            this.filterPane = $('<div>').attr('id','treeReportContentMenuFilter');
            $element.append(this.menuPane);
            $element.append(this.filterPane);
            this.refreshReportMenu();
            this.bindEventForMenuPane();
          
        },
        
      
        menuItemClicked:function(e) {
            var $self = this;
            e.preventDefault();
            var itemClicked = $(e.currentTarget);  
            // update the filters 
            var reportId = itemClicked.attr('id');
            var filterUrl = $self.options.baseUrl + '/search/filterSearchView';   
            $self.filterPane.load(filterUrl, {
                'host':'All',
                'report':reportId,
                'inclist': encodeURIComponent($self._context.includes),
                'exlist': encodeURIComponent($self._context.excludes)   
            },function(data) {
                $self.filterPane.find('form').attr('target','_blank');
            });
        },     
        
        _modifyContext:function () {
            var $self = this;
            var includes = encodeURIComponent($self._context.includes);
            var excludes = encodeURIComponent($self._context.excludes);
          
            var contextWidget = $('#'+$self.options.contextWidgetElement).data('contextfinder');
            if (contextWidget) {
                // if context finder is initialized
                // just manipulate the hidden fields for context finder for now
                // have to access some pulic methods to set the context later
                $self.filterPane.find('input[name=inclist]').val(includes);
                $self.filterPane.find('input[name=exlist]').val(excludes);
            }
        },
        
        setContext: function(includes, excludes) {
            var $self = this;
            $self._context.includes = includes;
            $self._context.excludes = excludes;
            $self._modifyContext();
        },
        
        setHostContext: function(hostKey) {
            var $self = this;
            var includes = [common.canonify('PK_' + hostKey)];
            var excludes = [];
            $self.setContext(includes,excludes);
        },
        
        bindEventForMenuPane: function() {
            this.menuPane.delegate('a','click',$.proxy(this.menuItemClicked,this));            
        },
        
        
        refreshReportMenu: function() {
            var self = this;
            var menuUrl = self.options.baseUrl+'/search/generateReportMenu';
            self.menuPane.load(menuUrl);
        },      

        destroy: function() {
            $.Widget.prototype.destroy.call(this);
        }

    });

    $.extend($.ui.reportUI, {
        instances: []
    });
})(jQuery);