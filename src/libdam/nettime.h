/* nettime HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* program to get time from a network time server host */
/* version %I% last-modified %G% */


/* Copyright © 2009 David A­D­ Morano.  All rights reserved. */

#ifndef	NETTIME_INCLUDE
#define	NETTIME_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* system types */
#include	<sys/time.h>		/* system-time related types */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	NETTIME		struct nettime_head


struct nettime_head {
	TIMEVAL		trip ;		/* one-round-trip time */
	TIMEVAL		off ;		/* offset between net and us */
	int		proto ;
	int		pf ;
} ;

typedef	NETTIME		nettime ;

EXTERNC_begin

extern int getnettime(nettime *,int,int,cchar *,cchar *,int) noex ;

EXTERNC_end


#endif /* NETTIME_INCLUDE */


