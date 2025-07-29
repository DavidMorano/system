/* dirlist HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* directory list manager */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-09-10, David A­D­ Morano
	I created this modeled after something similar that was
	used for some of my PCS programs ('bbnews' I think).

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	DIRLIST_INCLUDE
#define	DIRLIST_INCLUDE 


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>
#include	<vecobj.h>


#define	DIRLIST		struct dirlist_head
#define	DIRLIST_CUR	struct dirlist_cursor
#define	DIRLIST_MAGIC	0x09854123


struct dirlist_cursor {
	int		i ;
} ;

struct dirlist_head {
	vecobj		*dbp ;
	uint		magic ;
	int		tlen ;
} ;

typedef DIRLIST_CUR	dirlist_cur ;

#ifdef	__cplusplus
enum dirlistmems {
	dirlistmem_start,
	dirlistmem_semi,
	dirlistmem_count,
	dirlistmem_strsize,
	dirlistmem_join,
	dirlistmem_finish,
	dirlistmem_overlast
} ;
struct dirlist ;
struct dirlist_co {
	dirlist		*op = nullptr ;
	int		w = -1 ;
	void operator () (dirlist *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (dirlist_co) */
struct dirlist : dirlist_head {
	dirlist_co	start ;
	dirlist_co	semi ;
	dirlist_co	count ;
	dirlist_co	strsize ;
	dirlist_co	join ;
	dirlist_co	finish ;
	dirlist() noex {
	    start(this,dirlistmem_start) ;
	    semi(this,dirlistmem_semi) ;
	    count(this,dirlistmem_count) ;
	    strsize(this,dirlistmem_strsize) ;
	    join(this,dirlistmem_join) ;
	    finish(this,dirlistmem_finish) ;
	    magic = 0 ;
	} ;
	dirlist(const dirlist &) = delete ;
	dirlist &operator = (const dirlist &) = delete ;
	int adds(cchar *,int) noex ;
	int add(cchar *,int) noex ;
	int curbegin(dirlist_cur *) noex ;
	int curend(dirlist_cur *) noex ;
	int curenum(dirlist_cur *,char *,int) noex ;
	int curget(dirlist_cur *,cchar **) noex ;
	int joinmk(char *,int) noex ;
	operator int () noex ;
	void dtor() noex ;
	destruct dirlist() {
	    if (magic) dtor() ;
	} ;
} ; /* end struct (dirlist) */
#else	/* __cplusplus */
typedef DIRLIST		dirlist ;
#endif /* __cplusplus */


EXTERNC_begin

extern int dirlist_start(dirlist *) noex ;
extern int dirlist_semi(dirlist *) noex ;
extern int dirlist_adds(dirlist *,cchar *,int) noex ;
extern int dirlist_add(dirlist *,cchar *,int) noex ;
extern int dirlist_count(dirlist *) noex ;
extern int dirlist_strsize(dirlist *) noex ;
extern int dirlist_curbegin(dirlist *,dirlist_cur *) noex ;
extern int dirlist_curend(dirlist *,dirlist_cur *) noex ;
extern int dirlist_curenum(dirlist *,dirlist_cur *,char *,int) noex ;
extern int dirlist_curget(dirlist *,dirlist_cur *,cchar **) noex ;
extern int dirlist_join(dirlist *) noex ;
extern int dirlist_joinsize(dirlist *) noex ;
extern int dirlist_joinmk(dirlist *,char *,int) noex ;
extern int dirlist_finish(dirlist *) noex ;

EXTERNC_end


#endif /* DIRLIST_INCLUDE */


