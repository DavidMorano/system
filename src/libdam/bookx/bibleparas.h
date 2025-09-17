/* bibleparas HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */


/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	BIBLEPARAS_INCLUDE
#define	BIBLEPARAS_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>

#include	<bpi.h>


#define	BIBLEPARAS_MAGIC	0x99447246
#define	BIBLEPARAS		struct bibleparas_head
#define	BIBLEPARAS_OBJ		struct bibleparas_object
#define	BIBLEPARAS_FL		struct bibleparas_flags
#define	BIBLEPARAS_CITE		struct bibleparas_query
#define	BIBLEPARAS_Q		struct bibleparas_query
#define	BIBLEPARAS_CUR		struct bibleparas_cursor
#define	BIBLEPARAS_INFO		struct bibleparas_information
/* default DB name */
#define	BIBLEPARAS_DBNAME	"default"


struct bibleparas_object {
	cchar		*name ;
	uint		objsize ;
	uint		cursize ;
} ;

struct bibleparas_information {
	time_t		dbtime ;		/* db-time */
	time_t		vitime ;		/* vi-time */
	uint		maxbook ;
	uint		maxchapter ;
	uint		nverses ;
	uint		nzverses ;
} ;

struct bibleparas_query {
	uchar		b, c, v ;
} ;

struct bibleparas_cursor {
	BPI_CUR		vicur ;
} ;

struct bibleparas_flags {
	uint		vind:1 ;		/* index is loaded */
} ;

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
	uint		magic ;
	int		nverses ;
	int		ncursors ;
} ;

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
extern int bibleparas_enum(bibleparas *,bibleparas_cur *,bibleparas_q *) noex ;
extern int bibleparas_curend(bibleparas *,bibleparas_cur *) noex ;
extern int bibleparas_audit(bibleparas *) noex ;
extern int bibleparas_info(bibleparas *,bibleparas_info *) noex ;
extern int bibleparas_close(bibleparas *) noex ;

EXTERNC_end


#endif /* BIBLEPARAS_INCLUDE */


