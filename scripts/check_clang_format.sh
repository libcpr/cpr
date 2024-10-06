#!/bin/bash

# Based on: https://gist.github.com/leilee/1d0915a583f8f29414cc21cd86e7151b
# Checks if all files are formatted based on the clang-format formatting rules.
# Execute as follows:
# ./scripts/check_clang_format.sh tests src

printf "📑  Checking if your code fulfills all clang-format rules...\n"

RET_CODE=0

function format() {
    for f in $(find $@ -name '*.h' -or -name '*.hpp' -or -name '*.c' -or -name '*.cpp'); do 
        clang-format -i --dry-run --Werror --style=file ${f};
        ret=$?
        if [ $ret -ne 0 ]; then
            RET_CODE=$ret
        fi
    done

    echo "~~~ $@ directory checked ~~~";
}

# Check all of the arguments first to make sure they're all directories
for dir in "$@"; do
    if [ ! -d "${dir}" ]; then
        echo "${dir} is not a directory";
    else
        format ${dir};
    fi
done

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

if [ $RET_CODE -eq 0 ]; then
    printf "✅ ${GREEN}Everything up to standard :party: ${NC}\n"
else
    printf "❌ ${RED}Not up to formatting standard :sad_face: ${NC}\n"
    echo "Try running run_clang_format.sh to format all files."
fi

exit $RET_CODE