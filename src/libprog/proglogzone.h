/* proglogzone HEADER */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* log handling for Time-Zones stastics */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	PROGLOGZONE_INCLUDE
#define	PROGLOGZONE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>

#include	"proginfo.h"


EXTERNC_begin

extern int proglogzone_begin(proginfo *) noex ;
extern int proglogzone_end(proginfo *) noex ;
extern int proglogzone_update(proginfo *,cchar *,int,int,cchar *) noex ;

EXTERNC_end


#endif /* PROGLOGZONE_INCLUDE */


