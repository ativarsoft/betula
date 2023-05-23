DMD=ldc2
ifeq ($(DMD),dmd)
DFLAGS=-fPIC -betterC
else
DFLAGS=-relocation-model=pic --betterC -nodefaultlib
endif

%.o: %.d
	$(DMD) -c $(DFLAGS) -of="$@" $<

