/* endian HEADER */
/* lang=C20 */

/* machine endian management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
        This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This module provides machine endian managemnt.

*******************************************************************************/

#ifndef ENDIAN_INCLUDE
#define ENDIAN_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>


#ifndef	ENDIAN
#define	ENDIAN		endianval
#endif

#ifndef	ENDIANSTR
#define	ENDIANSTR	endianstr
#endif


extern int	endianval ;

extern cchar	*endianstr ;


#endif /* ENDIAN_INCLUDE */


