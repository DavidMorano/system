/* ustropts HEADER */
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


#include	<envstandards.h>	/* ordered first to configure */
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>


#ifndef	STRBUF
#define	STRBUF		struct strbuf
#endif

#ifndef	STRPEEK
#define	STRPEEK		struct strpeek
#endif

#ifndef	STRRECVFD
#define	STRRECVFD	struct strrecvfd
#endif

#ifndef	STRIOCTL	
#define	STRIOCTL	struct strioctl
#endif


#if	defined(SYSHAS_STREAMS) && (SYSHAS_STREAMS > 0)

/* we have it: do nothing for now */

#else /* defined(SYSHAS_STREAMS) && (SYSHAS_STREAMS > 0) */

#ifndef	I_PEEK
#define	I_PEEK		0
#endif

#ifndef	I_STR
#define	I_STR		1
#endif

#ifndef	ISPTM
#define	ISPTM		0
#endif

struct strbuf {
	char		*buf ;
	int		len ;
	int		maxlen ;
} ;

struct strpeek {
	STRBUF		ctlbuf ;
	STRBUF		databuf ;
	uint		flags ;
} ;

struct strrecvfd {      
        int		fd ;
        uid_t		uid ;
        gid_t		gid ;
        char		fill[8] ;
} ;

struct strioctl {
	char		*ic_dp ;	/* data pointer */
	int		ic_len ;	/* data length */
	int		ic_cmd ;	/* command */
	int		ic_timout ;	/* timeout */
} ;

EXTERNC_begin

extern int getmsg(int,STRBUF *,STRBUF *,int *) noex ;
extern int getpmsg(int,STRBUF *,STRBUF *,int *,int *) noex ;
extern int putmsg(int,STRBUF *,STRBUF *,int *) noex ;
extern int putpmsg(int,STRBUF *,STRBUF *,int *,int *) noex ;

EXTERNC_end

#endif /* defined(SYSHAS_STREAMS) && (SYSHAS_STREAMS > 0) */


#endif /* USTROPTS_INCLUDE */


