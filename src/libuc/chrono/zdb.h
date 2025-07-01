/* zdb HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* time-zone database management */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	ZDB_INCLUDE
#define	ZDB_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>


#define	ZDB		struct zdb_head


struct zdb_head {
	cchar		*name ;		/* static-data (not allocated) */
	short		woff ;		/* minutes west of GMT */
	short		isdst ;		/* values: -1, 0, +1 */
} ;

#ifdef	__cplusplus
enum zdbmems {
	zdbmem_count,
	zdbmem_overlast
} ;
struct zdb ;
struct zdb_co {
	zdb		*op = nullptr ;
	int		w = -1 ;
	void operator () (zdb *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (zdb_co) */
struct zdb : zdb_head {
	zdb_co		count ;
	zdb() noex {
	    count(this,zdbmem_count) ;
	} ;
	zdb(const zdb &) = delete ;
	zdb &operator = (const zdb &) = delete ;
	int nameoff(cchar *,int,int) noex ;
	int setname(cchar *,int) noex ;
	int setoff(int) noex ;
	int offisdst(int,int) noex ;
} ; /* end struct (zdb) */
#else	/* __cplusplus */
typedef ZDB		zdb ;
#endif /* __cplusplus */

EXTERNC_begin

extern int	zdb_nameoff(zdb *,cchar *,int,int) noex ;
extern int	zdb_setname(zdb *,cchar *,int) noex ;
extern int	zdb_setoff(zdb *,int) noex ;
extern int	zdb_offisdst(zdb *,int,int) noex ;
extern int	zdb_count(zdb *) noex ;

EXTERNC_end


#endif /* ZDB_INCLUDE */


