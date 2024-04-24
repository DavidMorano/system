/* conslog HEADER */
/* lang=C++20 */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CONSLOG_INCLUDE
#define	CONSLOG_INCLUDE

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/log.h>
#include	<sys/strlog.h>
#include	<sys/syslog.h>
#include	<stdarg.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<vecstr.h>
#include	<localmisc.h>


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

extern int conslog_open(CONSLOG *,int) noex ;
extern int conslog_write(CONSLOG *,int,cchar *,int) noex ;
extern int conslog_printf(CONSLOG *,int,cchar *,...) noex ;
extern int conslog_vprintf(CONSLOG *,int,cchar *,va_list) noex ;
extern int conslog_check(CONSLOG *,time_t) noex ;
extern int conslog_count(CONSLOG *) noex ;
extern int conslog_close(CONSLOG *) noex ;

EXTERNC_end


#endif /* CONSLOG_INCLUDE */


