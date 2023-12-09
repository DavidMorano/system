/* mkplogid */
/* lang=C20 */

/* make a prefix log ID */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* non-switchable print-outs */
#define	CF_SBUF		0		/* use 'sbuf(3dam)' */

/* revision history:

	= 1998-09-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mkplogid

	Description:
	This subroutine makes a prefix log ID for the PCSPOLL program
	(or other programs that have sub-jobs associated with them
	and where they require a prefix log ID). This "prefix" ID
	is a string which is suitable to be combined with a second
	string (the sub-ID) to form a complete ID suitable for use
	in identifying a sub-job of a daemon (or other) program.

	Synopsis:
	int mkplogid(char *rbuf,int rlen,cchar *nodename,int v) noex

	Arguments:
	rbuf		destination buffer
	rlen		destination length
	nodename	nodename
	v		value

	Returns:
	<0		error
	>=0		length of result

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<limits.h>
#include	<unistd.h>
#include	<stdlib.h>
#include	<string.h>
#include	<usystem.h>
#include	<ctdec.h>
#include	<strwcpy.h>
#include	<localmisc.h>


/* local defines */

#define	MAXNC		10		/* total maximum characters */


/* external subroutines */

extern "C" {
    extern int	ndigits(int,int) noex ;
    extern int	ipow(int,int) noex ;
}


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int mkplogid(char *rbuf,int rlen,cchar *nodename,int v) noex {
	const int	maxdigs = ndigits(PID_MAX,10) ;
	const int	dlen = DIGBUFLEN ;
	int		rs ;
	int		modval ;
	int		nl ;
	int		ni = 0 ;
	int		di = 0 ;
	int		tl = 0 ;
	char		dbuf[DIGBUFLEN+1] ;

	if (rbuf == NULL) return SR_FAULT ;
	if (nodename == NULL) return SR_FAULT ;

	rbuf[0] = '\0' ;
	nl = strlen(nodename) ;

	modval = (maxdigs < 10) ? ipow(10,maxdigs) : INT_MAX ;
	v = (v % modval) ; /* limits the decimal part to maxdigs */

	if ((rs = ctdeci(dbuf,dlen,v)) >= 0) {
	    const int	dl = rs ;

	    tl = nl + dl ;

	    {
	        int	c ;
	        for (c = 0 ; tl > MAXNC ; c += 1) {
	            switch (c) {
	            case 0:
	                if (nl > 3) {
	                    ni += 2 ;
	                    nl -= 2 ;
	                }
	                break ;
	            case 1:
	                if (dl > 5) {
	                    di = (dl-5) ;
	                    nl = 5 ;
	                } else if (nl > 5) {
	                    nl = 5 ;
	                }
	                break ;
	            case 2:
	                nl = 5 ;
	                break ;
	            } /* end switch */
	            tl = nl + dl ;
	        } /* end for */
	    } /* end block */

#if	CF_SBUF
	    {
	        SBUF	b ;
	        if ((rs = sbuf_start(&b,rbuf,rlen)) >= 0) {
		    int	rs1 ;
	            sbuf_strw(&b,(nodename + ni),nl) ;
	            sbuf_strw(&b,(dbuf + di),dl) ;
	            rs1 = sbuf_finish(&b) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (sbuf) */
	    }
#else
	    if ((rlen < 0) || (tl <= rlen)) {
	        char	*rp = rbuf ;
	        rp = strwcpy(rp,(nodename + ni),nl) ;
	        rp = strwcpy(rp,(dbuf + di),dl) ;
	    } else
	        rs = SR_OVERFLOW ;
#endif /* CF_SBUF */

	} /* end if (ctdeci) */

	return (rs >= 0) ? tl : rs ;
}
/* end subroutine (mkplogid) */


