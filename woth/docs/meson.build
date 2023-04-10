doxygen = find_program('doxygen', required: false)

if doxygen.found()

    cdata = configuration_data()
    cdata.set('VERSION', meson.project_version())

    if find_program('dot', required: false).found()
        cdata.set('HAVE_DOT', 'YES')
    else
        cdata.set('HAVE_DOT', 'NO')
    endif

    cdata.set('PROJECT_NAME', 'Wagic, the Homebrew!?')
    cdata.set(
        'PROJECT_SRCDIR',
        join_paths(meson.current_source_dir(), '..', 'include'),
    )
    cdata.set('PROJECT_DOCDIR', meson.current_source_dir())
    cdata.set('PROJECT_BUILDDIR', meson.build_root())
    cdata.set('PROJECT_OUTPUTDIR', meson.current_build_dir())

    doxyfile = configure_file(
        input: 'Doxyfile.in',
        output: 'Doxyfile',
        configuration: cdata,
        install: false,
    )

    datadir = join_paths(get_option('datadir'), 'doc', 'tmp')

    html_target = custom_target(
        'doc',
        build_always_stale: not meson.is_subproject(),
        input: doxyfile,
        output: 'html',
        command: [doxygen, doxyfile],
        install: not meson.is_subproject(),
        install_dir: datadir,
    )

endif
