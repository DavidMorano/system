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
	Flags:
	r	read
	w	write
	a	append (write-only)
	m	append (read-write)
	+	append (read-write)
	c	create
	e	exclusive-create
	x	exclusive
	t	truncate
	n	n-delay
	F	MINFD
	N	NETWORD
	M	MINMODE
	C	CLOEXEC

	Synopsis:
	int getoflags(cchar *os) noex

	Arguments:
	os		the given "open-string"

	Returns:
	>=0		the resulitng UNIX® open-flags
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>		/* POSIX */
#include	<fcntl.h>		/* POSIX |O_{xx}| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

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
	int		rs = SR_OK ;
	int		of = O_CLOEXEC ;
	if (os) ylikely {
	    amode	am{} ;
	    if (os[0]) ylikely {
	        for (cchar *osp = os ; *osp ; ) {
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
	            case 'b': /* POSIX® "binary" mode -- NOOP UNIXes® */
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
	            case 'C':
		        of |= O_CLOEXEC ;	/* close-on-exec */
		        break ;
	            case 'F':
		        of |= O_MINFD ;	/* minimum-file-descriptor */
		        break ;
	            case 'N':
	                of |= O_NETWORK ;	/* "network" file */
		        break ;
	            case 'M':
	                of |= O_MINMODE ;	/* minimum permissions-mode */
		        break ;
	            } /* end switch */
	        } /* end for (open flags) */
	    } /* end if (non-empty) */
	    if (am.rd && am.wr) {
	        of |= O_RDWR ;
	    } else if (am.wr) {
	        of |= O_WRONLY ;
	    } else {
	        of |= O_RDONLY ;
	    }
	} else {
	    of |= O_RDONLY ;
	} /* end if (non-null) */
	return (rs >= 0) ? of : rs ;
} /* end subroutine (getoflags) */


