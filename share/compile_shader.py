# coding: utf8

import sys
import subprocess
import tempfile


def compile_shader(glslc_executable, source, header):
    print(glslc_executable, source, header)
    binary_file = tempfile.NamedTemporaryFile(delete=False)
    glsl = subprocess.run([glslc_executable, source, "-o", binary_file.name], shell=True)

    program = binary_file.read()
    print(program)

    output = open(header, "w")

    for byte in program:
        output.write(f"{byte:#04x}, ")

    binary_file.close()

    pass


compile_shader(sys.argv[1], sys.argv[2], sys.argv[3])
