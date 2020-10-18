const g_reserverd_values = ["___number",
    "___size",
    "___parent",
    "___type",
    "___title",
    "crc"
];

const g_has_to_specified_in_msg = ["unique",
    "version_major",
    "version_minor"
];
const dynamic_array_class = 'dynamic_array';
// let is_big_endian = false;
// $(function () {
//     $('#endian').change(function () {
//         if($(this).prop('checked'))
//         {
//             $('table.message_scheme').css('direction','rtl');
//             is_big_endian = false;
//         }
//         else
//         {
//             $('table.message_scheme').css('direction','ltr');
//             is_big_endian = true;
//         }
//     });
// })
function update_msg_tabel (jseditor, aMsgFieldEditor) {
    function lcm(x, y) {
        if (typeof x !== 'number')
                x = parseInt (x);
        if (typeof y !== 'number')
                y = parseInt (y);

        function gcd(x, y) {
            x = Math.abs(x);
            y = Math.abs(y);
            while (y)
            {
                let t = y;
                y = x % y;
                x = t;
            }
            return x;
        }
        return (!x || !y)? 0:Math.abs((x * y)/gcd(x ,y));
    }
    function get_id(path) {
        return path.replaceAll('.', '_');
    }

    const headerFieldsPath = 'root.header.___fields';
    const _tb_class = 'message_scheme';
    let msgEditor = aMsgFieldEditor.parent;
    const messagePath = msgEditor.path;
    const NUM_OF_BIT_FIELDS = 32;
    const messageID = get_id(messagePath);


    function appendElements(_i_num_bits, aNumBits, tr, _tb, uniqueId, aClasses) {
        function set_attr(aFor, aLen) {
            aFor.attr('colspan',
                aLen
            );
            if (uniqueId)
                aFor.attr('fieldID', uniqueId);
            if (aClasses) {
                if (Array.isArray(aClasses))
                    aClasses.forEach(function (element, index, array) {
                        aFor.addClass(element);
                    });
                else
                    aFor.addClass(aClasses);
            }
            return aFor;
        }

        let _bits = aNumBits;
        for (; _bits > 0;) {
            if ((_i_num_bits + _bits) > NUM_OF_BIT_FIELDS) {
                if (_i_num_bits !== NUM_OF_BIT_FIELDS) {
                    set_attr($("<td>"), NUM_OF_BIT_FIELDS - _i_num_bits).appendTo(tr);
                }
                _bits -= NUM_OF_BIT_FIELDS - _i_num_bits;
                tr = $("<tr>").appendTo(_tb);
                _i_num_bits = 0;
            } else {
                _i_num_bits += _bits;

                set_attr($("<td>"), _bits).appendTo(tr);

                _bits = 0;
            }
        }
        return [_i_num_bits, tr];
    }

    function appendDynamicFields(valueElement,/* _msg_num_bits,*/ _tb_dynamic, fieldPath) {
        const typeElement = valueElement['TypeOfLogicalType'];
        const valueId = valueElement['id'];
        const fieldID = get_id(fieldPath);
        const uniqueId = fieldID + '_dynamic';

        function appendRow() {
            /*_msg_num_bits += NUM_OF_BIT_FIELDS;*/
            let tr = $("<tr>").attr('fieldID', uniqueId).addClass(dynamic_array_class).appendTo(_tb_dynamic)
            if ('DynamicOrder' in valueElement)
                tr.attr('DynamicOrder', valueElement['DynamicOrder'])
            tr.on('click', function () {

                let _scroll_to = $('[fieldID=\"' + fieldID + '\"]');
                if (_scroll_to.length > 0) {

                    _scroll_to.get(0).scrollIntoView();
                    _scroll_to.show().fadeOut(250).fadeIn(250).fadeOut(250).fadeIn(250).fadeOut(250).fadeIn(250);
                }
            });
            return tr;
        }

        $("<td>").text(valueId + ": dynamic array of type " + typeElement).attr('colspan',
            NUM_OF_BIT_FIELDS
        ).addClass(dynamic_array_class).appendTo(appendRow());

        // $("<td>").text('').attr('colspan',
        //     NUM_OF_BIT_FIELDS
        // ).addClass(dynamic_array_class).appendTo(appendRow());

        /*return _msg_num_bits;*/
    }

    // function append_empty_fields(_i_raw_bits, _msg_num_bits, tr) {
    //     $("<td>").attr('colspan',
    //         NUM_OF_BIT_FIELDS - _i_raw_bits
    //     ).addClass('empty').appendTo(tr);
    //     _msg_num_bits += NUM_OF_BIT_FIELDS - _i_raw_bits;
    //     _i_raw_bits = 0;
    //     return [_i_raw_bits, _msg_num_bits];
    // }

    function alignment_fields(_i_raw_bits, _msg_num_bits, tr, _tb, aOldType, aNewType) {
        const uniqueId = 'align_'+_msg_num_bits;
        function align_impl(aWhat) {
            if (typeof aWhat !== 'number')
                aWhat = parseInt (aWhat);
            console.assert(typeof aWhat === 'number','Wtf');
            const _new_bit_len = aWhat * 8;
            let _align = _msg_num_bits % _new_bit_len;
            if (_align !== 0) {
                console.assert( _new_bit_len > _align, 'Wtf!');
                _align = _new_bit_len - _align;
                _msg_num_bits += _align;
                [_i_raw_bits, tr] = appendElements(_i_raw_bits,
                    _align,
                    tr, _tb, uniqueId, 'align');
            }
        }

        if( aOldType !== null ) {
            if ( aNewType !== null ) {
                if ( aOldType!== aNewType ) {
                    align_impl(aOldType);
                    align_impl(aNewType);
                }
            }else
            {
                align_impl(aOldType);
            }
        }
        return [_i_raw_bits, _msg_num_bits, tr];
    }

    function append_fields(aFieldEditor, _table, _errors) {
        const msgPath = aFieldEditor.parent.path;
        let msgEditor = jseditor.getEditor(msgPath);

        function on_click(_scroll_to) {
            if (_scroll_to.length > 0) {

                let _tab = $('a[href="#' + msgPath + '"]');
                if (_tab.length > 0)
                    _tab[0].click();

                _scroll_to.get(0).scrollIntoView();
                _scroll_to.show().fadeOut(250).fadeIn(250).fadeOut(250).fadeIn(250).fadeOut(250).fadeIn(250);
            }
        }

        const fieldPath = aFieldEditor.path;
        const fieldID = get_id(fieldPath);

        let mf = aFieldEditor.getValue();//value;
        let _tb = $("<tbody>").appendTo(_table);
        _tb.attr("id", fieldID);
        let _i_raw_bits = 0;
        let _msg_num_bits = 0;
        let tr = $("<tr>").appendTo(_tb);
        let uniqueID_set = new Set();

        let _last_type_align = null;
        let _message_alignment = NUM_OF_BIT_FIELDS / 8;

        function get_field_size_and_align_msg(typeElement,  _i) {
            let _type_size = null
            let _new_type = null

            if (typeElement in _numeric_limits) {
                _new_type = _numeric_limits[typeElement];
            } else {
                let _types = jseditor.getValue().user_types.find(obj => {
                    return obj.___name === typeElement
                });
                if (_types) {
                    _new_type = {
                        'size': _types.___size,
                        'align' : 4 //@todo calculate align
                    }

                } else {
                    let _enum_type = jseditor.getValue().user_enums.find(obj => {
                        return obj.___name === typeElement
                    });
                    if (_enum_type) {
                        _new_type = _numeric_limits[_enum_type.type];
                    }
                }

            }
            if (_new_type !== null) {
                // const _new_type = _numeric_limits[typeElement];

                const _new_type_align = typeof (_new_type['align']) === 'number' ? _new_type['align'] :
                    parseInt(_new_type['align']);

                _type_size = typeof (_new_type['size']) === 'number' ? _new_type['size'] :
                    parseInt(_new_type['size']);

                const _old_msg_num_bits = _msg_num_bits;

                [_i_raw_bits, _msg_num_bits, tr] = alignment_fields(_i_raw_bits,
                    _msg_num_bits, tr, _tb,
                    _last_type_align, _new_type_align);

                _last_type_align = _new_type_align;
                _message_alignment = lcm(_message_alignment, _last_type_align);

                if (_old_msg_num_bits !== _msg_num_bits) {
                    let fieldElement = _tb.find('[fieldID=\"align_' + _old_msg_num_bits + '\"]');

                    fieldElement.on('click', function () {
                        on_click($('[data-schemapath=\"' + fieldPath + '.' + _i + '.type\"]'));
                        if (parseInt(_i) > 0)
                            on_click($('[data-schemapath=\"' + fieldPath + '.' + (parseInt(_i) - 1).toString() + '.type\"]'));
                    });
                }
            } else
                _type_size = null;

            return _type_size;
        }

        if(_numeric_limits !== null)
        for (let _i in mf) {

            if (!mf.hasOwnProperty(_i))
                continue;

            let valueElement = mf[_i];
            const uniqueId = fieldID + '_' + _i;

            let valueId = valueElement['id'];
            if (valueId.length === 0)
                valueId = _i;

            let LogicalType = null;
            if ('LogicalType' in valueElement)
                LogicalType = valueElement['LogicalType'];

            const typeElement = valueElement['type'];

            const _type_size = get_field_size_and_align_msg(typeElement,  _i);

            let _max_type_bits = -1;

            if (_type_size != null) {
                _max_type_bits = _type_size * 8;
            } else {
                _errors.push(`No type info for ${typeElement} of field ${valueId}; `);
                continue;
            }
            let _num_bits = _max_type_bits;
            const _user_bits = typeof (valueElement['bits'])  === 'number'? valueElement['bits']:
                    parseInt(valueElement['bits']) ;
            const _array_len = typeof (valueElement['array_len'] )  === 'number'? valueElement['array_len'] :
                    parseInt(valueElement['array_len']) ;

            if (_user_bits > 0) {
                _num_bits = Math.min(_user_bits, _num_bits);
            }
            _msg_num_bits += _num_bits;
            [_i_raw_bits, tr] = appendElements(_i_raw_bits, _num_bits, tr, _tb, uniqueId, null);

            if (_array_len> 1) {
                for (let j = 1; j < _array_len; ++j) {
                    _msg_num_bits += _num_bits;
                    [_i_raw_bits, tr] = appendElements(_i_raw_bits, _num_bits, tr, _tb, uniqueId, null);
                }
            }

            let fieldElement = _tb.find('[fieldID=\"' + uniqueId + '\"]');

            fieldElement.text(valueId).on('click', function () {
                on_click($('[data-schemapath=\"' + fieldPath + '.' + _i + '\"]'));
            });
            if (LogicalType) {
                fieldElement.attr("LogicalType", LogicalType);
            }

            if (_user_bits > (_type_size * 8)) {
                fieldElement.addClass('error');
                _errors.push(`Amount of bits (${_user_bits}) is greater then ${_max_type_bits}. See field ${valueId}; `);
            }
            if (_array_len > 0) {
                fieldElement.addClass('array');
            }
            if (_user_bits > 0 && _array_len> 0) {
                fieldElement.addClass('error');
                _errors.push(`Using bit fields with array no allowed. See field ${valueId}; `);
            }
            if (uniqueID_set.has(valueId)) {
                fieldElement.addClass('error');
                _errors.push(`Field ID ${valueId} isn't unique; `);
            } else
                uniqueID_set.add(valueId);

        }

        if ('___size' in msgEditor.getValue()) {

            const sizeEditor = jseditor.getEditor(msgPath + '.' + '___size');
            const _user_size = 8 * (typeof (sizeEditor.getValue())  === 'number'? sizeEditor.getValue() :
                    parseInt(sizeEditor.getValue()))
            if (_user_size> 0 && _msg_num_bits < _user_size ) {

                const _new_type_align = 1;

                [_i_raw_bits, _msg_num_bits, tr] = alignment_fields(_i_raw_bits,
                        _msg_num_bits, tr, _tb,
                    _last_type_align , _new_type_align);

                _last_type_align = _new_type_align;

                const _append_bites = Math.floor((_user_size - _msg_num_bits));

                if(_append_bites > 0 ) {

                    const uniqueId = "reserved";

                    _msg_num_bits += _append_bites;

                    [_i_raw_bits, tr] = appendElements(_i_raw_bits,
                        _append_bites,
                        tr, _tb, uniqueId, 'reserved');

                    let fieldElement = _tb.find('[fieldID=\"' + uniqueId + '\"]');

                    fieldElement.on('click', function () {
                        on_click($('[data-schemapath=\"' + msgPath + '.___size' + '\"]'));
                    });
                }
            }
        }
        [_i_raw_bits, _msg_num_bits, tr] = alignment_fields(_i_raw_bits,
                        _msg_num_bits, tr, _tb, _last_type_align,
                        _message_alignment);

        if ('___type' in msgEditor.getValue()) {
            const typeEditor = jseditor.getEditor(msgPath + '.' + '___type');

            if (typeEditor.getValue() === "dynamic") {

                let _tb_dynamic = $("tbody." + dynamic_array_class, _table);
                if (_tb_dynamic.length === 0) {
                    _tb_dynamic = $("<tbody>").addClass(dynamic_array_class).appendTo(_table);
                }
                for (let _i in mf) {
                    if (!mf.hasOwnProperty(_i))
                        continue;
                    let valueElement = mf[_i];
                    if ('LogicalType' in valueElement
                        && 'TypeOfLogicalType' in valueElement
                        && valueElement['LogicalType'] === 'dynamic array'
                    ) {
                        const valPath = fieldPath + '.' + _i;
                        /*_msg_num_bits =*/ appendDynamicFields(valueElement, /*_msg_num_bits,*/ _tb_dynamic, valPath);
                    }
                }
                $("tr." + dynamic_array_class, _tb_dynamic).sort(function (left, right) {
                    const valLeft = $(left).attr('DynamicOrder');
                    const valRight = $(right).attr('DynamicOrder');
                    if (!valLeft || !valRight)
                        return false;
                    return parseInt(valLeft) > parseInt(valRight);
                }).appendTo(_tb_dynamic);
            }

        }
        return _msg_num_bits;
    }

    function create_fields_of_msg(_table, aFieldEditor, _errors) {

        const msgPath = aFieldEditor.parent.path;
        let msgEditor = jseditor.getEditor(msgPath);
        let parentEditor = null;
        let numberEditor = null;
        if ('___parent' in msgEditor.getValue())
            parentEditor = jseditor.getEditor(msgPath + '.' + '___parent');

        if ('___number' in msgEditor.getValue())
            numberEditor = jseditor.getEditor(msgPath + '.' + '___number');

        if (parentEditor
            && parentEditor.getValue()
            && numberEditor
            && parentEditor.getValue().toString() !== numberEditor.getValue().toString()
        ) {
            const _parent = parentEditor.getValue();

            for (let key in jseditor.editors) {
                let matchKey = key.match("\\.___fields$");
                if (matchKey != null /*indexOf(".___fields")!== -1*/
                    && jseditor.editors.hasOwnProperty(key)
                ) {
                    const messageEditor = jseditor.getEditor(key);
                    if ('___number' in messageEditor.parent.getValue()) {
                        let numberEditor2 = jseditor.getEditor(messageEditor.parent.path + '.' + '___number');

                        if (numberEditor2.getValue().toString() === _parent.toString()) {
                            create_fields_of_msg(_table, messageEditor, _errors);
                            break;
                        }
                    }
                }
            }
        }
        return append_fields(aFieldEditor, _table, _errors);
    }

    function addOrReplaceInDOM(_table) {
        const fieldPath = aMsgFieldEditor.path;
        const fieldID = get_id(fieldPath);

        let messageTable = $('#' + messageID);
        if (messageTable.length > 0) {
            console.assert(messageTable.length === 1, "Several table of " + messageID);
            messageTable.replaceWith(_table);
        } else {
            const _own_table = '[data-schemapath=\"' + fieldPath + '\"]';
            let _div_ed = $(_own_table);
            if (_div_ed.length > 0) {
                console.assert(_div_ed.length === 1, "Several path of " + fieldPath);
                _table.prependTo(_div_ed[0]);
            }
        }
        let _field_in_other_msgs = $('table.' + _tb_class + ':not(' + '#' + messageID + ') > tbody#' + fieldID);
        if (_field_in_other_msgs.length > 0) {
            let fieldTable = $('[fieldID=\"' + fieldID + '\"]', _table);
            if (fieldTable.length > 0) {
                console.assert(fieldTable.length === 1, "Several path of " + fieldPath);
                _field_in_other_msgs.replaceWith(fieldTable.clone(true, true));
            }
        }
    }

    function createTableOfMsg() {
        let _table = $("<table>");
        _table.addClass(_tb_class);
        const is_big_endian=jseditor.getEditor('root.settings.endian') !== null &&
            jseditor.getEditor('root.settings.endian').getValue().toLowerCase() === 'big';
        if(is_big_endian)
            _table.css('direction','ltr');

        _table.attr('id', messageID);

        let _errors = [];
        {
            let _head = $("<thead>").appendTo(_table);
            let _head_tr = $("<tr>").appendTo(_head);

            for (let i = 0; i < NUM_OF_BIT_FIELDS; ++i) {
                $("<th>").text(i).appendTo(_head_tr);
            }
        }
        {
            if ('___number' in msgEditor.getValue()) {
                let headerFieldsEditor = jseditor.getEditor(headerFieldsPath);
                append_fields(headerFieldsEditor, _table, _errors)
                const headerId = get_id(headerFieldsPath);

                let headerTable = $('#' + headerId, _table);
                console.assert(headerTable.length === 1, "No message's header");

                if (headerTable.length > 0) {
                    $('td', headerTable).each(function (index, td) {
                        if (td.innerText in msgEditor.getValue()) {
                            let valEditor = jseditor.getEditor(msgEditor.path + '.' + td.innerText);
                            if (valEditor)
                                td.innerText = valEditor.getValue().toString();
                        }
                    });
                    $('td[isunique=\"number\"]', headerTable).each(function (index, td) {
                        let valEditor = jseditor.getEditor(msgEditor.path + '.___number');
                        td.innerText = valEditor.getValue().toString();
                    });
                }
            }
            const _own_size = create_fields_of_msg(_table, aMsgFieldEditor, _errors);
            {
                const msgPath = aMsgFieldEditor.parent.path;
                let msgEditor = jseditor.getEditor(msgPath);

                if ('___size' in msgEditor.getValue()) {

                    const sizeEditor = jseditor.getEditor(msgPath + '.' + '___size');
                    if (sizeEditor.getValue() !== _own_size / 8)
                        console.assert(_own_size / 8 > sizeEditor.getValue())
                        sizeEditor.setValue(_own_size / 8)
                }
            }
            //Move to end dynamic fields
            let _tb_dynamic = $("tbody." + dynamic_array_class, _table);
            if (_tb_dynamic.length !== 0) {
                _table.append(_tb_dynamic);
            }

        }
        if (_errors.length > 0) {
            let _tfoot = $("<tfoot>").appendTo(_table);
            let _tfoot_tr = $("<tr>").appendTo(_tfoot);
            let _tfoot_td = $("<td>").attr('colspan',
                NUM_OF_BIT_FIELDS
            ).appendTo(_tfoot_tr);
            let _tfoot_label = $("<label>").appendTo(_tfoot_td);
            _errors.forEach(function (value, index, array) {
                _tfoot_label.append(value);
            });
        }
        return _table;
    }

    if (aMsgFieldEditor) {
        let _table = createTableOfMsg();

        addOrReplaceInDOM(_table, _tb_class);
    }
}
var addNewRow = function (editor, jseditor) {

    const _messages_id = 'root.messages';

    let field_path = editor.path + '.___fields';
    for (const _val in editor.getValue()) {
        if (_val === '___description')
            continue;
        const valuePath = editor.path + '.' + _val;
        jseditor.watch(valuePath, function () {

            let msg_editor = jseditor.getEditor(_messages_id);
            if (_val === '___number') {
                const numberEditor = jseditor.getEditor(valuePath);
                const new_number_value = numberEditor.getValue();
                const old_number_value = numberEditor.parent.getValue()['___number'];

                if (old_number_value !== null && old_number_value !== new_number_value) {


                    for (let _i_msg in msg_editor.getValue()) {
                        if (msg_editor.getValue().hasOwnProperty(_i_msg)) {
                            let cut_msg_editor = jseditor.getEditor(_messages_id + "." + _i_msg);

                            if (cut_msg_editor !==null && '___parent' in cut_msg_editor.getValue()
                                && cut_msg_editor.getValue()['___parent'].toString() === old_number_value.toString()
                            ) {
                                let parentEditor = jseditor.getEditor(cut_msg_editor.path + '.' + '___parent');

                                if (parentEditor.enum_values.includes(old_number_value.toString()))
                                    parentEditor.enum_values[parentEditor.enum_values.indexOf(old_number_value.toString())] = new_number_value.toString();

                                parentEditor.setValue(new_number_value);

                            }
                        }
                    }
                }
            }
            if (_val === '___type') {
                const typeEditor = jseditor.getEditor(valuePath);
                const new_type_value = typeEditor.getValue();
                if (new_type_value === 'dynamic') {
                    const number_value = jseditor.getEditor(valuePath).parent.getValue()['___number'];
                    for (let _i_msg in msg_editor.getValue()) {
                        if (msg_editor.getValue().hasOwnProperty(_i_msg)) {
                            let cut_msg_editor = jseditor.getEditor(_messages_id + "." + _i_msg);

                            if ('___parent' in cut_msg_editor.getValue()
                                && cut_msg_editor.getValue()['___parent'].toString() === number_value.toString()
                            ) {
                                let typeEditor = jseditor.getEditor(cut_msg_editor.path + '.' + '___type');
                                typeEditor.setValue(new_type_value);
                            }
                        }
                    }
                }
            }
            update_msg_tabel(jseditor, jseditor.getEditor(field_path));
        });
    }

    update_msg_tabel(jseditor, jseditor.getEditor(field_path));


}
var updateMessageScheme = function (jseditor) {
    const _header_id = 'root.header';
    const _messages_id = 'root.messages';
    const _header_field_id = 'root.header.___fields';
    let ed = jseditor.getEditor(_header_field_id);
    let msg_editor = jseditor.getEditor(_messages_id);
    let needUpdateEditors = [];

    function updateMessageField(_val_id, _header_id) {
        for (let _i_msg in msg_editor.getValue()) {
            let cut_msg_editor = jseditor.getEditor(_messages_id + "." + _i_msg);

            if (_val_id in _header_id.properties)
                cut_msg_editor.schema.properties[_val_id] = _header_id.properties[_val_id];
            else if (_val_id in cut_msg_editor.schema.properties)
                delete cut_msg_editor.schema.properties[_val_id];

            if (_val_id in cut_msg_editor.cached_editors)
                delete cut_msg_editor.cached_editors[_val_id];

            const _field_path = cut_msg_editor.path + '.___fields';
            needUpdateEditors.push(_field_path);

            if (_val_id in _header_id.properties) {
                cut_msg_editor.addObjectProperty(_val_id);

                jseditor.watch(cut_msg_editor.path + '.' + _val_id, function () {
                    update_msg_tabel(jseditor, jseditor.getEditor(_field_path));
                });

            } else
                cut_msg_editor.removeObjectProperty(_val_id);
        }
    }

    if (ed) {
        let _header_id = null;
        for (let file in jseditor.refs) {
            let matchKey = file.match("header_id$");
            if (matchKey != null
                && jseditor.refs.hasOwnProperty(file)
            ) {
                _header_id = jseditor.refs[file];
                break;
            }
        }

        let values = ed.getValue();

        let _list_of_field = ["___number"];
        let _list_of_description = new Map();
        let _list_of_type = new Map();

        for (const i in values) {

            if (!values.hasOwnProperty(i))
                continue;
            const val = values[i];
            if ('LogicalType' in val
                && g_reserverd_values.indexOf(val['id']) === -1
            ) {
                const _val_id = val['id'];
                if (g_has_to_specified_in_msg.indexOf(val['LogicalType']) !== -1) {

                    const typeElement = val["type"]
                    if (typeElement in _numeric_limits)
                    {
                        if(val['LogicalType'] === "unique")
                            continue;//Cannot add not enumerated value

                        if(_numeric_limits[typeElement]["is_integer"].toLowerCase === 'true')
                            _list_of_type.set(_val_id,
                                {
                                    "type":'integer',
                                    'minimum':parseInt(_numeric_limits[typeElement]['min']),
                                    'maximum':parseInt(_numeric_limits[typeElement]['max'])
                                }
                                );
                        else
                            _list_of_type.set(_val_id, {"type":'number'});
                    }else
                    {
                        let _enum_type = jseditor.getValue().user_enums.find(obj => {
                            return obj.___name === typeElement
                        });
                        if (_enum_type) {

                            let _enum_values = []
                            for (let _num in _enum_type["___numbers"])
                            {
                                const _v=  _enum_type["___numbers"][_num];
                                _enum_values.push(_v["id"].toString())
                            }
                            if (_enum_values.length > 1)
                                _enum_values.push(_enum_values.join(', '))

                            let _val = {
                                "type": 'string',
                                "pattern": '(?:(?:|, )(' + _enum_values.join('|') + '))+$'
                            }
                            _list_of_type.set(_val_id,_val);
                        }else
                        {
                            console.assert(false,'Object is not implemented');//@todo implement
                        }
                    }
                    _list_of_field.push(_val_id);
                    _list_of_description.set(_val_id, val['description']);
                }
            }
        }
        for (const i in _list_of_field) {
            const _val_id = _list_of_field[i];
            let is_changed = false;


            if (!(_val_id in _header_id.properties)) {
                is_changed = true;

                //todo limitation
                _header_id.properties[_val_id] = Object.assign({
                    "propertyOrder": -1,
                    "title": _list_of_description.get(_val_id)
                },_list_of_type.get(_val_id) );

                _header_id.required.push(_val_id);
            }
            if (is_changed)
                updateMessageField(_val_id, _header_id);

        }
        for (const i in _header_id.required) {
            const _val_id = _header_id.required[i];

            if (_list_of_field.indexOf(_val_id) === -1) {

                delete _header_id.properties[_val_id];
                const _index = _header_id.required.indexOf(_val_id)
                if (_index !== 0)
                    _header_id.required.splice(_index, 1);

                updateMessageField(_val_id, _header_id);
            }
        }
    }
    (new Set(needUpdateEditors)).forEach(function (value) {
        update_msg_tabel(jseditor, jseditor.getEditor(value));
    })

}