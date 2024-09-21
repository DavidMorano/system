/* txtindexes HEADER */
/* lang=C20 */

/* read or audit a TXTINDEX database */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-10-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	TXTINDEXES_INCLUDE
#define	TXTINDEXES_INCLUDE


#include	<envstandards.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<ptm.h>

#include	"txtindexhdr.h"		/* this is the hash-file-header */


#define	TXTINDEXES_MAGIC	0x88773421
#define	TXTINDEXES		struct txtindexes_head
#define	TXTINDEXES_OBJ		struct txtindexes_object
#define	TXTINDEXES_CUR		struct txtindexes_cursor
#define	TXTINDEXES_TAG		struct txtindexes_tagitem
#define	TXTINDEXES_FI		struct txtindexes_file
#define	TXTINDEXES_MI		struct txtindexes_mile
#define	TXTINDEXES_INFO		struct txtindexes_information


struct txtindexes_object {
	cchar		*name ;
	uint		objsize ;
	uint		cursize ;
} ;

struct txtindexes_cursor {
	uint		*taglist ;
	uint		taglen ;
	uint		i ;
} ;

struct txtindexes_tagitem {
	uint		recoff ;
	uint		reclen ;
	char		fname[MAXPATHLEN + 1] ;
} ;

struct txtindexes_file {
	char		*mapdata ;
	time_t		ti_mod ;
	time_t		ti_map ;
	size_t		mapsize ;
} ;

struct txtindexes_mile {
	cchar		*sdn ;
	cchar		*sfn ;
	cchar		*estab ;	/* eigen-string table */
	uint		*table ;	/* mapped hash table */
	uint		*lists ;	/* mapped lists */
	uint		*ertab ;	/* eigen-record table */
	uint		(*eitab)[3] ;	/* eigen-index table */
} ;

/* returned information */
struct txtindexes_information {
	time_t		ctime ;		/* index creation-time */
	time_t		mtime ;		/* index modification-time */
	uint		count ;		/* number of tags */
	uint		neigen ;
	uint		minwlen ;	/* minimum word length */
	uint		maxwlen ;	/* maximum word length */
	char		sdn[MAXPATHLEN + 1] ;
	char		sfn[MAXPATHLEN + 1] ;
} ;

struct txtindexes_head {
	cchar 		*dbname ;
	ptm		*mxp ;
	TXTINDEXES_FI	hf ;
	TXTINDEXES_FI	tf ;
	TXTINDEXES_MI	mi ;
	txtindexhdr	ifi ;		/* index-file (header) information */
	time_t		ti_lastcheck ;	/* time last check of file */
	uint		magic ;
	int		ncursors ;
} ;

typedef	TXTINDEXES		txtindexes ;
typedef	TXTINDEXES_OBJ		txtindexes_obj ;
typedef	TXTINDEXES_CUR		txtindexes_cur ;
typedef	TXTINDEXES_TAG		txtindexes_tag ;
typedef	TXTINDEXES_FI		txtindexes_fi ;
typedef	TXTINDEXES_MI		txtindexes_mi ;
typedef	TXTINDEXES_INFO		txtindexes_info ;

EXTERNC_begin

extern int	txtindexes_open(txtindexes *,cchar *) noex ;
extern int	txtindexes_count(txtindexes *) noex ;
extern int	txtindexes_neigen(txtindexes *) noex ;
extern int	txtindexes_getinfo(txtindexes *,txtindexes_info *) noex ;
extern int	txtindexes_iseigen(txtindexes *,cchar *,int) noex ;
extern int	txtindexes_curbegin(txtindexes *,txtindexes_cur *) noex ;
extern int	txtindexes_lookup(txtindexes *,txtindexes_cur *,cchar **) noex ;
extern int	txtindexes_read(txtindexes *,txtindexes_cur *,
			txtindexes_tag *) noex ;
extern int	txtindexes_curend(txtindexes *,txtindexes_cur *) noex ;
extern int	txtindexes_audit(txtindexes *) noex ;
extern int	txtindexes_close(txtindexes *) noex ;

EXTERNC_end


#endif /* TXTINDEXES_INCLUDE */


