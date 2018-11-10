/**
 * @author Thomas Lenz <thomas.lenz96@gmail.com> AS The2b
 * @date 12 November 2018
 * @project Package Manager
 * @file Starter.cpp
 * @error -300
 *
 * pkg-mgr [-dv] [-c /path/to/config/file] [-r /path/to/system/root/] [-l /path/to/tarball/library/] [-i /path/to/installed/pkg/directory/] -m command [command-specific-options...]
 *
 * @TODO Revamp error codes
 * @TODO Make any function which calls exit outside of this file instead return false or -1, and check for that here. That'll make testing easier.
 * @TODO Break some of these things apart. There's too much here for a main script.
 */

#include <stdio.h>      // printf, fprintf
#include <map>          // maps
#include <system_error>

#include "Options.h"
#include "Config.h"
#include "Pkg.h"

// @TODO Remove this, check for the stem, NOT the full name, since we can't guarentee that they'll just be tars
#define DEFAULT_EXTENSION ".tar"

// @TODO See if I can move this to a header file
// @TODO Add an exclusions option
// @TODO Add a smart option
struct option getopt_options[] =
{
    { "verbosity",              required_argument,  0,  'v' },
    { "global-config",          required_argument,  0,  'g' },
    { "user-config",            required_argument,  0,  'u' },
    { "system-root",            required_argument,  0,  's' },
    { "package-library",        required_argument,  0,  'l' },
    { "installed-pkg-library",  required_argument,  0,  'i' },
    { "mode",                   required_argument,  0,  'm' },
    { "help",                   no_argument,        0,  'h' },
    { 0,                        0,                  0,  0   }
};

// Forward declaration of functions
void printHelp();
void parseOptions(Options& opts, char* argv[], int argc, char*& optarg, int& optind);

int main(int argc, char* argv[]) {

    // getopt vars
    extern char *optarg;
    extern int optind;

    Options options = Options(NOP);

    parseOptions(options, argv, argc, optarg, optind);

    // I want to focus on 6 modes of operation which should be easy enough: install, uninstall, follow, unfollow, list-all, and list-installed
    // After that, search and owner, and then go into release v1
    // Other functions I want to add: import, purge, interactive mode, package user creation
    // Depending on the mode, we'll execute a different operation
    // Mode validation is done in Options::setMode() for actual -m values
    // Verification that there IS a -m value is done here
    if((options.getOptMask() & MASK_MODE) == 0) {
        fprintf(stderr,"Error: pkg-mgr requires a valid mode of operation. Check pkg-mgr -h for more information.\n");
        exit(-1);
    }
    
    // Listing all or listing installed make sense without additional options, so do them up here, and return if we do.
    // However, it does not make sense to do these with additional arguments, so verify there are none. If there are, say something and exit
    if((options.getModeIndex() == LIST_ALL || options.getModeIndex() == LIST_INSTALLED) && optind < argc) {
        if(options.getVerbosity() != 0) {
            fprintf(stderr,"Error: The mode %s cannot be used with additional packages\n",options.getModeStr().c_str());
        }
        exit(-303);
    
    }

    // If the user explicitly gives us a configuration file, make sure it exists
    bool globalConfigExists = std::filesystem::exists(options.getGlobalConfigPath());

    if((options.getOptMask() & MASK_GLOBAL_CONFIG_PATH) && !globalConfigExists) {
        if(options.getVerbosity() != 0) {
            fprintf(stderr,"Error: Specified global configuration file %s does not exist\n",options.getGlobalConfigPath().c_str());
        }

        exit(-306);
    }

    // Since we need to re-check if this exists later, don't bother storing it
    if((options.getOptMask() & MASK_USER_CONFIG_PATH) && !std::filesystem::exists(options.getUserConfigPath())) {
        if(options.getVerbosity() != 0) {
            fprintf(stderr,"Error: Specified user configuration file %s does not exist\n",options.getUserConfigPath().c_str());
        }
        exit(-307);
    }

    // Read in our configuration files, starting with the global conf, then the user conf
    // Do not change values set by flags
    // Start with a blank config file. If we find the global, merge it. Same for the user config.
    Config masterConfig = Config("/dev/null", options.getVerbosity());
    if(globalConfigExists) {
        Config globalConfig = Config(options.getGlobalConfigPath(), options.getVerbosity());
        mergeConfig(masterConfig, globalConfig, options.getVerbosity());

        // If we weren't given an explicit user config path, check the global config
        if(options.getOptMask() & MASK_USER_CONFIG_PATH == 0) {
            if(masterConfig.getConfigMap()->find(KEY_USER_CONFIG_PATH) != masterConfig.getConfigMap()->end()) {
                // I don't want to add this key if it isn't there, which the [] operator will. at() throws an exception if it can't find it, which is very helpful here
                std::string userCfgPath = masterConfig.getConfigMap()->at(KEY_USER_CONFIG_PATH);
                options.setUserConfigPath(userCfgPath);
            }
        }
    }

    // We don't actually need the userConfig object to exist outside of this scope. Instead, we can just use our 
    if(std::filesystem::exists(options.getUserConfigPath())) {
        Config userConfig = Config(options.getUserConfigPath(), options.getVerbosity());

        // Transpose the user config's map onto the global config's
        mergeConfig(masterConfig, userConfig, options.getVerbosity());
    }

    // Apply the config to our current options
    options.applyConfig(masterConfig);
    
    switch(options.getModeIndex()) {
        case LIST_ALL:
            listAllPkgs(options.getTarLibraryPath(), options.getVerbosity());
            return 0;

        case LIST_INSTALLED:
            listInstalledPkgs(options.getInstalledPkgsPath(), options.getVerbosity());
            return 0;
    }

    // Make sure there are packages listed
    if(argc <= optind) {
        if(options.getVerbosity() != 0) {
            fprintf(stderr,"Error: The mode %s cannot be used without additional packages\n",options.getModeStr().c_str());
        }
    }
    
    // Everything after this is going to be interpreted as packages, which are operated on from left to right based on the mode of operation
    std::vector<Pkg> pkgs;
    std::string tarLibrary = options.getTarLibraryPath();

    while (optind < argc) {
        // Build a list of packages which the user is requesting. By doing this, we can verify they all exist before moving forward, and risking breaking critical components if they require a dependency the user doesn't have or mistyped
        // Note that Pkg.cpp is what does the validation, not this class
        pkgs.push_back(Pkg(std::string(tarLibrary + "/" + argv[optind] + DEFAULT_EXTENSION), options.getVerbosity()));
        optind++;
    }

    for(int index = 0; index < pkgs.size(); index++) {
        int res = 0;

        switch(options.getModeIndex()) {
            case INSTALL:
                if(options.getVerbosity() >= 3) { 
                    printf("Operation: install\nCurrent package: %s\n",pkgs[index].getPkgName().c_str());
                }

                res = pkgs[index].installPkg(options.getSystemRoot(), options.getInstalledPkgsPath(), options.getVerbosity(), options.getExcludedFiles(), options.getSmartOperation());
                if(options.getVerbosity() == 4) {
                    fprintf(stderr,"Errno is %d\n",errno);
                    if(errno != 0) {
                        perror("Error after installation");
                    }
                }

                break;
            case UNINSTALL:
                if(options.getVerbosity() >= 3){ 
                    printf("Operation: uninstall\nCurrent package: %s\n",pkgs[index].getPkgName().c_str());
                }

                res = pkgs[index].uninstallPkg(options.getSystemRoot(), options.getInstalledPkgsPath(), options.getVerbosity(), options.getExcludedFiles(), options.getSmartOperation());
                if(options.getVerbosity() == 4) {
                    fprintf(stderr,"Errno is %d\n",errno);
                    if(errno != 0) {
                        perror("Error after uninstallation");
                    }
                }

                break;
            case FOLLOW:
                if(options.getVerbosity() >= 3){ 
                    printf("Operation: follow\nCurrent package: %s\n",pkgs[index].getPkgName().c_str());
                }

                res = pkgs[index].followPkg(options.getInstalledPkgsPath(), options.getVerbosity());
                if(options.getVerbosity() == 4) {
                    fprintf(stderr,"Errno is %d\n",errno);
                    if(errno != 0) {
                        perror("Error after following");
                    }
                }

                break;
            case UNFOLLOW:
                if(options.getVerbosity() >= 3){ 
                    printf("Operation: unfollow\nCurrent package: %s\n",pkgs[index].getPkgName().c_str());
                }

                res = pkgs[index].unfollowPkg(options.getInstalledPkgsPath(), options.getVerbosity());
                if(options.getVerbosity() == 4) {
                    fprintf(stderr,"Errno is %d\n",errno);
                    if(errno != 0) {
                        perror("Error after unfollowing");
                    }
                }

                break;
            default:
                // Mode of operation is validated when set. This should never occur
                fprintf(stderr,"Operation mode not recognized. This message should never be seen. If you are reading this, please file an issue on the pkg-mgr GitHub repository.\n");
                break;
        }
    }
}

void parseOptions(Options& opts, char* argv[], int argc, char*& optarg, int& optind) {
    int c;

    // Parse our options and react accordingly
    while((c = getopt_long(argc, argv, "hv:g:u:s:l:i:m:", getopt_options, &optind)) != -1) {
        switch(c) {
            case 'v':
                opts.setVerbosity((unsigned int)atoi(optarg));
                opts.addToOptMask(MASK_VERBOSE);
                break;
            case 'm':
                opts.setMode(std::string(optarg));
                opts.addToOptMask(MASK_MODE);
                break;
            case 'g':
                opts.setGlobalConfigPath(optarg);
                opts.addToOptMask(MASK_GLOBAL_CONFIG_PATH);
                break;
            case 'u':
                opts.setUserConfigPath(optarg);
                opts.addToOptMask(MASK_USER_CONFIG_PATH);
                break;
            case 's':
                opts.setSystemRoot(optarg);
                opts.addToOptMask(MASK_SYSTEM_ROOT);
                break;
            case 'l':
                opts.setTarLibraryPath(optarg);
                opts.addToOptMask(MASK_TAR_LIBRARY_PATH);
                break;
            case 'i':
                opts.setInstalledPkgsPath(optarg);
                opts.addToOptMask(MASK_INSTALLED_PKG_PATH);
                break;
            case 'h':
                printHelp();
                exit(0);
            default:
                fprintf(stderr,"Unrecognized option %c\n",c);
                exit(-300);
        }
    }
}

// @TODO
void printHelp() {
    printf("Usage: pkg-mgr [-h] [-v n] [-g /path/to/file] [-u /path/to/file] [-s /path/to/sys/root/] [-l /path/to/pkgs] [-i /path/to/installed/pkgs] -m mode package(s)\n");
    printf("\n");
    printf("    -m, --mode: The mode of operation; one of [i]nstall, [u]ninstall, [f]ollow, [u]n[f]ollow, [l]ist-[a]ll, [l]ist-[i]nstalled\n");
    printf("    -v, --verbosity: When followed by an integer between 0 and 4, the verbosity is set to that level. 0 silences output, 1 only prints warnings and errors. Default setting: %d\n",DEFAULT_VERBOSITY);
    printf("    -g, --global-config: The path to the global config file. Any options in here can be overridden by the user config file. Default setting: %s\n",DEFAULT_GLOBAL_CONFIG_PATH);
    printf("    -u, --user-config: The path to the user config file. This file overrides the global config file. Default setting: %s\n",DEFAULT_USER_CONFIG_PATH);
    printf("    -s, --system-root: The path to the root directory to install packages to, or to uninstall them from. Default setting: %s\n",DEFAULT_SYSTEM_ROOT);
    printf("    -l, --package-library: The path the package tarballs are stored. Default setting: %s\n",DEFAULT_TAR_LIBRARY_PATH);
    printf("    -i, --installed-pkg-library: The path to the installed-pkgs directory. Default setting: %s\n",DEFAULT_INSTALLED_PKG_PATH);
    printf("    -h, --help: Print this help message\n");
    printf("\n");
    printf("Any number of packages can be listed, unless in one of the list modes. Packages will be operated on from left to right.\n");
}
