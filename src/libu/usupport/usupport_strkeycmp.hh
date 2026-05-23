/* usupport_strkeycmp HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* make a character out of an integer */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This was written for Rightcore Network Services (RNS).
 
*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPORTSTRKEYCMP_INCLUDE
#define	USUPPORTSTRKEYCMP_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


namespace libu {
    extern int strkeybasecmpo(cchar *,cchar *)		noex ;
    extern int strkeybasecmpr(cchar *,cchar *)		noex ;
    extern int strkeycasecmpo(cchar *,cchar *)		noex ;
    extern int strkeycasecmpr(cchar *,cchar *)		noex ;
    extern int strkeyfoldcmpo(cchar *,cchar *)		noex ;
    extern int strkeyfoldcmpr(cchar *,cchar *)		noex ;
    extern int strkeybasecmp(cchar *,cchar *)		noex ;
    extern int strkeycasecmp(cchar *,cchar *)		noex ;
    extern int strkeyfoldcmp(cchar *,cchar *)		noex ;
} /* end namespace (libu) */

namespace libu {
    inline int strkeycmp(cchar *s1,cchar *s2)		noex {
    	return libu::strkeybasecmp(s1,s2) ;
    }
} /* end namespace (libu) */

namespace libu {
    extern int strkeyxbasecmpo(cchar *,cchar *)		noex ;
    extern int strkeyxbasecmpr(cchar *,cchar *)		noex ;
    extern int strkeyxcasecmpo(cchar *,cchar *)		noex ;
    extern int strkeyxcasecmpr(cchar *,cchar *)		noex ;
    extern int strkeyxfoldcmpo(cchar *,cchar *)		noex ;
    extern int strkeyxfoldcmpr(cchar *,cchar *)		noex ;
} /* end namespace (libu) */

namespace libu {
    inline int strkeyxbasecmp(cchar *s1,cchar *s2)	noex {
	return libu::strkeyxbasecmpo(s1,s2) ;
    }
    inline int strkeyxcasecmp(cchar *s1,cchar *s2)	noex {
    	return libu::strkeyxcasecmpo(s1,s2) ;
    }
    inline int strkeyxfoldcmp(cchar *s1,cchar *s2)	noex {
    	return libu::strkeyxfoldcmpo(s1,s2) ;
    }
} /* end namespace (libu) */


#endif /* __cplusplus */
#endif /* USUPPORTSTRKEYCMP_INCLUDE */


