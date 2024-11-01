/* parts HEADER */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* AS-String (parts) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	parts

	Description:
	This is an email address handling module object.  It can
	parse out and store hierarchically organized EMAs.

*******************************************************************************/

#ifndef	PARTS_INCLUDE
#define	PARTS_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/param.h>
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* for |strlen(3c)| */
#include	<usystem.h>
#include	<mallocstuff.h>
#include	<strn.h>
#include	<char.h>
#include	<ascii.h>
#include	<localmisc.h>

#include	"asstr.hh"


namespace emaobj {
    extern int	partsbegin(asstr *) noex ;
    extern int	partslen(asstr *) noex ;
    extern int	partsend(asstr *) noex ;
}


#endif /* __cplusplus */
#endif /* PARTS_INCLUDE */


