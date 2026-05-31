/* mkmid HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* create a mail message ID */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mkmid

	Description:
	This subroutine is used to create a mail message ID for
	certain PCS programs.

	Synopsis:
	int mkmid(char *rbuf,int rlen,cc *dn,cc *nn,pid_t pid,int serial) noex

	Arguments:
	rbuf		caller-supplied buffer to place result in
	rlen		length of caller supplied buffer
	dn		domain-name
	nn		node-name
	pid		process ID
	serial		serial number

	Returns:
	>=0		length of returned ID

*******************************************************************************/

#ifndef	MKMID_INCLUDE
#define	MKMID_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int	mkmid(char *,int,cchar *,cchar *,pid_t,int) noex ;

EXTERNC_end


#endif /* MKMID_INCLUDE */


