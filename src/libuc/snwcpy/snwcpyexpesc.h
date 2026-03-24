/* snwcpyexpesc HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* string-index operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SNWCPYEXPESC_INCLUDE
#define	SNWCPYEXPESC_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<stddef.h>		/* <- for |wchar_t| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int snwcpyexpesc(char *,int,cchar *,int) noex ;

EXTERNC_end


#endif /* SNWCPYEXPESC_INCLUDE */


