/* bvi HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */


/* revision history:

	= 2008-10-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	BVI_INCLUDE
#define	BVI_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>

#include	<bvihdr.h>		/* this is the hash-file-header */


#define	BVI_MAGIC	0x88773421
#define	BVI_SUF		"bvi"		/* variable-index */
#define	BVI		struct bvi_head
#define	BVI_OBJ		struct bvi_object
#define	BVI_CUR		struct bvi_cursor
#define	BVI_V		struct bvi_verse
#define	BVI_Q		struct bvi_query
#define	BVI_LINE	struct bvi_liner
#define	BVI_INFO	struct bvi_information
#define	BVI_FMI		struct bvi_filemap


struct bvi_object {
	cchar		*name ;
	uint		objsize ;
	uint		cursize ;
} ;

struct bvi_information {
	time_t		ctime ;
	time_t		mtime ;
	uint		maxbook ;
	uint		maxchapter ;
	uint		count ;
	uint		nzverses ;
} ;

struct bvi_query {
	uchar		b, c, v ;
} ;

struct bvi_liner {
	uint		loff ;
	uint		llen ;
} ;

struct bvi_verse {
	BVI_L		bvi_l	*lines ;
	uint		voff ;
	uint		vlen ;
	uchar		nlines, b, c, v ;
} ;

struct bvi_c {
	int		i ;
} ;

struct bvi_filemap {
	char		*mapdata ;	/* file map */
	time_t		ti_mod ;	/* time file modication */
	time_t		ti_map ;	/* time file map */
	size_t		mapsize ;
	uint		(*vt)[4] ;	/* mapped verses table */
	uint		(*lt)[2] ;	/* mapped lines table */
} ;

struct bvi_head {
	cchar 		*dbname ;
	cchar		*fname ;
	BVI_FMI		fmi ;		/* file-map information */
	bvihdr		fhi ;		/* file-header information */
	time_t		ti_lastcheck ;	/* time last check of file */
	uint		magic ;
	int		ncursors ;
} ;

typedef	BVI		bvi ;
typedef	BVI_OBJ		bvi_obj ;
typedef	BVI_CUR		bvi_cur ;
typedef	BVI_V		bvi_v ;
typedef	BVI_Q		bvi_q ;
typedef	BVI_LINE	bvi_line ;
typedef	BVI_INFO	bvi_info ;
typedef	BVI_FMI		bvi_fmi ;

EXTERNC_begin

extern int	bvi_open(bvi *,cchar *) noex ;
extern int	bvi_count(bvi *) noex ;
extern int	bvi_info(bvi *,bvi_info *) noex ;
extern int	bvi_read(bvi *,bvi_v *,char *,int,bvi_q *) noex ;
extern int	bvi_get(bvi *,bvi_q *,bvi_v *,char *,int) noex ;
extern int	bvi_curbegin(bvi *,bvi_cur *) noex ;
extern int	bvi_enum(bvi *,bvi_cur *,bvi_v *,char *,int) noex ;
extern int	bvi_curend(bvi *,bvi_cur *) noex ;
extern int	bvi_audit(bvi *) noex ;
extern int	bvi_chapters(bvi *,int,uchar *,int) noex ;
extern int	bvi_close(bvi *) noex ;

EXTERNC_end

#endif /* BVI_INCLUDE */


