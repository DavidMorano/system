/* usupport_cmpstrkn HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* c-string comparisons w/ maximum length and case variations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPORTCMPSTRKN_INCLUDE
#define	USUPPORTCMPSTRKN_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


namespace libu {
    extern int	cmpstrknbc		(cchar *,cchar *,int) noex ;
    extern int	cmpstrknic		(cchar *,cchar *,int) noex ;
    extern int	cmpstrknfc		(cchar *,cchar *,int) noex ;
} /* end namespace (libu) */

namespace libu {
    inline int cmpstrkn		(cchar *a,cchar *sp,int sl) noex {
    	return cmpstrknbc(a,sp,sl) ;
    } /* end */
} /* end namespace (libu) */

namespace libu {
    extern int	cmpstrknbco		(cchar *,cchar *,int) noex ;
    extern int	cmpstrknbcr		(cchar *,cchar *,int) noex ;
} /* end namespace (libu) */

namespace libu {
    extern int	cmpstrknico		(cchar *,cchar *,int) noex ;
    extern int	cmpstrknicr		(cchar *,cchar *,int) noex ;
} /* end namespace (libu) */

namespace libu {
    extern int	cmpstrknfco		(cchar *,cchar *,int) noex ;
    extern int	cmpstrknfcr		(cchar *,cchar *,int) noex ;
} /* end namespace (libu) */

namespace libu {
    extern int	cmpstrknøbco	(cchar *,cchar *,int) noex ;
    extern int	cmpstrknøbcr	(cchar *,cchar *,int) noex ;
} /* end namespace (libu) */

namespace libu {
    extern int	cmpstrknøico	(cchar *,cchar *,int) noex ;
    extern int	cmpstrknøicr	(cchar *,cchar *,int) noex ;
} /* end namespace (libu) */

namespace libu {
    extern int	cmpstrknøfco	(cchar *,cchar *,int) noex ;
    extern int	cmpstrknøfcr	(cchar *,cchar *,int) noex ;
} /* end namespace (libu) */

namespace libu {
    inline int cmpstrknøbc	(cchar *s1,cchar *s2,int n) noex {
    	return cmpstrknøbco(s1,s2,n) ;
    } /* end */
    inline int cmpstrknøic	(cchar *s1,cchar *s2,int n) noex {
    	return cmpstrknøico(s1,s2,n) ;
    } /* end */
    inline int cmpstrknøfc	(cchar *s1,cchar *s2,int n) noex {
    	return cmpstrknøfco(s1,s2,n) ;
    } /* end */
} /* end namespace (libu) */


#endif /* USUPPORTCMPSTRKN_INCLUDE */


