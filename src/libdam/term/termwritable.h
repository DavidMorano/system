/* termwritable HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* determine if a terminal is writable */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	termwritable

	Description:
	See the support file for a full description (whatever there is
	of that).

	Synopsis:
	int termwritable(cchar *fname) noex

	Arguments:
	fname		file-name of terminal to check

	Returns:
	>0		yes
	==0		no
	<0		error (system-return)

*******************************************************************************/

#ifndef	TERMWRITABLE_INCLUDE
#define	TERMWRITABLE_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int termwritable(cchar *) noex ;

EXTERNC_end



#endif /* TERMWRITABLE_INCLUDE */


