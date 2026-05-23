/* usuport_strnkeycmp HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* find a character in a counted c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPORTSTRNKEYCMP_INCLUDE
#define	USUPPORTSTRNKEYCMP_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


namespace libu {
    extern int	strnkeybasecmp	(cchar *,cchar *,int) noex ;
    extern int	strnkeycasecmp	(cchar *,cchar *,int) noex ;
    extern int	strnkeyfoldcmp	(cchar *,cchar *,int) noex ;
} /* end namespace (libu) */

namespace libu {
    inline int	strnkeycmp(cchar *a,cchar *sp,int sl) noex {
	return libu::strnkeybasecmp(a,sp,sl) ;
    }
} /* end namespace (libu) */

namespace libu {
    extern int	strnkeybasecmpo	(cchar *,cchar *,int) noex ;
    extern int	strnkeybasecmpr	(cchar *,cchar *,int) noex ;
} /* end namespace (libu) */

namespace libu {
    extern int	strnkeycasecmpo	(cchar *,cchar *,int) noex ;
    extern int	strnkeycasecmpr	(cchar *,cchar *,int) noex ;
} /* end namespace (libu) */

namespace libu {
    extern int	strnkeyfoldcmpo	(cchar *,cchar *,int) noex ;
    extern int	strnkeyfoldcmpr	(cchar *,cchar *,int) noex ;
} /* end namespace (libu) */

namespace libu {
    extern int	strnkeyxbasecmpo	(cchar *,cchar *,int) noex ;
    extern int	strnkeyxbasecmpr	(cchar *,cchar *,int) noex ;
} /* end namespace (libu) */

namespace libu {
    extern int	strnkeyxcasecmpo	(cchar *,cchar *,int) noex ;
    extern int	strnkeyxcasecmpr	(cchar *,cchar *,int) noex ;
} /* end namespace (libu) */

namespace libu {
    extern int	strnkeyxfoldcmpo	(cchar *,cchar *,int) noex ;
    extern int	strnkeyxfoldcmpr	(cchar *,cchar *,int) noex ;
} /* end namespace (libu) */

namespace libu {
    inline int strnkeyxbasecmp	(cchar *s1,cchar *s2,int n) noex {
    	return libu::strnkeyxbasecmpo(s1,s2,n) ;
    }
    inline int strnkeyxcasecmp	(cchar *s1,cchar *s2,int n) noex {
    	return libu::strnkeyxcasecmpo(s1,s2,n) ;
    }
    inline int strnkeyxfoldcmp	(cchar *s1,cchar *s2,int n) noex {
    	return libu::strnkeyxfoldcmpo(s1,s2,n) ;
    }
} /* end namespace (libu) */


#endif /* __cplusplus */
#endif /* USUPPORTSTRNKEYCMP_INCLUDE */


