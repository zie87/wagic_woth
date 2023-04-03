psp_inc_dir = include_directories('include/psp')
freetype2_inc_dir = include_directories('include/psp/freetype2')
zipfs_inc_dir = include_directories('src/zipFS')

hge_srcs = [
    'src/hge/hgecolor.cpp',
    'src/hge/hgeparticle.cpp',
    'src/hge/hgerect.cpp',
    'src/hge/hgevector.cpp',
    'src/hge/hgedistort.cpp',
    'src/hge/hgefont.cpp',
]

hge_c_args    = ['-D_PSP_FW_VERSION=150']
hge_cpp_args  = hge_c_args

hgetools = static_library(
    'hgetools',
    sources: hge_srcs,
    include_directories: [psp_inc_dir, freetype2_inc_dir, zipfs_inc_dir],
    c_args: hge_c_args,
    cpp_args: hge_cpp_args,
)

hge_dep = declare_dependency(
    link_with: hgetools,
    include_directories: [psp_inc_dir, freetype2_inc_dir, zipfs_inc_dir],
)

boost_inc_dir = include_directories('../Boost')

jge_srcs = [
'src/JApp.cpp',
'src/JGBKFont.cpp',
'src/JGE.cpp',
'src/JGui.cpp',
'src/JLBFont.cpp',
'src/JLogger.cpp',
'src/JGameObject.cpp',
'src/JSpline.cpp',
'src/JAnimator.cpp',
'src/JResourceManager.cpp',
'src/JFileSystem.cpp',
'src/JNetwork.cpp',
'src/JParticle.cpp',
'src/JParticleEmitter.cpp',
'src/JParticleEffect.cpp',
'src/JParticleSystem.cpp',
'src/zipFS/zfsystem.cpp',
'src/zipFS/ziphdr.cpp',
'src/zipFS/zstream.cpp',
'src/JSprite.cpp',
'src/Vector2D.cpp',
'src/tinyxml/tinystr.cpp',
'src/tinyxml/tinyxml.cpp',
'src/tinyxml/tinyxmlparser.cpp',
'src/tinyxml/tinyxmlerror.cpp',
'src/Encoding.cpp',
'src/JTTFont.cpp',
'src/JMD2Model.cpp',
'src/JOBJModel.cpp',
'src/JSocket.cpp',
'src/JGfx.cpp',
'src/JSfx.cpp',
'src/JAudio.cpp',
'src/JMP3.cpp',
'src/decoder_prx.c',
'src/main.cpp',
'src/vram.c',
]

jge_inc_dir = include_directories('./include')
jge_c_args    = ['-D_PSP_FW_VERSION=371']
jge_cpp_args  = jge_c_args + ['-std=c++03', '-DDEVHOOK', '-DPSPFW3XX', '-DPSP']

png_dep = dependency('libpng')
jpeg_dep = dependency('libjpeg')

jge_dependencies = [hge_dep, png_dep, jpeg_dep]

if get_option('buildtype') == 'debug'
    jge_cpp_args = jge_cpp_args + ['-ggdb3']
endif

jgetools = static_library(
    'jge300',
    sources: jge_srcs,
    include_directories: [psp_inc_dir, freetype2_inc_dir, boost_inc_dir, zipfs_inc_dir],
    override_options: ['cpp_std=c++03', 'warning_level=0'],
    c_args: jge_c_args,
    cpp_args: jge_cpp_args,
    dependencies: jge_dependencies,
)

jge_dep = declare_dependency(
    link_with: jgetools,
    include_directories: [psp_inc_dir, freetype2_inc_dir, boost_inc_dir, zipfs_inc_dir, jge_inc_dir],
    dependencies: jge_dependencies,
)
