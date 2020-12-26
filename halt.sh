#! bin/bash

cd pintos/src/userprog
make clean
make

cd build

pintos-mkdisk filesys.dsk --filesys-size=2
pintos --qemu --disk=filesys.dsk -- -f -q

echo "\033[47m\033[30mBEGIN_halt\033[0m"
make tests/userprog/halt.result
echo "\033[47m\033[30mEND_halt\033[0m"

cd ../../../..
