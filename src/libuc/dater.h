/* dater HEADER */
/* lang=C20 */

/* dater manipulation object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-03, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	DATER_INCLUDE
#define	DATER_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/timeb.h>		/* for 'struct timeb' */
#include	<usystem.h>
#include	<date.h>
#include	<localmisc.h>


/* object defines */

#define	DATER_MAGIC	0x26213711
#define	DATER		struct dater_head
#define	DATER_FL	struct dater_flags
#define	DATER_ZINFO	struct dater_zinfo
#define	DATER_ZNAMESIZE	8		/* maximum TZ name length */

/* dater-type-strings (DTSes) */

#define	DATER_DTSSTD		0		/* email envelope */
#define	DATER_DTSENV		DATER_DTSSTD	/* email envelope */
#define	DATER_DTSHDR		1		/* email header */
#define	DATER_DTSMSG		DATER_DTSHDR	/* message header */
#define	DATER_DTSSTRDIG		2		/* string of digits */
#define	DATER_DTSLOGZ		3		/* 'logz' type */
#define	DATER_DTSGMLOGZ		4		/* 'logz' type for GMT */
#define	DATER_DTSCTIME		DATER_DTSENV	/* same as UNIX 'ctime' */
#define	DATER_DTSEND		5		/* *end* */


#ifdef	COMMENT
struct timeb {
	time_t		time ; 		/* time, seconds since the epoch */
	unsigned short 	millitm ;	/* 1000 msec of additional accuracy */
	short		timezone ;	/* timezone, minutes west of GMT */
	short		dstflag ;	/* DST flag */
} ;
#endif /* COMMENT */

struct dater_zinfo {
	int		zoff ;		/* minutes west of GMT */
	int		isdst ;
	char		zname[DATER_ZNAMESIZE + 1] ;
} ;

struct dater_flags {
	uint		zname:1 ;	/* we have a timezone name string */
	uint		zoff:1 ;	/* we have a timezone offset */
	uint		tzset:1 ;	/* has it been called? */
	uint		cb:1 ;		/* have current time-offset */
	uint		czn:1 ;		/* have current zone-name */
	uint		cyear:1 ;	/* have current year */
} ;

struct dater_head {
	TIMEB		cb ;		/* current */
	TIMEB		b ;
	DATER_FL	f ;
	uint		magic ;
	short		cyear ;		/* current */
	char		cname[DATER_ZNAMESIZE] ;
	char		zname[DATER_ZNAMESIZE] ;
} ;

#ifdef	COMMENT
typedef struct dater_head	dater ;
#endif

EXTERNC_begin

extern int dater_start(DATER *,TIMEB *,cchar *,int) noex ;
extern int dater_startcopy(DATER *,DATER *) noex ;
extern int dater_setcopy(DATER *,DATER *) noex ;
extern int dater_setstd(DATER *,cchar *,int) noex ;
extern int dater_setmsg(DATER *,cchar *,int) noex ;
extern int dater_setstrdig(DATER *,cchar *,int) noex ;
extern int dater_setlogz(DATER *,cchar *,int) noex ;
extern int dater_settouch(DATER *,cchar *,int) noex ;
extern int dater_settoucht(DATER *,cchar *,int) noex ;
extern int dater_settmzon(DATER *,TM *,int,cchar *,int) noex ;
extern int dater_settmzo(DATER *,TM *,int) noex ;
extern int dater_settmzn(DATER *,TM *,cchar *,int) noex ;
extern int dater_settimezn(DATER *,time_t,cchar *,int) noex ;
extern int dater_settimezon(DATER *,time_t,int,cchar *,int) noex ;
extern int dater_setzinfo(DATER *,DATER_ZINFO *) noex ;
extern int dater_tzinfo(DATER *,DATER_ZINFO *) noex ;
extern int dater_mkdatestr(DATER *,int,char *,int) noex ;
extern int dater_mkstd(DATER *,char *,int) noex ;
extern int dater_mkenv(DATER *,char *,int) noex ;
extern int dater_mkmsg(DATER *,char *,int) noex ;
extern int dater_mkhdr(DATER *,char *,int) noex ;
extern int dater_mkstrdig(DATER *,char *,int) noex ;
extern int dater_mklogz(DATER *,char *,int) noex ;
extern int dater_gettime(DATER *,time_t *) noex ;
extern int dater_getzoneoff(DATER *,int *) noex ;
extern int dater_getzonename(DATER *,char *,int) noex ;
extern int dater_getzinfo(DATER *,DATER_ZINFO *) noex ;
extern int dater_getdate(DATER *,DATE *) noex ;
extern int dater_diff(DATER *,DATER *,time_t *) noex ;
extern int dater_finish(DATER *) noex ;

#ifdef	COMMENT
extern int dater_nzones(DATER *) noex ;
extern int dater_zinfo(DATER *,DATER_ZINFO *,int) noex ;
#endif /* COMMENT */

EXTERNC_end


#endif /* DATER_INCLUDE */


