from distutils.core import setup, Extension, DEBUG

sfc_module = Extension('simple_graphs', sources = ['Source.cpp'])

setup(name = 'simple_graphs', version = '1.0',
    description = 'Python Package with superfastcode C++ extension',
    ext_modules = [sfc_module]
    )

