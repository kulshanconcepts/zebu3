PREFIX=`pwd`/../../
mkdir build-binutils
cd build-binutils
../binutils/configure --target=$TARGET --prefix=$PREFIX --disable-nls --with-sysroot --disable-werror
make all install
cd ..

