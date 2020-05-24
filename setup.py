from distutils.core import setup, Extension
import sys
import numpy

# compile the c++ extension
_sources = [
    'player.cc',
    'board.cc',
    'game.cc',
    'renderer.cc',
    'controller.cc',
    'utils.cc',
    'py_module.cc'
]
sources = ["src/src_cpp/" + filename for filename in _sources]
sources += [
    'src/SDL2_gfx-1.0.4/SDL2_gfxPrimitives.c'
]

if sys.platform == 'win32':
    library_dirs = []
else:
    library_dirs = ['~/lib', '/usr/local/lib', 'usr/lib']

module1 = Extension('gym_hexario.game_module',
                    language='c++',
                    include_dirs=[
                        'src/src_cpp/include',
                        'src/SDL2_gfx-1.0.4/',
                        numpy.get_include()],

                    libraries=['SDL2'],
                    library_dirs=library_dirs,
                    sources=sources)

setup(
    name='gym_hexario',
    version='0.0.1',
    description='gym environment for the game Hexar.io',
    author='David Hansmair',
    python_requires='>3.5.2',
    install_requires=['numpy', 'gym'],
    package_dir={'gym_hexario': 'src/gym_hexario'},
    packages=['gym_hexario', 'gym_hexario.envs'],
    ext_modules=[module1]
)
