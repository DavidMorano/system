/* matstrk HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* match the key part of a string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MATSTRK_INCLUDE
#define	MATSTRK_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_beg
    extern int	matstrk(con mainv,cchar *,int) noex ;
EXTERNC_end

#ifdef	__cplusplus
inline int matstrk(con mainv bs,cchar *sp) noex {
    	return matstrk(bs,sp) ;
} /* end */
#endif /* __cplusplus */


#endif /* MATSTRK_INCLUDE */


