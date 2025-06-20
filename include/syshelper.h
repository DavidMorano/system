/* syshelper HEADER */
/* charset=ISO8859-1 */
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


#define	SYSHELPER_MAGIC		0x93847561
#define	SYSHELPER		struct syshelper_head
#define	SYSHELPER_DEFFILE	"/tmp/syshelper"


struct syshelper_head {
	uint		magic ;
	int		fd ;		/* socket file descriptor */
	int		pid ;		/* daemon PID */
} ;

typedef SYSHELPER	syshelper ;

EXTERNC_begin

extern int syshelper_start(syshelper *,char *) noex ;
extern int syshelper_getpid(syshelper *,pid_t *) noex ;
extern int syshelper_finish(syshelper *) noex ;
extern int syshelper_localtime(syshelper *,time_t,TM *) noex ;
extern int syshelper_random(syshelper *,char *,int) noex ;

EXTERNC_end


#endif /* SYSHELPER_INCLUDE */


