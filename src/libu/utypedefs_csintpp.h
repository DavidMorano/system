/* utypedefs_csintpp HEADER (Integers-Signed-Constant) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* virtual-system definitions */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UTYPEDEFSCSINTPP_INCLUDE
#define	UTYPEDEFSCSINTPP_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usysnative.h>		/* useful native system headers */
#include	<clanguage.h>		/* <- necessary inclusion */
#include	<stdintx.h>		/* extended integer types */
#include	<syspredefined.h>	/* system pre-defined types */


#ifndef	TYPEDEF_CSCHARPP
#define	TYPEDEF_CSCHARPP
typedef const signed char **	cscharpp ;
#endif /* TYPEDEF_CSCHARPP */

#ifndef	TYPEDEF_CSSHORTPP
#define	TYPEDEF_CSSHORTPP
typedef const signed short **	csshortpp ;
#endif /* TYPEDEF_CSSHORTPP */

#ifndef	TYPEDEF_CSINTPP
#define	TYPEDEF_CSINTPP
typedef const signed int **	csintpp ;
#endif /* TYPEDEF_CSINTPP */

#ifndef	TYPEDEF_CSLONGPP
#define	TYPEDEF_CSLONGPP
typedef const signed long **	cslongpp ;
#endif /* TYPEDEF_CSLONGPP */


#endif /* UTYPEDEFSCSINTPP_INCLUDE */


