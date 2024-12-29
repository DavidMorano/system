/* progstrmailer SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* get the "mailer"-name string if we can */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	The subroutine was written from scratch but based on previous
	versions of the 'mkmsg' program.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	progstrmailer

	Description:
	Output the "x-mailer" header.

	Synopsis:
	int progstrmailer(progifo *pip) noex

	Returns:
	>=0	OK
	<0	error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<buffer.h>
#include	<filereadstrs.h>
#include	<mkpathx.h>
#include	<localmisc.h>

#include	"config.h"
#include	"defs.h"


/* local defines */


/* external subroutines */

extern "C" {
    extern int	progexpcook_subbuf(proginfo *,buffer *,cchar *,int) noex ;
}


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int progstrmailer(proginfo *pip,cchar *strp) noex {
	cint		llen = LINEBUFLEN ;
	int		rs = SR_OK ;
	int		strl = -1 ;
	char		lbuf[LINEBUFLEN+1] ;

	if (pip == NULL) return SR_FAULT ;

	if (strp == NULL) strp = STR_MAILER ;

	if (strp == NULL) {
	    cchar	*sfname = STRMAILERFNAME ;
	    char	fbuf[MAXPATHLEN+1] ;
	    if (sfname[0] != '/') {
		rs = mkpath2(fbuf,pip->pr,sfname) ;
		sfname = fbuf ;
	    }
	    if (rs >= 0) {
	        strp = lbuf ;
	        rs = filereadstrs(lbuf,llen,sfname) ;
		strl = rs ;
	    }
	}

	if ((rs >= 0) && (strp != NULL)) {
	    buffer	b ;
	    cint	bsize = (strl >= 0) ? (2*strl) : 50 ;
	    if ((rs = buffer_start(&b,bsize)) >= 0) {
	        if ((rs = progexpcook_subbuf(pip,&b,strp,strl)) >= 0) {
		    cchar	*bp ;
		    if ((rs = buffer_get(&b,&bp)) >= 0) {
			cchar	**vpp = &pip->hdr_mailer ;
			rs = proginfo_setentry(pip,vpp,bp,rs) ;
		    }
		}
		strl = buffer_finish(&b) ;
		if (rs >= 0) rs = strl ;
	    } /* end if (buffer) */
	} /* end if (string-expand) */

	return rs ;
}
/* end subroutine (progstrmailer) */


