

cjson_dep = dependency('libcjson')

woth_inc_dir = include_directories('include') 

woth_dep = declare_dependency(
  include_directories: woth_inc_dir,
  dependencies: [cjson_dep],
)

subdir('samples')
