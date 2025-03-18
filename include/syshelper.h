/* syshelper HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 (conformance reviewed) */

/* syshelper operations */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SYSHELPER_INCLUDE
#define	SYSHELPER_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* system types */
#include	<time.h>
#include	<usystem.h>


#define	SYSHELPER		struct syshelper_head


struct syshelper_head {
	uint	magic ;
	int	fd ;		/* socket file descriptor */
	int	pid ;		/* daemon PID */
} ;

typedef SYSHELPER	syshelper ;

EXTERNC_begin

extern int syshelper_start(SYSHELPER *,char *) noex ;
extern int syshelper_getpid(SYSHELPER *,pid_t *) noex ;
extern int syshelper_finish(SYSHELPER *) noex ;
extern int syshelper_localtime(SYSHELPER *,time_t,TM *) noex ;
extern int syshelper_random(SYSHELPER *,char *,int) noex ;

EXTERNC_end


#endif /* SYSHELPER_INCLUDE */


