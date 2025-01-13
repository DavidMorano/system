/* gettermlines SUPPORT */
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

	We get some terminal lines here.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<localmisc.h>

#include	"proginfo.hh"


/* local defines */

#ifndef	VARLINES
#define	VARLINES	"LINES"
#endif

#ifndef	VARTERMCAP
#define	VARTERMCAP	"TERMCAP"
#endif


/* external subroutines */


/* external variables */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int gettermlines(PROGINFO *pip) noex {
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


