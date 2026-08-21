/* hasneed HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* does the given c-string need to have some specified trailing item */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	HASNEED_INCLUDE
#define	HASNEED_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_begin

extern bool	hasneedchr	(cchar *,int,int) noex ;
extern bool	hasneedslash	(cchar *,int) noex ;

EXTERNC_end

#ifdef	__cplusplus
inline bool hasneedslash(cchar *sp) noex {
    	return hasneedslash(sp,-1) ;
} /* end subroutine */
#endif /* __cplusplus */


#endif /* HASNEED_INCLUDE */


