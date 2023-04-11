tinyxml_srcs = [
    'src/tinyxml/tinystr.cpp',
    'src/tinyxml/tinyxml.cpp',
    'src/tinyxml/tinyxmlerror.cpp',
    'src/tinyxml/tinyxmlparser.cpp',
]

tinyxml_inc_dirs = [include_directories('./src')]

tinyxml = static_library(
    'tinyxml',
    sources: tinyxml_srcs,
    include_directories: tinyxml_inc_dirs,
    override_options: ['warning_level=0'],
)

tinyxml_dep = declare_dependency(
    link_with: tinyxml,
    include_directories: tinyxml_inc_dirs,
)
