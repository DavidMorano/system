/* sighand HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* little object to handle signals */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This file was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SIGHAND_INCLUDE
#define	SIGHAND_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* system types */
#include	<signal.h>
#include	<usystem.h>


#define	SIGHAND_MAGIC	0x66938271
#define	SIGHAND		struct sighand_head
#define	SIGHAND_HA	struct sighand_handle


#ifndef	SIGHANDF_TYPEDEF
#define	SIGHANDF_TYPEDEF
EXTERNC_begin
typedef void (*sighand_f)(int,siginfo_t *,void *) noex ;
EXTERNC_end
#endif /* SIGHANDF_TYPEDEF */


struct sighand_handle {
	SIGACTION	action ;
	int		sig ;
} ;

struct sighand_head {
	SIGHAND_HA	*handles ;
	sigset_t	osm ;
	uint		magic ;
	int		nhandles ;
	int		nblocks ;
} ;

typedef SIGHAND		sighand ;
typedef	SIGHAND_HA	sighand_ha ;

EXTERNC_begin

typedef void (*sighand_handler)(int,siginfo_t *,void *) noex ;

extern int sighand_start(sighand *,cint *,cint *,cint *,sighand_f) noex ;
extern int sighand_finish(sighand *) noex ;

EXTERNC_end


#endif /* SIGHAND_INCLUDE */


