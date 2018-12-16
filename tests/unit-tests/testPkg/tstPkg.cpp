#include "tstPkg.h"

std::string execDir;

int main( int argc, char **argv) {
    execDir = std::string(dirname(argv[0]));

    CppUnit::TextUi::TestRunner mvsRunner;
    CppUnit::TextUi::TestRunner funcRunner;
    mvsRunner.addTest( PkgTest::memberVarSuite() );
    funcRunner.addTest( PkgTest::functionalitySuite() );

    mvsRunner.run();
    funcRunner.run();

    removeFullPath(BASE_DIR);

    return 0;
}

CppUnit::Test* PkgTest::memberVarSuite() {
    CppUnit::TestSuite* varSuite = new CppUnit::TestSuite( "PkgTest" );

    varSuite->addTest( new CppUnit::TestCaller<PkgTest>( "testPkgName", &PkgTest::testPkgName ));
    varSuite->addTest( new CppUnit::TestCaller<PkgTest>( "testPathname", &PkgTest::testPathname ));

    return varSuite;
}

CppUnit::Test* PkgTest::functionalitySuite() {
    CppUnit::TestSuite* funcSuite = new CppUnit::TestSuite( "PkgTest" );

    funcSuite->addTest( new CppUnit::TestCaller<PkgTest>( "testInstall", &PkgTest::testInstall ));
    funcSuite->addTest( new CppUnit::TestCaller<PkgTest>( "testUninstall", &PkgTest::testUninstall ));
    funcSuite->addTest( new CppUnit::TestCaller<PkgTest>( "testFollow", &PkgTest::testFollow ));
    funcSuite->addTest( new CppUnit::TestCaller<PkgTest>( "testUnfollow", &PkgTest::testUnfollow ));

    return funcSuite;
}

void PkgTest::setUp() {
    // Build the packages
    paths.push_back(BASE_DIR);
    paths.push_back(PKG_DIR);
    paths.push_back(INSTALLED_DIR);
    paths.push_back(FAKEROOT);
    paths.push_back(HASH_DIR);

    // Set up the environment
    if(std::filesystem::exists(BASE_DIR)) {
        removeFullPath(BASE_DIR);
        if(std::filesystem::exists(BASE_DIR)) {
            fprintf(stderr,"Could not clean the testing environment during the initial setup. %s\n",strerror(errno));
            exit(-1);
        }
    }

    if(buildTestEnvironment() == -1) {
        fprintf(stderr,"Error: Could not create the test environment during the initial setup. %s\n",strerror(errno));
        exit(-1);
    }

    buildPkgObjects();
}

void PkgTest::tearDown() {
    for(int index = 0; index < pkgVector.size(); index++) {
        delete pkgVector[index];
    }

    pkgVector.erase(pkgVector.begin(),pkgVector.end());
}

/* Member var test suite functions */
void PkgTest::testPkgName() {
    preTestPkgName();

    for(int index = 0; index < pkgVector.size(); index++) {
        Pkg pkg = *pkgVector[index];
        std::string pkgName = "test" + std::to_string(index);

        CPPUNIT_ASSERT( pkg.getPkgName() == pkgName );
    }

    postTestPkgName();
}

void PkgTest::testPathname() {
    preTestPathname();

    for(int index = 0; index < pkgVector.size(); index++) {
        Pkg pkg = *pkgVector[index];
        std::string pkgName = "test" + std::to_string(index) + ".tar";
        std::string pkgPath = PKG_DIR + pkgName;

        CPPUNIT_ASSERT( pkg.getPathname() == pkgPath );
    }

    postTestPathname();
}
/* End member var test suite functions */

/* Functionality test suite */
// @TODO Beef this up
void PkgTest::testInstall() {
    preTestInstall();

    // Install each package
    for(int index = 0; index < pkgVector.size(); index++) {
        Pkg pkg = *pkgVector[index];
        
        // Install the package
        int res = pkg.installPkg(FAKEROOT, INSTALLED_DIR, VERBOSITY, std::set<std::string>(), false);
        CPPUNIT_ASSERT(res == ARCHIVE_OK || res == ARCHIVE_EOF);
    }

    postTestInstall();
}

void PkgTest::testUninstall() {
    preTestUninstall();

    // Uninstall the packages
    for(int index = 0; index < pkgVector.size(); index++) {
        // Save the current directory contents for later
        std::filesystem::directory_iterator di(FAKEROOT);
        
        // Run the function
        pkgVector[index]->uninstallPkg(FAKEROOT,INSTALLED_DIR, VERBOSITY);

        // Verify that packages folders are not there, and that no other folders were touched
        for(std::filesystem::directory_iterator it = std::filesystem::begin(di); it != std::filesystem::end(di); it++) {
            bool exists = it->exists();
            (it->path() == pkgVector[index]->getPathname()) ? CPPUNIT_ASSERT(!exists) : CPPUNIT_ASSERT(exists);
        }
    }

    postTestUninstall();
}

void PkgTest::testFollow() {
    preTestFollow();
    
    for(int index = 0; index < pkgVector.size(); index++) {
        pkgVector[index]->followPkg(INSTALLED_DIR, VERBOSITY);
        
        std::string filePath = INSTALLED_DIR;
        filePath += pkgVector[index]->getPkgName();

        // Verify it exists
        CPPUNIT_ASSERT(std::filesystem::exists(filePath));
    }

    postTestFollow();
}

void PkgTest::testUnfollow() {
    preTestUnfollow();

    for(int index = 0; index < pkgVector.size(); index++) {
        pkgVector[index]->unfollowPkg(INSTALLED_DIR, VERBOSITY);

        std::string filePath = INSTALLED_DIR;
        filePath += pkgVector[index]->getPkgName();

        // Verify it does not exist
        CPPUNIT_ASSERT(!std::filesystem::exists(filePath));
    }

    postTestUnfollow();
}

// @TODO
// Just have an archive with the script and nothing else. Have the script touch a file. Check for that file's existence.
void PkgTest::testPreInstallScript() {
    preTestPreInstallScript();

    postTestPreInstallScript();
}

void PkgTest::testPostInstallScript() {
    preTestPostInstallScript();

    postTestPostInstallScript();
}

void PkgTest::testPreUninstallScript() {
    preTestPreUninstallScript();

    postTestPreUninstallScript();
}

void PkgTest::testPostUninstallScript() {
    preTestPostUninstallScript();

    postTestPostUninstallScript();
}
/* End functionality test suite */

/* Pre- and post-test functions */
int PkgTest::preTestMemberVars() {
    return 0;
}

int PkgTest::preTestPkgName() {
    return 0;
}

int PkgTest::preTestPathname() {
    return 0;
}

int PkgTest::preTestFunctionality() {
    int res = rebuildTestEnvironment();
    return res;
}

int PkgTest::preTestInstall() {
    buildPkgObjects();
    return 0;    
}

int PkgTest::preTestUninstall() {
    rebuildTestEnvironment();

    // So, uh... Yeah, I'm just going to leave this here. I'm tired and unit tests suck
    // @TODO Make this less lazy
    for(int index = 0; index < pkgVector.size(); index++) {
        std::string command = "tar -C ";
        command += FAKEROOT;
        command += " -xf ";
        command += PKG_DIR;
        command += "/test" + std::to_string(index) + ".tar";

        system(command.c_str());
    }

    buildPkgObjects();

    return 0;
}

int PkgTest::preTestFollow() {
    // Verify the files are not in the installed directory. If they are, remove them.
    for(int index = 0; index < pkgVector.size(); index++) {
        std::string path = INSTALLED_DIR;
        path += pkgVector[index]->getPkgName();

        if(std::filesystem::exists(path)) {
            int res = std::filesystem::remove(path);
            if(res != 0) {
                fprintf(stderr,"Error: Could not remove file %s when preparing for the follow function test. %s\n",path.c_str(),strerror(errno));
                return -1;
            }
        }
    }

    buildPkgObjects();
    return 0;
}

int PkgTest::preTestUnfollow() {
    // Check if files are in the installed directory. If they are not, create them
    for(int index = 0; index < pkgVector.size(); index++) {
        std::string path = INSTALLED_DIR;
        path += pkgVector[index]->getPkgName();

        if(!std::filesystem::exists(path)) {
            std::ofstream ofs(path);
            ofs.close();

            if(!std::filesystem::exists(path)) {
                fprintf(stderr,"Error: Could not create file %s when preparing for the unfollow function test. %s\n",path.c_str(),strerror(errno));
                return -1;
            }
        }
    }

    buildPkgObjects();
    return 0;
}

int PkgTest::postTestMemberVars() {
    return 0;
}

int PkgTest::postTestPkgName() {
    return 0;
}

int PkgTest::postTestPathname() {
    return 0;
}

int PkgTest::postTestFunctionality() {
    removeFullPath(BASE_DIR);
    return 0;
}

int PkgTest::postTestInstall() {
    tearDown();
    return 0;
}

int PkgTest::postTestUninstall() {
    tearDown();
    return 0;
}

int PkgTest::postTestFollow() {
    tearDown();
    return 0;
}

int PkgTest::postTestUnfollow() {
    tearDown();
    return 0;
}

int PkgTest::preTestPreInstallScript() {
    return 0;
}

int PkgTest::preTestPostInstallScript() {
    return 0;
}

int PkgTest::preTestPreUninstallScript() {
    return 0;
}

int PkgTest::preTestPostUninstallScript() {
    return 0;
}

int PkgTest::postTestPreInstallScript() {
    return 0;
}

int PkgTest::postTestPostInstallScript() {
    return 0;
}

int PkgTest::postTestPreUninstallScript() {
    return 0;
}

int PkgTest::postTestPostUninstallScript() {
    return 0;
}

/* Utility functions */
// This only exists because I didn't realize there was a thing, and haven't gotten around to refactoring it yet
// @TODO Refactor this away
int removeFullPath(const char *path) {
    if(std::filesystem::exists(path)) {
        bool res = std::filesystem::remove_all(path);
        return (res ? 0 : -1);
    }

    return 0;
}

int PkgTest::rebuildTestEnvironment() {
    // Remove the base dir
    int res = 0;
    res = removeFullPath(BASE_DIR);

    // Verify
    if(res != 0) {
        fprintf(stderr,"Error: Could not remove test enviroment when rebuilding it. %s\n",strerror(errno));
        return -1;
    }

    if(buildTestEnvironment() == -1) {
        return -1;
    }

    return copyPkgs();
}

int PkgTest::buildTestEnvironment() {
    int res = 0;
    res = buildTestFolders();
    return ((res == 0) ? copyPkgs() : res);
}

int PkgTest::buildTestFolders() {
    int res = 0;

    for(int index = 0; index < paths.size(); index++) {
        res = mkdir(paths[index], S_IRWXU | S_IRWXG | S_IRWXO);

        if(res == -1) {
            fprintf(stderr,"Error: A portion of the temporary environment (%s) could not be built. strerror = %s\n",paths[index],strerror(errno));
            return -1;
        }

        // If it exists, die, because it should have been cleaned up already
        else if(res == 1) {
            fprintf(stderr,"Error: A portion of the temporary environment continued to exist after the whole thing should have been smited. Exiting...\n");
            return -1;
        }
    }

    return 0;
}

// I like having things in RAM
int PkgTest::copyPkgs() {
    // Copy over our packages
    for(int index = 0; index < TEST_TAR_COUNT; index++) {
        std::string relPkgPath = "test" + std::to_string(index) + ".tar";

        std::string srcPkgPath = execDir + "/" + TEST_PKG_DIR + relPkgPath;
        std::string dstPkgPath = PKG_DIR + relPkgPath;

        if(std::filesystem::exists(dstPkgPath)) {
            if(!std::filesystem::remove(dstPkgPath)) {
                fprintf(stderr,"Error: Could not remove the test package %s while preparing for tstPkg...\n",dstPkgPath.c_str());
            }
        }

        bool res = std::filesystem::copy_file(srcPkgPath,dstPkgPath);

        if(!res) {
            fprintf(stderr,"Error: Could not copy the test package %s to %s while preparing for tstPkg...\n",srcPkgPath.c_str(),dstPkgPath.c_str());
            return -1;
        }
    }   

    return 0;
}

// This needs to happen because of how libtar handles tar headers
void PkgTest::buildPkgObjects() {
    // Build our packages
    for(int index = 0; index < TEST_TAR_COUNT; index++) {
        std::string pkgPath = PKG_DIR;
        pkgPath += "test" + std::to_string(index) + ".tar";
        pkgVector.push_back(new Pkg(pkgPath, 0));
    }
}
/* End utility functions */
