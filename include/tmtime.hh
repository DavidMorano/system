/* tmtime HEADER */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* this is supposed to provide OS-independent time management operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	TMTIME_INCLUDE
#define	TMTIME_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	TMTIME			tmtime
#define	TMTIME_BASEYEAR		1900


struct tmtime {
	char	*zname{} ;	/* time-zone name abbreviation (allocated) */
	int	sec ;		/* 0-61 (for up to two leap-seconds) */
	int	min ;		/* 0-59 */
	int	hour ;		/* 0-23 */
	int	mday ;		/* month-day (day-of-month 1-31) */
	int	mon ;		/* month 0-11 */
	int	year ;		/* number of years since 1900 */
	int	wday ;		/* week-day (day-of-week 0-6) */
	int	yday ;		/* year-day (day-of-year) */
	int	isdst ;
	int	gmtoff ;	/* offset from GMT (seconds west of GMT) */
	cint	baseyear = TMTIME_BASEYEAR ;
	tmtime() = default ;
	tmtime &operator = (const tmtime &) = delete ;
	tmtime(const tmtime &) = delete ;
	int insert(CTM *) noex ;
	int ztime(bool,time_t) noex ;
	int gmtime(time_t) noex ;
	int localtime(time_t) noex ;
	int extract(TM *) noex ;
	int mktime(time_t *) noex ;
	int adjtime(time_t *) noex ;
	void dtor() noex ;
	destruct tmtime() {
	    if (zname) dtor() ;
	} ;
} ; /* end struct (tmtime) */

EXTERNC_begin

extern int	tmtime_insert(tmtime *,CTM *) noex ;
extern int	tmtime_ztime(tmtime *,bool,time_t) noex ;
extern int	tmtime_gmtime(tmtime *,time_t) noex ;
extern int	tmtime_localtime(tmtime *,time_t) noex ;
extern int	tmtime_extract(tmtime *,TM *) noex ;
extern int	tmtime_mktime(tmtime *,time_t *) noex ;
extern int	tmtime_adjtime(tmtime *,time_t *) noex ;

#ifdef	COMMENT
extern int	tmtime_setznoe(tmtime *,cchar *,int) noex ;
extern int	mktime_settimez(tmtime *,cchar *,cchar *,time_t) noex ;
extern int	mktime_gettime(tmtime *,cchar *,time_t *) noex ;
#endif /* COMMENT */

EXTERNC_end


#endif /* __cpluplus */
#endif /* TMTIME_INCLUDE */


