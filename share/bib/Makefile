# MAKEFILE (for making the bibliographic index file)


T= Ind

ALL= $(T).ia $(T).i


FILES= ieee asi Rbstjissue Rv7man opt computer



all:		$(ALL)

$(T).ia:	$(FILES) Makefile
	mkkey $(FILES) | inv $(T)

$(T):		$(FILES) Makefile
	catbib $(FILES) > $@

$(T).i:		$(FILES) Makefile
	gindxbib -d $(PWD) -o $(T) $(FILES)
	chmod ug+w $@

again:
	rm -f $(ALL) $(T).i*

clean:
	rm -f $(ALL) $(T).i*



