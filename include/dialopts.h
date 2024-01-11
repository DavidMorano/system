/* dialopts HEADER */
/* lang=C20 */

/* set options on dialer file-descriptor */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	DIALOPTS_INCLUDE
#define	DIALOPTS_INCLUDE


#define	DIALOPT_NONE		0x00
#define	DIALOPT_KEEPALIVE	(1<<0)
#define	DIALOPT_LINGER		(1<<1)
#define	DIALOPT_LONG		(1<<2)	/* for |dialfinger(3uc)| */
#define	DIALOPT_PWD		(1<<3)	/* same directory as PWD */
#define	DIALOPT_NOLINGER	(1<<4)	/* don't linger */
#define	DIALOPT_NOLIGHT		(1<<5)	/* don't go into "light" mode */
#define	DIALOPT_ERRCHAN		(1<<6)	/* separate error channel */
#define	DIALOPT_REUSE		(1<<7)	/* reuse address */
#define	DIALOPT_WAIT		(1<<8)	/* wait for available node */
#define	DIALOPT_WTIMED		(1<<9)	/* use the time-out for waiting */
#define	DIALOPT_EMPTY		(1<<10)	/* use an empty machine first */
#define	DIALOPT_NOCHECK		(1<<11)	/* don't check cluster DB */


EXTERNC_begin

extern int	dialopts(int,int) noex ;

EXTERNC_end


#endif /* DIALOPTS_INCLUDE */


