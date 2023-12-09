/* ustropts INCLUDE */
/* lang=C20 */

/* version %I% last-modified %G% */
/* auxillary operating system support */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

	= 2017-08-01, David A­D­ Morano
	Updated for lack of interfaces in MacOS Darwin

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This file contains the UNIX® system types that the brain-damaged
	MacOS operating system does NOT have.  We are trying in a very
	small way to make up for some of the immense brain-damage within
	the Apple Darwin operating system.

	Below are some STREAMS structures. They were defined with 
	UNIX® System V Release 3.

*******************************************************************************/

#ifndef	USTROPTS_INCLUDE
#define	USTROPTS_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/time.h>		/* <- |TIMESPEC| is there */
#include	<time.h>
#include	<clanguage.h>


#ifndef	CBUFLEN
#define	CBUFLEN		(2*1024)
#endif

#ifndef	DBUFLEN
#define	DBUFLEN		(2*1024)
#endif


#ifndef	STRBUF
#define	STRBUF		struct strbuf
#endif

#ifndef	STRPEEK
#define	STRPEEK		struct strpeek
#endif


#if	defined(SYSHAS_STREAMS) && (SYSHAS_STREAMS > 0)

#include	<stropts.h>

#else /* defined(SYSHAS_STREAMS) && (SYSHAS_STREAMS > 0) */

#ifndef	I_PEEK
#define	I_PEEK		0
#endif

struct strbuf {
	char		*buf ;
	int		len ;
	int		maxlen ;
} ;

struct strpeek {
	STRBUF		ctlbuf ;
	STRBUF		databuf ;
	int		flags ;
} ;

int getmsg(int,STRBUF *,STRBUF *,int *) noex ;
int getpmsg(int,STRBUF *,STRBUF *,int *,int *) noex ;
int putmsg(int,STRBUF *,STRBUF *,int *) noex ;
int putpmsg(int,STRBUF *,STRBUF *,int *,int *) noex ;

#endif /* defined(SYSHAS_STREAMS) && (SYSHAS_STREAMS > 0) */


#endif /* USTROPTS_INCLUDE */


