/* vstrkeycmpx HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* v-string key-comparison functions */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	VSTRKEYCMPX_INCLUDE
#define	VSTRKEYCMPX_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


EXTERNC_begin

typedef int (*vcmp_f)(cchar **,cchar **) noex ;

extern void	vstrsort(cchar **,int,vcmp_f) noex ;

EXTERNC_end


#endif /* VSTRKEYCMPX_INCLUDE */


