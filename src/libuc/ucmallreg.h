/* ucmallreg HEADER */
/* lang=C20 */

/* support object for UNIX® C-language library Memory-Allocation management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This include provides the array indices for accessing the
	various memory-allocation information elements.

*******************************************************************************/

#ifndef	UCMALLREG_INCLUDE
#define	UCMALLREG_INCLUDE


#include	<envstandards.h>
#include	<usystem.h>


#define		UCMALLREG_CUR	struct ucmallreg_cur
#define		UCMALLREG_REG	struct ucmallreg_reg


enum ucmallregs {
	ucmallreg_used,
	ucmallreg_usedmax,
	ucmallreg_outnum,
	ucmallreg_outnummax,
	ucmallreg_outsize,
	ucmallreg_outsizemax,
	ucmallreg_under,
	ucmallreg_over,
	ucmallreg_notalloc,
	ucmallreg_notfree,
	ucmallreg_overlast
} ;

struct ucmallreg_cur {
	int		i ;
} ;

struct ucmallreg_reg {
	caddr_t		addr ;
	size_t		size ;
} ;

EXTERNC_begin

extern int ucmallreg_curbegin(UCMALLREG_CUR *) noex ;
extern int ucmallreg_curend(UCMALLREG_CUR *) noex ;
extern int ucmallreg_enum(UCMALLREG_CUR *,UCMALLREG_REG *) noex ;

EXTERNC_end


#endif /* UCMALLREG_INCLUDE */


