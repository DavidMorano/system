/* textlook INCLUDE */
/* charset=ISO8859-1 */
/* lang=C20 */

/* text look-up manager (we use the index and verify speculative results) */
/* version %I% last-modified %G% */


/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	TEXTLOOK_INCLUDE
#define	TEXTLOOK_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>
#include	<eigendb.h>
#include	<txtindex.h>
#include	<rtags.h>


#define	TEXTLOOK_MAGIC	0x99889298
#define	TEXTLOOK	struct textlook_head
#define	TEXTLOOK_FL	struct textlook_flags
#define	TEXTLOOK_CUR	struct textlook_cursor
#define	TEXTLOOK_OBJ	struct textlook_object
#define	TEXTLOOK_TAG	struct textlook_taget
#define	TEXTLOOK_INFO	struct textlook_information
/* query options */
#define	TEXTLOOK_OPREFIX	(1 << 0)	/* prefix match */


struct textlook_information {
	time_t		ticreat ;	/* index creation-time */
	time_t		timod ;		/* index modification-time */
	uint		count ;		/* number of tags */
	uint		neigen ;
	uint		minwlen ;	/* minimum word length */
	uint		maxwlen ;	/* maximum word length */
} ;

struct textlook_taget {
	uint		recoff ;
	uint		reclen ;
} ;

struct textlook_object {
	cchar		*name ;
	uint		objsize ;
	uint		cursize ;
} ;

struct textlook_cursor {
    	char		*tbuf ;
	rtags		tags ;
	rtags_cur	tcur ;
	uint		magic ;
	int		tlen ;
	int		ntags ;
} ;

struct textlook_flags {
	uint		ind:1 ;			/* text-index */
	uint		edb:1 ;
	uint		edbinit:1 ;
	uint		prefix:1 ;		/* prefix key-matches */
} ;

struct textlook_head {
	cchar		*pr ;
	cchar		*dbname ;		/* DB database name */
	cchar		*bdname ;		/* base-directory */
	cchar		*sdn ;
	cchar		*sfn ;
	void		*disp ;
	eigendb		*edp ;			/* eigen-data-pointer */
	txtindex	*idp ;			/* index-data-pointer */
	time_t		ti_lastcheck ;
	time_t		ti_tind ;		/* text-index */
	TEXTLOOK_FL	fl ;
	uint		magic ;
	int		pagesize ;
	int		dbfsize ;		/* DB file-size */
	int		minwlen ;		/* minimum key-word length */
	int		ncursors ;
} ;

typedef	TEXTLOOK	textlook ;
typedef	TEXTLOOK_FL	textlook_fl ;
typedef	TEXTLOOK_CUR	textlook_cur ;
typedef	TEXTLOOK_OBJ	textlook_obj ;
typedef	TEXTLOOK_TAG	textlook_tag ;
typedef	TEXTLOOK_INFO	textlook_info ;

EXTERNC_begin

extern int textlook_open(textlook *,cchar *,cchar *,cchar *) noex ;
extern int textlook_count(textlook *) noex ;
extern int textlook_getinfo(textlook *,textlook_info *) noex ;
extern int textlook_getsdn(textlook *,char *,int) noex ;
extern int textlook_getsfn(textlook *,char *,int) noex ;
extern int textlook_curbegin(textlook *,textlook_cur *) noex ;
extern int textlook_curlook(textlook *,textlook_cur *,int,cchar **) noex ;
extern int textlook_curread(textlook *,textlook_cur *,textlook_tag *,
		char *,int) noex ;
extern int textlook_curend(textlook *,textlook_cur *) noex ;
extern int textlook_audit(textlook *) noex ;
extern int textlook_close(textlook *) noex ;

EXTERNC_end


#endif /* TEXTLOOK_INCLUDE */


