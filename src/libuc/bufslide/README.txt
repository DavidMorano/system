BUFSLIDE

This object implements a sliding buffer.  This is very similar to
(and almost identical to) a FIFO.

Usage:

int	rs ;
int	rs1 ;
if (bufslide ob ; (rs = ob.start) >= 0) {
    for (;;) {
	rs = ob.add(...) ;
	if (rs < 0) break ;
    } /* end for */
    rs1 = ob.finish ;
    if (rs >= 0) rs = rs1 ;
} /* end if (ob) */

