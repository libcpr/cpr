#!/bin/bash

# Based on: https://gist.github.com/leilee/1d0915a583f8f29414cc21cd86e7151b
# Run from the project root directory as follows:
# ./scripts/run_clang_format.sh tests src

printf "📝  Running clang-format...\n"

function format() {
    for f in $(find $@ -name '*.h' -or -name '*.hpp' -or -name '*.c' -or -name '*.cpp'); do 
        echo "format ${f}";
        clang-format -i --style=file ${f};
    done

    echo "~~~ $@ directory formatted ~~~";
}

# Check all of the arguments first to make sure they're all directories
for dir in "$@"; do
    if [ ! -d "${dir}" ]; then
        echo "${dir} is not a directory";
    else
        format ${dir};
    fi
done

GREEN='\033[0;32m'
NC='\033[0m'

printf "✅ ${GREEN}Done. All files were formatted (if required).${NC}\n"
