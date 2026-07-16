/* utypedefs_cuintpp HEADER (Integers-UnSigned-Constant) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* virtual-system definitions */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

#ifndef	UTYPEDEFSCUINTPP_INCLUDE
#define	UTYPEDEFSCUINTPP_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usysnative.h>		/* useful native system headers */
#include	<clanguage.h>		/* <- necessary inclunsion */
#include	<stdintx.h>		/* extended integer types */
#include	<syspredefined.h>	/* system pre-defined types */


#ifndef	TYPEDEF_CUCHARPP
#define	TYPEDEF_CUCHARPP
typedef const unsigned char **	cucharpp ;
#endif /* TYPEDEF_CUCHARPP */

#ifndef	TYPEDEF_CUSHORTPP
#define	TYPEDEF_CUSHORTPP
typedef const unsigned short **	cushortpp ;
#endif /* TYPEDEF_CUSHORTPP */

#ifndef	TYPEDEF_CUINTPP
#define	TYPEDEF_CUINTPP
typedef const unsigned int **	cuintpp ;
#endif /* TYPEDEF_CUINTPP */

#ifndef	TYPEDEF_CULONGPP
#define	TYPEDEF_CULONGPP
typedef const unsigned long **	culongpp ;
#endif /* TYPEDEF_CULONGPP */


#endif /* UTYPEDEFSCUINTPP_INCLUDE */


