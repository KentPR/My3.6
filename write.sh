#! bin/bash

cd pintos/src/userprog
make clean
make

cd build

echo "\033[47m\033[30mBEGIN_write\033[0m"
make tests/userprog/write-normal.result
echo "\033[37mEND\033[0m"
make tests/userprog/write-zero.result
echo "\033[37mEND\033[0m"
make tests/userprog/write-bad-ptr.result
echo "\033[37mEND\033[0m"
make tests/userprog/write-boundary.result
echo "\033[37mEND\033[0m"
make tests/userprog/write-stdin.result
echo "\033[37mEND\033[0m"
make tests/userprog/write-bad-fd.result
echo "\033[37mEND\033[0m"
echo "\033[47m\033[30mEND_write\033[0m"

cd ../../../..
