/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

var sniffer =//static pane
    {
        MAIN_REGION: "sniffer_region",
        SNIFFED_DATA_DETAIL: "sniffer_data_detail",
        SNIFFED_DATA: "sniffer_data_region",
        BUTTONS: "buttons_region",
        DATA_OF_TAB: "tab_data",
        CLASS_CHECK_BOX: "handler_check_box",
        CLASS_TAB_OF_FROM: "tab_of_from",
        CLASS_RAW_COLOR: "handler-class-",
        FContainer: null,
        open: function (aPanel)
        {
            let self = sniffer;

            self.get_or_create_pane(aPanel);
            self.refresh_sniffer();
        },
        create_control_buttons: function (aTo)
        {
            let self = sniffer;
            let _button_add = $("<button>").button();
            _button_add.attr("id", "i_what_to_receive");
            _button_add.on("click", self.MIWantToReceive.open);
            _button_add.text("Add");
            _button_add.appendTo(aTo);

            let _button_refresh = $("<button>").button();
            _button_refresh.attr("id", "snif_refresh");
            _button_refresh.on("click", self.refresh_sniffer);
            _button_refresh.text("Refresh");
            _button_refresh.appendTo(aTo);

        },
        get_or_create_pane: function (aPanel)
        {
            let self = sniffer;

            if (self.FContainer)
                return self.FContainer;

            self.FContainer = $("<div>").attr("id", "sniffer_pane").appendTo(aPanel);

            let _buttons = $("<div>").attr({
                id: sniffer.BUTTONS
            }).appendTo(self.FContainer);
            self.create_control_buttons(_buttons);//todo object

            let _main = $("<div>").attr("id", self.MAIN_REGION).appendTo(self.FContainer);
            {
                let _data = $("<div>").attr({
                    id: sniffer.SNIFFED_DATA
                }).appendTo(_main);
                $("<p>").text("Sniffing").appendTo(_data);
                $("<ul>").appendTo(_data);
                try
                {
                    _data.tabs();
                } catch (e)
                {
                }
            }
            {
                let _detail = $("<div>").attr("id", self.SNIFFED_DATA_DETAIL).appendTo(_main);
                _detail.addClass("ui-corner-all ui-widget ui-widget-content");
                $("<p>").attr("id", "title").text("Detail").appendTo(_detail);
                $("<table>").addClass("data_detail_table").appendTo(_detail);
                //_detail.resizable()
            }
            storage.sniffed_data.on_new = self.update_data;
            storage.sniffed_data.updated = self.update_data;

            storage.dems.created = self.get_or_create_demand;
            storage.dems.removed = self.remove_demand;

            return self.FContainer;
        },
        refresh_sniffer: function ()
        {
            storage.sniffed_data.update();
        },
        create_data_table: function ()
        {
            let _data = $("<table>").addClass("data_table");
            let _tr = $("<tr>").appendTo(_data);
            $("<th>").text("From").appendTo(_tr);
            $("<th>").text("To").appendTo(_tr);
            $("<th>").text("Packet Number").appendTo(_tr);
            $("<th>").text("Type").appendTo(_tr);
            $("<th>").text("Version").appendTo(_tr);
            $("<tbody>").attr("id", sniffer.DATA_OF_TAB).appendTo(_data);
            return _data;
        },
        get_or_create_data_tab: function (_from)
        {
            let self = sniffer;
            let _snif_tabs = $("#" + sniffer.SNIFFED_DATA, self.FContainer);
            let _valid_id = 'tab-' + _from;
            let _tabs = $('#' + get_id_for(_valid_id), _snif_tabs);
            let _tab;
            if (_tabs.length === 0)
            {
                _tab = $("<div>").attr("id", _valid_id).addClass(sniffer.CLASS_TAB_OF_FROM).appendTo(_snif_tabs);
                let _request = $("<div>").attr("id", "request").appendTo(_tab);
                self.create_data_table().appendTo(_tab);

                let li = $("<li><a href='#" + _valid_id + "'>" + _from + "</a> </li>");
                _snif_tabs.find(".ui-tabs-nav").append(li);
                _snif_tabs.tabs("refresh");
            } else
            {
                console.assert(_tabs.length === 1, "invalid size array");
                _tab = _tabs[0];
            }
            return _tab;
        },
        get_handler_id: function (_handle_number)
        {
            return "handle-" + _handle_number;
        },
        get_id_by: function (_handle_number)
        {
            return _handle_number.match(/handle-(\d+)/)[1];
        },
        remove_handle: function (aHandle)
        {
            let self = sniffer;
            let _request =
                {
                    hand: aHandle
                };
            $.ajax(
                {
                    url: "sniffer/do_not_receive_msg",
                    type: "POST",
                    async: true,
                    data: JSON.stringify(
                        _request),
                    contentType: "application/json; charset=utf-8"
                }).done(function (data)
            {
                self.refresh_sniffer();
            }).error(function (jqXHR, textStatus)
            {
                alert("Cannot remove sniffer# " + aHandle + " as " + textStatus);
            });
        },
        remove_demand: function (aDem)
        {
            let aId = aDem.hand;
            let self = sniffer;
            const _handle_id = self.get_handler_id(aId);
            let _dem = $('#' + get_id_for(_handle_id), "#" + sniffer.SNIFFED_DATA, self.FContainer);
            let _parent = _dem.parent();
            $("label[for=" + get_id_for(_handle_id) + "]", _parent).remove();
            _dem.remove();

            let _tab = $("." + sniffer.CLASS_TAB_OF_FROM + ":not(:has(." + sniffer.CLASS_CHECK_BOX + "))", self.FContainer);
            if (_tab.length != 0)
            {
                $.each(_tab, function (i, obj)
                {
                    let _id = obj.id;
                    let _valid_id = get_id_for(_id);
                    let _href = $("a[href=#" + _valid_id + "]", "#" + sniffer.SNIFFED_DATA);
                    _href.parent().remove();//remove href
                    obj.remove();//remove tab
                });
            }
        },
        get_or_create_demand: function (aHandleDemand)
        {
            let self = sniffer;
            const _from = aHandleDemand.id;
            const _handle_number = aHandleDemand.hand;
            const _handle_id = self.get_handler_id(_handle_number);

            const _protocol = aHandleDemand.pl;
            let _tab = self.get_or_create_data_tab(_from);

            if ($('#' + get_id_for(_handle_id), _tab).length == 0)//no demand
            {
                try
                {
                    let _table = $("<table>")
                    {
                        let _tr = $("<tr>").appendTo(_table);
                        _tr.append($("<td>").attr("colspan", 2).text(_handle_number));

                        let _close = $("<span>").addClass('ui-icon ui-icon-close').attr("role", "presentation")
                            .css({
                                "position": "absolute",
                                "right": 0,
                                "top": 0,
                                "cursor": "crosshair"
                            }).appendTo(_table);
                        _close.on("click", function ()
                        {
                            self.remove_handle(_handle_number);
                        });

                    }
                    const _protocol = aHandleDemand.pl;
                    _table.append("<tr><td>Protocol:</td><td>" + _protocol + "</td></tr>");
                    let _head = $.toRepresentationHead(_protocol, aHandleDemand[_protocol]);
                    for (i in _head)
                        $.object_to_table(_head[i], _table);
                    self.add_check_box(_table, $("#request", _tab), _handle_id);
                } catch (err)
                {
                    alert("Could not fill header for " + _protocol + " as " + err.name
                        + "(" + err.message + ")");
                }
            }
            return _handle_id;
        },
        update_detail: function (aData)
        {
            let _table = $(".data_detail_table", "#" + sniffer.SNIFFED_DATA_DETAIL, self.FContainer);
            _table.empty();

            let _data = $.to_representation_form({
                usdt: aData.udata.usdt
            }, data_info);
            for (i in _data)
            {
                $.object_to_table(_data[i], _table);
            }
            const _id = aData.hand;
            let _dem = storage.dems.get(_id);
            let _head = undefined;
            if (_dem)
            {
                const _protocol = _dem.pl;
                _head = _dem[_protocol];
                _head.ver = undefined;

                let _parsed_head = $.toRepresentationHead(_protocol, _head);
                for (i in _parsed_head)
                    $.object_to_table(_parsed_head[i], _table);
            }

            if (aData.udata.data)
            {
                const _protocol = aData.by_parser;
                let _title = $("#title", "#" + sniffer.SNIFFED_DATA_DETAIL, self.FContainer);
                _title.text("Detail (parsed by " + _protocol + ")");

                let _parsed_data = $.toRepresentationData(_protocol, aData.udata.data, _head, _id);
                for (i in _parsed_data)//remove title
                    $.object_to_table(_parsed_data[i], _table);
            }
            filling_data_style(_table);
        },
        update_data: function (aData)
        {
            let self = sniffer;
            const _handle_id = self.get_handler_id(aData.hand);
            let _tab = ("." + sniffer.CLASS_TAB_OF_FROM + ":has(#" + get_id_for(_handle_id) + ")", self.FContainer);
            let _table = $("#" + sniffer.DATA_OF_TAB, _tab);
            let _cb = $("label[for='" + get_id_for(_handle_id) + "']", _tab);

            console.assert(_table.length == 1, "No data for " + _handle_id);
            console.assert(_cb.length == 1, "No handler for " + _handle_id);

            let _color_class = _cb.attr("class").match(new RegExp(self.CLASS_RAW_COLOR + "\\d*", "g"));
            console.assert(_color_class.length == 1, "No color class for " + _handle_id);

            let _tr = $("tr[seq=" + aData.seq_num + "]", _table);
            if (_tr.length == 0)
            {
                _tr = $("<tr>").prependTo(_table);
                _tr.addClass(_color_class[0]).attr({
                    for: get_id_for(_handle_id),
                    seq: aData.seq_num
                });
            } else//it's updated
                _tr.empty();

            _tr.unbind( "click" );
            _tr.click(function ()
            {
                self.update_detail(aData);
            });

            const _is_hide = $("#" + get_id_for(_handle_id), _tab).is(":checked");
            if (!_is_hide)
                _tr.hide();

            let _from = $.to_representation_form(aData.udata.usdt.rtg_.id, data_info);
            let _f = $("<td>").addClass("uuid-tooltip").text(_from[data_info.uuid.getName()]).appendTo(_tr);


            let _to = $.to_representation_form(aData.udata.usdt.uuids, data_info);

            $.object_to_table(_to[data_info.uuid.getName()], $("<td>").addClass("uuid-tooltip").appendTo(_tr).append("<table>"));

            $("<td>").text(aData.udata.usdt.pn).appendTo(_tr);
            $("<td>").text(aData.hand).appendTo(_tr);
            let _ver =
                data_info.ver.toRepresentation(aData.udata.usdt.ver);
            $("<td>").text(_ver).appendTo(_tr);
            filling_data_style(_tr);
            // _data.append($("<p>").text(JSON.stringify(aData, null, 2)));
        },
        MIWantToReceive: {
            SELECT_PROMPT: "-- select a protocol --",
            FWDialog: null,
            FWError: null,
            FWMain: null,
            FWProtocol: null,
            FWFrom: null,
            clean_errors: function ()
            {
                let self = sniffer.MIWantToReceive;
                self.FWError.empty();
                $(".ui-state-error", self.FWMain).removeClass("ui-state-error");
            }

            ,
            open: function ()
            {
                let self = sniffer.MIWantToReceive;
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
                    self.FWMain = $("<table>").appendTo($("<div>").appendTo(self.FWDialog));
                    self.FWError = $("<table>").appendTo($("<div>").appendTo(self.FWDialog));
                    self.FWFrom = self.create_from(self.FWMain);

                    let _par = storage.parsers.getForKernel();
                    if (_par.length == 0)
                    {
                        $("<label>").text("No protocol's parsers").addClass("ui-state-error").appendTo($("<td>").appendTo($("<tr>").appendTo(self.FWError)));
                    } else
                    {
                        self.create_protocol_dialog(_par);
                    }
                } else
                    self.clean_errors();
                self.FWDialog.dialog("open");
            }
            ,
            accept_demand: function ()
            {
                let self = sniffer.MIWantToReceive;
                self.clean_errors();

                let _valid = true;
                let _head;
                let _from;
                let _protocol;
                let _parser;

                if (!data_info.program_name.checking(self.FWFrom.val()))
                {
                    self.FWFrom.addClass("ui-state-error");
                    _valid = false;
                } else
                    _from = self.FWFrom.val();
                try
                {
                    _protocol = $("#select_protocol", self.dialog).find("option:selected").val();
                    _head = $.serialize_head(_protocol, self.FWProtocol);
                } catch (e)
                {
                    _valid = false;
                }

                let _input_parser = $("#parser", self.FWProtocol);
                if (_input_parser)
                {
                    _parser = _input_parser.find("option:selected").val();
                }//

                if (_valid)
                {
                    self.setup_demand(_from, _protocol, _head, _parser).done(function ()
                    {
                        self.FWDialog.dialog("close");
                        sniffer.refresh_sniffer();
                    }).error(function ()
                    {
                        let _error = $("<label>").text("Could not setup sniffer!").addClass("ui-state-error");
                        _error.appendTo($("<td>").appendTo($("<tr>").appendTo(self.FWError)));
                    });
                } else
                {
                    let _error = $("<label>").text("Could not setup sniffer!").addClass("ui-state-error");
                    _error.appendTo($("<td>").appendTo($("<tr>").appendTo(self.FWError)));
                }
            }
            ,
            create_from: function (aTo)
            {
                let self = sniffer.MIWantToReceive;
                let _tr = $("<tr>").appendTo(aTo);
                $("<label>").text("From: ").appendTo($("<td>").appendTo(_tr));

                return $("<input>").attr({
                    id: "From"
                }).appendTo($("<td>").appendTo(_tr));
            }
            ,
            create_protocol_dialog: function (aParser)
            {
                let self = sniffer.MIWantToReceive;
                if (self.FWProtocol)
                    return;
                let _protocol_tr = self.FWMain.append("<tr>");
                $("<label>").text("Protocol").appendTo($("<td>").appendTo(_protocol_tr));

                let _protocols = $("<select>").attr("id", "select_protocol");
                _protocols.appendTo($("<td>").appendTo(_protocol_tr));
                {
                    _protocols.append($("<option>").attr("label", self.SELECT_PROMPT).text(self.SELECT_PROMPT));
                    _protocols.prop("selectedIndex", -1);

                    aParser.forEach(function (item)
                    {
                        _protocols.append($("<option>").text(item));
                    });
                }
                self.FWProtocol = $("<tbody>").attr("id", "div_protocols");
                self.FWProtocol.appendTo(self.FWMain);

                _protocols.on("change", function ()
                {//customize opions

                    self.FWProtocol.empty();
                    self.clean_errors();

                    let _selected = $(this).find("option:selected");
                    let _val = _selected.val();
                    if (_val !== self.SELECT_PROMPT)
                    {
                        let _text = _selected.text();
                        let _fail = false;
                        //console.log("Protocol " + _val);
                        try
                        {
                            $.head_entry_field(_text, self.FWProtocol);
                        } catch (err)
                        {
                            $("<label>").text("Could not open input pane for " + _text + " as " + err.name
                                + "(" + err.message + ")").addClass("ui-state-error").appendTo($("<td>").appendTo($("<tr>").appendTo(self.FWError)));
                            _fail = true;
                        }
                        if (_val === "raw" && !_fail)
                        {
                            let _tr = $("<tr>").appendTo(self.FWProtocol);
                            $("<label>").text("Parsing by: ").appendTo($("<td>").appendTo(_tr));

                            let _by = $("<select>").attr({
                                id: "parser"
                            });

                            let _par = storage.parsers.get();
                            _par.forEach(function (item)
                            {
                                _by.append($("<option>").text(item));
                            });
                            _by.val(_text);

                            _by.appendTo($("<td>").appendTo(_tr));
                        }
                    }
                });
            }
            ,
            setup_demand: function (_from, _protocol, _head, _parser)
            {
                let self = sniffer.MIWantToReceive;
                let _request =
                    {
                        dem: {}
                    };
                _request.dem.id = _from;
                _request.dem.pl = _protocol;
                _request.dem[_protocol] = _head;
                if (_parser)
                    _request["by_parser"] = _parser;
                return $.ajax(
                    {
                        url: "sniffer/i_what_receive",
                        type: "POST",
                        async: true,
                        data: JSON.stringify(
                            _request),
                        contentType: "application/json; charset=utf-8"
                    });
            }
        },
        add_check_box: function (aTable, aTo, aName)
        {
            let self = sniffer;
            let _color = 0;
            for (; $("." + self.CLASS_RAW_COLOR + _color, aTo).length != 0; ++_color)
            {
            }
            console.assert($('#' + get_id_for(aName), aTo).length == 0, "The checkbox " + aName + " is exist");
            let _label = $("<label>").attr({
                for: aName
            }).addClass(self.CLASS_RAW_COLOR + _color).append(aTable);

            let _input = $("<input>").attr({
                checked: true,
                name: aName,
                id: aName,
                type: "checkbox"
            }).addClass("handler_check_box").change(function ()
            {
                let _tab = ("." + sniffer.CLASS_TAB_OF_FROM + ":has(#" + get_id_for(aName) + ")", self.FContainer);
                let _data_region = $("#" + sniffer.DATA_OF_TAB, _tab);
                let _data = $("tr[for='" + get_id_for(aName) + "']", _data_region);

                if ($(this).is(":checked"))
                {
                    _data.show();
                } else
                {
                    _data.hide();
                }
            });

            aTo.append(_label);
            aTo.append(_input);
            _input.checkboxradio({
                icon: false
            });
        }
    }

