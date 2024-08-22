/* strxcmp HEADER */
/* lang=C20 */

/* define file types according to the S_IFMT field of |mode_t| */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STRXCMP_INCLUDE
#define	STRXCMP_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>


EXTERNC_begin

extern int	strnkeycmp(cchar *,cchar *,int) noex ;
extern int	strnvaluecmp(cchar *,cchar *,int) noex ;

EXTERNC_end


#endif /* STRXCMP_INCLUDE */


