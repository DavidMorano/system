/* sinon HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* find a character or white-space */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SINON_INCLUDE
#define	SINON_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int sinonoct(cchar *,int) noex ;
extern int sinondec(cchar *,int) noex ;
extern int sinonhex(cchar *,int) noex ;
extern int sinonwht(cchar *,int) noex ;

EXTERNC_end


#endif /* SINON_INCLUDE */


