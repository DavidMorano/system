/* haseol HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* does the c-string contain a leading given character (after skipping WS) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	HASEOL_INCLUDE
#define	HASEOL_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_begin

extern bool	haseol(cchar *,int) noex ;

EXTERNC_end


#endif /* HASEOL_INCLUDE */


