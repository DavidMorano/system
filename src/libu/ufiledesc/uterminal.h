/* uterminal HEADER (UNIX® file operations) */
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
	uterminal

	Description:
	All of the UNIX® system calls that use (operate on) a
	file-descroptor.

*******************************************************************************/

#ifndef	UTERMINAL_INCLUDE
#define	UTERMINAL_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>
#include	<usysbase.h>

EXTERNC_begin

extern int	u_terminal(int) noex ;
extern int	u_termattrget(int,TERMIOS *) noex ;
extern int	u_termattrset(int,int,const TERMIOS *) noex ;

EXTERNC_end


#endif /* UTERMINAL_INCLUDE */


