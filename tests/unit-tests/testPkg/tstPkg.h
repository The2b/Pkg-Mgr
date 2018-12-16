#ifndef _THE2B_TST_PKG_H
#define _THE2B_TST_PKG_H

#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <libgen.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
// @TODO See if these are used in the source file. We can compile with or without them, but I think they're being called from one of the prior includes, and I'd like to have them if they're in my code for clarity that its required, even if it can be compiled without it
//#include <sys/stat.h>
//#include <sys/types.h>

#include <cppunit/TestAssert.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestCaller.h>
#include <cppunit/ui/text/TestRunner.h>

#include "Pkg.h"

#define TEST_TAR_COUNT 5
#define VERBOSITY 0

#define BASE_DIR "test-env/"
#define PKG_DIR "test-env/pkgs/"
#define INSTALLED_DIR "test-env/installed/"
#define HASH_DIR "test-env/hashes/"
#define FAKEROOT "test-env/sysroot/"
#define TEST_PKG_DIR "testPkgs/"

class PkgTest : public CppUnit::TestFixture {
    private:
        std::vector<Pkg*> pkgVector;
        std::vector<const char*> paths;

    public:
        // Data which we use during the installation test to verify the files are extracting as expected
        typedef struct fileData_s {
            const char* path;
            unsigned char* checksum;
            int numTar;
        } filedata;

        // First index is the package index, second is a pair of file paths and their md5 checksum values
        std::vector<filedata> fileVector;

        // Pre- and post- suite functions
        void setUp();
        void tearDown();

        // Member var tests
        void testPkgName();
        void testPathname();

        // Functionality tests
        void testInstall();
        void testUninstall();
        void testFollow();
        void testUnfollow();
        void testPreInstallScript();
        void testPostInstallScript();
        void testPreUninstallScript();
        void testPostUninstallScript();

        // Pre and post test functions
        int preTestMemberVars();
        int preTestPkgName();
        int preTestPathname();
        int postTestMemberVars();
        int postTestPkgName();
        int postTestPathname();

        int preTestFunctionality();
        int preTestInstall();
        int preTestUninstall();
        int preTestFollow();
        int preTestUnfollow();
        int preTestPreInstallScript();
        int preTestPostInstallScript();
        int preTestPreUninstallScript();
        int preTestPostUninstallScript();
        int postTestFunctionality();
        int postTestInstall();
        int postTestUninstall();
        int postTestFollow();
        int postTestUnfollow();
        int postTestPreInstallScript();
        int postTestPostInstallScript();
        int postTestPreUninstallScript();
        int postTestPostUninstallScript();

        // Test Suite functions
        static CppUnit::Test* memberVarSuite();
        static CppUnit::Test* functionalitySuite();

        int buildTestEnvironment();
        int buildTestFolders();
        int rebuildTestEnvironment();
        int copyPkgs();
        void buildPkgObjects();
};

// Utility functions independent of the class
int removeFullPath(const char* path);

#endif /* _THE2B_PKG_TST_H */
