BUFSLIDE

This object implements a sliding buffer.  This is very similar to
(and almost identical to) a FIFO.

Usage:

if (bufslide ob ; (rs = start) >= 0) {
    for (;;) {
	rs = ob.add(...) ;
	if (rs < 0) break ;
    } /* end for */
} /* end if (ob) */

