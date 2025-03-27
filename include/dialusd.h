/* dialusd HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* dial out to a TCP port */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	DIALUSD_INCLUDE
#define	DIALUSD_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>

#include	<dialopts.h>


EXTERNC_begin

extern int dialusd(cchar *,int,int) noex ;

EXTERNC_end


#endif /* DIALUSD_INCLUDE */


