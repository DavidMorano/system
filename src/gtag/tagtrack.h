/* tagtrack HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

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
#include	<sys/types.h>
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<vechand.h>
#include	<vecobj.h>


#define	TAGTRACK		struct tagtrack_head
#define	TAGTRACK_TAG		struct tagtrack_tag
#define	TAGTRACK_ESC		struct tagtrack_esc
#define	TAGTRACK_ENT		struct tagtrack_e
#define	TAGTRACK_CUR		struct tagtrack_cur


enum tagtypes {
	tagtype_table,
	tagtype_example,
	tagtype_figure,
	tagtype_equation,
	tagtype_overlast
} ; /* end enum */

/* store tags here */
struct tagtrack_tag {
	cchar		*name ;		/* tag name */
	int		c ;		/* tag-type count */
	int		tagtype ;	/* tag-type */
} ; /* end struct */

struct tagtrack_esc {
	TRACKTAG_TAG	*tagp ;
	uint		eoff ;
	int		elen ;
	int		fi ;
} ; /* end struct */

struct tagtrack_e {
	uint		eoff ;
	int		elen ;
	int		fi ;
	int		v ;
} ; /* end struct */

struct tagtrack_head {
	uint		magic ;
	int		c[tagtype_overlast] ;
	int		lc ;		/* last count */
	int		ltt ;		/* last tag-type */
	vechand		tags ;
	vecobj		list ;		/* list of escapes */
} ; /* end struct */

struct tagtrack_cur {
	int		i ;
} ; /* end struct */

EXTERNC_begin

extern int	tagtrack_start(TAGTRACK *) noex ;
extern int	tagtrack_finish(TAGTRACK *) noex ;
extern int	tagtrack_scanline(TAGTRACK *,int,uint,cchar *,int) noex ;
extern int	tagtrack_adds(TAGTRACK *,int,uint,int,cchar *,int) noex ;
extern int	tagtrack_curbegin(TAGTRACK *,TAGTRACK_CUR *) noex ;
extern int	tagtrack_curend(TAGTRACK *,TAGTRACK_CUR *) noex ;
extern int	tagtrack_enum(TAGTRACK *,TAGTRACK_CUR *,TAGTRACK_ENT *) noex ;
extern int	tagtrack_audit(TAGTRACK *) noex ;

EXTERNC_end


#endif /* TAGTRACK_INCLUDE */



