/* matstr HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* UNIX® kernel support subroutines */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-26, David A­D­ Morano
	This was first written to give a little bit to UNIX® what
	we have in our own circuit-pack OSes!

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MATSTRX_INCLUDE
#define	MATSTRX_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


EXTERNC_beg
    extern int matstrbc		(con mainv,cchar *,int) noex ;
    extern int matstric		(con mainv,cchar *,int) noex ;
    extern int matstrfc		(con mainv,cchar *,int) noex ;
EXTERNC_end

EXTERNC_beg
    inline int matstr		(con mainv bs,cchar *sp,int sl) noex {
	return matstrbc(bs,sp,sl) ;
    } /* end subroutine */
EXTERNC_end

#ifdef	__cplusplus
    inline int matstrbc		(con mainv bs,cchar *sp) noex {
	return matstrbc(bs,sp,-1) ;
    } /* end */
    inline int matstric		(con mainv bs,cchar *sp) noex {
	return matstric(bs,sp,-1) ;
    } /* end */
    inline int matstrfc		(con mainv bs,cchar *sp) noex {
	return matstrfc(bs,sp,-1) ;
    } /* end */
#endif /* __cplusplus */

#ifdef	__cplusplus
    inline int matstr		(con mainv bs,cchar *sp) noex {
	return matstrbc(bs,sp,-1) ;
    } /* end subroutine */
#endif /* __cplusplus */


#endif /* MATSTRX_INCLUDE */


