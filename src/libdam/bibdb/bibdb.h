/* bibdb HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* Bibliographical DataBase */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This module was changed to serve in the REFERM program.

	= 2005-10-01, David A­D­ Morano
	This was changed to work in the MMCITE program.  The old
	REFERM program is really obsolete.  It used a database lookup
	strategy to remote databases.  The high-level problem is:
	what to do if the cited BIB entry is not found?  How does a
	maintainer of the present (local) document know what that
	BIB entry was?  The new strategy (implemented by the MMCITE
	program) is more like what is done with BibTeX in the TeX
	(or LaTeX) world.  All BIB databases are really expected to
	be maintained by the document creator -- not some centralized
	entity.  The older centralized model reflected more the use
	in the corporate world (where different people create BIB
	entries) than in the more "modern" personal-responsibility
	type of world! :-)  Anyway, this is the way the gods seem
	to now want to do things.  Deal with it!

*/

/* Copyright © 1998,2005 David A­D­ Morano.  All rights reserved. */

#ifndef	BIBDB_INCLUDE
#define	BIBDB_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<vecobj.h>		/* LIBUC */
#include	<hdb.h>			/* LIBUC */


#define	BIBDB			struct bibdb_head
#define	BIBDB_CUR		struct bibdb_cursor
#define	BIBDB_ENT		struct bibdb_entry
#define	BIBDB_MAGIC		0x86537484
#define	BIBDB_QUERYKEY		"Q"
#define	BIBDB_CITEKEYLEN	100

enum bibdbos {
    	bibdbo_uniq,
    	bibdbo_overlast
} ; /* end enum */

#ifdef	__cplusplus
struct bibdboms {
    	static constexpr int	uniq	= (1 << bibdbo_uniq) ;
} ; /* end struct (bibdboms) */
#endif /* __cplusplus */

/* operational options */
#define	BIBDB_OUNIQ	(1 << bibdbo_uniq) ; /* queries must be unique */

struct bibdb_cursor {
	hdb_cur		*icurp ;
} ; /* end struct */

struct bibdb_entry {
	cchar		*(*keyvals)[2] ;
	cchar		*query ;
	int		nkeys ;
	int		sz ;
	int		fi ;		/* file index */
} ; /* end struct */

struct bibdb_head {
	cchar		*qkp ;		/* query-key-name pointer */
	vecobj		*flp ;		/* file-list pointer */
	hdb		*klp ;		/* key-list pointer */
	uint		magval ;
	uint		opts ;
	int		unindexed ;	/* number of unindexed files */
	int		qkl ;		/* query-key-name length */
} ; /* end struct */

typedef	BIBDB		bibdb ;
typedef	BIBDB_CUR	bibdb_cur ;
typedef	BIBDB_ENT	bibdb_ent ;

EXTERNC_begin

extern int	bibdb_start	(bibdb *,cchar *,int) noex ;
extern int	bibdb_finish	(bibdb *) noex ;
extern int	bibdb_add	(bibdb *,cchar *) noex ;
extern int	bibdb_count	(bibdb *) noex ;
extern int	bibdb_query	(bibdb *,cchar *,
					bibdb_ent *,char *,int) noex ;
extern int	bibdb_curbegin	(bibdb *,bibdb_cur *) noex ;
extern int	bibdb_curend	(bibdb *,bibdb_cur *) noex ;
extern int	bibdb_curdel	(bibdb *,bibdb_cur *,int) noex ;
extern int	bibdb_curenum	(bibdb *,bibdb_cur *,
					bibdb_ent *,char *,int) noex ;

EXTERNC_end

#ifdef	__cplusplus
extern const bibdboms		bibdbom ;
#endif /* __cplusplus */


#endif /* BIBDB_INCLUDE */


