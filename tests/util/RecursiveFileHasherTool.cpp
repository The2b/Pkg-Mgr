/**
 * @author Thomas Lenz <thomas.lenz96@gmail.com> AS The2b
 * @date 29 November 2018
 * @project Package Manager
 * @file RecursiveFileHasherTool.cpp
 *
 * This tiny program just hashes every file in a path given by the first argument to the program. This is used so we can create a regular, expected format which to test against to verify files are being extracted correctly. The second argument, if it exists, will be the output folder name. For saftey, this will never overwrite a file, unless there is no second argument. Because this is merely an assistive tool for the developer, the usual checks and boundaries in the actual program are not here. There is virtually no sanitation or verification in this file. May God have mercy on your souls.
 *
 * I'm using mostly C style here since OpenSSL, the core of this, is a C library. However, since we only want to hash regular files, we need a test for this, and std::filesystem is the best way I know to check if a directory entry is a file. Because of this, it still needs to be C++
 */

#include "RecursiveFileHasherTool.h"

int main(int argc, char** argv) {
    if(argc < 2) {
        fprintf(stderr,"Error: No path given. Exiting...\n");
        exit(1);
    }

    FILE* out;

    if(argc < 3) {
        out = fopen("file-hasher-output","wb+");
    } else {
        if(std::filesystem::exists(argv[2])) {
            fprintf(stderr,"Error: Refusing to overwrite %s...\n",argv[3]);
            exit(2);
        }

        out = fopen(argv[2],"w");
    }

    for(auto& path: std::filesystem::recursive_directory_iterator(argv[1])) {
        if(std::filesystem::is_regular_file(path)) {
            unsigned char hash[SHA_DIGEST_LENGTH];
            hashFile(hash,path.path());

            fprintf(out,"%s,",path.path().c_str());

            for(int index = 0; index < SHA_DIGEST_LENGTH; index++) {
                fprintf(out,"%02x",hash[index]);
            }

            fprintf(out,"\n");
        }
    }

    fclose(out);

    return 0;
}
