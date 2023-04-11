unity_cpp_dir = include_directories('include')

unity_dep = dependency('unity')

unity_cpp_lib = static_library(
    'unity_cpp',
    sources: ['src/unity_fixture.c'],
    include_directories: unity_cpp_dir,
    dependencies: [unity_dep],
    c_args: ['-DUNITY_FIXTURE_NO_EXTRAS']
)

unity_cpp_dep = declare_dependency(
    link_with: unity_cpp_lib,
    include_directories: unity_cpp_dir,
    dependencies: [unity_dep],
    compile_args: ['-DUNITY_FIXTURE_NO_EXTRAS']
)
