(function ($) {
    $.widget('ui.combobox', {
        options: {
            imagesUrl: '../../themes/default/images/astrolabe/',
            defaultbehaviour: true,
            defaultNoneSelectedLabel: '(none)',
            defaultAddItemPlaceholder: 'Add tree',
            maxLength: 15
        },

        _create: function () {
            var $self = this;

            var $dl = $('<dl>');
            $dl.addClass('combobox');
            {
                $self._dt = $('<dt>');
                {
                    $self._selectedId = null;
                    
                    $self._selectedLabel = $('<span>');
                    $self._selectedLabel.addClass('selectedLabel');
                    $self._selectedLabel.html($self.options.defaultNoneSelectedLabel);
                    $self._dt.append($self._selectedLabel);
                    
                    var $arrowIconBox = $('<div>');
                    $arrowIconBox.addClass('arrowIconBox');
                    {
                        var $arrowIcon = $('<img>');
                        $arrowIcon.addClass('arrowIcon');
                        $arrowIcon.attr('src', $self.options.imagesUrl + 'combobox-arrow-down.png');
                        $arrowIconBox.append($arrowIcon);
                    }
                    $self._dt.append($arrowIconBox);
                    
                }
                $self._dt.click(function(event) {
                   $self._onClickHeader($self, event); 
                });
                $dl.append($self._dt);
                
                $self._dd = $('<dd>');
                {
                    $self._ul = $('<ul>');
                    {
                        var $addItem = $('<li>');
                        $addItem.addClass('addItem');
                        {
                            $self._addInput = $('<input>');
                            $self._addInput.addClass('addInput');
                            $self._addInput.attr('placeholder', $self.options.defaultAddItemPlaceholder);
                            $self._addInput.attr('maxlength', $self.options.maxLength);
                            $addItem.append($self._addInput);
                            
                            $addButton = $('<img>');
                            $addButton.addClass('addButton');
                            $addButton.attr('src', $self.options.imagesUrl + 'combobox-button-add.png');
                            $addButton.click(function(event) {
                                var $id = $self._addInput.val();
                                
                                if ($id !== undefined && 
                                    $id !== null &&
                                    $id.length > 0) {
                                    $self.addItem($id, $id);
                                    $self._trigger("itemAdded", null, {id: $id});

                                    $self._setSelectedItem($self, $id, $id);
                                    $self._trigger("itemSelected", null, {id: $id});
                                }
                                event.stopPropagation();
                            });
                            $addItem.append($addButton);
                        }
                        $self._ul.append($addItem);
                    }
                    $self._dd.append($self._ul);
                }
                $dl.append($self._dd);
            }
            
            $self._setExpanded(false);
            
            $self.element.append($dl);
        },
        
        clear: function() {
            var $self = this;
            $self._ul.remove('li');
        },
        
        selectItem: function(id) {
            if (id === undefined || id === null) {
                
            }
            else {
                
            }
        },
    
        _setSelectedItem: function($self, id) {
            $self._selectedId = id;
            $self._selectedLabel.html(id);
            
            $self._setExpanded(false);
            $self._trigger("itemSelected", null, {id: id});
        },
        
        addItem: function(id) {
            var $self = this;

            var $li = $('<li>');
            {
                var $deleteIcon = $('<img>');
                $deleteIcon.attr('src', $self.options.imagesUrl + 'combobox-button-delete.png');
                $deleteIcon.addClass('deleteIcon');
                $deleteIcon.click(function(event) {
                    $li.remove();
                    $self._trigger("itemDeleted", null, {id: id});
                    event.stopPropagation();
                });
                $li.append($deleteIcon);

                var $labelSpan = $('<span>');
                $labelSpan.addClass('itemLabel');
                $labelSpan.html(decodeURIComponent(id));
                $labelSpan.click(function(event) {
                   $self._setSelectedItem($self, id);
                   event.stopPropagation();
                });
                $li.append($labelSpan);
            }
            $self._ul.prepend($li);
        },
        
        _onClickHeader: function($self, event) {
            var isExpanded = $self._dd.is(":visible");
            $self._setExpanded(!isExpanded);
        },

        _setExpanded: function(expanded) {
            var $self = this;
            
            if (expanded === true) {
                $self._addInput.val('');
            }

            $self._dd.toggle(expanded);
        },

        destroy: function() {
            $.Widget.prototype.destroy.call(this);
        }

    });
    
    $.extend($.ui.combobox, {
        instances: []
    });
})(jQuery);