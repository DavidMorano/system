/* ucreadlink SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* read a symbolic link */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>
#include	<unistd.h>
#include	<cstring>
#include	<usystem.h>
#include	<libmallocxx.h>
#include	<nonpath.h>
#include	<localmisc.h>

#pragma		GCC dependency	"mod/libutil.ccm"

import libutil ;			/* |getlenstr(3u)| */

/* local defines */

#define	FSTYPE_FLOAT	"float"

#ifdef	_ST_FSTYPSZ
#define	FSTYPESZ	_ST_FSTYPSZ
#else
#define	FSTYPESZ	16
#endif


/* external subroutines */

extern "C" {
    extern int	mkuserpath(char *,cchar *,cchar *,int) noex ;
    extern int	mkcdpath(char *,cchar *,int) noex ;
    extern int	mkvarpath(char *,cchar *,int) noex ;
    extern int	hasvarpathprefix(cchar *,int) noex ;
}


/* forward references */


/* exported variables */


/* exported subroutines */

int uc_readlink(cchar *fname,char *rbuf,int rlen) noex {
    	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	int		len = 0 ; /* return-value */
	if (fname && rbuf) {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if (int fl ; (fl = gelenstr(fname)) > 0) {
	        if ((rs = nonpath(fname,fl)) > 0) {
		    if (char *ebuf ; (rs = libmalloc_mp(&ebuf)) >= 0) {
	                if ((rs = mkuserpath(ebuf,np,fname,fl)) > 0) {
	                    fname = ebuf ;
	                } else if (rs == 0) {
	                    if ((rs = mkcdpath(ebuf,fname,fl)) > 0) {
	                        fname = ebuf ;
	                    } else if (rs == 0) {
	                        rs = mkvarpath(ebuf,fname,fl) ;
	                        if (rs > 0) fname = ebuf ;
		            }
	                } /* end if */
	                if (rs >= 0) {
	                    rs = u_readlink(fname,rbuf,rlen) ;
			    len = rs ;
	                }
			rs = rslibfree(rs,ebuf) ;
		    } /* end if (m-a-f) */
	        } else if (rs >= 0) {
		    rs = u_readlink(fname,rbuf,rlen) ;
		    len = rs ;
	        } /* end if (nonpath or other) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (uc_readlink) */


