/**
 * @author Thomas Lenz <thomas.lenz96@gmail.com> AS The2b
 * @date 12 November 2018
 * @project Package Manager
 * @file Options.h
 * @error -500
 */

#ifndef _THE2B_OPTIONS_H
#define _THE2B_OPTIONS_H

#include <stdlib.h>     // getenv
#include <getopt.h>     // Processing command line flags
#include <cmath>        // pow; Using cmath instead of math.h because it has additional overloads that are more efficient
#include <string>       // strings
#include <set>          // sets
#include <filesystem>   // exists
#include <system_error>

#include "Config.h"
#include "config.h"

// The following directives are a last-resort fail-safe
// If it ever gets to the point that these are being used, they won't respect autoconf's prefix
// That being said, it shouldn't ever get to this point, unless someone deliberately attempts to break something
#ifndef DEFAULT_VERBOSITY
#define DEFAULT_VERBOSITY 2
#endif /* DEFAULT_VERBOSITY */

#ifndef DEFAULT_SMART_OP
#define DEFAULT_SMART_OP true
#endif /* DEFAULT_SMART_OP */

#ifndef DEFAULT_GLOBAL_CONFIG_PATH
#define DEFAULT_GLOBAL_CONFIG_PATH "/etc/pkg-mgr.conf"
#endif /* DEFAULT_GLOBAL_CONFIG_PATH */

// Remember this one is relative to each user's home directory
#ifndef DEFAULT_USER_CONFIG_PATH
#define DEFAULT_USER_CONFIG_PATH "/.config/pkg-mgr.conf"
#endif /* DEFAULT_USER_CONFIG_PATH */

#ifndef DEFAULT_SYSTEM_ROOT
#define DEFAULT_SYSTEM_ROOT "/"
#endif /* DEFAULT_SYSTEM_ROOT */

#ifndef DEFAULT_TAR_LIBRARY_PATH
#define DEFAULT_TAR_LIBRARY_PATH "/var/lib/pkg-mgr/pkgs/"
#endif /* DEFAULT_TAR_LIBRARY_PATH */

#ifndef DEFAULT_INSTALLED_PKG_PATH
#define DEFAULT_INSTALLED_PKG_PATH "/var/lib/pkg-mgr/pkgs/"
#endif /* DEFAULT_INSTALLED_PKG_PATH */

#ifndef DEFAULT_EXCLUDED_FILES
#define DEFAULT_EXCLUDED_FILES {}
#endif /* DEFAULT_EXCLUDED_FILES */

#define DEFAULT_OPT_MASK 0

// Modes of operation
#define INSTALL 0
#define UNINSTALL 1
#define FOLLOW 2
#define UNFOLLOW 3
#define LIST_ALL 4
#define LIST_INSTALLED 5
#define SEARCH 6
#define OWNER 7
#define IMPORT 8
#define PURGE 9
#define NOP 99
#define NOP_KEY "NONE_OF_THE_ABOVE"

// Mask bits
#define MASK_VERBOSE 1
#define MASK_SMART_OP 2
#define MASK_MODE 4
#define MASK_GLOBAL_CONFIG_PATH 8
#define MASK_USER_CONFIG_PATH 16
#define MASK_SYSTEM_ROOT 32
#define MASK_TAR_LIBRARY_PATH 64
#define MASK_INSTALLED_PKG_PATH 128
#define MASK_EXCLUDED_FILES 256
// The number of bits the mask uses
#define MASK_SIZE 9

struct mode_s {
    unsigned int modeIndex = NOP;
    std::string modeStr = NOP_KEY;

    bool operator<(const mode_s& m) const {
        return modeIndex < m.modeIndex;
    }

    bool operator>(const mode_s& m) const {
        return modeIndex > m.modeIndex;
    }

    bool operator==(const mode_s& m) const {
        return (modeIndex == m.modeIndex) && (modeStr == m.modeStr);
    }
};

class Options {
    private:
        unsigned int optMask;
        unsigned int verbosity;
        bool smartOperation;
        mode_s mode = { NOP, NOP_KEY };
        std::string globalConfigPath;
        std::string userConfigPath;
        std::string systemRoot;
        std::string tarLibraryPath;
        std::string installedPkgsPath;
        std::set<std::string> excludedFiles;
        
        // Takes a string mode and returns the proper mode integer
        unsigned int translateMode(std::string modeStr, bool silent = false);
        unsigned int translateMode(std::string modeStr, unsigned int verbosity = DEFAULT_VERBOSITY);

    public:
        // Constructors
        Options(unsigned int mode, unsigned int verbosity = DEFAULT_VERBOSITY, bool smartOperation = DEFAULT_SMART_OP, unsigned int optMask = DEFAULT_OPT_MASK, std::string globalConfigPath = DEFAULT_GLOBAL_CONFIG_PATH, std::string userConfigPath = DEFAULT_USER_CONFIG_PATH, std::string systemRoot = DEFAULT_SYSTEM_ROOT, std::string tarLibraryPath = DEFAULT_TAR_LIBRARY_PATH, std::string installedPkgsPath = DEFAULT_INSTALLED_PKG_PATH, std::set<std::string> excludedFiles = DEFAULT_EXCLUDED_FILES);

        // Getters
        mode_s getMode();
        unsigned int getModeIndex();
        std::string getModeStr();
        unsigned int getOptMask();
        unsigned int getVerbosity();
        bool getSmartOperation();
        std::string getGlobalConfigPath();
        std::string getUserConfigPath();
        std::string getSystemRoot();
        std::string getTarLibraryPath();
        std::string getInstalledPkgsPath();
        std::set<std::string> getExcludedFiles();

        // Setters
        bool setMode(unsigned int mode, bool silent = false);
        bool setMode(unsigned int mode, unsigned int verbosity = DEFAULT_VERBOSITY);
        bool setMode(std::string mode, bool silent = false);
        bool setMode(std::string mode, unsigned int verbosity = DEFAULT_VERBOSITY);
        bool setMode(mode_s m, bool silent = false);
        bool setMode(mode_s m, unsigned int verbosity = DEFAULT_VERBOSITY);
        bool setOptMask(unsigned int optMask, bool silent = false);
        bool setOptMask(unsigned int optMask, unsigned int verbosity = DEFAULT_VERBOSITY);
        bool setVerbosity(unsigned int verbosityLevel, bool silent = false);
        bool setVerbosity(unsigned int verbosityLevel, unsigned int verbosity = DEFAULT_VERBOSITY);
        bool setVerbosity(const char* verbosityLevel, bool silent = false);
        bool setVerbosity(const char* verbosityLevel, unsigned int verbosity = DEFAULT_VERBOSITY);
        bool setSmartOperation(bool smartOperation, bool silent = false);
        bool setSmartOperation(bool smartOperation, unsigned int verbosity = DEFAULT_VERBOSITY);
        bool setGlobalConfigPath(std::string globalConfigPath, bool silent = false);
        bool setGlobalConfigPath(std::string globalConfigPath, unsigned int verbosity = DEFAULT_VERBOSITY);
        bool setUserConfigPath(std::string userConfigPath, bool silent = false);
        bool setUserConfigPath(std::string userConfigPath, unsigned int verbosity = DEFAULT_VERBOSITY);
        bool setSystemRoot(std::string systemRoot, bool silent = false);
        bool setSystemRoot(std::string systemRoot, unsigned int verbosity = DEFAULT_VERBOSITY);
        bool setTarLibraryPath(std::string tarLibrary, bool silent = false);
        bool setTarLibraryPath(std::string tarLibrary, unsigned int verbosity = DEFAULT_VERBOSITY);
        bool setInstalledPkgsPath(std::string installedPkgsPath, bool silent = false);
        bool setInstalledPkgsPath(std::string installedPkgsPath, unsigned int verbosity = DEFAULT_VERBOSITY);
        bool setExcludedFiles(std::set<std::string> excludedFiles, bool silent = false);
        bool setExcludedFiles(std::set<std::string> excludedFiles, unsigned int verbosity = DEFAULT_VERBOSITY);

        // Adds the values to the options as appropriate
        bool addToOptMask(unsigned int optMask, bool silent = false);
        bool addToOptMask(unsigned int optMask, unsigned int verbosity = DEFAULT_VERBOSITY);
        bool addToExcludedFiles(std::string, bool silent = false);
        bool addToExcludedFiles(std::string, unsigned int verbosity = DEFAULT_VERBOSITY);

        // Applies a config to the options as appropriate
        // Has logic for CLI arguments to take priority
        bool applyConfig(IConfigMap& conf, bool silent = false);
        bool applyConfig(IConfigMap& conf, unsigned int verbosity = DEFAULT_VERBOSITY);
};

#endif
