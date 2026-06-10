/* mkbangname HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* make (or get) a GECOS name out of the raw PASSWD c-string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mkbangname

	Description:
	This subroutine create a name that combines the nodename
	and the username into a "bangname."

	Synopsis:
	int mkuibang(char *nbuf,int nlen,userinfo *uip) noex

	Arguments:
	nbuf		buffer to receive resulting name
	nlen		length of supplied buffer
	uip		pointer to USERINFO object

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#ifndef	MKBANGNAME_INCLUDE
#define	MKBANGNAME_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<userinfo.h>		/* LIBUC <- money shot */


EXTERNC_begin

extern int	mkbangname(char *,int,userinfo *) noex ;

EXTERNC_end


#endif /* MKBANGNAME_INCLUDE */


