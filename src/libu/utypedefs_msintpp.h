/* utypedefs_msintpp HEADER (Integer-Signed-Mutable) */
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

#ifndef	UTYPEDEFSMSINTPP_INCLUDE
#define	UTYPEDEFSMSINTPP_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usysnative.h>		/* useful native system headers */
#include	<clanguage.h>		/* <- necessary inclusion */
#include	<stdintx.h>		/* extended integer types */
#include	<syspredefined.h>	/* system pre-defined types */


#ifndef	TYPEDEF_SCHARPP
#define	TYPEDEF_SCHARPP
typedef signed char **		scharpp ;
#endif /* TYPEDEF_SCHARPP */

#ifndef	TYPEDEF_SSHORTPP
#define	TYPEDEF_SSHORTPP
typedef signed short **		sshortpp ;
#endif /* TYPEDEF_SSHORTPP */

#ifndef	TYPEDEF_SINTPP
#define	TYPEDEF_SINTPP
typedef signed int **		sintpp ;
#endif /* TYPEDEF_SINTPPP */

#ifndef	TYPEDEF_SLONGPP
#define	TYPEDEF_SLONGPP
typedef signed long **		slongpp ;
#endif /* TYPEDEF_SLONGPP */


#endif /* UTYPEDEFSMSINTPP_INCLUDE */


