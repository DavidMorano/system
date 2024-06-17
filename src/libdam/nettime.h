/* nettime HEADER */
/* lang=C20 */

/* program to get time from a network time server host */
/* version %I% last-modified %G% */


/* Copyright © 2009 David A­D­ Morano.  All rights reserved. */

#ifndef	NETTIME_INCLUDE
#define	NETTIME_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/time.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>


#define	NETTIME		struct nettime_head


struct nettime_head {
	TIMEVAL		trip ;		/* one-round-trip time */
	TIMEVAL		off ;		/* offset between net and us */
	int		proto ;
	int		pf ;
} ;

typedef	NETIME		nettime ;

EXTERNC_begin

extern int nettime(nettime *,int,int,cchar *,cchar *,int) noex ;

EXTERNC_end


#endif /* NETTIME_INCLUDE */


