#!/usr/bin/env python
# coding=utf-8

import sys
import jinja2
import json
import jsonschema
import datetime
import os
import shutil
from generate_source import generate_source


model_file = open('EasyProtocol.json', 'r')
model_json = model_file.read()
model_file.close()
model = json.loads(model_json)


aExitPath = 'e:/tmp/cmake_test_protocol'


model_schema_file = open('schemas/protocol_schema.json', 'r')
model_schema_json = model_schema_file.read()
model_schema_file.close()
model_schema = json.loads(model_schema_json)
jsonschema.validate(model, model_schema)

generate_source(aExitPath, model)


