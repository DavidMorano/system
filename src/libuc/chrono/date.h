/* date HEADER */
/* encoding=ISO8859-1 */
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
} ;

typedef DATE		date ;

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


