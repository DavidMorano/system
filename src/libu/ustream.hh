/* ustream HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* support some buffered file operations */
/* version %I% last-modified %G% */


/* revision history:

	= 2002-04-01, David A­D­ Morano
	This code was originally written for Rightcore Network
	Services (RNS).

*/

/* Copyright © 2002 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	ustream

	Description:
        This little object supports some buffered file operations for
        low-overhead buffered I-O requirements.

*******************************************************************************/

#ifndef	USTREAM_INCLUDE
#define	USTREAM_INCLUDE
#ifdef	__cplusplus /* (C++ only) */


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX system types */
#include	<unistd.h>		/* POSIX system types */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstdarg>		/* CSTD |va_list(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define	USTREAM_MAGIC	0x21387462
#define	USTREAM_RCNET	3		/* read-count for network */


#endif /* __cplusplus (C++ only) */
#endif /* USTREAM_INCLUDE */


