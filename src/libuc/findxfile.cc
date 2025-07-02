/* findxfile SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* find an executable file */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-11-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	findxfile

	Description:
	This subroutine is used to find an executable file using
	the existing PATH environment variable.

	Synopsis:
	int findxfile(ids *idp,char *rbuf,cchar *pn) noex

	Arguments:
	idp		pointer to IDS object
	rbuf		buffer to receive resulting path
	pn		program-name string to search for

	Returns:
	>0		length of found path
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstring>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<getprogpath.h>
#include	<ids.h>
#include	<vecstr.h>
#include	<mkpathx.h>
#include	<pathclean.h>
#include	<xfile.h>
#include	<strx.h>
#include	<localmisc.h>

#include	"findxfile.h"

import uconstants ;

/* local defines */

#define	NENTS	40		/* initial number path components */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int findxfile(ids *idp,char *rbuf,cchar *pn) noex {
    	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (idp && rbuf && pn) {
	    rbuf[0] = '\0' ;
	    rs = SR_INVALID ;
	    if (pn[0]) {
	        static cchar	*path = getenv(varname.path) ;
	        rs = SR_NOENT ;
	        if (path) {
	            cint	vn = NENTS ;
		    cint	vo = 0 ;
	            bool	f_pwd = false ;
	            if (vecstr plist ; (rs = plist.start(vn,vo)) >= 0) {
		        cchar	*sp = path ;
	                if (char *cbuf{} ; (rs = malloc_mp(&cbuf)) >= 0) {
	                    for (cc *tp ; (tp = strbrk(sp,":;")) != np ; ) {
				cint tl = intconv(tp - sp) ;
	                        if ((tp - sp) == 0) {
	                            f_pwd = true ;
		                }
	                        if ((rs = pathclean(cbuf,sp,tl)) >= 0) {
	                            rs = plist.adduniq(cbuf,rs) ;
	                        }
	                        sp = (tp + 1) ;
	                        if (rs < 0) break ;
	                    } /* end while */
		            rs = rsfree(rs,cbuf) ;
		        } /* end if (m-a-f) */
	                if ((rs >= 0) && (sp[0] != '\0')) {
	                    rs = plist.adduniq(sp,-1) ;
		        }
	                if (rs >= 0) {
			    auto	gp = getprogpath ;
	                    if ((rs = gp(idp,&plist,rbuf,pn,-1)) >= 0) {
		                len = rs ;
			    } else if (rs == SR_NOENT) {
	                         if ((! f_pwd) && ((rs = xfile(idp,pn)) >= 0)) {
	                             rs = mkpath1(rbuf,pn) ;
		                     len = rs ;
		                 }
			    } /* end if */
	                } /* end if */
	                rs1 = plist.finish ;
		        if (rs >= 0) rs = rs1 ;
	            } /* end if (path-list) */
	        } else {
	            if ((rs = xfile(idp,pn)) >= 0) {
		        rs = mkpath(rbuf,pn) ;
		        len = rs ;
	            }
	        } /* end if (non-null) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (findxfile) */


