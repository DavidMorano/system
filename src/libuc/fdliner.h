/* fdliner HEADER */
/* lang=C20 */

/* file-read line hanlding */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	FDLINER_INCLUDE
#define	FDLINER_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<filebuf.h>


#define	FDLINER			struct fdliner_head
#define	FDLINER_FL		struct fdliner_flags
#define	FDLINER_MAGIC		0x53272856
#define	FDLINER_BUFSIZEDEF	1024
#define	FDLINER_BUFSIZEBLOCK	512


struct fdliner_flags {
	bool		fnet ;
} ;

struct fdliner_head {
	filebuf		*fbp ;		/* allocated */
	char		*lbuf ;
	off_t		poff ;		/* file-offset previous */
	off_t		foff ;		/* file-offset current */
	FDLINER_FL	f ;
	int		llen ;
	int		fbo ;
	int		to ;		/* read time-out */
} ;


typedef FDLINER		fdliner ;
typedef FDLINER_FL	fdliner_fl ;

EXTERNC_begin

extern int liner_start(fdliner *,int,off_t,int) noex ;
extern int liner_finish(fdliner *) noex ;
extern int liner_read(fdliner *,cchar **) noex ;
extern int liner_done(fdliner *) noex ;

EXTERNC_end


#endif /* FDLINER_INCLUDE */


