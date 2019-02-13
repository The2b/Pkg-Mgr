# @author Thomas Lenz <thomas.lenz96@gmail.com> AS The2b
# @date 12 February 2019
# @project Package Manager
# @file doxygen-version-test.sh

# Get the version from the third line of ${topsrcdir}/NEWS.md, which will be the newest version
VERSION=$(head -3 ../../NEWS.md | grep -P "## Version [0-9]\.[0-9]\.[0-9]:" | grep -oP "[0-9]\.[0-9]\.[0-9]")

for ver in {html,man,latex} ; do
    echo "Testing $ver doxygen config file for the version number $VERSION..."

    # Make sure that all of our doxygen files match up
    DOX_VERSION_MATCH=$(grep -P "PROJECT_NUMBER\s*=\s*\"v$VERSION\"" ../../docs/doxy-config-*)

    if [[ $DOX_VERSION_MATCH ]] ; then
        echo "$ver doxygen config file passed!"
    else
        echo "$ver doxygen config file failed!"
    fi
    
    echo
done
