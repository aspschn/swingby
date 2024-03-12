#!/usr/bin/env python3

import os

if __name__ == '__main__':
    shaders_h = '''#ifndef _FOUNDATION_SHADERS
#define _FOUNDATION_SHADERS

'''

    files = os.listdir('shaders')

    print(files)

    for file in files:
        f = open(f'shaders/{file}', 'r')
        content = f.read()

        name = file.replace('.', '_')
        shaders_h += f'static const char *{name}_shader =\n'
        for line in content.split('\n'):
            shaders_h += f'"{line}\\n"\n'
        shaders_h += ';\n\n'

        f.close()

    shaders_h += '#endif\n'

    print(shaders_h)

    f = open('src/shaders.h', 'w')
    f.write(shaders_h)
    f.close()

