/* matstrm HEADER (Match Prefix String) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* Prefix-Match a String */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MATSTRM_INCLUDE
#define	MATSTRM_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_beg
    extern int matstrmbc(con mainv,int,cchar *,int) noex ;
    extern int matstrmic(con mainv,int,cchar *,int) noex ;
    extern int matstrmfc(con mainv,int,cchar *,int) noex ;
EXTERNC_end

EXTERNC_beg
    local inline int matstrm(con mainv bs,int n,cchar *sp,int sl) noex {
	return matstrmbc(bs,n,sp,sl) ;
    } /* end */
EXTERNC_end

#ifdef	__cplusplus
    inline int matstrm(con mainv bs,int n,cchar *sp) noex {
	return matstrmbc(bs,n,sp,-1) ;
    } /* end */
#endif /* __cplusplus */


#endif /* MATSTRM_INCLUDE */


