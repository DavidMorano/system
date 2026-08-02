/* getsystypenum SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the system 'type-number' and 'osnum' for a given system and release */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getsystypenum

	Description:
	Given a system-name c-string and a release c-string, this
	subroutine returns (if found) the corresponding system-type-name
	and OS-number c-string.

	Synopsis:
	int getsystypenum(char *rbuf,char *nbuf,cc *stn,cc *release) noex

	Arguments:
	tbuf		result buffer for 'osts' result
	nbuf		result buffer for 'osnum' result
	stn		specified SYSNAME c-string
	release		specified RELEASE c-string

	Returns:
	>=0		OK length of returned type-name string
	<0		error (system-return)

	Notes:
	1. The two output buffer ('rbuf' and 'tbuf') must be at
	least the length (plus-1) of the username-len on the current
	system.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |strchr(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<bufsizeget.h>		/* LIBUC */
#include	<matstr.h>		/* LIBUC */
#include	<snwcpy.h>		/* LIBUC */
#include	<systypenames.h>	/* |systypename| */
#include	<localmisc.h>		/* LIBU */

#include	"getsystypenum.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int	getfield(cchar *,int,cchar **) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int getsystypenum(char *tbuf,char *nbuf,cchar *stn,cchar *release) noex {
	int		rs = SR_FAULT ;
	if (tbuf && nbuf && stn && release) ylikely {
	    rs = SR_INVALID ;
	    tbuf[0] = '\0' ;
	    nbuf[0] = '\0' ;
	    if (stn[0] && release[0]) ylikely {
		if ((rs = bufsizeget(bufsize_un)) >= 0) ylikely {
		    cint	olen = rs ;
	            if (int si ; (si = matstr(systypename,stn,-1)) >= 0) {
	                int	cl ;
	                cchar	*osts = nullptr ;
	                cchar	*cp ;
	                rs = SR_NOTFOUND ;
	                switch (si) {
	                case systypename_sysv:
	                case systypename_linux:
		            osts = "SYSV" ;
		            if ((cl = getfield(release,1,&cp)) > 0) {
		                rs = snwcpy(nbuf,olen,cp,cl) ;
		            }
		            break ;
	                case systypename_darwin:
		            osts = "BSD" ;
		            if ((cl = getfield(release,0,&cp)) > 0) {
		                rs = snwcpy(nbuf,olen,cp,cl) ;
		            }
		            break ;
			default:
			    rs = SR_BUGCHECK ;
			    break ;
	                } /* end switch */
	                if ((rs >= 0) && osts) {
		            rs = snwcpy(tbuf,olen,osts) ;
	                }
	            } else {
	                rs = SR_NOTFOUND ;
	            }
		} /* end if (bufsizeget) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (getsystypenum) */


/* local subroutines */

local int getfield(cchar *sp,int n,cchar **rpp) noex {
	int		i = 0 ; /* used-afterwards */
	int		cl = -1 ; /* return-value */
	cchar		*cp = nullptr ; /* used-multiple */
	for (cchar *tp ; (tp = strchr(sp,'.')) != nullptr ; ) {
	    if (i == n) {
		cp = sp ;
		cl = intconv(tp - sp) ;
		break ;
	    }
	    sp = (tp + 1) ;
	    i += 1 ;
	} /* end for */
	if ((cp == nullptr) && sp[0] && (i == n)) {
	    cp = sp ;
	    cl = lenstr(sp) ;
	}
	if (rpp) {
	    *rpp = cp ;
	}
	return cl ;
} /* end subroutine (getfield) */


