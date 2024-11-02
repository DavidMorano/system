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
#define	DAYOFMONTH_NMONS	12


struct dayofmonth_month {
	signed char	days[6][7] ;
} ;

struct dayofmonth_head {
	DAYOFMONTH_MON	*months[DAYOFMONTH_NMONS] ;
	uint		magic ;
	int		year ;
	int		isdst ;
	int		gmtoff ;
} ;

typedef	DAYOFMONTH	dayofmonth ;
typedef	DAYOFMONTH_MON	dayofmonth_mon ;

EXTERNC_begin

extern int dayofmonth_start(dayofmonth *,int) noex ;
extern int dayofmonth_lookup(dayofmonth *,int,int,int) noex ;
extern int dayofmonth_finish(dayofmonth *) noex ;

EXTERNC_end


#endif /* DAYOFMONTH_INCLUDE */


