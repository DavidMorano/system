/* utypedefs_crintpp HEADER (Integers-Constant) */
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

#ifndef	UTYPEDEFSCRINTPP_INCLUDE
#define	UTYPEDEFSCRINTPP_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usysnative.h>		/* useful native system headers */
#include	<clanguage.h>		/* <- necessary inclusion */
#include	<stdintx.h>		/* extended integer types */
#include	<syspredefined.h>	/* system pre-defined types */


#ifndef	TYPEDEF_CCHARPP
#define	TYPEDEF_CCHARPP
typedef const char **		ccharpp ;
#endif /* TYPEDEF_CCHARPP */

#ifndef	TYPEDEF_CSHORTPP
#define	TYPEDEF_CSHORTPP
typedef const short **		cshortpp ;
#endif /* TYPEDEF_CSHORPP */

#ifndef	TYPEDEF_CINTPP
#define	TYPEDEF_CINTPP
typedef const int **		crintpp ;
#endif /* TYPEDEF_CINTPP */

#ifndef	TYPEDEF_CLONGPP
#define	TYPEDEF_CLONGPP
typedef const long **		clongpp ;
#endif /* TYPEDEF_CLONGPP */


#endif /* UTYPEDEFSCRINTPP_INCLUDE */


