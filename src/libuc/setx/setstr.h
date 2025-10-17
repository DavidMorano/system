/* setstr HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* a set object for c-strings */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SETSTR_INCLUDE
#define	SETSTR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>
#include	<hdb.h>


#define	SETSTR		hdb
#define	SETSTR_CUR	hdb_cur


typedef SETSTR		setstr_head ;
typedef SETSTR_CUR	setstr_cur ;

#ifdef	__cplusplus
enum setstrmems {
    	setstrmem_start,
	setstrmem_delall,
	setstrmem_count,
	setstrmem_finish,
	setstrmem_overlast
} ;
struct setstr ;
struct setstr_co {
	setstr		*op = nullptr ;
	int		w = -1 ;
	void operator () (setstr *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	int operator () (int = 0) noex ;
	operator int () noex {
	    return operator () () ;
	} ;
} ; /* end struct (setstr_co) */
struct setstr : setstr_head {
	setstr_co	start ;
	setstr_co	delall ;
	setstr_co	count ;
	setstr_co	finish ;
	bool		fopen{} ;
	setstr() noex {
	    start(this,setstrmem_start) ;
	    delall(this,setstrmem_delall) ;
	    count(this,setstrmem_count) ;
	    finish(this,setstrmem_finish) ;
	} ;
	setstr(const setstr &) = delete ;
	setstr &operator = (const setstr &) = delete ;
	int already(cchar *,int = -1) noex ;
	int add(cchar *,int = -1) noex ;
	int del(cchar *,int = -1) noex ;
	int curbegin(setstr_cur *) noex ;
	int curenum(setstr_cur *,cchar **) noex ;
	int curend(setstr_cur *) noex ;
	void dtor() noex ;
	operator int () noex ;
	destruct setstr() {
	    if (fopen) dtor() ;
	} ;
} ; /* end struct (setstr) */
#else	/* __cplusplus */
typedef SETSTR		setstr ;
#endif /* __cplusplus */

EXTERNC_begin

extern int setstr_start		(setstr *,int) noex ;
extern int setstr_already	(setstr *,cchar *,int) noex ;
extern int setstr_add		(setstr *,cchar *,int) noex ;
extern int setstr_del		(setstr *,cchar *,int) noex ;
extern int setstr_delall	(setstr *) noex ;
extern int setstr_count		(setstr *) noex ;
extern int setstr_curbegin	(setstr *,setstr_cur *) noex ;
extern int setstr_curenum	(setstr *,setstr_cur *,cchar **) noex ;
extern int setstr_curend	(setstr *,setstr_cur *) noex ;
extern int setstr_finish	(setstr *) noex ;

EXTERNC_end


#endif /* SETSTR_INCLUDE */


