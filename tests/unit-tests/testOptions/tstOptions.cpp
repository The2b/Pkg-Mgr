/**
 * @author Thomas Lenz <thomas.lenz96@gmail.com> AS The2b
 * @date 26 November 2018
 * @project Package Manager
 * @file tstOptions.cpp
 *
 * So, the source that this test checks has a lot of complex setters. Because this is where we store all of our information, I decided to have all of our option validation (from config files and cli args) in Options.cpp, in order to keep it centralized. Because of this, I want to test the validity of the complex setters, specifically that all intended values are passable, and that a sample of invalid values cannot go through.
 */

#include "tstOptions.h"

int main(int argc, char** argv) {
    CppUnit::TextUi::TestRunner optsRunner;
    optsRunner.addTest(OptionsTest::optionsTestSuite());

    optsRunner.run();

    return 0;
}

CppUnit::Test* OptionsTest::optionsTestSuite() {
    CppUnit::TestSuite* optsSuite = new CppUnit::TestSuite( "optsSuite" );
    
    optsSuite->addTest( new CppUnit::TestCaller<OptionsTest>( "testSetMode", &OptionsTest::testSetMode ));
    optsSuite->addTest( new CppUnit::TestCaller<OptionsTest>( "testVerbosity", &OptionsTest::testVerbosity ));
    optsSuite->addTest( new CppUnit::TestCaller<OptionsTest>( "testAddToOptMask", &OptionsTest::testAddToOptMask ));
    optsSuite->addTest( new CppUnit::TestCaller<OptionsTest>( "testApplyConfig", &OptionsTest::testApplyConfig ));

    return optsSuite;
}

void OptionsTest::setUp() {
    // Valid modes; modeIndexes are defined by the pre-processor directives in Options.h
    modesVector = {
        { INSTALL, "install" },
        { UNINSTALL, "uninstall" },
        { FOLLOW,   "follow" },
        { UNFOLLOW, "unfollow" },
        { LIST_ALL, "list-all" },
        { LIST_INSTALLED, "list-installed" },
        //{ SEARCH, "search" },
        //{ OWNER, "owner" },
        //{ IMPORT, "import" },
        //{ PURGE, "purge" },
        { NOP, NOP_KEY }
    };

    opts = new Options(NOP);
}

void OptionsTest::tearDown() {
    delete opts;
}

// setMode has valid modes listed in Options.cpp, under the variable modesVector
// It also has a few overloads. One for an unsigned int with the enumerated value
// One for a string, which calls a private function to translate the string to an int, and then calls the int version. Testing the string version will give coverage to both the str-to-int translator as well as the string setter.
// The third overload passes an actual mode struct. In this case, both the modeIndex and modeStr must be identical. This function is never used, however, and only exists as a copy-setter
void OptionsTest::testSetMode() {
    preTestSetMode();

    // Test the int version
    for(int index = 0; index < modesVector.size(); index++) {
        opts->setMode(modesVector[index].modeIndex);
        CPPUNIT_ASSERT(opts->getMode() == modesVector[index]);
    }

    // The string version
    for(int index = 0; index < modesVector.size(); index++) {
        opts->setMode(modesVector[index].modeStr);
        CPPUNIT_ASSERT(opts->getMode() == modesVector[index]);
    }

    // And the mode_s version
    for(int index = 0; index < modesVector.size(); index++) {
        opts->setMode(modesVector[index]);
        CPPUNIT_ASSERT(opts->getMode() == modesVector[index]);
    }

    postTestSetMode();

    return;
}

// setVerbosity also has some logic I'd like to be certain about. Firstly, there are two overloads. The first takes a const char*, verifies it has a size of one, and passes it to the int version if so. The int version verifies the int passed to it is between 0 and 4, and if so, sets it. I'd like to test both of these. Because the former calls the latter, testing that will give us coverage of both.
// I'm using 10 as the cap so that we can also test the one-character tester
void OptionsTest::testVerbosity() {
    preTestVerbosity();

    for(int index = 0; index < 10; index++) {
        bool res = opts->setVerbosity(std::to_string(index).c_str(), true);

        if(index < 5) {
            CPPUNIT_ASSERT((res) && (opts->getVerbosity() == index));
        }

        else {
            CPPUNIT_ASSERT((!res) && (opts->getVerbosity() != index));
        }
    }

    postTestVerbosity();

    return;
}

// addToOptMask has a set of values which it checks before actually changing optMask. These values are all powers of 2, since it's a bitmask
// I want to verify it is allowing these (and only these) variables to be added to the mask. To do this, I'll generate a few values which are not powers of 2 and test those
void OptionsTest::testAddToOptMask() {
    preTestVerbosity();

    // First, test all of our valid variables
    for(int index = 0; index < MASK_SIZE; index++) {
        unsigned int oldMask = opts->getOptMask();
        unsigned int valToAdd = (unsigned int)std::pow(2,index);

        bool res = opts->addToOptMask(valToAdd);

        CPPUNIT_ASSERT((res) && ((opts->getOptMask() - oldMask) == valToAdd));
    }

    // Next, test a few other variables
    for(int index = 0; index < 5; index++) {
        unsigned int oldMask = opts->getOptMask();
        unsigned int valToAdd;

        do {
            valToAdd = rand() % 2048;
        } while((valToAdd != 0) && ((valToAdd & (valToAdd - 1)) == 0)); // This tests to verify exactly a single bit is on

        bool res = opts->addToOptMask(valToAdd, true);
        
        CPPUNIT_ASSERT((!res) && ((opts->getOptMask() == oldMask)));
    }

    postTestVerbosity();

    return;
}

void OptionsTest::testApplyConfig() {
    preTestApplyConfig();

    // For this, we can mock the Config object we would normally expect with our own map   
    MockConfigMap mock = MockConfigMap();
    std::map<std::string, std::string> mockMap = *mock.getConfigMap();

    opts->applyConfig(mock, true);


    // Validate the settings are as we expect
    // Anything commented out has not yet been implemented
    CPPUNIT_ASSERT(std::to_string(opts->getVerbosity()) == mockMap[KEY_VERBOSE]);
    //CPPUNIT_ASSERT(std::string(opts->getSmartOperation()) == mockMap[KEY_SMART_OP]);
    CPPUNIT_ASSERT(opts->getGlobalConfigPath() == mockMap[KEY_GLOBAL_CONFIG_PATH]);
    CPPUNIT_ASSERT(opts->getUserConfigPath() == mockMap[KEY_USER_CONFIG_PATH]);
    CPPUNIT_ASSERT(opts->getSystemRoot() == mockMap[KEY_SYSTEM_ROOT]);
    CPPUNIT_ASSERT(opts->getTarLibraryPath() == mockMap[KEY_TAR_LIBRARY_PATH]);
    CPPUNIT_ASSERT(opts->getInstalledPkgsPath() == mockMap[KEY_INSTALLED_PKG_PATH]);
    //CPPUNIT_ASSERT(opts->getExcludedFiles() == mockMap[KEY_EXCLUDED_FILES]);

    postTestApplyConfig();
}

int OptionsTest::preTestSetMode() {
    return 0;
}

int OptionsTest::preTestVerbosity() {
    return 0;
}

int OptionsTest::preTestAddToOptMask() {
    return 0;
}

int OptionsTest::preTestApplyConfig() {
    return 0;
}

int OptionsTest::postTestSetMode() {
    return 0;
}

int OptionsTest::postTestVerbosity() {
    return 0;
}

int OptionsTest::postTestAddToOptMask() {
    return 0;
}

int OptionsTest::postTestApplyConfig() {
    return 0;
}

// Anything commented out in the following function is not meant to be changed at this point in time
// @TODO Verify that /tmp/ directory exists
std::map<std::string, std::string>* MockConfigMap::getConfigMap() {
    return &mockMap;
}
