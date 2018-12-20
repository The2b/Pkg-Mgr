# @author Thomas Lenz <thomas.lenz96@gmail.com> AS The2b
# @date 03 December 2018
# @project Package Manager
# @file tstFollow.py
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

__TEST_ENV_ROOT_DIR = str(os.getcwd() + "/" + os.path.dirname(sys.argv[0]) + "/follow-test-env/")
__TEST_ENV_INSTALLED_PKG_DIR = str(__TEST_ENV_ROOT_DIR + "/installed/")
__TEST_PKGS_DIR = str(os.getcwd() + "/" + os.path.dirname(sys.argv[0]) + "/test-pkgs/")

NUM_TEST_TARS = 5

PKG_MGR_PATH = os.environ['PKG_MGR_PATH']

# @TODO
def isIndexFileInstalledCorrectly(pkgName, installedPkgDir):
    if(os.path.isfile(str(installedPkgDir + "/" + pkgName))):
        return True
    else:
        print("File %s could not be found during follow test. Exiting." % (installedPkgDir + "/" + pkgName))
        return False

def followPkg(pkgName, installedPkgPath, tarLibrary, verbosity):
    cmdStr = str(PKG_MGR_PATH + " -mf -i " + installedPkgPath + " -l " + tarLibrary + " -v " + str(verbosity) + " " + pkgName)
    os.system(cmdStr)

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

if __name__ == '__main__':
    print("Cleaning up old test environment...")
    if(removeDirTree(__TEST_ENV_ROOT_DIR) == -1):
        print("Permission error when attempting to remove an old test environment. Exiting...")
        sys.exit(-1)

    print("Done clearing out old test environment!")

    print("Creating test environment...")
    if(createTestEnvironment() == -1):
        print("Permission error while creating the test environment for the unfollow test... Exiting")
        sys.exit(-1)
    elif(not os.path.isdir(__TEST_ENV_ROOT_DIR)):
        print("The directory tree starting at %s could not be created... Exiting")
        sys.exit(-2)

    print("Test environment created successfully!")

    for index in range(0,NUM_TEST_TARS):
        pkgName = "test" + str(index)

        print("Following package %s in directory %s..." % (pkgName,__TEST_ENV_INSTALLED_PKG_DIR))

        if(followPkg(pkgName, __TEST_ENV_INSTALLED_PKG_DIR, __TEST_PKGS_DIR, 4) == -1):
            print("Permission error while writing the index file %s for the follow test... Exiting" % (__TEST_ENV_INSTALLED_PKG_DIR + pkgName))
            removeDirTree(__TEST_ENV_ROOT_DIR)
            sys.exit(-1)

        if(isIndexFileInstalledCorrectly(pkgName, __TEST_ENV_INSTALLED_PKG_DIR)):
            print("Follow test passed!")

        else:
            print("Error: pkg-mgr failed to follow the package %s correctly. Exiting..." % (pkgName))
            sys.exit(-1)

    removeDirTree(__TEST_ENV_ROOT_DIR)
