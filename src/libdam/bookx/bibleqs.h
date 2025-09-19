/* bibleqs HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */


/* revision history:

	- 2008-10-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	BIBLEQS_INCLUDE
#define	BIBLEQS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<eigendb.h>

#include	<txtindex.h>


#define	BIBLEQS_MAGIC	0x99889298
#define	BIBLEQS		struct bibleqs_head
#define	BIBLEQS_CUR	struct bibleqs_cursor
#define	BIBLEQS_OBJ	struct bibleqs_object
#define	BIBLEQS_Q	struct bibleqs_query
#define	BIBLEQS_CITE	struct bibleqs_query
#define	BIBLEQS_FL	struct bibleqs_flags
/* query options */
#define	BIBLEQS_OPREFIX	0x01		/* prefix match */


struct bibleqs_query {
	uchar		b, c, v ;
} ;

struct bibleqs_object {
	cchar		*name ;
	uint		objsize ;
	uint		cursize ;
} ;

struct bibleqs_cursor {
	uint		*verses ;		/* file-offsets to tags */
	uint		nverses ;
	int		i ;
} ;

struct bibleqs_flags {
	uint		ind:1 ;			/* text-index (opened) */
	uint		edb:1 ;
	uint		edbinit:1 ;
	uint		prefix:1 ;		/* prefix key-matches */
} ;

struct bibleqs_head {
	cchar		*pr ;
	cchar		*dbname ;		/* DB database name */
	cchar		*dbfname ;		/* DB filename */
	char		*dbmdata ;		/* DB map-data */
	eigendb		edb ;
	txtindex	ind ;
	time_t		ti_db ;			/* DB file modification */
	time_t		ti_map ;		/* map */
	time_t		ti_lastcheck ;
	time_t		ti_tind ;		/* text-index */
	size_t		dbmsize ;		/* DB map-size */
	BIBLEQS_FL	fl ;
	uint		magic ;
	int		minwlen ;		/* minimum key-word length */
	int		ncursors ;
	uchar		wterms[32] ;
} ;

typedef	BIBLEQS		bibleqs ;
typedef	BIBLEQS_FL	bibleqs_fl ;
typedef	BIBLEQS_CUR	bibleqs_cur ;
typedef	BIBLEQS_OBJ	bibleqs_obj ;
typedef	BIBLEQS_Q	bibleqs_q ;
typedef	BIBLEQS_CITE	bibleqs_cite ;

EXTERNC_begin

extern int bibleqs_open(bibleqs *,cchar	 *,cchar *) noex ;
extern int bibleqs_count(bibleqs *) noex ;
extern int bibleqs_audit(bibleqs *) noex ;
extern int bibleqs_curbegin(bibleqs *,bibleqs_cur *) noex ;
extern int bibleqs_lookup(bibleqs *,bibleqs_cur *,int,cchar **) noex ;
extern int bibleqs_read(bibleqs *,bibleqs_cur *,bibleqs_q *,char *,int) noex ;
extern int bibleqs_curend(bibleqs *,bibleqs_cur *) noex ;
extern int bibleqs_close(bibleqs *) noex ;

EXTERNC_end


#endif /* BIBLEQS_INCLUDE */


