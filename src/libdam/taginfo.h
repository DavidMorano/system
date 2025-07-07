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
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	TAGINFO		struct taginfo


struct taginfo {
	ulong		recoff ;
	ulong		reclen ;
	int		fnamelen ;
} ;

EXTERNC_begin

extern int	taginfo_parse(taginfo *,cchar *,int) noex ;

EXTERNC_end


#endif /* TAGINFO_INCLUDE */


