echo "========================================="
echo "Building binutils..."
echo "========================================="
PREFIX=`pwd`/../..
mkdir build-binutils
cd build-binutils
../binutils/configure --target=$TARGET --prefix=$PREFIX --disable-nls --with-sysroot --disable-werror > binutils.log 2>&1
STATUS=$?
if [ $STATUS -ne 0 ]
	then
	echo "Failed to configure binutils. Tail of output:"
	tail -n 60 binutils.log
	cd ..
	exit $STATUS
fi

make all install >> binutils.log 2>&1
STATUS=$?
if [ $STATUS -ne 0 ]
	then
	echo "Failed to build binutils. Tail of output:"
	tail -n 60 binutils.log
fi

cd ..
exit $STATUS
