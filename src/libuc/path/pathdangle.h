/* pathdangle HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* find the sub-path of a dangling soft-link */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	PATHDANGLE_INCLUDE
#define	PATHDANGLE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin
extern int pathdangle(cchar *,int) noex ;
EXTERNC_end

#ifdef	__cplusplus
local inline int pathdangle(cchar *sp) noex {
    	return pathdangle(sp,-1) ;
} /* end */
#endif /* __cplusplus */


#endif /* PATHDANGLE_INCLUDE */


