/* sigign HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* little object to ignore (block?) signals */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This file was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SIGIGN_INCLUDE
#define	SIGIGN_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® system types? */
#include	<signal.h>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define	SIGIGN		struct sigign_head
#define	SIGIGN_HA	struct sigign_handle
#define	SIGIGN_MAGIC	0x66938271


struct sigign_handle {
	SIGACTION	action ;
	int		sig ;
} ; /* end struct (sigign_handle) */

struct sigign_head {
	SIGIGN_HA	*handles ;
	sigset_t	osm ;
	uint		magval ;
	int		nhandles ;
	int		nblocks ;
} ; /* end struct (sigign_head) */

typedef	SIGIGN_HA	sigign_ha ;

#ifdef	__cplusplus
enum sigignmems {
	sigignmem_start,
	sigignmem_finish,
	sigignmem_overlast
} ; /* end enum */
struct sigign ;
struct sigign_co {
	sigign		*op = nullptr ;
	int		w = -1 ;
	void operator () (sigign *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () (const int * = nullptr) noex ;
} ; /* end struct (sigign_co) */
struct sigign : sigign_head {
	sigign_co	start ;
	sigign_co	finish ;
	sigign() noex {
	    start	(this,sigignmem_start) ;
	    finish	(this,sigignmem_finish) ;
	    magval = 0 ;
	} ; /* end ctor */
	void dtor() noex ;
	destruct sigign() {
	    if (magval) dtor() ;
	} ; /* end destruct */
} ; /* end class (sigign) */

#else
typedef	SIGIGN		sigign ;
#endif /* __cplusplus */


EXTERNC_begin

extern int	sigign_start(sigign *,cint *) noex ;
extern int	sigign_finish(sigign *) noex ;

EXTERNC_end


#endif /* SIGIGN_INCLUDE */


