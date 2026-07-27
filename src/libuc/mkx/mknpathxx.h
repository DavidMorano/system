/* mknpathxx HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* provided are various general c-string creation subroutines */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A-D- Morano
	This was written for Rightcore Network Services (RNS).
 
	= 2018-11-27, David A-D- Morano
	I created an inline version for the C language. We will
	likely do something for C++ at some other time.

	= 2023-10-23, David A-D- Morano
	At some point I added an inline (here) for the C++ language.

*/

/* Copyright © 2000,2018,2023 David A-D- Morano.  All rights reserved. */

#ifndef	MKNPATHXX_INCLUDE
#define	MKNPATHXX_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int	mknpathexp	(char *,int,cchar *,int) noex ;
extern int	mknpathuser	(char *,int,cchar *,cchar *,int) noex ;
extern int	mknpathvar	(char *,int,cchar *,int) noex ;

EXTERNC_end


#endif /* MKNPATHXX_INCLUDE */


