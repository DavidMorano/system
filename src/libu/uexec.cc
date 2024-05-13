/* uexec SUPPORT */
/* lang=C++20 */

/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services
	(RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Coding note:
	The Solaris® (from Sun Microsystems) OS is messed up if
	|execvp(2)| is called with a rooted program file.  Why is
	it hosed? Who can fathom the screw-up known as Solaris?  So
	we check if the program file is rooted ourselves and if it
	is, we change and call |execv(2)| instead.  Did I mention
	that Solaris is a messed up OS?

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<cerrno>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<localmisc.h>

#include	"uexec.h"


/* local defines */


/* external subroutines */


/* exported subroutines */

int u_execve(cchar *p,mainv argv,mainv envv) noex {
	int		rs ;
	int		to_again = utimeout[uto_again] ;
	int		to_nomem = utimeout[uto_nomem] ;
	int		to_txtbusy = utimeout[uto_txtbusy] ;
	bool		f_exit = false ;
	char		*const *eav = (char *const *) argv ;
	char		*const *eev = (char *const *) envv ;
	repeat {
	    if ((rs = execve(p,eav,eev)) < 0) rs = (- errno) ;
	    if (rs < 0) {
	        switch (rs) {
	        case SR_AGAIN:
	            if (to_again-- > 0) {
	                msleep(1000) ;
		    } else {
			f_exit = true ;
		    }
		    break ;
	        case SR_NOMEM:
	            if (to_nomem -- > 0) {
	                msleep(1000) ;
		    } else {
			f_exit = true ;
		    }
		    break ;
	        case SR_TXTBUSY:
	            if (to_txtbusy -- > 0) {
	                msleep(1000) ;
		    } else {
			f_exit = true ;
		    }
		    break ;
	        case SR_INTR:
	            break ;
		default:
		    f_exit = true ;
		    break ;
	        } /* end switch */
	    } /* end if (error) */
	} until ((rs >= 0) || f_exit) ;
	return rs ;
}
/* end subroutine (u_execve) */

int u_execv(cchar *p,mainv argv) noex {
	char		*const	*eav = (char *const *) argv ;
	int		rs ;
	int		to_again = utimeout[uto_again] ;
	int		to_nomem = utimeout[uto_nomem] ;
	int		to_txtbusy = utimeout[uto_txtbusy] ;
	bool		f_exit = false ;
	repeat {
	    if ((rs = execv(p,eav)) < 0) rs = (- errno) ;
	    if (rs < 0) {
	        switch (rs) {
	        case SR_AGAIN:
	            if (to_again-- > 0) {
	                msleep(1000) ;
		    } else {
			f_exit = true ;
		    }
		    break ;
	        case SR_NOMEM:
	            if (to_nomem -- > 0) {
	                msleep(1000) ;
		    } else {
			f_exit = true ;
		    }
		    break ;
	        case SR_TXTBUSY:
	            if (to_txtbusy -- > 0) {
	                msleep(1000) ;
		    } else {
			f_exit = true ;
		    }
		    break ;
	        case SR_INTR:
	            break ;
		default:
		    f_exit = true ;
		    break ;
	        } /* end switch */
	    } /* end if (error) */
	} until ((rs >= 0) || f_exit) ;
	return rs ;
}
/* end subroutine (u_execv) */

int u_execvp(cchar *p,mainv argv) noex {
	char		*const	*eav = (char *const *) argv ;
	int		rs ;
	int		to_again = utimeout[uto_again] ;
	int		to_nomem = utimeout[uto_nomem] ;
	int		to_txtbusy = utimeout[uto_txtbusy] ;
	bool		f_exit = false ;
	repeat {
	    if (p[0] != '/') {
	        if ((rs = execvp(p,eav)) < 0) rs = (- errno) ;
	    } else {
	        if ((rs = execv(p,eav)) < 0) rs = (- errno) ;
	    }
	    if (rs < 0) {
	        switch (rs) {
	        case SR_AGAIN:
	            if (to_again-- > 0) {
	                msleep(1000) ;
		    } else {
			f_exit = true ;
		    }
		    break ;
	        case SR_NOMEM:
	            if (to_nomem -- > 0) {
	                msleep(1000) ;
		    } else {
			f_exit = true ;
		    }
		    break ;
	        case SR_TXTBUSY:
	            if (to_txtbusy -- > 0) {
	                msleep(1000) ;
		    } else {
			f_exit = true ;
		    }
		    break ;
	        case SR_INTR:
	            break ;
		default:
		    f_exit = true ;
		    break ;
	        } /* end switch */
	    } /* end if (error) */
	} until ((rs >= 0) || f_exit) ;
	return rs ;
}
/* end subroutine (u_execvp) */


