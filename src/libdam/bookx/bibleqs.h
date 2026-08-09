/* bibleqs HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* bible-query database manager */
/* version %I% last-modified %G% */


/* revision history:

	- 2008-10-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	BIBLEQS_INCLUDE
#define	BIBLEQS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<fieldterms.h>		/* LIBUC */
#include	<eigendb.h>		/* LIBUC */
#include	<txtindex.h>		/* LIBDAM */
#include	<biblecite.h>		/* LIBDAM */


#define	BIBLEQS		struct bibleqs_head
#define	BIBLEQS_FL	struct bibleqs_flags
#define	BIBLEQS_CUR	struct bibleqs_cursor
#define	BIBLEQS_OBJ	struct bibleqs_object
#define	BIBLEQS_Q	biblecite
#define	BIBLEQS_CITE	biblecite
#define	BIBLEQS_MAGIC	0x99889298
#define	BIBLEQS_DBDNAME	"share/bibledbs"
#define	BIBLEQS_DBNAME	"av"

enum bibleqos {
	bibleqo_prefix,
	bibleqo_overlast
} ; /* end enum (bibleqos) */

#ifdef	__cplusplus
struct bibleqms {
	static constexpr int	prefix	= (1 << bibleqo_prefix) ;
} ; /* end struct (bibleqms) */
#endif /* __cplusplus */

/* query options */
#define	BIBLEQS_OPREFIX		(1 << bibleqo_prefix)	/* prefix match */

struct bibleqs_object {
	cchar		*name ;
	uint		objsz ;
	uint		cursz ;
} ; /* end struct */

struct bibleqs_flags {
	uint		ind:1 ;			/* text-index (opened) */
	uint		edb:1 ;
	uint		edbinit:1 ;
	uint		prefix:1 ;		/* prefix key-matches */
} ; /* end struct */

struct bibleqs_cursor {
	uint		*verses ;		/* file-offsets to tags */
	int		nverses ;
	int		i ;
} ; /* end struct */

struct bibleqs_head {
	cchar		*pr ;
	cchar		*dbname ;		/* DB database name */
	cchar		*dbfname ;		/* DB filename */
	char		*dbmdata ;		/* DB map-data */
	eigendb		*edbp ;
	txtindex	*indp ;
	time_t		ti_db ;			/* DB file modification */
	time_t		ti_map ;		/* map */
	time_t		ti_lastcheck ;
	time_t		ti_tind ;		/* text-index */
	size_t		dbmsize ;		/* DB map-size */
	BIBLEQS_FL	fl ;
	uint		magval ;
	int		minwlen ;		/* minimum key-word length */
	int		ncursors ;
} ; /* end struct */

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
extern int bibleqs_curlook(bibleqs *,bibleqs_cur *,int,cchar **) noex ;
extern int bibleqs_curenum(bibleqs *,bibleqs_cur *,bibleqs_q *,
		char *,int) noex ;
extern int bibleqs_curend(bibleqs *,bibleqs_cur *) noex ;
extern int bibleqs_close(bibleqs *) noex ;

EXTERNC_end

extern const bibleqs_obj	bibleqs_modinfo ;
extern const bibleqms		bibleqm ;


#endif /* BIBLEQS_INCLUDE */


