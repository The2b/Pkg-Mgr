# News

## Version 1.0.2
    Did a whole bunch with documentation
        
        * Added a whole bunch of new documentation

        * Updated a bit of the old documentation

        * Modified what we had to be properly picked up by Doxygen

        * Created targets to build documenation in LaTex, HTML, man-pages, or all three via Doxygen

        * Created 3 Doxygen config files in the docs directory (one for each format)

        * Removed the pre-built docs, since users can now easily build their own

    Added a version test to the test harness

    Removed some TODO's I have decided against implementing

    Changed Options.cpp functions to use integer verbosity like everything else. The old boolean verbosity functions are now deprecated.

    Changed configure to get default values for some of the variables otherwise set by environment variables to use configure's native path options (prefix, sysconfdir, etc).

    Added the defaults for the above values to their description

    Created a new defaultDefinitions automake file, so I can use them in multiple places while still keeping them in one place

    Updated text files to use Markdown format now (extention: .md)

## Version 1.0.1:
    Options.cpp updated to use verbosity consistant with the rest of the code base
    
    Doxygen documentation has been completed, and can now be built for web servers. Testing needs to be done on this, however.
    
    Due to name changes to certain files, configure.ac has been refactored

## Version 1.0.0:
    Initial Release!
