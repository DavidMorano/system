/* hello HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* HELLO loadable service for MFSERVE */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	HELLO_INCLUDE
#define	HELLO_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<pthread.h>
#include	<ptm.h>
#include	<ptc.h>
#include	<vecpstr.h>

#include	"sreq.h"


#define	HELLO			struct hello_head
#define	HELLO_FL		struct hello_flags
#define	HELLO_MAGIC		0x88773424


struct hello_flags {
	uint		args:1 ;
	uint		working:1 ;
} ;

struct hello_head {
	mainv		envv ;
	cchar		*pr ;
	SREQ		*jep ;
	ptm		m ;		/* mutex */
	ptc		c ;		/* condition variable */
	vecpstr		args ;
	pid_t		pid ;
	pthread_t	tid ;
	volatile int	f_abort ;	/* command from parent thread */
	volatile int	f_exiting ;	/* thread is exiting */
	HELLO_FL	fl ;
	uint		magic ;
	int		ifd ;
	int		ofd ;
} ; /* end struct (hello_head) */

EXTERNC_begin

extern int hello_start(HELLO *,cchar *,SREQ *,cchar **,cchar **) noex ;
extern int hello_check(HELLO *) noex ;
extern int hello_abort(HELLO *) noex ;
extern int hello_finish(HELLO *) noex ;

EXTERNC_end


#endif /* HELLO_INCLUDE */


