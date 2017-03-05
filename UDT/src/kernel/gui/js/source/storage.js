/**
 * Created by Sergey on 19.02.2017.
 */
function update_udt_net(aSinc)
{
    let self = storage.parsers;
    $.ajax(
        {
            type: "GET",
            data: {query: "udt_net"},
            dataType: "json",
            async: aSinc,
            cache: true
        }).done(function (data)
    {
        storage.susbscriber_net.set(data.state.udt_net);
        storage.roads.set(data.state.udt_net.roads);
    }).error(function (jqXHR, textStatus)
    {
        alert("Could not load susbscribers as " + textStatus);
    });
};
var storage =
        {
            //
            get kernel_uuid()
            {
                return storage["FKernelUuid"];
            },
            set kernel_uuid(aVal)
            {
                if (storage.hasOwnProperty("FKernelUuid") && aVal !== storage.FKernelUuid)
                {
                    console.error("Invalid uuid " + aVal + " lastest " + storage.FKernelUuid);
                    alert("The page will be reloaded as the server has been reloaded!");
                    location.reload(true);
                }
                storage["FKernelUuid"] = aVal;
            },
            sniffed_data: {
                get: function (aVal)
                {
                    let self = storage.sniffed_data;
                    if (!aVal)
                        return self.data;

                    for (let i = 0; i < self.data.length; ++i)
                    {
                        if (self.data[i].seq_num == aVal)
                            return self.data[i];
                    }
                    return null;
                },
                set: function (aVal)
                {
                    let self = storage.sniffed_data;

                    if ($.isArray(aVal))
                    {
                        for (let i = aVal.length; --i >= 0;)
                        {
                            if (aVal.hasOwnProperty(i))
                            {
                                self.data.push(aVal[i]);
                                self.last_packet = parseInt(aVal[i].seq_num, 10);
                                try
                                {
                                    self.on_new(aVal[i]);
                                } catch (e)
                                {
                                }
                            }
                        }

                    } else if (aVal)
                    {
                        self.data.push(aVal);
                        self.last_packet = parseInt(aVal.seq_num, 10);
                        try
                        {
                            self.on_new(aVal);
                        } catch (e)
                        {
                        }
                    }

                }
                ,
                on_new: function (event)
                {
                }
                ,
                update: function ()
                {
                    let self = storage.sniffed_data;
                    $.ajax(
                        {
                            url: "sniffer/snif_state",
                            type: "POST",
                            data: JSON.stringify(
                                {
                                    "seq_num": self.last_packet
                                }),
                            dataType: "json",
                            contentType: "application/json; charset=utf-8"
                        }).done(function (data)
                    {
                        storage.dems.set(data.snif_state.dems);
                        self.set(data.snif_state.sniffed_data);
                    })
                }
                ,
                last_packet: "1",
                data: []
            },
            dems: {
                get: function (aVal)
                {
                    let self = storage.dems;
                    if (!aVal)
                        return self.data;

                    for (let i = 0; i < self.data.length; ++i)
                    {
                        if (self.data[i].hand == aVal)
                            return self.data[i];
                    }

                    return null;
                }
                ,
                set: function (aVal)
                {
                    let self = storage.dems;
                    let _dems = new Array;
                    if (aVal.hasOwnProperty("dem"))
                    {
                        if ($.isArray(aVal.dem))
                            _dems = aVal.dem;
                        else
                            _dems.push(aVal.dem);
                    }

                    let _exist = {};
                    _dems.forEach(function (item)
                    {
                        _exist[item.hand] = item;
                    });

                    self.data.forEach(function (item)
                    {
                        const _id = item.hand;
                        if (!_exist.hasOwnProperty(_id))
                        {
                            self.removed(item);
                        } else
                            _exist[_id] = undefined;

                    });
                    for (let _i in _exist)
                    {
                        if (_exist.hasOwnProperty(_i) && _exist[_i])
                        {
                            self.created(_exist[_i]);
                        }
                    }
                    ;
                    self.data = _dems;
                    try
                    {
                        self.updated(_dems);
                    } catch (e)
                    {

                    }
                }
                ,
                updated: function (event)
                {
                }
                ,
                removed: function (event)
                {
                }
                ,
                created: function (event)
                {
                }
                ,
                update: function (aSinc = true)
                {
                    let self = storage.dems;
                    $.ajax(
                        {
                            url: "sniffer/snif_state/dems",
                            type: "POST",
                            data: JSON.stringify(
                                {
                                    some: "val"
                                }),
                            async: aSinc,
                            dataType: "json",
                            contentType: "application/json; charset=utf-8"
                        }).done(function (data)
                    {
                        self.set(data.snif_state.dems);
                    })
                }
                ,
                data: []
            },
            parsers: {
                update: function (aSinc = true)
                {
                    let self = storage.parsers;
                    $.ajax(
                        {
                            type: "GET",
                            data: {query: "parsers"},
                            dataType: "json",
                            async: aSinc,
                            cache: true
                        }).done(function (data)
                    {
                        self.dataOfKernel.length = 0;
                        for (let _i in data.state.parsers)
                        {
                            if (data.state.parsers.hasOwnProperty(_i))
                            {
                                self.dataOfKernel.push(_i);
                                self.add(_i)
                            }
                        }
                        Object.defineProperty(self.dataOfKernel, "inited", {
                            enumerable: false,
                            value: true,
                            configurable:false
                        });
                    }).error(function (jqXHR, textStatus)
                    {
                        alert("Could not load gui protocols as " + textStatus);
                    });
                    let _pars = jQuery.get_protocol_parser();
                    for (let _i in _pars)
                    {
                        if (_pars.hasOwnProperty(_i))
                        {
                            self.add(_i)
                        }
                    }
                    self.updated(self.data);
                    Object.defineProperty(self.data, "inited", {
                        enumerable: false,
                        value: true,
                        configurable:false
                    });
                }, get: function ()
                {
                    let self = storage.parsers;
                    if (!self.data.hasOwnProperty("inited"))
                    {
                        self.update(false);
                    }
                    return self.data;
                },
                getForKernel: function ()
                {
                    let self = storage.parsers;
                    if (!self.dataOfKernel.hasOwnProperty("inited"))
                    {
                        self.update(false);
                    }
                    return self.dataOfKernel;
                },
                updated: function (event)
                {
                },
                on_new: function (event)
                {
                },
                add: function (aVal)
                {
                    let self = storage.parsers;
                    if ($.inArray(aVal, self.data) == -1)
                    {
                        self.data.push(aVal);
                        self.on_new(aVal);
                    }
                }
                ,
                data: [],
                dataOfKernel: []
            },
            susbscriber_net: {
                set: function (aData)
                {
                    let self = storage.susbscriber_net;
                    let _as_array = new Array;
                    if (aData.hasOwnProperty("kinf"))
                    {
                        if ($.isArray(aData.kinf))
                            _as_array = aData.kinf;
                        else
                            _as_array.push(aData.kinf);
                    }
                    let _new_sub = {};
                    let _conv = data_info.uuid.toRepresentation;
                    _as_array.forEach(function (item)
                    {
                        _new_sub[_conv(item.info.id.uuid)] = item.info;

                        if (item.hasOwnProperty("klink"))
                        {
                            if ($.isArray(item.klink))
                            {
                                item.klink.forEach(function (aClient)
                                {
                                    if (!_new_sub.hasOwnProperty(_conv(aClient.info.id.uuid)))
                                        _new_sub[_conv(aClient.info.id.uuid)] = aClient.info;
                                });
                            }
                            else if (!_new_sub.hasOwnProperty(_conv(item.klink.info.id.uuid)))
                                _new_sub[_conv(item.klink.info.id.uuid)] = item.klink.info;
                        }
                    });

                    let _exist = $.extend({}, _new_sub);
                    for (let _i in self.data)
                    {
                        if (self.data.hasOwnProperty(_i))
                        {
                            if (!_exist.hasOwnProperty(_i))
                            {
                                _new_sub[_i] = self.data[_i];
                                Object.defineProperty(_new_sub[_i], "is_removed", {
                                    enumerable: false,
                                    value: true,
                                    configurable:false
                                });
                                self.removed(_i);
                            }
                            else
                                _exist[_i] = undefined
                        }
                    }
                    for (let _i in _exist)
                    {
                        if (_exist.hasOwnProperty(_i) && _exist[_i])
                        {
                            self.on_new(_exist[_i]);
                        }
                    }
                    self.dataSub = _as_array;
                    self.data = _new_sub;
                    try
                    {
                        self.updated(_as_array);
                    } catch (e)
                    {

                    }
                    Object.defineProperty(self.data, "inited", {
                        enumerable: false,
                        value: true,
                        configurable:false
                    });
                },
                update: function (aSinc = true)
                {
                    update_udt_net(aSinc);
                }, get: function (aVal)
                {
                    let self = storage.susbscriber_net;
                    if (!self.data.hasOwnProperty("inited"))
                    {
                        self.update(false);
                    }

                    if (!aVal)
                        return self.data;

                    if (!self.data.hasOwnProperty(aVal))
                        self.update(false);

                    return self.data[aVal];
                },
                updated: function (event)
                {
                },
                on_new: function (event)
                {
                }, removed: function (event)
                {
                },
                data: [],
                dataSub: [],
            }
            ,
            roads: {
                set: function (aData)
                {
                    return;//todo create graph
                },
                update: function (aSinc = true)
                {
                    update_udt_net(aSinc);
                }, get: function (aVal)
                {
                    let self = storage.roads;
                    if (!self.data.hasOwnProperty("inited"))
                    {
                        self.update(false);
                    }

                    if (!aVal)
                        return self.data;
                    return self.data[aVal];
                },
                add: function (aVal)
                {
                    let self = storage.roads;
                    if ($.inArray(aVal, self.data) == -1)
                    {
                        self.data.push(aVal);
                        self.on_new(aVal);
                    }
                },
                updated: function (event)
                {
                },
                on_new: function (event)
                {
                },
                removed: function (event)
                {
                },
                data: []
            }
        }
    ;
