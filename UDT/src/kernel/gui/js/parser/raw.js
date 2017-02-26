/* 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

console.log("load raw");

jQuery.protocol_parser.raw =
    {
        head: {
            entry_field: function (aPanel)
            {
                {
                    var _tr = $("<tr>").appendTo(aPanel);
                    $("<label>").text("The number").appendTo($("<td>").appendTo(_tr));
                    $("<input>").attr({
                        id: "number",
                        type: "number",
                        min: "0",
                        step: "1"
                    }).appendTo($("<td>").appendTo(_tr));
                }
                {
                    var _tr = $("<tr>").appendTo(aPanel);
                    $("<label>").text("The version").appendTo($("<td>").appendTo(_tr));
                    $("<input>").attr({
                        id: "version"
                    }).change(function ()
                    {
                        var _reg = /[^(\d)+\.(\d)+\.(\d)]/g;
                        if (this.value.match(_reg))
                        {
                            this.value = this.value.replace(_reg, '');
                        }
                    }).appendTo($("<td>").appendTo(_tr));
                }
            },
            serialize: function (aPanel)
            {
                var _rval = {};
                var _number = $("#number", aPanel);
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

                var _version = $("#version", aPanel);
                if (_number)
                {
                    var _ver = data_info.ver.getFromString(_version.val());
                    $.extend(_rval, _ver);
                }
                return _rval;
            },
            toRepresentation: function (aInfo)
            {
                var _table =
                    {
                        "#:": parseInt(aInfo.num, 10)
                    };

                data_info.ver.toRepresentation(aInfo.ver, _table);
                return _table;
            }
        }
    };