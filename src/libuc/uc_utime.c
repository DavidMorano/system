/* uc_utime */

/* set UNIX® times on a file */
/* translation layer interface for UNIX® equivalents */


#define	CF_DEBUGS	0		/* compile-time debugging */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This is the middleware version of |u_utime(3u)|.

	Notes:
	1. The fields of the UTIMBUF object are:
		actime 		- access time in seconds
		modtime 	- modification time in seconds

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<utime.h>		/* for |utimbuf| */
#include	<usystem.h>


/* local defines */


/* external subroutines */

extern int	mkexpandpath(char *,cchar *,int) ;

#if	CF_DEBUGS
extern int	debugprintf(cchar *,...) ;
extern int	strlinelen(cchar *,int,int) ;
#endif


extern cchar	*getourenv(cchar **,cchar *) ;

/* exported subroutines */


int uc_utime(cchar *fname,const struct utimbuf *utp)
{
	const int	elen = MAXPATHLEN ;
	int		rs ;
	char		*ebuf ;

	if ((rs = uc_libmalloc((elen+1),&ebuf)) >= 0) {
	    if ((rs = mkexpandpath(ebuf,fname,-1)) > 0) {
		rs = u_utime(ebuf,utp) ;
	    } else if (rs == 0) {
		rs = u_utime(fname,utp) ;
	    }
	    uc_libfree(ebuf) ;
	} /* end if (m-a) */

	return rs ;
}
/* end subroutine (uc_utime) */


