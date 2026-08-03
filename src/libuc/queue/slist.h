/* slist HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* regular (no-frills) pointer queue */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-03, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SLIST_INCLUDE
#define	SLIST_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<stddef.h>		/* CSTD */
#include	<stdlib.h>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define	SLIST		struct slist_head
#define	SLIST_ENT	struct slist_entry
#define	SLIST_CUR	struct slist_cursor


struct slist_cursor {
	SLIST_ENT	*entp ;
} ; /* end struct (slist_cursor) */

struct slist_entry {
	SLIST_ENT	*next ;
} ; /* end struct (slist_entry) */

struct slist_head {
	SLIST_ENT	*head ;
	SLIST_ENT	*tail ;
	int		cnt ;
} ; /* end struct (slist_head) */

typedef	SLIST_ENT	slist_ent ;
typedef	SLIST_CUR	slist_cur ;

#ifdef	__cplusplus
enum slistmems {
    	slistmem_start,
	slistmem_count,
	slistmem_audit,
	slistmem_finish,
	slistmem_overlast
} ; /* end enum (slistmems) */
struct slist ;
struct slist_co {
	slist		*op = nullptr ;
	int		w = -1 ;
	void operator () (slist *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (slist_co) */
struct slist : slist_head {
	slist_co	start ;
	slist_co	count ;
	slist_co	audit ;
	slist_co	finish ;
	slist() noex {
	    start	(this,slistmem_start) ;
	    count	(this,slistmem_count) ;
	    audit	(this,slistmem_audit) ;
	    finish	(this,slistmem_finish) ;
	    head = nullptr ;
	} ;
	slist(const slist &) = delete ;
	slist &operator = (const slist &) = delete ;
	int ins		(slist_ent *) noex ;
	int insgroup	(slist_ent *,int,int) noex ;
	int present	(slist_ent *) noex ;
	int gethead	(slist_ent **) noex ;
	int gettail	(slist_ent **) noex ;
	int rem		(slist_ent **) noex ;
	int unlink	(slist_ent *) noex ;
	int curbegin	(slist_cur *) noex ;
	int curenum	(slist_cur *,slist_ent **) noex ;
	int curend	(slist_cur *) noex ;
	void dtor	() noex ;
	destruct slist() {
	    if (head) dtor() ;
	} ;
} ; /* end struct (slist) */
#else	/* __cplusplus */
typedef SLIST		slist ;
#endif /* __cplusplus */

EXTERNC_begin

extern int slist_start		(slist *) noex ;
extern int slist_ins		(slist *,slist_ent *) noex ;
extern int slist_insgroup	(slist *,slist_ent *,int,int) noex ;
extern int slist_present	(slist *,slist_ent *) noex ;
extern int slist_gethead	(slist *,slist_ent **) noex ;
extern int slist_gettail	(slist *,slist_ent **) noex ;
extern int slist_rem		(slist *,slist_ent **) noex ;
extern int slist_unlink		(slist *,slist_ent *) noex ;
extern int slist_curbegin	(slist *,slist_cur *) noex ;
extern int slist_curend		(slist *,slist_cur *) noex ;
extern int slist_curenum	(slist *,slist_cur *,slist_ent **) noex ;
extern int slist_count		(slist *) noex ;
extern int slist_audit		(slist *) noex ;
extern int slist_finish		(slist *) noex ;

EXTERNC_end


#endif /* SLIST_INCLUDE */


