/* hasend HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* does the given c-string have the specified trailing character */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	HASEND_INCLUDE
#define	HASEND_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_begin

extern bool	hasendchr	(cchar *,int,int) noex ;
extern bool	hasendslash	(cchar *,int) noex ;

EXTERNC_end

#ifdef	__cplusplus
inline bool hasendslash(cchar *sp) noex {
    	return hasendslash(sp,-1) ;
} /* end subroutine */
#endif /* __cplusplus */


#endif /* HASEND_INCLUDE */


