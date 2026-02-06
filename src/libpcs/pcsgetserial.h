/* pcsgetserial HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* PCS get serial number */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	PCSGETSERIAL_INCLUDE
#define	PCSGETSERIAL_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


EXTERNC_begin

extern int 	pcsgetserial(cchar *) noex ;

EXTERNC_end


#endif /* PCSGETSERIAL_INCLUDE */


