/* mfs-debug SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* MFS-debug */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* switchable at invocation */

/* revision history:

	= 2011-01-25, David A­D­ Morano
	I had to separate this code due to AST-code conflicts over
	the system socket structure definitions.

	= 2017-08-10, David A­D­ Morano
	This subroutine was borrowed to code MFSERVE.

*/

/* Copyright © 2011,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Description:
	This is MFS used for debugging.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */

#include	"mfsmain.h"
#include	"mfsconfig.h"
#include	"mfslocinfo.h"
#include	"mfslog.h"
#include	"proginfo.hh"


/* local typedefs */


/* local defines */

#ifndef	PBUFLEN
#define	PBUFLEN		(4 * MAXPATHLEN)
#endif

#ifndef	VBUFLEN
#define	VBUFLEN		(4 * MAXPATHLEN)
#endif

#ifndef	EBUFLEN
#define	EBUFLEN		(3 * MAXPATHLEN)
#endif

#define	NDF		"/tmp/mfs.deb"


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int mfsdebug_lockprint(PI *pip,cchar *place) noex {
	int		rs = SR_OK ;

	if (pip == NULL) return SR_FAULT ;

#if	CF_DEBUG
	if (DEBUGLEVEL(5)) {
	    LOCINFO	*lip = pip->lip ;
	    bfile	lf ;
	    int		rs1 ;
	    cchar	*lockfname = lip->pidfname ;
	    if (place != NULL)
	        debugprintf("mfsdebug_lockprint: place=%s\n",place) ;
	    debugprintf("mfsdebug_lockprint: lockfname=%s\n",lockfname) ;
	    if ((rs1 = bopen(&lf,lockfname,"r",0666)) >= 0) {
	        cint	llen = LINEBUFLEN ;
	        char		lbuf[LINEBUFLEN+1] ;
	        while ((rs1 = breadln(&lf,lbuf,llen)) > 0) {
	            int	ll = strllen(lbuf,rs1,60) ;
	            debugprintf("mfsdebug_lockprint: >%r<\n",lbuf,ll) ;
	        }
	        bclose(&lf) ;
	    }
	    debugprintf("mfsdebug_lockprint: end rs=%d\n",rs1) ;
	}
#endif /* CF_DEBUG */

	return rs ;
}
/* end subroutine (mfsdebug_lockprint) */


