/* conslog HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* send log messages to the system logger device */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CONSLOG_INCLUDE
#define	CONSLOG_INCLUDE

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/log.h>
#include	<sys/strlog.h>
#include	<sys/syslog.h>
#include	<stdarg.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<vecstr.h>


#define	CONSLOG_MAGIC		0x13f3c201
#define	CONSLOG			struct conslog_head
#define	CONSLOG_LINELEN		80


struct conslog_head {
	time_t		ti_open ;
	time_t		ti_write ;
	uint		magic ;
	int		logfac ;
	int		lfd ;
	int		c ;
} ;

typedef CONSLOG		conslog ;

EXTERNC_begin

extern int conslog_open(conslog *,int) noex ;
extern int conslog_write(conslog *,int,cchar *,int) noex ;
extern int conslog_printf(conslog *,int,cchar *,...) noex ;
extern int conslog_vprintf(conslog *,int,cchar *,va_list) noex ;
extern int conslog_check(conslog *,time_t) noex ;
extern int conslog_count(conslog *) noex ;
extern int conslog_close(conslog *) noex ;

EXTERNC_end


#endif /* conslog_INCLUDE */


