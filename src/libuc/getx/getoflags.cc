/* getoflags SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get (create) the UNIX® "open" flags from an "open-string" */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
        This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************* 
	Name:
	getoflags

	Description:
	This subroutine gets (really creates) an integer containing
	the UNIX® "open" flags from a given "open-string."

	Synopsis:
	int getoflags(cchar *os) noex

	Arguments:
	os		the given "open-string"

	Returns:
	>=0		the resulitng UNIX® open-flags
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>
#include	<fcntl.h>		/* |O_{xx}| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"getoflags.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct amode {
	bool	rd ;
	bool	wr ;
    } ; /* end struct (amode) */
} /* end namespace */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int getoflags(cchar *os) noex {
	int		rs = SR_FAULT ;
	int		of = O_CLOEXEC ;
	if (os) {
	    amode	am{} ;
	    cchar	*osp = os ;
	    while (*osp) {
	        switch (cint sc = mkchar(*osp++) ; sc) {
	        case 'r':
	            am.rd = true ;
	            break ;
	        case 'w':
	            am.wr = true ;
	            break ;
	        case 'm':
	        case '+':
	            am.rd = true ;
	            am.wr = true ;
		    of |= O_APPEND ;
	            break ;
	        case 'a':
	            am.wr = true ;
	            of |= O_APPEND ;
	            break ;
	        case 'b': /* POSIX® "binary" mode -- nothing on real UNIXes® */
	            break ;
	        case 'c':
	            of |= O_CREAT ;
	            break ;
	        case 'e':
	            of |= (O_CREAT | O_EXCL) ;
	            break ;
	        case 't':
	            of |= (O_CREAT | O_TRUNC) ;
	            break ;
	        case 'n':
	            of |= O_NDELAY ;
	            break ;
	        case 'x':
	            of |= O_EXCL ;
		    break ;
	        case 'F':
		    of |= O_MINFD ;	/* minimum-file-descriptor */
		    break ;
	        case 'N':
	            of |= O_NETWORK ;	/* "network" file */
		    break ;
	        case 'M':
	            of |= O_MINMODE ;	/* minimum file-permissions-mode */
		    break ;
	        } /* end switch */
	   } /* end while (open flags) */
	   if (am.rd && am.wr) {
	       of |= O_RDWR ;
	   } else if (am.wr) {
	       of |= O_WRONLY ;
	   } else {
	       of |= O_RDONLY ;
	   }
	} /* end if (non-null) */
	return (rs >= 0) ? of : rs ;
}
/* end subroutine (getoflags) */


