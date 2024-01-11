/* mkjobfile SUPPORT */
/* lang=C++20 */

/* make a temporary job file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-10, David A­D­ Morano
	This subroutine was originally written.

	= 1999-04-19, David A­D­ Morano
	I made a correction for short filenames when the host name
	is less than 4 characters long!

*/

/* Copyright © 1998,1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mkjobfile

	Description:
	Make a file which serves as a job submital file.  The file
	will have a name which is, as nearly as possible, totally
	unique throughout the world when it is always made by this
	routine.

	Synopsis:
	int mkjobfile(char *rbuf,mode_t fm,cchar *tmpdname) noex

	Arguments:
	- result buffer pointer (should be |maxpathlen|+1 in size)
	- file creation mode for created file
	- buffer holding the name of a directory to make the file in

	Returns:
	>=0	GOOD (and length in bytes of the result)
	<0	BAD (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstdlib>
#include	<cstring>
#include	<ctime>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<getnodename.h>
#include	<base64.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"mkfile.h"


/* local defines */

#define	PREFIXLEN	4		/* file-name prefix length */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int mkform(char *,cchar *,mode_t) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int mkjobfile(char *rbuf,cchar *dname,mode_t fm) noex {
	int		rs = SR_FAULT ;
	int		rl = 0 ;
	if (rbuf && dname) {
	    rs = SR_INVALID ;
	    if (dname[0]) {
		USTAT	sb ;
	        if ((rs = u_stat(dname,&sb)) >= 0) {
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
/* end subroutine (mkjobfile) */


/* local subroutines */

static int mkform(char *rbuf,cchar *dname,mode_t fm) noex {
	int		rs ;
	int		rs1 ;
	int		rl = 0 ;
	char		*nbuf{} ;
	if ((rs = malloc_nn(&nbuf)) >= 0) {
	    int		nlen = rs ;
	    if ((rs = getnodename(nbuf,nlen)) >= 0) {
		cint	pl = PREFIXLEN ;
		if (rs > pl) nbuf[pl] = '\0' ;
		rs = mkartfile(rbuf,dname,nbuf,0,fm) ;
		rl = rs ;
	    } /* end if (getnodename) */
	    rs1 = uc_free(nbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (getnodename) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (mkform) */


