const g_reserverd_values = ["___number",
    "___size",
    "___parent",
    "___type"];

const g_has_to_specified_in_msg = ["unique",
    "version_major",
    "version_minor"
];

var update_msg_tabel = function (jseditor, aMsgFieldEditor) {
    function get_id(path) {
        return path.replaceAll('.', '_');
    }

    const headerFieldsPath = 'root.header.___fields';
    const _tb_class = 'message_scheme';
    let msgEditor = aMsgFieldEditor.parent;
    const messagePath = msgEditor.path;
    const NUM_OF_BIT_FIELDS = 32;
    const messageID = get_id(messagePath);
    const dynamic_array_class = 'dynamic_array';


    function appendElements(_i_num_bits, aNumBits, uniqueId, tr, _tb) {
        let _bits = aNumBits;
        for (; _bits > 0;) {
            if ((_i_num_bits + _bits) > NUM_OF_BIT_FIELDS) {
                if(_i_num_bits !== NUM_OF_BIT_FIELDS)
                    $("<td>").attr('colspan',
                        NUM_OF_BIT_FIELDS - _i_num_bits
                    ).attr('fieldID', uniqueId).appendTo(tr);

                _bits -= NUM_OF_BIT_FIELDS - _i_num_bits;
                tr = $("<tr>").appendTo(_tb);
                _i_num_bits = 0;
            } else {
                _i_num_bits += _bits;

                $("<td>").attr('colspan',
                    _bits
                ).attr('fieldID', uniqueId).appendTo(tr);

                _bits = 0;
            }
        }
        return [_i_num_bits, tr];
    }

    function appendDynamicFields(valueElement, _msg_num_bits, _tb_dynamic, fieldPath) {
        const typeElement = valueElement['DynamicType'];
        const valueId = valueElement['id'];
        const fieldID = get_id(fieldPath);
        const uniqueId=fieldID + '_dynamic';

        function appendRow() {
            _msg_num_bits += NUM_OF_BIT_FIELDS;
            let tr = $("<tr>").attr('fieldID', uniqueId).addClass(dynamic_array_class).appendTo(_tb_dynamic)
            if ('DynamicOrder' in valueElement)
                tr.attr('DynamicOrder', valueElement['DynamicOrder'])
            tr.on('click', function () {

                let _scroll_to = $('[fieldID=\"'+ fieldID+'\"]');
                if (_scroll_to.length > 0) {

                    _scroll_to.get(0).scrollIntoView();
                    _scroll_to.show().fadeOut(250).fadeIn(250).fadeOut(250).fadeIn(250).fadeOut(250).fadeIn(250);
                }
            });
            return tr;
        }

        $("<td>").text(valueId+": dynamic array of type " + typeElement).attr('colspan',
            NUM_OF_BIT_FIELDS
        ).addClass(dynamic_array_class).appendTo(appendRow());

        // $("<td>").text('').attr('colspan',
        //     NUM_OF_BIT_FIELDS
        // ).addClass(dynamic_array_class).appendTo(appendRow());

        return _msg_num_bits;
    }

    function append_fields(aFieldEditor, _table, _errors) {
        const msgPath = aFieldEditor.parent.path;
        let msgEditor = jseditor.getEditor(msgPath);

        const fieldPath = aFieldEditor.path;
        const fieldID = get_id(fieldPath);

        let mf = aFieldEditor.value;
        let _tb = $("<tbody>").appendTo(_table);
        _tb.attr("id", fieldID);
        let _i_raw_bits = 0;
        let _msg_num_bits = 0;
        let tr = $("<tr>").appendTo(_tb);
        let uniqueID_set = new Set();

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

            let _type_info = null;
            if (typeElement in _numeric_limits) {
                _type_info = _numeric_limits[typeElement];
            }
            let _max_type_bits = -1;
            if (_type_info != null) {
                _max_type_bits = _type_info['size'] * 8;
            } else {
                _errors.push(`No type info for ${typeElement} of field ${valueId}; `);
                continue;
            }
            let _num_bits = _max_type_bits;
            const _user_bits = valueElement['bits'];

            if (_user_bits > 0) {
                _num_bits = Math.min(_user_bits, _num_bits);
            }
            _msg_num_bits += _num_bits;
            [_i_raw_bits, tr] = appendElements(_i_raw_bits, _num_bits, uniqueId, tr, _tb);

            if (valueElement['array_len'] > 1) {
                let _array_len = valueElement['array_len'];
                for (let j = 1; j < _array_len; ++j) {
                    _msg_num_bits += _num_bits;
                    [_i_raw_bits, tr] = appendElements(_i_raw_bits, _num_bits, uniqueId, tr, _tb);
                }
            }

            let fieldElement = _tb.find('[fieldID=\"'+ uniqueId+'\"]');

            fieldElement.text(valueId).on('click', function () {
                let _scroll_to = $('[data-schemapath=\"' + fieldPath + '.' + _i + '\"]');
                if (_scroll_to.length > 0) {

                    let _tab = $('a[href="#' + msgPath + '"]');
                    if (_tab.length > 0)
                        _tab[0].click();

                    _scroll_to.get(0).scrollIntoView();
                    _scroll_to.show().fadeOut(250).fadeIn(250).fadeOut(250).fadeIn(250).fadeOut(250).fadeIn(250);
                }
            });
            if (LogicalType) {
                fieldElement.attr("LogicalType", LogicalType);
            }

            if (valueElement['bits'] > (_type_info['size'] * 8)) {
                fieldElement.addClass('error');
                _errors.push(`Amount of bits (${_user_bits}) is greater then ${_max_type_bits}. See field ${valueId}; `);
            }
            if (valueElement['array_len'] > 0) {
                fieldElement.addClass('array');
            }
            if (valueElement['bits'] > 0 && valueElement['array_len'] > 0) {
                fieldElement.addClass('error');
                _errors.push(`Using bit fields with array no allowed. See field ${valueId}; `);
            }
            if (uniqueID_set.has(valueId)) {
                fieldElement.addClass('error');
                _errors.push(`Field ID ${valueId} isn't unique; `);
            } else
                uniqueID_set.add(valueId);

        }
        if (_i_raw_bits !== NUM_OF_BIT_FIELDS) {

            $("<td>").attr('colspan',
                NUM_OF_BIT_FIELDS - _i_raw_bits
            ).addClass('empty').appendTo(tr);
            _msg_num_bits += NUM_OF_BIT_FIELDS - _i_raw_bits;
            _i_raw_bits = 0;
        }
        if ('___size' in msgEditor.value) {

            const sizeEditor = jseditor.getEditor(msgPath + '.' + '___size');

            if (sizeEditor.value > 0 && _msg_num_bits < sizeEditor.value * 8) {

                let _num_row = Math.floor((sizeEditor.value * 8 - _msg_num_bits) / NUM_OF_BIT_FIELDS);

                for (let j = 0; j < _num_row; ++j) {
                    _msg_num_bits += NUM_OF_BIT_FIELDS;
                    tr = $("<tr>").appendTo(_tb);
                    $("<td>").attr('colspan',
                        NUM_OF_BIT_FIELDS
                    ).attr('fieldID', "reserved").addClass('empty').appendTo(tr);
                }
                let _mod_row = (sizeEditor.value * 8 - _msg_num_bits) % NUM_OF_BIT_FIELDS;
                if (_mod_row > 0) {
                    _msg_num_bits += NUM_OF_BIT_FIELDS;

                    tr = $("<tr>").appendTo(_tb);
                    $("<td>").attr('colspan',
                        NUM_OF_BIT_FIELDS
                    ).attr('fieldID', "reserved").addClass('empty').appendTo(tr);
                }
            }
        }
        if ('___type' in msgEditor.value) {
            const typeEditor = jseditor.getEditor(msgPath + '.' + '___type');

            if (typeEditor.value === "dynamic") {

                let _tb_dynamic = $("tbody." + dynamic_array_class, _table);
                if (_tb_dynamic.length === 0) {
                    _tb_dynamic = $("<tbody>").addClass(dynamic_array_class).appendTo(_table);
                }
                for (let _i in mf) {
                    if (!mf.hasOwnProperty(_i))
                        continue;
                    let valueElement = mf[_i];
                    if ('LogicalType' in valueElement
                        && 'DynamicType' in valueElement
                        && valueElement['LogicalType'] === 'dynamic array'
                    ) {
                        const valPath = fieldPath + '.' + _i;
                        _msg_num_bits = appendDynamicFields(valueElement, _msg_num_bits, _tb_dynamic, valPath);
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
        if ('___parent' in msgEditor.value)
            parentEditor = jseditor.getEditor(msgPath + '.' + '___parent');

        if ('___number' in msgEditor.value)
            numberEditor = jseditor.getEditor(msgPath + '.' + '___number');

        if (parentEditor
            && parentEditor.value
            && numberEditor
            && parentEditor.value.toString() !== numberEditor.value.toString()
        ) {
            const _parent = parentEditor.value;

            for (let key in jseditor.editors) {
                let matchKey = key.match("\\.___fields$");
                if (matchKey != null /*indexOf(".___fields")!== -1*/
                    && jseditor.editors.hasOwnProperty(key)
                ) {
                    const messageEditor = jseditor.getEditor(key);
                    if ('___number' in messageEditor.parent.value) {
                        let numberEditor2 = jseditor.getEditor(messageEditor.parent.path + '.' + '___number');

                        if (numberEditor2.value.toString() === _parent.toString()) {
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
            let fieldTable = $('[fieldID=\"'+ fieldID+'\"]', _table);
            if(fieldTable.length>0) {
                console.assert(fieldTable.length === 1, "Several path of " + fieldPath);
                _field_in_other_msgs.replaceWith(fieldTable.clone(true, true));
            }
        }
    }

    function createTableOfMsg() {
        let _table = $("<table>");
        _table.addClass(_tb_class);
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
            if ('___number' in msgEditor.value) {
                let headerFieldsEditor = jseditor.getEditor(headerFieldsPath);
                append_fields(headerFieldsEditor, _table, _errors)
                const headerId = get_id(headerFieldsPath);

                let headerTable = $('#' + headerId, _table);
                console.assert(headerTable.length === 1, "No message's header");

                if (headerTable.length > 0) {
                    $('td', headerTable).each(function (index, td) {
                        if (td.innerText in msgEditor.value) {
                            let valEditor = jseditor.getEditor(msgEditor.path + '.' + td.innerText);
                            if (valEditor)
                                td.innerText = valEditor.value.toString();
                        }
                    });
                    $('td[isunique=\"number\"]', headerTable).each(function (index, td) {
                        let valEditor = jseditor.getEditor(msgEditor.path + '.___number');
                        td.innerText = valEditor.value.toString();
                    });
                }
            }
            create_fields_of_msg(_table, aMsgFieldEditor, _errors);
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
    for (const _val in editor.value) {
        if (_val === '___description')
            continue;
        const valuePath = editor.path + '.' + _val;
        jseditor.watch(valuePath, function () {

            let msg_editor = jseditor.getEditor(_messages_id);
            if (_val === '___number') {
                const numberEditor = jseditor.getEditor(valuePath);
                const new_number_value = numberEditor.value;
                const old_number_value = numberEditor.parent.value.___number;

                if (old_number_value && old_number_value !== new_number_value) {


                    for (let _i_msg in msg_editor.value) {
                        if (msg_editor.value.hasOwnProperty(_i_msg)) {
                            let cut_msg_editor = jseditor.getEditor(_messages_id + "." + _i_msg);

                            if ('___parent' in cut_msg_editor.value
                                && cut_msg_editor.value.___parent.toString() === old_number_value.toString()
                            ) {
                                let parentEditor = jseditor.getEditor(cut_msg_editor.path + '.' + '___parent');
                                parentEditor.value = new_number_value;

                            }
                        }
                    }
                }
            }
            if(_val === '___type')
            {
                const typeEditor = jseditor.getEditor(valuePath);
                const new_type_value = typeEditor .value;
                if(new_type_value === 'dynamic') {
                    const number_value = jseditor.getEditor(valuePath).parent.value.___number;
                    for (let _i_msg in msg_editor.value) {
                        if (msg_editor.value.hasOwnProperty(_i_msg)) {
                            let cut_msg_editor = jseditor.getEditor(_messages_id + "." + _i_msg);

                            if ('___parent' in cut_msg_editor.value
                                && cut_msg_editor.value.___parent.toString() === number_value.toString()
                            ) {
                                let typeEditor = jseditor.getEditor(cut_msg_editor.path + '.' + '___type');
                                typeEditor.value = new_type_value;
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
        for (let _i_msg in msg_editor.value) {
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

        let values = ed.value;

        let _list_of_field = ["___number"];
        let _list_of_description = new Map();

        for (const i in values) {

            if (!values.hasOwnProperty(i))
                continue;
            const val = values[i];
            if ('LogicalType' in val
                && g_reserverd_values.indexOf(val['id']) === -1
            ) {
                const _val_id = val['id'];
                if (g_has_to_specified_in_msg.indexOf(val['LogicalType']) !== -1) {

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

                _header_id.properties[_val_id] = {
                    "type": "integer",
                    "propertyOrder": -1,
                    "title": _list_of_description.get(_val_id)
                };
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