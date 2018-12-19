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

import pdb
import os
import sys
import shutil
import tarfile

__TEST_ENV_ROOT_DIR = str(os.getcwd() + "/" + os.path.dirname(sys.argv[0]) + "/install-test-env/")
__TEST_ENV_INSTALLED_PKG_DIR = str(__TEST_ENV_ROOT_DIR + "installed/")
__TEST_PKGS_DIR = str(os.getcwd() + "/" + os.path.dirname(sys.argv[0]) + "/test-pkgs/")
__TEST_CONFIG_PATH = str(os.getcwd() + "/" + os.path.dirname(sys.argv[0]) + "/pkg-mgr.conf")
__TEMP_DIR = "/tmp/"
NUM_TEST_TARS = 5
SCRIPT_NAMES = set(["post-install.sh", "post-uninstall.sh", "pre-install.sh", "pre-uninstall.sh"])

PKG_MGR_PATH = os.environ['PKG_MGR_PATH']

def isTarFileInstalledCorrectly(tarPath,systemRoot):
    tf = tarfile.open(tarPath)
    for tarMember in tf.getmembers():
        try:
            if(tarMember.name in SCRIPT_NAMES):
                if(checkScript(tarMember.name, tarPath)):
                    continue

                return False
            elif(os.path.isfile(str(systemRoot + tarMember.name)) or os.path.isdir(str(systemRoot + tarMember.name))):
                continue
            else:
                print("File %s could not be found during installation test. Exiting." % (systemRoot + tarMember.name))
                return False
        except PermissionError:
            print("Permission error when looking for the file %s during installation test. Exiting" % (systemRoot + tarMember.name))
            return False

    return True

def extractTarFile(pkgName, tarLibPath, installedPkgPath, systemRoot, verbosity):
    cmdStr = str(PKG_MGR_PATH + " -mi -s " + systemRoot + " -l " + tarLibPath + " -i " + installedPkgPath + " -v " + str(verbosity) + " -g " + __TEST_CONFIG_PATH + " -u " + __TEST_CONFIG_PATH + " " + pkgName)
    tmpVal = os.system(cmdStr)

def checkScript(scriptName, tarPath):
    # The path build here is equal to "/tmp/" + pkgName + (scriptName sans extention)
    testPath = __TEMP_DIR + os.path.basename(os.path.splitext(tarPath)[0]) + "/" + os.path.basename(scriptName)
    if(os.path.isfile(testPath)):
        return True
    
    return False

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
        shutil.rmtree(__TEST_ENV_ROOT_DIR)
    except FileNotFoundError:
        return 1
    except PermissionError:
        return -1

    return 0

def removeScriptFolders():
    for index in range(NUM_TEST_TARS):
        path = __TEMP_DIR + "test" + str(index)
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

        if(extractTarFile(pkgName, tarDir, __TEST_ENV_INSTALLED_PKG_DIR, __TEST_ENV_ROOT_DIR, 0) == -1):
            print("Permission error while extracting the tar file %s for the installation test... Exiting" % pkgPath)
            removeDirTree(__TEST_ENV_ROOT_DIR)
            removeScriptFolders()
            sys.exit(2)

        print("Installation complete!")
        print("Validating the correctness of the installation")

        if(isTarFileInstalledCorrectly(pkgPath, __TEST_ENV_ROOT_DIR)):
            print("Installation test passed!")
            removeDirTree(__TEST_ENV_ROOT_DIR)
            removeScriptFolders()
            sys.exit(0)

        else:
            print("Error: pkg-mgr failed to install the package %s correctly. Exiting..." % (pkgName))
            sys.exit(1)
