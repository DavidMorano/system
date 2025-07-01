/* dialpass HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* dial out to a PASS port */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	DIALPASS_INCLUDE
#define	DIALPASS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>

#include	<dialopts.h>


EXTERNC_begin

extern int dialpass(cchar *,int,int) noex ;

EXTERNC_end


#endif /* DIALPASS_INCLUDE */


