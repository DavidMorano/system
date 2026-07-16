/* utypedefs_muintpp HEADER (Integer-UnSigned-Mutable) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* virtual-system definitions */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

	= 2017-08-01, David A­D­ Morano
	Updated for lack of interfaces in MacOS Darwin

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

#ifndef	UTYPEDEFSMUINTPP_INCLUDE
#define	UTYPEDEFSMUINTPP_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usysnative.h>		/* useful native system headers */
#include	<clanguage.h>		/* <- necessary inclusion */
#include	<stdintx.h>		/* extended integer types */
#include	<syspredefined.h>	/* system pre-defined types */


#ifndef	TYPEDEF_UCHARPP
#define	TYPEDEF_UCHARPP
typedef unsigned char **	ucharpp ;
#endif /* TYPEDEF_UCHARPP */

#ifndef	TYPEDEF_USHORTPP
#define	TYPEDEF_USHORTPP
typedef unsigned short **	ushortpp ;
#endif /* TYPEDEF_USHORTPP */

#ifndef	TYPEDEF_UINTPP
#define	TYPEDEF_UINTPP
typedef unsigned int **		uintpp ;
#endif /* TYPEDEF_UINTPPP */

#ifndef	TYPEDEF_ULONGPP
#define	TYPEDEF_ULONGPP
typedef unsigned long **	ulongpp ;
#endif /* TYPEDEF_ULONGPP */


#endif /* UTYPEDEFSMUINTPP_INCLUDE */


