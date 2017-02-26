/* 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

$(document).ready(function ()
{

    $(document).ajaxComplete(function (event, jqXHR, ajaxOptions)
        {
            var _response = jqXHR.getResponseHeader("server_uuid");
            storage.kernel_uuid = _response;
        }
    );
    (function ($)
    {
        jQuery.get_protocol_parser = function (aVal)
        {
            if (!jQuery.protocol_parser)
                jQuery.protocol_parser = {};

            if (jQuery.protocol_parser.hasOwnProperty(aVal))
                return jQuery.protocol_parser[aVal];
            var _file = "js/parser/" + aVal + ".js";
            jQuery.ajax(
                {
                    url: _file,
                    async: false,
                    type: "GET",
                    dataType: "script",
                    cache: true
                }).error(function (jqXHR, textStatus)
            {
                //throw "cannot load protocol parser from " + _file;
            });
            if (jQuery.protocol_parser.hasOwnProperty(aVal))
                return jQuery.protocol_parser[aVal];
            return null;
        };
        jQuery.toRepresentationHead = function (aProtocol, aVal)
        {
            const _protocol = aProtocol;

            var _parser = jQuery.get_protocol_parser(_protocol);
            if (_parser && _parser.hasOwnProperty("head"))
                return jQuery.to_representation_form(aVal, _parser["head"]);
            else//convert object to array
                return jQuery.to_representation_form(aVal);
        }
        jQuery.head_entry_field = function (aProtocol, aTo)
        {
            const _protocol = aProtocol;
            var _parser = $.get_protocol_parser(_protocol);
            if (_parser && _parser.hasOwnProperty("head"))
            {
                _parser.head.entry_field(aTo);
            } else
            {
                var _tr = $("<tr>").appendTo(aTo);
                $("<label>").text('Enter the protocol head.').appendTo($("<td>").appendTo(_tr));
                $("<textarea>").attr({
                    id: "json_input",
                    placeholder: "as JSON"
                }).appendTo($("<td>").appendTo(_tr));
            }
            aTo.addClass(_protocol);
        }
        jQuery.serialize_head = function (aProtocol, aFrom)
        {
            const _protocol = aProtocol;
            var _parser = $.get_protocol_parser(_protocol);
            if (_parser && _parser.hasOwnProperty("head"))
            {
                return _parser.head.serialize(aFrom);
            } else
            {
                try
                {
                    var c = $.parseJSON($("#json_input", aFrom).val());
                } catch (e)
                {
                    $("#json_input", aFrom).addClass("ui-state-error");
                    throw e;
                }
                return $("#json_input", aFrom).val();
            }
        }
        jQuery.object_to_table = function (_array, aTable)
        {
            const _is_array = $.isArray(_array);

            if ($.isArray(_array) || (typeof _array === 'object'))
            for (var i in _array)
            {
                if (_array.hasOwnProperty(i))
                {
                    var _tr = jQuery("<tr>").appendTo(aTable);
                    var _td_data;
                    if (!_is_array)
                    {
                        _tr.append("<td>" + i + "</td>");
                        _td_data = $("<td>").appendTo(_tr);
                    } else
                        _td_data = $("<td>").attr("colspan", 2).appendTo(_tr);

                    if ($.isArray(_array[i]) || (typeof _array[i] === 'object'))
                    {
                        var _table = jQuery("<table>").appendTo(_td_data);
                        jQuery.object_to_table(_array[i], _table);
                    } else
                    {
                        _td_data.text(_array[i]);
                    }
                }
            }else
                $("<td>").text(_array).appendTo($("<tr>").appendTo(aTable));
        }

        jQuery.to_representation_form = function (aVal, aStartPath)
        {
            function handle_array(aArray, aKey)
            {
                var _array = new Array;
                $.each(aArray, function (i, obj)
                {
                    if (aStartPath != null && aStartPath[aKey] != null && aStartPath[aKey].hasOwnProperty("toRepresentation"))
                    {
                        _array.push(aStartPath[aKey].toRepresentation(obj));
                    }
                    else if ($.isArray(obj) || (typeof obj === 'object'))
                    {
                        _array.push(jQuery.to_representation_form(obj, aStartPath));
                    } else
                    {
                        _array.push(obj);
                    }
                });
                return _array;
            }

            if ($.isArray(aVal))
            {
                return handle_array(aVal);
            }
            else if (typeof aVal === 'object')
            {
                var _tmp = {};
                for (var i in aVal)
                {
                    if (aVal.hasOwnProperty(i))
                    {
                        if (aStartPath != null && aStartPath[i] != null && aStartPath[i].hasOwnProperty("toRepresentation"))
                        {
                            if ($.isArray(aVal[i]))
                                _tmp[aStartPath[i].getName()] = handle_array(aVal[i], i);
                            else
                                _tmp[aStartPath[i].getName()] = aStartPath[i].toRepresentation(aVal[i]);
                        }
                        else if (($.isArray(aVal[i]) && aVal[i].length > 0) || ((typeof aVal[i] === 'object') && !$.isEmptyObject(aVal[i])))
                        {
                            _tmp[i] = jQuery.to_representation_form(aVal[i], aStartPath);
                        } else
                        {
                            _tmp[i] = aVal[i];
                        }
                    }
                }
                return _tmp;
            } else if (aStartPath != null && aStartPath.hasOwnProperty("toRepresentation"))
            {
                var _tmp = {};
                _tmp[aStartPath.getName()] = aStartPath.toRepresentation(aVal)
                return _tmp;
            } else
            {
                return aVal;
            }
        }
    })(jQuery);

    document.title = "UDT";
    $("#tabs").tabs(
        {
            active: 2,
            collapsible: false,
            event: "click",
            // beforeLoad: function(event,ui)
            //{
            //   ui.jqXHR.fail(function(){ ui.panel.html("Couldn't load this tab.");
            //                            });
            //}

            activate: function (event, ui)
            {
                var _tab_id = ui.newPanel.attr("id");
                switch (_tab_id)
                {
                    case "tabs-info":
                        udt_info(ui.newPanel.empty());
                        break;
                    case "tabs-sniffer":
                        sniffer.open(ui.newPanel);
                        break;
                    default:
                        ui.newPanel.empty().html("Couldn't load this tab");
                        break;
                }
            }
        }
    );


});

