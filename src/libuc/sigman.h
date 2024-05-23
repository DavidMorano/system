/* sigman HEADER */
/* lang=C20 */

/* little object to block signals */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This file was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SIGMAN_INCLUDE
#define	SIGMAN_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<signal.h>
#include	<usystem.h>
#include	<localmisc.h>


#define	SIGMAN_MAGIC	0x66938271
#define	SIGMAN		struct sigman_head
#define	SIGMAN_HAND	struct sigman_handle


struct sigman_handle {
	SIGACTION	action ;
	int		sig ;
} ;

struct sigman_head {
	SIGMAN_HAND	*handles ;
	sigset_t	osm ;
	uint		magic ;
	int		nhandles ;
	int		nblocks ;
} ;

typedef	SIGMAN		sigman ;
typedef	SIGMAN_HAND	sigman_hand ;

EXTERNC_begin

typedef void (*sigmanhand_f)(int) noex ;

extern int	sigman_start(sigman *,cint *,cint *,cint *,sigmanhand_f) noex ;
extern int	sigman_finish(sigman *) noex ;

EXTERNC_end


#endif /* SIGMAN_INCLUDE */


