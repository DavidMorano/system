/* egs HEADER */
/* encoding=ISO8859-1 */
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
#include	<usystem.h>		/* system types */


/* object defines */
#define	EGS_DEFFILE	"/tmp/entropy"
#define	EGS_MAGIC	0x93847561
#define	EGS		struct egs_head


struct egs_head {
	uint		magic ;
	pid_t		pid ;		/* daemon PID */
	int		fd ;		/* socket file descriptor */
} ;

typedef EGS		egs ;

EXTERNC_begin

extern int egs_open(egs *,cchar *) noex ;
extern int egs_read(egs *,char *,int) noex ;
extern int egs_write(egs *,cchar *,int) noex ;
extern int egs_level(egs *) noex ;
extern int egs_getpid(egs *,pid_t *) noex ;
extern int egs_close(egs *) noex ;

EXTERNC_end


#endif /* EGS_INCLUDE */


