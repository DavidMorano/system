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


#include	<envstandards.h>	/* MUST be first to configure */


#ifndef	SAT_SUBROUTINE
#define	SAT_SUBROUTINE
#ifdef	__cplusplus
#define	sat_add			add_sat
#define	sat_sub			sub_sat
#define	sat_mul			mul_sat
#define	sat_div			div_sat
#endif /* __cplusplus */
#endif /* SAT_SUBROUTINE */


#endif /* CLANGUAGESAT_INCLUDE */


