/* localmisc_integer HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* miscellaneous pre-processor definitions */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-15, David A­D­ Morano
	This code is a collection of miscellaneious pre-processor
	defintions that were eidely used on the various embedded
	code projects and also on many of the support tool projects.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	LOCALMISCINTEGER_INCLUDE
#define	LOCALMISCINTEGER_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */


#ifndef	BFLOOR
#define	BFLOOR(v,m)	((v) & (~ ((m) - 1)))
#endif

#ifndef	BCEIL
#define	BCEIL(v,m)	(((v) + ((m) - 1)) & (~ ((m) - 1)))
#endif

#ifndef	CEILINT
#define	CEILINT(v)	BCEIL((v),int(sizeof(int)))
#endif

#ifndef	MODP2
#define	MODP2(v,n)	((v) & ((n) - 1))
#endif


#endif /* LOCALMISCINTEGER_INCLUDE */


