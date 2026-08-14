/* tagtrack HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* track tagtypes in DWB documents */
/* version %I% last-modified %G% */


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

#ifndef	TAGTRACK_INCLUDE
#define	TAGTRACK_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<vechand.h>		/* LIBUC */
#include	<vecobj.h>		/* LIBUC */


#define	TAGTRACK		struct tagtrack_head
#define	TAGTRACK_TAG		struct tagtrack_tager
#define	TAGTRACK_ESC		struct tagtrack_escape
#define	TAGTRACK_ENT		struct tagtrack_entry
#define	TAGTRACK_CUR		struct tagtrack_cursor
#define	TAGTRACK_MAGIC		0x31887239
#define	TAGTRACK_DEFENTS	20


enum tagtypes {
	tagtype_table,
	tagtype_example,
	tagtype_figure,
	tagtype_equation,
	tagtype_overlast
} ; /* end enum */

/* store tags here */
struct tagtrack_tager {
	cchar		*name ;		/* tag name */
	int		c ;		/* tag-type count */
	int		tagtype ;	/* tag-type */
} ; /* end struct */

struct tagtrack_escape {
	TAGTRACK_TAG	*tagp ;
	uint		eoff ;
	int		elen ;
	int		fi ;
} ; /* end struct */

struct tagtrack_entry {
	uint		eoff ;
	int		elen ;
	int		fi ;
	int		v ;
} ; /* end struct */

struct tagtrack_head {
	vechand		*tlp ;		/* tag-list-pointer */
	vecobj		*elp ;		/* ESC-list-pointer */
	uint		magval ;
	int		c[tagtype_overlast] ;
	int		lc ;		/* last count */
	int		ltt ;		/* last tag-type */
} ; /* end struct */

struct tagtrack_cursor {
	int		i ;
} ; /* end struct */

typedef	TAGTRACK	tagtrack ;
typedef	TAGTRACK_TAG	tagtrack_tag ;
typedef	TAGTRACK_ESC	tagtrack_esc ;
typedef	TAGTRACK_ENT	tagtrack_ent ;
typedef	TAGTRACK_CUR	tagtrack_cur ;

EXTERNC_begin

extern int tagtrack_start	(tagtrack *) noex ;
extern int tagtrack_finish	(tagtrack *) noex ;
extern int tagtrack_scanline	(tagtrack *,int,uint,cchar *,int) noex ;
extern int tagtrack_add		(tagtrack *,int,uint,int,cchar *,int) noex ;
extern int tagtrack_adds	(tagtrack *,int,uint,int,cchar *,int) noex ;
extern int tagtrack_curbegin	(tagtrack *,tagtrack_cur *) noex ;
extern int tagtrack_curend	(tagtrack *,tagtrack_cur *) noex ;
extern int tagtrack_curenum	(tagtrack *,tagtrack_cur *,
		tagtrack_ent *) noex ;
extern int tagtrack_audit	(tagtrack *) noex ;

EXTERNC_end


#endif /* TAGTRACK_INCLUDE */



