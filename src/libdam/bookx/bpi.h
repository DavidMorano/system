/* bpi HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 (conformance reviewed) */

/* Bible Paragraph Index */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	BPI_INCLUDE
#define	BPI_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>

#include	<bpihdr.h>		/* this is the hash-file-header */


#define	BPI_SUF		"bpi"		/* bible-paragraph-index */
#define	BPI		struct bpi_head
#define	BPI_FMI		struct bpi_filemap
#define	BPI_Q		struct bpi_query
#define	BPI_V		struct bpi_verse
#define	BPI_OBJ		struct bpi_object
#define	BPI_CUR		struct bpi_cursor
#define	BPI_INFO	struct bpi_infomation


struct bpi_object {
	cchar		*name ;
	uint		objsize ;
	uint		cursize ;
} ;

struct bpi_information {
	time_t		ctime ;
	time_t		mtime ;
	uint		maxbook ;
	uint		maxchapter ;
	uint		count ;
	uint		nzverses ;
} ;

struct bpi_query {
	uchar		b, c, v ;
} ;

struct bpi_verse {
	uchar		nlines, b, c, v ;
} ;

struct bpi_cursor {
	int		i ;
} ;

struct bpi_filemap {
	char		*mapdata ;	/* file map */
	uint		(*vt)[1] ;	/* mapped verses table */
	time_t		ti_mod ;	/* time file modication */
	time_t		ti_map ;	/* time file map */
	size_t		mapsize ;
} ;

struct bpi_head {
	cchar 		*dbname ;
	cchar		*fname ;
	BPI_FMI		fmi ;		/* file-map information */
	bpihdr		fhi ;		/* file-header information */
	time_t		ti_lastcheck ;	/* time last check of file */
	uint		magic ;
	int		ncursors ;
} ;

typedef	BPI		bpi ;
typedef	BPI_FMI		bpi_fmi ;
typedef	BPI_Q		bpi_q ;
typedef	BPI_V		bpi_v ;
typedef	BPI_OBJ		bpi_obj ;
typedef	BPI_CUR		bpi_cur ;
typedef	BPI_INFO	bpi_info ;

EXTERNC_begin

extern int	bpi_open(bpi *,cchar *) noex ;
extern int	bpi_count(bpi *) noex ;
extern int	bpi_info(bpi *,bpi_i *) noex ;
extern int	bpi_get(bpi *,bpi_q *) noex ;
extern int	bpi_curbegin(bpi *,bpi_cir *) noex ;
extern int	bpi_enum(bpi *,bpi_cir *,bpi_v *) noex ;
extern int	bpi_curend(bpi *,bpi_cir *) noex ;
extern int	bpi_audit(bpi *) noex ;
extern int	bpi_close(bpi *) noex ;

EXTERNC_end


#endif /* BPI_INCLUDE */


