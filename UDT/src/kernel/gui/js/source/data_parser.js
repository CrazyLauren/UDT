/* 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
var data_info =
    {
        ver: {
            getName: function ()
            {
                return "Version";
            },
            toRepresentation: function (aVer)
            {
                var _tmp = {};
                if (aVer.hasOwnProperty("mj") || aVer.hasOwnProperty("mn") || aVer.hasOwnProperty("rs"))
                    _tmp[data_info.ver.getName()] = aVer.mj + "." + aVer.mn + "." + aVer.rs;
                else
                    _tmp[data_info.ver.getName()] = "undefined";
                return _tmp;
            },
            getFromString: function (aStr)
            {
                console.log("Parsing " + aStr);
                var _rval = {ver: {}};
                var _reg = /^(\d+)\.?(\d+)?\.?(\d+)?/;
                var _found = aStr.match(_reg);
                if (_found)
                {
                    if (_found[1])
                        _rval.ver.mj = _found[1];
                    if (_found[2])
                        _rval.ver.mn = _found[2];
                    if (_found[3])
                        _rval.ver.rs = _found[3];
                }
                return _rval;
            }

        },
        id: {
            getName: function ()
            {
                return "Id";
            },
            toRepresentation: function (aId)
            {
                var _tmp = {};
                _tmp["Name"] = aId.n;
                $.extend(_tmp, jQuery.to_representation_form(
                    {
                        uuid: aId.uuid
                    },data_info));
                //$.extend(_tmp,
                //    data_info.uuid.toRepresentation(aId.uuid));
                return _tmp;
            }
        },
        uuid: {
            getName: function ()
            {
                return "UUID";
            },
            toRepresentation: function (uuid)
            {
                var _num = parseInt(uuid, 10);
                return _num.toString(16).toUpperCase();
            }
        },
        type: {
            getName: function ()
            {
                return "Type";
            },
            toRepresentation: function (aInfo)
            {
                var _tmp = {};
                switch (aInfo)
                {
                    case "1":
                        _tmp["Type"] = "Kernel";
                        break;
                    case "2":
                        _tmp["Type"] = "Consumer";
                        break;
                }
                return _tmp;
            }
        },
        info: {
            getName: function ()
            {
                return "Info";
            },
            toRepresentation: function (aInfo)
            {
                var _tmp = {};
                _tmp["PID"] = aInfo.pid;
                _tmp["Path"] = aInfo.path;
                var _date = new Date(1000 * aInfo.tm);
                _tmp["Start time"] = _date.toUTCString();

                $.extend(_tmp, jQuery.to_representation_form(
                    {
                        type: aInfo.type,
                        id: aInfo.id,
                        ver: aInfo.ver
                    },data_info));
                return _tmp;
            }
        },
        desc: {
            getName: function ()
            {
                return "Connections";
            },
            toRepresentation: function (aInfo)
            {
                var _tmp = {};
                _tmp["Number of connection"] = aInfo.last;
                $.extend(_tmp, jQuery.to_representation_form(
                    {
                        kerd: aInfo.kerd
                    },data_info));
                return _tmp;
            }
        },
        kerd: {
            getName: function ()
            {
                return "Descriptor";
            },
            toRepresentation: function (aInfo)
            {
                var _tmp = {};
                _tmp["#"] = aInfo.numd;
                $.extend(_tmp, jQuery.to_representation_form(
                    {
                        DescriptorInfo: aInfo.DescriptorInfo
                    },data_info));
                return _tmp;
            }
        },
        DescriptorInfo: {
            getName: function ()
            {
                return "Connect info";
            },
            toRepresentation: function (aInfo)
            {
                var _tmp = {};
                $.extend(_tmp, jQuery.to_representation_form(
                    {
                        info: aInfo.info,
                        addr: aInfo.addr
                    },data_info));
                return _tmp;
            }
        },
        res: {
            getName: function ()
            {
                return "External modules";
            },
            toRepresentation: function (aVal)
            {
                var _tmp = {};
                if (aVal.hasOwnProperty("epath") && !$.isEmptyObject(aVal.epath))
                    _tmp["Search directory"] = aVal.epath;
                else
                    _tmp["Search directory"] = "Current";
                $.extend(_tmp, jQuery.to_representation_form(
                    {
                        emod: aVal.emod
                    },data_info));
                return _tmp;
            }
        },
        emod: {
            getName: function ()
            {
                return "Plugin";
            },
            toRepresentation: function (aVal)
            {
                var _tmp = {};
                _tmp["Name"] = aVal.ename;

                if (aVal.hasOwnProperty("lib"))
                    $.extend(_tmp, jQuery.to_representation_form(
                        {
                            lib: aVal.lib
                        },data_info));
                else
                    _tmp[data_info.lib.getName()] = "Cannot load";
                if (aVal.hasOwnProperty("reg"))
                    $.extend(_tmp, jQuery.to_representation_form(
                        {
                            reg: aVal.reg
                        },data_info));
                else
                    _tmp[data_info.reg.getName()] = "Not exist";
                return _tmp;
            }
        },
        reg: {
            getName: function ()
            {
                return "Registator";
            },
            toRepresentation: function (aVal)
            {
                var _tmp = {};
                _tmp["Registring"] = aVal.type;
                $.extend(_tmp, jQuery.to_representation_form(
                    {
                        ver: aVal.ver
                    },data_info));
                return _tmp;
            }
        },
        lib: {
            getName: function ()
            {
                return "Library";
            },
            toRepresentation: function (aVal, aTable)
            {
                var _tmp = {};
                if (aVal.hasOwnProperty("name") && !$.isEmptyObject(aVal.name))
                    _tmp["From library"] = aVal.name;
                else
                    _tmp["From library"] = "Static linked";
                return _tmp;
            }
        },
        program_name: {
            checking: function (aVal)
            {
                var _re1 = /^[^\s@\.]$/;
                var _re2 = /^[^\s@\.]+@?[^\s@]+/;
                var _re3 = /^@?[^\s@]+$/;

                return _re1.test(aVal) || _re2.test(aVal) || _re3.test(aVal);
            }
        },
        addr: {
            getName: function ()
            {
                return "Address";
            },
            toRepresentation: function (aVal)
            {
                var _tmp = {};
                _tmp["IP"] = aVal.ip;
                _tmp["Port"] = aVal.port;
                return _tmp;
            }
        }
    };
function get_id_for(aVal)
{
    return aVal.replace(/[!"#$%&'()*+,.\/:;<=>?@[\\\]^{|}~]/g, '\\$&');
}