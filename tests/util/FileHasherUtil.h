/**
 * @author Thomas Lenz <thomas.lenz96@gmail.com> AS The2b
 * @date 29 November 2018
 * @project Package Manager
 * @file fileHasherUtil.h
 */

#include <openssl/sha.h>
#include <filesystem>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int hashFile(unsigned char* buf, std::string path);
