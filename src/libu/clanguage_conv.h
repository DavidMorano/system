/* clanguage_conv HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* C-language defines */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-15, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CLANGUAGECONV_INCLUDE
#define	CLANGUAGECONV_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */


#ifndef	charconv
#define	charconv		char
#endif

#ifndef	scharconv
#define	scharconv		schar
#endif
#ifndef	shortconv
#define	shortconv		short
#endif
#ifndef	intconv
#define	intconv			int
#endif
#ifndef	longconv
#define	longconv		long
#endif
#ifndef	longlongconv
#define	longlongconv		longlong
#endif

#ifndef	scharconv
#define	scharconv		schar
#endif
#ifndef	sshortconv
#define	sshortconv		sshort
#endif
#ifndef	sintconv
#define	sintconv		sint
#endif
#ifndef	slongconv
#define	slongconv		slong
#endif
#ifndef	slonglongconv
#define	slonglongconv		slonglong
#endif

#ifndef	ucharconv
#define	ucharconv		uchar
#endif
#ifndef	ushortconv
#define	ushortconv		ushort
#endif
#ifndef	uintconv
#define	uintconv		uint
#endif
#ifndef	ulongconv
#define	ulongconv		ulong
#endif
#ifndef	ulonglongconv
#define	ulonglongconv		ulonglong
#endif

#ifndef	floatconv
#define	floatconv		float
#endif

#ifndef	doubleconv
#define	doubleconv		double
#endif


#endif /* CLANGUAGECONV_INCLUDE */


