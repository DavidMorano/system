/* localmisc_rets HEADER */
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

#ifndef	LOCALMISCRETS_INCLUDE
#define	LOCALMISCRETS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */


/****
Note: These (|OK| and |BAD| below) are the same as the UNIX® basic
return values; namely, '0' and '-1'.  But thes have been used for
user-oriented code rather than system-oriented code.
****/

#ifndef	OK
#define	OK		0
#endif

#ifndef	BAD
#define	BAD		-1
#endif


#endif /* LOCALMISCRETS_INCLUDE */


