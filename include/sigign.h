/* sigign HEADER */
/* lang=C20 */

/* little object to block signals */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This file was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SIGIGN_INCLUDE
#define	SIGIGN_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<signal.h>
#include	<usystem.h>


#define	SIGIGN_MAGIC	0x66938271
#define	SIGIGN		struct sigign_head
#define	SIGIGN_HANDLE	struct sigign_handle


struct sigign_handle {
	SIGACTION	action ;
	int		sig ;
} ;

struct sigign_head {
	uint		magic ;
	sigset_t	osm ;
	SIGIGN_HANDLE	*handles ;
	int		nhandles ;
	int		nblocks ;
} ;

typedef	SIGIGN		sigign ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int	sigign_start(SIGIGN *,cint *) noex ;
extern int	sigign_finish(SIGIGN *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* SIGIGN_INCLUDE */


