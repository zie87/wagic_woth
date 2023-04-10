woth_inc_dir = include_directories('include')

woth_dep = declare_dependency(
    include_directories: woth_inc_dir,
)

subdir('tests')
if get_option('build_docs')
    subdir('docs')
endif
