/* eigendb HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* store eigen words in a database */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	EIGENDB_INCLUDE
#define	EIGENDB_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<strpack.h>		/* LIBUC */
#include	<hdb.h>			/* LIBUC */


#define	EIGENDB		struct eigendb_head
#define	EIGENDB_CUR	struct eigendb_cursor
#define	EIGENDB_MAGIC	0x83726112


struct eigendb_cursor {
	hdb_cur		*hcp ;
} ; /* end struct */

struct eigendb_head {
	strpack		*spp ;		/* string-pack-pointer */
	hdb		*dbp ;		/* data-base-pointer */
	uint		magval ;
} ; /* end struct */

typedef	EIGENDB_CUR	eigendb_cur ;

#ifdef	__cplusplus
enum eigendbmems {
	eigendbmem_count,
	eigendbmem_close,
	eigendbmem_overlast
} ; /* end enum (eigendbmems) */
struct eigendb ;
struct eigendb_co {
	eigendb		*op = nullptr ;
	int		w = -1 ;
	void operator () (eigendb *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (eigendb_co) */
struct eigendb : eigendb_head {
	eigendb_co	count ;
	eigendb_co	close ;
	eigendb() noex {
	    count	(this,eigendbmem_count) ;
	    close	(this,eigendbmem_close) ;
	    magval = 0 ;
	} ; /* end ctor */
	eigendb(const eigendb &) = delete ;
	eigendb &operator = (const eigendb &) = delete ;
	int open	(cchar *) noex ;
	int addfile	(cchar *) noex ;
	int addword	(cchar *,int = -1) noex ;
	int exists	(cchar *,int = -1) noex ;
	int curbegin	(eigendb_cur *) noex ;
	int curend	(eigendb_cur *) noex ;
	int curenum	(eigendb_cur *,cchar **) noex ;
	operator int () noex ;
	void dtor() noex ;
	destruct eigendb() {
	    if (magval) dtor() ;
	} ;
} ; /* end struct (eigendb) */
#else	/* __cplusplus */
typedef EIGENDB		eigendb ;
#endif /* __cplusplus */

EXTERNC_begin

extern int eigendb_open		(eigendb *,cchar *) noex ;
extern int eigendb_addfile	(eigendb *,cchar *) noex ;
extern int eigendb_addword	(eigendb *,cchar *,int) noex ;
extern int eigendb_exists	(eigendb *,cchar *,int) noex ;
extern int eigendb_count	(eigendb *) noex ;
extern int eigendb_curbegin	(eigendb *,eigendb_cur *) noex ;
extern int eigendb_curend	(eigendb *,eigendb_cur *) noex ;
extern int eigendb_curenum	(eigendb *,eigendb_cur *,cchar **) noex ;
extern int eigendb_close	(eigendb *) noex ;

EXTERNC_end


#endif /* EIGENDB_INCLUDE */


