/* bibleverses HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */


/* revision history:

	- 2008-10-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */


#ifndef	BIBLEVERSES_INCLUDE
#define	BIBLEVERSES_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>

#include	<bvi.h>


#define	BIBLEVERSES_MAGIC	0x99447245
#define	BIBLEVERSES		struct bibleverses_head
#define	BIBLEVERSES_FL		struct bibleverses_flags
#define	BIBLEVERSES_OBJ		struct bibleverses_object
#define	BIBLEVERSES_CITE	struct bibleverses_query
#define	bibleverses_q		struct bibleverses_query
#define	BIBLEVERSES_CUR		struct bibleverses_cursor
#define	BIBLEVERSES_INFO	struct bibleverses_information
#define	BIBLEVERSES_DBNAME	"av"


struct bibleverses_object {
	cchar		*name ;
	uint		objsize ;
	uint		cursize ;
} ;

struct bibleverses_information {
	time_t		dbtime ;		/* db-time */
	time_t		vitime ;		/* vi-time */
	uint		maxbook ;
	uint		maxchapter ;
	uint		nverses ;
	uint		nzverses ;
} ;

struct bibleverses_query {
	uchar		b, c, v ;
} ;

struct bibleverses_cursor {
	BVI_CUR		vicur ;
} ;

struct bibleverses_flags {
	uint		vind:1 ;		/* index is loaded */
} ;

struct bibleverses_head {
	cchar		*pr ;
	cchar 		*dbname ;		/* DB-name */
	cchar 		*dbfname ;		/* DB file-name */
	char		*mapdata ;		/* memory-map address */
	BIBLEVERSES_FL	fl ;
	bvi		vind ;			/* verse-index */
	time_t		ti_db ;			/* DB file modification */
	time_t		ti_map ;		/* DB map */
	time_t		ti_lastcheck ;		/* last check of file */
	time_t		ti_vind ;		/* verse-index */
	size_t		mapsize ;		/* map size */
	size_t		filesize ;		/* file size */
	uint		magic ;
	int		nverses ;
	int		ncursors ;
} ;

typedef	BIBLEVERSES		bibleverses ;
typedef	BIBLEVERSES_FL		bibleverses_fl ;
typedef	BIBLEVERSES_OBJ		bibleverses_obj ;
typedef	BIBLEVERSES_CITE	bibleverses_cite ;
typedef	bibleverses_q		bibleverses_q ;
typedef	BIBLEVERSES_CUR		bibleverses_cur ;
typedef	BIBLEVERSES_INFO	bibleverses_info ;

EXTERNC_begin

extern int bibleverses_open(bibleverses *,cchar *,cchar *) noex ;
extern int bibleverses_count(bibleverses *) noex ;
extern int bibleverses_read(bibleverses *,char *,int,bibleverses_q *) noex ;
extern int bibleverses_get(bibleverses *,bibleverses_q *,char *,int) noex ;
extern int bibleverses_curbegin(bibleverses *,bibleverses_cur *) noex ;
extern int bibleverses_enum(bibleverses *,bibleverses_cur *,
			bibleverses_qUERY *,char *,int) noex ;
extern int bibleverses_curend(bibleverses *,bibleverses_cur *) noex ;
extern int bibleverses_audit(bibleverses *) noex ;
extern int bibleverses_info(bibleverses *,bibleverses_info *) noex ;
extern int bibleverses_chapters(bibleverses *,int,uchar *,int) noex ;
extern int bibleverses_close(bibleverses *) noex ;

EXTERNC_end


#endif /* BIBLEVERSES_INCLUDE */


