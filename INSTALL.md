# Prerequisites, Building, and Installation

## Prerequisites
The following prerequisites are required to build Pkg-Mgr:
    * A valid C++17 filesystem library
    * A valid C++ compiler which supports the C++17 standard
    * A valid libc compliant with the C++17 standard
    * A valid, usable "make" installation
    * libarchive

Eventually, most items (particularly the C++17 and stdc++fs support) will have alternatives more widely available on various machines. For now, these are all strict requirements.

## Building & Installation
Once all of the prereqs are met, you can run
    ./configure

If all of the prereqs are installed, there should be no error messages. If so, you can run the following command to build Pkg-Mgr:
    make

And to install it,
    make install

### Build Environment Variables
The default options for your build can be changed at compile-time. The following options can be set as exported environment variables to change the default options for your build:
    DEFAULT_VERBOSITY = The default verbosity the program should use
    DEFAULT_SMART_OP = Currently unimplemented; Once implemented, this will check for file-collision when uninstalling packages, and verify that no files within the package already exist within the filesystem before continuting.
    DEFAULT_TAR_LIBRARY_PATH = The default path at which the program should look for packages
    DEFAULT_INSTALLED_PKG_PATH = The default path at which the program should store and look for index files, such that it knows what is and is not installed.
    DEFAULT_GLOBAL_CONFIG_PATH = The default path at which the program should look for the global configuration file. This is an absolute path.
    DEFAULT_USER_CONFIG_PATH = The default path at which the program should look for user-specific configuration files. This is relative to the user's home directory.
    DEFAULT_SYSTEM_ROOT = The root folder which the program should install packages to or uninstall packages from.
    DEFAULT_EXCLUDED_FILES = Currently unimplemented; Once implemented, this will determine filepaths which should be ignored while installing or uninstalling packages.
