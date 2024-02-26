/* ismmclass HEADER */
/* lang=C20 */

/* character classes for mail content purposes */
/* version %I% last-modified %G% */


/* revision history:

	= 1996-03-01, David A­D­ Morano
	The subroutine set was written from scratch to do what the
	previous program by the same name did.

*/

/* Copyright © 1996 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	These subroutines check a character to see if it is part
	of a special character class. See the code for details!

*******************************************************************************/

#ifndef	ISMMCLASS_INCLUDE
#define	ISMMCLASS_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>


EXTERNC_begin

extern bool	 ismmclass_7bit(int) noex ;
extern bool	 ismmclass_8bit(int) noex ;
extern bool	 ismmclass_binary(int) noex ;

EXTERNC_end


#endif /* ISMMCLASS_INCLUDE */


