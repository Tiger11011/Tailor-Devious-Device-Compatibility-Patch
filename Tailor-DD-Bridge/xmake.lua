set_xmakever('3.0.1')
includes('../Tailor-DD-Compat/lib/commonlibsse-ng')
set_project('TailorDDBridge')
set_version('0.4.0')
set_license('GPL-3.0-or-later')
set_languages('clatest', 'c++23')
set_encodings('utf-8')
add_rules('mode.debug', 'mode.release')
add_requires('nlohmann_json v3.12.0')
add_requires('imgui v1.92.6', {configs = {dx11 = true, win32 = true, freetype = true}})

target('minhook-local')
    set_kind('static')
    add_files('lib/minhook-1.3.4/src/*.c', 'lib/minhook-1.3.4/src/hde/hde64.c')
    add_includedirs('lib/minhook-1.3.4/include', {public = true})

target('TailorDDBridge')
    add_deps('commonlibsse-ng', 'minhook-local')
    add_packages('nlohmann_json', 'imgui')
    add_defines('NOMINMAX', 'WIN32_LEAN_AND_MEAN')
    add_syslinks('bcrypt', 'd3d11', 'dxgi', 'd3dcompiler')
    add_rules('commonlibsse-ng.plugin', {
        name = 'TailorDDBridge', author = 'Tailor DD compatibility project',
        description = 'Independent Tailor DD fitting compatibility (experimental).',
        options = {address_library = true, struct_dependent = false}
    })
    add_includedirs('src')
    add_files('src/*.cpp')
    set_pcxxheader('src/PCH.h')
