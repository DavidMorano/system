/* getexecname SUPPORT */
/* lang=C++20 */

/* get the file-name (file-path) that was used to 'exec' this program */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-16, David A­D­ Morano
	This subroutine was written so that we could use a single
	file-system directory interface due to all of the changes
	in the POSIX standard and previous UNIX® standards.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getexecname

	Description:
	This subroutine returns the name (file-path) by which the
	current program was 'exec'ed.

	Synopssi:
	cchar *getexecname() noex

	Returns:
	-		the exec-name pointer or NULL (if not found)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getprogname(3c)| */
#include	<usystem.h>
#include	<localmisc.h>
#include	<getexecname.h>

#if	defined(SYSHAS_Darwin) && (SYSHAS_Darwin > 0)

cchar *getexecname() noex {
	return getprogname() ;
}
/* end subroutine (getexecname) */

#endif /* defined(SYSHAS_Darwin) && (SYSHAS_Darwin > 0) */


