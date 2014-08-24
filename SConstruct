env = Environment(
    CC='g++',
    CCFLAGS='-std=c++11 -pedantic -Wall -Wextra -Wno-switch -O3',
    CPPPATH='#/',
    LIBS=['SDL2', 'GL', 'GLEW']
)

source = Glob('src/*.cpp')
source += ['lib/lodepng/lodepng.cpp']

program = env.Program(target='bin/billboarding', source=source);
