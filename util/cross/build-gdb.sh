echo "========================================="
echo "Building gdb..."
echo "========================================="
PREFIX=`pwd`/../..
mkdir build-gdb
cd build-gdb
PATH=$PATH:$PREFIX/bin
../gdb/configure --target $TARGET --prefix=$PREFIX > gdb.log 2>&1
STATUS=$?
if [ $STATUS -ne 0 ]
	then
	echo "Failed to configure gdb. Tail of output:"
	tail -n 60 gdb.log
	cd ..
	exit $STATUS
fi

make install >> gdb.log 2>&1
STATUS=$?
if [ $STATUS -ne 0 ]
	then
	echo "Failed to build gdb. Tail of output:"
	tail -n 60 gdb.log
fi

cd ..
exit $STATUS