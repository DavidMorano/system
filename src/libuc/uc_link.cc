/* uc_link */
/* charset=ISO8859-1 */

/* interface component for UNIX® library-3c */
/* POSIX shared-meory ("shm(3rt)') file-unlink */


#define	CF_DEBUGS	0		/* compile-time debugging */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<unistd.h>
#include	<mkx.h>
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */

#if	CF_DEBUGS
extern int	debugprintf(cchar *,...) ;
extern int	strlinelen(cchar *,int,int) ;
#endif


/* exported variables */


/* exported subroutines */

int uc_link(cchar *ofname,cchar *nfname)
{
	const int	plen = MAXPATHLEN ;
	int		rs ;
	int		size = 0 ;
	int		rv = 0 ;
	char		*bp ;

	size += ((plen+1) * 2) ;
	if ((rs = uc_libmalloc(size,&bp)) >= 0) {
	    int		ol = -1 ;
	    char	*obuf = bp + ((plen+1)*0) ;
	    char	*nbuf = bp + ((plen+1)*1) ;
	    if ((rs = mkpathexp(obuf,ofname,-1)) >= 0) {
		if (rs > 0) {
		    ol = rs ;
		    ofname = obuf ;
		}
	        if ((rs = mkuserpath(nbuf,NULL,nfname,ol)) >= 0) {
		    if (rs > 0) nfname = nbuf ;
		    rs = u_link(ofname,nfname) ;
		    rv = rs ;
		}
	    } /* end if */
	    uc_libfree(bp) ;
	} /* end if (m-a) */

	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (uc_link) */


