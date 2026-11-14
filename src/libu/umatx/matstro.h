/* matstro HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* USUPPORT Match-String-Option */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MATSTRO_INCLUDE
#define	MATSTRO_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_beg
    extern int matstrobc(con mainv,int,cchar *,int) noex ;
    extern int matstroic(con mainv,int,cchar *,int) noex ;
    extern int matstrofc(con mainv,int,cchar *,int) noex ;
EXTERNC_end

EXTERNC_beg
    inline int matstro(con mainv bs,int n,cchar *sp,int sl) noex {
	return matstrobc(bs,n,sp,sl) ;
    } /* end subroutine (matstro) */
EXTERNC_end

#ifdef	__cplusplus
    inline int matstrobc	(con mainv bs,int n,cchar *sp) noex {
	return matstrobc(bs,n,sp,-1) ;
    } /* end */
    inline int matstroic	(con mainv bs,int n,cchar *sp) noex {
	return matstroic(bs,n,sp,-1) ;
    } /* end */
    inline int matstrofc	(con mainv bs,int n,cchar *sp) noex {
	return matstrofc(bs,n,sp,-1) ;
    } /* end */
#endif /* __cplusplus */

#ifdef	__cplusplus
    inline int matstro		(con mainv bs,int n,cchar *sp) noex {
	return matstrobc(bs,n,sp,-1) ;
    } /* end subroutine */
#endif /* __cplusplus */


#endif /* MATSTRO_INCLUDE */


