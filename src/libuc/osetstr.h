/* osetstr HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* ordered set of strings */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	OSETSTR_INCLUDE
#define	OSETSTR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	OSETSTR_MAGIC	0x97351229
#define	OSETSTR		struct osetstr_head
#define	OSETSTR_CUR	struct osetstr_cursor


struct osetstr_cursor {
	void		*itp ;
} ;

struct osetstr_head {
	void		*setp ;
	uint		magic ;
} ;

typedef OSETSTR_CUR	osetstr_cur ;

#ifdef	__cplusplus
enum osetstrmems {
    	osetstrmem_start,
	osetstrmem_count,
	osetstrmem_delall,
	osetstrmem_finish,
	osetstrmem_overlast
} ;
struct osetstr ;
struct osetstr_co {
	osetstr		*op = nullptr ;
	int		w = -1 ;
	void operator () (osetstr *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	int operator () (int = 0) noex ;
	operator int () noex {
	    return operator () () ;
	} ;
} ; /* end struct (osetstr_co) */
struct osetstr : osetstr_head {
	osetstr_co	start ;
	osetstr_co	delall ;
	osetstr_co	count ;
	osetstr_co	finish ;
	osetstr() noex {
	    start(this,osetstrmem_start) ;
	    count(this,osetstrmem_count) ;
	    delall(this,osetstrmem_delall) ;
	    finish(this,osetstrmem_finish) ;
	} ;
	osetstr(const osetstr &) = delete ;
	osetstr &operator = (const osetstr &) = delete ;
	int already(cchar *,int = -1) noex ;
	int add(cchar *,int = -1) noex ;
	int del(cchar *,int = -1) noex ;
	int curbegin(osetstr_cur *) noex ;
	int curenum(osetstr_cur *,cchar **) noex ;
	int curend(osetstr_cur *) noex ;
	void dtor() noex ;
	~osetstr() {
	    dtor() ;
	} ;
} ; /* end struct (osetstr) */
#else	/* __cplusplus */
typedef OSETSTR		osetstr ;
#endif /* __cplusplus */


EXTERNC_begin

extern int osetstr_start(osetstr *,int) noex ;
extern int osetstr_already(osetstr *,cchar *,int) noex ;
extern int osetstr_add(osetstr *,cchar *,int) noex ;
extern int osetstr_del(osetstr *,cchar *,int) noex ;
extern int osetstr_delall(osetstr *) noex ;
extern int osetstr_count(osetstr *) noex ;
extern int osetstr_curbegin(osetstr *,osetstr_cur *) noex ;
extern int osetstr_curenum(osetstr *,osetstr_cur *,cchar **) noex ;
extern int osetstr_curend(osetstr *,osetstr_cur *) noex ;
extern int osetstr_finish(osetstr *) noex ;

extern int osetstr_loadfile(osetstr *,int,cchar *) noex ;

EXTERNC_end

#ifdef	__cplusplus

template<typename ... Args>
inline int osetstr_magic(osetstr *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = SR_NOTOPEN ;
	    if (op->magic == OSETSTR_MAGIC) {
		rs = SR_BUGCHECK ;
		if (op->setp != nullptr) {
		    rs = SR_OK ;
		}
	    }
	}
	return rs ;
}

#endif /* __cplusplus */


#endif /* OSETSTR_INCLUDE */


