#!/usr/bin/env python
# coding=utf-8

# ***** BEGIN LICENSE BLOCK *****
# This file is part of LIBPF
# (C) Copyright 2015 Paolo Greppi simevo s.r.l.
# ***** END LICENSE BLOCK ***** */

""" j2.py: generate cc / h file from JSON DSL """

import sys
import jinja2
import json
import jsonschema
import datetime
import os
import shutil

def gen_source(aExitPath,model):

    lib_name = model["name"]

    templateLoader = jinja2.FileSystemLoader(searchpath='templates')
    templateEnv = jinja2.Environment(loader=templateLoader)

    def gen_file(aName, aTo='', aOutName=''):
        _file = templateEnv.get_template('{0}.tmpl'.format(aName))
        _file.globals['now'] = datetime.datetime.utcnow()
        _file_result = _file.render(model)
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

model_file = open('Simple.json', 'r')
model_json = model_file.read()
model_file.close()
model = json.loads(model_json)

aExitPath='e:/tmp/cmake_test_protocol'

gen_source(aExitPath,model)

"""
model_schema_file = open(sys.argv[2], 'r')
model_schema_json = model_schema_file.read()
model_schema_file.close()
model_schema = json.loads(model_schema_json)
jsonschema.validate(model, model_schema)
"""


