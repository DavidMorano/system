/* logfile HEADER */
/* lang=C20 */

/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-22, David A�D� Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	LOGFILE_INCLUDE
#define	LOGFILE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<stdarg.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<localmisc.h>


/* object defines */
#define	LOGFILE			struct logfile_head
#define	LOGFILE_MAGIC		0x13f3c200
#define	LOGFILE_BUFSIZE		(2 * 1024)
#define	LOGFILE_LOGSIZE		80000	/* default log-file length */
#define	LOGFILE_PERCENT		20	/* percent allowed over limit */
#define	LOGFILE_LOGIDLEN	15
#define	LOGFILE_LINELEN		80
#define	LOGFILE_FMTLEN		(LOGFILE_LINELEN - (LOGFILE_LOGIDLEN + 1))
#define	LOGFILE_USERLEN		(LOGFILE_LINELEN - (LOGFILE_LOGIDLEN + 1))

/* control codes */
#define	LOGFILE_CNOP		0
#define	LOGFILE_CSIZE		1


struct logfile_head {
	cchar		*fname ;
	char		*buf ;
	time_t		ti_open ;
	time_t		ti_data ;
	time_t		ti_write ;
	mode_t		operm ;
	uint		magic ;
	int		oflags ;
	int		lfd ;
	int		logidlen ;
	int		bufsize ;
	int		len ;		/* length of buffer filled so far */
	int		percent ;
	char		logid[LOGFILE_LOGIDLEN + 1] ;
} ;

typedef LOGFILE		logfile ;

EXTERNC_begin

extern int logfile_open(logfile *,cchar *,int,mode_t,cchar *) noex ;
extern int logfile_setid(logfile *,cchar *) noex ;
extern int logfile_write(logfile *,cchar *,int) noex ;
extern int logfile_print(logfile *,cchar *,int) noex ;
extern int logfile_printf(logfile *,cchar *,...) noex ;
extern int logfile_vprintf(logfile *,cchar *,va_list) noex ;
extern int logfile_checksize(logfile *,int) noex ;
extern int logfile_check(logfile *,time_t) noex ;
extern int logfile_flush(logfile *) noex ;
extern int logfile_chmod(logfile *,mode_t) noex ;
extern int logfile_control(logfile *,int,void *) noex ;
extern int logfile_close(logfile *) noex ;

EXTERNC_end


#endif /* LOGFILE_INCLUDE */


