/* utypedefs_mrintpp HEADER (Integer-UnSigned-Mutable) */
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

#ifndef	UTYPEDEFSMRINTPP_INCLUDE
#define	UTYPEDEFSMRINTPP_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usysnative.h>		/* useful native system headers */
#include	<clanguage.h>		/* <- necessary inclusion */
#include	<stdintx.h>		/* extended integer types */
#include	<syspredefined.h>	/* system pre-defined types */


#ifndef	TYPEDEF_CHARPP
#define	TYPEDEF_CHARPP
typedef char **		charpp ;
#endif /* TYPEDEF_CHARPP */

#ifndef	TYPEDEF_SHORTPP
#define	TYPEDEF_SHORTPP
typedef short **	shortpp ;
#endif /* TYPEDEF_SHORTPP */

#ifndef	TYPEDEF_INTPP
#define	TYPEDEF_INTPP
typedef int **		intpp ;
#endif /* TYPEDEF_INTPPP */

#ifndef	TYPEDEF_LONGPP
#define	TYPEDEF_LONGPP
typedef long **		longpp ;
#endif /* TYPEDEF_LONGPP */


#endif /* UTYPEDEFSMRINTPP_INCLUDE */


