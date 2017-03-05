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
                let _tmp = {};
                if (aVer.hasOwnProperty("mj") || aVer.hasOwnProperty("mn") || aVer.hasOwnProperty("rs"))
                    return aVer.mj + "." + aVer.mn + "." + aVer.rs;
                else
                    return "undefined";
            },
            getFromString: function (aStr)
            {
                console.log("Parsing " + aStr);
                let _rval = {ver: {}};
                let _reg = /^(\d+)\.?(\d+)?\.?(\d+)?/;
                let _found = aStr.match(_reg);
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
                let _tmp = {};
                _tmp["Name"] = aId.n;
                $.extend(_tmp, jQuery.to_representation_form(
                    {
                        uuid: aId.uuid
                    }, data_info));
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
                return uuid;
 /*               let _num = parseInt(uuid, 10);
                let _str = _num.toString(16).toUpperCase();
                /!*"%02x-%02x-"
                 "%02x-%02x-"
                 "%02x-%02x-"
                 "%02x-%02x"*!/
                return "0x" + _str.substr(0,2) + "-0x" + _str.substr(2,2) +
                    "-0x" + _str.substr(4,2) + "-0x" + _str.substr(6,2)+
                    "-0x" + _str.substr(8,2) + "-0x" + _str.substr(10,2)+
                    "-0x" + _str.substr(12,2) + "-0x" + _str.substr(14,2);*/
            }
        },
        type: {
            getName: function ()
            {
                return "Type";
            },
            toRepresentation: function (aInfo)
            {
                let _tmp = {};
                switch (aInfo)
                {
                    case "1":
                        return "Kernel";
                        break;
                    case "2":
                        return "Consumer";
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
                let _tmp = {};
                _tmp["PID"] = aInfo.pid;
                _tmp["Path"] = aInfo.path;
                let _date = new Date(1000 * aInfo.tm);
                _tmp["Start time"] = _date.toUTCString();

                $.extend(_tmp, jQuery.to_representation_form(
                    {
                        type: aInfo.type,
                        id: aInfo.id,
                        ver: aInfo.ver
                    }, data_info));
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
                let _tmp = {};
                _tmp["Number of connection"] = aInfo.last;
                $.extend(_tmp, jQuery.to_representation_form(
                    {
                        kerd: aInfo.kerd
                    }, data_info));
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
                let _tmp = {};
                _tmp["#"] = aInfo.numd;
                $.extend(_tmp, jQuery.to_representation_form(
                    {
                        DescriptorInfo: aInfo.DescriptorInfo
                    }, data_info));
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
                let _tmp = {};
                $.extend(_tmp, jQuery.to_representation_form(
                    {
                        info: aInfo.info,
                        addr: aInfo.addr
                    }, data_info));
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
                let _tmp = {};
                if (aVal.hasOwnProperty("epath") && !$.isEmptyObject(aVal.epath))
                    _tmp["Search directory"] = aVal.epath;
                else
                    _tmp["Search directory"] = "Current";
                $.extend(_tmp, jQuery.to_representation_form(
                    {
                        emod: aVal.emod
                    }, data_info));
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
                let _tmp = {};
                _tmp["Name"] = aVal.ename;

                if (aVal.hasOwnProperty("lib"))
                    $.extend(_tmp, jQuery.to_representation_form(
                        {
                            lib: aVal.lib
                        }, data_info));
                else
                    _tmp[data_info.lib.getName()] = "Cannot load";
                if (aVal.hasOwnProperty("reg"))
                    $.extend(_tmp, jQuery.to_representation_form(
                        {
                            reg: aVal.reg
                        }, data_info));
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
                let _tmp = {};
                _tmp["Registring"] = aVal.type;
                $.extend(_tmp, jQuery.to_representation_form(
                    {
                        ver: aVal.ver
                    }, data_info));
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
                let _tmp = {};
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
                let _re1 = /^[^\s@\.]$/;
                let _re2 = /^[^\s@\.]+@?[^\s@]+/;
                let _re3 = /^@?[^\s@]+$/;

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
                let _tmp = {};
                _tmp["IP"] = aVal.ip;
                _tmp["Port"] = aVal.port;
                return _tmp;
            }
        },
        usdt: {
            getName: function ()
            {
                return "Data Info";
            },
            toRepresentation: function (aVal)
            {
                let _protocol;
                if (!aVal.hasOwnProperty("pl") || $.isEmptyObject(aVal.pl))
                    _protocol = "raw";
                else
                    _protocol = aVal.pl;
                let _tmp = {};

                // if(_tmp["Protocol"]=="raw")
                //     _tmp["#"]=aVal.nr;
                _tmp["Packet Number"] = aVal.pn;
                _tmp["Protocol"] = _protocol;

                let _ver = jQuery.to_representation_form(
                    {
                        ver: aVal.ver
                    }, data_info);
                $.extend(_tmp, _ver);

                _tmp["From"] = data_info.id.toRepresentation(aVal.rtg_.id);

                let _to = jQuery.to_representation_form(aVal.uuids, data_info);
                _tmp["To"] = _to[data_info.uuid.getName()];

                let _reg = jQuery.to_representation_form(aVal.reg, data_info);
                _tmp["Registered By"] = _reg[data_info.uuid.getName()];

                return _tmp;
            }
        },
    };
function get_id_for(aVal)
{
    let _val = aVal.replace(/[!"#$%&'()*+,.\/:;<=>?@[\\\]^{|}~]/g, '\\$&');

    return _val.replace(/[\s]/g, '_');
    ;
}