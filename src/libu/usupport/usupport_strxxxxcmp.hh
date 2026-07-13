/* usupport_strxxxxcmp HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* USUPPORT compare c-strings w/ various case insensitivities */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-23, David A-D- Morano
	This was written for Rightcore Network Services (RNS).
 
*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPORTSTRXXXXCMP_INCLUDE
#define	USUPPORTSTRXXXXCMP_INCLUDE
#ifdef	__cplusplus	/* C++ only! */


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


namespace libu {
    extern int	strbasecmp(cchar *,cchar *) noex ;
    extern int	strfoldcmp(cchar *,cchar *) noex ;
} /* end namespace (libu) */

namespace libu {
    extern int	strbasecmpo(cchar *,cchar *) noex ;
    extern int	strbasecmpr(cchar *,cchar *) noex ;
} /* end namespace (libu) */

namespace libu {
    extern int	strcasecmpo(cchar *,cchar *) noex ;
    extern int	strcasecmpr(cchar *,cchar *) noex ;
} /* end namespace (libu) */

namespace libu {
    extern int	strfoldcmpo(cchar *,cchar *) noex ;
    extern int	strfoldcmpr(cchar *,cchar *) noex ;
} /* end namespace (libu) */

namespace libu {
    extern int	strxbasecmpo(cchar *,cchar *) noex ;
    extern int	strxbasecmpr(cchar *,cchar *) noex ;
} /* end namespace (libu) */

namespace libu {
    extern int	strxcasecmpo(cchar *,cchar *) noex ;
    extern int	strxcasecmpr(cchar *,cchar *) noex ;
} /* end namespace (libu) */

namespace libu {
    extern int	strxfoldcmpo(cchar *,cchar *) noex ;
    extern int	strxfoldcmpr(cchar *,cchar *) noex ;
} /* end namespace (libu) */

namespace libu {
    inline int strxbasecmp(cchar *s1,cchar *s2) noex {
    	return libu::strxbasecmpo(s1,s2) ;
    }
    local inline int strxcasecmp(cchar *s1,cchar *s2) noex {
    	return libu::strxcasecmpo(s1,s2) ;
    }
    local inline int strxfoldcmp(cchar *s1,cchar *s2) noex {
    	return libu::strxfoldcmpo(s1,s2) ;
    }
} /* end namespace (libu) */


#endif /* __cplusplus (C++ only) */
#endif /* USUPPORTSTRXXXXCMP_INCLUDE */


