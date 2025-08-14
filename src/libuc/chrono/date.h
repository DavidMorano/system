/* date HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* date storage object */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	DATE_INCLUDE
#define	DATE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<time.h>		/* |time_t| */
#include	<usystem.h>


#define	DATE		struct date_head


struct date_head {
	char		*zname ;		/* time-zone abbreviation */
	time_t		time ;			/* UNIX® time */
	short		zoff ;			/* minutes west of GMT */
	short		isdst ;			/* is-daylight-savings time */
} ; /* end struct (date_head) */

#ifdef	__cplusplus
enum datemems {
	datemem_finish,
	datemem_overlast
} ;
struct date ;
struct date_co {
	date		*op = nullptr ;
	int		w = -1 ;
	void operator () (date *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (date_co) */
struct date : date_head {
	date_co	finish ;
	date() noex {
	    finish(this,datemem_finish) ;
	    zname = nullptr ;
	} ;
	date(const date &) = delete ;
	date &operator = (const date &) = delete ;
	int start(time_t,int,int,cchar *,int) noex ;
	int setzname(cchar *,int) noex ;
	int copy(date *) noex ;
	int gettime(time_t *) noex ;
	int getzoff(int *) noex ;
	int getisdst(int *) noex ;
	int getzname(char *,int) noex ;
	void dtor() noex ;
	destruct date() {
	    if (zname) dtor() ;
	} ;
} ; /* end struct (date) */
#else	/* __cplusplus */
typedef DATE		date ;
#endif /* __cplusplus */

EXTERNC_begin

extern int date_start(date *,time_t,int,int,cchar *,int) noex ;
extern int date_setzname(date *,cchar *,int) noex ;
extern int date_copy(date *,date *) noex ;
extern int date_gettime(date *,time_t *) noex ;
extern int date_getzoff(date *,int *) noex ;
extern int date_getisdst(date *,int *) noex ;
extern int date_getzname(date *,char *,int) noex ;
extern int date_finish(date *) noex ;

EXTERNC_end


#endif /* DATE_INCLUDE */


