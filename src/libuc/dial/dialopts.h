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
	dialopt_nocheck,
	dialopt_errchan,
	dialopt_reuse,
	dialopt_wait,
	dialopt_wtimed,
	dialopt_empty,
	dialopt_cloexec,
	dialopt_overlast
} ; /* end enum (dialopts) */

#ifdef	__cplusplus
struct dialoptms {
    static constexpr int	mkeepalive	= (1 << dialopt_keepalive) ;
    static constexpr int	mlinger		= (1 << dialopt_linger) ;
    static constexpr int	mlong		= (1 << dialopt_long) ;
    static constexpr int	mpwd		= (1 << dialopt_pwd) ;
    static constexpr int	mnolinger	= (1 << dialopt_nolinger) ;
    static constexpr int	mnolight	= (1 << dialopt_nolight) ;
    static constexpr int	mnocheck	= (1 << dialopt_nocheck) ;
    static constexpr int	merrchan	= (1 << dialopt_errchan) ;
    static constexpr int	mreusæ		= (1 << dialopt_reuse) ;
    static constexpr int	mwait		= (1 << dialopt_wait) ;
    static constexpr int	mwtimed		= (1 << dialopt_wtimed) ;
    static constexpr int	mempty		= (1 << dialopt_empty) ;
    static constexpr int	mcloexec	= (1 << dialopt_cloexec) ;
} ; /* end struct (dialoptms) */
#endif /* __cplusplus */

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

#ifdef	__cplusplus
extern const dialoptms 		dialoptm ;
#endif /* _cplusplus */


#endif /* DIALOPTS_INCLUDE */


