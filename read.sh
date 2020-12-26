#! bin/bash

cd pintos/src/userprog
make clean
make

cd build

echo "\033[47m\033[30mBEGIN_read\033[0m"
make tests/userprog/read-normal.result
echo "\033[36mEND\033[0m"
make tests/userprog/read-zero.result
echo "\033[36mEND\033[0m"
make tests/userprog/read-bad-ptr.result
echo "\033[36mEND\033[0m"
make tests/userprog/read-boundary.result
echo "\033[36mEND\033[0m"
make tests/userprog/read-stdout.result
echo "\033[36mEND\033[0m"
make tests/userprog/read-bad-fd.result
echo "\033[36mEND\033[0m"
echo "\033[47m\033[30mEND_read\033[0m"

cd ../../../..
