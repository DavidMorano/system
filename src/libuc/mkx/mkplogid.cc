/* mkplogid SUPPORT */
/* lang=C++20 */

/* make a prefix log ID (PrefixLogId) */
/* version %I% last-modified %G% */


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
	>=0		length of result
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<climits>
#include	<cstdlib>
#include	<cstring>		/* for |strlen(3c)| */
#include	<usystem.h>
#include	<ctdec.h>
#include	<strwcpy.h>
#include	<ndigit.h>
#include	<ipow.h>
#include	<localmisc.h>		/* |DIGBUFLEN| */

#include	"mkx.h"


/* local defines */

#define	MAXNC		10		/* total maximum characters */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr int	dlen = DIGBUFLEN ;


/* exported variables */


/* exported subroutines */

int mkplogid(char *rbuf,int rlen,cchar *nodename,int v) noex {
	int		rs = SR_FAULT ;
	int		tl = 0 ;
	if (rbuf && nodename) {
	    int		nl = strlen(nodename) ;
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if (v >= 0) {
		static int	rsm = ndigit(PID_MAX,10) ;
		if ((rs = rsm) >= 0) {
	            cint	maxdigs = rs ;
		    int		modval ;
		    int		ni = 0 ;
		    int		di = 0 ;
		    char	dbuf[dlen + 1] ;
		    modval = (maxdigs < 10) ? ipow(10,maxdigs) : INT_MAX ;
		    v = (v % modval) ; /* limits the decimal part to maxdigs */
		    if ((rs = ctdeci(dbuf,dlen,v)) >= 0) {
	    	        cint	dl = rs ;
	    	        tl = (nl + dl) ;
	                for (int ch = 0 ; tl > MAXNC ; ch += 1) {
	                    switch (ch) {
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
	                if ((rlen < 0) || (tl <= rlen)) {
	                    char	*rp = rbuf ;
	                    rp = strwcpy(rp,(nodename + ni),nl) ;
	                    rp = strwcpy(rp,(dbuf + di),dl) ;
	                } else {
	                    rs = SR_OVERFLOW ;
	               }
	            } /* end if (ctdeci) */
	        } /* end if (ndigit) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? tl : rs ;
}
/* end subroutine (mkplogid) */


