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
        $busyIcon: $('<span class="loadinggif" style="display:none">')
                   .html('&nbsp;'),
        $errorDiv: $('<div>').addClass('error'),
       

        _create: function () {
            var $element = this.element;
            this.menuPane = $('<div>').attr('id','treeReportContentMenu');
            this.reportTitlePane = $('<div style="margin:5px;"><span class="name"><span>').attr('id','treeReportContentMenuTitle').append(this.$busyIcon);
            this.filterPane = $('<div>').attr('id','treeReportContentMenuFilter');
            $element.append(this.menuPane);
            $element.append(this.reportTitlePane);
            $element.append(this.filterPane);
            this.refreshReportMenu();
            this.bindEventForMenuPane();
          
        },
        
      
        menuItemClicked:function(e) {
            var $self = this;
            e.preventDefault();        
            var $itemClicked = $(e.currentTarget);
            if ($itemClicked.hasClass('savesearch-url')) {                
                // check if savedsearch is clicked
                // load the url in different tab
                var url = $itemClicked.attr('href');
                $self.openNewWindow(url, '_blank');
            }
            /* 
              this is workaround for weakest hosts report. we don't have any 
              filters except inc/exc, that's why we should show filter form - redirect to the report 
              after settting context
            */
            if ($itemClicked.attr('id') == 'weakest-hosts') {
                var url = $itemClicked.attr('href');
                if ($self._context.includes.length > 0) {
                    url  += '/inclist/' + $self._context.includes;
                }
                $self.openNewWindow(url, '_blank');
            }
            
            
            //remove the previous selected items
            $self.menuPane.find('.selected').removeClass("selected");
            //hide submenu after clicked
            $itemClicked.parent().parent().hide();
            
            $itemClicked.addClass('selected');
            $itemClicked.parents().eq(2).find('span').addClass('selected');
            var reportId = $itemClicked.attr('id');
            var reportText = $itemClicked.text();
            var filterUrl = $self.options.baseUrl + '/search/filterSearchView';
            
            $self.reportTitlePane.find('span.name').html(reportText);
            $self.$busyIcon.show();
            $self.filterPane.html('');
            $self.filterPane.load(filterUrl, {
                'host':'All',
                'report':reportId,
                'inclist': encodeURIComponent($self._context.includes),
                'exlist': encodeURIComponent($self._context.excludes)
            },function(response, status, xhr) {
                 $self.$busyIcon.hide();
                if (status == 'error') {
                    $self.$errorDiv.html(response);
                     $self.filterPane.html($self.$errorDiv);
                    return;
                }
                $self.filterPane.find('form').attr('target', '_blank');
                $self.filterPane.find('#hclist').hide();
                
                
            });
        },
        _modifyContext:function () {
            var $self = this;
            var includes = encodeURIComponent($self._context.includes);
            var excludes = encodeURIComponent($self._context.excludes);
            $self.filterPane.find('input[name=inclist]').val(includes);
            $self.filterPane.find('input[name=exlist]').val(excludes);
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
            var self =this;
            this.menuPane.delegate('a','click',$.proxy(this.menuItemClicked,this));  
            //for showing up the hidden sub menus
            self.menuPane.hover(
             function(){
                self.menuPane.find('ul').removeAttr("style")
             },
             function(){
                 self.menuPane.find('ul').removeAttr("style")
             });
        },
        
        
        refreshReportMenu: function() {
            var self = this;
            var menuUrl = self.options.baseUrl+'/search/generateReportMenu';
            self.menuPane.load(menuUrl);
        },

        destroy: function() {
            $.Widget.prototype.destroy.call(this);
        },
        
        openNewWindow: function(url, target) {
            window.open(url, target);
            return;
        }

    });

    $.extend($.ui.reportUI, {
        instances: []
    });
})(jQuery);