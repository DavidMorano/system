/* gettermlines SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* retrieve some information about any LOGIN terminal lines */
/* version %I% last-modified %G% */


/* revision history:

	= 1995-05-01, David A­D­ Morano
	This code module was completely rewritten to replace any
	original garbage that was here before.

	= 1998-11-22, David A­D­ Morano
        I did some clean-up.

*/

/* Copyright © 1995,1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	gettermlines

	Description:
	We get some terminal lines here.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<csysbase.h>		/* LIBU */
#include	<vardefs.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"proginfo.hh"


/* local defines */

#ifndef	VARTERMCAP
#define	VARTERMCAP	"TERMCAP"
#endif


/* external subroutines */


/* external variables */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int gettermlines(PI *pip) noex {
	int		rs = SR_OK ;
	int		n = pip->termlines ;
	if (n <= 0) {
	    cchar	*cp ;
	    if ((cp = getenv(VARLINES)) == nullptr) {
	        if ((cp = getenv(VARTERMCAP)) != nullptr) {
	            while ((cp = strchr(cp,':')) != nullptr) {
	                cp += 1 ;
	                if (strncmp(cp,"li#",3) == 0) {
	                    n = atoi(cp + 3) ;
		        }
	            } /* end while */
	        } /* end if */
	    } else {
	        n = atoi(cp) ;
	    }
	    pip->termlines = n ;
	} /* end if (needed) */
	return (n >= 0) ? n : rs ;
}
/* end subroutine (gettermlines) */


