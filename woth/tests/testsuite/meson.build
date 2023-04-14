wth_ts_srcs = ['src/test/wagic_wrapper.cpp']

wth_ts_inc_dir = include_directories('./include')
wth_ts_c_args = []
wth_ts_cpp_args = ['-DTESTSUITE=1']

if host_machine.system() == 'psp'
    wth_ts_srcs = wth_ts_srcs + ['src/main_psp.cpp']
else
    wth_ts_srcs = wth_ts_srcs + ['src/main_sdl.cpp']
endif

wth_ts_name = 'wagic_testsuite'
wth_ts = executable(
    wth_ts_name,
    sources: wth_ts_srcs,
    c_args: wth_ts_c_args,
    cpp_args: wth_ts_cpp_args,
    override_options: ['warning_level=0'],
    include_directories: [wth_ts_inc_dir],
    dependencies: [wthlib_dep, jge_dep],
)

wth_ts_workdir = join_paths(
    meson.project_source_root(),
    'woth',
    'bin',
)

test('woth testsuite', wth_ts, workdir: wth_ts_workdir)

if host_machine.system() == 'psp'
    post_build = meson.get_external_property('post_build_script')
    post_build_script = '@0@'.format(
        find_program(meson.source_root() + '/tools/' + post_build).path(),
    )
    custom_target(
        'wth_ts_post_build',
        input: wth_ts,
        output: 'wth_ts_post_build',
        command: [post_build_script, '@INPUT@'],
        build_by_default: true,
        depends: [wth_ts],
    )
endif
