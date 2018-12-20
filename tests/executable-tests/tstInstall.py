# @author Thomas Lenz <thomas.lenz96@gmail.com> AS The2b
# @date 03 December 2018
# @project Package Manager
# @file tstInstall.py
#
# This script tests a built pkg-mgr's ability to install packages correctly
#
# To do so, it does the following:
#   Create a clean environment
#   Use pkg-mgr to install the package into a test folder
#   Then, iterate through the archive and verify its path exists within the test folder
#   If all files exist, return 0
#   If any file is missing, return -1
#   Do this for all 5 test packages

import os
import sys
import shutil
import time
import tarfile

__TEST_ENV_ROOT_DIR = str(os.getcwd() + "/" + os.path.dirname(sys.argv[0]) + "/install-test-env/")
__TEST_ENV_INSTALLED_PKG_DIR = str(__TEST_ENV_ROOT_DIR + "installed/")
__TEST_PKGS_DIR = str(os.getcwd() + "/" + os.path.dirname(sys.argv[0]) + "/test-pkgs/")
__TEST_CONFIG_PATH = str(os.getcwd() + "/" + os.path.dirname(sys.argv[0]) + "/pkg-mgr.conf")
__TEMP_DIR = "/tmp/"
NUM_TEST_TARS = 5
SCRIPT_NAMES = set(["post-install.sh", "pre-install.sh"])

PKG_MGR_PATH = os.environ['PKG_MGR_PATH']

def isTarFileInstalledCorrectly(tarPath,systemRoot):
    tf = tarfile.open(tarPath)
    for tarMember in tf.getmembers():
        try:
            if(tarMember.name in SCRIPT_NAMES):
                # This returns a tuple. The first index is a boolean representing success, the latter is the test path the program looked for
                res = checkScript(tarMember.name, tarPath)
                if(res[0]):
                    print("Script %s extracted and executed correctly" % res[1])
                    #continue # I'm leaving this here as a waring to everyone who tries to fuck with this. I spent hours trying to figure out why this was failing. Turns out, using continue within a try block which is nested in a for loop doesn't start the next iteration of the for loop. Python doesn't complain about it or stop it from running, but it's illegal behavior.
                
                else:
                    print("Script %s failed to extract and execute correctly" % res[1])
                    return (3,tarMember.name,res[1])

            elif(os.path.isfile(str(systemRoot + tarMember.name)) or os.path.isdir(str(systemRoot + tarMember.name))):
                #continue # See above
                pass
            else:
                print("File %s could not be found during installation test. Exiting." % (systemRoot + tarMember.name))
                return (1,tarMember.name)
        except PermissionError:
            print("Permission error when looking for the file %s during installation test. Exiting" % (systemRoot + tarMember.name))
            return (2,tarMember.name)

    return (0,None)

def extractTarFile(pkgName, tarLibPath, installedPkgPath, systemRoot, verbosity):
    cmdStr = str(PKG_MGR_PATH + " -mi -s " + systemRoot + " -l " + tarLibPath + " -i " + installedPkgPath + " -v " + str(verbosity) + " -g " + __TEST_CONFIG_PATH + " -u " + __TEST_CONFIG_PATH + " " + pkgName)
    retVal = os.system(cmdStr)
    return retVal

def checkScript(scriptName, tarPath):
    # The path build here is equal to "/tmp/" + pkgName + (scriptName sans extention)
    scriptType = os.path.splitext(scriptName)[0]
    pkgName = os.path.basename(os.path.splitext(tarPath)[0])

    testPath = __TEMP_DIR + pkgName + "/" + (scriptType)
    if(os.path.isfile(testPath)):
        return (True,testPath)
    
    return (False,testPath)

def createTestEnvironment():
    err = 0
    try:
        os.mkdir(__TEST_ENV_ROOT_DIR)
    except FileExistsError:
        err = 1
    except PermissionError:
        return -1
    try:
        os.mkdir(__TEST_ENV_INSTALLED_PKG_DIR)
    except FileExistsError:
        err = 1
    except PermissionError:
        return -1

    return err

def removeDirTree(path):
    try:
        shutil.rmtree(path)
    except FileNotFoundError:
        return 1
    except PermissionError:
        return -1

    return 0

def removeScriptFolders():
    for index in range(NUM_TEST_TARS):
        for sn in SCRIPT_NAMES:
            path = __TEMP_DIR + "test" + str(index) + "-" + os.path.splitext(sn)[0]
            removeDirTree(path)

if __name__ == '__main__':
    print("Cleaning up old test environment...")
    if(removeDirTree(__TEST_ENV_ROOT_DIR) == -1):
        print("Permission error when attempting to remove an old test environment. Exiting...")
        sys.exit(-1)

    removeScriptFolders()

    print("Done clearing out old test environment!")

    print("Creating test environment...")
    if(createTestEnvironment() == -1):
        print("Permission error while creating the test environment for the installation test... Exiting")
        sys.exit(-1)
    print("Test environment created successfully!")

    for index in range(NUM_TEST_TARS):
        tarDir = str(__TEST_PKGS_DIR)
        pkgName = "test" + str(index)
        ext = ".tar"

        pkgPath = str(tarDir + pkgName + ext)

        print("Installing %s to %s..." % (pkgPath,__TEST_ENV_ROOT_DIR))

        if(extractTarFile(pkgName, tarDir, __TEST_ENV_INSTALLED_PKG_DIR, __TEST_ENV_ROOT_DIR, 4) == -1):
            print("Permission error while extracting the tar file %s for the installation test... Exiting" % pkgPath)
            #removeDirTree(__TEST_ENV_ROOT_DIR)
            #removeScriptFolders()
            sys.exit(2)

        print("Installation complete!")
        print("Validating the correctness of the installation")
    
        # This returns a tuple. The first index is an integer with the exit status. The second is the file which the program failed on (if it failed) or None (if it passed). The third only exists if res[0] is three (script failure), and is the path of the file the program was looking for
        res = isTarFileInstalledCorrectly(pkgPath, __TEST_ENV_ROOT_DIR)

        if(res[0] == 0):
            print("Installation test passed!")
            removeScriptFolders()

        elif(res[0] == 1):
            print("Error: pkg-mgr failed to install the package %s correctly. The file %s failed to install. Exiting..." % (pkgName,res[1]))
            sys.exit(1)

        elif(res[0] == 2):
            print("Error: pkg-mgr failed to install the package %s correctly. There was a permission issue at the file %s. Exiting..." % (pkgName,res[1]))
            sys.exit(2)

        elif(res[0] == 3):
            print("Error: pkg-mgr failed to install the package %s correctly. The script %s failed to extract and execute correctly. The file %s was missing. Exiting..." % (pkgName,res[1],res[2]))
            sys.exit(3)

    removeDirTree(__TEST_ENV_ROOT_DIR)
