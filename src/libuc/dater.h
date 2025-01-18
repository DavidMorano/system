/* dater HEADER */
/* encoding=ISO8859-1 */
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
#include	<sys/timeb.h>		/* for 'struct timeb' */
#include	<usystem.h>
#include	<snx.h>
#include	<strwcpy.h>
#include	<date.h>
#include	<localmisc.h>		/* |TZABBRLEN| */


/* object defines */
#define	DATER_MAGIC		0x26213711
#define	DATER			struct dater_head
#define	DATER_FL		struct dater_flags
#define	DATER_ZI		struct dater_zinfo
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
	ushort 		millitm ;	/* 1000 msec of additional accuracy */
	short		timezone ;	/* timezone, minutes west of GMT */
	short		dstflag ;	/* DST flag */
} ;
#endif /* COMMENT */

struct dater_zinfo {
	int		zoff ;		/* minutes west of GMT */
	int		isdst ;
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
	char		*cname ;
	char		*zname ;
	TIMEB		cb ;		/* current */
	TIMEB		b ;
	DATER_FL	fl ;
	uint		magic ;
	int		znamelen ;	/* public variable */
	short		cyear ;		/* current */
} ;

typedef DATER		dater ;
typedef	DATER_FL	dater_fl ;
typedef	DATER_ZI	dater_zi ;

EXTERNC_begin

extern int dater_start(dater *,TIMEB *,cchar *,int) noex ;
extern int dater_startcopy(dater *,dater *) noex ;
extern int dater_setcopy(dater *,dater *) noex ;
extern int dater_setstd(dater *,cchar *,int) noex ;
extern int dater_setmsg(dater *,cchar *,int) noex ;
extern int dater_setstrdig(dater *,cchar *,int) noex ;
extern int dater_setlogz(dater *,cchar *,int) noex ;
extern int dater_settouch(dater *,cchar *,int) noex ;
extern int dater_settoucht(dater *,cchar *,int) noex ;
extern int dater_settmzon(dater *,TM *,int,cchar *,int) noex ;
extern int dater_settmzo(dater *,TM *,int) noex ;
extern int dater_settmzn(dater *,TM *,cchar *,int) noex ;
extern int dater_settimezn(dater *,time_t,cchar *,int) noex ;
extern int dater_settimezon(dater *,time_t,int,cchar *,int) noex ;
extern int dater_setzinfo(dater *,dater_zi *,cc *,int) noex ;
extern int dater_tzinfo(dater *,dater_zi *) noex ;
extern int dater_mkdatestr(dater *,int,char *,int) noex ;
extern int dater_mkstd(dater *,char *,int) noex ;
extern int dater_mkenv(dater *,char *,int) noex ;
extern int dater_mkmsg(dater *,char *,int) noex ;
extern int dater_mkhdr(dater *,char *,int) noex ;
extern int dater_mkstrdig(dater *,char *,int) noex ;
extern int dater_mklogz(dater *,char *,int) noex ;
extern int dater_mkgmtlogz(dater *,char *,int) noex ;
extern int dater_gettime(dater *,time_t *) noex ;
extern int dater_getzoneoff(dater *,int *) noex ;
extern int dater_getzonename(dater *,char *,int) noex ;
extern int dater_zinfoget(dater *,dater_zi *,char *,int) noex ;
extern int dater_zinfoset(dater *,dater_zi *,cchar *,int) noex ;
extern int dater_diff(dater *,dater *,time_t *) noex ;
extern int dater_finish(dater *) noex ;

extern int dater_getdate(dater *,date *) noex ;
extern int dater_getbbtime(dater *,cchar *,int,time_t *) noex ;
extern int dater_setkey(dater *,cc *,int,TIMEB *,cc *) noex ;

#ifdef	COMMENT
extern int dater_nzones(dater *) noex ;
extern int dater_zinfo(dater *,dater_zi *,int) noex ;
#endif /* COMMENT */

EXTERNC_end

#ifdef	__cplusplus

template<typename ... Args>
static inline int dater_magic(dater *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == DATER_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}

#endif /* __cplusplus */


#endif /* DATER_INCLUDE */


