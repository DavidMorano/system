/* zprintf HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* a ertain kind of debugging version of PRINTF */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	ZPRINTF_INCLUDE
#define	ZPRINTF_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<stdarg.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int zprint(cchar *,cchar *,int) noex ;
extern int zprintf(cchar *,cchar *,...) noex ;
extern int zprinthexblock(cc *,cc *,int,cvoid *,int) noex ;

EXTERNC_end


#endif /* ZPRINTF_INCLUDE */


