/* localmisc_logical HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* miscellaneous pre-processor definitions */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-15, David A­D­ Morano
	This code is a collection of miscellaneious pre-processor
	defintions that were eidely used on the various embedded
	code projects and also on many of the support tool projects.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	LOCALMISCLOGICAL_INCLUDE
#define	LOCALMISCLOGICAL_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */


#ifndef	LEQUIV /* should be operator » !^^ « */
#define	LEQUIV(a,b)	(((a) && (b)) || ((! (a)) && (! (b))))
#endif

#ifndef	LXOR /* should be operator » ^^ « */
#define	LXOR(a,b)	(((a) && (! (b))) || ((! (a)) && (b)))
#endif

#ifndef	LOGEQUIVA /* should be operator » !^^ « */
#define	LOGEQUIVA(a,b)	(((a) && (b)) || ((! (a)) && (! (b))))
#endif

#ifndef	LOGDIFFER /* should be operator » ^^ « */
#define	LOGDIFFER(a,b)	(! LOGEQUIVA(a,b))
#endif

#ifndef	SUBROUTINE_LEQUIV
#define	SUBROUTINE_LEQUIV

local inline bool lequiv(bool a1,bool a2) noex {
	return LEQUIV(a1,a2) ;
} /* end subroutine */

local inline bool lxor(bool a1,bool a2) noex {
	return LXOR(a1,a2) ;
} /* end subroutine */

#endif /* SUBROUTINE_LEQUIV */


#endif /* LOCALMISCLOGICAL_INCLUDE */


