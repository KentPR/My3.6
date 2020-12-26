#! bin/bash

cd pintos/src/userprog
make clean
make

cd build

echo "\033[47m\033[30mBEGIN_exec\033[0m"
make tests/userprog/exec-once.result
echo "\033[31mEND\033[0m"
make tests/userprog/exec-multiple.result
echo "\033[31mEND\033[0m"
make tests/userprog/exec-arg.result
echo "\033[31mEND\033[0m"
make tests/userprog/exec-missing.result
echo "\033[31mEND\033[0m"
make tests/userprog/exec-bad-ptr.result
echo "\033[31mEND\033[0m"
echo "\033[47m\033[30mEND_exec\033[0m"

cd ../../../..
