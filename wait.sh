#! bin/bash

cd pintos/src/userprog
make clean
make

cd build

echo "\033[47m\033[30mBEGIN_wait\033[0m"
make tests/userprog/wait-simple.result
echo "\033[32mEND\033[0m"
make tests/userprog/wait-twice.result
echo "\033[32mEND\033[0m"
make tests/userprog/wait-killed.result
echo "\033[32mEND\033[0m"
make tests/userprog/wait-bad-pid.result
echo "\033[32mEND\033[0m"
echo "\033[47m\033[30mEND_wait\033[0m"

cd ../../../..
