$(document).ready(function () {

        JSONEditor.defaults.options.template = 'swig'
        JSONEditor.defaults.theme = 'bootstrap3';
        JSONEditor.defaults.iconlib = 'bootstrap3';
        JSONEditor.defaults.custom_validators.push(function (schema, value, path) {
            let errors = [];
            if (schema.type !== 'object') {
                return errors;
            }
            if (!('properties' in schema)) {
                return errors;
            }
            let property = '';
            let uniqueValues = [];
            let uniqueTitleFieldsStr = new Map();
            let uniqueFieldsStr = new Map();
            Object.keys(schema.properties).forEach(function (propKey) {
                property = propKey;
                let path = 'root.' + property;
                const propObj = schema.properties[propKey];
                if (propObj.type !== 'array') {
                    return;
                }
                if (!('uniqueFields' in propObj)) {
                    return;
                }
                if(uniqueFieldsStr.size !== propObj.uniqueFields.length)
                    Object.keys(propObj.uniqueFields).forEach(function (field) {
                        if (!uniqueFieldsStr.has(propObj.uniqueFields[field]) ) {
                            uniqueTitleFieldsStr.set(propObj.uniqueFields[field], '');
                            uniqueFieldsStr.set(propObj.uniqueFields[field], []);
                            if('title' in propObj)
                            uniqueTitleFieldsStr.set(propObj.uniqueFields[field],propObj['title']);
                        }

                    });
                if (!(propKey in value) || !value[propKey].length) {
                    return;
                }
                value[propKey].forEach(function (valueObj) {
                    //let fieldValueStr = '';
                    Object.keys(valueObj).forEach(function (field) {
                        if (propObj.uniqueFields.indexOf(field) > -1) {
                            let _val=uniqueFieldsStr.get(field)
                            if (_val) {
                                    _val.push(valueObj[field]);
                                    // uniqueTitleFieldsStr.set(propObj.uniqueFields[field],
                                    //     uniqueTitleFieldsStr.get(propObj.uniqueFields[field]) + ", " +
                                    //     valueObj[field]);
                                    //fieldValueStr += valueObj[field];
                                    if (!(field in uniqueValues))
                                        uniqueValues[field] = [];
                                    uniqueValues[field].push(valueObj[field]);
                                }
                        }
                    });
                    //uniqueValues.push(fieldValueStr);
                });
            });
            Object.keys(uniqueValues).forEach(function (aUnique) {
                if ((new Set(uniqueValues[aUnique])).size !== uniqueValues[aUnique].length) {
                    errors.push({
                        path: path,
                        property: property,
                        message: 'Value for ' + uniqueTitleFieldsStr.get(aUnique) + ' must be unique across ' + uniqueFieldsStr.get(aUnique).join(', ')
                    });
                }
            });
            return errors;
        });

        var jseditor = null;

        function create_editor() {
            if (jseditor instanceof window.JSONEditor)
                jseditor.destroy();

            jseditor = new JSONEditor(document.getElementById('editor_holder'), {
                // Enable fetching schemas via ajax
                ajax: true,

                // The schema for the editor
                schema: {
                    $ref: "schemas/protocol_schema.json"
                },

                // Require all properties by default
                required_by_default: true,
                disable_properties: true
            });
            jseditor.on('ready', function () {

                jseditor.on('addRow', editor => {
                    if (editor && ('___fields' in editor.value)) {
                        addNewRow(editor, jseditor);
                    }
                });

                jseditor.on('change', function () {
                    // Get an array of errors from the validator
                    let errors = jseditor.validate();

                    let indicator = document.getElementById('valid_indicator');

                    // Not valid
                    if (errors.length) {
                        indicator.style.color = 'red';
                        indicator.textContent = "not valid";
                    }
                    // Valid
                    else {
                        indicator.style.color = 'green';
                        indicator.textContent = "valid";
                    }
                });
                const _field = 'root.header.___fields';
                if (jseditor.getEditor(_field)) {
                    const _size = 'root.header.___size';

                    jseditor.watch(_field, function () {
                        update_msg_tabel(jseditor, jseditor.getEditor(_field));
                        updateMessageScheme(jseditor);
                    });
                    jseditor.watch(_size, function () {
                        update_msg_tabel(jseditor, jseditor.getEditor(_field));
                    });

                    update_msg_tabel(jseditor, jseditor.getEditor(_field));
                }
            });


            return jseditor;
        }

        create_editor();


        var open_protocol = function (protocol_json) {
            create_editor();
            if (protocol_json != null) {
                jseditor.on('ready', function () {

                    jseditor.setValue(protocol_json);
                    const _messages_id = 'root.messages';
                    let msg_editor = jseditor.getEditor(_messages_id);
                    if (msg_editor)
                        for (let _i_msg in msg_editor.value) {
                            if (!msg_editor.value.hasOwnProperty(_i_msg))
                                continue;
                            else {
                                let _editor = jseditor.getEditor(msg_editor.value[_i_msg].path + '.___fields');
                                if (_editor) update_msg_tabel(jseditor, _editor);
                            }
                        }

                });
            }
        }


        $("#save").click(function () {
            let example = jseditor.getValue();
            const filename = example.name ? example.name + '.json' : 'example.json';

            let blob = new Blob([JSON.stringify(example, null, 2)],
                {type: "application/json;charset=utf-8 "});

            var a = document.createElement('a');
            a.download = filename;
            a.href = URL.createObjectURL(blob);
            a.dataset.downloadurl = ['text/plain', a.download, a.href].join(':');
            a.dispatchEvent(new MouseEvent('click', {'view': window, 'bubbles': true, 'cancelable': false}));
        });

        $("#open").click(function () {
            var a = $(document.createElement('input'));
            a.attr("type", "file");
            a.attr("accept", ".json");
            a.trigger("click");
            a.on('change', function (ev) {
                let files = ev.target.files;
                if (files.length < 1) {
                    alert("Files hasn't selected ...");
                    return;
                }
                let file = files[0];
                let reader = new FileReader()
                reader.onload = function (eval) {
                    let protocol = JSON.parse(eval.target.result);
                    open_protocol(protocol);
                }
                reader.readAsText(file);
            })
        });
        $("#refresh").click(function () {
            let _value = jseditor.getValue();
            open_protocol(null);
            jseditor.on('ready', function () {
                open_protocol(_value);
            });
        });


    }
);