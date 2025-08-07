/* setostr HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* ordered set of strings */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SETOSTR_INCLUDE
#define	SETOSTR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	SETOSTR_MAGIC	0x97351229
#define	SETOSTR		struct setostr_head
#define	SETOSTR_CUR	struct setostr_cursor


struct setostr_cursor {
	void		*itp ;
} ;

struct setostr_head {
	void		*setp ;
	uint		magic ;
} ;

typedef SETOSTR_CUR	setostr_cur ;

#ifdef	__cplusplus
enum setostrmems {
    	setostrmem_start,
	setostrmem_delall,
	setostrmem_count,
	setostrmem_finish,
	setostrmem_overlast
} ;
struct setostr ;
struct setostr_co {
	setostr		*op = nullptr ;
	int		w = -1 ;
	void operator () (setostr *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	int operator () (int = 0) noex ;
	operator int () noex {
	    return operator () () ;
	} ;
} ; /* end struct (setostr_co) */
struct setostr : setostr_head {
	setostr_co	start ;
	setostr_co	delall ;
	setostr_co	count ;
	setostr_co	finish ;
	setostr() noex {
	    start(this,setostrmem_start) ;
	    delall(this,setostrmem_delall) ;
	    count(this,setostrmem_count) ;
	    finish(this,setostrmem_finish) ;
	    magic = 0 ;
	} ;
	setostr(const setostr &) = delete ;
	setostr &operator = (const setostr &) = delete ;
	int already(cchar *,int = -1) noex ;
	int add(cchar *,int = -1) noex ;
	int del(cchar *,int = -1) noex ;
	int curbegin(setostr_cur *) noex ;
	int curenum(setostr_cur *,cchar **) noex ;
	int curend(setostr_cur *) noex ;
	void dtor() noex ;
	destruct setostr() {
	    if (magic) dtor() ;
	} ;
} ; /* end struct (setostr) */
#else	/* __cplusplus */
typedef SETOSTR		setostr ;
#endif /* __cplusplus */


EXTERNC_begin

extern int setostr_start(setostr *,int) noex ;
extern int setostr_already(setostr *,cchar *,int) noex ;
extern int setostr_add(setostr *,cchar *,int) noex ;
extern int setostr_del(setostr *,cchar *,int) noex ;
extern int setostr_delall(setostr *) noex ;
extern int setostr_count(setostr *) noex ;
extern int setostr_curbegin(setostr *,setostr_cur *) noex ;
extern int setostr_curenum(setostr *,setostr_cur *,cchar **) noex ;
extern int setostr_curend(setostr *,setostr_cur *) noex ;
extern int setostr_finish(setostr *) noex ;

extern int setostr_loadfile(setostr *,int,cchar *) noex ;

EXTERNC_end

#ifdef	__cplusplus

template<typename ... Args>
inline int setostr_magic(setostr *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = SR_NOTOPEN ;
	    if (op->magic == SETOSTR_MAGIC) {
		rs = SR_BUGCHECK ;
		if (op->setp != nullptr) {
		    rs = SR_OK ;
		}
	    }
	}
	return rs ;
}

#endif /* __cplusplus */


#endif /* SETOSTR_INCLUDE */


