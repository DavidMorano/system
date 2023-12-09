/* uinet INCLUDE */
/* lang=C20 */

/* UNIX� UINET support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A�D� Morano
	This module was originally written.

	= 2017-08-01, David A�D� Morano
	Updated for lack of interfaces in Apple-Darwin

*/

/* Copyright � 1998,2017 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	This file contains some support for the UNIX� UINET related
	operations.

*******************************************************************************/

#ifndef	UINET_INCLUDE
#define	UINET_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>


#ifndef	PF_INET4
#ifdef	PF_INET
#define	PF_INET4	PF_INET
#else
#define	PF_INET4	2
#endif
#endif

#ifndef	AF_INET4
#ifdef	AF_INET
#define	AF_INET4	AF_INET
#else
#define	AF_INET4	2
#endif
#endif

#ifndef	INET4ADDRLEN
#define	INET4ADDRLEN	sizeof(in4_addr_t)
#endif

#ifndef	INET6ADDRLEN
#define	INET6ADDRLEN	sizeof(in6_addr_t)
#endif

#ifndef	INETXADDRLEN
#define	INETXADDRLEN	MAX(INET6ADDRLEN,INET4ADDRLEN)
#endif

#ifndef	INET4_ADDRSTRLEN
#define	INET4_ADDRSTRLEN	16
#endif

#ifndef	INET6_ADDRSTRLEN
#define	INET6_ADDRSTRLEN	46	/* Solaris� says that this is 46! */
#endif

#ifndef	INETX_ADDRSTRLEN
#define	INETX_ADDRSTRLEN	MAX(INET4_ADDRSTRLEN,INET6_ADDRSTRLEN)
#endif


#endif /* UINET_INCLUDE */


