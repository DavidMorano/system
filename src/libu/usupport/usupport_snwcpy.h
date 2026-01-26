/* usupport_snwcpy HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* UNIX® kernel support subroutines */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-26, David A­D­ Morano
	This was first written to give a little bit to UNIX® 
	of what we have in our own circuit-pack OSes!

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPORTSNWCPY_INCLUDE
#define	USUPPORTSNWCPY_INCLUDE
#ifdef	__cplusplus /* (C++ only) */


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


namespace libu {
    extern int snwcpy(char *,int,cchar *,int = -1) noex ;
} /* end namespace (libu) */


#endif /* __cplusplus (C++ only) */
#endif /* USUPPORTSNWCPY_INCLUDE */


