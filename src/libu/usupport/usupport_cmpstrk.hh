/* usupport_cmpstrk HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* c-string comparisons w/ case variations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This was written for Rightcore Network Services (RNS).
 
*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPORTCMPSTRK_INCLUDE
#define	USUPPORTCMPSTRK_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<cmpstrkn.h>		/* LIBU */


namespace libu {
    extern int cmpstrkbc(cchar *,cchar *) noex ;
    extern int cmpstrkic(cchar *,cchar *) noex ;
    extern int cmpstrkfc(cchar *,cchar *) noex ;
} /* end namespace (libu) */

namespace libu {
    extern int cmpstrkbco(cchar *,cchar *) noex ;
    extern int cmpstrkbcr(cchar *,cchar *) noex ;
} /* end namespace (libu) */

namespace libu {
    extern int cmpstrkico(cchar *,cchar *) noex ;
    extern int cmpstrkicr(cchar *,cchar *) noex ;
} /* end namespace (libu) */

namespace libu {
    extern int cmpstrkfco(cchar *,cchar *) noex ;
    extern int cmpstrkfcr(cchar *,cchar *) noex ;
} /* end namespace (libu) */

namespace libu {
    local inline int cmpstrk(cchar *s1,cchar *s2) noex {
    	return cmpstrkbc(s1,s2) ;
    } /* end */
} /* end namespace (libu) */

namespace libu {
    extern int cmpstrkøbco(cchar *,cchar *) noex ;
    extern int cmpstrkøbcr(cchar *,cchar *) noex ;
} /* end namespace (libu) */

namespace libu {
    extern int cmpstrkøico(cchar *,cchar *) noex ;
    extern int cmpstrkøicr(cchar *,cchar *) noex ;
} /* end namespace (libu) */

namespace libu {
    extern int cmpstrkøfco(cchar *,cchar *) noex ;
    extern int cmpstrkøfcr(cchar *,cchar *) noex ;
} /* end namespace (libu) */

namespace libu {
    inline int cmpstrkøbc(cchar *s1,cchar *s2) noex {
    	return cmpstrkøbco(s1,s2) ;
    } /* end */
    inline int cmpstrkøic(cchar *s1,cchar *s2) noex {
    	return cmpstrkøico(s1,s2) ;
    } /* end */
    inline int cmpstrkøfc(cchar *s1,cchar *s2) noex {
    	return cmpstrkøfco(s1,s2) ;
    } /* end */
} /* end namespace (libu) */


#endif /* USUPPORTCMPSTRK_INCLUDE */


