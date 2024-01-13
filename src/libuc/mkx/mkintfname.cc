/* mkintfname SUPPORT */
/* lang=C++20 */

/* make an open-intercept filename from components */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-12-03, David A­D­ Morano
	This code was born out of frustration with cleaning up bad
	legacy code (of which there is quite a bit -- like almost
	all of it).

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mkintfname

	Description:
	This subroutine constructs an open-intercept filename (from
	its constituent parts).  An open-intercept filename looks
	like:
		[<dn>/]<prn>ô<inter>

	Synopsis:
	int mkintfname(char *rbuf,cchar *dn,cchar *prn,cchar *inter) noex

	Arguments:
	rbuf		result buffer
	dn		directory name
	prn		program-root name
	inter		intercept service

	Returns:
	>0		result string length
	==		?
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<cstdarg>
#include	<cstring>
#include	<usystem.h>
#include	<bufsizevar.hh>
#include	<storebuf.h>
#include	<localmisc.h>

#include	"mkx.h"


/* external subroutines */


/* external variables */


/* local strutures */


/* forward references */


/* local variables */

static bufsizevar	maxpathlen(getbufsize_mp) ;


/* exported variables */


/* exported subroutines */

int mkintfname(char *rbuf,cchar *dn,cchar *prn,cchar *inter) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (rbuf && prn && inter) {
	    rbuf[0] = '\0' ;
	    if ((rs = maxpathlen) >= 0) {
		cint	rlen = rs ;
	        if ((rs >= 0) && dn && dn[0]) {
	            if (rs >= 0) {
	                rs = storebuf_strw(rbuf,rlen,i,dn,-1) ;
	                i += rs ;
	            }
	            if ((rs >= 0) && (i > 0) && (rbuf[i-1] != '/')) {
	                rs = storebuf_char(rbuf,rlen,i,'/') ;
	                i += rs ;
	            }
	        } /* end if (had a directory) */
	        if (rs >= 0) {
	            rs = storebuf_strw(rbuf,rlen,i,prn,-1) ;
	            i += rs ;
	        }
	        if (rs >= 0) {
	            rs = storebuf_char(rbuf,rlen,i,'º') ;
	            i += rs ;
	        }
	        if (rs >= 0) {
	            rs = storebuf_strw(rbuf,rlen,i,inter,-1) ;
	            i += rs ;
	        }
	    } /* end if (maxpathlen) */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (mkintfname) */


