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
 * This map is used to translate configuration file options to values which we can use in switch statements, for efficiency
 * This means that in order for a new configuration option to be recognized, its string representation, as well as a unique integer identifier must be added to this map.
 * In addition, a valid option must also be added to the function applyConfig
 * These values are set by pre-processor directives in Options.h
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

// @TODO See if we really need this
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
 * This is used as a validation set, such that we can check if a given mode is valid
 * In order to add a new mode of operation, its identifier must be added to this set
 */
std::set<unsigned int> validModes = {
    0,1,2,3,4,5,6,7,8,9,99
};

/**
 * This is a set used to validate that the optMask values we send to addToOptMask are valid
 * Specifically, this set is used to make sure that any values given to addToOptMask are powers of two
 */
std::set<unsigned int> validOptMaskVals = {
    0,1,2,4,8,16,32,64,128,256
};

/**
 * The constructor for the Options class. Most of these have default values set by the pre-processor at compile time via environment variables in config.h. If those can't be found (which should never be the case), there are hard-coded fail-safe values in the header Options.h
 *
 * Just calls the set* functions
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
    setMode(mode);
    setVerbosity(verbosity);
    setSmartOperation(smartOperation);
    setGlobalConfigPath(globalConfigPath);
    setUserConfigPath(userConfigPath);
    setSystemRoot(systemRoot);
    setTarLibraryPath(tarLibraryPath);
    setInstalledPkgsPath(installedPkgsPath);
    setExcludedFiles(excludedFiles);
    setOptMask(optMask);
}/*}}}*/

/**
 * Getter for the mode_s object associated with the Options object
 *
 * @returns mode_struct mode
 */
mode_s Options::getMode() {/*{{{*/
    return mode;
}/*}}}*/

/**
 * Getter for the mode index integer associated with the mode_s object associated with the Options object
 * 
 * @returns unsigned int modeIndex
 */
unsigned int Options::getModeIndex() {/*{{{*/
    return mode.modeIndex;
}/*}}}*/

/**
 * Getter for the mode string associated with the mode_s object associated with the Options object
 * 
 * @returns std::string modeStr
 */
std::string Options::getModeStr() {/*{{{*/
    return mode.modeStr;
}/*}}}*/

/**
 * Getter for the option mask integer associated with the Options object
 *
 * @returns unsigned int optionMask
 */
unsigned int Options::getOptMask() {/*{{{*/
    return optMask;
}/*}}}*/

/**
 * Getter for the verbosity integer associated with the Options object
 * 
 * @returns unsigned int verbosityLevel
 */
unsigned int Options::getVerbosity() {/*{{{*/
    return verbosity;
}/*}}}*/

/**
 * Getter for the smart op boolean associated with the Options object
 *
 * @returns bool smartOperation
 */
bool Options::getSmartOperation() {/*{{{*/
    return smartOperation;
}/*}}}*/

/**
 * Getter for the global configuration file path associated with the Options object
 *
 * @returns std::string globalConfigPath
 */
std::string Options::getGlobalConfigPath() {/*{{{*/
    return globalConfigPath;
}/*}}}*/

/**
 * Getter for the user configuration file path associated with the Options object
 *
 * @returns std::string userConfigPath
 */
std::string Options::getUserConfigPath() {/*{{{*/
    return userConfigPath;
}/*}}}*/

/**
 * Getter for the system root path associated with the Options object
 *
 * @returns std::string systemRoot
 */
std::string Options::getSystemRoot() {/*{{{*/
    return systemRoot;
}/*}}}*/

/**
 * Getter for the tar package library path associated with the Options object
 *
 * @returns std::string tarLibraryPath
 */
std::string Options::getTarLibraryPath() {/*{{{*/
    return tarLibraryPath;
}/*}}}*/

/**
 * Getter for the installed packages path associated with the Options object
 *
 * @returns std::string installedPkgIndexDir
 */
std::string Options::getInstalledPkgsPath() {/*{{{*/
    return installedPkgsPath;
}/*}}}*/

/**
 * Getter for the excluded file paths associated with the Options object
 *
 * @returns std::set<std::string> excludedFiles
 */
std::set<std::string> Options::getExcludedFiles() {/*{{{*/
    return excludedFiles;
}/*}}}*/

// @TODO Refactor this to have an actual integer verbosity level like everything else
/**
 * Sets a mode based on the mode_s passed to it
 * This is mostly going to be used in the background from the other two setMode functions, but can be used if you want to create your own mode_s struct variable
 *
 * @param mode_struct mode
 * @param bool silent
 *
 * @returns bool wasModeValid
 */
bool Options::setMode(mode_s m, bool silent) {/*{{{*/
    if((modes.find(m.modeIndex) != modes.end()) && (modes.at(m.modeIndex).modeStr == m.modeStr)) {
        mode = m;
        return true;
    }

    else {
        if(!silent) {
            fprintf(stderr,"Error: Invalid mode passed to Options::setMode(mode_s m). This should never occur. Please contact the creator of the program.\n");
        }

        return false;
    }
}/*}}}*/

// @TODO Refactor this to have an actual integer verbosity level like everything else
/**
 * Sets the mode based on the integer passed to it, which is used as an index for the "modes" map
 *
 * @param unsigned int modeIndex
 * @param bool silent
 *
 * @returns bool wasModeValid
 */
bool Options::setMode(unsigned int m, bool silent) {/*{{{*/
    // Validate the mode requested
    if((modes.find(m) != modes.end())) {
        mode = modes.at(m);
        return true;
    }

    else {
        if(!silent) {
            fprintf(stderr,"Error: pkg-mgr requires a valid mode of operation. Check pkg-mgr -h for more details.\n");
        }

        return false;
    }
}/*}}}*/

// @TODO Refactor this to have an actual integer verbosity level like everything else
/**
 * Sets a mode based on the string passed to it. 
 * The string is then used as a key for the "modeStrToInt" map
 *
 * @param std::string modeString
 * @param bool silent
 *
 * @returns wasModeValid
 */
bool Options::setMode(std::string m, bool silent) {/*{{{*/
    return setMode(translateMode(m),silent);
}/*}}}*/

// @TODO Refactor this to have an actual integer verbosity level like everything else
/**
 * Sets the option mask.
 * The option mask is used to track which options were set by the user via the CLI, such that they are not overridden by the Configs we read later
 * Consequentially, this should only be used after the options are read from the CLI
 *
 * @param unsigned int optionMask
 * @param bool silent
 * 
 * @returns bool wasOptionMaskValid
 */
bool Options::setOptMask(unsigned int o, bool silent) {/*{{{*/
    if(o < (unsigned int)std::pow(2,MASK_SIZE)) {
        optMask = o;
        return true;
    }

    else {
        if(!silent) {
            fprintf(stderr,"Error: An invalid optMask was passed to Options::setOptMask. This should never occur. Please contact the creator of the program.\n");
        }

        return false;
    }
}/*}}}*/

// @TODO Refactor this to have an actual integer verbosity level like everything else
/**
 * Sets the verbosity to use for future operations.
 * Note that this alone does not set verbosity, but stores it
 * The value must still be passed to the operations correctly
 *
 * @param unsigned int verbosityLevel
 * @param bool silent
 *
 * @returns bool wasVerbosityValid
 */
bool Options::setVerbosity(unsigned int v, bool silent) {/*{{{*/
    // Make sure it's between 0 and 4
    if(v >= 0 && v <= 4) {
        verbosity = v;
        return true;
    }

    else {
        if(!silent) {
            fprintf(stderr,"Error: Verbosity must be an integer between 0 and 4.\n");
        }

        return false;
    }
}/*}}}*/

// @TODO Refactor this to have an actual integer verbosity level like everything else
/**
 * Sets the verbosity to use for future operations.
 * Note that this alone does not set verbosity, but stores it
 * The value must still be passed to the operations correctly
 *
 * This overload will fail if the second index is not '\0', since values are meant to be from 0 to 4
 * This is also meant to be used such that the command-line option or a line from a file stream can be passed directly to it, rather than parsing it out manually
 * If setVerbosity needs to be called from somewhere from within the program for some reason, you should probably use the unsigned int overload instead
 *
 * @param const char* verbosityLevel
 * @param bool silent
 * 
 * @returns bool wasVerbosityValid
 */
bool Options::setVerbosity(const char* v, bool silent) {/*{{{*/
    if(v[1] == '\0') {
        return setVerbosity((unsigned int)atoi(v), silent);
    }

    else {
        if(!silent) {
            fprintf(stderr,"Error: Verbosity must be an integer between 0 and 4.\n");
        }

        return false;
    }
}/*}}}*/

// @TODO Refactor this to have an actual integer verbosity level like everything else
// @TODO Write an overload for const char* and/or string instead of bool
/**
 * Sets whether or not we are going to use smart operation.
 * Currently, the smart operation itself is not implemented; However, we have this here for the future
 * This function always returns true, since there cannot be an invalid value without error'ing out when the function is called
 *
 * @param bool smartOperation
 * @param bool silent
 *
 * @returns bool wasSmartOperationValid
 */
bool Options::setSmartOperation(bool so, bool silent) {/*{{{*/
    smartOperation = so;
    return true;
}/*}}}*/

// @TODO Refactor this to have an actual integer verbosity level like everything else
/**
 * Sets the global configuration file path to read.
 * This option is only really helpful in the command line. Any other usage (AKA when used in a configuration file) will print out a warning that it does nothing, and the file path supplied is being ignored
 *
 * @param std::string globalConfigPath
 * @param bool silent
 *
 * @returns bool didGlobalConfigPathExist
 */
bool Options::setGlobalConfigPath(std::string gcp, bool silent) {/*{{{*/
    std::error_code e;
    if(std::filesystem::exists(gcp,e)) {
        globalConfigPath = gcp;
        return true;
    }

    else {
        if(!silent) {
            fprintf(stderr,"Error: Given global configuration file path %s does not exist.\n",gcp.c_str());
            fprintf(stderr,"%s\n",e.message().c_str());
        }

        return false;
    }
}/*}}}*/

// @TODO Refactor this to have an actual integer verbosity level like everything else
/**
 * Sets the user file path to read.
 * 
 * This is always relative to the user's home directory. Currently, there is no way to override this, as this is desired behavior, mainly so that each user can have their own programs in their home directories. This may be changed in the future, however, if I have a change of heart.
 *
 * Note that there will be no warning if we have a user configuration file path in the user configuration file. This is because the configuration file itself is abstracted away, such that there is no difference between a user configuration file and a global configuration file, as far as the objects are concerned, and thus no way of knowing which file the option came from. The difference is made in which is applied first (global), which will be overridden by the second (user).
 *
 * @param std::string userConfigPath
 * @param bool silent
 *
 * @returns bool didUserConfigPathExist
 */
bool Options::setUserConfigPath(std::string ucp, bool silent) {/*{{{*/
    std::error_code e;
    std::string realUCP = getenv("HOME");
    realUCP += ucp;
    if(std::filesystem::exists(std::string(realUCP),e)) {
        userConfigPath = realUCP;
        return true;
    }

    else {
        if(!silent) {
            fprintf(stderr,"Error: The given user configuration path %s does not exist.\n",realUCP.c_str());
            fprintf(stderr,e.message().c_str());
        }

        return false;
    }
}/*}}}*/

// @TODO Refactor this to have an actual integer verbosity level like everything else
/**
 * Sets the system root to operate on.
 *
 * Packages will be installed to or removed using this directory as a prefix.
 *
 * @param std::string systemRoot
 * @param bool silent
 *
 * @returns bool isSystemRootADirectory
 */
bool Options::setSystemRoot(std::string sr, bool silent) {/*{{{*/
    // Validate the existence of the system root
    std::error_code e;
    if(std::filesystem::is_directory(sr,e)) {
        systemRoot = sr;
        return true;
    }

    else {
        if(!silent) {
            fprintf(stderr,"Error: System root directory %s does not exist or is not a directory.\n",sr.c_str());
            fprintf(stderr,e.message().c_str());
        }

        return false;
    }
}/*}}}*/

// @TODO Refactor this to have an actual integer verbosity level like everything else
/**
 * Sets the folder path in which we should look for tar packages.
 *
 * @param std::string tarLibraryPath
 * @param bool silent
 *
 * @param return didTarLibraryPathExist
 */
bool Options::setTarLibraryPath(std::string tlp, bool silent) {/*{{{*/
    // Validate the existence of the tar library
    std::error_code e;
    if(std::filesystem::exists(tlp,e)) {
        tarLibraryPath = tlp;
        return true;
    }

    else {
        if(!silent) {
            fprintf(stderr,"Error: Tar package library %s does not exist\n",tlp.c_str());
            fprintf(stderr,"%s\n",e.message().c_str());
        }
        
        return false;
    }
}/*}}}*/

// @TODO Refactor this to have an actual integer verbosity level like everything else
/**
 * Sets the folder path where index files of which packages are and are not installed are stored.
 *
 * This folder is where the files created by and removed by the follow/unfollow operations (called either on their own or by install/uninstall) are stored.
 *
 * @param std::string setInstalledPkgIndexDirectory
 * @param bool silent
 *
 * @returns bool didInstalledPkgIndexDirectoryExist
 */
bool Options::setInstalledPkgsPath(std::string ipp, bool silent) {/*{{{*/
    // Validate the existence of the installed pkg path
    std::error_code e;
    if(std::filesystem::exists(ipp,e)) {
        installedPkgsPath = ipp;
        return true;
    }

    else {
        if(!silent) {
            fprintf(stderr,"Error: Installed pkg dir %s does not exist",ipp.c_str());
            fprintf(stderr,e.message().c_str());
        }

        return false;
    }
}/*}}}*/

// @TODO Refactor this to have an actual integer verbosity level like everything else
/**
 * Sets the files which should be ignored when installing or uninstalling packages.
 *
 * The actual implementation of excluded files is not complete. As such, this option currently does nothing.
 *
 * Always returns true
 *
 * @param std::set<std::string> excludedFileSet
 * @param bool silent
 *
 * @returns bool true
 */
bool Options::setExcludedFiles(std::set<std::string> ef, bool silent) {/*{{{*/
    excludedFiles = ef;
    return true;
}/*}}}*/

// @TODO Refactor this to have an actual integer verbosity level like everything else
/**
 * OR's a given value with the current option mask.
 *
 * This is the recommended usage for option masks. The input should always be a power of 2, and is checked against the set "validOptMasks". Calling this function with 0 as the first argument does nothing.
 *
 * @param unsigned int optMaskToAdd
 * @param bool silent
 *
 * @returns wasOptMaskValid
 */
bool Options::addToOptMask(unsigned int opt, bool silent) {/*{{{*/
    // Validate the optMask value
    if(validOptMaskVals.find(opt) != validOptMaskVals.end()) {
        optMask |= opt;
        return true;
    }

    else {
        if(!silent) {
            fprintf(stderr,"Error: An invalid optMask value was passed to Options\n");
        }
        
        return false;
    }
}/*}}}*/

// @TODO Refactor this to have an actual integer verbosity level like everything else
/**
 * This adds the file path given to the excluded files set. Currently, however, the usage of that set is not implemented. As such, this option currently does not do anything.
 *
 * Always returns true
 *
 * @param std::string pathToAdd
 * @param bool silent
 *
 * @returns bool true
 */
bool Options::addToExcludedFiles(std::string path, bool silent) {/*{{{*/
    excludedFiles.insert(path);
    return true;
}/*}}}*/

// @TODO Refactor this to have an actual integer verbosity level like everything else
/**
 * Turns a mode string into an integer, per modeStrToInt
 *
 * @param std::string modeString
 * @param bool silent
 *
 * @returns unsigned int modeIndex
 */
unsigned int Options::translateMode(std::string m, bool silent) {/*{{{*/
    if(modeStrToInt.find(m) != modeStrToInt.end()) {
        return modeStrToInt.at(m);
    }

    else {
        if(!silent) {
            fprintf(stderr,"Error: pkg-mgr requires a valid mode of operation. Check pkg-mgr -h for more information.\n");
        }
        exit(-1);
    }

    // This should never get here, but just to be safe, catch it and return NOP
    return NOP;
}/*}}}*/

// @TODO Test
// @TODO Verbosity
// @TODO Refactor this to have an actual integer verbosity level like everything else
/**
 * Applies a Config object (or any object implementing IConfigMap, in Config.h) to the Options object which calls this function.
 *
 * Any matching keys are overridden, and thus user configurations should be read last, such that the user's own options take priority over the global configuration file. Values set by the user via CLI are processed first, however. To avoid overwriting the user's manual options, this function checks against the optMask for whether or not the option was set via CLI. If so, the configuration file is ignored.
 *
 * @param IConfigMap& configMapObject
 * @param bool silent
 *
 * @returns bool wasApplicationSuccessful
 */
bool Options::applyConfig(IConfigMap& conf, bool silent) {/*{{{*/
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
                    if(!setVerbosity(it->second.c_str())) {
                        return false;
                    }
                }

                break;


            case MASK_SMART_OP: 
                // Break for now, print out a warning
                // @TODO
                if(!silent) {
                    fprintf(stderr,"Warning: Smart mode is not yet implemented. Ignoring...\n");
                }

                break;
                                

            case MASK_GLOBAL_CONFIG_PATH: 
                // @TODO Look at this
                if(!silent) {
                    fprintf(stderr,"Uh... Did you put a global configuration file in your configuration file? We're already past the time where we read the configuration file, once we look at it, so I don't know what you thought you were going to accomplish with this...\n");
                }

                if(!setGlobalConfigPath(it->second)) {
                    return false;
                }

                break;
                                          

            case MASK_USER_CONFIG_PATH: 
                // Don't do anything, since we've already read this. Again, here for completion
                break;
                                        

            case MASK_SYSTEM_ROOT: 
                // Validation here is handled in the packages "installPkg" function
                if((mask & MASK_SYSTEM_ROOT) == 0) {
                    if(!setSystemRoot(it->second)) {
                        return false;
                    }
                }

                break;
                                   

            case MASK_TAR_LIBRARY_PATH: 
                // Validation here is handled in the packages "installPkg" function
                if((mask & MASK_TAR_LIBRARY_PATH) == 0) {
                    if(!setTarLibraryPath(it->second)) {
                        return false;
                    }
                }

                break;
                                

            case MASK_INSTALLED_PKG_PATH: 
                if((mask & MASK_INSTALLED_PKG_PATH) == 0) {
                    if(!setInstalledPkgsPath(it->second)) {
                        return false;
                    }
                }

                break;
                                          

            case MASK_EXCLUDED_FILES: 
                if((mask & MASK_EXCLUDED_FILES) == 0) {
                    //@TODO
                    if(!silent) {
                        fprintf(stderr,"Warning: Excluded files not yet implemented...\n");
                    }
                }

                break;
                                      

            default: 
                if(!silent) {
                    fprintf(stderr,"Warning: Unrecognized configuration option %s\n. Attempting to continue normally...",it->first.c_str());
                }
        }
    }

    return true;
}/*}}}*/
