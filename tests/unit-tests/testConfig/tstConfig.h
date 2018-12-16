#include <stdlib.h>
#include <libgen.h>
#include <map>
#include <vector>
#include <fstream>
#include <cppunit/TestAssert.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestFixture.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TestResultCollector.h>

#include "Config.h"

#define TEST_CONFIG_NAME "/tst-config.conf"
#define VERBOSITY 0

class MockConfigMap : public IConfigMap {
    public:
        std::string valVerbose = "3";
        std::string valGCP = "/tmp/pkg-mgr-test/pkg-mgr.conf";
        std::string valUCP = "/tmp/pkg-mgr-test/pkg-mgr.conf";
        std::string valSR = "/tmp/pkg-mgr-test/";
        std::string valTarLib = "/tmp/pkg-mgr-test/pkgs/";
        std::string valIPD = "/tmp/pkg-mgr-tets/installed/";

        std::map<std::string, std::string> configVals = {};
        std::map<std::string, std::string>* getConfigMap();
};

class ConfigTest : public CppUnit::TestFixture {
    private:
        MockConfigMap mcm;
        Config* cfg;
        std::string cfgName;
        std::string cfgPath;
    public:
        void setUp();
        void tearDown();
        static CppUnit::Test* configSuite();
        void testPathname();
        void testConfigStrings();
        void testConfigMap();
        void testFindDelim();
        void testMergeConfigs();
        int preTestPathname();
        int preTestConfigStrings();
        int preTestConfigMap();
        int preTestFindDelim();
        int preTestMergeConfigs();
        int postTestPathname();
        int postTestConfigStrings();
        int postTestConfigMap();
        int postTestFindDelim();
        int postTestMergeConfigs();
};

bool strMapComparison(std::map<std::string, std::string>& m1, std::map<std::string, std::string>& m2, unsigned int verbosity);
