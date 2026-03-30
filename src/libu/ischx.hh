/* ischx HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* test a character for a property */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	ISCHX_INCLUDE
#define	ISCHX_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<ascii.h>


namespace libu {
    extern bool	ischalpha	(int) noex attrpure ;
    extern bool	ischalnum	(int) noex attrpure ;
    extern bool	ischwhite	(int) noex attrpure ;
    extern bool	ischlower	(int) noex attrpure ;
    extern bool	ischupper	(int) noex attrpure ;
} /* end namespace (libu) */

namespace libu {
    inline bool	ischdigit	(int ch) noex attrconst {
	return (ch >= '0') && (ch <= '9') ;
    }
    inline bool	ischdigex	(int ch) noex attrconst {
	bool f = false ;
	f = f || ((ch >= '0') && (ch <= '9')) ;
	f = f || ((ch >= 'A') && (ch <= 'F')) ;
	f = f || ((ch >= 'a') && (ch <= 'f')) ;
	return f ;
    }
    inline bool	ischblank	(int ch) noex attrconst {
	return (ch == CH_SP) || (ch == CH_TAB) ;
    }
} /* end namespace (libu) */


#endif /* __cplusplus */
#endif /* ISCHX_INCLUDE */


