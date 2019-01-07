var udt_net =//static pane
    {
        FContainer: null,
        FButtons: null,
        FCy: null,
        FWDialog: null,
        open: function (aPanel)
        {
            let self = udt_net;
            self.get_or_create_pane(aPanel);
            self.refresh();
        },
        create_control_buttons: function (aTo)
        {
            let self = udt_net;
            let _button_refresh = $("<button>").addClass("class-udt-net-button").button();
            _button_refresh.attr("id", "udt_net_refresh");
            _button_refresh.on("click", self.refresh);
            _button_refresh.text("Refresh");
            _button_refresh.appendTo(aTo);
        },
        get_or_create_dialog: function ()
        {
            if (!self.FWDialog)
            {
                self.FWDialog = $("<div>").dialog(
                    {
                        title: "I want to sniff!",
                        autoOpen: false,
                        height: 400,
                        width: 350,
                        modal: true,
                        resizable: true,
                        buttons: {
                            "Accept": self.accept_demand,
                            Cancel: function ()
                            {
                                self.FWDialog.dialog("close");
                            }
                        },
                        close: function ()
                        {
                        }
                    }
                );
            }
            return self.FWDialog;
        },
        reset: function ()
        {
            let self = udt_net;
            self.FCy.remove("[packet_loop]");

            self.FCy.style().resetToDefault().fromJson([
                {
                    selector: 'node[type="1"]',
                    style: {
                        'label': 'data(name)',
                        'text-valign': 'center',
                        'width': 'label',
                        'height': 'auto',
                        'shape': 'ellipse',
                        'background-color': "lightseagreen"
                    }
                },
                {
                    selector: 'node[type="2"][name!="GUI"]',
                    style: {
                        'label': 'data(name)',
                        'text-valign': 'center',
                        'width': 'label',
                        'height': 'auto',
                        'shape': 'octagon',
                        'background-color': "cornflowerblue"
                    }
                },
                {
                    selector: 'node[type="2"][name="GUI"][!fromme]',
                    style: {
                        'label': 'data(name)',
                        'text-valign': 'center',
                        'width': 'label',
                        'height': 'auto',
                        'shape': 'star',
                        'background-color': "cornflowerblue"
                    }
                },
                {
                    selector: 'node[type="2"][name="GUI"][fromme]',
                    style: {
                        'label': 'data(name)',
                        'text-valign': 'center',
                        'width': 'label',
                        'height': 'auto',
                        'shape': 'star',
                        'background-color': "gold"
                    }
                },

                {
                    selector: 'edge',
                    style: {
                        events: 'no'
                    }
                }
            ]).update();
            self.FCy.$("*").show();
        },
        fill_routes_dialog: function (msg, aId, aTo)
        {
            let self = udt_net;
            let _root = self.FCy.$("#" + aId);

            function get_route_for(item)
            {
                let _rstar = {
                    has_loop: false,
                    registrators: self.FCy.collection(),
                    real: self.FCy.collection(),
                    found: false,
                    has_registrator_loop:false,
                    has_real_loop:false,
                };
                if (item.to.length != 0)
                {
                    item.to.forEach(function (_to)
                    {
                        let _star = self.FCy.elements().aStar(
                            {
                                root: _root,
                                goal: "#" + _to.uuid,
                                weight: function ()
                                {
                                    return parseInt(this.data("weight"), 10);
                                }
                            });
                        if (_star.found && _star.path.length == 1)
                        {
                            console.assert(aId == _to.uuid, "has to be equal");

                            let _his_kernel = _star.path.neighborhood("node[type='1']");
                            _star = self.FCy.elements().aStar(
                                {
                                    root: _root,
                                    goal: _his_kernel,
                                    weight: function ()
                                    {
                                        return parseInt(this.data("weight"), 10);
                                    }
                                });

                            if (_to.is_real)
                                _rstar.has_real_loop = true;
                            if (_to.is_registrator)
                                _rstar.has_registrator_loop = true;
                        }
                        _rstar.found = _rstar.found || _star.found;
                        if (_to.is_real)
                            _rstar.real = _rstar.real.union(_star.path);
                        if (_to.is_registrator)
                            _rstar.registrators = _rstar.registrators.union(_star.path);
                    })
                }
                return _rstar;
            }

            msg.forEach(function (item)
            {
                let _star = get_route_for(item);

                let _input = $("<input>").attr({
                    checked: false,
                    type: "radio",
                    name: "route-selection"
                }).change(function ()
                {
                    self.reset();
                    if (this.checked && _star.found)
                    {
                        if (_star.has_real_loop || _star.has_registrator_loop)
                        {
                            let _id_loop = aId + "_to_" + aId;
                            let _loop = self.FCy.$("#" + _id_loop);
                            if (_loop.length == 0)
                            {
                                _loop = self.FCy.add({
                                    group: "edges",
                                    data: {
                                        id: _id_loop,
                                        source: aId,
                                        target: aId,
                                        weight: 1,
                                        packet_loop: "1"
                                    }
                                });
                            }
                            if (_star.has_real_loop)
                                _star.real = _star.real.union(_loop);
                            if (_star.has_registrator_loop)
                                _star.registrators = _star.registrators.union(_loop);

                        }
                        //registrator stile
                        self.FCy.style().selector(_star.registrators.filter("edge")).style(
                            {
                                'line-style': 'dashed',
                                'line-color': 'green'
                            }
                        );
                        self.FCy.style().selector(_star.registrators.filter("node")).style(
                            {
                                'background-color': "green"
                            }
                        );
                        //real stile
                        self.FCy.style().selector(_star.real.filter("edge")).style(
                            {
                                'line-color': 'red',
                                'line-style': 'solid'
                            }
                        );
                        self.FCy.style().selector(_star.real.filter("node")).style(
                            {
                                'background-color': "red"
                            }
                        );


                        self.FCy.style().selector(_root).style(
                            {
                                'background-color': "gold"
                            }
                        );
                        self.FCy.style().update();
                    }
                }).uniqueId();
                if (!_star.found)
                {
                    _input.attr("disabled", "disabled");
                }

                let _msg_num = {};
                $.extend(_msg_num, $.to_representation_form({
                    pl: item.pl
                }, data_info));
                $.extend(_msg_num, $.toRepresentationHead(item.pl, item[item.pl]));

                let _table = $("<table>");
                $.object_to_table(_msg_num, _table, true);
                let _label = $("<label>").attr({
                    for: _input.attr("id")
                }).append(_table);

                aTo.append(_label);
                aTo.append(_input);

            });
            $("input", aTo).checkboxradio({
                icon: false
            });
        },
        show_packet_from: function (aEle)
        {
            let self = udt_net;
            let aId = aEle.id();
            let msg = storage.route.get(aId);
            let _dialog = self.get_or_create_packet_dialog(aId, "from");

            let aTo = $("<div>").appendTo(_dialog);
            self.fill_routes_dialog(msg, aId, aTo);
            _dialog.dialog("open");
        },
        get_or_create_packet_dialog: function (aId, aType)
        {
            let self = udt_net;
            $("#packet-" + aType + "-" + aId).remove();

            let _dialog = $("<div>").dialog(
                {
                    title: "Message " + aType + " " + aId,
                    autoOpen: false,
                    height: 400,
                    width: "auto",
                    modal: false,
                    resizable: true,
                    close: function ()
                    {
                        $(this).remove();
                        self.reset();
                        $("input", ".route-selector").attr("checked", false);
                    }
                }).attr("id", "packet-" + aType + "-" + aId).addClass("route-selector");
            return _dialog;
        },
        show_packet_to: function (aEle)
        {
            let self = udt_net;
            let aId = aEle.id();
            let msg = storage.route.getTo(aId);
            let _dialog = self.get_or_create_packet_dialog(aId, "to");

            let aTo = $("<div>").appendTo(_dialog);
            self.fill_routes_dialog(msg, aId, aTo);
            _dialog.dialog("open");
        },
        disconnect: function (aId)
        {
            alert("The cmd isn't implemented");
        },
        append_menu_buttons: function (aTo)
        {
            let self = udt_net;
            aTo.cxtmenu({
                selector: 'core',

                commands: [
                    {
                        content: 'Refresh',
                        select: function ()
                        {
                            self.refresh();
                        }
                    }
                ]
            });
            aTo.cxtmenu({
                selector: 'node[type="2"][fromme][name!="GUI"]',

                commands: [
                    {
                        content: 'Msg from me',
                        select: function (ele)
                        {
                            self.show_packet_from(ele);
                        }
                    },

                    {
                        content: 'Msg to me',
                        select: function (ele)
                        {
                            self.show_packet_to(ele);
                        }
                    },
                    {
                        content: 'Reconnect',
                        select: function (ele)
                        {
                            self.disconnect(ele);
                        }

                    },
                    {
                        content: 'Disconnect',
                        select: function (ele)
                        {
                            self.disconnect(ele);
                        }

                    }
                ]
            })
            aTo.cxtmenu({
                selector: 'node[type="2"][!fromme],[name="GUI"]',

                commands: [
                    {
                        content: 'Msg from me',
                        select: function (ele)
                        {
                            self.show_packet_from(ele);
                        }
                    },

                    {
                        content: 'Msg to me',
                        select: function (ele)
                        {
                            self.show_packet_to(ele);
                        }
                    }
                ]
            });
            aTo.cxtmenu({
                selector: 'node[type="1"][fromme]',

                commands: [
                    {
                        content: 'Reconnect',
                        select: function (ele)
                        {
                            self.disconnect(ele);
                        }

                    },
                    {
                        content: 'Disconnect',
                        select: function (ele)
                        {
                            self.disconnect(ele);
                        }

                    }
                ]
            });
        },

        get_or_create_pane: function (aPanel)
        {
            let self = udt_net;

            if (self.FContainer)
                return self.FContainer;

            self.FContainer = $("<div>").attr("id", "udt_net_pane").appendTo(aPanel);

            self.FButtons = $("<div>").attr({
                id: "udt_net_button_region"
            }).appendTo(self.FContainer);
            self.create_control_buttons(self.FButtons);
            let _cy = $("<div>").attr("id", "udt_net_region").appendTo(self.FContainer);
            self.FCy = cytoscape({
                container: _cy,
                layout: {
                    name: 'breadthfirst',
                    roots: storage.kernel_uuid
                }
            });
            self.reset();
            self.append_menu_buttons(self.FCy);
            storage.roads.updated = self.update;
            return self.FContainer;
        },
        refresh: function ()
        {
            storage.roads.update();
            storage.route.update();
        },
        add_tooltip: function (aCy)
        {
            aCy.elements("node").qtip({
                content: function ()
                {
                    let _uuid = this.id();
                    let _info = storage.susbscriber_net.get(_uuid);
                    if (_info)
                    {
                        let _to = $("<table>");
                        let _rep = $.to_representation_form({info: _info}, data_info);
                        if (_rep.hasOwnProperty("is_removed"))
                            _to.addClass("ui-state-error");
                        $.object_to_table(_rep, _to, true);
                        return _to;
                    }
                    else
                    {
                        return $("<span>").addClass("ui-state-error").text("Additional info is not exist");
                    }
                },
                show: {
                    event: 'mouseover',
                    delay: 2000
                }, hide: {
                    event: 'mouseout, mousedown, cxttapstart, taphold'
                },
                style: {
                    widget: true, // Use the jQuery UI widget classes
                    def: false // Remove the default styling (usually a good idea, see below)
                }
            });
        },
        update: function (aEle)
        {
            let _elemts={}
            $.extend(_elemts,aEle);

            let self = udt_net;
            let _my = {};
            const _I = storage.kernel_uuid;
            _elemts.edges.forEach(function (item)
            {
                if (item.data.source == _I)
                    _my[item.data.target] = true;
            })
            _elemts.nodes.forEach(function (item)
            {
                item.info = storage.susbscriber_net.get(item.data.id, true);
                if (item.info)
                {
                    item.data.type = item.info.type;
                    item.data.name = item.info.id.n;
                }
                if (_my.hasOwnProperty(item.data.id))
                    item.data.fromme = true;
            })

            //self.FCy.add(aElements);
            //self.FCy.reset();
            self.FCy.load(_elemts);
            // just use the regular qtip api but on cy elements
            self.add_tooltip(self.FCy);
        }
    }