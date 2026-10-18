{
    arch = "x64",
    artifacts = {
        installdir = [[<LOCAL_PATH_REMOVED>]],
        remotedir = [[<LOCAL_PATH_REMOVED>]]
    },
    configs = {
        bzip2 = false,
        debug = false,
        harfbuzz = false,
        pic = true,
        png = false,
        runtimes = "MD",
        shared = false,
        woff2 = false,
        zlib = true
    },
    deps = {
        zlib = {
            buildhash = "1087d98a25354760941affad70a541d9",
            version = "v1.3.2"
        }
    },
    description = "A freely available software library to render fonts.",
    envs = { },
    librarydeps = {
        "zlib"
    },
    license = "BSD",
    mode = "release",
    name = "freetype",
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
        includedirs = "include/freetype2"
    },
    version = "2.14.1"
}