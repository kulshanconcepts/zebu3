if [ ! -e "kernel.bin" ]
then
	echo "You need to run 'make' first."
	exit
fi

DO_DEBUG=
if [ $# -ge 1 ] && [ $1 == "debug" ]
then
	DO_DEBUG="-s -S"
	echo "QEMU will start with debugging. Attach GQB using ./debug.sh"
	echo
fi

../../bin/qemu-system-arm -kernel kernel.bin -initrd kernel.bin -cpu arm1176 -m 256 -M raspi -serial stdio $DO_DEBUG

