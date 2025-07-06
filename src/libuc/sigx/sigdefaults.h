/* sigdefaults HEADER */
/* charset=ISO8859-1 */
/* lang=C99 */

/* set a set of signals totheir default behavioe */
/* version %I% last-modified %G% */


/* revision history:

	= 2014-04-04, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2014 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	sigdefaults

	Description:
	Set the specified signals (as given in an array of signals)
	to their default (behavior).

*******************************************************************************/

#ifndef	SIGDEFAULTS_INCLUDE
#define	SIGDEFAULTS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>


EXTERNC_begin

extern int	sigdefaults(int *) noex ;

EXTERNC_end


#endif /* SIGDEFAULTS_INCLUDE */


