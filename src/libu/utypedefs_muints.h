/* utypedefs_muints HEADER (Integer-UnSigned-Mutable) */
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

#ifndef	UTYPEDEFSMUINTS_INCLUDE
#define	UTYPEDEFSMUINTS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usysnative.h>		/* useful native system headers */
#include	<clanguage.h>		/* <- necessary inclusion */
#include	<stdintx.h>		/* extended integer types */
#include	<syspredefined.h>	/* system pre-defined types */


#ifndef	TYPEDEF_UCHAR
#define	TYPEDEF_UCHAR
typedef unsigned char		uchar ;
#endif /* TYPEDEF_UCHAR */

#ifndef	TYPEDEF_USHORT
#define	TYPEDEF_USHORT
typedef unsigned short		ushort ;
#endif /* TYPEDEF_USHORT */

#ifndef	TYPEDEF_UINT
#define	TYPEDEF_UINT
typedef unsigned int		uint ;
#endif /* TYPEDEF_UINT */

#ifndef	TYPEDEF_ULONG
#define	TYPEDEF_ULONG
typedef unsigned long		ulong ;
#endif /* TYPEDEF_ULONG */


#endif /* UTYPEDEFSMUINTS_INCLUDE */


