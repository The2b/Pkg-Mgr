/**
 * @author Thomas Lenz <thomas.lenz96@gmail.com> AS The2b
 * @date 21 November 2018
 * @project Package Manager
 * @file tstConfig.cpp
 */

#include "tstConfig.h"

std::string execDir;

int main(int argc, char** argv) {
    execDir = std::string(dirname(argv[0]));
    CppUnit::TextUi::TestRunner configRunner;
    configRunner.addTest( ConfigTest::configSuite() );

    configRunner.run();

    return 0;
}

CppUnit::Test* ConfigTest::configSuite() {
    CppUnit::TestSuite* cfgSuite = new CppUnit::TestSuite( "ConfigTest" );

    cfgSuite->addTest( new CppUnit::TestCaller<ConfigTest>( "testPathname", &ConfigTest::testPathname ));
    cfgSuite->addTest( new CppUnit::TestCaller<ConfigTest>( "testConfigStrings", &ConfigTest::testConfigStrings ));
    cfgSuite->addTest( new CppUnit::TestCaller<ConfigTest>( "testConfigMap", &ConfigTest::testConfigMap ));
    cfgSuite->addTest( new CppUnit::TestCaller<ConfigTest>( "testFindDelim", &ConfigTest::testFindDelim ));

    return cfgSuite;
}

void ConfigTest::setUp() {
    cfgName = std::string(TEST_CONFIG_NAME);
    cfgPath = execDir + "/" + cfgName;
    cfg = new Config(cfgPath,VERBOSITY);
    return;
}

void ConfigTest::tearDown() {
    delete cfg;
    return;
}

void ConfigTest::testPathname() {
    CPPUNIT_ASSERT(cfgPath == cfg->getPathname());
    return;
}

/**
 * Tests that the configuration file is correctly being read
 * The configuration file we use for this will have all our special cases:
 *  *All* instances of our interpreted variables, none of which are at their default default value
 *  A blank variable name (first character is =) with a = after it
 *  A blank variable name with a = after it
 *  Just a =
 *  A variable name with an escaped delimiter
 *  A variable value with a delimiter
 */
void ConfigTest::testConfigStrings() {
    std::ifstream ifs(execDir + TEST_CONFIG_NAME);
    std::vector<std::string> cfgStrs = cfg->getConfigStrings();
        
    for(int index = 0; index < cfgStrs.size(); index++) {
        std::string str = "";
        std::getline(ifs,str);
        
        CPPUNIT_ASSERT(cfgStrs[index] == str);
    }

    ifs.close();

    return;
}

/**
 * Tests that the configuration file is correctly being parsed
 * The configuration file we use for this will have all our special cases:
 *  *All* instances of our interpreted variables, none of which are at their default default value
 *  A blank variable name (first character is =) with a = after it
 *  A blank variable name with a = after it
 *  Just a =
 *  A variable name with an escaped delimiter
 *  A variable value with a delimiter
 *
 * Note that for each variable name which ends with the first non-escaped delimiter will have an index, but a value of "". This is expected behavior.
 *
 * This test is extremely fragile. Take care when editing it for additional features or configuration options.
 */
void ConfigTest::testConfigMap() {
    std::map<std::string, std::string> controlMap;
    controlMap["test3\\=test3"] = "test3";
    controlMap["test4"] = "test4=test4";
    controlMap["verbosity"] = "4";
    controlMap["smartOperation"] = "true";
    controlMap["userConfigPath"] = ".testing/test";
    controlMap["systemRoot"] = "/testing/test";
    controlMap["packageLibraryPath"] = "/var/lib/testing/test/pkgs";
    controlMap["installedPkgs"] = "/var/lib/testing/test/installed";
    controlMap["excludedFiles"] = "test,test2,test3";

    std::map<std::string, std::string> testMap = *cfg->getConfigMap();

    CPPUNIT_ASSERT(strMapComparison(controlMap,testMap,4));
    return;
}

/**
 * Tests that delimiters are properly being found in all cases
 *  *All* instances of our interpreted variables, none of which are at their default default value
 *  A blank variable name (first character is =) with a = after it
 *  A blank variable name with a = after it
 *  Just a =
 *  A variable name with an escaped delimiter
 *  A variable value with a delimiter
 *
 * These are far less fragile than testConfigMap and testConfigStrings
 */
void ConfigTest::testFindDelim() {
    std::map<std::string,int> testStrsMap;
    testStrsMap["verbosity=\"4\""] = 9;
    testStrsMap["smartOperation=\"true\""] = 14;
    testStrsMap["userConfigPath=\".testing/test\""] = 14;
    testStrsMap["systemRoot=\"/testing/test\""] = 10;
    testStrsMap["packageLibraryPath=\"/var/lib/testing/test/pkgs\""] = 18;
    testStrsMap["installedPkgs=\"/var/lib/testing/test/installed\""] = 13;
    testStrsMap["excludedFiles=\"test,test2,test3\""] = 13;
    testStrsMap["="] = 0;
    testStrsMap["test\\=345=\"678\""] = 9;
    testStrsMap["test91011=121314=151617"] = 9;

    for(std::map<std::string,int>::iterator it = testStrsMap.begin(); it != testStrsMap.end(); it++) {
        int delim = -1;
        delim = findDelim(it->first,'=');

        CPPUNIT_ASSERT(delim == it->second);
    }

    return;
}

// @TODO
void ConfigTest::testMergeConfigs() {
    preTestMergeConfigs();
    // We have a config with totally unique options from the actual configuration file. Run mergeConfigs and verify the options are all overwritten. One option, globalConfigPath, does not exist in the configuration file (since it is useless). This is used here to test that adding options not in the configuration file still works correctly.
    // In order to preserve our configuration for other tests, we will overlay cfg onto mcm
    mergeConfig(mcm, *cfg, 0);

    std::map<std::string, std::string> mcmMap = *mcm.getConfigMap();
    
    for(std::map<std::string, std::string>::iterator it = mcmMap.begin(); it != mcmMap.end(); it++) {
        CPPUNIT_ASSERT(cfg->getConfigMap()->at(it->first) == it->second);
    }
    
    postTestMergeConfigs();
    return;
}

int ConfigTest::preTestPathname() {
    return 0;
}

int ConfigTest::preTestConfigStrings() {
    return 0;
}

int ConfigTest::preTestConfigMap() {
    return 0;
}

int ConfigTest::preTestFindDelim() {
    return 0;
}

int ConfigTest::preTestMergeConfigs() {
    // These variables are defined in the header. The keys are in Options.h
    mcm.configVals[KEY_VERBOSE] = mcm.valVerbose;
    mcm.configVals[KEY_GLOBAL_CONFIG_PATH] = mcm.valGCP;
    mcm.configVals[KEY_USER_CONFIG_PATH] = mcm.valUCP;
    mcm.configVals[KEY_SYSTEM_ROOT] = mcm.valSR;
    mcm.configVals[KEY_TAR_LIBRARY_PATH] = mcm.valTarLib;
    mcm.configVals[KEY_INSTALLED_PKG_PATH] = mcm.valIPD;
    return 0;
}

int ConfigTest::postTestPathname() {
    return 0;
}

int ConfigTest::postTestConfigStrings() {
    return 0;
}

int ConfigTest::postTestConfigMap() {
    return 0;
}

int ConfigTest::postTestFindDelim() {
    return 0;
}

int ConfigTest::postTestMergeConfigs() {
    return 0;
}

bool strMapComparison(std::map<std::string, std::string>& m1, std::map<std::string, std::string>& m2, unsigned int verbosity) {
    if(m1.size() != m2.size()) {
        if(verbosity != 0) {
            printf("Maps are not of equal size\n");
        }
            return false;
    }

    for(std::map<std::string, std::string>::iterator it = m1.begin(); it != m1.end(); it++) {
        if(m2.find(it->first) != m2.end()) {
            if(m2.at(it->first) != it->second) {
                if(verbosity != 0) {
                    printf("Map values \"%s\" are not equal. The first is equal to \"%s\" and the second \"%s\"\n",it->first,it->second,m2.at(it->first));
                }
                    return false;
            }
        }
        else {
            printf("Map 2 did not have an index for \"%s\"\n",it->first);
        }
    }

    return true;
}

std::map<std::string, std::string>* MockConfigMap::getConfigMap() {
    return &configVals;
}
