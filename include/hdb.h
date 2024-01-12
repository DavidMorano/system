/* hdb HEADER */
/* lang=C20 */

/* general-purpose in-core hashing, DBM-like interface */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-01, David A­D­ Morano
	Orginally written due to frustration with other types of these things.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	HDB_INCLUDE
#define	HDB_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<lookaside.h>
#include	<localmisc.h>


#define HDB_MAGIC	314159261
#define	HDB_DEFSIZE	10
#define	HDB		struct hdb_head
#define HDB_DATUM	struct hdb_datum
#define HDB_DAT		struct hdb_datum
#define	HDB_CUR		struct hdb_cursor
#define HDB_ENT		struct hdb_entry


#ifdef	__cplusplus
extern "C" {
#endif

typedef unsigned int	(*hdbhash_f)(cvoid *,int) noex ;
typedef int		(*hdbcmp_f)(cvoid *,cvoid *,int) noex ;

#ifdef	__cplusplus
}
#endif

struct hdb_datum {
	cvoid		*buf ;
	int		len ;
} ;

struct hdb_entry {
	HDB_ENT		*next ;		/* next in hash chain */
	HDB_ENT		*same ;		/* next w/ same key */
	HDB_DATUM	key ;
	HDB_DATUM	val ;
	uint		hv ;		/* hash-value of key */
} ;

struct hdb_cursor {
	int		i, j, k ;
} ;

struct hdb_head {
	HDB_ENT		**htaddr ;	/* array HDB_ENT pointers */
	hdbhash_f	hashfunc ;
	hdbcmp_f	cmpfunc ;
	lookaside	es ;		/* key-entries */
	uint		magic ;
	int		htlen ;
	int		count ;
	int		at ;		/* allocation-type */
} ; /* end struct (hdb_head) */

typedef struct hdb_head		hdb ;
typedef struct hdb_datum	hdb_dat ;
typedef struct hdb_entry	hdb_ent ;
typedef struct hdb_cursor	hdb_cur ;

EXTERNC_begin

extern int hdb_start(hdb *,int,int,hdbhash_f,hdbcmp_f) noex ;
extern int hdb_store(hdb *,hdb_dat,hdb_dat) noex ;
extern int hdb_curbegin(hdb *,hdb_cur *) noex ;
extern int hdb_curend(hdb *,hdb_cur *) noex ;
extern int hdb_fetch(hdb *,hdb_dat,hdb_cur *,hdb_dat *) noex ;
extern int hdb_fetchrec(hdb *,hdb_dat,hdb_cur *,
			hdb_dat *,hdb_dat *) noex ;
extern int hdb_getkeyrec(hdb *,hdb_dat,hdb_cur *,
			hdb_dat *,hdb_dat *) noex ;
extern int hdb_nextrec(hdb *,hdb_dat,hdb_cur *) noex ;
extern int hdb_delkey(hdb *,hdb_dat) noex ;
extern int hdb_enum(hdb *,hdb_cur *,hdb_dat *,hdb_dat *) noex ;
extern int hdb_getrec(hdb *,hdb_cur *,hdb_dat *,hdb_dat *) noex ;
extern int hdb_next(hdb *,hdb_cur *) noex ;
extern int hdb_delcur(hdb *,hdb_cur *,int) noex ;
extern int hdb_delall(hdb *) noex ;
extern int hdb_count(hdb *) noex ;
extern int hdb_hashtablen(hdb *,uint *) noex ;
extern int hdb_hashtabcounts(hdb *,int *,int) noex ;
extern int hdb_audit(hdb *) noex ;
extern int hdb_finish(hdb *) noex ;

EXTERNC_end


#endif /* HDB_INCLUDE */


