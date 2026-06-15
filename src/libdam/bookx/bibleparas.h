/* bibleparas HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* BIBLEPARAS implementation */
/* version %I% last-modified %G% */


/* revision history:

	- 2008-10-01, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	BIBLEPARAS_INCLUDE
#define	BIBLEPARAS_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<bpi.h>			/* LIBDAM */


#define	BIBLEPARAS		struct bibleparas_head
#define	BIBLEPARAS_OBJ		struct bibleparas_object
#define	BIBLEPARAS_FL		struct bibleparas_flags
#define	BIBLEPARAS_CITE		struct bibleparas_query
#define	BIBLEPARAS_Q		struct bibleparas_query
#define	BIBLEPARAS_CUR		struct bibleparas_cursor
#define	BIBLEPARAS_INFO		struct bibleparas_information
#define	BIBLEPARAS_MAGIC	0x99447246
/* default DB name */
#define	BIBLEPARAS_DBNAME	"default"


struct bibleparas_object {
	cchar		*name ;
	uint		objsz ;
	uint		cursz ;
} ; /* end struct */

struct bibleparas_information {
	time_t		dbtime ;		/* db-time */
	time_t		vitime ;		/* vi-time */
	uint		maxbook ;
	uint		maxchapter ;
	uint		nverses ;
	uint		nzverses ;
} ; /* end struct */

struct bibleparas_query {
	uchar		b, c, v ;
} ; /* end struct */

struct bibleparas_cursor {
	BPI_CUR		vicur ;
} ; /* end struct */

struct bibleparas_flags {
	uint		vind:1 ;		/* index is loaded */
} ; /* end struct */

struct bibleparas_head {
	cchar		*pr ;
	cchar 		*dbname ;		/* DB-name */
	cchar 		*dbfname ;		/* DB file-name */
	char		*mapdata ;		/* memory-map address */
	BPI		vind ;			/* verse-index */
	time_t		ti_db ;			/* DB file modification */
	time_t		ti_map ;		/* DB map */
	time_t		ti_lastcheck ;		/* last check of file */
	time_t		ti_vind ;		/* verse-index */
	size_t		mapsize ;		/* map size */
	size_t		filesize ;		/* file size */
	BIBLEPARAS_FL	fl ;
	uint		magval ;
	int		nverses ;
	int		ncursors ;
} ; /* end struct */

typedef	BIBLEPARAS		bibleparas ;
typedef	BIBLEPARAS_OBJ		bibleparas_obj ;
typedef	BIBLEPARAS_FL		bibleparas_fl ;
typedef	BIBLEPARAS_CITE		bibleparas_cite ;
typedef	BIBLEPARAS_Q		bibleparas_q ;
typedef	BIBLEPARAS_CUR		bibleparas_cur ;
typedef	BIBLEPARAS_INFO		bibleparas_info ;

EXTERNC_begin

extern int bibleparas_open(bibleparas *,cchar *,cchar *) noex ;
extern int bibleparas_count(bibleparas *) noex ;
extern int bibleparas_ispara(bibleparas *,bibleparas_q *) noex ;
extern int bibleparas_curbegin(bibleparas *,bibleparas_cur *) noex ;
extern int bibleparas_curenum(bibleparas *,bibleparas_cur *,
		bibleparas_q *) noex ;
extern int bibleparas_curend(bibleparas *,bibleparas_cur *) noex ;
extern int bibleparas_audit(bibleparas *) noex ;
extern int bibleparas_getinfo(bibleparas *,bibleparas_info *) noex ;
extern int bibleparas_close(bibleparas *) noex ;

EXTERNC_end


#endif /* BIBLEPARAS_INCLUDE */


