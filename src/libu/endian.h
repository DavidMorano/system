/* endian HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* machine endian management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A�D� Morano
        This code was originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	This module provides machine endian managemnt.

*******************************************************************************/

#ifndef ENDIAN_INCLUDE
#define ENDIAN_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
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


extern int	endianval ;

extern cchar	*endianstr ;


#ifdef	__cplusplus

#include	<bit>			/* |endian(3c++)| */

struct machendianinfo {
	cbool islittle	= (std::endian::native == std::endian::little) ;
	cbool isbig	= (std::endian::native == std::endian::big) ;
} ;

constexpr machendianinfo	machendian ;

#endif /* __cplusplus */


#endif /* ENDIAN_INCLUDE */


