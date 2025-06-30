/* uckvamatch SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX� library-3c */
/* system data-abse key-value handling */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	uc_kvamatch

	Description:
	This subroutine finds a keyname (if it exists) in a KVA-type
	of object.

	Synopsis:
	int uc_kvamatch(kva_t *kva,cchar *keyname,cchar **rpp) noex

	Arguments:
	kva		key-value handle
	keyname		key-name to lookup
	rpp		pointer to pointer to results

	Returns:
	>=0		key-name found and this is the length (including zero)
	SR_NOTFOUND	key-name not found
	SR_NOSYS	function not implemented on this system
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* <- for |strlen(3c)| */
#include	<secdb.h>		/* <- the money shot */
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */

#if	defined(SYSHAS_USERATTR) && (SYSHAS_USERATTR > 0)
#define	F_USERATTR	1
#else /* defined(SYSHAS_USERATTR) && (SYSHAS_USERATTR > 0) */
#define	F_USERATTR	0
#endif /* defined(SYSHAS_USERATTR) && (SYSHAS_USERATTR > 0) */


/* imported namespaces */

using std::nullptr_t ;			/* type */


/* local variables */

constexpr bool		f_userattr = F_USERATTR ;


/* exported subroutines */

int uc_kvamatch(kva_t *kva,cchar *keyname,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	cchar		*rp = nullptr ;
	if (kva && keyname) {
	    rs = SR_INVALID ;
	    if (keyname[0]) {
		rs = SR_OK ;
	        if_constexpr (f_userattr) {
		    const nullptr_t	np{} ;
		    char	*kp = (char *) keyname ;
	            if ((rp = kva_match(kva,kp)) != np) {
	                rs = strlen(rp) ;
	            } else {
	                rs = SR_NOTFOUND ;
	            }
	        } else {
	            rs = SR_NOSYS ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	if (rpp) *rpp = (rs >= 0) ? rp : nullptr ;
	return rs ;
}
/* end subroutine (uc_kvamatch) */


