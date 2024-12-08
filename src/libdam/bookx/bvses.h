/* bvses HEADER */
/* encoding=ISO8859-1 */
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
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>

#include	<bvshdr.h>		/* this has the file-header */


#define	BVSES_MAGIC	0x88773421
#define	BVSES_SUF	"bvs"		/* variable-index */
#define	BVSES		struct bvses_head
#define	BVSES_OBJ	struct bvses_object
#define	BVSES_V		struct bvses_verse
#define	BVSES_INFO	struct bvses_information
#define	BVSES_FMI	struct bvses_filemap


struct bvses_object {
	cchar		*name ;
	uint		objsize ;
	uint		cursize ;
} ;

struct bvses_information {
	time_t		ctime ;
	time_t		mtime ;
	uint		nzbooks ;		/* number of non-zero books */
	uint		nbooks ;
	uint		nchapters ;
	uint		nverses ;
	uint		nzverses ;
} ;

struct bvses_verse {
	uchar		b, c, v ;
} ;

struct bvses_filemap {
	char		*mapdata ;	/* file map-data */
	time_t		ti_mod ;	/* time file modication */
	time_t		ti_map ;	/* time file map */
	size_t		mapsize ;	/* file map-size */
	ushort		(*bt)[4] ;	/* mapped book table */
	uchar		*ct ;		/* mapped chapter table */
} ;

struct bvses_head {
	cchar 		*pr ;
	cchar 		*dbname ;
	cchar		*fname ;
	BVSES_FMI	fmi ;		/* file-map information */
	bvshdr		fhi ;		/* file-header information */
	time_t		ti_lastcheck ;	/* time last check of file */
	uint		magic ;
	int		ncursors ;
} ;

typedef	BVSES		bvses ;
typedef	BVSES_OBJ	bvses_obj ;
typedef	BVSES_V		bvses_v ;
typedef	BVSES_INFO	bvses_info ;
typedef	BVSES_FMI	bvses_fmi ;

EXTERNC_begin

extern int	bvses_open(bvses *,cchar *,cchar *) noex ;
extern int	bvses_count(bvses *) noex ;
extern int	bvses_info(bvses *,bvses_info *) noex ;
extern int	bvses_mkmodquery(bvses *,bvses_v *,int) noex ;
extern int	bvses_audit(bvses *) noex ;
extern int	bvses_close(bvses *) noex ;

EXTERNC_end


#endif /* BVSES_INCLUDE */


