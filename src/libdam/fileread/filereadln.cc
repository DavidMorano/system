/* filereadln SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* read a single line from a file! */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	filereadln

	Description:
	This subroutine reads a single line from a file (the string
	of which means something to someone).

	Synopsis:
	int filereadln(cchar *fname,char *rbuf,int rlen) noex

	Arguments:
	fname		file to read
	rbuf		user supplied buffer to hold result
	rlen		length of user supplied buffer

	Returns:
	>=0		length of returned string
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<filer.h>
#include	<sfx.h>
#include	<sncpyxw.h>
#include	<localmisc.h>

#include	"filereadln.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int filereadln(cchar *fname,char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (fname && rbuf) {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if (fname[0] && (rlen > 0)) {
		if (char *lbuf{} ; (rs = malloc_ml(&lbuf)) >= 0) {
		    cint	llen = rs ;
		    cint	of = O_RDONLY ;
		    cmode	om = 0666 ;
	            if ((rs = uc_open(fname,of,om)) >= 0) {
	                cint	fd = rs ;
	                if (filer b ; (rs = b.start(fd,0z,512,0)) >= 0) {
	                    while ((rs = b.readln(lbuf,llen)) > 0) {
				int	cl ;
		                cchar	*cp{} ;
		                if ((cl = sfcontent(lbuf,rs,&cp)) > 0) {
	                            rs = sncpy1w(rbuf,rlen,cp,cl) ;
	                            len = rs ;
	                        }
	                        if (len > 0) break ;
	                        if (rs < 0) break ;
	                    } /* end while (reading lines) */
	                    rs1 = filer_finish(&b) ;
		            if (rs >= 0) rs = rs1 ;
	                } /* end if (filer) */
	                rs1 = uc_close(fd) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (uc_open) */
	            rs1 = uc_free(lbuf) ;
	            if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (filereadln) */


