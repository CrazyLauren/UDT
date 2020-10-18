#!/usr/bin/env python
# coding=utf-8

import sys
import json
import jsonschema
from py.source_generator import generate


def read_protocol(aPath):
    model_file = open(aPath, 'r', encoding='utf-8')
    model_json = model_file.read()
    model_file.close()
    model = json.loads(model_json)

    model_schema_file = open('schemas/protocol_schema.json', 'r', encoding='utf-8')
    model_schema_json = model_schema_file.read()
    model_schema_file.close()
    model_schema = json.loads(model_schema_json)
    jsonschema.validate(model, model_schema)

    return model


if len(sys.argv) != 3:
    print("usage: %s protocol.json output Path " % sys.argv[0])
    sys.exit(-1)

generate(sys.argv[2], read_protocol(sys.argv[1]))
