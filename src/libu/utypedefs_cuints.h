/* utypedefs_cuints HEADER (Integers-UnSigned-Constant) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* virtual-system definitions */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

#ifndef	UTYPEDEFSCUINTS_INCLUDE
#define	UTYPEDEFSCUINTS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usysnative.h>		/* useful native system headers */
#include	<clanguage.h>		/* <- necessary inclunsion */
#include	<stdintx.h>		/* extended integer types */
#include	<syspredefined.h>	/* system pre-defined types */


#ifndef	TYPEDEF_CUCHAR
#define	TYPEDEF_CUCHAR
typedef const unsigned char	cuchar ;
#endif /* TYPEDEF_CUCHAR */

#ifndef	TYPEDEF_CUSHORT
#define	TYPEDEF_CUSHORT
typedef const unsigned short	cushort ;
#endif /* TYPEDEF_CUSHORT */

#ifndef	TYPEDEF_CUINT
#define	TYPEDEF_CUINT
typedef const unsigned int	cuint ;
#endif /* TYPEDEF_CUINT */

#ifndef	TYPEDEF_CULONG
#define	TYPEDEF_CULONG
typedef const unsigned long	culong ;
#endif /* TYPEDEF_CULONG */


#endif /* UTYPEDEFSCUINTS_INCLUDE */


