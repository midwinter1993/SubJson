#!/usr/bin/env python
# -*- coding: utf-8 -*-

import json

d = {
    "a": 1,
    "bb": "who is bb",
    "ccc": {
        "ccc-a": [111, 333],
        "ccc-bb": "in nested"
    },
    "dddd": ["dddd-a", "dddd-b"]
}

with open('./data.json', 'w') as fd:
    json.dump(d, fd, indent=4)
