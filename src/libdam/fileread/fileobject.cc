/* fileobject SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* determine if a named file is an object file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	fileobject

	Description:
	Given a file-name we determine if it is an object file (or
	core file).

	Synopsis:
	int fileobject(cchar *fname) noex

	Arguments:
	fname		file-path to check

	Returns:
	>0		is an object file
	==0		not an object file
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |lenstr(3c)| + |memcmp(3c)| */
#include	<usystem.h>
#include	<localmisc.h>

#include	"fileobject.h"

import libutil ;

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr char		magstr[] = "\177ELF" ;

constexpr cint		maglen = clenstr(magstr) ;


/* exported variables */


/* exported subroutines */

int fileobject(cchar *fname) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		f = false ;
	if (fname) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
		cint	of = O_RDONLY ;
		cmode	om = 0 ;
		if ((rs = uc_open(fname,of,om)) >= 0) {
	            cint	fd = rs ;
	            if (ustat sb ; (rs = uc_fstat(fd,&sb)) >= 0) {
	                if (S_ISREG(sb.st_mode)) {
		            char	magbuf[maglen + 1] ;
	                    if ((rs = uc_read(fd,magbuf,maglen)) >= maglen) {
	                        f = (memcmp(magbuf,magstr,maglen) == 0) ;
		            }
			} /* end if (regular file) */
	            } /* end if (stat) */
	            rs1 = uc_close(fd) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (open) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (fileobject) */


