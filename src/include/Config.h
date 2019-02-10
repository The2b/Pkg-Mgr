/**
 * @author Thomas Lenz <thomas.lenz96@gmail.com> AS The2b
 * @date 11 November 2018
 * @project Package Manager
 * @file Config.h
 */

#ifndef _THE2B_CLASS_CONFIG_H
#define _THE2B_CLASS_CONFIG_H

#include <stdio.h>  // printf, fprintf
#include <string>   // std::string
#include <map>      // maps
#include <vector>   // vectors
#include <fstream>  // Reading configs

// The keys used for parsable config file options 
#define KEY_VERBOSE "verbosity"
#define KEY_SMART_OP "smartOperation"
#define KEY_GLOBAL_CONFIG_PATH "globalConfigPath"
#define KEY_USER_CONFIG_PATH "userConfigPath"
#define KEY_SYSTEM_ROOT "systemRoot"
#define KEY_TAR_LIBRARY_PATH "packageLibraryPath"
#define KEY_INSTALLED_PKG_PATH "installedPkgPath"
#define KEY_EXCLUDED_FILES "excludedFiles"

// The character we use for comments
#define COMMENT_CHAR '#'

// Our delimiter
#define DELIM_CHAR '='

/**
 * @class IConfigMap
 * 
 * @brief This is the minimum we need exposed for other classes usage
 *
 * @details In pparticular, Options applyConfig function uses this interface
 * This interface is used to make unit testing said function far easier, as I can simply return a constant map.
 */
class IConfigMap {
    public:
        virtual std::map<std::string, std::string>* getConfigMap() = 0;
};

/**
 * @class Config
 * 
 * @brief This class is used to read, parse, and store data from configuration files or the command-line
 * 
 * @details Valid keys are defined in configKeys in Options.cpp
 */
class Config : public IConfigMap {
    friend void mergeConfig(IConfigMap& baseConfig, IConfigMap& newConfig, unsigned int verbosity = DEFAULT_VERBOSITY);
    
    private:
        bool isOpen;
        std::string pathname;
        std::ifstream confFileStream;
        std::map<std::string, std::string> configVals;

        // @TODO I may not need this. I'll need to see if this is even useful/worth the memory after being parsed
        std::vector<std::string> readVals;

        std::vector<std::string> readConfig(std::ifstream& fileStream, unsigned int verbosity = DEFAULT_VERBOSITY);
        std::map<std::string, std::string> parseConfig(std::vector<std::string> rawStrs, unsigned int verbosity = DEFAULT_VERBOSITY, char delim = DELIM_CHAR);

    public:
        // This one has a verbosity argument such that we can use verbosity before actually reading it from the config file and applying it to the options
        Config(std::string path = "/dev/null", unsigned int verbosity = DEFAULT_VERBOSITY, char delim = DELIM_CHAR);
        ~Config();
        std::string getPathname();
        std::vector<std::string> getConfigStrings();
        std::map<std::string, std::string>* getConfigMap();

        // The overloads w/o any arguments operate on confFileStream
        // All of the following have guards against operating on already opened or closed stream
        bool openStream(std::ifstream& fileStream);
        bool openStream();
        bool closeStream(std::ifstream& fileStream);
        bool closeStream();
};

int findDelim(std::string str, const char delim);

#endif /* _THE2B_CLASS_CONFIG_H */
