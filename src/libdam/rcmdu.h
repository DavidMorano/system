/* rcmdu HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* get connection to remote command (unpriviledged) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	rcmdu

	Description:
	This is a dialer to use the underlying RSH program to make
	a "SHELL" remote connection to another machine.

	Synopsis:
	int rcmdu(cchar *rhost,cchar *ruser,cchar *cmd,int *fd2p) noex

	Arguments:
	rhost		remote hostname
	rsuer		remote username
	cmd		remote command
	fd2p		optional pointer to integer to receive STDERR

	Rerturns:
	-		file descriptor to remove command

*******************************************************************************/

#ifndef	RCMDU_INCLUDE
#define	RCMDU_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


EXTERNC_begin

extern int	rcmdu(cchar *,cchar *,cchar *,int *) noex ;

EXTERNC_end


#endif /* RCMDU_INCLUDE */


