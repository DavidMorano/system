/* bdb HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This module was changed to serve in the REFERM program.

	= 2005-10-01, David A­D­ Morano
	This was changed to work in the MMCITE program. The old
	REFERM program is really obsolete. It used a database lookup
	strategy to remote databases. The high-level problem is:
	what to do if the cited BIB entry isn't found? How does a
	maintainer of the present (local) document know what that
	BIB entry was? The new strategy (implemented by the MMCITE
	program) is more like what is done with BibTeX in the TeX
	(or LaTeX) world. All BIB databases are really expected to
	be maintained by the document creator -- not some centralized
	entiry. The older centralized model reflected more the use
	in the corporate world (where different people create BIB
	entries) than in the more "modern" personal-responsibility
	type of world! :-) Anyway, this is the way the gods seem
	to now want to do things. Deal with it!

*/

/* Copyright © 1998,2005 David A­D­ Morano.  All rights reserved. */

#ifndef	BDB_INCLUDE
#define	BDB_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<vecobj.h>
#include	<hdb.h>


#define	BDB		struct bdb_head
#define	BDB_CUR		struct bdb_cursor
#define	BDB_ENT		struct bdb_entry
/* ? */
#define	BDB_QUERYKEY	"Q"
#define	BDB_CITEKEYLEN	100
#define	BDB_BUFLEN	(3 * MAXPATHLEN)
/* operational options */
#define	BDB_OUNIQ	0x01		/* queries must be unique */


struct bdb_cursor {
	HDB_CUR		cur ;
} ;

struct bdb_entry {
	cchar		*(*keyvals)[2] ;
	cchar		*query ;
	int		nkeys ;
	int		size ;
	int		fi ;		/* file index */
} ;

struct bdb_head {
	cchar		*qkbuf ;	/* query key name (usually 'Q') */
	vecobj		files ;
	hdb		keys ;
	uint		magic ;
	uint		opts ;
	int		unindexed ;	/* number of unindexed files */
	int		qklen ;
} ;

typedef	BDB		bdb ;
typedef	BDB_CUR		bdb_cur ;
typedef	BDB_ENT		bdb_ent ;

EXTERNC_begin

extern int	bdb_start(BDB *,cchar *,int) noex ;
extern int	bdb_finish(BDB *) noex ;
extern int	bdb_add(BDB *,cchar *) noex ;
extern int	bdb_count(BDB *) noex ;
extern int	bdb_query(BDB *,cchar *,BDB_ENT *,char *,int) noex ;
extern int	bdb_curbegin(BDB *,BDB_CUR *) noex ;
extern int	bdb_curend(BDB *,BDB_CUR *) noex ;
extern int	bdb_delcur(BDB *,BDB_CUR *,int) noex ;

EXTERNC_end


#endif /* BDB_INCLUDE */


