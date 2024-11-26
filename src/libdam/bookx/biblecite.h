/* biblecite HEADER */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* bible-citation */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */


#ifndef	BIBLECITE_INCLUDE
#define	BIBLECITE_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	BIBLECITE	struct biblecite_head


struct biblecite_head {
	uint	b, c, v ;
} ;

typedef	BIBLECITE	biblecite ;

EXTERNC_begin

extern int	isbiblecite(biblecite *,cchar *,int,int *) noex ;

EXTERNC_end


#endif /* biblecite_INCLUDE */


