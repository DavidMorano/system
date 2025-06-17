/* funmode HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* Function-Mode */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	FUNMODE_INCLUDE
#define	FUNMODE_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


enum struct funmode : int {
    	wr,
	rd,
	overlast
} ;


#endif /* __cplusplus */
#endif /* FUNMODE_INCLUDE */


