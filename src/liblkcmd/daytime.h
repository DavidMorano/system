/* daytime HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* DAYTIME loadable service for MFSERVE */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	DAYTIME_INCLUDE
#define	DAYTIME_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* system-types */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<pthread.h>
#include	<vecpstr.h>
#include	<sreq.h>

#include	"sreq.h"


#define	DAYTIME			struct daytime_head
#define	DAYTIME_FL		struct daytime_flags
#define	DAYTIME_MAGIC		0x88773424


struct daytime_flags {
	uint		args:1 ;
	uint		working:1 ;
} ;

struct daytime_head {
	cchar		*pr ;
	mainv		envv ;
	SREQ		*jep ;
	vecpstr		args ;
	volatile int	f_abort ;	/* command from parent thread */
	volatile int	f_exiting ;	/* thread is exiting */
	pid_t		pid ;
	pthread_t	tid ;
	DAYTIME_FL	fl ;
	uint		magic ;
	int		ifd ;
	int		ofd ;
} ;

typedef	DAYTIME		daytime ;
typedef	DAYTIME_FL	daytime_fl ;

EXTERNC_begin

extern int daytime_start(daytime *,cchar *,SREQ *,mainv,mainv) noex ;
extern int daytime_check(daytime *) noex ;
extern int daytime_abort(daytime *) noex ;
extern int daytime_finish(daytime *) noex ;

EXTERNC_end


#endif /* DAYTIME_INCLUDE */


