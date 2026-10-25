/* tliaddr_war SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* subroutine to process socket interface type addresses */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* switchable debug print-outs */

/* revision history:

	= 1999-08-17, David A­D­ Morano
        This subroutine was taken from the LOGDIR/LOGNAME program (fist written
        for the SunOS 4.xx environment in 1989). This whole program is replacing
        serveral that did similar things in the past.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	tliaddr_raw

	Description:
	This subroutine tries to make a socket-style TLI address
	out of the given arguments.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */

#include	"config.h"
#include	"defs.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */
import digtab ;				/* |getdig(3u)| */

/* local defines */

#ifndef	PI
#define	PI		poginfo
#endif


/* external subroutines */


/* external variables */


/* forward references */

int		progout_open(PI *) noex ;
int		progout_close(PI *) noex ;

local int	makehex(char *,int,cchar *,int) noex ;


/* local structures */


/* local variables */


/* exported variables */


/* exported subroutines */

int prograw(PI *pip,cchar *familyname,cchar *netaddr1,cchar *netaddr2) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (netaddr1 && (netaddr1[0] != '\0')) {
	    if ((rs = progout_open(pip)) >= 0) {
		cint	hexlen = HEXBUFLEN ;
	        char	hexbuf[HEXBUFLEN + 1] ;
	        if ((rs = makehex(hexbuf,hexlen,netaddr1,-1)) >= 0) {
		    rs = progout_printf(pip,"%r\n",hexbuf,rs) ;
		}
	        rs1 = progout_close(pip) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (progout) */
	} else {
	    cchar	*pn = pip->progname ;
	    cchar	*fmt = "%s: invalid address specified >%s<\n" ;
	    bprintf(pip->efp,fmt,pn,netaddr1) ;
	} /* end if */
	return rs ;
} /* end subroutine (prograw) */


/* local subroutines */

local int makehex(char *hexbuf,int hexlen,cchar *addr,int µalen) noex {
	int		rs = SR_OK ;
	int		j = 0 ; /* return-value */
	if (cint alen = getlenstr(addr,µalen) ; alen >= 0) {
	    if (int ndig = (2*(alen*2)) ; hexlen >= ndig) {
	        int		v ;
	        hexbuf[j++] = '\\' ;
	        hexbuf[j++] = 'x' ;
	        for (int i = 0 ; i < alen ; i += 1) {
	            v = MKCHAR(addr[i]) ;
	            hexbuf[j++] = getdig((v >> 4) & 15) ;
	            hexbuf[j++] = getdig((v >> 0) & 15) ;
	        } /* end for */
	        hexbuf[j] = '\0' ;
	    } else {
	        rs = SR_OVERFLOW ;
	    }
	} /* end if (getlenstr) */
	return (rs >= 0) ? j : rs ;
} /* end subroutine (makehex) */


