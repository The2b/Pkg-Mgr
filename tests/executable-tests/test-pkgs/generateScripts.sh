#!/bin/bash

TEMP_DIR_ROOT="/tmp/test"
INDEX=$1

for SCRIPT_NAME in pre-install post-install pre-uninstall post-uninstall ; do
    echo -e "#!/bin/bash\nmkdir $TEMP_DIR_ROOT$INDEX\ntouch $TEMP_DIR_ROOT$INDEX/$SCRIPT_NAME" > "$SCRIPT_NAME.sh"
    chmod u+x "$SCRIPT_NAME.sh"
done
