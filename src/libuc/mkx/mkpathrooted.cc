/* mkpathrooted */
/* lang=C++20 */

/* make a file rooted from the present-working-directory */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mkfdfnam

	Description:
	This subroutine creates a string that represents a file
	that is actually already open on a given file-descriptor
	(FD).  The filename looks like:
		*<fd>
	where:
	fd		is the decimal representation of the FD

	Synopsis:
	int mkfdfname(char *dbuf,int dlen,int fd)

	Arguments:
	dbuf		destination buffer pointer
	dlen		destination buffer length
	int		fd ;

	Returns:
	>=0		length of created string
	<0		error

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>
#include	<mkpath.h>
#include	<libmallocxx.h>
#include	<getpwd.h>
#include	<localmisc.h>

#include	"mkpathrooted.h"


/* local defines */


/* external subroutines */


/* forward references */


/* local variables */


/* exported subroutines */

int mkpathrooted(char *rbuf,cchar *fn) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rl = 0 ;
	if (rbuf && fn) {
	    rs = SR_INVALID ;
	    if (fn[0]) {
	        if (fn[0] != '/') {
	            char	*pbuf{} ;
	            if ((rs = libmalloc_mp(&pbuf)) >= 0) {
	                cint	plen = rs ;
	                if ((rs = getpwd(pbuf,plen)) >= 0) {
	                    rs = mkpath2(rbuf,pbuf,fn) ;
		            rl = rs ;
	                }
	                rs1 = uc_libfree(pbuf) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (rooting file) */
	        } else {
		    rs = mkpath(rbuf,fn) ;
		    rl = rs ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end if (mkpathrooted) */


