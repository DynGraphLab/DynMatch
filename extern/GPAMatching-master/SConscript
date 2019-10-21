#
import platform
import sys

# Get the current platform.
SYSTEM = platform.uname()[0]

Import('env')

# Build a library from the code in lib/.
libgpa_matching = [
                      'lib/data_structure/graph_access.cpp',
                      'lib/matching/gpa_matching.cpp',
                      'lib/matching/path_set.cpp',
                      'lib/matching/path.cpp',
                      'lib/tools/random_functions.cpp',
                     ]

if env['program'] == 'gpa_matching':
        env.Program('gpa_matching', ['app/gpa_matching.cpp']+libgpa_matching, LIBS=['libargtable2'])

