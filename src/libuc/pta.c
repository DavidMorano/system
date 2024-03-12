/* pta */
/* lang=C20 */

/* POSIX® Thread Attribute manipulation */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This object provides the initialization add-on for a PTM
	object.  Enjoy!

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<pthread.h>
#include	<usystem.h>
#include	<localmisc.h>

#include	"pta.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* forward references */


/* exported subroutines */

int pta_create(pta *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    int		to_nomem = utimeout[uto_nomem] ;
	    int		to_again = utimeout[uto_again] ;
	    bool	f_exit = false ;
	    repeat {
	        rs = pthread_attr_init(op) ;
	        if (rs > 0) rs = (- rs) ;
	        if (rs < 0) {
	            switch (rs) {
	            case SR_NOMEM:
	                if (to_nomem-- > 0) {
		            msleep(1000) ;
		        } else {
	                    f_exit = true ;
		        }
	                break ;
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
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (pta_create) */

int pta_destroy(pta *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    (void) pthread_attr_destroy(op) ;
	}
	return rs ;
}
/* end subroutine (pta_destroy) */

int pta_setstacksize(pta *op,size_t v) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = pthread_attr_setstacksize(op,v) ;
	    if (rs > 0) rs = (- rs) ;
	}
	return rs ;
}
/* end subroutine (pta_setstacksize) */

int pta_getstacksize(pta *op,size_t *vp) noex {
	int		rs = SR_FAULT ;
	if (op && vp) {
	    rs = pthread_attr_getstacksize(op,vp) ;
	    if (rs > 0) rs = (- rs) ;
	}
	return rs ;
}
/* end subroutine (pta_getstacksize) */

int pta_setguardsize(pta *op,size_t v) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = pthread_attr_setguardsize(op,v) ;
	    if (rs > 0) rs = (- rs) ;
	}
	return rs ;
}
/* end subroutine (pta_setguardsize) */

int pta_getguardsize(pta *op,size_t *vp) noex {
	int		rs = SR_FAULT ;
	if (op && vp) {
	    rs = pthread_attr_getguardsize(op,vp) ;
	    if (rs > 0) rs = (- rs) ;
	}
	return rs ;
}
/* end subroutine (pta_getguardsize) */

int pta_setstackaddr(pta *op,void *vp) noex {
	int		rs = SR_FAULT ;
	if (op && vp) {
	    rs = pthread_attr_setstackaddr(op,vp) ;
	    if (rs > 0) rs = (- rs) ;
	}
	return rs ;
}
/* end subroutine (pta_setstackaddr) */

int pta_getstackaddr(pta *op,void **vpp) noex {
	int		rs = SR_FAULT ;
	if (op && vpp) {
	    rs = pthread_attr_getstackaddr(op,vpp) ;
	    if (rs > 0) rs = (- rs) ;
	}
	return rs ;
}
/* end subroutine (pta_getstackaddr) */

int pta_setdetachstate(pta *op,int v) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = pthread_attr_setdetachstate(op,v) ;
	    if (rs > 0) rs = (- rs) ;
	}
	return rs ;
}
/* end subroutine (pta_setdetachstate) */

int pta_getdetachstate(pta *op,int *vp) noex {
	int		rs = SR_FAULT ;
	if (op && vp) {
	    rs = pthread_attr_getdetachstate(op,vp) ;
	    if (rs > 0) rs = (- rs) ;
	}
	return rs ;
}
/* end subroutine (pta_getdetachstate) */

int pta_setscope(pta *op,int v) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = pthread_attr_setscope(op,v) ;
	    if (rs > 0) rs = (- rs) ;
	}
	return rs ;
}
/* end subroutine (pta_setscope) */

int pta_getscope(pta *op,int *vp) noex {
	int		rs = SR_FAULT ;
	if (op) {
	     rs = pthread_attr_getscope(op,vp) ;
	     if (rs > 0) rs = (- rs) ;
	}
	return rs ;
}
/* end subroutine (pta_getscope) */

int pta_setinheritsched(pta *op,int v) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = pthread_attr_setinheritsched(op,v) ;
	    if (rs > 0) rs = (- rs) ;
	}
	return rs ;
}
/* end subroutine (pta_setinheritsched) */

int pta_getinheritsched(pta *op,int *vp) noex {
	int		rs = SR_FAULT ;
	if (op && vp) {
	    rs = pthread_attr_getinheritsched(op,vp) ;
	    if (rs > 0) rs = (- rs) ;
	}
	return rs ;
}
/* end subroutine (pta_getinheritsched) */

int pta_setschedpolicy(pta *op,int v) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = pthread_attr_setschedpolicy(op,v) ;
	    if (rs > 0) rs = (- rs) ;
	}
	return rs ;
}
/* end subroutine (pta_setschedpolicy) */

int pta_getschedpolicy(pta *op,int *vp) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = pthread_attr_getschedpolicy(op,vp) ;
	    if (rs > 0) rs = (- rs) ;
	}
	return rs ;
}
/* end subroutine (pta_getschedpolicy) */

int pta_setschedparam(pta *op,const struct sched_param *vp) noex {
	int		rs = SR_FAULT ;
	if (op && vp) {
	    rs = pthread_attr_setschedparam(op,vp) ;
	    if (rs > 0) rs = (- rs) ;
	}
	return rs ;
}
/* end subroutine (pta_setschedparam) */

int pta_getschedparam(pta *op,struct sched_param *vp) noex {
	int		rs = SR_FAULT ;
	if (op && vp) {
	    rs = pthread_attr_getschedparam(op,vp) ;
	    if (rs > 0) rs = (- rs) ;
	}
	return rs ;
}
/* end subroutine (pta_getschedparam) */


int pta_setstack(pta *op,void *saddr,size_t ssize) noex {
	int		rs = SR_FAULT ;
	if (op && saddr) {
	    if ((rs = pta_setstackaddr(op,saddr)) >= 0) {
	        rs = pta_setstacksize(op,ssize) ;
	    }
	}
	return rs ;
}
/* end subroutine (pta_setstack) */


