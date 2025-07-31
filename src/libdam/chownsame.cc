/* chownsame SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* make all directories in a directory path */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-10, David A­D­ Morano
	This code was originally written.  

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	chownsame

  	Description:
	This subroutine changes the owner of a file (or directory)
	to be the same as another file (or directory) given as a
	reference.

	Synopsis:
	int chownsame(cchar *dname,cchar *ref)

	Arguments:
	dname		direcrtory path to a new directory to create
	ref		reference file or directory

	Returns:
	>0		ownership changed
	==0		ownership not changed
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>
#include	<unistd.h>		/* |_PC_{xx}| */
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<localmisc.h>

#include	"chownsame.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* external subroutines */

int chownsame(cchar *dname,cchar *ref) noex {
    	cnullptr	np{} ;
	cint		cmd = _PC_CHOWN_RESTRICTED ;
	int		rs = SR_FAULT ;
	int		f = false ; /* return-value */
	if (dname && ref) {
	    rs = SR_INVALID ;
	    if (dname[0] && ref[0]) {
	        if ((rs = uc_pathconf(dname,cmd,np)) == 0) {
	            if (ustat sb ; (rs = uc_stat(ref,&sb)) >= 0) {
	                f = true ;
	                rs = uc_chown(dname,sb.st_uid,sb.st_gid) ;
	            } /* end if (uc_stat) */
	        } /* end if (uc_pathconf) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (chownsame) */

int chownref(cchar *dname,cchar *ref) noex {
    	return chownsame(dname,ref) ;
}


