#! bin/bash

cd pintos/src/userprog
make clean
make

cd build

echo "\033[47m\033[30mBEGIN_open\033[0m"
make tests/userprog/open-missing.result
echo "\033[34mEND\033[0m"
make tests/userprog/open-normal.result
echo "\033[34mEND\033[0m"
make tests/userprog/open-twice.result
echo "\033[34mEND\033[0m"
make tests/userprog/open-boundary.result
echo "\033[34mEND\033[0m"
make tests/userprog/open-empty.result
echo "\033[34mEND\033[0m"
make tests/userprog/open-null.result
echo "\033[34mEND\033[0m"
make tests/userprog/open-bad-ptr.result
echo "\033[34mEND\033[0m"
echo "\033[47m\033[30mEND_open\033[0m"

cd ../../../..
