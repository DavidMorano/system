# MAKEFILE

T= newmail

SRCROOT= $(LOCAL)


BINDIR= $(SRCROOT)/bin


all:		$(T)


$(T):		$(T).ksh
	cp -p $(T).ksh $(T)

install:	$(T)
	makenewer -r $(T) $(BINDIR)

clean:
	rm -f $(T)


