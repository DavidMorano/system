/* ucfork HEADER (LIBUC Terminal-Control) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* LIBUC Teerminal-Control functions */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	ucatexit

	Names:
	uc_atexit

	Description:
	UNIX® operations related to |fork(2)|.

  	Synopsis:
	int uc_atexit(void_f func) noex 

	Arguments:
	func		function to register

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#ifndef	UCATEXIT_INCLUDE
#define	UCATEXIT_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU |void_f(3u)| */


EXTERNC_begin

extern int uc_atexit(void_f func) noex ;

EXTERNC_end


#endif /* UCATEXIT_INCLUDE */


