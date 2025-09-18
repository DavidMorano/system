/* fifostr HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* FIFO string operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-12-09, David A­D­ Morano
	Originally written for Rightcore Network Services.

	= 2017-09-12, David A­D­ Morano
	Small interface change to |fifostr_entread()|.

*/

/* Copyright © 1999,2017 David A­D­ Morano.  All rights reserved. */

#ifndef	FIFOSTR_INCLUDE
#define	FIFOSTR_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	FIFOSTR_MAGIC	0x12345678
#define	FIFOSTR		struct fifostr_head
#define	FIFOSTR_CUR	struct fifostr_cursor
#define	FIFOSTR_ENT	struct fifostr_entry


struct fifostr_entry {
	FIFOSTR_ENT	*next ;
	FIFOSTR_ENT	*prev ;
	int		slen ;
} ;

struct fifostr_head {
	FIFOSTR_ENT	*head ;
	FIFOSTR_ENT	*tail ;
	uint		magic ;
	int		ic ;		/* item count */
	int		cnt ;		/* character count */
} ;

struct fifostr_cursor {
	FIFOSTR_ENT	*current ;
} ;

typedef FIFOSTR_CUR	fifostr_cur ;
typedef FIFOSTR_ENT	fifostr_ent ;

#ifdef	__cplusplus
enum fifostrmems {
	fifostrmem_start,
	fifostrmem_headlen,
	fifostrmem_count,
	fifostrmem_finish,
	fifostrmem_overlast
} ; /* end enum (fifostrmems) */
struct fifostr ;
struct fifostr_co {
	fifostr		*op = nullptr ;
	int		w = -1 ;
	void operator () (fifostr *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (fifostr_co) */
struct fifostr : fifostr_head {
	fifostr_co	start ;
	fifostr_co	headlen ;
	fifostr_co	count ;
	fifostr_co	finish ;
	fifostr() noex {
	    start	(this,fifostrmem_start) ;
	    headlen	(this,fifostrmem_headlen) ;
	    count	(this,fifostrmem_count) ;
	    finish	(this,fifostrmem_finish) ;
	    magic = 0 ;
	} ; /* end ctor */
	fifostr(const fifostr &) = delete ;
	fifostr &operator = (const fifostr &) = delete ;
	int add		(cchar *,int = -1) noex ;
	int headread	(char *,int) noex ;
	int entread	(char *,int,int) noex ;
	int entlen	(int) noex ;
	int rem		(char *,int) noex ;
	int curbegin	(fifostr_cur *) noex ;
	int curend	(fifostr_cur *) noex ;
	int curenum	(fifostr_cur *,char *,int) noex ;
	int curdel	(fifostr_cur *) noex ;
	void dtor() noex ;
	destruct fifostr() {
	    if (magic) dtor() ;
	} ;
} ; /* end struct (fifostr) */
#else	/* __cplusplus */
typedef FIFOSTR		fifostr ;
#endif /* __cplusplus */

EXTERNC_begin

typedef int (*fifostr_cmp)(cchar *,cchar *) noex ;

extern int fifostr_start	(fifostr *) noex ;
extern int fifostr_add		(fifostr *,cchar *,int) noex ;
extern int fifostr_headread	(fifostr *,char *,int) noex ;
extern int fifostr_headlen	(fifostr *) noex ;
extern int fifostr_entread	(fifostr *,char *,int,int) noex ;
extern int fifostr_entlen	(fifostr *,int) noex ;
extern int fifostr_rem		(fifostr *,char *,int) noex ;
extern int fifostr_count	(fifostr *) noex ;
extern int fifostr_curbegin	(fifostr *,fifostr_cur *) noex ;
extern int fifostr_curend	(fifostr *,fifostr_cur *) noex ;
extern int fifostr_curenum	(fifostr *,fifostr_cur *,char *,int) noex ;
extern int fifostr_curdel	(fifostr *,fifostr_cur *) noex ;
extern int fifostr_finish	(fifostr *) noex ;

EXTERNC_end


#endif /* FIFOSTR_INCLUDE */


