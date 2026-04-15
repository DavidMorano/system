/* convdec HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* convert a long (signed or unsigned) integer to its decimal string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CONVDEC_INCLUDE
#define	CONVDEC_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


EXTERNC_begin

extern char	*convdecs(long,		char *) noex ;
extern char	*convdecu(ulong,	char *) noex ;

EXTERNC_end


#endif /* CONVDEC_INCLUDE */


