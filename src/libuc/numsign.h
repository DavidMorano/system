/* numsign HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* number sign access */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	NUMSIGN_INCLUDE
#define	NUMSIGN_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<limits.h>		/* CSTD |CHAR_BIT| */
#include	<stddef.h>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<stdintx.h>		/* LIBU */


#ifdef	__cplusplus

template<typename T>
constexpr bool numsign(T v) noex {
	cint	n = (CHAR_BIT * szof(T)) ;
	return bool((v >> (n - 1)) & 1) ;
} /* end subroutine-template */

#endif /* __cplusplus */

EXTERNC_begin

extern bool numsigni	(int) noex ;
extern bool numsignl	(long) noex ;
extern bool numsignll	(longlong) noex ;
extern bool numsignui	(uint) noex ;
extern bool numsignul	(ulong) noex ;
extern bool numsignull	(ulonglong) noex ;

EXTERNC_end


#endif /* NUMSIGN_INCLUDE */


