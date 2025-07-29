/* endian HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* machine endian management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
        This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	endian

	Description:
	This module provides machine endian management.

	Notes:
	Why is the interface of this code so strange?  Because the
	interface predates this present code by a long time.  The
	original code was based on the preprocessor defines: ENDIAN
	and ENDIANSTR.  The 'ENDIAN' value returned an integer:
	0==litle-endian, 1=big-endian.  The 'ENDIANSTR' value
	returned a c-string: "0"=little-endian, "1"=big-endian.

*******************************************************************************/

#ifndef ENDIAN_INCLUDE
#define ENDIAN_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<stdint.h>		/* |uint32_t| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


#ifndef	ENDIAN
#define	ENDIAN		endianval
#endif

#ifndef	ENDIANSTR
#define	ENDIANSTR	endianstr
#endif

EXTERNC_begin
extern uint32_t		 ntohi(uint32_t) noex ;
extern uint32_t		 htoni(uint32_t) noex ;
EXTERNC_end

extern int	endianval ;

extern cchar	*endianstr ;


#ifdef	__cplusplus

#include	<bit>			/* |endian(3c++)| */

struct machendianinfo {
	static cbool islittle	= (std::endian::native == std::endian::little) ;
	static cbool isbig	= (std::endian::native == std::endian::big) ;
} ;

constexpr machendianinfo	machendian ;

#endif /* __cplusplus */


#endif /* ENDIAN_INCLUDE */


