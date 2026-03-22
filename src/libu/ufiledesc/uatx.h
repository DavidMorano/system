/* uatx HEADER (UNIX® file operations) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:

	Description:
	All of the UNIX® system calls that use (operate on) a
	file-descroptor.

*******************************************************************************/

#ifndef	UATX_INCLUDE
#define	UATX_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>
#include	<usysbase.h>

EXTERNC_begin

extern int u_ataccess	(int,cchar *,int,int) noex ;
extern int u_atperm	(int,cchar *,int) noex ;
extern int u_atlink	(int,cchar *,int,cchar *,int) noex ;

local inline int u_linkat(int fd,cc *fn,int dfd,cc *dfn,int fl) noex {
    	return u_atlink(fd,fn,dfd,dfn,fl) ;
}

EXTERNC_end


#endif /* UATX_INCLUDE */


