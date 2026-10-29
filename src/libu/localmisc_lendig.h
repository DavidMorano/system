/* localmisc_lendig HEADER */
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

/* common digit base (2, 8, 10, 16) buffer lengths (convenience defines) */

#ifndef	LOCALMISCLENDIG_INCLUDE
#define	LOCALMISCLENDIG_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<localmisc_minmax.h>	/* LIBU */


#ifndef	BINBUFLEN
#define	BINBUFLEN	256		/* can hold |int256_t| in binary */
#endif

#ifndef	OCTBUFLEN
#define	OCTBUFLEN	86		/* can hold |int256_t| in octal */
#endif

#ifndef	DECBUFLEN
#define	DECBUFLEN	78		/* can hold |int256_t| in decimal */
#endif

#ifndef	HEXBUFLEN
#define	HEXBUFLEN	64		/* can hold |int256_t| in hexadecimal */
#endif

#ifndef	DIGBUFLEN
#define	DIGBUFLEN	MAX(MAX(MAX(BINBUFLEN,OCTBUFLEN),DECBUFLEN),HEXBUFLEN)
#endif


#endif /* LOCALMISCLENDIG_INCLUDE */


