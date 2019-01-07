(function ($)
{
    jQuery.get_protocol_parser = function (aVal)
    {
        let self = jQuery.get_protocol_parser;
        if (self.protocol_parser == undefined)
        {
            self.protocol_parser = {}
        }

        if (!aVal)
            return self.protocol_parser;

        if (self.protocol_parser.hasOwnProperty(aVal))
            return self.protocol_parser[aVal];
        let _file = "js/parser/" + aVal + ".js";
        jQuery.ajax(
            {
                url: _file,
                async: false,
                type: "GET",
                dataType: "script",
                cache: true
            }).done(function ()
        {
            storage.parsers.add(aVal);
        }).error(function (jqXHR, textStatus)
        {
            //throw "cannot load protocol parser from " + _file;
        });
        if (self.protocol_parser.hasOwnProperty(aVal))
            return self.protocol_parser[aVal];
        return null;
    };
    //$.toRepresentationData(aData.udata.data, _head);
    jQuery.toRepresentationData = function (aProtocol, aData, aHead, aId)
    {
        let _parser = jQuery.get_protocol_parser(aProtocol);
        if (_parser && _parser.hasOwnProperty("data"))
        {
            return jQuery.to_representation_form({
                "data": aData
            }, _parser, aHead);
        } else
            return jQuery.to_representation_form({
                "data": aData
            });
    }
    jQuery.toRepresentationHead = function (aProtocol, aVal)
    {
        const _protocol = aProtocol;

        let _parser = jQuery.get_protocol_parser(_protocol);
        if (_parser && _parser.hasOwnProperty("head"))
            return jQuery.to_representation_form({
                "head": aVal
            }, _parser);
        else//convert object to array
            return jQuery.to_representation_form({
                "head": aVal
            });
    }
    jQuery.head_entry_field = function (aProtocol, aTo)
    {
        const _protocol = aProtocol;
        let _parser = $.get_protocol_parser(_protocol);
        if (_parser && _parser.hasOwnProperty("head"))
        {
            _parser.head.entry_field(aTo);
        } else
        {
            let _tr = $("<tr>").appendTo(aTo);
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
        let _parser = $.get_protocol_parser(_protocol);
        if (_parser && _parser.hasOwnProperty("head"))
        {
            return _parser.head.serialize(aFrom);
        } else
        {
            try
            {
                let c = $.parseJSON($("#json_input", aFrom).val());
                return c;
            } catch (e)
            {
                $("#json_input", aFrom).addClass("ui-state-error");
                throw e;
            }
        }
    }
    jQuery.object_to_table = function (_array, aTable,aOnlyChild)
    {
        const _is_array = $.isArray(_array);
        if ($.isArray(_array) || (typeof _array === 'object'))
            for (let i in _array)
            {
                if (_array.hasOwnProperty(i))
                {
                    if(!aOnlyChild)
                    {
                        let _tr = jQuery("<tr>").appendTo(aTable);
                        let _td_data;
                        if (!_is_array)
                        {
                            _tr.append("<td>" + i + "</td>");
                            _td_data = $("<td>").appendTo(_tr);
                        } else
                            _td_data = $("<td>").attr("colspan", 2).appendTo(_tr);

                        if ($.isArray(_array[i]) || (typeof _array[i] === 'object'))
                        {
                            let _table = jQuery("<table>").appendTo(_td_data);
                            jQuery.object_to_table(_array[i], _table);
                            if ($.isArray(_array[i]))
                                $("tr >td", _table).addClass(get_id_for("class-" + i));
                            else
                                _table.addClass(get_id_for("class-" + i));
                        } else
                        {
                            _td_data.text(_array[i]).addClass(get_id_for("class-" + i));
                        }
                    }else
                    {
                        if ($.isArray(_array[i]) || (typeof _array[i] === 'object'))
                        {
                            let _table = jQuery("<table>")
                            jQuery.object_to_table(_array[i], _table);
                            if ($.isArray(_array[i]))
                                $("tr >td", _table).addClass(get_id_for("class-" + i));
                            else
                                _table.addClass(get_id_for("class-" + i));
                            $( _table.children()).appendTo(aTable);

                        } else
                        {
                            let _tr = jQuery("<tr>").appendTo(aTable);
                            let  _td_data = $("<td>").attr("colspan", 2).appendTo(_tr);
                            _td_data.text(_array[i]).addClass(get_id_for("class-" + i));
                        }
                    }
                }
            }
        else
            $("<td>").attr("colspan", 2).text(_array).appendTo($("<tr>").appendTo(aTable));
    }

    jQuery.to_representation_form = function (aVal, aStartPath, aArgs)
    {
        function handle_array(aArray, aKey)
        {
            let _array = new Array;
            $.each(aArray, function (i, obj)
            {
                if (aStartPath != null && aStartPath[aKey] != null && aStartPath[aKey].hasOwnProperty("toRepresentation"))
                {
                    _array.push(aStartPath[aKey].toRepresentation(obj, aArgs));
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

        function define_as_class(aField, i)
        {
            // Object.defineProperty(aField, "class", {
            //      enumerable: false,
            //      value: i,
            //      configurable: false
            //  });
        }

        if ($.isArray(aVal))
        {
            return handle_array(aVal);
        }
        else if (typeof aVal === 'object')
        {
            let _tmp = {};
            for (let i in aVal)
            {
                if (aVal.hasOwnProperty(i))
                {
                    let _name = i;
                    if (aStartPath != null && aStartPath[i] != null && aStartPath[i].hasOwnProperty("toRepresentation"))
                    {
                        _name = aStartPath[i].getName();
                        if ($.isArray(aVal[i]))
                            _tmp[_name] = handle_array(aVal[i], i);
                        else
                            _tmp[_name] = aStartPath[i].toRepresentation(aVal[i], aArgs);
                    }
                    else if (($.isArray(aVal[i]) && aVal[i].length > 0) || ((typeof aVal[i] === 'object') && !$.isEmptyObject(aVal[i])))
                    {
                        _tmp[_name] = jQuery.to_representation_form(aVal[i], aStartPath);
                    } else
                    {
                        _tmp[_name] = aVal[i];
                    }
                    define_as_class(_tmp[_name], i);
                }
            }
            return _tmp;
        } else if (aStartPath != null && aStartPath.hasOwnProperty("toRepresentation"))
        {
            //let _tmp = {};
            //_tmp[aStartPath.getName()] = aStartPath.toRepresentation(aVal, aArgs)
            //define_as_class(aStartPath.getName(), aStartPath.getName());
            //return _tmp;
            return aStartPath.toRepresentation(aVal, aArgs);
        } else
        {
            return aVal;
        }
    }
    $(document).ajaxComplete(function (event, jqXHR, ajaxOptions)
        {
            let _response = jqXHR.getResponseHeader("server_uuid");
            if (storage)
                storage.kernel_uuid = _response;
        }
    );
})(jQuery);
