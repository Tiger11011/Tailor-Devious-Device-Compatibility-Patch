{
    arch = "x64",
    artifacts = {
        installdir = [[<LOCAL_PATH_REMOVED>]]
    },
    configs = {
        android = false,
        builtin_math_operations = false,
        debug = false,
        dx10 = false,
        dx11 = true,
        dx12 = false,
        dx9 = false,
        freetype = true,
        glfw = false,
        glfw_opengl3 = false,
        glfw_vulkan = false,
        no_debug_tools = false,
        no_demo_windows = false,
        no_obsolete_functions = false,
        opengl2 = false,
        opengl3 = false,
        osx = false,
        pic = true,
        runtimes = "MD",
        sdl2 = false,
        sdl2_no_renderer = false,
        sdl2_opengl3 = false,
        sdl2_renderer = false,
        sdl3 = false,
        sdl3_gpu = false,
        sdl3_renderer = false,
        sdlrenderer = false,
        shared = false,
        volk = false,
        vulkan = false,
        vulkan_no_proto = false,
        wchar32 = false,
        wgpu = false,
        wgpu_backend = "wgpu",
        win32 = true
    },
    deps = {
        freetype = {
            buildhash = "0a1a367b029f453fa4ed7e1b3db0871e",
            version = "2.14.1"
        },
        zlib = {
            buildhash = "1087d98a25354760941affad70a541d9",
            version = "v1.3.2"
        }
    },
    description = "Bloat-free Immediate Mode Graphical User interface for C++ with minimal dependencies",
    envs = { },
    librarydeps = {
        "freetype",
        "zlib"
    },
    license = "MIT",
    mode = "release",
    name = "imgui",
    pathenvs = {
        "ACLOCAL_PATH",
        "CMAKE_PREFIX_PATH",
        "DYLD_LIBRARY_PATH",
        "LD_LIBRARY_PATH",
        "PATH",
        "PKG_CONFIG_PATH",
        "PYTHONPATH"
    },
    plat = "windows",
    repo = {
        branch = "master",
        commit = "bd29d77f242e247589457638272d9936a24a3fe0",
        name = "xmake-repo",
        url = "https://gitlab.com/tboox/xmake-repo.git"
    },
    vars = {
        includedirs = {
            "include",
            "include/imgui",
            "include/backends",
            "include/misc/cpp"
        },
        syslinks = "imm32"
    },
    version = "v1.92.6"
}