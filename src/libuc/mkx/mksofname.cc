/* mksofname SUPPORT */
/* lang=C++20 */

/* make a shared-object filename from components */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-12-03, David A­D­ Morano
	This code was born out of frustration with cleaning up bad
	legacy code (of which there is quite a bit -- like almost
	all of it).

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine constructs a filename from various specified
	components.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<cstring>
#include	<usystem.h>
#include	<bufsizevar.hh>
#include	<storebuf.h>


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local strutures */


/* forward references */


/* local variables */

static bufsizevar	maxpathlen(getbufsize_mp) ;


/* exported variables */


/* exported subroutines */

int mksofname(char *rbuf,cchar *dn,cchar *name,cchar *ext) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (rbuf && dn && name && ext) {
	    rs = SR_INVALID ;
	    if (dn[0] && name[0]) {
		if ((rs = maxpathlen) >= 0) {
		    cint	rlen = rs ;
	            if (rs >= 0) {
	                rs = storebuf_strw(rbuf,rlen,i,dn,-1) ;
	                i += rs ;
	            }
	            if ((rs >= 0) && (i > 0) && (rbuf[i-1] != '/')) {
	                rs = storebuf_char(rbuf,rlen,i,'/') ;
	                i += rs ;
	            }
	            if (rs >= 0) {
	                rs = storebuf_strw(rbuf,rlen,i,name,-1) ;
	                i += rs ;
	            }
	            if (ext[0]) {
	                if ((rs >= 0) && (ext[0] != '.')) {
	                    rs = storebuf_char(rbuf,rlen,i,'.') ;
	                    i += rs ;
	                }
	                if (rs >= 0) {
	                    rs = storebuf_strw(rbuf,rlen,i,ext,-1) ;
	                    i += rs ;
	                }
	            } /* end if (had extension) */
		} /* end if (maxpathlen) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (mksofname) */


