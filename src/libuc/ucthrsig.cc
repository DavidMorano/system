/* ucthrsig SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>
#include	<sys/types.h>
#include	<pthread.h>
#include	<cerrno>
#include	<csignal>
#include	<cstddef>		/* |nullptr_t| */
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* exported subroutines */

int uc_thrkill(pthread_t tid,int sn) noex {
	int		rs ;
	int		to_again = utimeout[uto_again] ;
	bool		f_exit = false ;
	repeat {
	    rs = SR_OK ;
	    if (errno_t ec ; (ec = pthread_kill(tid,sn)) > 0) {
		rs = (- ec) ;
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

int uc_thrsigqueue(pthread_t tid,int sn,const SIGVAL val) noex {
	int		rs ;
	int		to_again = utimeout[uto_again] ;
	bool		f_exit = false ;
	repeat {
	    rs = SR_OK ;
	    if (errno_t ec ; (ec = pthread_sigqueue(tid,sn,val)) > 0) {
		rs = (- ec) ;
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
/* end subroutine (uc_thrsigqueue) */


