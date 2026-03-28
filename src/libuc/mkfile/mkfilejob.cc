/* mkfilejob SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* make a temporary job file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-10, David A­D­ Morano
	This code was originally written.

	= 1999-04-19, David A­D­ Morano
	I made a correction for short filenames when the host name
	is less than 4 characters long!

*/

/* Copyright © 1998,1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mkfilejob

	Description:
	Make a file which serves as a job submital file.  The file
	will have a name which is, as nearly as possible, totally
	unique throughout the world when it is always made by this
	routine.

	Synopsis:
	int mkfilejob(char *rbuf,cchar *dname,mode_t fm) noex

	Arguments:
	rbuf	result buffer pointer (should be |maxpathlen|+1 in size)
	dname	directory name to create file in
	fm	file-mode of created file

	Returns:
	>=0	OK (and length in bytes of the result)
	<0	error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<getnodename.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"mkfile.h"


/* local defines */

#define	PREFIXLEN	4		/* file-name prefix length */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int mkform(char *,cchar *,mode_t) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int mkfilejob(char *rbuf,cchar *dname,mode_t fm) noex {
	int		rs = SR_FAULT ;
	int		rl = 0 ;
	if (rbuf && dname) ylikely {
	    rs = SR_INVALID ;
	    if (dname[0]) ylikely {
		if (ustat sb ; (rs = u_stat(dname,&sb)) >= 0) ylikely {
	    	    rs = SR_NOTDIR ;
	            if (S_ISDIR(sb.st_mode)) {
			cmode	pm = (fm & S_IAMB) ;
			rs = mkform(rbuf,dname,pm) ;
			rl = rs ;
		    } /* end if (was directory) */
		} /* end if (u_stat) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (mkfilejob) */


/* local subroutines */

local int mkform(char *rbuf,cchar *dname,mode_t fm) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rl = 0 ;
	if (char *nbuf ; (rs = lm_nn(&nbuf)) >= 0) ylikely {
	    cint	nlen = rs ;
	    if ((rs = getnodename(nbuf,nlen)) >= 0) ylikely {
		cint	pl = PREFIXLEN ;
		if (rs > pl) nbuf[pl] = '\0' ;
		rs = mkfileart(rbuf,dname,nbuf,0,fm) ;
		rl = rs ;
	    } /* end if (getnodename) */
	    rs1 = lm_free(nbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (mkform) */


