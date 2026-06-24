/* vstrsort HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* a shell-sort on array of pointers to c-strings */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	VSTRSORT_INCLUDE
#define	VSTRSORT_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_begin

typedef int	(*vstrsort_f)(cchar **,cchar **) noex ;

extern void	vstrsort(cchar **,int,vstrsort_f) noex ;

EXTERNC_end


#endif /* VSTRSORT_INCLUDE */


