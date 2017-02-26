/* 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

function add_check_box(aTable, aTo, aName)
{
    console.assert($('#' + get_id_for(aName), aTo).length == 0, "The checkbox " + aName + " is exist");
    var _label = $("<label>").attr({
        for: aName
    }).append(aTable);
    var _input = $("<input>").attr({
        checked: true,
        name: aName,
        id: aName,
        type: "checkbox"
    }).addClass("handler_check_box");
    aTo.append(_label);
    aTo.append(_input);
    _input.checkboxradio({
        icon: false
    });
}


var sniffer =//static pane
    {
        MAIN_REGION: "sniffer_region",
        BUTTONS: "buttons_region",
        DATA_OF_TAB: "tab_data",
        CLASS_CHECK_BOX: "handler_check_box",
        CLASS_TAB_OF_FROM: "tab_of_from",
        FContainer: null,
        open: function (aPanel)
        {
            var self = sniffer;

            self.get_or_create_pane(aPanel);
            self.refresh_sniffer();
        },
        create_control_buttons: function (aTo)
        {
            var self = sniffer;
            var _button_add = $("<button>").button();
            _button_add.attr("id", "i_what_to_receive");
            _button_add.on("click", self.MIWantToReceive.open);
            _button_add.text("Add");
            _button_add.appendTo(aTo);

            var _button_refresh = $("<button>").button();
            _button_refresh.attr("id", "snif_refresh");
            _button_refresh.on("click", self.refresh_sniffer);
            _button_refresh.text("Refresh");
            _button_refresh.appendTo(aTo);
        },
        get_or_create_pane: function (aPanel)
        {
            var self = sniffer;

            if (self.FContainer)
                return self.FContainer;

            self.FContainer = $("<div>").attr("id", "sniffer_pane").appendTo(aPanel);

            var _buttons = $("<div>").attr({
                id: sniffer.BUTTONS
            }).appendTo(self.FContainer);
            self.create_control_buttons(_buttons);//todo object

            var _data = $("<div>").attr({
                id: sniffer.MAIN_REGION
            }).appendTo(self.FContainer);

            $("<ul>").appendTo(_data);
            try
            {
                _data.tabs();
            } catch (e)
            {
            }
            return self.FContainer;
        },
        refresh_sniffer: function ()
        {
            var self = sniffer;
            $.ajax(
                {
                    url: "sniffer/snif_state",
                    type: "POST",
                    data: JSON.stringify(
                        {
                            "seq_num": self.latest_sniffed_packet
                        }),
                    dataType: "json",
                    contentType: "application/json; charset=utf-8"
                }).done(function (data)
            {
                self.handle_sniffed_data(data);
            }).error()
            {
            }
            ;
        }, handle_sniffed_data: function (aData)
    {
        var self = sniffer;
        var _dems = aData.snif_state.dems;
        self.update_demands(_dems);
        //data
        var _data = aData.snif_state.sniffed_data;

        if ($.isArray(_data))
        {
            for (var i = _data.length; --i >= 0;)
            {
                if (_data.hasOwnProperty(i))
                {
                    self.update_data(_data[i]);
                }
            }

        } else if (_data)
        {
            self.update_data(_data);
        }
    },
        create_data_table: function ()
        {
            var _data = $("<table>");
            var _tr = $("<tr>").appendTo(_data);
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
            var self = sniffer;
            var _snif_tabs = $("#" + sniffer.MAIN_REGION, self.FContainer);
            var _valid_id = 'tab-' + _from;
            var _tabs = $('#' + get_id_for(_valid_id), _snif_tabs);
            var _tab;
            if (_tabs.length === 0)
            {
                _tab = $("<div>").attr("id", _valid_id).addClass(sniffer.CLASS_TAB_OF_FROM).appendTo(_snif_tabs);
                var _request = $("<div>").attr("id", "request").appendTo(_tab);
                self.create_data_table().appendTo(_tab);

                var li = $("<li><a href='#" + _valid_id + "'>" + _from + "</a> </li>");
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
            var self = sniffer;
            var _request =
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
        update_demands: function (_dems)
        {
            var self = sniffer;
            var _exist = {};
            if ($.isArray(_dems.dem))
            {
                for (var _i in _dems.dem)
                {
                    if (_dems.dem.hasOwnProperty(_i))
                    {
                        _exist[self.get_or_create_demand(_dems.dem[_i])] = true;
                    }
                }
            } else if (_dems.dem)
            {
                _exist[self.get_or_create_demand(_dems.dem)] = true;
            }
            var _exist_demands = $("." + sniffer.CLASS_CHECK_BOX, "#" + sniffer.MAIN_REGION, self.FContainer);
            _exist_demands.each(function ()
            {
                const _id = self.get_id_by(this.id);
                if (!_exist.hasOwnProperty(this.id))
                {
                    self.remove_demand(_id);//".ui-tabs-panel>#request>#handle-6"
                }
            });
        },
        remove_demand: function (aId)
        {
            var self = sniffer;
            const _handle_id = self.get_handler_id(aId);
            var _dem = $('#' + get_id_for(_handle_id), "#" + sniffer.MAIN_REGION, self.FContainer);
            var _parent = _dem.parent();
            $("label[for=" + get_id_for(_handle_id) + "]", _parent).remove();
            _dem.remove();

            var _tab = $("." + sniffer.CLASS_TAB_OF_FROM + ":not(:has(." + sniffer.CLASS_CHECK_BOX + "))", self.FContainer);
            if (_tab.length != 0)
            {
                $.each(_tab, function (i, obj)
                {
                    var _id = obj.id;
                    var _valid_id = get_id_for(_id);
                    var _href = $("a[href=#" + _valid_id + "]", "#" + sniffer.MAIN_REGION);
                    _href.parent().remove();//remove href
                    obj.remove();//remove tab
                });
            }
        },
        get_or_create_demand: function (aHandleDemand)
        {
            var self = sniffer;
            const _from = aHandleDemand.id;
            const _handle_number = aHandleDemand.hand;
            const _handle_id = self.get_handler_id(_handle_number);

            const _protocol = aHandleDemand.pl;
            var _tab = self.get_or_create_data_tab(_from);

            if ($('#' + get_id_for(_handle_id), _tab).length == 0)//no demand
            {
                try
                {
                    var _table = $("<table>")
                    {
                        var _tr = $("<tr>").appendTo(_table);
                        _tr.append($("<td>").attr("colspan", 2).text(_handle_number));

                        var _close = $("<span>").addClass('ui-icon ui-icon-close').attr("role", "presentation")
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
                    $.object_to_table($.toRepresentationHead(_protocol, aHandleDemand[_protocol]), _table);

                    add_check_box(_table, $("#request", _tab), _handle_id);
                } catch (err)
                {
                    alert("Could not fill header for " + _protocol + " as " + err.name
                        + "(" + err.message + ")");
                }
            }
            return _handle_id;
        },
        update_data: function (aData)
        {
            var self = sniffer;
            const _handle_id = self.get_handler_id(aData.hand);
            var _data = $("#" + sniffer.DATA_OF_TAB, ":has(#" + get_id_for(_handle_id) + ")", self.FContainer);
            const _seq_num = parseInt(aData.seq_num, 10);
            console.assert(_data.length == 1, "No data for " + _handle_id);
            self.latest_sniffed_packet = Math.max(_seq_num, self.latest_sniffed_packet);


            var _tr = $("<tr>").prependTo(_data);
            var _from = $.to_representation_form(aData.udata.usdt.rtg_.id, data_info);
            $("<td>").text(_from[data_info.uuid.getName()]).appendTo(_tr);

            var _to = $.to_representation_form(aData.udata.usdt.uuids, data_info);

            $.object_to_table(_to[data_info.uuid.getName()], $("<td>").appendTo(_tr).append("<table>"));

            $("<td>").text(aData.udata.usdt.pn).appendTo(_tr);
            $("<td>").text(aData.hand).appendTo(_tr);
            var _ver =
                data_info.ver.toRepresentation(aData.udata.usdt.ver);
            $("<td>").text(_ver[data_info.ver.getName()]).appendTo(_tr);
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
                var self = sniffer.MIWantToReceive;
                self.FWError.empty();
                $(".ui-state-error", self.FWMain).removeClass("ui-state-error");
            }

            ,
            open: function ()
            {
                var self = sniffer.MIWantToReceive;
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

                    $.ajax(
                        {
                            type: "GET",
                            data: {query: "parsers"},
                            dataType: "json",
                            async: true,
                            cache: true
                        }).done(function (data)
                    {
                        self.create_protocol_dialog(data.state.parsers);
                    }).error(function (jqXHR, textStatus)
                    {
                        $("<label>").text("Could not load gui protocols as " + textStatus).addClass("ui-state-error").appendTo($("<td>").appendTo($("<tr>").appendTo(self.FWError)));
                    });
                }
                self.clean_errors();
                self.FWDialog.dialog("open");
            }
            ,
            accept_demand: function ()
            {
                var self = sniffer.MIWantToReceive;
                self.clean_errors();

                var _valid = true;
                var _head;
                var _from;
                var _protocol;
                var _parser;

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

                var _input_parser = $("#parser", self.FWProtocol);
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
                        var _error = $("<label>").text("Could not setup sniffer!").addClass("ui-state-error");
                        _error.appendTo($("<td>").appendTo($("<tr>").appendTo(self.FWError)));
                    });
                } else
                {
                    var _error = $("<label>").text("Could not setup sniffer!").addClass("ui-state-error");
                    _error.appendTo($("<td>").appendTo($("<tr>").appendTo(self.FWError)));
                }
            }
            ,
            create_from: function (aTo)
            {
                var self = sniffer.MIWantToReceive;
                var _tr = $("<tr>").appendTo(aTo);
                $("<label>").text("From: ").appendTo($("<td>").appendTo(_tr));

                return $("<input>").attr({
                    id: "From"
                }).appendTo($("<td>").appendTo(_tr));
            }
            ,
            create_protocol_dialog: function (aParser)
            {
                var self = sniffer.MIWantToReceive;
                if (self.FWProtocol)
                    return;
                var _protocol_tr = self.FWMain.append("<tr>");
                $("<label>").text("Protocol").appendTo($("<td>").appendTo(_protocol_tr));

                var _protocols = $("<select>").attr("id", "select_protocol");
                _protocols.appendTo($("<td>").appendTo(_protocol_tr));
                {
                    _protocols.prop("selectedIndex", -1);
                    _protocols.append($("<option>").attr("label", " ").text(self.SELECT_PROMPT));

                    {
                        for (var _i in aParser)
                        {
                            if (aParser.hasOwnProperty(_i))
                            {
                                _protocols.append($("<option>").text(_i));
                            }
                        }
                    }
                }
                self.FWProtocol = $("<tbody>").attr("id", "div_protocols");
                self.FWProtocol.appendTo(self.FWMain);

                _protocols.on("change", function ()
                {//customize opions

                    self.FWProtocol.empty();
                    self.clean_errors();

                    var _selected = $(this).find("option:selected");
                    var _val = _selected.val();
                    if (_val !== self.SELECT_PROMPT)
                    {
                        var _text = _selected.text();
                        var _fail = false;
                        console.log("Protocol " + _val);
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

                            var _tr = $("<tr>").appendTo(self.FWProtocol);
                            $("<label>").text("Parsing by: ").appendTo($("<td>").appendTo(_tr));
                            var _by = _protocols.clone().attr({
                                id: "parser"
                            }).val(_text);
                            _by.find("option").eq(0).remove();

                            _by.appendTo($("<td>").appendTo(_tr));
                        }
                    }
                });
            }
            ,
            setup_demand: function (_from, _protocol, _head, _parser)
            {
                var self = sniffer.MIWantToReceive;
                var _request =
                    {
                        dem: {}
                    };
                _request.dem.id = _from;
                _request.dem.pl = _protocol;
                _request.dem[_protocol] = _head;
                if (_parser)
                    _request.dem["by_parser"] = _parser;
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
        latest_sniffed_packet: 1
    }

