/* hasprint HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* does a counted c-string have some characteristic? */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	HASPRINT_INCLUDE
#define	HASPRINT_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


EXTERNC_begin

extern bool hasprintcmd	(cchar *,int) noex ;
extern bool hasprintbad	(cchar *,int) noex ;

EXTERNC_end


#endif /* HASPRINT_INCLUDE */


