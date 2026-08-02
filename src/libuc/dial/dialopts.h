/* dialopts HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* set options on dialer file-descriptor */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	DIALOPTS_INCLUDE
#define	DIALOPTS_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


enum dialopts {
	dialopt_keepalive,
	dialopt_linger,
	dialopt_long,
	dialopt_pwd,
	dialopt_nolinger,
	dialopt_nolight,
	dialopt_errchan,
	dialopt_reuse,
	dialopt_wait,
	dialopt_wtimed,
	dialopt_empty,
	dialopt_nocheck,
	dialopt_cloexec,
	dialopt_overlast
} ; /* end enum (dialopts) */


#define	DIALOPT_NONE		0
/* keep connection alive */
#define	DIALOPT_KEEPALIVE	(1 << dialopt_keepalive)
/* make the connection linger open */
#define	DIALOPT_LINGER		(1 << dialopt_linger)
/* for |dialfinger(3uc)| */
#define	DIALOPT_LONG		(1 << dialopt_long)
/* same directory as PWD */
#define	DIALOPT_PWD		(1 << dialopt_pwd)
/* do not linger */
#define	DIALOPT_NOLINGER	(1 << dialopt_nolinger)
/* do not go into "light" mode */
#define	DIALOPT_NOLIGHT		(1 << dialopt_nolight)
/* separate error channel */
#define	DIALOPT_ERRCHAN		(1 << dialopt_errchan)
/* reuse address */
#define	DIALOPT_REUSE		(1 << dialopt_reuse)
/* wait for available node */
#define	DIALOPT_WAIT		(1 << dialopt_wait)
/* use the time-out for waiting */
#define	DIALOPT_WTIMED		(1 << dialopt_wtimed)
/* use an empty machine first */
#define	DIALOPT_EMPTY		(1 << dialopt_empty)
/* do not check cluster DB */
#define	DIALOPT_NOCHECK		(1 << dialopt_nocheck)
/* close-on-exec */
#define	DIALOPT_CLOEXEC		(1 << dialopt_cloexec)


EXTERNC_begin

extern int	dialopts(int,int) noex ;

EXTERNC_end


#endif /* DIALOPTS_INCLUDE */


