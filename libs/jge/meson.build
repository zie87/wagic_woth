zipfs_inc_dir = include_directories('src/zipFS')

hge_srcs = [
    'src/hge/hgecolor.cpp',
    'src/hge/hgeparticle.cpp',
    'src/hge/hgerect.cpp',
    'src/hge/hgedistort.cpp',
    'src/hge/hgefont.cpp',
]

hge_c_args = []
hge_cpp_args = []
hge_inc_dirs = [include_directories('./include'), zipfs_inc_dir]
hge_dependencies = []

if host_machine.system() == 'psp'
    hge_c_args = ['-DPSP', '-D_PSP_FW_VERSION=150']
    hge_cpp_args = hge_c_args
else
    sdl2_dep = dependency('sdl2')

    hge_c_args = hge_c_args + ['-DSDL_CONFIG']

    if host_machine.system() == 'linux'
        hge_c_args = hge_c_args + ['-DLINUX']
    elif host_machine.system() == 'windows'
        hge_c_args = hge_c_args + ['-DWIN32']
    endif

    hge_cpp_args = hge_c_args
    hge_dependencies = [sdl2_dep]
endif

hgetools = static_library(
    'hgetools',
    sources: hge_srcs,
    include_directories: hge_inc_dirs,
    c_args: hge_c_args,
    cpp_args: hge_cpp_args,
    dependencies: hge_dependencies,
)

hge_dep = declare_dependency(
    link_with: hgetools,
    include_directories: hge_inc_dirs,
    dependencies: hge_dependencies,
)


jge_srcs = [
    'src/JApp.cpp',
    'src/JGE.cpp',
    'src/JGui.cpp',
    'src/JLBFont.cpp',
    'src/JLogger.cpp',
    'src/Encoding.cpp',
    'src/JGameObject.cpp',
    'src/JSprite.cpp',
    'src/JMD2Model.cpp',
    'src/JOBJModel.cpp',
    'src/JAnimator.cpp',
    'src/JSpline.cpp',
    'src/JParticle.cpp',
    'src/JParticleEmitter.cpp',
    'src/JParticleEffect.cpp',
    'src/JParticleSystem.cpp',
    'src/JResourceManager.cpp',
    'src/JFileSystem.cpp',
    'src/JNetwork.cpp',
    'src/zipFS/zfsystem.cpp',
    'src/zipFS/ziphdr.cpp',
    'src/zipFS/zstream.cpp',
]

jge_main_srcs = []

jge_c_args = []
jge_cpp_args = []
jge_inc_dirs = [include_directories('./include'), zipfs_inc_dir]

png_dep = dependency('libpng')
jpeg_dep = dependency('libjpeg')

jge_dependencies = [tinyxml_dep, hge_dep, png_dep, jpeg_dep]

if host_machine.system() == 'psp'
    freetype_dep = dependency('freetype2')
    jge_srcs = (
        jge_srcs + [
            'src/JGBKFont.cpp',
            'src/JTTFont.cpp',
            'src/JSocket.cpp',
            'src/JGfx.cpp',
            'src/JSfx.cpp',
            'src/JAudio.cpp',
            'src/JMP3.cpp',
            'src/decoder_prx.c',
            'src/vram.c',
        ]
    )

    jge_main_srcs = (
        jge_main_srcs + [
            'src/main.cpp',
        ]
    )

    jge_c_args = jge_c_args + ['-D_PSP_FW_VERSION=371']
    jge_cpp_args = jge_cpp_args + ['-DDEVHOOK', '-DPSPFW3XX', '-DPSP']
    jge_dependencies = jge_dependencies + [freetype_dep]
else
    jge_srcs = (
        jge_srcs + [
            'src/JDistortionMesh.cpp',
            'src/pc/JSfx.cpp',
            'src/pc/JGfx.cpp',
            'src/pc/JSocket.cpp',
        ]
    )

    jge_main_srcs = (
        jge_main_srcs + [
            'src/SDLmain.cpp',
        ]
    )

    sdl2_dep = dependency('sdl2')
    thread_dep = dependency('threads')

    jge_c_args = jge_c_args + ['-DSDL_CONFIG']

    if host_machine.system() == 'linux'
        opengl_dep = dependency('OpenGL')
        jge_c_args = jge_c_args + ['-DLINUX']
        jge_dependencies = jge_dependencies + [opengl_dep]
    elif host_machine.system() == 'windows'
        jge_c_args = jge_c_args + ['-DWIN32']
    endif

    jge_cpp_args = jge_c_args
    jge_dependencies = jge_dependencies + [sdl2_dep, thread_dep]
endif

if get_option('buildtype') == 'debug'
    jge_cpp_args = jge_cpp_args + ['-DDOLOG=1', '-ggdb3']
endif

jgetools = static_library(
    'jge300',
    sources: jge_srcs,
    include_directories: jge_inc_dirs,
    override_options: ['warning_level=0'],
    c_args: jge_c_args,
    cpp_args: jge_cpp_args,
    dependencies: jge_dependencies,
)

jgemain = static_library(
    'jgemain',
    sources: jge_main_srcs,
    include_directories: jge_inc_dirs,
    override_options: ['warning_level=0'],
    c_args: jge_c_args,
    cpp_args: jge_cpp_args,
    dependencies: jge_dependencies,
)

jge_dep = declare_dependency(
    link_with: jgetools,
    include_directories: jge_inc_dirs,
    dependencies: jge_dependencies,
)

jgemain_dep = declare_dependency(
    link_with: [jgetools, jgemain],
    include_directories: jge_inc_dirs,
    dependencies: jge_dependencies,
)

subdir('tests')
if get_option('build_docs')
    subdir('docs')
endif
