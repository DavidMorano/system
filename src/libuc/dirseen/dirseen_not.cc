/* dirseen_not SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* some extra utility subroutines (methods) for the dirseen object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
    	dirseen_notseen
    	dirseen_notadd

	Description:
	This pair of subroutines makes finding previously not seen
	directories easier.

	Synopsis:
	int dirseen_notseen(dirseen *op,ustat *sbp,cchar *dbuf,int dlen) noex
	int dirseen_notadd(dirseen *op,ustat *sbp,cchar *dbuf,int dlen) noex

	Arguments:
	op		pointer to object
	sbp		pointer to ustat structure
	dbuf		directory buffer
	dlen		length of string in directory buffer

	Returns:
	>0		not yet seen
	==0		previously seen
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<pathclean.h>
#include	<strnul.hh>
#include	<isnot.h>
#include	<localmisc.h>

#include	"dirseen.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int dirseen_notseen(dirseen *op,ustat *sbp,cchar *dbuf,int dlen) noex {
	cint		rsn = SR_NOENT ;
	int		rs ;
	int		f = false ;
	if ((rs = dirseen_magic(op,sbp,dbuf)) >= 0) ylikely {
	    sbp->st_ino = 0 ;
	    sbp->st_dev = 0 ;
	    if ((rs = dirseen_havename(op,dbuf,dlen)) == rsn) {
		if (strnul ns(dbuf,dlen) ; (rs = u_stat(ns,sbp)) >= 0) {
	            if (S_ISDIR(sbp->st_mode)) {
	    	        if ((rs = dirseen_havedevino(op,sbp)) == rsn) {
			    rs = SR_OK ;
			    f = true ;
		        }
		    }
	        } else if (isNotPresent(rs)) {
		    rs = SR_OK ;
	        }
	    } /* end if (dirseen_havename) */
	} /* end if (magic) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (dirseen_notseen) */

int dirseen_notadd(dirseen *op,ustat *sbp,cchar *dbuf,int dlen) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = dirseen_magic(op,sbp,dbuf)) >= 0) ylikely {
	    if (char *cbuf ; (rs = malloc_mp(&cbuf)) >= 0) {
	        if ((rs = pathclean(cbuf,dbuf,dlen)) > 0) {
	            rs = dirseen_add(op,cbuf,rs,sbp) ;
	        }
	        rs1 = malloc_free(cbuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dirseen_notadd) */


/* local subroutines */

int dirseen::notseen(ustat *sbp,cchar *dbuf,int dlen) noex {
	return dirseen_notseen(this,sbp,dbuf,dlen) ;
}

int dirseen::notadd(ustat *sbp,cchar *dbuf,int dlen) noex {
	return dirseen_notadd(this,sbp,dbuf,dlen) ;
}


