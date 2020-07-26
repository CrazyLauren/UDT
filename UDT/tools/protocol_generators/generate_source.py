import jinja2
import json
import datetime
import os
import shutil


def generate_source(aExitPath, aModel):
    lib_name = aModel["name"]

    type_info_file = open('schemas/numeric_limits.json', 'r')
    type_info_json = type_info_file.read()
    type_info_file.close()
    type_info = json.loads(type_info_json)

    template_loader = jinja2.FileSystemLoader(searchpath='templates')
    template_env = jinja2.Environment(loader=template_loader,
                                      lstrip_blocks=True,
                                      trim_blocks=True
                                      )

    def get_type_info(field_type):
        if isinstance(field_type,str):
            return type_info[field_type]
        else:
            _rval = []
            for field in field_type:
                _rval.append(type_info[field])
            return _rval


    def bit_field(fields):
        _rval = []
        _field_info = dict()
        for field in fields:
            if 'bits' in field and int(field['bits']) > 0:

                if len(_field_info):

                    if field['type'] != _field_info['type']:
                        raise ValueError("The bit fields type doesn't equal. Prev type {0}, new type".
                                         format(_field_info['type'], field['type'])
                                         )
                    _field_info['fields'].append(field)
                    _field_info['bit_len'] += int(field['bits'])

                    _type_size = int(type_info[field['type']]['size'])

                    if _field_info['bit_len'] == _type_size * 8:
                        _rval.append(_field_info)
                        _field_info = dict()
                else:
                    _field_info['type'] = field['type']
                    _field_info['fields'] = []
                    _field_info['fields'].append(field)
                    _field_info['bit_len'] = 0
                    _field_info['bit_len'] += int(field['bits'])
            else:
                if len(_field_info):
                    raise ValueError("In the bit fields some bit doesnt defined.")

        if len(_field_info):
            _rval.append(_field_info)
            _field_info = dict()
        return _rval

    def get_message_fields_size(aMsg):
        _rval = 0
        for field in aMsg['___fields']:
            _type_size = int(type_info[field['type']]['size'])
            if int(field['bits']) > 0:
                _rval += int(field['bits'])
            elif int(field['array_len']) > 0:
                _rval += _type_size * 8 * int(field['array_len'])
            else:
                _rval += _type_size * 8
        return _rval

    def get_message_size( aMsg ):
        _rval = get_message_fields_size(aMsg)
        if '___size' in aMsg and int(aMsg['___size'])*8 > _rval:
            _rval = int(aMsg['___size'])*8

        return _rval + _rval % 32

    def get_header_field_info( aHeader, aInfo ,aIsAll=False):
        _rval=[]
        for field in aHeader['___fields']:
            if 'LogicalType' in field and field['LogicalType'] in aInfo:
                _rval.append(field);
                if not aIsAll:
                    break
        if len(_rval) == 0:
            return ''
        elif aIsAll:
            return _rval
        else:
            return _rval[0]


    template_env.filters['bit_field'] = bit_field
    template_env.filters['field_size'] = get_message_fields_size
    template_env.filters['full_size'] = get_message_size
    template_env.filters['logical_type'] = get_header_field_info
    template_env.filters['type_info'] = get_type_info


    def gen_file(aName, aTo='', aOutName=''):
        _file = template_env.get_template('{0}.tmpl'.format(aName))
        _file.globals['now'] = datetime.datetime.utcnow()
        _file_result = _file.render(aModel)
        if not aOutName:
            aOutName = aName
        with open(os.path.join(aTo, aOutName), "w") as fh:
            fh.write(_file_result)

    lib_path = os.path.join(aExitPath, lib_name)
    if os.path.exists(lib_path):
        shutil.rmtree(lib_path)
    os.mkdir(lib_path)

    os.mkdir(os.path.join(lib_path, "CMakeModules"))

    os.mkdir(os.path.join(lib_path, "src"))
    os.mkdir(os.path.join(lib_path, "tools"))
    os.mkdir(os.path.join(lib_path, "examples"))
    os.mkdir(os.path.join(lib_path, "tests"))
    os.mkdir(os.path.join(lib_path, "doc"))

    gen_file("src/protocol.h", lib_path,
             "src/protocol_{0}.h".format(str.lower(lib_name)))

    gen_file("src/attribute_packed.h", lib_path,
             "src/protocol_{0}_attribute_packed.h".format(str.lower(lib_name)))

    gen_file("src/protocol_swap_endian.h", lib_path,
             "src/protocol_{0}_swap_endian.h".format(str.lower(lib_name)))
    gen_file("src/protocol_msg_headers.h", lib_path,
             "src/protocol_{0}_msg_headers.h".format(str.lower(lib_name)))
    gen_file("src/protocol_serialize_binary.h", lib_path,
             "src/protocol_{0}_serialize_binary.h".format(str.lower(lib_name)))

    gen_file("src/protocol_serialize.h", lib_path,
             "src/protocol_{0}_serialize.h".format(str.lower(lib_name)))
    gen_file("src/protocol_ostream.h", lib_path,
              "src/protocol_{0}_ostream.h".format(str.lower(lib_name)))

    gen_file("src/import_export_macros.h", lib_path,
             "src/{0}_import_export.h".format(str.lower(lib_name)))
    gen_file("src/CProtocol.h", lib_path,
             "src/CProtocol_{0}.h".format(lib_name))
    gen_file("src/CProtocol.cpp", lib_path,
             "src/CProtocol_{0}.cpp".format(lib_name))
    gen_file("src/CMakeLists.txt", lib_path)
    gen_file("CMakeModules/config.h.cmake", lib_path)
    gen_file("CMakeModules/configure_PROTOCOL.cmake", lib_path,
             "CMakeModules/configure_{0}.cmake".format(str.upper(lib_name)))
    gen_file("CMakeModules/dependencies_search.cmake", lib_path)
    gen_file("CMakeModules/PROTOCOL_CPack.cmake", lib_path,
             "CMakeModules/{0}_CPack.cmake".format(str.upper(lib_name)))
    gen_file("LICENCE.txt", lib_path)
    gen_file("CMakeLists.txt", lib_path)
    shutil.copytree("templates/CMakeModules/functions",
                    os.path.join(lib_path,
                                 "CMakeModules/functions"))

    gen_file("tools/CMakeLists.txt", lib_path)
    gen_file("examples/CMakeLists.txt", lib_path)
    gen_file("tests/CMakeLists.txt", lib_path)
    gen_file("doc/doxygen.conf.in", lib_path)
    shutil.copyfile("templates/doc/doxygen.css", os.path.join(lib_path, "doc/doxygen.css"))
