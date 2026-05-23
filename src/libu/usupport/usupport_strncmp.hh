/* usupport_strncmp HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* find a character in a counted c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPORTSTRNCMP_INCLUDE
#define	USUPPORTSTRNCMP_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


namespace libu {
    extern int	strnbasecmp	(cchar *,cchar *,int) noex ;
    extern int	strncasecmp	(cchar *,cchar *,int) noex ;
    extern int	strnfoldcmp	(cchar *,cchar *,int) noex ;
} /* end namespace (libu) */

namespace libu {
    extern int	strnbasecmpo	(cchar *,cchar *,int) noex ;
    extern int	strnbasecmpr	(cchar *,cchar *,int) noex ;
} /* end namespace (libu) */

namespace libu {
    extern int	strncasecmpo	(cchar *,cchar *,int) noex ;
    extern int	strncasecmpr	(cchar *,cchar *,int) noex ;
} /* end namespace (libu) */

namespace libu {
    extern int	strnfoldcmpo	(cchar *,cchar *,int) noex ;
    extern int	strnfoldcmpr	(cchar *,cchar *,int) noex ;
} /* end namespace (libu) */

namespace libu {
    extern int	strnxbasecmpo	(cchar *,cchar *,int) noex ;
    extern int	strnxbasecmpr	(cchar *,cchar *,int) noex ;
} /* end namespace (libu) */

namespace libu {
    extern int	strnxcasecmpo	(cchar *,cchar *,int) noex ;
    extern int	strnxcasecmpr	(cchar *,cchar *,int) noex ;
} /* end namespace (libu) */

namespace libu {
    extern int	strnxfoldcmpo	(cchar *,cchar *,int) noex ;
    extern int	strnxfoldcmpr	(cchar *,cchar *,int) noex ;
} /* end namespace (libu) */

namespace libu {
    inline int strnxbasecmp	(cchar *s1,cchar *s2,int n) noex {
    	return libu::strnxbasecmpo(s1,s2,n) ;
    }
    inline int strnxcasecmp	(cchar *s1,cchar *s2,int n) noex {
    	return libu::strnxcasecmpo(s1,s2,n) ;
    }
    inline int strnxfoldcmp	(cchar *s1,cchar *s2,int n) noex {
    	return libu::strnxfoldcmpo(s1,s2,n) ;
    }
} /* end namespace (libu) */


#endif /* __cplusplus */
#endif /* USUPPORTSTRNCMP_INCLUDE */


