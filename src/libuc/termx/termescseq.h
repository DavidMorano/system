/* termescseq HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* Terminal-Control-Sequene */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	TERMESCSEQ_INCLUDE
#define	TERMESCSEQ_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int	termescseq(char *,int,int,int,int,int,int) noex ;

EXTERNC_end


#endif /* TERMESCSEQ_INCLUDE */


