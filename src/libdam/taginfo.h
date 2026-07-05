/* taginfo HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* parse a tag (given in a string) */
/* version %I% last-modified %G% */


/* revision history:

	= 1994-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

#ifndef	TAGINFO_INCLUDE
#define	TAGINFO_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */


#define	TAGINFO		struct taginfo_head


struct taginfo_head {
	ulong		recoff ;
	ulong		reclen ;
	int		fnamelen ;
} ; /* end if (taginfo) */

typedef	TAGINFO		taginfo ;

EXTERNC_begin

extern int	taginfo_parse(taginfo *,cchar *,int) noex ;

EXTERNC_end


#endif /* TAGINFO_INCLUDE */


