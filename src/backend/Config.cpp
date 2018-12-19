/**
 * @author Thomas Lenz <thomas.lenz96@gmail.com> AS The2b
 * @date 11 November 2018
 * @project Package Manager
 * @file Config.cpp
 * @error -200
 */

#include "Config.h"

/**
 * A constructor for a Config object, which represents a file holding various configuration options for the package manager
 *
 * @param [in] std::string path, The path to the configuration file
 * @param [in] unsigned int verbosity, The verbosity level to use while reading the configuration file; between 0 and 4
 * @param [in] char delim = '=', The delimiter to use to seperate keys and values
 *
 * @return Config, a config object
 */
Config::Config(std::string path, unsigned int verbosity, char delim) {/*{{{*/
    pathname = path;
    confFileStream = std::ifstream(pathname.c_str());
    //readVals = readConfig(confFileStream, verbosity);
    configVals = parseConfig(getConfigStrings(), verbosity, delim);
}/*}}}*/

/**
 * A destructor used to guarentee the file streams are closed when we destroy the object
 *
 * @return NULL
 */
Config::~Config() {/*{{{*/
    closeStream(confFileStream);
}/*}}}*/

/**
 * Returns the pathname of the Config object
 *
 * @return std::string pathname
 */
std::string Config::getPathname() {/*{{{*/
    return pathname;
}/*}}}*/

/**
 * Returns a vector of strings, each one representing a single line from the configuration file this Config object represents
 * Because this is rarely (never, at the time of writing this) going to be used, this function merely reads the file each time it is called
 * Because of this, it is not guarenteed to be equal to the configVals map, which is the parsed version of the configuration file
 * This design choice was made because the little-to-zero usage doesn't justify the potential memory cost of holding the entire file in memory until the object is destroyed
 * 
 * @return std::vector<std::string> configStrings
 */
std::vector<std::string> Config::getConfigStrings() {/*{{{*/
    std::ifstream ifs(pathname.c_str());
    return readConfig(ifs,0);
}/*}}}*/

/**
 * Returns a map which has both keys and values of std::string
 * The map represents the parsed configuration file
 *
 * @return std::map<std::string, std::string> parsedConfigVals
 */
std::map<std::string, std::string>* Config::getConfigMap() {/*{{{*/
    return &configVals;
}/*}}}*/

// Error checking should be based on the size of the vector (ie not 0)
// @TODO Make the error more specific
/**
 * Reads through the configuration file and creates a vector of strings where each index represents a line from the configuration file
 *
 * @param [in] std::ifstream& [in] configFileStream, an open, good filestream
 * @param [in] unsigned int verbosity, the verbosity to use while reading the configuration file
 *
 * @return std::vector<std::string> configLines, a vector with each index being its own line
 */
std::vector<std::string> Config::readConfig(std::ifstream& fileStream, unsigned int verbosity) {/*{{{*/
    std::vector<std::string> readVals;

    // Check the status of the filestream
    int index = 0;
    if(fileStream.good()) {
        do {
            std::string line;
            std::getline(fileStream,line);

            readVals.push_back(line);
            index++;
        } while(fileStream.good());

        // Check why we got out
        // EOF is ideal, since that means we read the whole file
        // Note that in this case (success), the user is responsible for closing fileStream
        if(fileStream.eof()) {
            fileStream.close();
            return readVals;
        }

        // If we fail. say so
        else if(fileStream.fail()) {
            if(verbosity != 0) {
                fprintf(stderr,"Error while reading config file %s...\n",getPathname());
            }

            fileStream.close();
            closeStream();
            exit(-201);
        }

        // We should never get here unless a RAM bit is flipped somehow...
        else {
            if(verbosity != 0) {
                fprintf(stderr,"We left our config reader loop, but no fail bits, bad bits, or eof bits were set. This should never occur, and you should e-mail the creator about this issue. Exiting...\n");
            }

            fileStream.close();
            closeStream();
            exit(-200);
        }
    }

    else {
        if(verbosity != 0) {
            printf("Error: Could not read the config file %s...\n",getPathname());
        }

        return readVals;
    }
}/*}}}*/

// @TODO Make this such that comment characters can appear anywhere, not just as the first character
/**
 * A function used to parse a string vector, usually created by readConfig
 * You can escape a delimiter with a \
 * Note that if this occurs, the actual string passed will escape the backslash, resulting in "\\"
 * If the first character is equal to COMMENT_CHAR (defined by a pre-processor directive in Config.h, # by default), the line is ignored entirely
 * If there is no delimiter, the line is ignored
 * If the only un-escaped valid delimiter is the first character (0th index), the line is ignored
 * Any delimiters after the first un-escaped delimiter are considered to be part of the value, regardless of whether or not they are escaped
 *
 * @param [in] std::vector<std::string> rawStrings, a vector of strings to parse and create a map out of
 * @param [in] unsigned int verbosity, the verbosity to use while parsing the strings
 * @param [in] char delim, the character to delimit on
 *
 * @return std::map<std::string, std::string> parsedConfig
 */
std::map<std::string, std::string> Config::parseConfig(std::vector<std::string> rawStrings, unsigned int verbosity, char delim) {/*{{{*/
    std::map<std::string, std::string> confMap;

    // For each line, find the first instance of the non-escaped delimiter, split the string based on that, and add it to the map
    // Note that if the delimiter does not exist, a warning will be printed out, but the program will continue on as normal
    for(int index = 0; index < rawStrings.size(); index++) {
        std::string str = rawStrings[index];

        if(str != "" && str[0] != COMMENT_CHAR) {
            int usedDelim = findDelim(str,DELIM_CHAR);
            
            // If usedDelim is 0, it never got changed, which means we never found a valid one
            // If it's -1, we have an empty string, and we can ignore it
            if(usedDelim > 0) {
                // Split the string
                std::string key = str.substr(0,usedDelim);
                std::string val = str.substr(usedDelim + 1,str.npos);

                // If a value is found, add it to the map
                if(val != "") {
                    confMap.insert(std::pair<std::string, std::string>(key,val));
                }
                else {
                    if(verbosity != 0) {
                        fprintf(stderr,"Warning: Configuration option %s has no value. Ignoring...\n",key.c_str());
                    }
                }
            }

            // Try to continue on as normal
            else {
                if(verbosity != 0) {
                    fprintf(stderr,"Warning: The string \"%s\", line %d in the configuration file, is invalid. Attempting to continue on as normal...\n",str.c_str(),index);
                }
            }
        }
    }

    // Done reading
    return confMap;
}/*}}}*/

/**
 * Find the location of the first unescaped delimiter in a given string
 * The escape character is currently hard-coded to \
 * @TODO Fix the hard-coded nature if the escape character
 *
 * @param [in] std::string str, the string to search through
 * @param [in] const char delim, the character to delimit on
 *
 * @return int delimLoc, the location of the first un-escaped delimiter
 */
int findDelim(std::string str,const char delim) {/*{{{*/
    // The following directives do the following: Save the current state of our warning settings (such as -Werror) which come from the command line, ignore overflow (since this is intended), and then restore the old settings
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverflow"
    // Set a value for the last delim we found, so we can start looking after it, if it was escaped
    int lastDelim = str.npos; // str.npos = -1; I'm writing it this way for clarity. Ignore warnings caused by this overflow.
#pragma GCC diagnostic pop
    int usedDelim = -1;

    do {
        // Find a delimiter
        lastDelim = str.find(delim,lastDelim + 1);

        // Check if it is escaped
        if((lastDelim != str.npos) && (*(str.c_str() + (lastDelim - 1)) != '\\')) {
            // If not, set our usable value
            usedDelim = lastDelim;
        }

        // This will go into an infinite look
    } while((lastDelim != str.npos) && (usedDelim == -1)); // Checking for delimiters of 0 is handled by the calling function

    return usedDelim;
}/*}}}*/

/**
 * Checks to see if a given filestream is open, and if it is, close it
 *
 * @param [in] std::ifstream& fileStream, the filestream to close
 *
 * @return bool isClosed, the opposite of fileStream.is_open()
 */
bool Config::closeStream(std::ifstream& fileStream) {/*{{{*/
    if(fileStream.is_open()) {
        fileStream.close();
    }

    return !fileStream.is_open();
}/*}}}*/

/**
 * A shortcut to execute closeStream on the filestream associated with the Config object used to call the function
 *
 * @return bool isClosed, the opposite of Config.confFileStream.is_open()
 */
bool Config::closeStream() {/*{{{*/
    return closeStream(confFileStream);
}/*}}}*/

/**
 * Checks to see if a given filestream is closed, and if it is, open in
 *
 * @param [in] std::ifstream& fileStream, the filestream to open
 *
 * @return bool isOpen, fileStream.is_open() afterwards
 */
bool Config::openStream(std::ifstream& fileStream) {/*{{{*/
    if(!fileStream.is_open()) {
        fileStream.close();
    }

    return fileStream.is_open() && fileStream.good();
}/*}}}*/

/**
 * A shortcut to execute openStream on the filestream associated with the Config object used to call the function
 *
 * @return bool isOpen, Config.confFileStream.is_open()
 */
bool Config::openStream() {/*{{{*/
    return openStream(confFileStream);
}/*}}}*/

// @TODO Add consistency between using getConfigMap() and Config.configVals
/**
 * Merge a new config map into and on top of the base config
 * Adds all the keys (and their values) unique to newConfig to baseConfig
 * Replaces all the values w/ keys they share with the newConfig values
 *
 * @param [in,out] IConfigMap& baseConfig, the IConfigMap-implementing object to apply the second Config object's data onto
 * @param [in] IConfigMap& newConfig, the IConfigMap-implementing object whose values you are going to overlay onto baseConfig
 * @param [in] unsigned int verbosity, the verbosity to use when merging Config maps. Only matters if at 4 or higher
 */
void mergeConfig(IConfigMap& baseConfig, IConfigMap& newConfig, unsigned int verbosity) {/*{{{*/
    if(!newConfig.getConfigMap()->empty()) {
        std::map<std::string, std::string> newConfMap = *newConfig.getConfigMap();
        std::map<std::string, std::string>* baseConfMap = baseConfig.getConfigMap();

        for(std::map<std::string, std::string>::iterator it = newConfMap.begin(); it != newConfMap.end(); it++) {
            if(verbosity >= 4) {
                printf("newConfMap key: %s\nnewConfMap value: %s\n\n",it->first.c_str(), newConfMap.at(it->first).c_str());
            }
            
            // Abuse the fact that the [] operatorn will add in keys if they are not in the map
            baseConfMap->operator[](it->first) = newConfMap.at(it->first);
            
            if(verbosity >= 4) {
                printf("baseConfig.configVals[%s]: %s\n\n",it->first.c_str(), baseConfMap->at(it->first).c_str());
            }
        }
    }

    return;
}/*}}}*/
