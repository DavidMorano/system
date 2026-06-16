/* bvimk HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* make a BVI database */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-10-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	BVIMK_INCLUDE
#define	BVIMK_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<vecobj.h>		/* LIBUC */


#define	BVIMK		struct bvimk_head
#define	BVIMK_FL	struct bvimk_flags
#define	BVIMK_OBJ	struct bvimk_object
#define	BVIMK_V		struct bvimk_verse
#define	BVIMK_LINE	struct bvimk_liner
#define	BVIMK_INFO	struct bvimk_information
#define	BVIMK_MAGIC	0x88773422
#define	BVIMK_INTOPEN	(10*60)
#define	BVIMK_INTSTALE	(5*60)


struct bvimk_object {
	cchar		*name ;
	uint		objsz ;
	uint		cursz ;
} ; /* end struct */

struct bvimk_information {
	uint		maxbook ;
	uint		maxchap ;
	uint		maxvers ;
	uint		nverses ;
	uint		nzverses ;
} ; /* end struct */

struct bvimk_liner {
	uint		loff ;
	uint		llen ;
} ; /* end struct */

struct bvimk_verse {
	BVIMK_LINE	*lines ;
	uint		voff ;
	uint		vlen ;
	uchar		nlines, b, c, v ;
} ; /* end struct */

struct bvimk_flags {
	uint		notsorted:1 ;
	uint		ofcreat:1 ;
	uint		ofexcl:1 ;
	uint		inprogress:1 ;
	uint		created:1 ;
	uint		abort:1 ;
} ; /* end struct */

struct bvimk_head {
	cchar 		*dbname ;
	cchar		*idname ;
	char		*nidxfname ;
	vecobj		*vlp ;		/* verse-list-pointer */
	vecobj		*llp ;		/* line-list-pointer */
	BVIMK_FL	fl ;
	uint		pcitation ;
	uint		maxbook ;
	uint		maxchap ;
	uint		maxvers ;
	uint		nverses ;
	uint		nzverses ;
	uint		magval ;
	int		nfd ;
	mode_t		om ;
} ; /* end struct */

typedef	BVIMK		bvimk ;
typedef	BVIMK_FL	bvimk_fl ;
typedef	BVIMK_OBJ	bvimk_obj ;
typedef	BVIMK_V		bvimk_v ;
typedef	BVIMK_LINE	bvimk_line ;
typedef	BVIMK_INFO	bvimk_info ;

EXTERNC_begin

extern int	bvimk_open	(bvimk *,cchar *,int,mode_t) noex ;
extern int	bvimk_add	(bvimk *,bvimk_v *) noex ;
extern int	bvimk_abort	(bvimk *,int) noex ;
extern int	bvimk_getinfo	(bvimk *,bvimk_info *) noex ;
extern int	bvimk_close	(bvimk *) noex ;

EXTERNC_end


#endif /* BVIMK_INCLUDE */


