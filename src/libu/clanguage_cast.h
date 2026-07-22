/* clanguage_cast HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* C-language defines */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-15, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CLANGUAGECAST_INCLUDE
#define	CLANGUAGECAST_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */


#ifdef	__cplusplus
#ifndef	cast_static
#define	cast_static		static_cast
#define	cast_dynamic		dynamic_cast
#define	cast_const		const_cast
#define	cast_reinterpret	reinterpret_cast
#define	cast_bit		bit_cast
#define	cast_saturate		saturate_cast
#define	cast_sat		saturate_cast
#define	saturate		saturate_cast
#define	cast_any		any_cast
#endif
#endif /* __cplusplus */

#ifndef	CAST_R
#define	CAST_R			cast_reinterpret
#endif


#endif /* CLANGUAGECAST_INCLUDE */


