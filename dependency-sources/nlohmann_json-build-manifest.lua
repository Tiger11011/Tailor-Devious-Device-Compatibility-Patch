{
    arch = "x64",
    artifacts = {
        installdir = [[<LOCAL_PATH_REMOVED>]]
    },
    configs = {
        cmake = false,
        debug = false,
        pic = true,
        runtimes = "MD",
        shared = false
    },
    description = "JSON for Modern C++",
    envs = { },
    kind = "library",
    license = "MIT",
    mode = "release",
    name = "nlohmann_json",
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
    version = "v3.12.0"
}