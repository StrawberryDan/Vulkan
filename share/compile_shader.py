#!/usr/bin/env python3

import sys
import subprocess
import tempfile


def compile_shader(glslc_executable, source, header):
    binary_file = tempfile.NamedTemporaryFile(delete=False)

    command = f"{glslc_executable} {source} -o {binary_file.name}"
    print(command)
    glsl = subprocess.run(command, shell=True)
    if glsl.returncode != 0:
        sys.exit(glsl.returncode)

    program = binary_file.read()
    print(program)

    output = open(header, "w")

    for byte in program:
        output.write(f"{byte:#04x}, ")

    binary_file.close()

    pass


compile_shader(sys.argv[1], sys.argv[2], sys.argv[3])
