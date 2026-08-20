/* calworder HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* word management - CALWORDER */
/* CALYEAR object loader */
/* version %I% last-modified %G% */

/* revision history:

	= 1999-05-19, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

#ifndef	CALWORDER_INCLUDE
#define	CALWORDER_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<calent.h>		/* LIBDAM */


#define	CALWORDER	struct calworder_head


struct calworder_head {
	calent_line	*lines ;
	cchar		*md ;
	cchar		*sp ;
	int		sl ;
	int		i ;
	int		nlines ;
} ; /* end struct (calworder_head) */

typedef	CALWORDER	calworder ;

EXTERNC_begin

extern int	calworder_start(calworder *,cchar *,calent *) noex ;
extern int	calworder_finish(calworder *) noex ;
extern int	calworder_get(calworder *,cchar **) noex ;

EXTERNC_end


#endif /* CALWORDER_INCLUDE */


