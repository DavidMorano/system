/* itimerval HEADER */
/* charset=ISO8859-1 */
/* lang=C99 */

/* interval-timer object methods */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	itimerval

	Description:
	Methods for the ITIMERVAL object.

*******************************************************************************/

#ifndef	ITIMERVAL_INCLUDE
#define	ITIMERVAL_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<timeval.hh>


EXTERNC_begin

extern int itimerval_load(ITIMERVAL *,CTIMEVAL *,CTIMEVAL *) noex ;

EXTERNC_end


#endif /* ITIMERVAL_INCLUDE */


