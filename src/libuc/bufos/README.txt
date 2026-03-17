BUFOS

This ia tiny outbuf buffer hack of some kind.  Somewhere something
like this was thought to have been needed (somewhere) and so we
have it (to this present day).  Enjoy!

Usage:

if (bufos ob ; (rs = start) >= 0) {
    for (;;) {
	rs = ob.add(...) ;
	if (rs < 0) break ;
    } /* end for */
} /* end if (ob) */

