DMD=ldc2
ifeq ($(DMD),dmd)
DFLAGS=-fPIC -betterC
else
DFLAGS=-relocation-model=pic --betterC -nodefaultlib -preview=dip1021
endif

%.o: %.d
	$(DMD) -c $(DFLAGS) -of="$@" $<

