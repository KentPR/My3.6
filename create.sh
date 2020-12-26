#! bin/bash

cd pintos/src/userprog
make clean
make

cd build

echo "\033[47m\033[30mBEGIN_close\033[0m"
make tests/userprog/create-empty.result
echo "\033[33mEND\033[0m"
make tests/userprog/create-long.result
echo "\033[33mEND\033[0m"
make tests/userprog/create-normal.result
echo "\033[33mEND\033[0m"
make tests/userprog/create-exists.result
echo "\033[33mEND\033[0m"
make tests/userprog/create-null.result
echo "\033[33mEND\033[0m"
make tests/userprog/create-bad-ptr.result
echo "\033[33mEND\033[0m"
make tests/userprog/create-bound.result
echo "\033[33mEND\033[0m"
echo "\033[47m\033[30mEND_create\033[0m"

cd ../../../..
