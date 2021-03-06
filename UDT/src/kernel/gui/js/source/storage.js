function update_udt_net(aSinc)
{
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
                getFailFor: function (aVal)
                {
                    let self = storage.sniffed_data;
                    if (!aVal)
                        return self.dataFails;

                    const _from = aVal.udata.usdt.rtg_.id.uuid;
                    const _packet = aVal.udata.usdt.pn;
                    let _fails = new Array;
                    self.dataFails.forEach(function (item)
                    {
                        const _fail_from = item.fsend.usdt.rtg_.id.uuid;
                        const _fail_packet = item.fsend.usdt.pn;
                        if (_from == _fail_from && _fail_packet == _packet)
                        {
                            _fails.push(item);
                        }
                    });
                    if (_fails.length != 0)
                        return _fails;
                    return null;
                },
                put: function (aVal)
                {
                    let self = storage.sniffed_data;
                    self.last_packet = aVal.seq_num;
                    if (aVal.hasOwnProperty("fsend"))
                    {
                        self.dataFails.push(aVal);

                        const _fail_from = aVal.fsend.usdt.rtg_.id.uuid;
                        const _fail_packet = aVal.fsend.usdt.pn;
                        self.data.forEach(function (item)
                        {
                            const _from = item.udata.usdt.rtg_.id.uuid;
                            const _packet = item.udata.usdt.pn;
                            if (_from == _fail_from && _fail_packet == _packet)
                            {
                                self.updated(item);
                            }
                        });
                    } else if (aVal.hasOwnProperty("udata"))
                    {
                        self.data.push(aVal);
                        try
                        {
                            self.on_new(aVal);
                        } catch (e)
                        {
                        }
                    } else
                        console.assert(false, JSON.stringify(aVal, null, 2));
                }, set: function (aVal)
                {
                    let self = storage.sniffed_data;

                    if ($.isArray(aVal))
                    {
                        for (let i = aVal.length; --i >= 0;)
                        {
                            if (aVal.hasOwnProperty(i))
                            {
                                self.put(aVal[i]);
                            }
                        }

                    } else if (aVal)
                    {
                        self.put(aVal);
                    }

                }
                ,
                on_new: function (event)
                {
                }
                ,
                updated: function (event)
                {
                },
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
                        if (data.snif_state.hasOwnProperty("dems"))
                            storage.dems.set(data.snif_state.dems);
                        if (data.snif_state.hasOwnProperty("sniffed_data"))
                            self.set(data.snif_state.sniffed_data);
                    })
                }
                ,
                last_packet: "1",
                data: [],
                dataFails: []
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
                            configurable: false
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
                        configurable: false
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
                                    configurable: false
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
                        configurable: false
                    });
                },
                update: function (aSinc = true)
                {
                    update_udt_net(aSinc);
                },
                get: function (aVal, noRequest)
                {
                    let self = storage.susbscriber_net;
                    if (!self.data.hasOwnProperty("inited"))
                    {
                        self.update(false);
                    }

                    if (!aVal)
                        return self.data;

                    if (!self.data.hasOwnProperty(aVal) && !noRequest)
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
                    let self = storage.roads;
                    let _data = [];
                    if (aData.hasOwnProperty("road"))
                    {
                        if ($.isArray(aData.road))
                            _data = aData.road;
                        else
                            _data.push(aData.road);
                    }
                    let _elements =
                        {
                            nodes: [],
                            edges: []
                        };

                    let _exist_edge={};
                    _data.forEach(function (item)
                    {
                        let _node={};
                        _node.data =
                            {
                                id: item.from,
                            };

                        _elements.nodes.push(_node);

                        let _way_list = [];
                        if (item.hasOwnProperty("way"))
                        {
                            if ($.isArray(item.way))
                                _way_list = item.way;
                            else
                                _way_list.push(item.way);
                        }
                        _way_list.forEach(function (iw)
                        {
                            if(!_exist_edge.hasOwnProperty(item.from) || !_exist_edge[item.from].hasOwnProperty(iw.to))
                            {
                                if(!_exist_edge.hasOwnProperty(item.from))
                                    _exist_edge[item.from]={};
                                if(!_exist_edge[item.from].hasOwnProperty(iw.to))
                                    _exist_edge[item.from][iw.to]=true;

                                if(!_exist_edge.hasOwnProperty(iw.to))
                                    _exist_edge[iw.to]={};
                                if(!_exist_edge[iw.to].hasOwnProperty(item.from))
                                    _exist_edge[iw.to][item.from]=true;

                                let _way = {};
                                _way.data = {
                                    id: item.from + "_to_" + iw.to,
                                    source: item.from,
                                    target: iw.to,
                                    weight: iw.lat
                                }
                                _elements.edges.push(_way);
                            }
                        })
                    });
                    self.updated(_elements);
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
                    //todo
                },
                removed: function (event)
                {
                    //todo
                },
                data: []
            },
            route: {
                set: function (aData)
                {
                    let self = storage.route;
                    let _data = [];
                    if (aData.hasOwnProperty("msg"))
                    {
                        if ($.isArray(aData.msg))
                            _data = aData.msg;
                        else
                            _data.push(aData.msg);

                        _data.forEach(function (item)
                        {
                            if(item.hasOwnProperty("to"))
                            {
                                if(!$.isArray(item.to))
                                {
                                    let _t=[item.to];
                                    item.to=_t;
                                }
                                item.to.forEach(function (val)//parsing bool
                                {
                                    if(val.hasOwnProperty("is_real"))
                                        val.is_real=JSON.parse(val.is_real);
                                    if(val.hasOwnProperty("is_registrator"))
                                        val.is_registrator=JSON.parse(val.is_registrator);
                                })
                            }else
                                item.to=[];

                        })
                    }
                    self.data=_data;
                    self.updated(_data);
                },
                update: function (aSinc = true)
                {
                    let self = storage.route;
                    $.ajax(
                        {
                            type: "GET",
                            data: {query: "route"},
                            dataType: "json",
                            async: aSinc,
                            cache: true
                        }).done(function (data)
                    {
                        self.set(data.state.route.demands);
                    }).error(function (jqXHR, textStatus)
                    {
                        alert("Could not load demands as " + textStatus);
                    });
                }, get: function (aVal)
                {
                    let self = storage.route;
                    if (!self.data.hasOwnProperty("inited"))
                    {
                        self.update(false);
                    }

                    if (!aVal)
                        return self.data;

                    let _rval=[];
                    self.data.forEach(function (item)
                    {
                        if(item.uuid==aVal)
                            _rval.push(item);
                    })
                    return _rval;
                }, getTo: function (aVal)
                {
                    let self = storage.route;
                    if (!self.data.hasOwnProperty("inited"))
                    {
                        self.update(false);
                    }


                    let _rval = [];
                    let _hash={};
                    self.data.forEach(function (item)
                    {
                        // if (item.uuid == aVal)
                        //     _rval.push(item);
                        item.to.forEach(function (val)
                        {
                            if (val.uuid == aVal)
                            {
                                let _tmp={};
                                $.extend(_tmp,item);
                                _tmp.uuid=undefined;
                                _tmp.to=undefined;

                                let _id=0;
                                let _string=JSON.stringify(_tmp);

                                if(_hash.hasOwnProperty(_string))
                                {
                                    _id=_hash[_string];
                                }else
                                {
                                    _id=_rval.length;
                                    _hash[_string]=_id;
                                    _tmp.uuid=aVal;
                                    _tmp.to=[];
                                    _rval.push(_tmp);
                                }
                                let _from={};
                                $.extend(_from,val);
                                _from.uuid=item.uuid;
                                _rval[_id].to.push(_from);
                            }
                        })
                    })
                    return _rval;
                },
                updated: function (event)
                {
                },
                on_new: function (event)
                {
                    //todo
                },
                removed: function (event)
                {
                    //todo
                },
                data: []
            }
        }
    ;
