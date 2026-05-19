/* usupport_ustrftime HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® kernel support subroutines */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-26, David A­D­ Morano
	This was first written to give a little bit to UNIX® what
	we have in our own circuit-pack OSes!

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPORTUSTRFTIME_INCLUDE
#define	USUPPORTUSTRFTIME_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


namespace libu {
    extern int ustrftime(char *,int,cchar *,CTM *) noex ;
}


#endif /* __cplusplus */
#endif /* USUPPORTUSTRFTIME_INCLUDE */


