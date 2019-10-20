
jQuery.get_protocol_parser().raw =
    {
        head: {
            getName:function()
            {
                return "raw";
            },
            entry_field: function (aPanel)
            {
                {
                    let _tr = $("<tr>").appendTo(aPanel);
                    $("<label>").text("The number").appendTo($("<td>").appendTo(_tr));
                    $("<input>").attr({
                        id: "number",
                        type: "number",
                        min: "0",
                        step: "1"
                    }).appendTo($("<td>").appendTo(_tr));
                }
                {
                    let _tr = $("<tr>").appendTo(aPanel);
                    $("<label>").text("The version").appendTo($("<td>").appendTo(_tr));
                    $("<input>").attr({
                        id: "version"
                    }).change(function ()
                    {
                        let _reg = /[^(\d)+\.(\d)+\.(\d)]/g;
                        if (this.value.match(_reg))
                        {
                            this.value = this.value.replace(_reg, '');
                        }
                    }).appendTo($("<td>").appendTo(_tr));
                }
            },
            serialize: function (aPanel)
            {
                let _rval = {};
                let _number = $("#number", aPanel);
                if (_number)
                {
                    _rval.num = parseInt(_number.val(), 10);
                    if (isNaN(_rval.num) || _rval.num < 0)
                    {
                        _number.addClass("ui-state-error");
                        throw "invalid number";
                    }
                } else
                    throw "no number";

                let _version = $("#version", aPanel);
                if (_number)
                {
                    let _ver = data_info.ver.getFromString(_version.val());
                    $.extend(_rval, _ver);
                }
                return _rval;
            },
            toRepresentation: function (aInfo)
            {
                let _table =
                    {
                        "#:": parseInt(aInfo.num, 10)
                    };
                if(aInfo.hasOwnProperty("ver") &&aInfo.ver)
                    _table[data_info.ver.getName()] = data_info.ver.toRepresentation(aInfo.ver);
                return _table;
            }
        }
    };