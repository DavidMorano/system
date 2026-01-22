/* ecmsg_loadfile SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* load a message from a file */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-06-13, David A­D­ Morano
	This subroutine was written for Rightcore Network Services
	(RNS).

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	ecmsg_loadfile

	Description:
        This little dittie loads a text line from a file.

	Returns:
	>=0	number of bytes read from the file
	<0	error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<bufsizevar.hh>
#include	<intsat.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"ecmsg.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int uc_open(cchar *,int,mode_t) noex ;
    extern int uc_stat(cchar *,ustat *) noex ;
    extern int uc_readn(int,void *,int) noex ;
    extern int uc_close(int) noex ;
}


/* external variables */


/* local structures */


/* forward references */

static int	getlinelen(cchar *) noex ;


/* local variables */

static bufsizevar	maxlinelen(bufsize_ml) ;


/* exported variables */


/* exported subroutines */

int ecmsg_loadfile(ecmsg *op,cchar *fn) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (op && fn) {
	    rs = SR_INVALID ;
	    if (fn[0]) {
	        if ((rs = getlinelen(fn)) >= 0) {
		    int		mlen = rs ;
	            char	*mbuf ;
	            if (mlen > ECMSG_MAXBUFLEN) mlen = ECMSG_MAXBUFLEN ;
	            if ((rs = lm_mall((mlen+1),&mbuf)) >= 0) {
		        cint	of = O_RDONLY ;
		        cmode	om = 0666 ;
	                if ((rs= uc_open(fn,of,om)) >= 0) {
		            cint	mfd = rs ;
		            if ((rs = uc_readn(mfd,mbuf,mlen)) >= 0) {
	                        rs = ecmsg_loadbuf(op,mbuf,rs) ;
		                len = rs ;
		            }
		            rs1 = uc_close(mfd) ;
		            if (rs >= 0) rs = rs1 ;
	                } /* end if (uc_open) */
	                rs1 = lm_free(mbuf) ;
		        if (rs >= 0) rs = rs1 ;
	            } /* end if (m-a-f) */
	        } /* end if (getlinelen) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (ecmsg_loadfile) */


/* local subroutines */

static int getlinelen(cchar *fn) noex {
	USTAT		sb ;
	int		rs ;
	int		mlen = 0 ;
	if ((rs = uc_stat(fn,&sb)) >= 0) {
	    mlen = intsat(sb.st_size) ;
	} else if (isNotPresent(rs)) {
	    rs = maxlinelen ;
	    mlen = rs ;
	}
	return (rs >= 0) ? mlen : rs ;
}
/* end subroutine (getlinelen) */


