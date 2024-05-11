cpmaddpackage(
    NAME
    CLI11
    GITHUB_REPOSITORY
    CLIUtils/CLI11
    VERSION
    2.3.2
    OPTIONS
    "CLI11_PRECOMPILED ON"
    EXCLUDE_FROM_ALL
    TRUE
    SYSTEM
    TRUE)

cpmaddpackage(
    NAME
    googletest
    GITHUB_REPOSITORY
    google/googletest
    VERSION
    1.14.0
    OPTIONS
    "INSTALL_GTEST OFF"
    "BUILD_GMOCK OFF")
