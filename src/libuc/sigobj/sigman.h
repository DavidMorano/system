/* sigman HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* little object to manage signals */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This file was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SIGMAN_INCLUDE
#define	SIGMAN_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<signal.h>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define	SIGMAN		struct sigman_head
#define	SIGMAN_HA	struct sigman_hale
#define	SIGMAN_MAGIC	0x66938271


struct sigman_hale {
	SIGACTION	action ;
	int		sig ;
} ; /* end struct */

struct sigman_head {
	SIGMAN_HA	*handles ;
	sigset_t	osm ;
	uint		magval ;
	int		nhs ;
	int		nblks ;
} ; /* end struct */

typedef	SIGMAN_HA	sigman_ha ;

EXTERNC_begin
typedef void (*sigmanhand_f)(int) noex ;
EXTERNC_end

#ifdef	__cplusplus
enum sigmanmems {
	sigmanmem_finish,
	sigmanmem_overlast
} ; /* end enum */
struct sigman ;
struct sigman_co {
	sigman		*op = nullptr ;
	int		w = -1 ;
	void operator () (sigman *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
} ; /* end struct (sigman_co) */
struct sigman : sigman_head {
	sigman_co	finish ;
	sigman() noex {
	    finish	(this,sigmanmem_finish) ;
	    magval = 0 ;
	} ; /* end ctor */
	int start(cint *,cint *,cint *,sigmanhand_f) noex ;
	void dtor() noex ;
	destruct sigman() {
	    if (magval) dtor() ;
	} ; /* end destruct */
} ; /* end class (sigman) */

#else
typedef	SIGMAN		sigman ;
#endif /* __cplusplus */

EXTERNC_begin

extern int	sigman_start(sigman *,cint *,cint *,cint *,sigmanhand_f) noex ;
extern int	sigman_finish(sigman *) noex ;

EXTERNC_end


#endif /* SIGMAN_INCLUDE */


