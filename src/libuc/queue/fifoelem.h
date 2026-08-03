/* fifoelem HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* FIFO container object */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	FIFOELEM_INCLUDE
#define	FIFOELEM_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<stddef.h>		/* CSTD */
#include	<stdlib.h>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define	FIFOELEM	struct fifoelem_head
#define	FIFOELEM_ENT	struct fifoelem_entry
#define	FIFOELEM_CUR	struct fifoelem_cursor
#define	FIFOELEM_MAGIC	0x12345678


struct fifoelem_entry {
	FIFOELEM_ENT	*next ;		/* must be first! */
	FIFOELEM_ENT	*prev ;
	void		*dp ;		/* data-pointer */
	int		dl ;		/* data-length */
} ; /* end struct (fifoelem_entry) */

struct fifoelem_head {
	FIFOELEM_ENT	*head ;
	FIFOELEM_ENT	*tail ;
	uint		magval ;
	int		n ;
	int		esz ;
} ; /* end struct (fifoelem_head) */

struct fifoelem_cursor {
	FIFOELEM_ENT	*current ;
} ; /* end struct (fifoelem_cursor) */

typedef FIFOELEM_ENT	fifoelem_ent ;
typedef FIFOELEM_CUR	fifoelem_cur ;

EXTERNC_begin
typedef int (*fifoelem_cmp)(cvoid *,cvoid *) noex ;
EXTERNC_end


#ifdef	__cplusplus
enum fifoelemmems {
	fifoelemmem_del,
	fifoelemmem_count,
	fifoelemmem_finish,
	fifoelemmem_overlast
} ; /* end enum (fifoelemmems) */
struct fifoelem ;
struct fifoelem_co {
	fifoelem	*op = nullptr ;
	int		w = -1 ;
	void operator () (fifoelem *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex {
	    return operator int () ;
	} ;
} ; /* end struct (fifoelem_co) */
struct fifoelem : fifoelem_head {
	fifoelem_co	del ;
	fifoelem_co	count ;
	fifoelem_co	finish ;
	fifoelem() noex {
	    del		(this,fifoelemmem_del) ;
	    count	(this,fifoelemmem_count) ;
	    finish	(this,fifoelemmem_finish) ;
	    magval = 0 ;
	} ; /* end ctor */
	fifoelem(const fifoelem &) = delete ;
	fifoelem &operator = (const fifoelem &) = delete ;
	int start	(int) noex ;
	int ins		(void *) noex ;
	int get		(fifoelem_ent **) noex ;
	int rem		(void *,int) noex ;
	int present	(cvoid *,int,fifoelem_cmp) noex ;
	int curbegin	(fifoelem_cur *) noex ;
	int curend	(fifoelem_cur *) noex ;
	int curdel	(fifoelem_cur *) noex ;
	int curenum	(fifoelem_cur *,fifoelem_ent **) noex ;
	void dtor() noex ;
	operator int () noex ;
	destruct fifoelem() {
	    if (magval) dtor() ;
	} ;
} ; /* end struct (fifoelem) */
#else	/* __cplusplus */
typedef FIFOSTR		fifoelem ;
#endif /* __cplusplus */

EXTERNC_begin

extern int fifoelem_start    (fifoelem *,int) noex ;
extern int fifoelem_finish   (fifoelem *) noex ;
extern int fifoelem_ins	     (fifoelem *,void *) noex ;
extern int fifoelem_get	     (fifoelem *,fifoelem_ent **) noex ;
extern int fifoelem_rem	     (fifoelem *,void *,int) noex ;
extern int fifoelem_del	     (fifoelem *) noex ;
extern int fifoelem_count    (fifoelem *) noex ;
extern int fifoelem_present  (fifoelem *,cvoid *,int,fifoelem_cmp) noex ;
extern int fifoelem_curbegin (fifoelem *,fifoelem_cur *) noex ;
extern int fifoelem_curend   (fifoelem *,fifoelem_cur *) noex ;
extern int fifoelem_curdel   (fifoelem *,fifoelem_cur *) noex ;
extern int fifoelem_curenum  (fifoelem *,fifoelem_cur *,fifoelem_ent **) noex ;

/*
extern int fifoelem_curenum(fifoelem *,fifoelem_cur *,void *) noex ;
extern int fifoelem_search(fifoelem *,char *,int (*)(),char **) noex ;
extern int fifoelem_find(fifoelem *,void *) noex ;
*/

EXTERNC_end


#endif /* FIFOELEM_INCLUDE */


