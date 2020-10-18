import copy
from py import protocol_schema as ps

NUM_OF_BIT_FIELDS = 32
MAX_ID_LEN = 8


def generate_latex_schema(aWhat):
    _language = aWhat['settings']['language'] if 'settings' in aWhat and \
                                                 'language' in aWhat['settings'] \
        else 'en'

    def document_fields_of(aMsg, aNext=1):
        # aMsg['___fields'] = ps.bit_field(aMsg['___fields'], False)
        _i_num_bits = 0

        _max_id_len = 1
        _max_type_len = 1

        def refine_id(f, aLen):
            nonlocal aNext
            nonlocal _max_id_len
            nonlocal _max_type_len
            _max_id_len = max(_max_id_len, len(f['id']))
            _max_id_len = min(_max_id_len, MAX_ID_LEN)

            _type_len = len(f['type'])

            if 'array_len' in f and f['array_len'] > 0:
                _type_len += 2

            _max_type_len = max(_max_type_len, _type_len)
            _max_type_len = min(_max_type_len, MAX_ID_LEN)

            if len(f['id']) > 0:
                if len(f['id']) < aLen * 2:
                    f['number'] = f['id']
                else:
                    f['number'] = ('#' if _language != 'ru' else '№') + str(aNext)
                aNext += 1

        for field in aMsg['___fields']:
            if 'array_len' in field and field['array_len'] > 0:
                field['word'] = int(ps.type_info[field['type']]['size']) * field['array_len']

                refine_id(field, int(ps.type_info[field['type']]['size']) * 8)
                _i_num_bits = fuctorial_if_need(field['word'] * 8, _i_num_bits, field)

            elif 'bit_len' not in field:
                field['word'] = int(ps.type_info[field['type']]['size'])

                refine_id(field, field['word'] * 8)
                _i_num_bits = fuctorial_if_need(field['word'] * 8, _i_num_bits, field)
            else:
                for bf in field['fields']:
                    refine_id(bf, bf['bits'])
                    _i_num_bits = fuctorial_if_need(bf['bits'], _i_num_bits, bf)

        if '___latex' not in aMsg:
            aMsg['___latex'] = dict()
        aMsg['___latex']['max_unique_id'] = _max_id_len
        aMsg['___latex']['max_type_id'] = _max_type_len
        return aNext

    def fuctorial_if_need(_bit_len, _i_num_bits, field):
        _is_array = True if 'array_len' in field and field['array_len'] > 0 else False

        _el_size = int(ps.type_info[field['type']]['size']) * 8

        def append_fractional(_bit_len, _end, aSides, aTo):

            if _is_array and _bit_len > _el_size:

                _el_len = _bit_len // _el_size
                _rem = int(_bit_len - _el_len * _el_size)
                while _el_len > 0:
                    field['fractional'].append({
                        'sides': aSides + ('l' if 'l' not in aSides and _el_len != 1 else ''),
                        'bits': _rem if _el_len == 1 and _rem != 0 else _el_size,
                        'end_word': _end if _el_len == 1 else False
                    })
                    _el_len -= 1
            else:
                aTo['fractional'].append({
                    'sides': aSides,
                    'bits': _bit_len,
                    'end_word': _end
                })

        start_bit_len = copy.copy(_bit_len)

        while _bit_len > 0:
            _len = _i_num_bits + _bit_len
            if _len > NUM_OF_BIT_FIELDS:

                if _is_array:
                    _remain = max(NUM_OF_BIT_FIELDS, _el_size) - _i_num_bits
                else:
                    _remain = NUM_OF_BIT_FIELDS - _i_num_bits

                _i_num_bits = 0
                _bit_len -= _remain

                _diff = start_bit_len - _bit_len

                _side = 'l'

                if 'fractional' not in field:
                    field['fractional'] = []
                    _side += 'r'
                elif (_diff + _remain) % NUM_OF_BIT_FIELDS == 0:
                    _side += 'r'

                if _el_size <= NUM_OF_BIT_FIELDS:
                    _side += 'tb'
                else:
                    if _diff % _el_size == 0:
                        _side += 'b'
                    if (_bit_len + _remain) % _el_size == 0:
                        _side += 't'

                append_fractional(_remain, True,
                                  _side,
                                  field)

                if _is_array and _bit_len > NUM_OF_BIT_FIELDS and _diff >= _el_size:

                    field['fractional'].append({
                        'skippedwords': _bit_len // NUM_OF_BIT_FIELDS,
                        'end_word': True
                    })
                    _bit_len = _bit_len % max(_el_size, NUM_OF_BIT_FIELDS)
                    if _bit_len == 0:
                        _bit_len = max(_el_size, NUM_OF_BIT_FIELDS)
            else:
                _i_num_bits += _bit_len

                _end = False
                if _i_num_bits == NUM_OF_BIT_FIELDS:
                    _i_num_bits = 0
                    _end = True

                if 'fractional' in field:
                    append_fractional(_bit_len, _end, 'lr' + ('bt' if _el_size <= NUM_OF_BIT_FIELDS else 'b'), field)
                else:
                    field['end_word'] = _end
                    if not field['id'] and 'sides' not in field:
                        if _end:
                            field['sides'] = 'btr' if _bit_len != NUM_OF_BIT_FIELDS else 'btlr'
                        else:
                            field['sides'] = 'btl' if _i_num_bits - _bit_len == 0 else 'bt'

                _bit_len = 0

        if 'fractional' in field and 'number' in field:
            if _is_array:
                _skip_from = -1
                _full_len = len(field['fractional'])
                for i, fract in enumerate(field['fractional']):
                    if 'skippedwords' in fract:
                        _skip_from = i
                        _full_len -= 1
                    else:
                        fract['number'] = field['number'] + '[' \
                                          + str(i if _skip_from < 0
                                                else field['word'] + (i - _full_len)) \
                                          + ']'
            elif len(field['fractional']) == 2:
                field['fractional'][1]['number'] = field['number']
            elif len(field['fractional']) > 2:
                field['fractional'][-1]['number'] = field['number']
        return _i_num_bits

    _doc_protocol = copy.deepcopy(aWhat)

    _doc_protocol['NUM_OF_BIT_FIELDS'] = NUM_OF_BIT_FIELDS
    document_fields_of(_doc_protocol['header'])

    for msg in _doc_protocol['messages']:
        # _next = 1

        if '___parent' in msg:
            for parent in reversed(msg['___parent']):
                document_fields_of(parent)

        document_fields_of(msg)

    for _key in _doc_protocol['types']:
        _type = _doc_protocol['types'][_key]
        if not _type['standard'] and '___fields' in _type:
            document_fields_of(_type)

    return _doc_protocol

def latex_escape(val):
    return str(val) \
        .replace('\\', '$\\backslash$') \
        .replace('$', '\\$') \
        .replace('#', '\\#') \
        .replace('%', '\\%') \
        .replace('&', '\\&') \
        .replace('^', '\\^') \
        .replace('{', '\\{') \
        .replace('}', '\\}') \
        .replace('~', '\\~') \
        .replace('_', '\\_')

def split_text(aText, aMaxLen, aIsEcape = True):
    _len = len(aText)
    _parts = (_len // aMaxLen) + 1
    _part_len = _len // _parts

    _rval = ""
    for i in range(0, _parts):
        _begin = i * _part_len
        _end = _len if (i + 1) == _parts else (i + 1) * _part_len
        _rval += latex_escape(aText[_begin:_end]) if aIsEcape else aText[_begin:_end]
        if (i + 1) != _parts:
            _rval += "\\\\ \\relax "

    return _rval



