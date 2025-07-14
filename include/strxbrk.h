/* strxbrk HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* c-string comparisons */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRXBRK_INCLUDE
#define	STRXBRK_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<string.h>		/* |strchr(3c)| + |strpbrk(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


EXTERNC_begin

extern char	*strobrk(cchar *,cchar *) noex ;
extern char	*strrbrk(cchar *,cchar *) noex ;

EXTERNC_end


#endif /* STRXBRK_INCLUDE */


