/* dialcprog HEADER */
/* lang=C20 */

/* set options on dialer file-descriptor */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	DIALCPROG_INCLUDE
#define	DIALCPROG_INCLUDE


EXTERNC_begin

extern int dialcprog(cchar *pr,cchar *node,cchar *fname,mainv av,mainv ev,
		int to,int opts,int *fd2p) noex ;

EXTERNC_end


#endif /* DIALCPROG_INCLUDE */


