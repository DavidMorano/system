/* bibleverses HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* BIBLEVERSES implementation */
/* version %I% last-modified %G% */


/* revision history:

	- 2008-10-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	BIBLEVERSES_INCLUDE
#define	BIBLEVERSES_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<biblecite.h>		/* LIBDAM */
#include	<bvi.h>			/* LIBDAM */


#define	BIBLEVERSES		struct bibleverses_head
#define	BIBLEVERSES_FL		struct bibleverses_flags
#define	BIBLEVERSES_OBJ		struct bibleverses_object
#define	BIBLEVERSES_CITE	biblecite
#define	BIBLEVERSES_Q		biblecite
#define	BIBLEVERSES_CUR		struct bibleverses_cursor
#define	BIBLEVERSES_INFO	struct bibleverses_information
#define	BIBLEVERSES_MAGIC	0x99447245
#define	BIBLEVERSES_DBNAME	"av"


struct bibleverses_object {
	cchar		*name ;
	uint		objsz ;
	uint		cursz ;
} ; /* end struct */

struct bibleverses_information {
	time_t		dbtime ;		/* db-time */
	time_t		vitime ;		/* vi-time */
	uint		maxbook ;
	uint		maxchap ;
	uint		nverses ;
	uint		nzverses ;
} ; /* end struct */

struct bibleverses_cursor {
	bvi_cur		*vicurp ;
} ; /* end struct */

struct bibleverses_flags {
	uint		vind:1 ;		/* index is loaded */
} ; /* end struct */

struct bibleverses_head {
	cchar		*pr ;
	cchar 		*dbname ;		/* DB-name */
	cchar 		*dbfname ;		/* DB file-name */
	char		*mapdata ;		/* memory-map address */
	bvi		*bvip ;			/* verse-index pointer */
	time_t		ti_db ;			/* DB file modification */
	time_t		ti_map ;		/* DB map */
	time_t		ti_lastcheck ;		/* last check of file */
	time_t		ti_vind ;		/* verse-index */
	size_t		mapsize ;		/* map size */
	size_t		filesize ;		/* file size */
	BIBLEVERSES_FL	fl ;
	uint		magval ;
	int		nverses ;
	int		ncursors ;
} ; /* end struct (bibleverses_head) */

typedef	BIBLEVERSES		bibleverses ;
typedef	BIBLEVERSES_FL		bibleverses_fl ;
typedef	BIBLEVERSES_OBJ		bibleverses_obj ;
typedef	BIBLEVERSES_CITE	bibleverses_cite ;
typedef	BIBLEVERSES_Q		bibleverses_q ;
typedef	BIBLEVERSES_CUR		bibleverses_cur ;
typedef	BIBLEVERSES_INFO	bibleverses_info ;

EXTERNC_begin

extern int bibleverses_open(bibleverses *,cchar *,cchar *) noex ;
extern int bibleverses_count(bibleverses *) noex ;
extern int bibleverses_read(bibleverses *,char *,int,
			con bibleverses_q *) noex ;
extern int bibleverses_get(bibleverses *,bibleverses_q *,char *,int) noex ;
extern int bibleverses_curbegin(bibleverses *,bibleverses_cur *) noex ;
extern int bibleverses_curenum(bibleverses *,bibleverses_cur *,
			bibleverses_q *,char *,int) noex ;
extern int bibleverses_curend(bibleverses *,bibleverses_cur *) noex ;
extern int bibleverses_audit(bibleverses *) noex ;
extern int bibleverses_getinfo(bibleverses *,bibleverses_info *) noex ;
extern int bibleverses_chapters(bibleverses *,int,uchar *,int) noex ;
extern int bibleverses_close(bibleverses *) noex ;

EXTERNC_end

extern const bibleverses_obj	bibleverses_modinfo ;


#endif /* BIBLEVERSES_INCLUDE */


