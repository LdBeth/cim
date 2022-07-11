</$objtype/mkfile

TARG=cim

OFILES=ctrans.$O dict.$O

BIN=/$objtype/bin

</sys/src/cmd/mkone

install:
	cp cim.man /sys/man/1/ktrans

uninstall:
	rm /sys/man/1/cim
	rm /$objtype/bin/cim
