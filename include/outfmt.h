/* outfmt HEADER */
/* lang=C++20 */

/* output formats */
/* version %I% last-modified %G% */


/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

#ifndef	OUTFMT_INCLUDE
#define	OUTFMT_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>


enum ofis {
	outfmt_raw,
	outfmt_fill,
	outfmt_bible,
	outfmt_overlast
} ;

extern const char	*outfmts[] ;


#endif /* OUTFMT_INCLUDE */


