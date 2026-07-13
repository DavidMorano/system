/* usupport_vstrkeycmp HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* v-string key-comparison functions */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPORTVSTRKEYCMP_INCLUDE
#define	USUPPORTVSTRKEYCMP_INCLUDE
#ifdef	__cplusplus	/* C++ only! */


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<vstrorders.h>		/* LIBU */


namespace libu {
    extern int	vstrkeybasecmp(cchar **,cchar **) noex ;
    extern int	vstrkeycasecmp(cchar **,cchar **) noex ;
    extern int	vstrkeyfoldcmp(cchar **,cchar **) noex ;
} /* end nanespace (libu) */

namespace libu {
    extern int	vstrkeybasecmpx(cchar **,cchar **,vstrorders) noex ;
    extern int	vstrkeycasecmpx(cchar **,cchar **,vstrorders) noex ;
    extern int	vstrkeyfoldcmpx(cchar **,cchar **,vstrorders) noex ;
} /* end nanespace (libu) */

namespace libu {
    inline int vstrkeycmp(cchar **s1pp,cchar **s2pp) noex {
	return libu::vstrkeybasecmp(s1pp,s2pp) ;
    }
    inline int vstrkeycmpo(cchar **s1pp,cchar **s2pp) noex {
	return libu::vstrkeybasecmpx(s1pp,s2pp,vstrorder_obverse) ;
    }
    inline int vstrkeycmpr(cchar **s1pp,cchar **s2pp) noex {
	return libu::vstrkeybasecmpx(s1pp,s2pp,vstrorder_reverse) ;
    }
} /* end nanespace (libu) */

namespace libu {
    inline int vstrkeybasecmpo(cchar **s1pp,cchar **s2pp) noex {
	return libu::vstrkeybasecmpx(s1pp,s2pp,vstrorder_obverse) ;
    }
    inline int vstrkeybasecmpr(cchar **s1pp,cchar **s2pp) noex {
	return libu::vstrkeybasecmpx(s1pp,s2pp,vstrorder_reverse) ;
    }
} /* end nanespace (libu) */

namespace libu {
    inline int vstrkeycasecmpo(cchar **s1pp,cchar **s2pp) noex {
	return libu::vstrkeycasecmpx(s1pp,s2pp,vstrorder_obverse) ;
    }
    inline int vstrkeycasecmpr(cchar **s1pp,cchar **s2pp) noex {
	return libu::vstrkeycasecmpx(s1pp,s2pp,vstrorder_reverse) ;
    }
} /* end nanespace (libu) */

namespace libu {
    inline int vstrkeyfoldcmpo(cchar **s1pp,cchar **s2pp) noex {
	return libu::vstrkeyfoldcmpx(s1pp,s2pp,vstrorder_obverse) ;
    }
    inline int vstrkeyfoldcmpr(cchar **s1pp,cchar **s2pp) noex {
	return libu::vstrkeyfoldcmpx(s1pp,s2pp,vstrorder_reverse) ;
    }
} /* end nanespace (libu) */


#endif /* __cplusplus (C++ only) */
#endif /* USUPPORTVSTRKEYCMP_INCLUDE */


