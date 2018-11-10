# @author Thomas Lenz <thomas.lenz96@gmail.com> AS The2b
# @date 03 December 2018
# @project Package Manager
# @file tstUninstall.py
#
# This script tests a built pkg-mgr's ability to uninstall packages correctly
#
# To do so, it does the following:
#   Create a clean environment
#   Use python's tar library to unpack the package into a test folder
#   Use pkg-mgr's uninstaller to uninstall the package
#   If no files from within the tar file exist, return 0
#   If any file exists, return -1
#   Do this for all 5 test packages

import os
import sys
import shutil
import tarfile

__TEST_ENV_ROOT_DIR = str(os.getcwd() + "/" + os.path.dirname(sys.argv[0]) + "/uninstall-test-env/")
__TEST_ENV_INSTALLED_PKG_DIR = str(__TEST_ENV_ROOT_DIR + "installed/")
__TEST_PKGS_DIR = str(os.getcwd() + "/" + os.path.dirname(sys.argv[0]) + "/test-pkgs/")
NUM_TEST_TARS = 5

PKG_MGR_PATH = os.environ['PKG_MGR_PATH']

def extractTarFile(pkgPath, systemRoot):
    tf = tarfile.open(pkgPath)
    try:
        tf.extractall(path=systemRoot)
    except PermissionError:
        print("Permission error when unpacking the tar %s to prepare for the uninstall test",pkgPath)
        return -1

    return 0

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

def isTarFileUninstalledCorrectly(pkgPath, systemRoot):
    tf = tarfile.open(pkgPath)
    for tarMember in tf.getmembers():
        if(os.path.isfile(str(systemRoot + tarMember.name)) or os.path.isdir(str(systemRoot + tarMember.name))):
            print("The file %s was still found on the system..." % str(systemRoot + tarMember.name))
            return False

    return True

# @TODO
def uninstallTarPkg(pkgName, systemRoot, tarLibraryPath, installedPkgPath, verbosity):
    cmdStr = PKG_MGR_PATH + " -mu -s " + systemRoot + " -i " + installedPkgPath + " -l " + tarLibraryPath + " -v " + str(verbosity) + " " + pkgName
    if(os.system(cmdStr) != 0):
        return -1

if __name__ == '__main__':
    print("Cleaning up old test environment...")
    if(removeDirTree(__TEST_ENV_ROOT_DIR) == -1):
        print("Permission error when attempting to remove an old test environment. Exiting...")
        sys.exit(-1)
    print("Done cleaning up old test environment!")

    print("Creating test environment...")
    if(createTestEnvironment() == -1):
        print("Permission error while creating the test environment for the uninstallation test... Exiting")
        sys.exit(-1)
    print("Test environment created successfully!")

    for index in range(0,NUM_TEST_TARS):
        tarDir = str(__TEST_PKGS_DIR)
        pkgName = "test" + str(index)
        ext = ".tar"

        print("Unpacking %s to %s..." % (str(tarDir + pkgName + ext),__TEST_ENV_ROOT_DIR))

        if(extractTarFile(str(tarDir + pkgName + ext), __TEST_ENV_ROOT_DIR) == -1):
            print("Permission error while extracting the tar file %s for the uninstallation test... Exiting" % (tarDir + pkgName + ext))
            removeDirTree(__TEST_ENV_ROOT_DIR)
            sys.exit(-1)

        print("Uninstalling package %s with pkg-mgr..." % pkgName)

        if(uninstallTarPkg(pkgName, __TEST_ENV_ROOT_DIR, __TEST_PKGS_DIR, __TEST_ENV_INSTALLED_PKG_DIR, 4) == -1):
            print("An error occured while uninstalling the package %s... Exiting" % (tarDir + pkgName + ext))
            sys.exit(-2)

        print("Package uninstalled!")

        print("Verifying uninstalltion...")
        if(isTarFileUninstalledCorrectly(str(tarDir + pkgName + ext), __TEST_ENV_ROOT_DIR)):
            print("Uninstallation test passed!")
            removeDirTree(__TEST_ENV_ROOT_DIR)
            sys.exit(0)

        else:
            print("Error: pkg-mgr failed to uninstall the package %s correctly. Exiting..." % (pkgName))
            sys.exit(-1)
