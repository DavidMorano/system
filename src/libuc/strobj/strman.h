/* strman HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* special string manipulations */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRMAN_INCLUDE
#define	STRMAN_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	STRMAN		struct strman_head


struct strman_head {
	cchar		*sp ;
	int		sl ;
} ;

typedef	STRMAN		strman ;

EXTERNC_begin

extern int	strman_start(strman *,cchar *,int) noex ;
extern int	strman_breakfield(strman *,cchar *,cchar **) noex ;
extern int	strman_span(strman *,cchar *) noex ;
extern int	strman_whitechr(strman *,int) noex ;	/* skip wht until */
extern int	strman_finish(strman *) noex ;

static inline int strman_whitecolon(strman *sop) noex {
	return strman_whitechr(sop,':') ;
}
static inline int strman_whitedash(strman *sop) noex {
	return strman_whitechr(sop,'-') ;
}

EXTERNC_end


#endif /* STRMAN_INCLUDE */


