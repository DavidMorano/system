/* localmisc_typealiases HEADER */
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

/*******************************************************************************

  	Description:
	These are historical type aliases for some basic scalar types.

*******************************************************************************/

#ifndef	LOCALMISCTYPEALIASES_INCLUDE
#define	LOCALMISCTYPEALIASES_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<time.h>		/* type |time_t| */


#ifndef	LONG
#define	LONG		long
#endif

#ifndef	SCHAR
#define	SCHAR		signed char
#endif

#ifndef	UCHAR
#define	UCHAR		unsigned char
#endif

#ifndef	USHORT
#define	USHORT		unsigned short
#endif

#ifndef	UINT
#define	UINT		unsigned int
#endif

#ifndef	ULONG
#define	ULONG		unsigned long
#endif


#endif /* LOCALMISCTYPEALIASES_INCLUDE */


