/* clanguage_externc HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* C-language defines */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-15, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CLANGUAGEEXTERNC_INCLUDE
#define	CLANGUAGEEXTERNC_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */


#ifndef	EXTERNC_begin
#ifdef	__cplusplus
#define	EXTERNC_begin		extern "C" {
#define	EXTERNC_end		}
#else
#define	EXTERNC_begin		/* externc_begin */
#define	EXTERNC_end		/* externc_end */
#endif /* __cplusplus */
#endif /* EXTERNC_begin */


#endif /* CLANGUAGEEXTERNC_INCLUDE */


