/* logsys HEADER */
/* lang=C20 */

/* send log messages to the system logger device */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	LOGSYS_INCLUDE
#define	LOGSYS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/log.h>
#include	<sys/strlog.h>
#include	<sys/syslog.h>
#include	<stdarg.h>
#include	<vecstr.h>
#include	<localmisc.h>


#define	LOGSYS_MAGIC		0x13f3c201
#define	LOGSYS			struct logsys_head
#define	LOGSYS_LOGIDLEN		15
#define	LOGSYS_LINELEN		80
#define	LOGSYS_USERLEN		(LOGSYS_LINELEN - (LOGSYS_LOGIDLEN + 1))


struct logsys_head {
	cchar		*logtag ;
	time_t		ti_open ;
	time_t		ti_write ;
	uint		magic ;
	int		logfac ;
	int		opts ;
	int		lfd ;
	int		logidlen ;
	int		n, c ;
	char		logid[LOGSYS_LOGIDLEN + 1] ;
} ;

typedef LOGSYS		logsys ;

EXTERNC_begin

extern int logsys_open(LOGSYS *,int,cchar *,cchar *,int) noex ;
extern int logsys_setid(LOGSYS *,cchar *) noex ;
extern int logsys_write(LOGSYS *,int,cchar *,int) noex ;
extern int logsys_printf(LOGSYS *,int,cchar *,...) noex ;
extern int logsys_vprintf(LOGSYS *,int,cchar *,va_list) noex ;
extern int logsys_check(LOGSYS *,time_t) noex ;
extern int logsys_flush(LOGSYS *) noex ;
extern int logsys_close(LOGSYS *) noex ;

EXTERNC_end


#endif /* LOGSYS_INCLUDE */


