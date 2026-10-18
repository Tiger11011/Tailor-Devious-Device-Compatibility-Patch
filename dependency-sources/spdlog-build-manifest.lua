{
    arch = "x64",
    artifacts = {
        installdir = [[<LOCAL_PATH_REMOVED>]]
    },
    configs = {
        debug = false,
        fmt_external = false,
        fmt_external_ho = false,
        header_only = false,
        noexcept = false,
        pic = true,
        runtimes = "MD",
        shared = false,
        std_format = true,
        thread_id = true,
        tls = true,
        wchar = true,
        wchar_console = false,
        wchar_filenames = false
    },
    deps = { },
    description = "Fast C++ logging library.",
    envs = { },
    librarydeps = { },
    license = "MIT",
    mode = "release",
    name = "spdlog",
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
        defines = {
            "SPDLOG_COMPILED_LIB",
            "SPDLOG_USE_STD_FORMAT",
            "SPDLOG_WCHAR_TO_UTF8_SUPPORT"
        }
    },
    version = "v1.16.0"
}