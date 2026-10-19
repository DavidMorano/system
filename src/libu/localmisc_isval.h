/* localmisc_isval HEADER */
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

#ifndef	LOCALMISCISVAL_INCLUDE
#define	LOCALMISCISVAL_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */


#define		isvalzero(v)	((v) == 0)
#define		isvalpos(v)	((v) > 0)
#define		isvalneg(v)	((v) < 0)

#define		isvaleq(v)	((v) == 0)
#define		isvalgt(v)	((v) > 0)
#define		isvallt(v)	((v) < 0)
#define		isvalge(v)	((v) >= 0)
#define		isvalle(v)	((v) <= 0)


#endif /* LOCALMISCISVAL_INCLUDE */


