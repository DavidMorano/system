/* ucsigset SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* interface component for UNIX® library-3c */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cerrno>
#include	<csignal>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<localmisc.h>

#include	"ucsigset.h"

#pragma		GCC dependency		"mod/usigsets.ccm"

import usigsets ;			/* |usigset(3u)| */

/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_sigsetempty(sigset_t *sp) noex {
    	return usigset.empty(sp) ;
}
/* end subroutine (uc_sigsetempty) */

int uc_sigsetfill(sigset_t *sp) noex {
    	return usigset.fill(sp) ;
}
/* end subroutine (uc_sigsetfill) */

int uc_sigsetadd(sigset_t *sp,int sn) noex {
    	return usigset.add(sp,sn) ;
}
/* end subroutine (uc_sigsetadd) */

int uc_sigsetdel(sigset_t *sp,int sn) noex {
    	return usigset.del(sp,sn) ;
}
/* end subroutine (uc_sigsetdel) */

int uc_sigsetismem(const sigset_t *sp,int sn) noex {
    	return usigset.ismem(sp,sn) ;
}
/* end subroutine (uc_sigsetismem) */


