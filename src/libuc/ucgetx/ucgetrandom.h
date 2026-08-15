/* ucgetrnadom HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* LIBUC get some pseudo-randomo data from the operaring system */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UCGETRANDOM_INCLUDE
#define	UCGETRANDOM_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/random.h>		/* POSIX® */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


/* options for |uc_getrandom(3uc)| */
#ifndef	GRND_RANDOM
#define	GRND_RANDOM	(1 << 0)	/* option for third argument */
#endif
#ifndef	GRND_NONBLOCK
#define	GRND_NONBLOCK	(1 << 1)
#endif
#ifndef	GRND_INSECURE
#define	GRND_INSECURE	(1 << 2)
#endif

EXTERNC_begin

extern int uc_getrandom		(void *,int,uint)	noex ;
extern int uc_getentropy	(void *,int)		noex ;

EXTERNC_end


#endif /* UCGETRANDOM_INCLUDE */


