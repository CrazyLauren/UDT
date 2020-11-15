import errno
import subprocess

import jinja2
import json
import datetime
import os
import shutil

from py import protocol_schema as ps, latex_schema as ls

g_asserting_that = "asserting that"
g_executor = "executor"
g_matching = "matching"

template_loader = jinja2.FileSystemLoader(searchpath='templates')
template_env = jinja2.Environment(loader=template_loader,
                                  lstrip_blocks=True,
                                  trim_blocks=True
                                  )


def cast_to_func(aField, aDynamic=False):
    if not aDynamic and \
            (str(aField['type']).lower() in ['char', 'uint8_t', 'int8_t']
             or (not aField['TypeInfo']["standard"] and
                 'type' in aField['TypeInfo']
                 and (str(aField['TypeInfo']['type']).lower()
                      in ['char', 'uint8_t', 'int8_t']))):
        return 'int'
    if aDynamic and \
            (str(aField['TypeOfLogicalType']).lower() in ['char', 'uint8_t', 'int8_t']
             or (not aField['LogicalTypeInfo']["standard"] and
                 'type' in aField['LogicalTypeInfo']
                 and (str(aField['LogicalTypeInfo']['type']).lower()
                      in ['char', 'uint8_t', 'int8_t']))):
        return 'int'
    return ''


template_env.filters['castto'] = cast_to_func
template_env.filters['fields'] = ps.all_field
template_env.filters['eswap'] = ps.swap_bitfield_endian
template_env.filters['is_enumerate'] = ps.is_enumerated_logical_type
template_env.filters['is_complex'] = ps.is_complex_type
template_env.filters['is_dynamic'] = ps.is_dynamic_type
template_env.filters['is_array'] = ps.is_array_type
template_env.filters['is_bit_field'] = ps.is_bit_field
template_env.filters['all_fields'] = ps.all_field_msg



template_env.filters['logical_type'] = ps.get_header_field_info
template_env.filters['bool'] = lambda Val: str(Val).lower() in ['true', '1', 'y', 'yes']

template_env.filters['latex_escape'] = ls.latex_escape
template_env.filters['split_text'] = ls.split_text
template_env.filters['limitation'] = ps.get_field_limitation
template_env.filters['path_list'] = ps.get_path_list

def alignment_field(aMsg, aIgnore=None):
    _rval, _offset, _msg_align = ps.alignment_fields(aMsg, aIgnore)
    return _rval


def get_message_size(aMsg):
    _rval, _offset, _msg_align = ps.alignment_fields(aMsg)
    return _offset


def gen_file(aName, aTo, aOutName, aProtocol2):
    _file = template_env.get_template('{0}.tmpl'.format(aName))
    _file.globals['now'] = datetime.datetime.utcnow()
    _file_result = _file.render(aProtocol2)
    if not aOutName:
        aOutName = aName
    with open(os.path.join(aTo, aOutName), "w", encoding='utf-8') as fh:
        fh.write(_file_result)

    if 'files' not in aProtocol2:
        aProtocol2['files'] = []

    with open(os.path.join(aTo, aOutName), 'rb') as fh:
        import hashlib
        _val = {'name': aOutName, 'path': aTo, 'md5': hashlib.md5(fh.read()).hexdigest()}
        aProtocol2['files'].append(_val)


def generate_source(lib_path, _protocol):
    lib_name = _protocol["name"]

    try:
        os.mkdir(os.path.join(lib_path, "CMakeModules"))

        os.mkdir(os.path.join(lib_path, "src"))
        os.mkdir(os.path.join(lib_path, "src/include"))
        os.mkdir(os.path.join(lib_path, "src/udt_lib"))
        os.mkdir(os.path.join(lib_path, "src/python_lib"))
        os.mkdir(os.path.join(lib_path, "src/matlab_lib"))


        os.mkdir(os.path.join(lib_path, "tools"))
        os.mkdir(os.path.join(lib_path, "examples"))
        os.mkdir(os.path.join(lib_path, "tests"))
        os.mkdir(os.path.join(lib_path, "tests/{0}_test".format(str.lower(lib_name))))
        os.mkdir(os.path.join(lib_path, "doc"))
        os.mkdir(os.path.join(lib_path, "gui"))
    except Exception:
        pass

    _protocol_file_name = "{0}{1}{2}".format(lib_path, os.sep, '{0}.json'.format(str.lower(lib_name)))
    with open(_protocol_file_name, 'w', encoding='utf-8') as fp:
        json.dump(_protocol, fp, indent=4)

    with open(_protocol_file_name, 'rb') as fp:
        import hashlib
        _protocol['md5'] = hashlib.md5(fp.read()).hexdigest()
        _protocol['file_name'] = '{0}.json'.format(str.lower(lib_name))

    #Generate source
    gen_file("src/CMakeLists.txt", lib_path, '', _protocol)

    #   Generate Header files
    gen_file("src/include/protocol.h", lib_path,
             "src/include/protocol_{0}.h".format(str.lower(lib_name)), _protocol)
    gen_file("src/include/protocol_impl.h", lib_path,
             "src/include/protocol_{0}_impl.h".format(str.lower(lib_name)), _protocol)
    #
    gen_file("src/include/attribute_packed.h", lib_path,
             "src/include/protocol_{0}_attribute_packed.h".format(str.lower(lib_name)), _protocol)
    #
    gen_file("src/include/protocol_swap_endian.h", lib_path,
             "src/include/protocol_{0}_swap_endian.h".format(str.lower(lib_name)), _protocol)
    gen_file("src/include/protocol_msg_headers.h", lib_path,
             "src/include/protocol_{0}_msg_headers.h".format(str.lower(lib_name)), _protocol)
    gen_file("src/include/protocol_serialize_binary.h", lib_path,
             "src/include/protocol_{0}_serialize_binary.h".format(str.lower(lib_name)), _protocol)
    gen_file("src/include/protocol_compare_operators.h", lib_path,
             "src/include/protocol_{0}_compare_operators.h".format(str.lower(lib_name)), _protocol)

    gen_file("src/include/protocol_udt.h", lib_path,
             "src/include/protocol_{0}_udt.h".format(str.lower(lib_name)), _protocol)

    gen_file("src/include/protocol_parser.h", lib_path,
             "src/include/protocol_{0}_parser.h".format(str.lower(lib_name)), _protocol)
    #
    gen_file("src/include/protocol_serialize.h", lib_path,
             "src/include/protocol_{0}_serialize.h".format(str.lower(lib_name)), _protocol)
    gen_file("src/include/protocol_ostream.h", lib_path,
             "src/include/protocol_{0}_ostream.h".format(str.lower(lib_name)), _protocol)
    #
    gen_file("src/include/import_export_macros.h", lib_path,
             "src/include/protocol_{0}_import_export.h".format(str.lower(lib_name)), _protocol)

    gen_file("src/include/protocol_test_message.h", lib_path,
             'src/include/protocol_{0}_test_message.h'.format(str.lower(lib_name)), _protocol)
    #
    gen_file("src/include/CMakeLists.txt", lib_path, '', _protocol)

    #   Generate UDT files
    gen_file("src/udt_lib/CProtocol.h", lib_path,
             "src/udt_lib/CProtocol_{0}.h".format(lib_name), _protocol)
    gen_file("src/udt_lib/CProtocol.cpp", lib_path,
             "src/udt_lib/CProtocol_{0}.cpp".format(lib_name), _protocol)
    gen_file("src/udt_lib/CMakeLists.txt", lib_path, '', _protocol)

    #   Generate python files
    gen_file("src/python_lib/CMakeLists.txt", lib_path, '', _protocol)
    gen_file("src/python_lib/python_source.cmake", lib_path, '', _protocol)
    gen_file("src/python_lib/python.i", lib_path,
             "src/python_lib/{0}_python.i".format(str.lower(lib_name)), _protocol)

    gen_file("src/python_lib/protocol_udt_api.h", lib_path,
             "src/python_lib/protocol_{0}_udt_api.h".format(str.lower(lib_name)), _protocol)
    gen_file("src/python_lib/protocol_udt_api.impl.h", lib_path,
             "src/python_lib/protocol_{0}_udt_api.impl.h".format(str.lower(lib_name)), _protocol)
    gen_file("src/python_lib/example.py", lib_path,
             "", _protocol)
    gen_file("src/python_lib/revision.c.in", lib_path,
             "", _protocol)
    gen_file("src/python_lib/ChangeLog.txt.in", lib_path,
             "", _protocol)
    gen_file("src/python_lib/revision.h", lib_path,
             "src/python_lib/protocol_{0}_python_revision.h.in".format(str.lower(lib_name)),
             _protocol)

    #   Generate matlab files
    gen_file("src/matlab_lib/CMakeLists.txt", lib_path, '', _protocol)
    gen_file("src/matlab_lib/matlab_compatibility.cmake", lib_path, '', _protocol)
    gen_file("src/matlab_lib/matlab_lib.cpp", lib_path,
             "src/matlab_lib/{0}.cpp".format(str.lower(lib_name)), _protocol)
    gen_file("src/matlab_lib/protocol_matlab_serialize.h", lib_path,
             "src/matlab_lib/protocol_{0}_matlab_serialize.h".format(str.lower(lib_name)), _protocol)
    gen_file("src/matlab_lib/protocol_matlab_udt_api.h", lib_path,
             "src/matlab_lib/protocol_{0}_matlab_udt_api.h".format(str.lower(lib_name)), _protocol)
    gen_file("src/matlab_lib/revision.c.in", lib_path,"", _protocol)
    gen_file("src/matlab_lib/ChangeLog.txt.in", lib_path, "", _protocol)
    gen_file("src/matlab_lib/example.m", lib_path, "", _protocol)
    gen_file("src/matlab_lib/protocol_simulink_udt_api.h", lib_path,
             "src/matlab_lib/protocol_{0}_simulink_udt_api.h".format(str.lower(lib_name)), _protocol)
    #Generate cmake files
    gen_file("CMakeModules/config.h.cmake", lib_path, '', _protocol)
    gen_file("CMakeModules/configure_PROTOCOL.cmake", lib_path,
             "CMakeModules/configure_{0}.cmake".format(str.upper(lib_name)), _protocol)
    gen_file("CMakeModules/dependencies_search.cmake", lib_path, '', _protocol)
    gen_file("CMakeModules/PROTOCOL_CPack.cmake", lib_path,
             "CMakeModules/{0}_CPack.cmake".format(str.upper(lib_name)), _protocol)
    gen_file("CMakeModules/matlab.prj.in", lib_path,
             "CMakeModules/matlab_{0}.prj.in".format(str.upper(lib_name)),
              _protocol)
    gen_file("LICENCE.txt", lib_path, '', _protocol)
    gen_file("CMakeLists.txt", lib_path, '', _protocol)

    # Copy common cmake files
    shutil.copytree("templates/CMakeModules/functions",
                    os.path.join(lib_path,
                                 "CMakeModules/functions"))

    shutil.copytree("templates/dependencies",
                    os.path.join(lib_path,
                                 "dependencies"))

    # Generate tools files
    gen_file("tools/CMakeLists.txt", lib_path, '', _protocol)

    # Generate examples files
    gen_file("examples/CMakeLists.txt", lib_path, '', _protocol)

    # Generate tests files
    gen_file("tests/CMakeLists.txt", lib_path, '', _protocol)

    gen_file("tests/protocol_test/CMakeLists.txt", lib_path,
             'tests/{0}_test/CMakeLists.txt'.format(str.lower(lib_name)), _protocol)


    gen_file("tests/protocol_test/protocol_test_generator.h", lib_path,
             'tests/{0}_test/protocol_{0}_test_generator.h'.format(str.lower(lib_name)), _protocol)

    gen_file("tests/protocol_test/serialize_test.h", lib_path,
             'tests/{0}_test/serialize_test_{0}.h'.format(str.lower(lib_name)), _protocol)

    gen_file("tests/protocol_test/serialize_test.cpp", lib_path,
             'tests/{0}_test/serialize_test_{0}.cpp'.format(str.lower(lib_name)), _protocol)

    gen_file("tests/protocol_test/subscriber_test_do.cpp", lib_path,
             'tests/{0}_test/subscriber_test_do_{0}.cpp'.format(str.lower(lib_name)), _protocol)

    gen_file("tests/protocol_test/subscriber_test.h", lib_path,
             'tests/{0}_test/subscriber_test_{0}.h'.format(str.lower(lib_name)), _protocol)

    gen_file("tests/protocol_test/subscriber_test.cpp", lib_path,
             'tests/{0}_test/subscriber_test_{0}.cpp'.format(str.lower(lib_name)), _protocol)

    gen_file("tests/protocol_test/parser_test.h", lib_path,
             'tests/{0}_test/parser_test_{0}.h'.format(str.lower(lib_name)), _protocol)

    gen_file("tests/protocol_test/parser_test.cpp", lib_path,
             'tests/{0}_test/parser_test_{0}.cpp'.format(str.lower(lib_name)), _protocol)

    gen_file("tests/protocol_test/main.cpp", lib_path,
             'tests/{0}_test/main.cpp'.format(str.lower(lib_name)), _protocol)

    # Generate doc files
    gen_file("doc/doxygen.conf.in", lib_path, '', _protocol)

    shutil.copyfile("templates/doc/doxygen.css", os.path.join(lib_path, "doc/doxygen.css"))

    # Generate GUI files
    gen_file("gui/CMakeLists.txt", lib_path, '', _protocol)
    gen_file("gui/js_gui.js", lib_path,
             "gui/{0}.js".format(str.lower(lib_name)), _protocol)

def generate_doc(aTo, aWhat):
    lib_name = aWhat["name"]
    _doc_protocol = ls.generate_latex_schema(aWhat)

    with open("{0}{1}{2}".format(aTo, os.sep, '{0}_doc.json'.format(str.lower(lib_name))), 'w', encoding='utf-8') as fp:
        json.dump(_doc_protocol, fp, indent=4)

    tex_file_name = os.path.join(aTo,
                                 "protocol_{0}.tex".format(str.lower(lib_name)))

    gen_file("doc/protocol.tex", aTo, tex_file_name, _doc_protocol)

    check_output_kwargs = {}
    try:
        command = 'latexmk -cd --pdf --interaction=nonstopmode ' + tex_file_name
        output = subprocess.check_output(command,
                                         stderr=subprocess.STDOUT,
                                         **check_output_kwargs)
        extensions = ['aux','log', 'out', 'fls',
                      'fdb_latexmk']

        for ext in extensions:
            try:
                os.remove(os.path.join(aTo,
                                       "protocol_{0}.{1}".format(str.lower(lib_name),
                                                                 ext
                                                                 )))
            except (OSError, IOError) as e:
                pass
    except (OSError, IOError) as e:
        # Use FileNotFoundError when python 2 is dropped
        os_error = e

        if os_error.errno == errno.ENOENT:
            try:
                print(os_error.output.decode())  # If compiler does not exist, try next in the list
            except Exception:
                pass

    except subprocess.CalledProcessError as e:
        # For all other errors print the output and raise the error
        try:
            print(e.output.decode())
        except Exception:
            pass
    else:
        print('PDF is Generated:'+tex_file_name)


def generate(aExitPath, aProtocol):
    lib_name = aProtocol["name"]
    _protocol = ps.generate_protocol(aProtocol)

    lib_path = os.path.join(aExitPath, lib_name)

    try:
        if os.path.exists(lib_path):
            shutil.rmtree(lib_path)
        os.mkdir(lib_path)
    except Exception:
        pass

    generate_source(lib_path, _protocol)

    # try:
    generate_doc(os.path.join(lib_path, "doc"), _protocol)
    # except Exception:
    #     pass
