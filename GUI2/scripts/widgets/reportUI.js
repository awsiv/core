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
        $busyIcon: $('<div style="margin:0px auto;width:50%" class="loadinggif">')
                   .html('&nbsp;'),
        $errorDiv: $('<div>').addClass('error'),
       

        _create: function () {
            var $element = this.element;
            this.menuPane = $('<div>').attr('id','treeReportContentMenu');
            this.reportTitlePane = $('<div style="margin:5px;">').attr('id','treeReportContentMenuTitle');
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
            //remove the previous selected items
            $self.menuPane.find('.selected').removeClass("selected");
            //hide submenu after clicked
            $itemClicked.parent().parent().hide();
            
            $itemClicked.addClass('selected');
            $itemClicked.parents().eq(2).find('span').addClass('selected');
            var reportId = $itemClicked.attr('id');
            var reportText = $itemClicked.text();
            var filterUrl = $self.options.baseUrl + '/search/filterSearchView';
              $.blockUI({ css:{
                        border: 'none',
                        padding: '15px',
                        backgroundColor: '#000',
                        '-webkit-border-radius': '10px',
                        '-moz-border-radius': '10px',
                        opacity: .5,
                        color: '#fff'
                    },
                    message: '<h1 class="ajaxloader2">Please wait...</h1>'
                });
            $self.reportTitlePane.html(reportText);
            $self.filterPane.load(filterUrl, {
                'host':'All',
                'report':reportId,
                'inclist': encodeURIComponent($self._context.includes),
                'exlist': encodeURIComponent($self._context.excludes)
            },function(response, status, xhr) {

                $('.blockUI').hide();   
                $(document).unblock();
                $.unblockUI();

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
        }

    });

    $.extend($.ui.reportUI, {
        instances: []
    });
})(jQuery);