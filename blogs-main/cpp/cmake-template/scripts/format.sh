#!/bin/bash

# 脚本功能：使用 clang-format 格式化项目自有源代码（src, include, tests 目录）。

set -e

SCRIPT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)
PROJECT_ROOT="$SCRIPT_DIR/.."

SOURCE_DIRS=(
    "$PROJECT_ROOT/src"
    "$PROJECT_ROOT/include"
    "$PROJECT_ROOT/tests"
)

FILE_PATTERNS=(-name "*.cc" -o -name "*.cpp" -o -name "*.cxx" -o -name "*.h" -o -name "*.hpp" -o -name "*.hxx")

if [ ${#SOURCE_DIRS[@]} -eq 0 ]; then
    echo "错误：没有指定任何源代码目录进行格式化"
    exit 1
fi

echo "正在格式化指定的源代码目录: src/, include/, tests/"

find "${SOURCE_DIRS[@]}" -type f \( "${FILE_PATTERNS[@]}" \) -print0 | xargs -0 clang-format -i

echo "格式化完成"