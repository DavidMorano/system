/* listenudp HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* listen on a UNIX® Socket-Stream port w/ a socket */
/* version %I% last-modified %G% */


/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	LISTENUDP_INCLUDE
#define	LISTENUDP_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */

EXTERNC_begin

extern int	listenudp(cchar *,mode_t,int) noex ;

EXTERNC_end


#endif /* LISTENUDP_INCLUDE */


