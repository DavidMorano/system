/* clanguage_sat HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* version %I% last-modified %G% */
/* C-language defines */


/* revision history:

	= 1998-02-15, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CLANGUAGESAT_INCLUDE
#define	CLANGUAGESAT_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* CSTD */
#include	<numeric>		/* C++STD */


#ifndef	SAT_SUBROUTINE
#define	SAT_SUBROUTINE
#ifndef	sat_add
#define	sat_add			std::add_sat
#define	sat_sub			std::sub_sat
#define	sat_mul			std::mul_sat
#define	sat_div			std::div_sat
#endif /* sat_add */
#endif /* SAT_SUBROUTINE */


#endif /* __cplusplus */
#endif /* CLANGUAGESAT_INCLUDE */


