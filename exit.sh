#! bin/bash

cd pintos/src/userprog
make clean
make

cd build

pintos-mkdisk filesys.dsk --filesys-size=2
pintos --qemu --disk=filesys.dsk -- -f -q

echo "\033[47m\033[30mBEGIN_exit\033[0m"
make tests/userprog/exit.result
echo "\033[47m\033[30mEND_exit\033[0m"

cd ../../../..
