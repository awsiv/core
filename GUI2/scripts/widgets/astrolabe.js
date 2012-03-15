// version 0.1
(function($) {
    $.widget('ui.astrolabe', {
        options: {
            baseUrl: '../',
            defaultbehaviour: true
        },

        _create: function() {
            var $self = this;

            $self.element.addClass('astrolabe');

            $self._menuContainer = $self._createMenu();
            $self.element.append($self._menuContainer);
            $self._loadProfileList();

            $self._listContainer = $('<div>');
            $self._listContainer.addClass('listContainer');
            $self._rootContainer = $('<ul>');
            $self._rootContainer.addClass('rootContainer');
            $self._listContainer.append($self._rootContainer);

            $self.element.append($self._listContainer);
            $self._listContainer.jScrollPane({
                showArrows: true,
                autoReinitialise: true,
                verticalArrowPositions: 'after'
            });
        },

        _init: function() {
            var $self = this;

            $self._selected = null;
            $self._uniqueIdCounter = 0;

            $self._loadProfile(null);
        },

        _createMenu: function() {
            var $self = this;

            var $menuContainer = $('<div>');
            $menuContainer.addClass('menu');

            $self._profilesCombo = $('<div>');
            $self._profilesCombo.addClass('profilesCombo');
            $self._profilesCombo.combobox({
                noneSelectedLabel: '(All Hosts)',
                addItemPlaceholder: 'Add Tree',

                itemSelected: function(event, args) {
                    $self._loadProfile(args.id);
                },

                itemAdded: function(event, args) {
                    $self._saveProfile(args.id, []);
                    $self._loadProfile(args.id);
                },

                itemDeleted: function(event, args) {
                    $self._deleteProfile(args.id);
                }
            });
            $menuContainer.append($self._profilesCombo);

            var $classFinderIcon = $('<a>');
            $classFinderIcon.addClass('classFinderIcon');
            $classFinderIcon.addClass('showqtip');
            $classFinderIcon.attr('title', 'Find hosts by the classes they have defined.');
            $classFinderIcon.html('&nbsp');
            $classFinderIcon.classfinder({ baseUrl: $self.options.baseUrl} );
            $menuContainer.append($classFinderIcon);

            var $hostFinderIcon = $('<a>');
            $hostFinderIcon.addClass('hostFinderIcon');
            $hostFinderIcon.addClass('showqtip');
            $hostFinderIcon.attr('title', 'Find hosts by name or address.');
            $hostFinderIcon.html('&nbsp');
            $hostFinderIcon.hostfinder({ baseUrl: $self.options.baseUrl} );
            $menuContainer.append($hostFinderIcon);

            var $promiseFinderIcon = $('<a>');
            $promiseFinderIcon.addClass('promiseFinderIcon');
            $promiseFinderIcon.addClass('showqtip');
            $promiseFinderIcon.attr('title', 'Find promises that make up the policy.');
            $promiseFinderIcon.html('&nbsp');
            $promiseFinderIcon.policyfinder({ baseUrl: $self.options.baseUrl} );
            $menuContainer.append($promiseFinderIcon);

            return $menuContainer;
        },

        setFilter: function(value) {
            var self = this;

            self._setNodeExpanded($(self._superNode), false);

            $(self._rootContainer).find('li.host').each(function() {
                var $host = $(this);
                var hostname = $host.attr('hostname');

                if (hostname.indexOf(value) != -1) {
                    var $parentNode = self._parentNode($host);

                    self._setNodeExpanded($parentNode, true);
                    $host.show();
                }
                else {
                    $host.hide();
                }
            });
        },

        _isNodeExpanded: function($node) {
            var self = this;

            return $node === null || $node.hasClass('expanded');
        },

        _setNodeExpanded: function($node, expanded) {
            var self = this;

            if ($node === null || $node === undefined)
                return;

            if (self._isNodeExpanded($node)) {
                if (expanded === false) {
                    $node.removeClass('expanded');
                    self._nodeContainer($node).hide();
                }
            }
            else {
                if (expanded === true) {
                    $node.addClass('expanded');

                    self._setNodeExpanded(self._parentNode($node), true);

                    self._nodeContainer($node).show();
                }
            }
        },

        _onClickNodeLabel: function($self, $node, event) {
            $self._selectElement($node);

            $self._trigger('nodeSelected', event, {
               path: $self._nodeLabels($node).reverse(),
               includes: $self._nodeIncludes($node),
               excludes: $self._nodeExcludes($node),
               count: $node.attr('count')
            });

            if (event !== null && event !== undefined) {
                event.stopPropagation();
            }
        },

        _onClickNodeIcon: function(self, node, event) {
            self._setNodeExpanded(node, !self._isNodeExpanded(node));

            if (self._isNodeExpanded(node)) {
                self._loadNode(node);
            }

            if (event !== null && event !== undefined) {
                event.stopPropagation();
            }
        },

        _selectElement: function($element) {
            var $self = this;

            if ($self._selected !== null) {
                $self._selected.removeClass('selectedElement');
            }

            $self._selected = $element;
            $self._selected.addClass('selectedElement');
        },

        _onClickHost: function($self, $host, event) {
            $self._selectElement($host);

            $self._trigger('hostSelected', event, {
               hostKey: $host.attr('hostkey'),
               hostName: $host.attr('hostname')
            });

            event.stopPropagation();
        },

        _createNode: function(label, classRegex, children) {
            var self = this;

            var $nodeItem = $('<li>');
            $nodeItem.addClass('node');

            $nodeItem.attr('label', label);
            $nodeItem.attr('class-regex', classRegex);

            var $iconElement = $('<div>');
            $iconElement.addClass('nodeIcon');
            $iconElement.click(function(event) {
                self._onClickNodeIcon(self, $nodeItem, event);
            });
            $nodeItem.append($iconElement);

            var $labelElement = $('<span>');
            $labelElement.addClass('nodeLabel');
            $labelElement.html(label);
            var contextMenuId = self.element.attr('id') + '-context-menu-' + self._uniqueIdCounter++;
            $labelElement.contextMenu(contextMenuId, self._createNodeContextMenu($nodeItem));
            $labelElement.click(function(event) {
                self._onClickNodeLabel(self, $nodeItem, event);
            });
            $nodeItem.append($labelElement);

            var $hostCountLabelElement = $('<span>');
            $hostCountLabelElement.addClass('hostCountLabel');
            $nodeItem.append($hostCountLabelElement);

            var $busyIcon = $('<span>');
            $busyIcon.addClass('busyIcon');
            $busyIcon.html('&nbsp;');
            $busyIcon.hide();
            $nodeItem.append($busyIcon);

            var $childrenList = self._createContainer(children);
            $childrenList.hide();
            $nodeItem.append($childrenList);

            return $nodeItem;
        },

        _createNodeContextMenu: function(nodeItem) {
            var self = this;

            return {
                'Add Node': {
                    click: function() {
                        var $dialog = self._createAddNodeDialog(nodeItem);
                        $dialog.dialog('open');
                    }
                },
                'Remove': {
                    click: function() {
                        var parentNode = self._parentNode($(nodeItem));
                        $(nodeItem).remove();
                        self._saveProfile(self._currentProfile, parentNode);
                    }
                }
            };
        },

        _createAddNodeDialog: function(parentNode) {
            var $self = this;

            var addNode = function($dialog) {
                var label = $('#astrolabe-add-node-label').val();
                if (label == '') {
                    $('#astrolabe-add-node-label').focus();
                    return;
                }

                var classRegex = $('#astrolabe-add-node-class').val();
                if (classRegex == '') {
                    $('#astrolabe-add-node-class').focus();
                    return;
                }

                var $node = $($self._createNode(label, classRegex, null));

                var $parentContainer = $($self._rootContainer);
                if (parentNode !== null) {
                    $parentContainer = $self._nodeContainer($(parentNode));
                }
                $parentContainer.append($node);

                $node.show();
                $self._saveProfile($self._currentProfile, parentNode);

                $self._countNode($node);

                $dialog.dialog('close');
                $dialog.dialog('destroy');
                $dialog.remove();
            }

            var $dialog = $('<div>')
                .load('/widget/astrolabeAddNodeDialog/', function() {
                    $('#astrolabe-add-node-label').focus();
                })
                .dialog({
                    autoOpen: false,
                    title: 'Add Class Filter',
                    width: 415,
                    buttons: {
                        'Add': function() {
                            addNode($(this));
                        },
                        'Cancel': function() {
                            $(this).dialog('close');
                            $(this).dialog('destroy');
                            $(this).remove();
                        }
                    },
                    draggable: false,
                    modal: true,
                    resizable: false
                });

            $dialog.keypress(function(event) {
                if (event.keyCode == $.ui.keyCode.ENTER) {
                    addNode($dialog);
                    event.preventDefault();
                    return false;
                }
            });

            return $dialog;
        },

        _createHost: function(key, name, colour) {
            var self = this;

            var hostItem = document.createElement('li');
            $(hostItem).addClass('host');
            $(hostItem).attr('hostkey', key);
            $(hostItem).attr('hostname', name);

            var $hostIcon = $('<div>');
            $hostIcon.addClass('hostIcon');
            $hostIcon.addClass(colour);
            $hostIcon.click(function(event) {
                self._onClickHost(self, $(hostItem), event);
            });
            $(hostItem).append($hostIcon);

            var $hostLabel = $('<span>');
            $hostLabel.addClass('hostLabel');
            $hostLabel.html(name);
            $hostLabel.click(function(event) {
                self._onClickHost(self, $(hostItem), event);
            });
            $(hostItem).append($hostLabel);

            return hostItem;
        },

        _createContainer: function(nodeDescriptionList) {
            var $self = this;

            var $container = $('<ul>');
            $container.addClass('container');

            if (nodeDescriptionList !== undefined &&
                nodeDescriptionList !== null &&
                nodeDescriptionList.length > 0) {
                $.each(nodeDescriptionList, function() {
                    var $node = $self._createNode(this.label, this.classRegex, this.children);
                    $container.append($node);
                });
            }

            return $container;
        },

        _nodeHostCountLabel: function($node, value) {
            if (value !== undefined) {
                $node.children('.hostCountLabel').html(value);
            }
            return value;
        },

        _nodeContainer: function($node) {
            return $node.children('ul');
        },

        _subNodes: function(node) {
            var self = this;

            return self._nodeContainer(node).children('li.node');
        },

        _subHosts: function(node) {
            var self = this;

            return self._nodeContainer(node).children('li.host');
        },

        _parentNode: function($node) {
            var $parent = $node.parent().parent();
            if ($parent.hasClass('node')) {
                return $parent;
            }
            else {
                return null;
            }
        },

        _nodeLabel: function($node) {
            return $node.attr('label');
        },

        _nodeClass: function(node) {
            return node.attr('class-regex');
        },

        _nodeLabels: function($node) {
            var $self = this;

            var labels = [$self._nodeLabel($node)];

            for ($node = $self._parentNode($node); $node != null; $node = $self._parentNode($node)) {
                labels.push($self._nodeLabel($node));
            }

            return labels;
        },

        _nodeIncludes: function($node) {
            var $self = this;

            var includes = [$self._nodeClass($node)];

            for ($node = $self._parentNode($node); $node != null; $node = $self._parentNode($node)) {
                includes.unshift($self._nodeClass($node));
            }

            return includes.slice(1);
        },

        _nodeExcludes: function($node) {
            var $self = this;

            var excludes = [];

            $.each($self._subNodes($node), function() {
                excludes.unshift($self._nodeClass($(this)));
            });

            return excludes;
        },

        _loadNode: function(node) {
            var self = this;

            self._subHosts(node).remove();

            var includes = self._nodeIncludes(node);
            var excludes = self._nodeExcludes(node);

            self._loadHosts(node, includes, excludes);
        },

        _loadHosts: function($node, includes, excludes) {
            var self = this;

            var container = self._nodeContainer($node);
            $node.children('.busyIcon').css('display', 'inline-block');

            $.getJSON(self._requestUrls.hosts(self, includes, excludes), function(hostDescriptionList) {
                if (hostDescriptionList !== null) {
                    var colourOrder = ['blue', 'red', 'yellow', 'green'];

                    hostDescriptionList.sort(function(a, b) {
                       return colourOrder.indexOf(a.colour) -
                           colourOrder.indexOf(b.colour);
                    });

                    $.each(hostDescriptionList, function() {
                        var hostItem = self._createHost(this.hostkey,
                            this.hostname, this.colour);
                        $(container).append(hostItem);
                    });
                }

                $node.children('.busyIcon').hide();
            });
        },

        _loadProfileList: function() {
            var self = this;

            var requestUrl = self.options.baseUrl + 'astrolabe/profile/';

            $.getJSON(requestUrl, function(profileList) {

                $_profilesCombo = self._profilesCombo;

                self._profilesCombo.combobox('clear');

                $.each(profileList, function(index, value) {
                    self._profilesCombo.combobox('addItem', value, value);
                });
            });
        },

        _serializeContainer: function($container) {
            var self = this;

            return $.map($container.children('.node'), function(node) {
                var $node = $(node);

                return nodeDescription = {
                    label: $node.attr('label'),
                    classRegex: $node.attr('class-regex'),
                    children: self._serializeContainer($node.children('.container'))
                };
            });
        },

        _profileUrl: function(profileId) {
            var self = this;
            return self.options.baseUrl + 'astrolabe/profile/' + profileId;
        },

        _saveProfile: function(profileId, refreshNode) {
            var $self = this;

            var nodeDescriptionList = $self._serializeContainer($self._nodeContainer($self._superNode));

            if (profileId !== null && profileId !== undefined) {
                $.ajax({
                    type: 'PUT',
                    url: $self._profileUrl(profileId),
                    contentType: 'application/json',
                    data: JSON.stringify(nodeDescriptionList),

                    success: function() {
                        if (refreshNode !== null) {
                            $self._loadNode($(refreshNode));
                        }
                    }
                });
            }
            else {
                if (refreshNode !== null) {
                    $self._loadNode($(refreshNode));
                }
            }
        },

        _deleteProfile: function(profileId) {
            var $self = this;

            $.ajax({
               type: 'DELETE',
               url: $self._profileUrl(profileId)
            });

            if ($self._currentProfile === profileId) {
                $self._rootContainer.children().remove();
            }
        },

        _loadProfile: function(profileId) {
            var $self = this;

            if ($self._currentProfile === profileId) {
                return;
            }

            $self._currentProfile = profileId;

            $self._rootContainer.children().remove();

            if ($self._currentProfile !== null) {
                $.ajax({
                   type: 'GET',
                   url: $self._profileUrl(profileId),
                   success: function(nodeDescriptionList) {
                       $self._createSuperNode($.parseJSON(nodeDescriptionList));
                       $self._onClickNodeLabel($self, $self._superNode, null);
                       $self._setNodeExpanded($self._superNode, true);
                       $self._loadNode($self._superNode);
                   },
                   error: function() {
                       $self._createSuperNode(null);
                       $self._onClickNodeLabel($self, $self._superNode, null);
                       $self._setNodeExpanded($self._superNode, true);
                       $self._loadNode($self._superNode);
                   }
                });
            }
            else {
                $self._createSuperNode(null);
                $self._onClickNodeLabel($self, $self._superNode, null);
                $self._setNodeExpanded($self._superNode, true);
                $self._loadNode($self._superNode);
            }
        },

        _recount: function() {
            var $self = this;
            var $node = $self._superNode;

            $self._rootContainer.find('.node').each(function(index, value) {
                var $node = $(value);
                $self._countNode($node);
            });
        },

        _countNode: function($node) {
            var $self = this;

            var includes = $self._nodeIncludes($node);
            $.getJSON($self._requestUrls.hostCount($self, includes, []), function(count) {
                $node.attr('count', count);
                $node.children('.hostCountLabel').html('(' + count + ')');
            });
        },

        _createSuperNode: function(nodeDescriptionList)
        {
            var $self = this;

            $self._rootContainer.children().remove();

            $self._superNode = $self._createNode('All Hosts',
                            '.*', nodeDescriptionList);

            $self._rootContainer.append($self._superNode);

            $self._recount();

            return $self._superNode;
        },

        _requestUrls: {

            hosts: function(self, includes, excludes) {

                var url = self.options.baseUrl + 'astrolabe/host?';

                if (includes.length > 0) {
                    url = url + 'includes=' + encodeURIComponent(includes) + '&';
                }

                if (excludes.length > 0) {
                    url = url + 'excludes=' + encodeURIComponent(excludes);
                }

                return url;
            },

            hostCount: function(self, includes) {

                var url = self.options.baseUrl + 'host/count?';

                if (includes.length > 0) {
                    url = url + 'includes=' + encodeURIComponent(includes);
                }

                return url;
            }
        },

        destroy: function() {
            $.Widget.prototype.destroy.call(this);
        }
    });

    $.extend($.ui.astrolabe, {
        instances: []
    });

})(jQuery);
