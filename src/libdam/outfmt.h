/* outfmt HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* output formats */
/* version %I% last-modified %G% */


/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

#ifndef	OUTFMT_INCLUDE
#define	OUTFMT_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


extern cpcchar	outfmts[] ;

enum outfmts {
	outfmt_raw,
	outfmt_fill,
	outfmt_bible,
	outfmt_overlast
} ; /* end enum (ofis) */


#endif /* OUTFMT_INCLUDE */


