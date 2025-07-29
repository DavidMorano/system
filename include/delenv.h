/* delenv HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* delete an environment variable */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	DELENV_INCLUDE
#define	DELENV_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


EXTERNC_begin

extern cchar	*delenv(cchar *) noex ;

EXTERNC_end


#endif /* DELENV_INCLUDE */


