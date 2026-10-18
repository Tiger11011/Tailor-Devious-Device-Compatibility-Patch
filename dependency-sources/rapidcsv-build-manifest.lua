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
    description = "C++ header-only library for CSV parsing (by d99kris)",
    envs = { },
    kind = "library",
    license = "BSD-3-Clause",
    mode = "release",
    name = "rapidcsv",
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
    version = "v8.92"
}