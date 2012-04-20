(function($) {
    $.widget('ui.combobox', {
        options: {
            imagesUrl: '../themes/default/images/astrolabe/',
            defaultbehaviour: true,
            noneSelectedLabel: '(none)',
            addItemPlaceholder: 'Add tree',
            itemExistsError: 'Item already exists in list',
            maxLength: 256
        },

        _create: function() {
            var $self = this;

            var $dl = $('<dl>');
            $dl.addClass('combobox');

            $self._dt = $('<dt>');

            $self._selectedId = null;

            $self._selectedLabel = $('<span>');
            $self._selectedLabel.addClass('selectedLabel');
            $self._selectedLabel.html($self.options.noneSelectedLabel);
            $self._dt.append($self._selectedLabel);

            var $arrowIconBox = $('<div>');
            $arrowIconBox.addClass('arrowIconBox');

            var $arrowIcon = $('<img>');
            $arrowIcon.addClass('arrowIcon');
            $arrowIcon.attr('src', $self.options.imagesUrl + 'combobox-arrow-down.png');
            $arrowIconBox.append($arrowIcon);

            $self._dt.append($arrowIconBox);
            $self._dt.click(function(event) {
                $self._onClickHeader($self, event);
            });
            $dl.append($self._dt);

            $self._dd = $('<dd>');

            $self._ul = $('<ul>');

            var $noneItem = $('<li>');
            $noneItem.click(function(event) {
                $self._setSelectedItem($self, null);
                event.stopPropagation();
            });
            var $noneItemLabelSpan = $('<span>');
            $noneItemLabelSpan.addClass('itemLabel');
            $noneItemLabelSpan.html($self.options.noneSelectedLabel);
            $noneItem.append($noneItemLabelSpan);
            $self._ul.prepend($noneItem);

            var $addItem = $('<li>');
            $addItem.addClass('addItem');

            $self._addInput = $('<input>');
            $self._addInput.addClass('addInput');
            $self._addInput.attr('placeholder', $self.options.addItemPlaceholder);
            $self._addInput.attr('maxlength', $self.options.maxLength);

            var onAddItem = function(id, event) {
                if (id !== undefined &&
                    id !== null &&
                    id.length > 0) {
                    if ($self.addItem(id, id) === true) {
                        $self._trigger('itemAdded', null, {
                            id: id
                        });

                        $self._setSelectedItem($self, id, id);
                        $self._trigger('itemSelected', null, {
                            id: id
                        });
                    }
                }
                else {
                    $self._addInput.focus();
                }
                event.stopPropagation();
            }

            $self._addInput.keypress(function(event) {
                if (event.which == 13) { // enter key
                    onAddItem($self._addInput.val(), event);
                }
            });
            $addItem.append($self._addInput);

            $addButton = $('<span>');
            $addButton.addClass('addButton');
            $addButton.click(function(event) {
                onAddItem($self._addInput.val(), event);
            });
            $addItem.append($addButton);

            $self._ul.append($addItem);

            $self._dd.append($self._ul);

            $dl.append($self._dd);


            $self._setExpanded(false);

            $self.element.append($dl);
        },

        _init: function() {
            var $self = this;

            $self._id = common.randomUUID();
            $self._selectedId = null;
        },

        clear: function() {
            var $self = this;
            $self._ul.remove('li');
        },

        _setSelectedItem: function($self, id) {
            $self._selectedId = id;

            if ($self._selectedId === null) {
                $self._selectedLabel.html($self.options.noneSelectedLabel);
            }
            else {
                $self._selectedLabel.html(decodeURIComponent(id));
            }

            $self._setExpanded(false);
            $self._trigger('itemSelected', null, {id: id});
        },

        _itemExists: function(id) {
            var $self = this;

            var found = false;
            $self._ul.find('li').each(function(index, item) {
                if ($(item).attr('itemId') == id) {
                    found = true;
                }
            });
            return found;
        },

        addItem: function(id) {
            var $self = this;

            if ($self._itemExists(id)) {
                alert($self.options.itemExistsError);
                return false;
            }

            var $li = $('<li>');
            $li.attr('itemId', id);
            $li.click(function(event) {
                $self._setSelectedItem($self, id);
                event.stopPropagation();
            });

            var $deleteIcon = $('<img>');
            $deleteIcon.attr('src', $self.options.imagesUrl + 'combobox-button-delete.png');
            $deleteIcon.addClass('deleteIcon');
            $deleteIcon.click(function(event) {
                $li.remove();
                $self._setSelectedItem($self, null);
                $self._trigger('itemDeleted', null, {
                    id: id
                });
                event.stopPropagation();
            });
            $li.append($deleteIcon);

            var $labelSpan = $('<span>');
            $labelSpan.addClass('itemLabel');
            $labelSpan.html(decodeURIComponent(id));
            $li.append($labelSpan);

            $self._ul.prepend($li);
            return true;
        },

        _onClickHeader: function($self, event) {
            var isExpanded = $self._dd.is(':visible');
            $self._setExpanded(!isExpanded);
        },

        _setExpanded: function(expanded) {
            var $self = this;

            if (expanded === true) {
                $('html').bind('click.' + $self._id, function(event) {
                    if ($(event.target).parents().is($self.element) === false) {
                        $self._setExpanded(false);
                    }
                });

                $self._addInput.val('');
            }
            else {
                $('html').unbind('click.' + $self._id);
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
