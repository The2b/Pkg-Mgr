/**
 * @author Thomas Lenz <thomas.lenz96@gmail.com> AS The2b
 * @date 18 November 2018
 * @project Package Manager
 * @file Options.cpp
 * @error -500
 *
 * Just a note, since this is where we are storing all of our information, I decided to do all the option validation (from sources like config files and cli args) here, so that it's centralized. This may be broken apart later, such that the Options class is merely a warehouse, while we have another OptionsValidation class/set of functions to actually test the validity of the options passed to us. However, that will all be under the hood.
 */

#include "Options.h"

/**
 * @brief This map is used to translate configuration file options to values which we can use in switch statements, for efficiency
 * This means that in order for a new configuration option to be recognized, its string representation, as well as a unique integer identifier must be added to this map.
 * In addition, a valid option must also be added to the function applyConfig
 * These values are set by pre-processor directives in Options.h
 *
 * @TODO Look into allowing this to be somehow logically made up on the spot, rather than keeping it in the heap, if we want to optimize size over efficiency
 */
std::map<std::string, unsigned int> configKeys =
{
    { KEY_VERBOSE, MASK_VERBOSE },
    //{ KEY_SMART_OP, MASK_SMART_OP}, // Not yet implemented
    { KEY_GLOBAL_CONFIG_PATH, MASK_GLOBAL_CONFIG_PATH }, // Pointless; Only here for completion, and so that there's no message sent out about an unknown option
    { KEY_USER_CONFIG_PATH, MASK_USER_CONFIG_PATH }, // By the time we get to this file, the maps are already merged, making this useless too. Keep uncommented out for the reason above.
    { KEY_SYSTEM_ROOT, MASK_SYSTEM_ROOT },
    { KEY_TAR_LIBRARY_PATH, MASK_TAR_LIBRARY_PATH },
    { KEY_INSTALLED_PKG_PATH, MASK_INSTALLED_PKG_PATH },
    //{ KEY_EXCLUDED_FILES, MASK_EXCLUDED_FILES } // Not yet implemented
};

/**
 * @brief A map used to take mode integers and create a mode_s struct
 *
 * @details The mode_s struct offers proper < & > & == operators, as well as storing the required string.
 * This allows for faster lookups if we are printing out the mode of operation, at the cost of size
 *
 * @TODO Replace mode_s with string, and then have a function that takes in a mode int and outputs a mode_s, using the new map to get its string and building the mode_s on the fly. This will cost a few clock cycles, but will reduce a good bit of redundant data on the heap
 * To the same point, make this a pair, so we can go either way (from int to string, or vice versa)
 */
std::map<int, mode_s> modes = {
    { INSTALL, mode_s{ INSTALL, "install" } },
    { UNINSTALL, mode_s{ UNINSTALL, "uninstall" } },
    { FOLLOW, mode_s{ FOLLOW, "follow" } },
    { UNFOLLOW, mode_s{ UNFOLLOW, "unfollow" } },
    { LIST_ALL, mode_s{ LIST_ALL, "list-all" } },
    { LIST_INSTALLED, mode_s{ LIST_INSTALLED, "list-installed" } },
    { NOP, mode_s{ NOP, NOP_KEY } }
};

// Modes of operation
// The values are replaced per the pre-processor directives in Options.h
// @TODO See if we really need this
// It should probably be conflated with the above, using pairs instead of maps
/**
 * A dictionary used to translate mode strings into integers we can switch on
 * In order to add a new mode of operation, its string representation(s) and integer identifier must be added to this map
 * All of the integers, as well as NOP_KEY, are defined per pre-processor directives in Options.h
 */
std::map<std::string, unsigned int> modeStrToInt = {
    { "install",        INSTALL },
    { "i",              INSTALL },
    { "uninstall",      UNINSTALL },
    { "u",              UNINSTALL },
    { "follow",         FOLLOW },
    { "f",              FOLLOW },
    { "unfollow",       UNFOLLOW },
    { "uf",             UNFOLLOW },
    { "list-all",       LIST_ALL },
    { "la",             LIST_ALL },
    { "list-installed", LIST_INSTALLED },
    { "li",             LIST_INSTALLED },
    { NOP_KEY,          NOP }
};

/**
 * @brief This is used as a validation set, such that we can check if a given mode is valid
 * 
 * @details In order to add a new mode of operation, its identifier must be added to this set
 */
std::set<unsigned int> validModes = {
    0,1,2,3,4,5,6,7,8,9,99
};

/**
 * @brief This is a set used to validate that the optMask values we send to addToOptMask are valid
 * @details Specifically, this set is used to make sure that any values given to addToOptMask are powers of two
 */
std::set<unsigned int> validOptMaskVals = {
    0,1,2,4,8,16,32,64,128,256
};

/**
 * @brief The constructor for the Options class. Most of these have default values set by the pre-processor at compile time via environment variables in config.h. If those can't be found (which should never be the case), there are hard-coded fail-safe values in the header Options.h
 *
 * @details Just calls the set* functions
 *
 * @param unsigned int mode
 * @param unsigned int verbosity
 * @param bool smartOperation
 * @param unsigned int optionMask
 * @param std::string globalConfigPath
 * @param std::string userConfigPath
 * @param std::string systemRoot
 * @param std::string tarLibraryPath
 * @param std::string installedPkgsPath
 * @param std::set<std::string> excludedFiles
 *
 * @returns Constructed Options object
 */
Options::Options(unsigned int mode, unsigned int verbosity, bool smartOperation, unsigned int optMask, std::string globalConfigPath, std::string userConfigPath, std::string systemRoot, std::string tarLibraryPath, std::string installedPkgsPath, std::set<std::string> excludedFiles) {/*{{{*/
    setMode(mode,verbosity);
    setVerbosity(verbosity, verbosity);
    setSmartOperation(smartOperation, verbosity);
    setGlobalConfigPath(globalConfigPath, verbosity);
    setUserConfigPath(userConfigPath, verbosity);
    setSystemRoot(systemRoot, verbosity);
    setTarLibraryPath(tarLibraryPath, verbosity);
    setInstalledPkgsPath(installedPkgsPath, verbosity);
    setExcludedFiles(excludedFiles, verbosity);
    setOptMask(optMask, verbosity);
}/*}}}*/

/**
 * @brief Getter for the mode_s object associated with the Options object
 *
 * @returns mode_struct mode
 */
mode_s Options::getMode() {/*{{{*/
    return mode;
}/*}}}*/

/**
 * @brief Getter for the mode index integer associated with the mode_s object associated with the Options object
 * 
 * @returns unsigned int modeIndex
 */
unsigned int Options::getModeIndex() {/*{{{*/
    return mode.modeIndex;
}/*}}}*/

/**
 * @brief Getter for the mode string associated with the mode_s object associated with the Options object
 * 
 * @returns std::string modeStr
 */
std::string Options::getModeStr() {/*{{{*/
    return mode.modeStr;
}/*}}}*/

/**
 * @brief Getter for the option mask integer associated with the Options object
 *
 * @returns unsigned int optionMask
 */
unsigned int Options::getOptMask() {/*{{{*/
    return optMask;
}/*}}}*/

/**
 * @brief Getter for the verbosity integer associated with the Options object
 * 
 * @returns unsigned int verbosityLevel
 */
unsigned int Options::getVerbosity() {/*{{{*/
    return verbosity;
}/*}}}*/

/**
 * @brief Getter for the smart op boolean associated with the Options object
 *
 * @returns bool smartOperation
 */
bool Options::getSmartOperation() {/*{{{*/
    return smartOperation;
}/*}}}*/

/**
 * @brief Getter for the global configuration file path associated with the Options object
 *
 * @returns std::string globalConfigPath
 */
std::string Options::getGlobalConfigPath() {/*{{{*/
    return globalConfigPath;
}/*}}}*/

/**
 * @brief Getter for the user configuration file path associated with the Options object
 *
 * @returns std::string userConfigPath
 */
std::string Options::getUserConfigPath() {/*{{{*/
    return userConfigPath;
}/*}}}*/

/**
 * @brief Getter for the system root path associated with the Options object
 *
 * @returns std::string systemRoot
 */
std::string Options::getSystemRoot() {/*{{{*/
    return systemRoot;
}/*}}}*/

/**
 * @brief Getter for the tar package library path associated with the Options object
 *
 * @returns std::string tarLibraryPath
 */
std::string Options::getTarLibraryPath() {/*{{{*/
    return tarLibraryPath;
}/*}}}*/

/**
 * @brief Getter for the installed packages path associated with the Options object
 *
 * @returns std::string installedPkgIndexDir
 */
std::string Options::getInstalledPkgsPath() {/*{{{*/
    return installedPkgsPath;
}/*}}}*/

/**
 * @brief Getter for the excluded file paths associated with the Options object
 *
 * @returns std::set<std::string> excludedFiles
 */
std::set<std::string> Options::getExcludedFiles() {/*{{{*/
    return excludedFiles;
}/*}}}*/

/**
 * @brief Sets a mode based on the mode_s passed to it
 *
 * @details This is mostly going to be used in the background from the other two setMode functions, but can be used if you want to create your own mode_s struct variable
 *
 * @deprecated Replaced with integer verbosity to be consistent with the rest of the program
 *
 * @param mode_struct mode
 * @param bool silent
 *
 * @returns bool wasModeValid
 */
bool Options::setMode(mode_s m, bool silent) {/*{{{*/
        return (silent ? setMode(m, (unsigned int)(0)) : setMode(m, (unsigned int)(2)));
}/*}}}*/

/**
 * @brief Sets a mode based on the mode_s passed to it
 * 
 * @details This is mostly going to be used in the background from the other two setMode functions, but can be used if you want to create your own mode_s struct variable
 *
 * @param mode_struct mode
 * @param unsigned int verbosity
 *
 * @returns bool wasModeValid
 */
bool Options::setMode(mode_s m, unsigned int verbosity) {/*{{{*/
    if((modes.find(m.modeIndex) != modes.end()) && (modes.at(m.modeIndex).modeStr == m.modeStr)) {
        mode = m;
        return true;
    }

    else {
        if(verbosity != 0) {
            fprintf(stderr,"Error: Invalid mode passed to Options::setMode(mode_s m). This should never occur. Please contact the creator of the program.\n");
        }

        return false;
    }
}/*}}}*/

/**
 * @brief Sets the mode based on the integer passed to it, which is used as an index for the "modes" map
 *
 * @deprecated Replaced with integer verbosity to be consistent with the rest of the program
 *
 * @param unsigned int modeIndex
 * @param bool silent
 *
 * @returns bool wasModeValid
 */
bool Options::setMode(unsigned int m, bool silent) {/*{{{*/
    return (silent ? setMode(m, (unsigned int)(0)) : setMode(m, (unsigned int)(2)));
}/*}}}*/

/**
 * @brief Sets the mode based on the integer passed to it, which is used as an index for the "modes" map
 *
 * @param unsigned int modeIndex
 * @param unsigned int verbosity
 *
 * @returns bool wasModeValid
 */
bool Options::setMode(unsigned int m, unsigned int verbosity) {/*{{{*/
    // Validate the mode requested
    if((modes.find(m) != modes.end())) {
        mode = modes.at(m);
        return true;
    }

    else {
        if(verbosity != 0) {
            fprintf(stderr,"Error: pkg-mgr requires a valid mode of operation. Check pkg-mgr -h for more details.\n");
        }

        return false;
    }
}/*}}}*/

/**
 * @brief Sets a mode based on the string passed to it. 
 * 
 * @details The string is then used as a key for the "modeStrToInt" map
 *
 * @deprecated Replaced with integer verbosity to be consistent with the rest of the program
 *
 * @param std::string modeString
 * @param bool silent
 *
 * @returns wasModeValid
 */
bool Options::setMode(std::string m, bool silent) {/*{{{*/
    return (silent ? setMode(translateMode(m, (unsigned int)(0)), (unsigned int)(0)) : setMode(translateMode(m, (unsigned int)(2)), (unsigned int)(0)));
}/*}}}*/

/**
 * @brief Sets a mode based on the string passed to it. 
 * 
 * @details The string is then used as a key for the "modeStrToInt" map
 *
 * @param std::string modeString
 * @param unsigned int verbosity
 *
 * @returns wasModeValid
 */
bool Options::setMode(std::string m, unsigned int verbosity) {/*{{{*/
    if(modeStrToInt.find(m) != modeStrToInt.end()) {
        return setMode(modeStrToInt.at(m), verbosity);
    }

    else {
        return false;
    }
}/*}}}*/

/**
 * @brief Sets the option mask.
 * 
 * @details The option mask is used to track which options were set by the user via the CLI, such that they are not overridden by the Configs we read later
 * Consequentially, this should only be used after the options are read from the CLI
 *
 * @deprecated Replaced with integer verbosity to be consistent with the rest of the program
 *
 * @param unsigned int optionMask
 * @param bool silent
 * 
 * @returns bool wasOptionMaskValid
 */
bool Options::setOptMask(unsigned int o, bool silent) {/*{{{*/
    return (silent ? setOptMask(o, (unsigned int)(0)) : setOptMask(o, (unsigned int)(2)));
}/*}}}*/

/**
 * @brief Sets the option mask.
 * 
 * @details The option mask is used to track which options were set by the user via the CLI, such that they are not overridden by the Configs we read later
 * Consequentially, this should only be used after the options are read from the CLI
 *
 * @param unsigned int optionMask
 * @param unsigned int verbosity
 * 
 * @returns bool wasOptionMaskValid
 */
bool Options::setOptMask(unsigned int o, unsigned int verbosity) {/*{{{*/
    if(o < (unsigned int)std::pow(2,MASK_SIZE)) {
        optMask = o;
        return true;
    }

    else {
        if(verbosity != 0) {
            fprintf(stderr,"Error: An invalid optMask was passed to Options::setOptMask. This should never occur. Please contact the creator of the program.\n");
        }

        return false;
    }
}/*}}}*/

/**
 * @brief Sets the verbosity to use for future operations.
 * 
 * @warning This alone does not set verbosity, but stores it
 * The value must still be passed to the operations correctly
 *
 * @deprecated Replaced with integer verbosity to be consistent with the rest of the program
 *
 * @param unsigned int verbosityLevel
 * @param bool silent
 *
 * @returns bool wasVerbosityValid
 */
bool Options::setVerbosity(unsigned int v, bool silent) {/*{{{*/
    return (silent ? setVerbosity(v, (unsigned int)(0)) : setVerbosity(v, (unsigned int)(2)));
}/*}}}*/

/**
 * @brief Sets the verbosity to use for future operations.
 * 
 * @warning This alone does not set verbosity, but stores it
 * The value must still be passed to the operations correctly
 *
 * @param unsigned int verbosityLevel
 * @param unsigned int verbosity
 *
 * @returns bool wasVerbosityValid
 */
bool Options::setVerbosity(unsigned int v, unsigned int verbosityLevel) {/*{{{*/
    // Make sure it's between 0 and 4
    if(v >= 0 && v <= 4) {
        verbosity = v;
        return true;
    }

    else {
        if(verbosityLevel != 0) {
            fprintf(stderr,"Error: Verbosity must be an integer between 0 and 4.\n");
        }

        return false;
    }
}/*}}}*/

/**
 * @brief Sets the verbosity to use for future operations.
 * 
 * @warning This alone does not set verbosity, but stores it
 * The value must still be passed to the operations correctly
 *
 * @warning This overload will fail if the second index is not '\0', since values are meant to be from 0 to 4
 * This is also meant to be used such that the command-line option or a line from a file stream can be passed directly to it, rather than parsing it out manually
 * If setVerbosity needs to be called from somewhere from within the program for some reason, you should probably use the unsigned int overload instead
 *
 * @deprecated Replaced with integer verbosity to be consistent with the rest of the program
 *
 * @param const char* verbosityLevel
 * @param bool silent
 * 
 * @returns bool wasVerbosityValid
 */
bool Options::setVerbosity(const char* v, bool silent) {/*{{{*/
    return (silent ? setVerbosity(v, (unsigned int)(0)) : setVerbosity(v, (unsigned int)(1)));
}/*}}}*/

/**
 * @brief Sets the verbosity to use for future operations.
 * 
 * @warning This alone does not set verbosity, but stores it
 * The value must still be passed to the operations correctly
 *
 * @warning This overload will fail if the second index is not '\0', since values are meant to be from 0 to 4
 * This is also meant to be used such that the command-line option or a line from a file stream can be passed directly to it, rather than parsing it out manually
 * If setVerbosity needs to be called from somewhere from within the program for some reason, you should probably use the unsigned int overload instead
 *
 * @param const char* verbosityLevel
 * @param unsigned int verbosity
 * 
 * @returns bool wasVerbosityValid
 */
bool Options::setVerbosity(const char* v, unsigned int verbosityLevel) {/*{{{*/
    // Check the second character, and make sure it is a null terminator
    if(v[1] == '\0') {
        return setVerbosity((unsigned int)atoi(v), verbosityLevel);
    }

    else {
        if(verbosityLevel != 0) {
            fprintf(stderr,"Error: Verbosity must be an integer between 0 and 4.\n");
        }

        return false;
    }
}/*}}}*/

// @TODO Write an overload for const char* and/or string instead of bool for the first parameter
/**
 * @brief Sets whether or not we are going to use smart operation.
 * 
 * @warning Currently, the smart operation itself is not implemented; However, we have this here for the future
 * 
 * @details This function always returns true, since there cannot be an invalid value without error'ing out when the function is called
 *
 * @deprecated Replaced with integer verbosity to be consistent with the rest of the program
 *
 * @param bool smartOperation
 * @param bool silent
 *
 * @returns bool wasSmartOperationValid
 */
bool Options::setSmartOperation(bool so, bool silent) {/*{{{*/
    // I know this isn't required, and it's so fucking weird that the compiler will almost certainly not realize that the verbosity doesn't matter, but the additional overhead is so low, I'm okay trading it for consistency.
    return (silent ? setSmartOperation(so, (unsigned int)(0)) : setSmartOperation(so, (unsigned int)(2)));
}/*}}}*/

/**
 * @brief Sets whether or not we are going to use smart operation.
 * 
 * @warning Currently, the smart operation itself is not implemented; However, we have this here for the future
 * 
 * @details This function always returns true, since there cannot be an invalid value without error'ing out when the function is called
 *
 * @param bool smartOperation
 * @param unsigned int verbosity
 *
 * @returns bool wasSmartOperationValid
 */
bool Options::setSmartOperation(bool so, unsigned int verbosity) {/*{{{*/
    smartOperation = so;
    return true;
}/*}}}*/

/**
 * @brief Sets the global configuration file path to read.
 * 
 * @details This option is only really helpful in the command line. Any other usage (AKA when used in a configuration file) will print out a warning that it does nothing, and the file path supplied is being ignored, if it is not silenced
 * A trailing directory separator is not required If you put one there, any warnings or errors will have two consecutive directory separators in them
 * The choice not to make logic to remove the second consecutive directory separator was mostly made for simplicity. If someone wants to put it in, go right ahead.
 *
 * @deprecated Replaced with integer verbosity to be consistent with the rest of the program
 *
 * @param std::string globalConfigPath
 * @param bool silent
 *
 * @returns bool didGlobalConfigPathExist
 */
bool Options::setGlobalConfigPath(std::string gcp, bool silent) {/*{{{*/
    return (silent ? setGlobalConfigPath(gcp, (unsigned int)(0)) : setGlobalConfigPath(gcp, (unsigned int)(2)));
}/*}}}*/

/**
 * @brief Sets the global configuration file path to read.
 *
 * @details This option is only really helpful in the command line. Any other usage (AKA when used in a configuration file) will print out a warning that it does nothing, and the file path supplied is being ignored, if verbosity is not 0
 * A trailing directory separator is not required If you put one there, any warnings or errors will have two consecutive directory separators in them
 * The choice not to make logic to remove the second consecutive directory separator was mostly made for simplicity. If someone wants to put it in, go right ahead.
 *
 * @param std::string globalConfigPath
 * @param unsigned int verbosity
 *
 * @returns bool didGlobalConfigPathExist
 */
bool Options::setGlobalConfigPath(std::string gcp, unsigned int verbosity) {/*{{{*/
    // If the path given is not absolute, make it so
    gcp = std::filesystem::absolute(gcp);
    
    std::error_code e;
    if(std::filesystem::exists(gcp,e)) {
        globalConfigPath = gcp;
        return true;
    }

    else {
        if(verbosity != 0) {
            fprintf(stderr,"Error: Given global configuration file path %s does not exist.\n",gcp.c_str());
            fprintf(stderr,"%s\n",e.message().c_str());
        }

        return false;
    }
}/*}}}*/

/**
 * @brief Sets the user file path to read.
 * 
 * @details If the path is relative, it will be relative to the user's home directory. There is currently no way to change this.
 * Alternatively, if it is absolute, it will be taken literally
 * A trailing directory separator is not required If you put one there, any warnings or errors will have two consecutive directory separators in them
 * The choice not to make logic to remove the second consecutive directory separator was mostly made for simplicity. If someone wants to put it in, go right ahead.
 *
 * @warning There will be no warning if we have a user configuration file path in the user configuration file. This is because the configuration file itself is abstracted away, such that there is no difference between a user configuration file and a global configuration file, as far as the objects are concerned, and thus no way of knowing which file the option came from. The difference is made in which is applied first (global), which will be overridden by the second (user).
 *
 * @deprecated Replaced with integer verbosity to be consistent with the rest of the program
 *
 * @param std::string userConfigPath
 * @param bool silent
 *
 * @returns bool didUserConfigPathExist
 */
bool Options::setUserConfigPath(std::string ucp, bool silent) {/*{{{*/
    return (silent ? setUserConfigPath(ucp, (unsigned int)(0)) : setUserConfigPath(ucp, (unsigned int)(2)));
}/*}}}*/

/**
 * @brief Sets the user file path to read.
 * 
 * @details If the path is relative, it will be relative to the user's home directory. There is currently no way to change this.
 *
 * Alternatively, if it is absolute, it will be taken literally
 *
 * @warning There will be no warning if we have a user configuration file path in the user configuration file. This is because the configuration file itself is abstracted away, such that there is no difference between a user configuration file and a global configuration file, as far as the objects are concerned, and thus no way of knowing which file the option came from. The difference is made in which is applied first (global), which will be overridden by the second (user).
 *
 * @param std::string userConfigPath
 * @param unsigned int verbosity
 *
 * @returns bool didUserConfigPathExist
 */
bool Options::setUserConfigPath(std::string ucp, unsigned int verbosity) {/*{{{*/
    // If we are using a relative path, concatanate our home directory
    std::filesystem::path realUCP = ucp;
    if(realUCP.is_relative()) {
        // @TODO Add more tests for the home directory
        realUCP = getenv("HOME");
        realUCP += "/" + ucp; // cat'ing does not add a directory seperator, and HOME may not have one
    }

    // Go on from there
    std::error_code e;
    if(std::filesystem::exists(realUCP,e)) {
        userConfigPath = realUCP;
        return true;
    }

    else {
        if(verbosity != 0) {
            fprintf(stderr,"Error: The given user configuration path %s does not exist.\n",realUCP.c_str());
            fprintf(stderr,"%s\n",e.message().c_str());
        }

        return false;
    }
}/*}}}*/

/**
 * @brief Sets the system root to operate on.
 *
 * @details Packages will be installed to or removed using this directory as a prefix.
 * A trailing directory separator is not required If you put one there, any warnings or errors will have two consecutive directory separators in them
 * The choice not to make logic to remove the second consecutive directory separator was mostly made for simplicity. If someone wants to put it in, go right ahead.
 *
 * @deprecated Replaced with integer verbosity to be consistent with the rest of the program
 *
 * @param std::string systemRoot
 * @param bool silent
 *
 * @returns bool isSystemRootADirectory
 */
bool Options::setSystemRoot(std::string sr, bool silent) {/*{{{*/
    return (silent ? setSystemRoot(sr, (unsigned int)(0)) : setSystemRoot(sr, (unsigned int)(2)));
}/*}}}*/

/**
 * @brief Sets the system root to operate on.
 *
 * @details Packages will be installed to or removed using this directory as a prefix.
 *
 * @param std::string systemRoot
 * @param unsigned int verbosity
 *
 * @returns bool isSystemRootADirectory
 */
bool Options::setSystemRoot(std::string sr, unsigned int verbosity) {/*{{{*/
    // If the path given is not absolute, make it so
    sr = std::filesystem::absolute(sr);
    
    // Validate the existence of the system root
    std::error_code e;
    if(std::filesystem::is_directory(sr,e)) {
        systemRoot = sr;
        return true;
    }

    else {
        if(verbosity != 0) {
            fprintf(stderr,"Error: System root directory %s does not exist or is not a directory.\n",sr.c_str());
            fprintf(stderr,"%s\n",e.message().c_str());
        }

        return false;
    }
}/*}}}*/

/**
 * @brief Sets the folder path in which we should look for tar packages.
 * 
 * @details This has some logic to it. First of all, if the given path is not absolute, it will be concatanated to the user's current working directory
 * Second, it will verify the folder exists, and refuse to set the variable if it does not, and return false. The calling function should handle this, since there are cases where this is not a problem
 * A trailing directory separator is not required If you put one there, any warnings or errors will have two consecutive directory separators in them
 * The choice not to make logic to remove the second consecutive directory separator was mostly made for simplicity. If someone wants to put it in, go right ahead.
 *
 * @deprecated Replaced with integer verbosity to be consistent with the rest of the program
 *
 * @param std::string tarLibraryPath
 * @param bool silent
 *
 * @param return didTarLibraryPathExist
 */
bool Options::setTarLibraryPath(std::string tlp, bool silent) {/*{{{*/
    return (silent ? setTarLibraryPath(tlp, (unsigned int)(0)) : setTarLibraryPath(tlp, (unsigned int)(2)));
}/*}}}*/

/**
 * @brief Sets the folder path in which we should look for tar packages.
 * 
 * @details This has some logic to it. First of all, if the given path is not absolute, it will be concatanated to the user's current working directory
 * Second, it will verify the folder exists, and refuse to set the variable if it does not, and return false. The calling function should handle this, since there are cases where this is not a problem
 * A trailing directory separator is not required If you put one there, any warnings or errors will have two consecutive directory separators in them
 * The choice not to make logic to remove the second consecutive directory separator was mostly made for simplicity. If someone wants to put it in, go right ahead.
 *
 * @param std::string tarLibraryPath
 * @param unsigned int verbosity
 *
 * @param return didTarLibraryPathExist
 */
bool Options::setTarLibraryPath(std::string tlp, unsigned int verbosity) {/*{{{*/
    // If the path given is not absolute, make it so
    tlp = std::filesystem::absolute(tlp);

    // Validate the existence of the tar library
    std::error_code e;
    if(std::filesystem::exists(tlp,e)) {
        tarLibraryPath = tlp;
        return true;
    }

    else {
        if(verbosity != 0) {
            fprintf(stderr,"Error: Tar package library %s does not exist\n",tlp.c_str());
            fprintf(stderr,"%s\n",e.message().c_str());
        }
        
        return false;
    }
}/*}}}*/

/**
 * @brief Sets the folder path where index files of which packages are and are not installed are stored.
 *
 * @details This folder is where the files created by and removed by the follow/unfollow operations (called either on their own or by install/uninstall) are stored.
 * A trailing directory separator is not required If you put one there, any warnings or errors will have two consecutive directory separators in them
 * The choice not to make logic to remove the second consecutive directory separator was mostly made for simplicity. If someone wants to put it in, go right ahead.
 *
 * @deprecated Replaced with integer verbosity to be consistent with the rest of the program
 *
 * @param std::string setInstalledPkgIndexDirectory
 * @param bool silent
 *
 * @returns bool didInstalledPkgIndexDirectoryExist
 */
bool Options::setInstalledPkgsPath(std::string ipp, bool silent) {/*{{{*/
    return (silent ? setInstalledPkgsPath(ipp, (unsigned int)(0)) : setInstalledPkgsPath(ipp, (unsigned int)(2)));
}/*}}}*/

/**
 * @brief Sets the folder path where index files of which packages are and are not installed are stored.
 *
 * @details This folder is where the files created by and removed by the follow/unfollow operations (called either on their own or by install/uninstall) are stored.
 * A trailing directory separator is not required If you put one there, any warnings or errors will have two consecutive directory separators in them
 * The choice not to make logic to remove the second consecutive directory separator was mostly made for simplicity. If someone wants to put it in, go right ahead.
 *
 * @param std::string setInstalledPkgIndexDirectory
 * @param unsigned int verbosity
 *
 * @returns bool didInstalledPkgIndexDirectoryExist
 */
bool Options::setInstalledPkgsPath(std::string ipp, unsigned int verbosity) {/*{{{*/
    // If the path given is not absolute, make it so
    ipp = std::filesystem::absolute(ipp);
    
    // Validate the existence of the installed pkg path
    std::error_code e;
    if(std::filesystem::exists(ipp,e)) {
        installedPkgsPath = ipp;
        return true;
    }

    else {
        if(verbosity != 0) {
            fprintf(stderr,"Error: Installed pkg dir %s does not exist\n",ipp.c_str());
            fprintf(stderr,"%s\n",e.message().c_str());
        }

        return false;
    }
}/*}}}*/

/**
 * @brief Sets the files which should be ignored when installing or uninstalling packages.
 *
 * @warning The actual implementation of excluded files is not complete. As such, this option currently does nothing.
 *
 * @details Always returns true
 *
 * @deprecated Replaced with integer verbosity to be consistent with the rest of the program
 *
 * @param std::set<std::string> excludedFileSet
 * @param bool silent
 *
 * @returns bool true
 */
bool Options::setExcludedFiles(std::set<std::string> ef, bool silent) {/*{{{*/
    return (silent ? setExcludedFiles(ef, (unsigned int)(0)) : setExcludedFiles(ef, (unsigned int)(2)));
}/*}}}*/

/**
 * @brief Sets the files which should be ignored when installing or uninstalling packages.
 *
 * @warning The actual implementation of excluded files is not complete. As such, this option currently does nothing.
 *
 * @details Always returns true
 *
 * @param std::set<std::string> excludedFileSet
 * @param unsigned int verbosity
 *
 * @returns bool true
 */
bool Options::setExcludedFiles(std::set<std::string> ef, unsigned int verbosity) {/*{{{*/
    excludedFiles = ef;
    return true;
}/*}}}*/

/**
 * @brief OR's a given value with the current option mask.
 *
 * @details This is the recommended usage for option masks. The input should always be a power of 2, and is checked against the set "validOptMasks". Calling this function with 0 as the first argument does nothing.
 *
 * @TODO Allow a size optimization that computes the valid opt masks at runtime, rather than storing them as constants in a set. Keep the current iteration as a performance optimization.
 *
 * @deprecated Replaced with integer verbosity to be consistent with the rest of the program
 *
 * @param unsigned int optMaskToAdd
 * @param bool silent
 *
 * @returns wasOptMaskValid
 */
bool Options::addToOptMask(unsigned int opt, bool silent) {/*{{{*/
    return (silent ? addToOptMask(opt, (unsigned int)(0)) : addToOptMask(opt, (unsigned int)(2)));
}/*}}}*/

/**
 * @brief OR's a given value with the current option mask.
 *
 * @details This is the recommended usage for option masks. The input should always be a power of 2, and is checked against the set "validOptMasks". Calling this function with 0 as the first argument does nothing.
 *
 * @TODO Allow a size optimization that computes the valid opt masks at runtime, rather than storing them as constants in a set. Keep the current iteration as a performance optimization.
 *
 * @param unsigned int optMaskToAdd
 * @param unsigned int verbosity
 *
 * @returns wasOptMaskValid
 */
bool Options::addToOptMask(unsigned int opt, unsigned int verbosity) {/*{{{*/
    // Validate the optMask value
    if(validOptMaskVals.find(opt) != validOptMaskVals.end()) {
        optMask |= opt;
        return true;
    }

    else {
        if(verbosity != 0) {
            fprintf(stderr,"Error: An invalid optMask value was passed to Options\n");
        }
        
        return false;
    }
}/*}}}*/

/**
 * @brief This adds the file path given to the excluded files set. 
 * 
 * @warning Currently, the usage of that set is not implemented. As such, this option currently does not do anything.
 *
 * @details Always returns true
 *
 * @deprecated Replaced with integer verbosity to be consistent with the rest of the program
 *
 * @param std::string pathToAdd
 * @param bool silent
 *
 * @returns bool true
 */
bool Options::addToExcludedFiles(std::string path, bool silent) {/*{{{*/
    return (silent ? addToExcludedFiles(path, (unsigned int)(0)) : addToExcludedFiles(path, (unsigned int)(2)));
}/*}}}*/

/**
 * @brief This adds the file path given to the excluded files set. 
 * 
 * @warning Currently, the usage of that set is not implemented. As such, this option currently does not do anything.
 *
 * @details Always returns true
 *
 * @param std::string pathToAdd
 * @param unsigned int verbosity
 *
 * @returns bool true
 */
bool Options::addToExcludedFiles(std::string path, unsigned int verbosity) {/*{{{*/
    excludedFiles.insert(path);
    return true;
}/*}}}*/

/**
 * @brief Turns a mode string into an integer, per modeStrToInt
 *
 * @deprecated Replaced with integer verbosity to be consistent with the rest of the program
 *
 * @param std::string modeString
 * @param bool silent
 *
 * @returns unsigned int modeIndex
 */
unsigned int Options::translateMode(std::string m, bool silent) {/*{{{*/
    return (silent ? translateMode(m, (unsigned int)(0)) : translateMode(m,(unsigned int)(2)));
}/*}}}*/

/**
 * @brief Turns a mode string into an integer, per modeStrToInt
 * 
 * @details Returns NOP on error. At this time, this should never occur. However, in the future, this will return NOP if an invalid mode is given.
 * @warning Currently exits if an invalid mode is given. This should be changed, and return NOP instead.
 * 
 * @TODO Fix the above
 *
 * @param std::string modeString
 * @param unsigned int verbosity
 *
 * @returns unsigned int modeIndex
 */
unsigned int Options::translateMode(std::string m, unsigned int verbosity) {/*{{{*/
    if(modeStrToInt.find(m) != modeStrToInt.end()) {
        return modeStrToInt.at(m);
    }

    else {
        if(verbosity != 0) {
            fprintf(stderr,"Error: pkg-mgr requires a valid mode of operation. Check pkg-mgr -h for more information.\n");
        }

        // @TODO Make this return NOP instead, and check for NOP
        exit(-1);
    }

    // This should never get here, but just to be safe, catch it and return NOP
    return NOP;
}/*}}}*/

/**
 * @brief Applies a Config object to the Options object which calls this function.
 *
 * @details The brief summary is a lie. It actually can use any object implementing IConfigMap, in Config.h
 
 * @warning Any matching keys are overridden, and thus user configurations should be read last, such that the user's own options take priority over the global configuration file. Values set by the user via CLI are processed first, however. To avoid overwriting the user's manual options, this function checks against the optMask for whether or not the option was set via CLI. If so, the configuration file is ignored.
 *
 * @deprecated Replaced with integer verbosity to be consistent with the rest of the program
 *
 * @param IConfigMap& configMapObject
 * @param bool silent
 *
 * @returns bool wasApplicationSuccessful
 */
bool Options::applyConfig(IConfigMap& conf, bool silent) {/*{{{*/
    return (silent ? applyConfig(conf, (unsigned int)(0)) : applyConfig(conf, (unsigned int)(2)));
}/*}}}*/

/**
 * @brief Applies a Config object to the Options object which calls this function.
 *
 * @details The brief summary is a lie. It actually can use any object implementing IConfigMap, in Config.h
 *
 * @warning Any matching keys are overridden, and thus user configurations should be read last, such that the user's own options take priority over the global configuration file. Values set by the user via CLI are processed first, however. To avoid overwriting the user's manual options, this function checks against the optMask for whether or not the option was set via CLI. If so, the configuration file is ignored.
 *
 * @param IConfigMap& configMapObject
 * @param unsigned int verbosity
 *
 * @returns bool wasApplicationSuccessful
 */
bool Options::applyConfig(IConfigMap& conf, unsigned int verbosity) {/*{{{*/
    std::map<std::string, std::string> confMap = *conf.getConfigMap();
    unsigned int mask = optMask;

    // For each entry in the config map, see if we have a matching option
    // Design-wise, it's this or go through each option, and I find this to be both cleaner and more maintainable
    // A note, I wouldn't rely on these exit codes for scripting purposes. Because of how liberally I like to spread exit codes, I may run out.
    // If I do, I'll likely make all of the invalid value handlers exit with the same code, instead of unique ones
    for(std::map<std::string, std::string>::iterator it = confMap.begin(); it != confMap.end(); it++) {
        switch(configKeys[it->first]) {
            case MASK_VERBOSE:
                // Make sure we don't have quiet or verbose set by the user
                if(((mask & MASK_VERBOSE) == 0)) {
                    // Translate the config value to a proper value
                    if(!setVerbosity(it->second.c_str(), verbosity)) {
                        return false;
                    }
                }

                break;


            case MASK_SMART_OP: 
                // Break for now, print out a warning
                // @TODO
                if(verbosity != 0) {
                    fprintf(stderr,"Warning: Smart mode is not yet implemented. Ignoring...\n");
                }

                break;
                                

            case MASK_GLOBAL_CONFIG_PATH: 
                // @TODO Look at this
                if(verbosity != 0) {
                    fprintf(stderr,"Uh... Did you put a global configuration file in your configuration file? We're already past the time where we read the configuration file, once we look at it, so I don't know what you thought you were going to accomplish with this...\n");
                }

                if(!setGlobalConfigPath(it->second, verbosity)) {
                    return false;
                }

                break;
                                          

            case MASK_USER_CONFIG_PATH: 
                // Don't do anything, since we've already read this. Again, here for completion
                break;
                                        

            case MASK_SYSTEM_ROOT: 
                // Validation here is handled in the packages "installPkg" function
                if((mask & MASK_SYSTEM_ROOT) == 0) {
                    if(!setSystemRoot(it->second, verbosity)) {
                        return false;
                    }
                }

                break;
                                   

            case MASK_TAR_LIBRARY_PATH: 
                // Validation here is handled in the packages "installPkg" function
                if((mask & MASK_TAR_LIBRARY_PATH) == 0) {
                    if(!setTarLibraryPath(it->second, verbosity)) {
                        return false;
                    }
                }

                break;
                                

            case MASK_INSTALLED_PKG_PATH: 
                if((mask & MASK_INSTALLED_PKG_PATH) == 0) {
                    if(!setInstalledPkgsPath(it->second, verbosity)) {
                        return false;
                    }
                }

                break;
                                          

            case MASK_EXCLUDED_FILES: 
                if((mask & MASK_EXCLUDED_FILES) == 0) {
                    //@TODO
                    if(verbosity != 0) {
                        fprintf(stderr,"Warning: Excluded files not yet implemented...\n");
                    }
                }

                break;
                                      

            default: 
                if(verbosity != 0) {
                    fprintf(stderr,"Warning: Unrecognized configuration option %s\n. Attempting to continue normally...",it->first.c_str());
                }
        }
    }

    return true;
}/*}}}*/
