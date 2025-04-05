/* getsystypenum SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* get the 'osts' and 'osnum' for the current system and release */
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
	We find (or make) a system type-name from the combination
	of the system 'sysname" and 'release'.  We also somehow
	divine a system OS number.

	Synopsis:
	int getsystypenum(char *rbuf,char *nbuf,cc *sysname,cc *release) noex

	Arguments:
	tbuf		buffer for 'osts' result
	nbuf		buffer for 'osnum' result
	sysname		specified SYSNAME string
	release		specified RELEASE string

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| + |strchr(3c)| */
#include	<usystem.h>
#include	<getbufsize.h>
#include	<matstr.h>
#include	<snwcpy.h>
#include	<localmisc.h>

#include	"getsystypenum.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	getfield(cchar *,int,cchar **) noex ;


/* local variables */

enum sysnames {
	sysname_sunos,
	sysname_darwin,
	sysname_linux,
	sysname_overlast
} ;

constexpr cpcchar	sysnames[] = {
	"SunOS",
	"Darwin",
	"Linux",
	nullptr
} ;


/* exported variables */


/* exported subroutines */

int getsystypenum(char *tbuf,char *nbuf,cchar *sysname,cchar *release) noex {
	int		rs = SR_FAULT ;
	if (tbuf && nbuf && sysname && release) {
	    rs = SR_INVALID ;
	    tbuf[0] = '\0' ;
	    nbuf[0] = '\0' ;
	    if (sysname[0] && release[0]) {
		if ((rs = getbufsize(getbufsize_un)) >= 0) {
		    cint	olen = rs ;
	            if (int si ; (si = matstr(sysnames,sysname,-1)) >= 0) {
	                int	cl ;
	                cchar	*osts = nullptr ;
	                cchar	*cp ;
	                rs = SR_NOTFOUND ;
	                switch (si) {
	                case sysname_sunos:
	                case sysname_linux:
		            osts = "SYSV" ;
		            if ((cl = getfield(release,1,&cp)) > 0) {
		                rs = snwcpy(nbuf,olen,cp,cl) ;
		            }
		            break ;
	                case sysname_darwin:
		            osts = "BSD" ;
		            if ((cl = getfield(release,0,&cp)) > 0) {
		                rs = snwcpy(nbuf,olen,cp,cl) ;
		            }
		            break ;
	                } /* end switch */
	                if ((rs >= 0) && osts) {
		            rs = snwcpy(tbuf,olen,osts) ;
	                }
	            } else {
	                rs = SR_NOTFOUND ;
	            }
		} /* end if (getbufsize) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (getsystypenum) */


/* local subroutines */

static int getfield(cchar *sp,int n,cchar **rpp) noex {
	int		i = 0 ;
	int		cl = -1 ;
	cchar		*tp ;
	cchar		*cp = nullptr ;
	while ((tp = strchr(sp,'.')) != nullptr) {
	    if (i == n) {
		cp = sp ;
		cl = intconv(tp - sp) ;
		break ;
	    }
	    sp = (tp+1) ;
	    i += 1 ;
	} /* end if */
	if ((cp == nullptr) && sp[0] && (i == n)) {
	    cp = sp ;
	    cl = xstrlen(sp) ;
	}
	if (rpp) {
	    *rpp = cp ;
	}
	return cl ;
}
/* end subroutine (getfield) */


