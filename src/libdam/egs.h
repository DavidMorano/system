/* egs HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* entropy-gathering-server operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	EGS_INCLUDE
#define	EGS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


/* object defines */
#define	EGS_DEFFILE	"/tmp/entropy"
#define	EGS_MAGIC	0x93847561
#define	EGS		struct egs_head


struct egs_head {
	pid_t		pid ;		/* daemon PID */
	uint		magval ;
	int		fd ;		/* socket file descriptor */
} ; /* end struct */

typedef EGS		egs ;

EXTERNC_begin

extern int egs_open	(egs *,cchar *) noex ;
extern int egs_read	(egs *,char *,int) noex ;
extern int egs_write	(egs *,cchar *,int) noex ;
extern int egs_level	(egs *) noex ;
extern int egs_getpid	(egs *,pid_t *) noex ;
extern int egs_close	(egs *) noex ;

EXTERNC_end


#endif /* EGS_INCLUDE */


