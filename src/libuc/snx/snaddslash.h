/* snaddslash HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* append a c-string to a given counted c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

  	Name:
	snaddslash

	Description:
	Ths object is used to concatenate a slash character ('/') to
	an existing counted c-string.

	Notes:
	This subroutine should be the same as (the older)
	|storebuf_str(3uc)|.

******************************************************************************/

#ifndef	SNADDSLASH_INCLUDE
#define	SNADDSLASH_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int snaddslash(char *,int,int) noex ;

EXTERNC_end


#endif /* SNADDSLASH_INCLUDE */


