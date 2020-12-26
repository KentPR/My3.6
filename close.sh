#! bin/bash

cd pintos/src/userprog
make clean
make

cd build

echo "\033[47m\033[30mBEGIN_close\033[0m"
make tests/userprog/close-normal.result
echo "\033[35mEND\033[0m"
make tests/userprog/close-twice.result
echo "\033[35mEND\033[0m"
make tests/userprog/close-stdin.result
echo "\033[35mEND\033[0m"
make tests/userprog/close-stdout.result
echo "\033[35mEND\033[0m"
make tests/userprog/close-bad-fd.result
echo "\033[35mEND\033[0m"
echo "\033[47m\033[30mEND_close\033[0m"
 
cd ../../../..
