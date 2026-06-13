/* bvses HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* access manager interface to a Bible Verse Structure DB */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-10-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	BVSES_INCLUDE
#define	BVSES_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<bvshdr.h>		/* LIBDAM file-header */


#define	BVSES		struct bvses_head
#define	BVSES_OBJ	struct bvses_object
#define	BVSES_V		struct bvses_verse
#define	BVSES_INFO	struct bvses_information
#define	BVSES_FMI	struct bvses_filemap
#define	BVSES_MAGIC	0x88773421
#define	BVSES_SUF	"bvs"		/* variable-index */


struct bvses_object {
	cchar		*name ;
	uint		objsz ;
	uint		cursz ;
} ; /* end struct */

struct bvses_information {
	time_t		ctime ;
	time_t		mtime ;
	uint		nzbooks ;		/* number of non-zero books */
	uint		nbooks ;
	uint		nchapters ;
	uint		nverses ;
	uint		nzverses ;
} ; /* end struct */

struct bvses_verse {
	uchar		b, c, v ;
} ; /* end struct */

struct bvses_filemap {
	charp		mapdata ;	/* file map-data */
	time_t		ti_mod ;	/* time file modication */
	time_t		ti_map ;	/* time file map */
	size_t		mapsize ;	/* file map-size */
	ushort		(*bt)[4] ;	/* mapped book table */
	uchar		*ct ;		/* mapped chapter table */
} ; /* end struct */

struct bvses_head {
	cchar 		*pr ;
	cchar 		*dbname ;
	cchar		*fname ;
	BVSES_FMI	*fmip ;		/* file-map information */
	bvshdr		*fhip ;		/* file-header information */
	time_t		ti_lastcheck ;	/* time last check of file */
	uint		magval ;
	int		ncursors ;
} ; /* end struct */

typedef	BVSES		bvses ;
typedef	BVSES_OBJ	bvses_obj ;
typedef	BVSES_V		bvses_v ;
typedef	BVSES_INFO	bvses_info ;
typedef	BVSES_FMI	bvses_fmi ;

EXTERNC_begin

extern int	bvses_open	(bvses *,cchar *,cchar *) noex ;
extern int	bvses_count	(bvses *) noex ;
extern int	bvses_getinfo	(bvses *,bvses_info *) noex ;
extern int	bvses_mkmodq	(bvses *,bvses_v *,int) noex ;
extern int	bvses_audit	(bvses *) noex ;
extern int	bvses_close	(bvses *) noex ;

EXTERNC_end

extern const bvses_obj		vcses_modinfo ;


#endif /* BVSES_INCLUDE */


