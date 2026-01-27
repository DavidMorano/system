/* mksofname SUPPORT */
/* charset=ISO8859-1 */
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

  	Name:
	mksofname

	Description:
	This subroutine constructs a full-path filename from various
	specified components.

	Synopsis:
	int mksofname(char *rbuf,cchar *dn,cchar *name,cchar *ext) noex

	Arguements:
	rbuf		result buffer
	dn		directory name
	name		base-name
	ext		extension

	Returns:
	>=0		result buffer length
	<0		error (system-return)

	See-also:
	mksofname
	mksoname
	mkshlibname

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<bufsizevar.hh>
#include	<storebuf.h>
#include	<localmisc.h>

#include	"mksofname.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

static bufsizevar	maxpathlen(bufsize_mp) ;


/* exported variables */


/* exported subroutines */

int mksofname(char *rbuf,cchar *dn,cchar *name,cchar *ext) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (rbuf && dn && name && ext) ylikely {
	    rs = SR_INVALID ;
	    if (dn[0] && name[0]) ylikely {
		if ((rs = maxpathlen) >= 0) ylikely {
		    if (storebuf sb(rbuf,rs) ; (rs = sb.strw(dn)) > 0) ylikely {
	                if (dn[rs - 1] != '/') {
	                    rs = sb.chr('/') ;
			}
	                if (rs >= 0) ylikely {
	                    rs = sb.strw(name) ;
	                }
	                if (ext[0]) {
	                    if ((rs >= 0) && (ext[0] != '.')) {
	                        rs = sb.chr('.') ;
	                    }
	                    if (rs >= 0) ylikely {
	                        rs = sb.strw(ext) ;
	                    }
	                } /* end if (had extension) */
		        len = sb.idx ;
		    } /* end if (storebuf) */
		} /* end if (maxpathlen) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (mksofname) */


