/* sntmtime HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* counted c-string operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SNTMTIME_INCLUDE
#define	SNTMTIME_INCLUDE
#ifdef	__cplusplus /* C++ only */


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<tmtime.hh>		/* LIBUC <- the money shot! */


EXTERNC_begin

extern int sntmtime(char *,int,tmtime *,cchar *) noex ;

EXTERNC_end


#endif /* __cplusplus (C++ only) */
#endif /* SNTMTIME_INCLUDE */


