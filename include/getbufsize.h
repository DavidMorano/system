/* getbufsize INCLUDE */
/* lang=C20 */

/* interface for getting system buffer sizes */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright  2001 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	This file provides the interface for retrieving system
	buffer sizes.  These sizes are determined dynamically at
	run-time but are otherwise expected to remain constant for
	the life of any process.


*******************************************************************************/

#ifndef	GETBUFSIZE_INCLUDE
#define	GETBUFSIZE_INCLUDE


#include	<envstandards.h>
#include	<getbufsizenames.h>
#include	<localmisc.h>


#define	GETBUFSIZE_CONF		"/etc/default/bufsize"
#define	GETBUFSIZE_DEFVAL	(4*1024)


#ifdef	__cplusplus
extern "C" {
#endif

extern int	getbufsize(int) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* GETBUFSIZE_INCLUDE */


