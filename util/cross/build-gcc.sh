PREFIX=`pwd`/../..
mkdir build-gcc
cd build-gcc
PATH=$PATH:$PREFIX/bin
../gcc/configure --target $TARGET --prefix=$PREFIX --disable-nls --without-headers --enable-languages=c,c++
make all-gcc all-target-libgcc install-target-libgcc install-gcc
cd ..

