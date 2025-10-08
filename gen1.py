import os
import re

base = os.path.dirname(__file__)
path = os.path.join(base, "src/mysqldumpsettings.h")

to_json = []
from_json = []

def ser(type_, name):
    if type_ == 'QStringList':
        return f'fromStringList({name})'
    return name

ENUMS = set()

def deser(type_, name, enums):
    if type_ == 'QStringList':
        return f'toStringList(obj["{name}"])'
    
    if type_ in enums:
        fn = 'toInt'
    else:
        fn = {
            'bool': 'toBool',
            'QString': 'toString',
        }.get(type_, 'toInt')

    dvType = {
        'bool': 'true',
        'QString': ''
    }.get(type_, '0')   

    dvName = {
        'completeInsert': 'false',
        'insertIgnore': 'false',
        'hexBlob': 'false'
    }.get(name, dvType)

    if type_ in enums:
        conv = f'({type_})'
    else:
        conv = ''

    return f'{conv} obj["{name}"].{fn}({dvName})'.strip()

with open(path, encoding='utf-8') as f:
    for line in f:
        m = re.match('^\\s*([^{()}]+)\\s+([^{()}]+)\\s*;\\s*$', line)
        if m:
            #print(m.group(0).strip())
            type_ = m.group(1).strip()
            if type_ == "class":
                continue
            name = m.group(2).strip()
            # obj["format"] = format;
            to_json.append(f'obj["{name}"] = {ser(type_, name)};')
            from_json.append(f'res.{name} = {deser(type_, name, ENUMS)};')
            continue
        m = re.match('^\\s*enum\\s*([a-z0-9A-Z_]+)', line)
        if m:
            ENUMS.add(m.group(1))

def to_json_lines(cn, body):
    yield '// generated'
    yield f"QJsonObject {cn}::toJson() const {{"
    yield f'QJsonObject obj;'
    for line in body:
        yield line
    yield "return obj;\n}\n"

def from_json_lines(cn, body):
    yield '// generated'
    yield f"{cn} {cn}::fromJson(const QJsonObject& obj) {{"
    yield f'{cn} res;'
    for line in body:
        yield line
    yield "return res;\n}\n"

with open("D:/w/tmp1.cpp", "w", encoding='utf-8') as f:
    cn = "MysqldumpSettings"
    for line in to_json_lines(cn, to_json):
        print(line, file=f)
    for line in from_json_lines(cn, from_json):
        print(line, file=f)
