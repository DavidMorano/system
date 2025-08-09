# MAKEFILE (pathadd)

T= pathadd

ALL= $(T)



BINDIR= $(REPOROOT)/bin


all:		$(ALL)

$(T):		$(T).fun
	rm -f $(T)
	ln -f $(T).fun $(T)

install:	$(T).fun
	makenewer -r -o rmsuf $(T).fun $(BINDIR)

clean:
	rm -f $(T)


