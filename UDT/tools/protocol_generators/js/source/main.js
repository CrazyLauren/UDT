$(document).ready(function () {

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
            let uniqueFieldsStr = '';
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
                uniqueFieldsStr = propObj.uniqueFields.join(', ');
                if (!(propKey in value) || !value[propKey].length) {
                    return;
                }
                value[propKey].forEach(function (valueObj) {
                    let fieldValueStr = '';
                    Object.keys(valueObj).forEach(function (field) {
                        if (propObj.uniqueFields.indexOf(field) > -1) {
                            fieldValueStr += valueObj[field];
                        }
                    });
                    uniqueValues.push(fieldValueStr);
                })
            });
            if ((new Set(uniqueValues)).size !== uniqueValues.length) {
                errors.push({
                    path: path,
                    property: property,
                    message: 'Value for ' + property + ' must be unique across ' + uniqueFieldsStr
                });
            }
            return errors;
        });

        var jseditor = new JSONEditor(document.getElementById('editor_holder'), {
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
        jseditor.on('addRow', editor => {
            ;
            if (editor && ('___fields' in editor.value)) {
                addNewRow(editor, jseditor);
            }
        });

        var open_protocol = function (protocol_json) {
            if (protocol_json != null) {
                jseditor.setValue(protocol_json);
                const _messages_id = 'root.messages';
                let msg_editor = jseditor.getEditor(_messages_id);
                for (let _i_msg in msg_editor.value) {
                    if (!msg_editor.value.hasOwnProperty(_i_msg))
                        continue;
                    else {
                        let _editor=jseditor.getEditor(msg_editor.value[_i_msg].path + '.___fields');
                        if(_editor) update_msg_tabel(jseditor,_editor);
                    }
                }
            } else {
                jseditor.setValue({});
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

        jseditor.on('ready', function () {

            const _field = 'root.header.___fields';
            const _size = 'root.header.___size';
            jseditor.watch(_field, function () {
                update_msg_tabel(jseditor, jseditor.getEditor(_field));
                updateMessageScheme(jseditor);
            });
            jseditor.watch(_size, function () {
                update_msg_tabel(jseditor, jseditor.getEditor(_field));
            });

            update_msg_tabel(jseditor, jseditor.getEditor(_field));

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
    }
);