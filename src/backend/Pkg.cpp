/**
 * @author Thomas Lenz <thomas.lenz96@gmail.com> AS The2b
 * @date 08 November 2018
 * @project Package Manager
 * @file Pkg.cpp
 * @error
 *      -100
 */

#include "Pkg.h"

/**
 * This sets up a Pkg object based on the path given.
 *
 * The tar library path is not taken into account by this function, in order to maintain loose coupling of parts. The extention is also required.
 * Currently, only *.tar packages are acceptable, but compressed packages will likely be added later.
 * This sets the package name variable, verifies the existance of the package, and nothing else. Other variables are set only when they are used. Specifically, the package contents are only checked when uninstalling a package. However, this is likely to change when smart operation is implemented.
 */
Pkg::Pkg(std::string path, unsigned int verbosity) {/*{{{*/
    pathname = path;

    // Get the filename, then remove the extension
    pkgName = std::filesystem::path(pathname).stem().string();
    
    // Verify the package actually exists
    if(!std::filesystem::exists(pathname)) {
        if(verbosity != 0) {
            fprintf(stderr,"Package %s could not be found\n",pkgName.c_str());
        }
        exit(-105);
    }
}/*}}}*/

// Builds our pkgContents set
/**
 * This builds a set of paths which the package contains.
 * This is not relative to the system root. Currently, this is only run by uninstallPkg, and is not stored in memory after the return value is garbage collected. However, this may change in the future.
 */
std::set<std::string> Pkg::buildPkgContents(unsigned int verbosity) {/*{{{*/
    std::set<std::string> pkgSet;

    // First, get a new archive struct and enable tar support
    archive* a = archive_read_new();
    if(!openArchiveWithTarSupport(a, pathname.c_str())) {
        return std::set<std::string>{};
    }
    
    archive_entry* ae;
    
    // Read our headers, and add each file path to our set
    while(archive_read_next_header(a,&ae) == ARCHIVE_OK) {
        std::string filePath = archive_entry_pathname(ae);
        pkgSet.insert(filePath);
    }

    return pkgSet;
}/*}}}*/

/**
 * A getter for the file path of the package.
 * 
 * This will always include the tar library path given when constructing the object, since that path is required when doing so.
 */
std::string Pkg::getPathname() {/*{{{*/
    return pathname;
}/*}}}*/

/**
 * A getter for the name of the package.
 */
std::string Pkg::getPkgName() {/*{{{*/
    return pkgName;
}/*}}}*/

/*
// Much like with the pkgContents builder, we need to iterate through each header to extract the files
// @TODO Profile
// @TODO Add in the capability for pre- and post- install scripts
// @TODO Add in checks for whether or not each file already exists in the filesystem (this can be external, and then passed in the exclusions set)
// @TODO Implement quick and smart modes. At the moment, it only operates in quick mode
// @TODO Add a quarentine mode, such that all old files are moved to a temporary directory and then deleted from there after the fact. That will let us catch certain signals, and undo our actions
*/
int Pkg::installPkg(std::string tarPath, std::string root, std::string installedPkgsPath, unsigned int verbosity, std::set<std::string> exclusions, bool quick) {/*{{{*/
    // Before doing anything, we should verify all paths we are given, sans exclusions, actually exist
    if(!std::filesystem::is_directory(std::filesystem::status(root))) {
        if(verbosity != 0) {
            fprintf(stderr,"Error: The installation path must be a directory, or a symbolic link to a directory.\n");
        }
        return -110;
    }

    if(!std::filesystem::is_directory(std::filesystem::status(installedPkgsPath))) {
        if(verbosity != 0) {
            fprintf(stderr,"Error: The \"installed packages\" path must be a directory, or a symbolic link to a directory.\n");
        }
        return -111;
    }

    if(!std::filesystem::exists(tarPath)) {
        if(verbosity != 0) {
            fprintf(stderr,"Error: Tar package path %s does not exist.\n",tarPath.c_str());
            return -112;
        }
    }


    // Add our scripts to our exclusions
    addScriptsToExclusions(exclusions);

    // Open our tar file
    archive* a;
    archive_entry* ae;
    if(!openArchiveWithTarSupport(a, tarPath.c_str(), verbosity)) {
        return -113;
    }

    int err = 0;
    int res = 0;

    // Go through each header, and extract the files/folders
    while((res = archive_read_next_header(a,&ae)) == ARCHIVE_OK && err == 0) {
        // First, change our pathname to reflect our system root
        const char* aePath = archive_entry_pathname(ae);
        std::string new_aePath = root + "/";
        new_aePath += aePath;
        archive_entry_set_pathname(ae,new_aePath.c_str());

        // Check our exception list
        if(exclusions.find(std::string(new_aePath)) == exclusions.end()) {
            err = archive_read_extract(a, ae, ARCHIVE_EXTRACT_ACL | ARCHIVE_EXTRACT_PERM | ARCHIVE_EXTRACT_SECURE_NODOTDOT | ARCHIVE_EXTRACT_XATTR);
        }
    }

    if(err != ARCHIVE_OK && err != ARCHIVE_EOF) {
        if(verbosity != 0) {
            fprintf(stderr,"Error: An error occured while reading the tar file %s.\n",pathname);
        }

        return err;
    }

    return res;

}/*}}}*/

// This function is here to work around the fact that I can't use member vars/functions as default parameters
/**
 * Installs a package using the values set when constructing the object.
 * Calls the superset overload with the objects derived from the constructor.
 */
int Pkg::installPkg(std::string root, std::string installedPkgsPath, unsigned int verbosity, std::set<std::string> exclusions, bool quick) {/*{{{*/
    return installPkg(pathname, root, installedPkgsPath, verbosity, exclusions, quick);
}/*}}}*/

/**
 * Calls installPkg, followPkg, and the appropriate scripts at the approrpiate times
 */
int Pkg::installPkgWithScripts(std::string root, std::string installedPkgsPath, unsigned int verbosity, std::set<std::string> exclusions, bool quick) {/*{{{*/
    // Store our original working directory
    char* oldDir = get_current_dir_name();

    // cd into the system root
    if(!moveToDir(root,verbosity)) {
        return -118;
    }

    // Run our pre-install script, if it exists
    int res = execPreInstallScript(verbosity);
    if(res < 0) {
        if(verbosity != 0) {
            fprintf(stderr,"Error: The pre-install script for the package %s returned error code %d. Bailing out...\n",pkgName.c_str(), res);
        }

        return res;
    }

    // Return to the old directory
    // Don't use moveToDir so we can be more specific with our error
    if(chdir(oldDir) != 0) {
        if(verbosity != 0) {
            fprintf(stderr,"Error: Could not return to the old working directory %s after running the pre-install script. Bailing out...\n",oldDir);
        }

        return -114;
    }
    
    res = installPkg(root,installedPkgsPath,verbosity,exclusions,quick);

    if(res == ARCHIVE_EOF) {
        // cd into the system root
        if(!moveToDir(root,verbosity)) {
            return -113;
        }

        // Run our post-install script, if it exists
        int scriptRes = execPostInstallScript(verbosity);
        if(scriptRes < 0) {
            if(verbosity != 0) {
                fprintf(stderr,"Error: The post-install script for the package %s returned error code %d. Attempting to continue...\n",pkgName.c_str(),res);
            }
        }
        
        // Return to the old directory
        // Don't use moveToDir so we can be more specific with our error
        if(chdir(oldDir) != 0) {
            if(verbosity != 0) {
                fprintf(stderr,"Error: Could not return to the old working directory %s after running the post-install script. This is mostly harmless, unless tests are being run...\n",oldDir);
            }
        }

        if(followPkg(installedPkgsPath, verbosity)) {
            if(verbosity >= 2) {
                printf("The package %s has been installed!\n",getPkgName().c_str());
            }
        }

        else {
            if(verbosity != 0) {
                fprintf(stderr,"The package appears to have been installed, but the database could not be updated. Run \"touch %s\" to update the database\n",(installedPkgsPath + "/" + getPkgName()).c_str());
            }
        }
    }

    else {
        if(verbosity != 0) {
            fprintf(stderr,"Error: The archive ran into an issue while attempting to install the package %s.  Bailing out...\n");
        }

        return -114;
    }

    free(oldDir);

    return res;
}/*}}}*/

// Here, we can just look at the package contents and remove the files
// @TODO Profile
// @TODO Add in the capability for pre- and post- uninstall scripts
// @TODO Add in checks for any shared files between packages (this can be external, and then passed in the exclusions set)
// @TODO Implement quick and smart modes. At the moment, it only operates in quick mode
// @TODO Decide on whether I want more sophisticated decision making, even for quick mode. Specifically, if we should delete pipes, block devices, sockets, character devices, etc
// @TODO Add a quarentine mode, such that all files are moved to a temporary directory and then deleted from there. That will let us catch certain signals, and undo our actions
// @TODO Seperate the installation and the scripts, have another function call them both
/**
 * Removes the files which are contained in a package.
 *
 * Currently, this does not check for other packages which have file collisions with the package being uninstalled. In addition, instead of moving the files to a temporary directory to be removed, it simply removes the files outright, as if calling "rm" on the file from a shell. This means that if we cancel an uninstallation part-way through, the damage cannot be undone. This is likely going to be changed in the future.
 */
int Pkg::uninstallPkg(std::set<std::string> pkgContents, std::string root, std::string installedPkgsPath, unsigned int verbosity, std::set<std::string> exclusions, bool quick) {/*{{{*/
    // Before doing anything, we should verify all paths we are given, sans exclusions, actually exist
    if(!std::filesystem::is_directory(std::filesystem::status(root))) {
        if(verbosity != 0) {
            fprintf(stderr,"Error: The installation path must be a directory, or a symbolic link to a directory.\n");
        }

        return -110;
    }

    if(!std::filesystem::is_directory(std::filesystem::status(installedPkgsPath))) {
        if(verbosity != 0) {
            fprintf(stderr,"Error: The \"installed packages\" path must be a directory, or a symbolic link to a directory.\n");
        }

        return -111;
    }

    // Add our scripts to our exclusions
    addScriptsToExclusions(exclusions);

    // Next, delete all the files within the package
    // To do this, we'll interate through our pkgContents
    // While doing so, we'll remove the files while putting directory paths into another vector
    // After we've deleted all the files, we'll delete all the now-empty directories within our package. We do this so we don't remove the root directory or anything of the like.
    int objectsRemoved = 0;
    int res = 0;

    // Turn our package contents into a vector
    std::vector<std::string> pkgContentsVector(pkgContents.begin(),pkgContents.end());

    // We know this vector is in alphabetical order, due to the stdlib implementation of sets
    // Because of this, if we iterate backwards through the paths, we will definitly get our deepest ones first
    // This means that if only the package wrote to those folders, they'll be empty, and we can safely delete them
    for(int index = pkgContentsVector.size() - 1; index >= 0; index--) {
        // Make a std::filesystem::path object so we can execute fs functions on it
        std::filesystem::path filePath = std::string(root + "/" + pkgContentsVector[index]);

        // If we're supposed to ignore the file, move on to the next one
        if(exclusions.find(filePath) != exclusions.end()) {
            continue;
        }

        // Check if its a directory. May want to check if its a symlink too...
        // @TODO
        bool isDir = std::filesystem::is_directory(filePath);
        bool exists = std::filesystem::exists(filePath);
        bool isEmpty = true;
        
        // This is to prevent errors arising from running is_empty on a non-existent path
        if(exists) {
            isEmpty = std::filesystem::is_empty(filePath);
        }

        // If it's an empty directory or some sort of file, remove it
        // @TODO Make this not be dangerous with things like device files
        if((isDir && isEmpty) || (exists && !isDir)) {
            if(std::filesystem::remove(filePath)) {
                // If we're here, remove returned 0. Tick our counter and move on.
                objectsRemoved++;
                continue;
            }

            else {
                if(verbosity != 0) {
                    fprintf(stderr,"The path %s existed, but could not be removed\n",filePath.c_str());
                }
                res = -1;
            }
        }

        // If it didn't exist, print a message saying so...
        else if (!exists) {
            if(verbosity != 0) {
                fprintf(stderr,"The path %s did not exist in the filesystem. Continuing.\n", filePath.c_str());
            }
        }

        else if (isDir && !isEmpty) {
            if(verbosity != 0) {
                fprintf(stderr,"The path %s is a non-empty directory, and so cannot be removed. Continuing.\n",filePath.c_str());
            }
        }

        else {
            // @TODO Make this less helpful /s
            // Added in strerror. Not sure if that actually makes this more helpful though, considering I don't even know what would cause this.
            if(verbosity != 0) {
                fprintf(stderr,"Error: Something went wrong while uninstalling %s, a part of %s, and we cannot tell what. strerror may. %s\n",filePath.c_str(),pkgName.c_str(),strerror(errno));
            }
        }
    }

    // Finally, return our objectsRemoved count
    return objectsRemoved;
}/*}}}*/

/**
 * Uninstalls a package using the values set when constructing the object.
 * Calls the superset overload with the objects derived from the constructor.
 */
int Pkg::uninstallPkg(std::string root, std::string installedPkgsPath, unsigned int verbosity, std::set<std::string> exclusions, bool quick) {/*{{{*/
    return uninstallPkg(buildPkgContents(verbosity), root, installedPkgsPath, verbosity, exclusions, quick);
}/*}}}*/

/**
 * Calls uninstallPkg, unfollowPkg, and the appropriate scripts at the appropriate times
 */
int Pkg::uninstallPkgWithScripts(std::string root, std::string installedPkgsPath, unsigned int verbosity, std::set<std::string> exclusions, bool quick) {/*{{{*/
    // Store our old working directory
    char* oldDir = get_current_dir_name();

    // cd into the system root
    if(!moveToDir(root,verbosity)) {
        return -112;
    }

    // Run our pre-install script, if it exists
    int res = execPreUninstallScript(verbosity);
    if(res < 0) {
        // Bail out
        if(verbosity < 0) {
            fprintf(stderr,"The pre-uninstall script for the package %s returned an error code. Bailing out...\n",pkgName.c_str());
        }

        return res;
    }

    // Return to the old directory
    if(chdir(oldDir) != 0) {
        if(verbosity != 0) {
            fprintf(stderr,"Error: Could not return to the old working directory %s after running the pre-uninstall script. Bailing out...\n",oldDir);
        }

        return -114;
    }

    res = uninstallPkg(root, installedPkgsPath, verbosity, exclusions, quick);

    // Move back into our system root for the post script
    if(!moveToDir(root,verbosity)) {
        return -116;
    }

    // Res is either less than 0 in case of an error, or >= 0 if there was no error (objectsRemoved)
    if(res >= 0) {
        // Run our post-uninstall script, if it exists
        int scriptRes = execPostUninstallScript(verbosity) < 0;
        if(scriptRes < 0) {
            if(verbosity != 0) {
                fprintf(stderr,"Error: The post-uninstall script for the package %s returned an error code %d. Attempting to continue...\n",pkgName.c_str(), res);
            }
        }

        if(unfollowPkg(installedPkgsPath, verbosity)) {
            if(verbosity >= 2) {
                printf("The package %s has been uninstalled!\n",getPkgName().c_str());
            }
        }
    }

    else {
        if(verbosity != 0) {
            fprintf(stderr,"Error: Something went wrong when removing the package %s: %s\n",pkgName.c_str(),strerror(errno));
        }
    }

    // Return to the old directory
    if(chdir(oldDir) != 0) {
        if(verbosity != 0) {
            fprintf(stderr,"Error: Could not return to the old working directory %s after running the post-uninstall script. This is mostly harmless, unless tests are being run...\n",oldDir);
        }

        return -115;
    }

    free(oldDir);

    return res;
}/*}}}*/

/**
 * Creates a file in the installed package index directory for the given Pkg object.
 *
 * This function verifies whether or not the package is already being followed (file matching the package name in the index directory), and if it is, does not touch it.
 * This is such that the user can still check when the package was followed/installed, even if they call this function after doing so.
 */
bool Pkg::followPkg(std::string installedPkgsPath, unsigned int verbosity) {/*{{{*/
    std::string path = installedPkgsPath + "/" + pkgName;
    bool exists = std::filesystem::exists(path);

    // If it's not there, create it
    if(!exists) {
        std::ofstream o;
        o.open(path);
        o.close();
        
        // Check again, If it's there, say so
        // Update the var so we don't check the disk again at return
        exists = std::filesystem::exists(path);
        if(exists) {
            if(verbosity >= 2) {
                printf("You are now following %s\n",pkgName.c_str());
            }
        }

        else {
            if(verbosity != 0) {
                fprintf(stderr,"Attempt to update the database could not be completed. Run \"touch %s\" to update the database manually\n",(installedPkgsPath + "/" + pkgName).c_str());
            }
        }
    }

    else {
        if(verbosity >= 2) {
            printf("You are already following %s\n",pkgName.c_str());
        }
    }

    return std::filesystem::exists(installedPkgsPath + "/" + pkgName);
}/*}}}*/

/**
 * Removes a file in the installed package index directory for the given Pkg object.
 *
 * This function checks whether or not the file actually exists, and if it does not, prints out a warning.
 */
bool Pkg::unfollowPkg(std::string installedPkgsPath, unsigned int verbosity) {/*{{{*/
    std::string path = installedPkgsPath + "/" + pkgName;
    bool exists = std::filesystem::exists(path);

    // If the file exists, remove it
    if(exists) {
        std::filesystem::remove(path);

        // Make sure its gone, update the var so we don't check the disk again at return
        exists = std::filesystem::exists(path);
        if(!exists) {
            if(verbosity >= 2) {
                printf("You are no longer following %s\n",pkgName.c_str());
            }
        }
        else {
            if(verbosity != 0) {
                fprintf(stderr,"Attempt to update the database could not be completed. Run \"rm %s\" to update the database manually\n",(installedPkgsPath + "/" + pkgName).c_str());
            }
        }
    }

    else {
        if(verbosity >= 2) {
            printf("You are not following %s\n",pkgName.c_str());
        }
    }

    return !exists;
}/*}}}*/

/**
 * Lists all of the packages which we can find in a given directory
 *
 * Per libc standards, none of the functions here can throw exceptions. Therefore, this function will always return true.
 *
 * @param std::string tarLibrary
 * @param unsigned int verbosity
 *
 * @returns bool wasListSuccessful
 */
bool listAllPkgs(std::string libraryPath, unsigned int verbosity) {/*{{{*/
    // Build a (recursive?) directory iterator, and for each tar file, print out its name (path?), sans the extension
    std::filesystem::recursive_directory_iterator di(libraryPath);

    for(auto& p: di) {
        if(p.path().extension() == ".tar") {
            printf("%s\n",p.path().stem().c_str());
        }
    }

    return true;
}/*}}}*/

/**
 * Lists all of the installed packages which we can find in our installed package index directory
 *
 * Per libc standards, none of the functions here can throw exceptions. Therefore, this function will always return true.
 *
 * @param std::string installedPkgsDirectoryt
 * @param unsigned int verbosity
 *
 * @returns bool wasListSuccessful
 */
bool listInstalledPkgs(std::string installedPkgsPath, unsigned int verbosity) {/*{{{*/
    // Build a (recursive?) directory iterator, and for each file, print our its name (path?)
    std::filesystem::recursive_directory_iterator di(installedPkgsPath);

    for(auto& p: di) {
        if(!p.path().has_extension()) {
            printf("%s\n",p.path().filename().c_str());
        }
    }

    return true;
}/*}}}*/

/**
 * Executes the pre-install shell script of the package, if it exists
 * @TODO Allow the user to use any given temporary directory
 * 
 * @param [in] unsigned int verbosity
 *
 * @returns int scriptExitCode
 */
int Pkg::execPreInstallScript(unsigned int verbosity) {/*{{{*/
    const std::string SCRIPT_NAME = PRE_INSTALL_NAME;
    const std::string EXTRACTION_DIR = "/tmp/" + pkgName + "-pre-install/";

    return extractAndExecScript(SCRIPT_NAME, EXTRACTION_DIR, pathname, verbosity);
}/*}}}*/

/**
 * Executes the post-install shell script of the package, if it exists
 * @TODO Allow the user to use any given temporary directory
 * 
 * @param [in] unsigned int verbosity
 *
 * @returns int scriptExitCode
 */
int Pkg::execPostInstallScript(unsigned int verbosity) {/*{{{*/
    const std::string SCRIPT_NAME = POST_INSTALL_NAME;
    const std::string EXTRACTION_DIR = "/tmp/" + pkgName + "-post-install/";

    return extractAndExecScript(SCRIPT_NAME, EXTRACTION_DIR, pathname, verbosity);
}/*}}}*/

/**
 * Executes the pre-uninstall shell script of the package, if it exists
 * @TODO Allow the user to use any given temporary directory
 * 
 * @param [in] unsigned int verbosity
 *
 * @returns int scriptExitCode
 */
int Pkg::execPreUninstallScript(unsigned int verbosity) {/*{{{*/
    const std::string SCRIPT_NAME = PRE_UNINSTALL_NAME;
    const std::string EXTRACTION_DIR = "/tmp/" + pkgName + "-pre-uninstall/";

    return extractAndExecScript(SCRIPT_NAME, EXTRACTION_DIR, pathname, verbosity);
}/*}}}*/

/**
 * Executes the post-uninstall shell script of the package, if it exists
 * @TODO Allow the user to use any given temporary directory
 *
 * @param [in] unsigned int verbosity
 *
 * @returns int scriptExitCode
 */
int Pkg::execPostUninstallScript(unsigned int verbosity) {/*{{{*/
    const std::string SCRIPT_NAME = POST_UNINSTALL_NAME;
    const std::string EXTRACTION_DIR = "/tmp/" + pkgName + "-post-uninstall/";

    return extractAndExecScript(SCRIPT_NAME, EXTRACTION_DIR, pathname, verbosity);
}/*}}}*/

/**
 * Opens an archive for reading and sets up tar support
 *
 * @param [out] archive* archive
 * @param [in] std::string archivePath
 * @param [in] unsigned int verbosity
 *
 * @returns bool success
 */
bool openArchiveWithTarSupport(struct archive*& a, std::string archivePath, unsigned int verbosity) {/*{{{*/
    a = archive_read_new();
    archive_read_support_format_tar(a);
    int res = archive_read_open_filename(a, archivePath.c_str(), TAR_BLOCKSIZE);

    // Verify the archive is still okay
    if(res != ARCHIVE_OK) {
        if(verbosity != 0) {
            fprintf(stderr,"Error: Could not prepare tarball support for package %s. %s\n",archivePath.c_str(),strerror(errno));
        }

        return false;
    }

    return true;
}/*}}}*/

/**
 * Extracts and executes a script from a tarball
 * The script must be in the root of the tarball
 *
 * @param [in] std::string scriptName
 * @param [in] std::stirng extractionDir
 * @param [in] std::string archivePath
 * @param [in] unsigned int verbosity
 */
int extractAndExecScript(std::string scriptName, std::string extractionDir, std::string archivePath, unsigned int verbosity) {/*{{{*/
    // Create the extraction directory if it does not already exist
    std::error_code e;
    if(!std::filesystem::create_directories(extractionDir, e) && e.value() != 0) {
        if(verbosity != 0) {
            fprintf(stderr,"Error: While attempting to create the folder %s to extract the script %s in archive %s, an unknown error occured\n",extractionDir.c_str(),scriptName.c_str(), archivePath.c_str());
            fprintf(stderr,"%s\n",e.message().c_str());
        }

        return -256;
    }
    
    archive* a = archive_read_new();
    archive_entry* ae = archive_entry_new();
    int res = 0;
    bool found = false;

    if(!openArchiveWithTarSupport(a,archivePath,verbosity)) {
        return -256;
    }

    // Read our headers, and see if any of them match our script name
    while((res = archive_read_next_header2(a, ae)) == ARCHIVE_OK) {
        std::string entryPath = archive_entry_pathname(ae);
        if(scriptName == entryPath) {
            if(verbosity >= 3) {
                found = true;
                printf("%s found for package %s\n", scriptName.c_str(), archivePath.c_str());
                break;
            }
        }
    }

    if(verbosity >= 3) {
        printf("Script %s returned %d\n",scriptName.c_str(),res);
    }
    
    // Error
    if(res == -1) {
        return -258;
    }

    // Found the script
    else if(found) {
        // Change the path at which the entry will be extracted
        std::string extractionPath = extractionDir + scriptName;
        archive_entry_set_pathname(ae, extractionPath.c_str());

        // Create a string for the system() call
        // @TODO Allow the shell used to be set on configure
        std::string execCmd = extractionPath;

        // Extract the script
        int err = archive_read_extract(a, ae, ARCHIVE_EXTRACT_ACL | ARCHIVE_EXTRACT_PERM | ARCHIVE_EXTRACT_SECURE_NODOTDOT | ARCHIVE_EXTRACT_XATTR );

        // Run the script
        return system(execCmd.c_str());
    }

    // The script was not found. 256 chosen since shell scripts, to my knowledge, should only return up to 255
    // @TODO Verify this
    else {
        return 256;
    }
}/*}}}*/

/**
 * Adds the pre- and post- install/uninstall scripts to the exclusions list
 *
 * @param [in/out] std::set<std::string>& exclusions
 */
void addScriptsToExclusions(std::set<std::string>& exclusions) {/*{{{*/
    exclusions.insert(PRE_INSTALL_NAME);
    exclusions.insert(POST_INSTALL_NAME);
    exclusions.insert(PRE_UNINSTALL_NAME);
    exclusions.insert(POST_UNINSTALL_NAME);
}/*}}}*/

/**
 * Change working directory to a given path
 * Prints an error message on failure
 *
 * @param [in] std::string path
 * @param [in] unsigned int verbosity
 *
 * @returns bool success
 */
bool moveToDir(std::string path, unsigned int verbosity) {/*{{{*/
    if(chdir(path.c_str()) != 0) {
        if(verbosity != 0) {
            fprintf(stderr,"Error: Attempt to change the working directory to %s failed. %s.\n",path.c_str(),strerror(errno));
        }

        return false;
    }

    return true;
}/*}}}*/
