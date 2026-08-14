/* citedb HEADER (Citætion Data-base) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* maintain a DB of encountered citations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This module was changed to serve in the REFERM program.

	= 2005-10-01, David A­D­ Morano
        This was changed to work in the MMCITE program. The old REFERM program
        is really obsolete. It used a database lookup strategy to remote
        databases. The high-level problem is: what to do if the cited BIB entry
        isn't found? How does a maintainer of the present (local) document know
        what that BIB entry was? The new strategy (implemented by the MMCITE
        program) is more like what is done with BibTeX in the TeX (or LaTeX)
        world. All BIB databases are really expected to be maintained by the
        document creator -- not some centralized entiry. The older centralized
        model reflected more the use in the corporate world (where different
        people create BIB entries) than in the more "modern"
        personal-responsibility type of world! :-) Anyway, this is the way the
        gods seem to now want to do things. Deal with it!

*/

/* Copyright © 1998,2005 David A­D­ Morano.  All rights reserved. */

#ifndef	CITEDB_INCLUDE
#define	CITEDB_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<vecobj.h>		/* LIBUC */
#include	<hdb.h>		/* LIBUC */


#define	CITEDB			struct citedb_head
#define	CITEDB_STORE		struct citedb_storer
#define	CITEDB_ENT		struct citedb_entry
#define	CITEDB_OFF		struct citedb_offset
#define	CITEDB_CUR		struct citedb_cursor
#define	CITEDB_MAGIC		0x86544567
#define	CITEDB_CITESTRLEN	3
#define	CITEDB_CITEKEYLEN	80


struct citedb_cursor {
	int		i ;
} ; /* end struct */

/* user visible structure */
struct citedb_entry {
	uint		off ;
	int		fi ;
	int		ci ;		/* index of this citation */
	int		n ;		/* total number of this citation */
	char		citekey[CITEDB_CITEKEYLEN + 1] ;
	char		citestr[CITEDB_CITESTRLEN + 1] ;
} ; /* end struct (citedb_entry) */

/* store key-offset in list */
struct citedb_offset {
	CITEDB_STORE	*sp ;
	uint		off ;		/* file offset of citation */
	int		fi ;		/* file index */
	int		ci ;		/* citation index of this citation */
} ; /* end struct */

/* store keyname in hash table */
struct citedb_storer {
	cchar		*citekey ;
	int		n ;		/* total number of this citation */
	char		citestr[CITEDB_CITESTRLEN + 1] ;
} ; /* end struct */

struct citedb_head {
	vecobj		*elp ;		/* entry-list-pointer */
	hdb		*slp ;		/* store-list-pointer */
	uint		magval ;
	int		citestrindex ;	/* incremented as necessary */
} ; /* end struct */

typedef	CITEDB		citedb ;
typedef	CITEDB_STORE	citedb_store ;
typedef	CITEDB_ENT	citedb_ent ;
typedef	CITEDB_OFF	citedb_off ;
typedef	CITEDB_CUR	citedb_cur ;

EXTERNC_begin

extern int citedb_start		(citedb *) noex ;
extern int citedb_finish	(citedb *) noex ;
extern int citedb_add		(citedb *op,int fi,uint fo,cc *kp,int kl) noex ;
extern int citedb_adds		(citedb *,int,uint,cchar *,int) noex ;
extern int citedb_curbegin	(citedb *,citedb_cur *) noex ;
extern int citedb_curend	(citedb *,citedb_cur *) noex ;
extern int citedb_curenum	(citedb *,citedb_cur *,citedb_ent *) noex ;
extern int citedb_fetch		(citedb *,cchar *,
			citedb_cur *,citedb_ent *) noex ;
extern int	citedb_audit(citedb *) noex ;

EXTERNC_end

#endif /* CITEDB_INCLUDE */



