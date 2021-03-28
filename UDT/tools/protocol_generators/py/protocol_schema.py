import json
import math
import copy
import json
import ast
import datetime
import random
from bisect import insort

g_header_logical_types = ['number', 'unique', 'size', 'version_minor',
                          'version_major', 'crc']
g_header_logical_crc = ['crc']
g_none_type = 'none'
g_enumerated_logical_types_ = ['number', 'unique', 'size', 'version_minor',
                               'version_major', 'enum']
g_dynamic_types = ['dynamic array']
g_real_types = ['float', 'double']
g_test_key = "test"
g_order_max = 100000000000000
type_info = None


def is_enumerated_logical_type(aLogacalType, aType=None):
    if aLogacalType in g_enumerated_logical_types_:
        if aLogacalType == 'unique':
            return 'enumSource' in aType
        else:
            return True
    return False


def get_all_complex_types(aProtocol):
    _rval = [aProtocol['header']]

    for msg in aProtocol['messages']:
        _rval.append(msg)

    for _key in aProtocol['types']:
        _type = aProtocol['types'][_key]
        if not _type['standard'] and '___fields' in _type:
            _rval.append(_type)

    return _rval


def get_parent(aMsgs, aMsg, aIsAll=False):
    _rval = []
    if '___parent' in aMsg and len(aMsg['___parent']) != 0:
        if isinstance(aMsg['___parent'], str) or isinstance(aMsg['___parent'], int):
            for msg in aMsgs:
                if int(aMsg['___parent']) == int(msg['___number']):
                    _rval.append(msg)
                    if aIsAll and len(msg['___parent']) != 0:
                        _rval.extend(get_parent(aMsgs, msg, aIsAll))
                    break
        else:
            _rval = aMsg['___parent']
    if aIsAll:
        return _rval
    else:
        return _rval[0]


def create_types(aTo, aFrom):
    type_info_file = open('schemas/numeric_limits.json', 'r', encoding='utf-8')
    type_info_json = type_info_file.read()
    type_info_file.close()

    global type_info
    type_info = json.loads(type_info_json)

    if 'types' not in aTo:
        aTo['types'] = dict()

    for _type in type_info:
        aTo['types'][_type] = type_info[_type]
        aTo['types'][_type]["standard"] = True

    def replace_type_to_impl(aFields, aNewType, aFromType):
        for field in aFields:
            if field['type'] == aFromType:
                field['type'] = aNewType
            elif 'TypeOfLogicalType' in field and field['TypeOfLogicalType'] == aFromType:
                field['TypeOfLogicalType'] = aNewType

    def replace_type_to(aNewType, aFromType):

        replace_type_to_impl(aTo['header']['___fields'], aNewType, aFromType)

        for msg in aTo['messages']:
            replace_type_to_impl(msg['___fields'], aNewType, aFromType)
        if 'user_types' in aTo:
            for msg in aTo['user_types']:
                replace_type_to_impl(msg['___fields'], aNewType, aFromType)

    if 'user_types' in aTo:
        for _type in aTo['user_types']:
            _new_name = _type['___name'] + '_t'

            _fields, _offset, _msg_align = alignment_fields(_type)
            _type['___fields'] = bit_field(_fields, False)
            _type['size'] = _offset // 8
            _type['align'] = _msg_align // 8
            _type['standard'] = False

            aTo['types'][_new_name] = _type
            type_info[_new_name] = _type

            replace_type_to(_new_name, _type['___name'])

        del aTo['user_types']

    if 'user_enums' in aTo:
        for enums in aTo['user_enums']:

            _new_name = enums['___name'] + '_t'
            aTo['types'][_new_name] = copy.deepcopy(type_info[enums['type']])

            type_info[_new_name] = aTo['types'][_new_name]

            _to = aTo['types'][_new_name]
            _to['standard'] = False

            for _field in enums:
                if _field == '___numbers':
                    _to['enum'] = []
                    _to['enumSource'] = []
                    for _val in enums[_field]:
                        if _val['number'] not in _to['enum']:
                            _to['enum'].append(_val['number'])
                        _to['enumSource'].append(_val)
                else:
                    _to[_field] = enums[_field]

            replace_type_to(_new_name, enums['___name'])

        del aTo['user_enums']

    for _type in aTo['types']:
        if not aTo['types'][_type]['standard'] and '___fields' in aTo['types'][_type]:
            for _field in all_field(aTo['types'][_type]['___fields']):
                handle_limits(_field, aTo)

    return type_info


def fill_child_info(aTo):
    _header_out = aTo['header']
    for i, msg in enumerate(aTo['messages']):

        _is_dynamic = '___type' in msg and msg['___type'] != 'static'
        if len(msg['___parent']) > 0:
            _first = msg['___parent'][0]
            if '___direct_child' not in _first:
                _first['___direct_child'] = []
            insort(_first['___direct_child'], i)

            for parent in msg['___parent']:
                if '___child' not in parent:
                    parent['___child'] = []
                insort(parent['___child'], i)
                if _is_dynamic:
                    parent['___has_dynamic_child'] = True


def get_header_field_info(aHeader, aInfo, aIsAll=False):
    _rval = []
    for field in all_field(aHeader['___fields']):
        if 'LogicalType' in field and field['LogicalType'] in aInfo:
            _rval.append(field)
            if not aIsAll:
                break
    if len(_rval) == 0:
        return ''
    elif aIsAll:
        return _rval
    else:
        return _rval[0]


def append_header_info(_header_out):
    _header_out['___LogicalType'] = dict()
    for _val in g_header_logical_types:
        _field = get_header_field_info(_header_out, _val, True)
        if _field:
            if _val not in _header_out['___LogicalType']:
                _header_out['___LogicalType'][_val] = []
            if type(_field) == list:
                _header_out['___LogicalType'][_val].extend(_field)
            else:
                _header_out['___LogicalType'][_val].append(_field)


def get_align_info(aWhat):
    return int(aWhat['___constants']['align']) * 8


def correct_fields(aTo):
    def correct(aWhat):
        _fields, _offset, _msg_align = alignment_fields(aWhat)
        aWhat['___fields'] = _fields
        aWhat['___constants']['size'] = _offset // 8
        aWhat['___constants']['align'] = _msg_align // 8

    _msgs_out = aTo['messages']
    correct(aTo['header'])

    for msg_out in _msgs_out:
        correct(msg_out)


def append_constant(aTo):
    _msgs_out = aTo['messages']

    _header_out = aTo['header']
    append_constant_to(_header_out, aTo)

    for msg_out in _msgs_out:
        append_constant_to(msg_out, aTo)


def append_full_size(aTo):
    _msgs_out = aTo['messages']
    _header_out = aTo['header']
    _header_out['___constants']['size_full'] = _header_out['___constants']['size']
    _header_out['___constants']['offset'] = 0
    for msg_out in _msgs_out:
        _size = 0

        for _parent in msg_out['___parent']:
            if 'size_full' in _parent['___constants']:
                _size += int(_parent['___constants']['size_full'])
                break
            else:
                _size += int(_parent['___constants']['size'])

        msg_out['___constants']['offset'] = _size
        _size += int(msg_out['___constants']['size'])
        msg_out['___constants']['size_full'] = _size


def append_constant_to(msg_out, aProtocol):
    _constants = dict()
    _constants['DynamicOrder'] = dict()
    _constants['byprotocol'] = dict()
    if '___number' in msg_out:
        _constants['number'] = msg_out['___number']

    _protocol_params = []
    for params in msg_out:
        if '___' not in params and params not in g_header_logical_crc:
            _constants['byprotocol'][params] = dict()
            if type(msg_out[params]) == str:

                values = msg_out[params].split(", ")

                _is_enum = True
                if len(values) == 1:
                    try:
                        _constants['byprotocol'][params]['value'] = int(values[0])
                        _is_enum = False
                    except ValueError:
                        pass
                if _is_enum and len(values) > 0:
                    _enum_type = None

                    for _key in aProtocol["types"]:
                        _type = aProtocol["types"][_key]
                        if 'enumSource' in _type:
                            for _enum in _type["enumSource"]:
                                if _enum['id'] in values:
                                    _enum_type = _type
                                    break
                        if _enum_type is not None:
                            break

                    if _enum_type is not None:
                        _enum_values = []

                        for _enum in _enum_type["enumSource"]:
                            if _enum['id'] in values:
                                values.remove(_enum['id'])
                                _enum_values.append(_enum)

                            if len(values) == 0:
                                break
                        _int_values = [int(x["number"]) for x in _enum_values]
                        _constants['byprotocol'][params]['value'] = _int_values
                        _constants['byprotocol'][params]['enumSource'] = _enum_values
            else:
                _constants['byprotocol'][params]['value'] = msg_out[params]
            _protocol_params.append(params)

    for _rem in _protocol_params:
        del msg_out[_rem]

    _dynamic_fields = []
    for i, field in enumerate(all_field(msg_out['___fields'])):
        if len(_protocol_params) and field['id'] in _protocol_params:
            _constants['byprotocol'][field['id']]['field_index'] = i
            _constants['byprotocol'][field['id']]['LogicalType'] = field['LogicalType']
            _protocol_params.remove(field['id'])

        handle_limits(field, aProtocol)

        if is_dynamic_type(field):
            if 'DynamicOrder' not in field:
                field['DynamicOrder'] = g_order_max

            _constants['DynamicOrder'][field['id']] = field['DynamicOrder']

            field['LogicalTypeInfo'] = aProtocol["types"][field['TypeOfLogicalType']]
            _dynamic_fields.append(field)
        elif field['LogicalType'] in g_header_logical_crc:
            field['TypeOfLogicalType'] = field["type"]
            if 'LogicalTypeInfo' not in field:
                field['TypeOfLogicalType'] = aProtocol["types"][field['TypeOfLogicalType']]

            field['TypeOfLogicalType'] = copy.deepcopy(field['TypeOfLogicalType'])
            field['TypeOfLogicalType']['polynom'] = int(field['sampling'])
            if 'limitation' in field and 'eq' in field['limitation']:
                field['TypeOfLogicalType']['crc_offset'] = int(field['limitation']['eq'])
            else:
                field['TypeOfLogicalType']['crc_offset'] = int(0)

    if len(_protocol_params):
        # todo from end
        for j, _parent in enumerate(msg_out['___parent']):

            for i, field in enumerate(all_field(_parent['___fields'])):
                if field['id'] in _protocol_params:
                    _constants['byprotocol'][field['id']]['LogicalType'] = field['LogicalType']
                    _constants['byprotocol'][field['id']]['field_index'] = i
                    _constants['byprotocol'][field['id']]['parent_index'] = j
                    _protocol_params.remove(field['id'])

                if not len(_protocol_params):
                    break

            if not len(_protocol_params):
                break

    msg_out['___constants'] = _constants
    if len(_dynamic_fields) > 0:
        _dynamic_fields.sort(key=lambda val: int(val['DynamicOrder']))
        msg_out['___dynamic_fields'] = _dynamic_fields


def handle_limits(field, aProtocol):
    if 'LogicalType' not in field:
        field['LogicalType'] = g_none_type
    field['TypeInfo'] = aProtocol["types"][field['type']]
    if 'limitation' in field and len(field['limitation']) > 0:
        _operators = ['>', '>=', '!=', '<', '<=', '==', '=', '<>']
        _operators_str = ['g', 'ge', 'no', 'l', 'le', 'eq', 'eq', 'no']
        import re
        pattern = re.compile(r",\s*([^>=<!]+[>=<!]+)")

        _data = pattern.split(field['limitation'])  # is list of operators and values
        if len(_data) > 0:  # cannot split by regex first operator (I am so sorry!)
            _data = re.split("([>=<!]+)", _data[0]) + _data[1:]

        _data = list(filter(lambda x: len(x) > 0, _data))

        if len(_data) > 0:
            field['limitation'] = dict()
            i = 0
            while i < len(_data):
                _val = _data[i]

                # find operator
                _op_str = None
                for j, op in enumerate(_operators):
                    if op == _val:
                        _op_str = _operators_str[j]
                        break

                if _op_str is not None and i + 1 < len(_data):
                    try:  # integer value
                        _val = ast.literal_eval(_data[i + 1].strip())
                        if type(_val) is dict:
                            _to = dict()
                            _obj_array = []
                            _obj_array.append({
                                'obj': _val,
                                'prefix': ''
                            })

                            while len(_obj_array) > 0:
                                _obj = _obj_array.pop(0)
                                _next = _obj['obj']
                                _prefix = _obj['prefix']
                                for key in _next.keys():
                                    if type(_next[key]) is dict:
                                        _obj_array.append({
                                            'obj': _next[key],
                                            'prefix': _prefix + key + '.'
                                        })
                                    else:
                                        _to[_prefix + key] = _next[key]

                            field['limitation'][_op_str] = _to
                        else:
                            field['limitation'][_op_str] = _val

                    except Exception:
                        pass
                        # try to as JSON
                        # try:
                        #     _js = "\"" + _data[i + 1] + "\""
                        #     _dict = json.loads(_js)
                        #     _list = zip(_dict)
                        #
                        #     field['limitation'][_op_str] = _js
                        # except Exception:
                        #     pass

                i += 2

            if len(field['limitation']) == 0:
                del field['limitation']
        else:
            del field['limitation']
    elif 'limitation' in field:
        del field['limitation']


def get_constant_path_of(aLogicalType, aFieldId):
    if aLogicalType == 'number':
        return aLogicalType
    elif aLogicalType == 'size':
        return 'size_full'
    elif aLogicalType == 'align':
        return 'align'
    elif aLogicalType == 'DynamicOrder':
        return 'DynamicOrder'
    else:
        return 'byprotocol.' + aFieldId + '.value'


def get_key_by_path(aFrom, aPath):
    curr = aFrom
    path = aPath.split('.')[:]
    while (len(path)):
        key = path.pop(0)
        curr = curr.get(key)
        if type(curr) is not dict and len(path):
            return None
    return curr


def get_path_list(aObject):
    _rval = []
    _is_array = type(aObject) == list
    _is_dict = type(aObject) == dict

    for i, _k in enumerate(aObject):
        _value = aObject[_k] if _is_dict else _k
        _key = "[" + str(i) + "]" if _is_array else "." + str(_k)

        if type(_value) == list or type(_value) == dict:
            _data = get_path_list(_value)
            for _adata in _data:  # array
                _rval.append({"key": _key + _adata["key"],
                              "value": _adata["value"]
                              })
        else:
            _rval.append(
                {"key": _key,
                 "value": _value
                 })
    return _rval


def fill_recursive_value(aName, aTo, aId, aValueName, aProtocol):
    append_to = None

    if aName == aTo['___name']:
        append_to = all_field(aTo['___fields'])[aId]
    else:
        for _parent in aTo['___parent']:
            if aName == _parent['___name']:
                append_to = all_field(_parent['___fields'])[aId]
                break
    _own_value = get_key_by_path(aTo['___constants'], aValueName)

    if _own_value is not None:
        if 'LogicalTypeInfo' not in append_to:
            append_to['LogicalTypeInfo'] = dict()

        if 'enum' not in append_to['LogicalTypeInfo']:
            append_to['LogicalTypeInfo']['enum'] = []
        if type(_own_value) == list:
            append_to['LogicalTypeInfo']['enum'].extend(_own_value)
        else:
            append_to['LogicalTypeInfo']['enum'].append(_own_value)

    if '___child' in aTo:
        for index in aTo['___child']:
            msg = aProtocol['messages'][index]
            _msg_value = get_key_by_path(msg['___constants'], aValueName)
            if _msg_value is not None:
                if 'LogicalTypeInfo' not in append_to:
                    append_to['LogicalTypeInfo'] = dict()

                if 'enum' not in append_to['LogicalTypeInfo']:
                    append_to['LogicalTypeInfo']['enum'] = []

                _list = _msg_value;
                if type(_msg_value) != list:
                    _list = [_msg_value]
                for val in _list:
                    if val not in append_to['LogicalTypeInfo']['enum']:
                        insort(append_to['LogicalTypeInfo']['enum'], val)

    if '___direct_child' in aTo:
        for index in aTo['___direct_child']:
            msg = aProtocol['messages'][index]
            fill_recursive_value(aName, msg, aId, aValueName, aProtocol)


def fill_enumerate_vlues(msg_out, aProtocol):
    for i, field in enumerate(all_field(msg_out['___fields'])):

        if 'LogicalType' in field and \
                is_enumerated_logical_type(field['LogicalType'], field['TypeInfo']):
            if field['LogicalType'] == 'enum':
                field['LogicalTypeInfo'] = aProtocol["types"][field['TypeOfLogicalType']]
            else:  # header ?
                _search_name = get_constant_path_of(field['LogicalType'], field['id'])
                if 'LogicalTypeInfo' not in field:
                    field['LogicalTypeInfo'] = dict()
                field['LogicalTypeInfo']['enum'] = []

                # _own_value = None
                # if _search_name in msg_out['___constants']:
                #     _own_value = msg_out['___constants'][_search_name]
                #     field['LogicalTypeInfo']['enum'].append(_own_value)
                #
                # if '___direct_child' in msg_out:
                fill_recursive_value(msg_out['___name'], msg_out, i, _search_name, aProtocol)


def fill_parent_info(aTo, aFrom):
    _header_out = aTo['header']
    _msgs = aFrom['messages']
    _msgs_out = aTo['messages']
    _sorted = []
    for (msg, msg_out) in zip(_msgs, _msgs_out):

        parents = get_parent(_msgs_out, msg_out, True)
        if len(parents) == 0 or parents[-1]['___name'] != _header_out['___name']:
            parents.append(_header_out)

        index = -1
        p_index = -1
        for _parent in parents:
            try:
                p_index = _sorted.index(_parent['___name'])
            except Exception:
                if p_index < 0:
                    _sorted.append(_parent['___name'])
                else:
                    _sorted.insert(p_index - 1, _parent['___name'])
                p_index = _sorted.index(_parent['___name'])
            index = max(p_index, index)

        if '___type' in msg_out:
            _new_parent_type = None
            for _parent in reversed(parents):
                if '___type' not in _parent:
                    _parent['___type'] = 'static'

                if _parent['___type'] != msg_out['___type']:

                    if _parent['___type'] == 'dynamic':
                        msg_out['___type'] = 'dynamic'
                        _new_parent_type = 'dynamic'
                    elif _new_parent_type != None:
                        _parent['___type'] = _new_parent_type
        else:
            msg_out['___type'] = 'static'

        msg_out['___parent'] = parents
        try:
            index = _sorted.index(msg_out['___name'])
        except:
            if index <= 0:
                _sorted.append(msg_out['___name'])
            else:
                _sorted.insert(index + 1, msg_out['___name'])
    _rmsgs = []
    for _id in _sorted:
        if _id != _header_out['___name']:
            for msg in _msgs_out:
                if _id == msg['___name']:
                    _rmsgs.append(msg)
                    break
    aTo['messages'] = _rmsgs
    return _msgs_out


def alignment_fields(aMsg, aIgnore=None):
    fields = all_field(aMsg['___fields'])
    _msg_align = 4 * 8
    if '___parent' in aMsg and isinstance(aMsg['___parent'], list):
        for _parent in aMsg['___parent']:
            if 'align' in _parent['___constants']:
                _msg_align = max(_msg_align, int(_parent['___constants']['align']) * 8)

    def lcm(x, y):
        return x * y // math.gcd(x, y)

    def append_align_to(_align_value, aType, aTo, aOffset):
        _type_prev_align = int(type_info[aType]['align']) * 8

        while _align_value > 0:
            _new_field = dict()
            _new_field['type'] = aType
            _new_field['TypeInfo'] = type_info[aType]
            _new_field['id'] = ''
            _new_field['offset'] = aOffset
            _bits = _align_value % _type_prev_align
            if _bits == 0:
                _bits = _type_prev_align
            _new_field['bits'] = _bits
            _align_value -= _bits
            aOffset += _bits
            aTo.append(_new_field)

    def align_if_need(aOffset, aField, aPrev, aTo):
        _type_align = int(type_info[aField['type']]['align']) * 8

        nonlocal _msg_align
        _msg_align = lcm(_msg_align, _type_align)

        _type_offset = (aOffset + _type_align - 1) // _type_align * _type_align
        if _type_offset > aOffset:
            _align_value = _type_offset - aOffset
            append_align_to(_align_value, aPrev['type'], aTo, aOffset)
            aOffset += _align_value

        elif _type_offset < aOffset:
            raise ValueError("Wtf!")

        return aOffset

    _rval = []
    _bit_info = False
    _prev_type_info = None
    _offset = 0  # in bits

    for field in fields:
        if aIgnore and 'LogicalType' in field and field['LogicalType'] in aIgnore:
            continue
        if 'bits' in field and int(field['bits']) > 0:

            if _bit_info and field['type'] == _prev_type_info['type']:
                _rval.append(field)
                field['offset'] = _offset
                _offset += int(field['bits'])
            else:
                if _bit_info:
                    _offset = align_if_need(_offset, _prev_type_info, _prev_type_info, _rval)  # own align
                    _bit_info = False

                if _prev_type_info:
                    _offset = align_if_need(_offset, field, _prev_type_info, _rval)

                _rval.append(field)

                field['offset'] = _offset
                _offset += int(field['bits'])

                _bit_info = True
                _prev_type_info = field

        else:
            if _bit_info:
                _offset = align_if_need(_offset, _prev_type_info, _prev_type_info, _rval)  # own align
                _bit_info = False

            if _prev_type_info:
                _offset = align_if_need(_offset, field, _prev_type_info, _rval)

            _prev_type_info = field
            _rval.append(field)

            field['offset'] = _offset
            if is_array_type(field):
                _offset += int(field['array_len']) * int(type_info[field['type']]['size']) * 8
            else:
                _offset += int(type_info[field['type']]['size']) * 8

    if _bit_info:
        _offset = align_if_need(_offset, _prev_type_info, _prev_type_info, _rval)  # own align
        _bit_info = False

    def align_msg(_offset, aType):
        nonlocal _msg_align
        nonlocal _rval
        _type_offset = (_offset + _msg_align - 1) // _msg_align * _msg_align

        if _type_offset > _offset:
            _align_value = _type_offset - _offset
            append_align_to(_align_value, aType, _rval, _offset)
            _offset += _align_value

        return _offset

    if 'is_integer' in type_info[_prev_type_info['type']] and \
            type_info[_prev_type_info['type']]['is_integer'] == 'true':
        _offset = align_msg(_offset, _prev_type_info['type'])
    else:
        _offset = align_msg(_offset, 'uint32_t')

    if '___size' in aMsg and int(aMsg['___size']) * 8 > _offset:
        _size = int(aMsg['___size']) * 8
        append_align_to(_size - _offset, 'uint8_t', _rval, _offset)
        _offset = _size
        _offset = align_msg(_offset, 'uint8_t')

    return _rval, _offset, _msg_align


def create_version_info(aProtocol):
    _versions = aProtocol["versions"]
    if len(_versions) > 0:
        _max_major = int(_versions[0]["major"])
        _max_minor = int(_versions[0]["minor"])
        _latest_ver = _versions[0]
        for _version in _versions:
            if aProtocol['settings']['language'] == 'ru':
                _date = _version["date"]
                _date_struct = datetime.datetime.fromisoformat(_date)
                _version["date"] = _date_struct.strftime("%d.%m.%Y") + " г."

            if int(_version["major"]) >= int(_latest_ver["major"]) and int(_version["minor"]) > int(
                    _latest_ver["minor"]):
                _latest_ver = _version

        aProtocol["version"] = _latest_ver
    else:
        aProtocol["version"] = dict()
        aProtocol["version"]["major"] = 0
        aProtocol["version"]["minor"] = 0
        aProtocol["version"]["revision"] = 0
        aProtocol["version"]["changes"] = ""
        aProtocol["version"]["date"] = ""


def get_value_type(aField):
    return aField["LogicalTypeInfo"] if is_dynamic_type(aField) else \
        aField["TypeInfo"]


def is_complex_type(field):
    return ('TypeInfo' in field
            and not field["TypeInfo"]["standard"]
            and '___fields' in field["TypeInfo"]) \
           or ('LogicalTypeInfo' in field and "standard" in field["LogicalTypeInfo"]
               and not field["LogicalTypeInfo"]["standard"]
               and '___fields' in field["LogicalTypeInfo"])


def generate_test_for(aMsgInfo):
    _const_value = dict()
    if '___constants' in aMsgInfo:
        for _const in aMsgInfo['___constants']["byprotocol"]:
            _val = aMsgInfo['___constants']["byprotocol"][_const]
            if "value" in _val:

                if isinstance(_val["value"], list):
                    _test = random.choice(_val["value"])
                else:
                    _test = _val["value"]

                _const_value[_const] = _test

    def gen_random(aMin, aMax, aEnum=None, aNotIn=None):
        _is_limit_valid = (aEnum is None or aMax > min(aEnum) and max(aEnum) > aMin) \
                          and (aNotIn is None or (aMax > max(aNotIn) + 1 or aMin < min(aNotIn)))
        _gval = random.randint(int(aMin), int(aMax)) if not aEnum else random.choice(aEnum)
        while _is_limit_valid and \
                _gval < aMin or _gval > aMax \
                or (aEnum is not None and _gval not in aEnum) \
                or (aNotIn is not None and _gval in aNotIn):
            _gval = random.randint(int(aMin), int(aMax)) if not aEnum else random.choice(aEnum)

        return _gval

    # only for data
    def gen_random_value_of(aField):

        _is_dynamic_array = is_dynamic_type(aField)
        _is_limit = 'limitation' in aField

        _type_info = get_value_type(aField)

        _min_type = int(max([float(_type_info["min"]), -2147483647]))
        _max_type = int(min([float(_type_info["max"]), 2147483647]))
        if 'bits' in aField and int(aField['bits']) > 0:
            _max_type = min([2 ** int(aField["bits"]), _max_type])
            _min_type = 0

        _enum = _type_info["enum"] if 'enum' in _type_info else None

        if _enum is None and 'LogicalTypeInfo' in aField \
                and 'enum' in aField["LogicalTypeInfo"]:
            _enum = aField["LogicalTypeInfo"]["enum"]

        if _is_limit:
            _limits = aField["limitation"]
            if 'eq' in _limits:
                return _limits["eq"]
            elif 'no' in _limits:
                return gen_random(_min_type, _max_type, _enum, [_limits['no']])
            else:
                _plus_min = 0
                _plus_max = 0
                if 'ge' in _limits:
                    _min = _limits["ge"]
                elif "g" in _limits:
                    _min = _limits["g"]
                    _plus_min = 1
                else:
                    _min = _min_type

                if 'le' in _limits:
                    _max = _limits["le"]
                    _plus_max = 1
                elif "l" in _limits:
                    _max = _limits["l"]
                else:
                    _max = _max_type

                return gen_random(_min + _plus_min, _max + _plus_max, _enum)
        else:
            return gen_random(_min_type, _max_type, _enum)

    def gen_random_field(aField):
        _is_complex = is_complex_type(aField)

        if _is_complex:
            _to = dict()
            gen_random_msg(get_value_type(aField), _to)
            return _to

        if aField["id"] in _const_value:
            return _const_value[aField["id"]]
        else:
            return gen_random_value_of(aField)

    def set_test_value(aField, aTo, aVal):
        if aTo is None:
            aField[g_test_key] = aVal
        else:
            aTo[aField['id']] = aVal

    def gen_random_msg(aMsg, aTo=None):

        for field in all_field(aMsg['___fields'], True):
            _id = field["id"]

            _is_dynamic = is_dynamic_type(field)
            _is_array = is_array_type(field)
            _is_bif_field = is_bit_field(field)

            if _is_dynamic or _is_array:
                _len = int(field["array_len"]) if _is_array \
                    else random.randint(0,
                                        min([1000,
                                             int(field["TypeInfo"]["max"])]))
                _to = [None] * _len
                for i in range(0, _len):
                    _to[i] = gen_random_field(field)

                set_test_value(field, aTo, _to)
            elif _is_bif_field:
                for bf in field["fields"]:
                    _to = gen_random_field(bf)
                    set_test_value(field, aTo, _to)
            else:
                _to = gen_random_field(field)
                set_test_value(field, aTo, _to)

    if '___parent' in aMsgInfo:
        for _parent in aMsgInfo['___parent']:
            gen_random_msg(_parent)

    gen_random_msg(aMsgInfo)


def fix_name_bug(aProtocol):
    # todo fix iterate from all dict
    from xml.sax.saxutils import unescape
    for _author in aProtocol["author"]:
        _author["organization"] = unescape(_author["organization"])


def generate_protocol(aProtocol):
    _rval = copy.deepcopy(aProtocol)

    if '___title' not in _rval['header']:
        if 'settings' in _rval and \
                'language' in _rval['settings'] and \
                _rval['settings']['language'] == 'ru':
            _rval['header']['___title'] = 'Заголовок сообщения'
        else:
            _rval['header']['___title'] = 'Protocol header format'

    _rval['name'] = str.lower(_rval['name'])

    fix_name_bug(_rval)
    create_version_info(_rval)

    create_types(_rval, aProtocol)

    fill_parent_info(_rval, aProtocol)

    fill_child_info(_rval)
    append_constant(_rval)

    correct_fields(_rval)

    append_full_size(_rval)
    append_bitfields_info(_rval)
    append_info(_rval)

    # create copy of parent
    update_references(_rval)

    append_header_info(_rval['header'])

    fill_enumerate_vlues(_rval['header'], _rval)

    for msg in _rval['messages']:
        fill_enumerate_vlues(msg, _rval)
        if msg['___parent'][-1]['___name'] == _rval['header']['___name']:
            append_header_info(msg['___parent'][-1])

    for msg in _rval['messages']:
        update_message_limitation(msg)

    for _type in get_all_complex_types(_rval):
        generate_test_for(_type)

    return _rval


# create limits for message
def update_message_limitation(aMsgInfo):
    def update_limits(aFields):
        for field in all_field(aFields, True):
            _type_info = get_value_type(field)

            if is_complex_type(field) and 'limitation' in field:

                for limit_type in field['limitation']:
                    _limit = field['limitation'][limit_type]

                    for _path_code in _limit:

                        _limit_value = _limit[_path_code]
                        path = _path_code.split('.')[:]

                        curr = _type_info["___fields"]
                        while len(path):
                            key = path.pop(0)

                            _find = None
                            for _val in all_field(curr):
                                if _val["id"] == key:
                                    _find = _val
                                    break
                            curr = _find

                            if is_complex_type(curr) and len(path) != 0:
                                curr = get_value_type(curr)["___fields"]
                            elif not is_complex_type(curr) and len(path) == 0:

                                if 'limitation' not in curr:
                                    curr['limitation'] = dict()

                                if limit_type not in curr['limitation']:
                                    curr['limitation'][limit_type] = dict()
                                    curr['limitation'][limit_type] = _limit_value
                                elif limit_type == "eq":
                                    print("Double limits for " + _path_code + " for " + field["id"] + " [" +
                                          curr['limitation']["eq"]
                                          + ", " + _limit_value
                                          + "] ")
                                elif limit_type in ["g, ge"]:
                                    _old_value = curr['limitation'][limit_type]
                                    if _old_value < _limit_value:
                                        curr['limitation'][limit_type] = _limit_value

                                elif limit_type in ["l, le"]:
                                    _old_value = curr['limitation'][limit_type]
                                    if _old_value > _limit_value:
                                        curr['limitation'][limit_type] = _limit_value

                                if "g" in curr['limitation'] and "ge" in curr['limitation']:
                                    if curr['limitation']["g"] > curr['limitation']["ge"]:
                                        del curr['limitation']["ge"]
                                    else:
                                        del curr['limitation']["g"]

                                if "l" in curr['limitation'] and "le" in curr['limitation']:
                                    if curr['limitation']["le"] >= curr['limitation']["l"]:
                                        del curr['limitation']["le"]
                                    else:
                                        del curr['limitation']["l"]
                            else:
                                print("Error cannot find key:" + _path_code + " for " + field["id"])
                del field['limitation']

    if '___parent' in aMsgInfo:
        for _parent in aMsgInfo['___parent']:
            update_limits(_parent['___fields'])

    update_limits(aMsgInfo['___fields'])


def get_field_limitation(aField):
    _rval = []

    if 'limitation' in aField:
        _rval.append(aField['limitation'])

    if is_complex_type(aField):
        for _field in all_field(get_value_type(aField)["___fields"], True):
            _cmpl = get_field_limitation(_field)
            for _val in _cmpl:
                _val2 = copy.deepcopy(_val)
                if 'id' not in _val2:
                    _val2['id'] = _field["id"]
                else:
                    _val2['id'] = _field["id"] + '.' + _val2['id']
                _rval.append(_val2)
    return _rval


def is_dynamic_type(_field):
    return "LogicalType" in _field \
           and _field["LogicalType"] in g_dynamic_types


def is_array_type(field):
    return 'array_len' in field and int(field['array_len']) > 0


def is_bit_field(field):
    return 'bit_len' in field and int(field['bit_len']) > 0


def append_info(_rval):
    _complex_types = get_all_complex_types(_rval)
    _is_float = False
    _is_dynamic = False

    for _type in _complex_types:
        _fields = all_field(_type['___fields'], True)
        for _field in _fields:
            if ("TypeInfo" in _field
                and "is_integer" in _field["TypeInfo"]
                and str.lower(_field["TypeInfo"]["is_integer"]) == "false") \
                    or ("LogicalTypeInfo" in _field
                        and "is_integer" in _field["LogicalTypeInfo"]
                        and str.lower(_field["LogicalTypeInfo"]["is_integer"]) == "false"):
                _is_float = True

            if is_dynamic_type(_field):
                _is_dynamic = True

            if _is_float and _is_dynamic:
                break

        if _is_float and _is_dynamic:
            break

    if "info" not in _rval:
        _rval["info"] = dict()

    _rval["info"]["has_float"] = _is_float
    _rval["info"]["has_dynamic"] = _is_dynamic


def get_msg_by_id(aMessages, aName):
    for msg in aMessages:
        if msg["___name"] == aName:
            return msg
    return None


def update_references(_rval):
    def update_references_impl(aMsg):
        for _field in all_field(aMsg['___fields'], True):
            if "LogicalTypeInfo" in _field:
                _field["LogicalTypeInfo"] = copy.deepcopy(_field["LogicalTypeInfo"])
            if "TypeInfo" in _field:
                _field["TypeInfo"] = copy.deepcopy(_field["TypeInfo"])

        if '___parent' in aMsg:
            aMsg['___parent'] = copy.deepcopy(aMsg['___parent'])

    for msg in _rval['messages']:
        update_references_impl(msg)

    update_references_impl(_rval['header'])

    for msg in _rval['messages']:
        if '___parent' in msg:
            for _parent in msg['___parent']:
                if '___parent' in _parent:
                    del _parent['___parent']


def append_bitfields_info(aTo):
    _msgs_out = aTo['messages']
    _header_out = aTo['header']
    _header_out['___fields'] = bit_field(_header_out['___fields'],
                                         False,
                                         _header_out['___constants']['size_full'] -
                                         _header_out['___constants']['size']
                                         )
    for msg_out in _msgs_out:
        msg_out['___fields'] = bit_field(msg_out['___fields'], False,
                                         msg_out['___constants']['size_full'] -
                                         msg_out['___constants']['size']
                                         )


def all_field(fields, aIgnoreReserv=False):
    _rval = []
    for field in fields:
        if is_bit_field(field):
            for bf in field['fields']:
                if not aIgnoreReserv or len(bf['id']) > 0:
                    _rval.append(bf)
        else:
            _rval.append(field)

    return _rval


def all_field_msg(aMsg, aIgnoreReserv=False):
    _rval = []
    if len(aMsg['___parent']) > 0:
        for _parent in reversed(aMsg['___parent']):
            _rval.extend(all_field(_parent['___fields'], aIgnoreReserv))

    _rval.extend(all_field(aMsg['___fields'], aIgnoreReserv))
    return _rval


def bit_field(fields, aIsIgnore=True, aBegin=0):
    _rval = []
    _field_info = dict()
    _bif_field_id = ""
    for field in fields:
        if 'bits' in field and int(field['bits']) > 0:

            if len(_field_info) and field['type'] == _field_info['type']:

                _field_info['fields'].append(field)
                _field_info['bit_len'] += int(field['bits'])
                _field_info['id'] = _bif_field_id if field['id'] or _field_info['id'] else ''
            else:
                if len(_field_info):
                    _rval.append(_field_info)
                    _field_info = dict()

                _field_info['type'] = field['type']
                _field_info['offset'] = field['offset']
                _field_info['TypeInfo'] = field['TypeInfo']
                _begin = aBegin + field['offset'] // 8
                _bif_field_id = 'byte_from_' + \
                                str(_begin) + \
                                '_to_' + str(_begin + int(field['TypeInfo']['size']))
                _field_info['id'] = _bif_field_id if field['id'] else ''
                _field_info['fields'] = []
                _field_info['fields'].append(field)
                _field_info['bit_len'] = 0
                _field_info['bit_len'] += int(field['bits'])

            if 'bit_len' in _field_info and _field_info['bit_len'] \
                    == int(type_info[field['type']]['size']) * 8:
                _rval.append(_field_info)
                _field_info = dict()
        else:
            if len(_field_info):
                _rval.append(_field_info)
                _field_info = dict()

            if not aIsIgnore:
                _rval.append(field)

    if len(_field_info):
        _rval.append(_field_info)
        _field_info = dict()
    return _rval


def swap_bitfield_endian(aFields):
    return aFields
