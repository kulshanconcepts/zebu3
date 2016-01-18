echo "========================================="
echo "Building gcc..."
echo "========================================="
PREFIX=`pwd`/../..
mkdir build-gcc
cd build-gcc
PATH=$PATH:$PREFIX/bin
../gcc/configure --target $TARGET --prefix=$PREFIX --disable-nls --without-headers --enable-languages=c,c++ > gcc.log 2>&1
STATUS=$?
if [ $STATUS -ne 0 ]
	then
	echo "Failed to configure gcc. Tail of output:"
	tail -n 60 gcc.log
	cd ..
	exit $STATUS
fi

make all-gcc all-target-libgcc install-target-libgcc install-gcc >> gcc.log 2>&1
STATUS=$?
if [ $STATUS -ne 0 ]
	then
	echo "Failed to build gcc. Tail of output:"
	tail -n 60 gcc.log
fi

cd ..
exit $STATUS
