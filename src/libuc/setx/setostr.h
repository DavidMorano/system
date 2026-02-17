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
#include	<usysbase.h>


#define	SETOSTR		struct setostr_head
#define	SETOSTR_CUR	struct setostr_cursor
#define	SETOSTR_MAGIC	0x97351229


constexpr uint		setostr_magicval = SETOSTR_MAGIC ;

struct setostr_cursor {
	void		*itp ;
} ; /* end struct */

struct setostr_head {
	void		*setp ;
	uint		magval ;
} ; /* end struct */

typedef SETOSTR_CUR	setostr_cur ;

#ifdef	__cplusplus
enum setostrmems {
    	setostrmem_start,
	setostrmem_delall,
	setostrmem_count,
	setostrmem_finish,
	setostrmem_overlast
} ; /* end enum */
struct setostr ;
struct setostr_ma {
        setostr	*op = nullptr ;
        void operator () (setostr *p,int) noex {
            op = p ;
        } ;
        template<typename ... Args> int operator () (Args ... ) noex ;
        operator int () noex ;
} ; /* end struct (setostr_ma) */
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
	setostr_ma	magic ;
	setostr() noex {
	    start	(this,setostrmem_start) ;
	    delall	(this,setostrmem_delall) ;
	    count	(this,setostrmem_count) ;
	    finish	(this,setostrmem_finish) ;
	    magic	(this,0) ;
	    magval = 0 ;
	} ; /* end ctor */
	setostr(const setostr &) = delete ;
	setostr &operator = (const setostr &) = delete ;
	int already	(cchar *,int = -1) noex ;
	int add		(cchar *,int = -1) noex ;
	int del		(cchar *,int = -1) noex ;
	int curbegin	(setostr_cur *) noex ;
	int curenum	(setostr_cur *,cchar **) noex ;
	int curend	(setostr_cur *) noex ;
	void dtor() noex ;
	operator int () noex ;
	destruct setostr() {
	    if (magval) dtor() ;
	} ;
} ; /* end struct (setostr) */
#else	/* __cplusplus */
typedef SETOSTR		setostr ;
#endif /* __cplusplus */


EXTERNC_begin

extern int setostr_start	(setostr *,int) noex ;
extern int setostr_already	(setostr *,cchar *,int) noex ;
extern int setostr_add		(setostr *,cchar *,int) noex ;
extern int setostr_del		(setostr *,cchar *,int) noex ;
extern int setostr_delall	(setostr *) noex ;
extern int setostr_count	(setostr *) noex ;
extern int setostr_curbegin	(setostr *,setostr_cur *) noex ;
extern int setostr_curenum	(setostr *,setostr_cur *,cchar **) noex ;
extern int setostr_curend	(setostr *,setostr_cur *) noex ;
extern int setostr_finish	(setostr *) noex ;

extern int setostr_loadfile	(setostr *,int,cchar *) noex ;

EXTERNC_end

#ifdef	__cplusplus

template<typename ... Args> 
int setostr_ma::operator () (Args ... args) noex {
        int             rs = SR_FAULT ;
        if ((... && args)) {
            rs = (op->magval == setostr_magicval) ? SR_OK : SR_NOTOPEN ;
        }
        return rs ;
} /* end method (setostr_ma::operator) */

inline setostr_ma::operator int () noex {
        return (op->magval == setostr_magicval) ? SR_OK : SR_NOTOPEN ;
} /* end method (setostr_ma::operator) */

#endif /* __cplusplus */


#endif /* SETOSTR_INCLUDE */


