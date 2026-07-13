/* usupport_vstrcmp HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* USUPPORT v-string comparison (w/ various cases) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPORTVSTRCMP_INCLUDE
#define	USUPPORTVSTRCMP_INCLUDE
#ifdef	__cplusplus	/* C++ only! */


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<vstrorders.h>		/* LIBU */


namespace libu {
   extern int	vstrbasecmp(cchar **,cchar **) noex ;
   extern int	vstrcasecmp(cchar **,cchar **) noex ;
   extern int	vstrfoldcmp(cchar **,cchar **) noex ;
} /* end namespace (libc) */

namespace libu {
   extern int	vstrbasecmpx(cchar **,cchar **,vstrorders) noex ;
   extern int	vstrcasecmpx(cchar **,cchar **,vstrorders) noex ;
   extern int	vstrfoldcmpx(cchar **,cchar **,vstrorders) noex ;
} /* end namespace (libc) */

namespace libu {
   inline int vstrcmp(cchar **s1pp,cchar **s2pp) noex {
	return libu::vstrbasecmp(s1pp,s2pp) ;
   }
   inline int vstrcmpo(cchar **s1pp,cchar **s2pp) noex {
	return libu::vstrbasecmpx(s1pp,s2pp,vstrorder_obverse) ;
   }
   inline int vstrcmpr(cchar **s1pp,cchar **s2pp) noex {
	return libu::vstrbasecmpx(s1pp,s2pp,vstrorder_reverse) ;
   }
} /* end namespace (libc) */

namespace libu {
   inline int vstrbasecmpo(cchar **s1pp,cchar **s2pp) noex {
	return libu::vstrbasecmpx(s1pp,s2pp,vstrorder_obverse) ;
   }
   inline int vstrbasecmpr(cchar **s1pp,cchar **s2pp) noex {
	return libu::vstrbasecmpx(s1pp,s2pp,vstrorder_reverse) ;
   }
} /* end namespace (libc) */

namespace libu {
   inline int vstrcasecmpo(cchar **s1pp,cchar **s2pp) noex {
	return libu::vstrcasecmpx(s1pp,s2pp,vstrorder_obverse) ;
   }
   inline int vstrcasecmpr(cchar **s1pp,cchar **s2pp) noex {
	return libu::vstrcasecmpx(s1pp,s2pp,vstrorder_reverse) ;
   }
} /* end namespace (libc) */

namespace libu {
   inline int vstrfoldcmpo(cchar **s1pp,cchar **s2pp) noex {
	return libu::vstrfoldcmpx(s1pp,s2pp,vstrorder_obverse) ;
   }
   inline int vstrfoldcmpr(cchar **s1pp,cchar **s2pp) noex {
	return libu::vstrfoldcmpx(s1pp,s2pp,vstrorder_reverse) ;
   }
} /* end namespace (libc) */


#endif /* __cplusplus (C++ only) */
#endif /* USUPPORTVSTRCMP_INCLUDE */


