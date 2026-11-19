/* usupport_cmpstrn HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* c-string comparisons w/ maximum length and case variations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPORTCMPSTRN_INCLUDE
#define	USUPPORTCMPSTRN_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


namespace libu {
    extern int	cmpstrnbc	(cchar *,cchar *,int = -1) noex ;
    extern int	cmpstrnic	(cchar *,cchar *,int = -1) noex ;
    extern int	cmpstrnfc	(cchar *,cchar *,int = -1) noex ;
} /* end namespace (libu) */

namespace libu {
    extern int	cmpstrnbco	(cchar *,cchar *,int = -1) noex ;
    extern int	cmpstrnbcr	(cchar *,cchar *,int = -1) noex ;
} /* end namespace (libu) */

namespace libu {
    extern int	cmpstrnico	(cchar *,cchar *,int = -1) noex ;
    extern int	cmpstrnicr	(cchar *,cchar *,int = -1) noex ;
} /* end namespace (libu) */

namespace libu {
    extern int	cmpstrnfco	(cchar *,cchar *,int = -1) noex ;
    extern int	cmpstrnfcr	(cchar *,cchar *,int = -1) noex ;
} /* end namespace (libu) */

namespace libu {
    extern int	cmpstrnøbco	(cchar *,cchar *,int = -1) noex ;
    extern int	cmpstrnøbcr	(cchar *,cchar *,int = -1) noex ;
} /* end namespace (libu) */

namespace libu {
    extern int	cmpstrnøico	(cchar *,cchar *,int = -1) noex ;
    extern int	cmpstrnøicr	(cchar *,cchar *,int = -1) noex ;
} /* end namespace (libu) */

namespace libu {
    extern int	cmpstrnøfco	(cchar *,cchar *,int = -1) noex ;
    extern int	cmpstrnøfcr	(cchar *,cchar *,int = -1) noex ;
} /* end namespace (libu) */

namespace libu {
    inline int cmpstrnøbc	(cchar *s1,cchar *s2,int n = -1) noex {
    	return cmpstrnøbco(s1,s2,n) ;
    } /* end */
    inline int cmpstrnøic	(cchar *s1,cchar *s2,int n = -1) noex {
    	return cmpstrnøico(s1,s2,n) ;
    } /* end */
    inline int cmpstrnøfc	(cchar *s1,cchar *s2,int n = -1) noex {
    	return cmpstrnøfco(s1,s2,n) ;
    } /* end */
} /* end namespace (libu) */


#endif /* USUPPORTCMPSTRN_INCLUDE */


