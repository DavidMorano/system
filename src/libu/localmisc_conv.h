/* localmisc_conv HEADER */
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

#ifndef	LOCALMISCCONV_INCLUDE
#define	LOCALMISCCONV_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */


#ifndef	MKCHAR
#define	MKCHAR(ch)	((ch) & 0xff)
#endif

#ifndef	MKBOOL
#define	MKBOOL(exp)	((exp) != 0)
#endif

#ifndef	UC
#define	UC(ch)		((unsigned char) (ch))
#endif


#endif /* LOCALMISCCONV_INCLUDE */


