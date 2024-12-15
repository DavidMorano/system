/* dayofmonth HEADER */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* day-of-month operations (determinations) */
/* version %I% last-modified %G% */


/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	DAYOFMONTH_INCLUDE
#define	DAYOFMONTH_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	DAYOFMONTH_MAGIC	0x99447245
#define	DAYOFMONTH		struct dayofmonth_head
#define	DAYOFMONTH_MON		struct dayofmonth_month
#define	DAYOFMONTH_NMONS	12	/* very constant! */


struct dayofmonth_month {
	signed char	days[6][7] ;
} ;

struct dayofmonth_head {
	DAYOFMONTH_MON	**months ;
	uint		magic ;
	int		year ;
	int		isdst ;
	int		gmtoff ;
} ;

#ifdef	__cplusplus
enum dayofmonthmems {
	dayofmonthmem_finish,
	dayofmonthmem_overlast
} ;
struct dayofmonth ;
struct dayofmonth_co {
	dayofmonth	*op = nullptr ;
	int		w = -1 ;
	void operator () (dayofmonth *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (dayofmonth_co) */
struct dayofmonth : dayofmonth_head {
	dayofmonth_co	finish ;
	dayofmonth() noex {
	    finish(this,dayofmonthmem_finish) ;
	} ;
	dayofmonth(const dayofmonth &) = delete ;
	dayofmonth &operator = (const dayofmonth &) = delete ;
	int start(int) noex ;
	int lookup(int,int,int) noex ;
	int mkday(int,cchar *,int) noex ;
	void dtor() noex ;
	~dayofmonth() {
	    dtor() ;
	} ;
} ; /* end struct (dayofmonth) */
#else	/* __cplusplus */
typedef DAYOFMONTH	dayofmonth ;
#endif /* __cplusplus */

typedef	DAYOFMONTH_MON	dayofmonth_mon ;

EXTERNC_begin

extern int dayofmonth_start(dayofmonth *,int) noex ;
extern int dayofmonth_lookup(dayofmonth *,int,int,int) noex ;
extern int dayofmonth_mkday(dayofmonth *,int,cchar *,int) noex ;
extern int dayofmonth_finish(dayofmonth *) noex ;

EXTERNC_end


#endif /* DAYOFMONTH_INCLUDE */


