/* HEADER keytracker */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* object to track used keys */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This module was changed to serve in the REFERM program.

	= 2005-10-01, David A­D­ Morano
	This was changed to work in the MMCITE program. The old
	REFERM program is really obsolete. It used a database lookup
	strategy to remote databases.  The high-level problem is:
	what to do if the cited BIB entry isn't found?  How does a
	maintainer of the present (local) document know what that
	BIB entry was?  The new strategy (implemented by the MMCITE
	program) is more like what is done with BibTeX in the TeX
	(or LaTeX) world. All BIB databases are really expected to
	be maintained by the document creator -- not some centralized
	entiry. The older centralized model reflected more the use
	in the corporate world (where different people create BIB
	entries) than in the more "modern" personal-responsibility
	type of world! :-)  Anyway, this is the way the gods seem
	to now want to do things. Deal with it!

*/

/* Copyright © 1998,2005 David A­D­ Morano.  All rights reserved. */

#ifndef	KEYTRACKER_INCLUDE
#define	KEYTRACKER_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<bits.h>		/* LIBUC */


#define	KEYTRACKER		struct keytracker_head

struct keytracker_head {
	bits		*bap ;
	cchar		*(*keyvals)[2] ;
	int		n ;
} ; /* end (struct keytracker_head) */

typedef KEYTRACKER	keytracker ;

EXTERNC_begin

extern int	keytracker_start	(keytracker *,cchar *(*)[2]) noex ;
extern int	keytracker_done		(keytracker *,int) noex ;
extern int	keytracker_more		(keytracker *,cchar *) noex ;
extern int	keytracker_finish	(keytracker *) noex ;

EXTERNC_end


#endif /* KEYTRACKER_INCLUDE */



