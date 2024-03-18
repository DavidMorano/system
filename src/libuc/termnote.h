/* termnote HEADER */
/* lang=C20 */

/* perform terminal noticing */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	TERMNOTE_INCLUDE
#define	TERMNOTE_INCLUDE

#include	<envstandards.h>	/* MUST be first to configure */
#include	<stdarg.h>
#include	<tmpx.h>
#include	<ids.h>
#include	<logfile.h>
#include	<localmisc.h>


/* object defines */
#define	TERMNOTE_MAGIC		0x13f3c200
#define	TERMNOTE		struct termnote_head
#define	TERMNOTE_FL		struct termnote_flags
#define	TERMNOTE_BUFSIZE	LINEBUFLEN
#define	TERMNOTE_MAXLINES	20
#define	TERMNOTE_LOGSIZE	400000

/* options */
#define	TERMNOTE_OBELL	(1<<0)		/* ring terminal bell */
#define	TERMNOTE_OBIFF	(1<<1)		/* must have group-execute perm */
#define	TERMNOTE_OALL	(1<<2)		/* all users */


struct termnote_flags {
	uint		tx:1 ;
	uint		lf:1 ;
} ;

struct termnote_head {
	ids		id ;
	tmpx		tx ;
	logfile		lf ;
	cchar		*pr ;
	cchar		*nodename ;
	time_t		ti_check ;
	time_t		ti_tmpx ;
	time_t		ti_logcheck ;
	time_t		ti_write ;
	TERMNOTE_FL	init, open ;
	uint		magic ;
	int		sn ;		/* serial-number */
	char		username[USERNAMELEN+1] ;
	char		logid[LOGIDLEN+1] ;
} ;

typedef TERMNOTE	termnote ;
typedef TERMNOTE_FL	termnote_fl ;

EXTERNC_begin

extern int termnote_open(termnote *,cchar *) noex ;
extern int termnote_printf(termnote *,cchar **,int,int,cchar *,...) noex ;
extern int termnote_vprintf(termnote *,cchar **,int,int,cchar *,va_list) noex ;
extern int termnote_write(termnote *,cchar **,int,int,cchar *,int) noex ;
extern int termnote_check(termnote *,time_t) noex ;
extern int termnote_close(termnote *) noex ;

EXTERNC_end


#endif /* TERMNOTE_MASTER */

#endif /* TERMNOTE_INCLUDE */


