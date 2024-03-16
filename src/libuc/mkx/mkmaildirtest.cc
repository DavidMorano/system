/* mkmaildirtest SUPPORT */
/* lang=C++20 */

/* make a test file-directory name for testing a mail directory */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-12-03, David A­D­ Morano
	This code was born out of frustration with cleaning up bad
	legacy code (of which there is quite a bit -- like almost
	all of it).

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine constructs a filename suitable for testing
	whether a mail-directory is accessible or not.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<cstdarg>
#include	<string.h>
#include	<usystem.h>
#include	<bufsizevar.hh>
#include	<storebuf.h>
#include	<localmisc.h>

#include	"mkx.h"


/* local defines */

#define	MAILDIR_SAVED	":saved"


/* external subroutines */


/* external variables */


/* local strutures */


/* forward references */


/* local variables */

static bufsizevar	maxpathlen(getbufsize_mp) ;


/* exported variables */


/* exported subroutines */

int mkmaildirtest(char *rbuf,cchar *dp,int dl) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (rbuf && dp) {
	    if ((rs = maxpathlen) >= 0) {
	        cint	rlen = rs ;
	        cchar	*saved = MAILDIR_SAVED ;
	        if (rs >= 0) {
	            rs = storebuf_strw(rbuf,rlen,i,dp,dl) ;
	            i += rs ;
	        }
	        if ((rs >= 0) && (i > 0) && (rbuf[i-1] != '/')) {
	            rs = storebuf_char(rbuf,rlen,i,'/') ;
	            i += rs ;
	        }
	        if (rs >= 0) {
	            rs = storebuf_strw(rbuf,rlen,i,saved,-1) ;
	            i += rs ;
	        }
	    } /* end if (maxpathlen) */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (mkmaildirtest) */


