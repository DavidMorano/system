/* u_fcntl SUPPORT */
/* lang=C++20 */

/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services
	(RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<errno.h>
#include	<stdarg.h>
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* forward refeferences */

static int	ufcntl(int,int,caddr_t) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int u_fcntl(int fd,int cmd,...) noex {
	int		rs = SR_BADF ;
	if (fd >= 0) {
	    rs = SR_INVALID ;
	    if (cmd >= 0) {
	        va_list		ap ;
	        caddr_t		any ;
	        va_begin(ap,cmd) ;
		{
	            any = va_arg(ap,caddr_t) ;
		    rs = ufcntl(fd,cmd,any) ;
		}
	        va_end(ap) ;
	    } /* end if (plausible-command) */
	} /* end if (good-FD) */
	return rs ;
}
/* end subroutine (u_fcntl) */


/* local subroutines */

static int ufcntl(int fd,int cmd,caddr_t any) noex {
	int		rs ;
	int		to_intr = utimeout[uto_intr] ;
	int		f_exit = false ;
	repeat {
	    if ((rs = fcntl(fd,cmd,any)) == -1) rs = (- errno) ;
	    if (rs < 0) {
	        switch (rs) {
	        case SR_INTR:
	            if (to_intr-- > 0) {
	                msleep(1000) ;
		    } else {
			f_exit = true ;
		    }
	            break ;
		default:
		    f_exit = true ;
		    break ;
	        } /* end switch */
	    } /* end if (error) */
	} until ((rs >= 0) || f_exit) ;
	if (rs == SR_ACCESS) {
	    switch (cmd) {
	    case F_GETLK:
	    case F_SETLK:
	    case F_SETLKW:
	        rs = SR_AGAIN ;
		break ;
	    } /* end switch */
	} /* end if (old mess-up) */
	return rs ;
}
/* end subroutine (ufcntl) */


