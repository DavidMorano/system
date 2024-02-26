/* isfiledesc HEADER */
/* lang=C20 */

/* is a file-descriptor associated with a terminal */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A�D� Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A�D� Morano.  All rights reserved. */

#ifndef	ISFILEDESC_INCLUDE
#define	ISFILEDESC_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>


EXTERNC_begin

extern int	isinteractive(int) noex ;
extern int	isasocket(int) noex ;

static inline int isterminal(int fd) noex {
	return isinteractive(fd) ;
}

EXTERNC_end


#endif /* ISFILEDESC_INCLUDE */


