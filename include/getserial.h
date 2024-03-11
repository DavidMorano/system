/* getserial HEADER */
/* lang=C20 */

/* subroutines to access the 'passwd' and 'group' databases */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETSERIAL_INCLUDE
#define	GETSERIAL_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>


EXTERNC_begin

extern int	getserial(cchar *) noex ;

EXTERNC_end


#endif /* GETSERIAL_INCLUDE */


