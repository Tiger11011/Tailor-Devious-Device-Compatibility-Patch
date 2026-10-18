{
    arch = "x64",
    artifacts = {
        installdir = [[<LOCAL_PATH_REMOVED>]]
    },
    configs = {
        debug = false,
        pic = true,
        runtimes = "MD",
        shared = false
    },
    deps = { },
    description = "DirectXMath is an all inline SIMD C++ linear algebra library for use in games and graphics apps.",
    envs = { },
    kind = "library",
    librarydeps = { },
    license = "MIT",
    mode = "release",
    name = "directxmath",
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
        includedirs = "include/directxmath"
    },
    version = "2024.02"
}