/* sisub HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* find a substring in a c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SISUB_INCLUDE
#define	SISUB_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int sibasesub(cchar *sp,int sl,cchar *ss) noex ;
extern int sicasesub(cchar *sp,int sl,cchar *ss) noex ;
extern int sifoldsub(cchar *sp,int sl,cchar *ss) noex ;

EXTERNC_end


#endif /* SISUB_INCLUDE */


