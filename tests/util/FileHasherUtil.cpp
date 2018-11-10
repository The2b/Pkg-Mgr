/**
 * @author Thomas Lenz <thomas.lenz96@gmail.com> AS The2b
 * @date 29 November 2018
 * @project Package Manager
 * @file fileHasher.cpp
 *
 * This tiny program just hashes every file in a path given by the first argument to the program. This is used so we can create a regular, expected format which to test against to verify files are being extracted correctly. Because this is merely an assistive tool for the developer, the usual checks and boundaries in the actual program are not here. There is virtually no sanitation or verification in this file. May God have mercy on your souls.
 *
 * I'm using mostly C style here since OpenSSL, the core of this, is a C library. However, since we only want to hash regular files, we need a test for this, and std::filesystem is the best way I know to check if a directory entry is a file. Because of this, it still needs to be C++
 */

#include "FileHasherUtil.h"

int hashFile(unsigned char* buf, std::string path) {
    if(std::filesystem::exists(path)) {
        std::ifstream ifs(path);
        SHA_CTX ctx;
        SHA1_Init(&ctx);

        while(ifs.good()) {
            char buf[512];
            ifs.get(buf,512,'\0');
            SHA1_Update(&ctx,buf,strlen(buf));
        }

        if(ifs.bad()) {
            fprintf(stderr,"An error occurred while reading the file %s...\n",path);
            return -1;
        }

        SHA1_Final(buf,&ctx);

        return 0;
    }

    else {
        fprintf(stderr,"The file %s does not exist or cannot be read...\n",path);
        return -2;
    }
}
