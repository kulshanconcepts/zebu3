PREFIX=`pwd`/../..
mkdir build-gdb
cd build-gdb
PATH=$PATH:$PREFIX/bin
../gdb/configure --target $TARGET --prefix=$PREFIX && make && make install
cd ..

