zipfs_inc_dir = include_directories('src/zipFS')

hge_srcs = [
    'src/hge/hgecolor.cpp',
    'src/hge/hgeparticle.cpp',
    'src/hge/hgerect.cpp',
    'src/hge/hgevector.cpp',
    'src/hge/hgedistort.cpp',
    'src/hge/hgefont.cpp',
]

hge_c_args = []
hge_cpp_args = []
hge_inc_dirs = [zipfs_inc_dir]
hge_dependencies = []

if host_machine.system() == 'psp'
    hge_c_args = ['-D_PSP_FW_VERSION=150']
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
    'src/Vector2D.cpp',
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
    'src/tinyxml/tinystr.cpp',
    'src/tinyxml/tinyxml.cpp',
    'src/tinyxml/tinyxmlerror.cpp',
    'src/tinyxml/tinyxmlparser.cpp',
]

jge_c_args = []
jge_cpp_args = []
jge_inc_dirs = [include_directories('./include'), zipfs_inc_dir]

png_dep = dependency('libpng')
jpeg_dep = dependency('libjpeg')

jge_dependencies = [hge_dep, png_dep, jpeg_dep]

if host_machine.system() == 'psp'
    boost_inc_dir = include_directories('../Boost')
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
            'src/main.cpp',
            'src/vram.c',
        ]
    )

    jge_c_args = jge_c_args + ['-D_PSP_FW_VERSION=371']
    jge_cpp_args = jge_cpp_args + ['-DDEVHOOK', '-DPSPFW3XX', '-DPSP']
    jge_inc_dirs = (jge_inc_dirs + [boost_inc_dir])
    jge_dependencies = jge_dependencies + [freetype_dep]
else
    jge_srcs = (
        jge_srcs + [
            'src/SDLmain.cpp',
            'src/JDistortionMesh.cpp',
            'src/pc/JSfx.cpp',
            'src/pc/JGfx.cpp',
            'src/pc/JSocket.cpp',
        ]
    )
    sdl2_dep = dependency('sdl2')
    boost_dep = dependency('boost', modules: ['system', 'date_time', 'thread'])

    jge_c_args = jge_c_args + ['-DSDL_CONFIG']

    if host_machine.system() == 'linux'
        opengl_dep = dependency('OpenGL')
        jge_c_args = jge_c_args + ['-DLINUX']
        jge_dependencies = jge_dependencies + [opengl_dep]
    elif host_machine.system() == 'windows'
        jge_c_args = jge_c_args + ['-DWIN32']
    endif

    jge_cpp_args = jge_c_args
    jge_dependencies = jge_dependencies + [sdl2_dep, boost_dep]
endif

if get_option('buildtype') == 'debug'
    jge_cpp_args = jge_cpp_args + ['-ggdb3']
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

jge_dep = declare_dependency(
    link_with: jgetools,
    include_directories: jge_inc_dirs,
    dependencies: jge_dependencies,
)
