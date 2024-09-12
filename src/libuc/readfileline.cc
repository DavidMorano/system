/* filereadln SUPPORT */
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

int filereadln(cchar *fn,char *rbuf,int rlen) noex {
	cint		to = -1 ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (fn && rbuf) {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if (fn[0]) {
		if (char *lbuf{} ; (rs = malloc_ml(&lbuf)) >= 0) {
		    cint	llen = rs ;
		    cint	of = O_RDONLY ;
		    cmode	om = 0666 ;
	            if ((rs = uc_open(fn,of,om)) >= 0) {
	                filer	b ;
	                cint	fd = rs ;
	                if ((rs = filer_start(&b,fd,0L,512,0)) >= 0) {
	                    while ((rs = filer_readln(&b,lbuf,llen,to)) > 0) {
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


