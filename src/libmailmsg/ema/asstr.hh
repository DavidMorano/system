/* asstr HEADER */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* AS-String (asstr) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	asstr

	Description:
	This is an email address handling module object.  It can
	parse out and store hierarchically organized EMAs.

*******************************************************************************/

#ifndef	ASSTR_INCLUDE
#define	ASSTR_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/param.h>
#include	<unistd.h>
#include	<cstdlib>
#include	<cstring>		/* for |strlen(3c)| */
#include	<usystem.h>
#include	<mallocstuff.h>
#include	<strn.h>
#include	<ascii.h>
#include	<char.h>
#include	<localmisc.h>


#define	ASSTR		struct asstr_head


namespace emaobj {
    enum sis {
	si_address,
	si_route,
	si_comment,
	si_overlast
    } ;
    struct asstr_head {
	char		*sp ;
	int		sl ;
	int		e ;
    } ; /* end struct (asstr_head) */
}

namespace emaobj {
    typedef ASSTR	asstr ;
}

namespace emaobj {
    extern int	asstr_start(asstr *) noex ;
    extern int	asstr_addchr(asstr *,int) noex ;
    extern int	asstr_get(asstr *) noex ;
    extern int	asstr_getprev(asstr *) noex ;
    extern int	asstr_adv(asstr *) noex ;
    extern int	asstr_skipwhite(asstr *) noex ;
    extern int	asstr_backwhite(asstr *) noex ;
    extern int	asstr_len(asstr *) noex ;
    extern int	asstr_finish(asstr *) noex ;
}


#endif /* __cplusplus */
#endif /* ASSTR_INCLUDE */


