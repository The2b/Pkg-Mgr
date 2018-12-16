/**
 * @author Thomas Lenz <thomas.lenz96@gmail.com AS The2b>
 * @date 26 November 2018
 * @project Package Manager
 * @file tstOptions.h
 *
 * Read the note in tstOptions.cpp as to why there are so many setter tests
 */

#include <stdlib.h>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <filesystem>

#include <cppunit/TestAssert.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestCaller.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TestResultCollector.h>

#include "Options.h"

class OptionsTest : public CppUnit::TestFixture {
    private:
        Options* opts;
        std::vector<mode_s> modesVector;
    public:
        // Test suite
        static CppUnit::Test* optionsTestSuite();

        // Pre- and post- suite functions
        void setUp();
        void tearDown();
        
        // Function tests
        void testSetMode();
        void testVerbosity();
        void testAddToOptMask();
        void testApplyConfig();

        // Pre- and post- test scripts
        int preTestSetMode();
        int preTestVerbosity();
        int preTestAddToOptMask();
        int preTestApplyConfig();

        int postTestSetMode();
        int postTestVerbosity();
        int postTestAddToOptMask();
        int postTestApplyConfig();
};

// This is a mockup for our testApplyConfig options
class MockConfigMap : public IConfigMap {
    public:
        std::map<std::string, std::string> mockMap = {
            { KEY_VERBOSE, "4" },
            //{ KEY_SMART_OP, "true" },
            { KEY_GLOBAL_CONFIG_PATH, "/tmp/pkg-mgr.conf" },
            { KEY_USER_CONFIG_PATH, ".pkg-mgr-test.conf" },
            { KEY_SYSTEM_ROOT, "/tmp/" },
            { KEY_TAR_LIBRARY_PATH, "/tmp/" },
            { KEY_INSTALLED_PKG_PATH, "/tmp/" },
            //{ KEY_EXCLUDED_FILES, "/tmp/bin/test1,/tmp/bin/test2" }
        };

        std::map<std::string, std::string>* getConfigMap();
};
