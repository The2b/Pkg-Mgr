/**
 * @author Thomas Lenz <thomas.lenz96@gmail.com> AS The2b
 * @date 10 November 2018
 * @project Package Manager
 * @file Pkg.h
 */

#ifndef _THE2B_PKG_H
#define _THE2B_PKG_H

#include <stdlib.h>     // exit
#include <stdio.h>      // printf, fprintf
#include <errno.h>      // errno, strerror
#include <string>       // std::string
#include <string.h>     // strcpy
#include <vector>       // vectors
#include <filesystem>   // C++17 filesystem
#include <fstream>      // ofstream
#include <unistd.h>     // lseek

#include <fcntl.h>      // O_RDONLY

#include <set>          // Sets
#include <archive.h>
#include <archive_entry.h>

#include "Config.h"
#include "Options.h"

// A tar file has a blocksize of 512
#define TAR_BLOCKSIZE 512

// I could not find these definitions in my header files, so I'll put it here. I may need to move this later, however, so don't rely on it
#define likely(x) __builtin_expect(static_cast<bool>(x), 1)
#define unlikely(x) __builtin_expect(static_cast<bool>(x), 0)

class Pkg {
    friend bool operator ==(const Pkg& a, const Pkg& b);
    
    private:
        // Read and write streams, respectively
        std::string pathname;
        std::string pkgName;

        // This will be a list of files within the tar file
        std::set<std::string> buildPkgContents(unsigned int verbosity = DEFAULT_VERBOSITY);

    public:
        // Declare our functions
        Pkg(std::string path, unsigned int verbosity = DEFAULT_VERBOSITY);
        std::string getPathname();
        std::string getPkgName();
        int installPkg(std::string tarPath, std::string root = DEFAULT_SYSTEM_ROOT, std::string installedPkgsPath = DEFAULT_INSTALLED_PKG_PATH, unsigned int verbosity = DEFAULT_VERBOSITY, std::set<std::string> exclusions = std::set<std::string>{}, bool quick = DEFAULT_SMART_OP);
        int uninstallPkg(std::set<std::string> pkgContents, std::string root = DEFAULT_SYSTEM_ROOT, std::string installedPkgsPath = DEFAULT_INSTALLED_PKG_PATH, unsigned int verbosity = DEFAULT_VERBOSITY, std::set<std::string> exclusions = std::set<std::string>{}, bool quick = DEFAULT_SMART_OP);

        // The following functions call their overloads with the appropriate member vars (tarR, tarHeaderVector, pkgContents)
        int installPkg(std::string root = DEFAULT_SYSTEM_ROOT, std::string installedPkgsPath = DEFAULT_INSTALLED_PKG_PATH, unsigned int verbosity = DEFAULT_VERBOSITY, std::set<std::string> exclusions = std::set<std::string>{}, bool quick = DEFAULT_SMART_OP);
        int uninstallPkg(std::string root = DEFAULT_SYSTEM_ROOT, std::string installedPkgsPath = DEFAULT_INSTALLED_PKG_PATH, unsigned int verbosity = DEFAULT_VERBOSITY, std::set<std::string> exclusions = std::set<std::string>{}, bool quick = DEFAULT_SMART_OP);
        bool followPkg(std::string installedPkgDir = DEFAULT_INSTALLED_PKG_PATH, unsigned int verbosity = DEFAULT_VERBOSITY);
        bool unfollowPkg(std::string installedPkgDir = DEFAULT_INSTALLED_PKG_PATH, unsigned int verbosity = DEFAULT_VERBOSITY);
};

bool listAllPkgs(std::string libraryPath, unsigned int verbosity = DEFAULT_VERBOSITY);
bool listInstalledPkgs(std::string installedPkgsPath, unsigned int verbosity = DEFAULT_VERBOSITY);

#endif /* _THE2B_PKG_H */
