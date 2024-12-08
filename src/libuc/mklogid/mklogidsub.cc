/* mklogidsub SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* make a sub (or serial) log-ID */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mklogidsub

	Description:
	This subroutine makes a composite (also called a "serial")
	log-ID for the PCSPOLL program (or other programs that have
	sub-jobs associated with them and where they require a
	prefix log ID).  A composite log-id has the form:
		<base>.<serial>
	where:
		<base>		is a base-string
		<serial>	is a serial-string (string of digits)
	The serial part is a maximum of six digits.  The base-string
	is adjusted to fit until it reaches a minimum of three
	characters.  The serial-string is then adjusted down to a
	minimum before failure is returned.

	Synopsis:
	int mklogidsub(char *dbuf,int dlen,cchar *bname,int v) noex

	Arguments:
	dbuf		destination buffer (for result)
	dlen		length of destination buffer (desired length)
	bname		base-name string
	v		value

	Returns:
	>=0		length of resulting string
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* for |strlen(3c)| */
#include	<usystem.h>
#include	<ctdec.h>
#include	<storebuf.h>
#include	<localmisc.h>

#include	"mklogid.h"


/* local defines */

#define	MINBAS		3		/* minimum base-name characters */
#define	MINDIG		2		/* minimum */
#define	MIDBAS		4		/* minimum base-name characters */
#define	MIDDIG		5		/* minimum */
#define	MAXBAS		5
#define	MAXDIG		6		/* same as PID_MAX? */

#define	MIDDLECHAR	'.'		/* middle joining character */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	mkjoin(char *,int,cchar *,int,cchar *,int) noex ;


/* local variables */

constexpr int	vlen = DIGBUFLEN ;


/* exported variables */


/* exported subroutines */

int mklogidsub(char *dbuf,int dlen,cchar *bname,int v) noex {
	int		rs = SR_FAULT ;
	if (dbuf && bname) {
	    rs = SR_INVALID ;
	    if (dlen < 0) dlen = LOGIDLEN ;
      	    dbuf[0] = '\0' ;
	    if (v >= 0) {
        	int		dmax ;
        	int		al ;
        	int		bl ;
        	int		vl = 0 ;
        	cchar		*bp ;
        	cchar		*vp ;
        	char		vbuf[vlen + 1] ;
        	rs = SR_OK ;
        	vp = vbuf ;
        	if (v >= 0) {
        	    rs = ctdeci(vbuf,vlen,v) ;
        	    vl = rs ;
        	}
        	if (rs >= 0) {
        	    bp = bname ;
        	    bl = strlen(bname) ;
        	    if ((bl + 1 + vl) > dlen) {
        	        dmax = MAXDIG ;
        	        if (vl > dmax) {
        	            al = (vl - dmax) ;
        	            vp += al ;
        	            vl -= al ;
        	        }
        	    } /* end if */
        	    if ((bl + 1 + vl) > dlen) {
        	        dmax = MAXBAS ;
        	        if (bl > dmax) {
        	            al = (bl - dmax) ;
        	            bl -= al ;
        	        }
        	    } /* end if */
        	    if ((bl + 1 + vl) > dlen) {
        	        dmax = MIDDIG ;
        	        if (vl > dmax) {
        	            al = (vl - dmax) ;
        	            vp += al ;
        	            vl -= al ;
        	        }
        	    } /* end if */
        	    if ((bl + 1 + vl) > dlen) {
        	        dmax = MIDBAS ;
        	        if (bl > dmax) {
        	            al = (bl - dmax) ;
        	            bl -= al ;
        	        }
        	    } /* end if */
        	    if ((bl + 1 + vl) > dlen) {
        	        dmax = MINDIG ;
        	        if (vl > dmax) {
        	            al = (vl - dmax) ;
        	            vp += al ;
        	            vl -= al ;
        	        }
        	    } /* end if */
        	    if ((bl + 1 + vl) > dlen) {
        	        dmax = MINBAS ;
        	        if (bl > dmax) {
        	            al = (bl - dmax) ;
        	            bl -= al ;
        	        }
        	    } /* end if */
        	    rs = mkjoin(dbuf,dlen,bp,bl,vp,vl) ;
        	} /* end if (ok) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mklogidsub) */


/* local subroutines */

static int mkjoin(char *dbuf,int dlen,cchar *bp,int bl,cchar *vp,int vl) noex {
	int		rs ;
	int		idx = 0 ;
	if (storebuf sb(dbuf,dlen) ; (rs = sb.strw(bp,bl)) >= 0) {
	    if ((vl >= 0) && vp[0]) {
	        if ((rs = sb.chr(MIDDLECHAR)) >= 0) {
	            if ((rs = sb.strw(vp,vl)) >= 0) {
		    	idx = sb.idx ;
		    }
		}
	    } /* end if (trailing part) */
	} /* end if (base part) */
	return (rs >= 0) ? idx : rs ;
}
/* end subroutine (mkjoin) */


