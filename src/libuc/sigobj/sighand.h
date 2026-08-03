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
#include	<sys/types.h>		/* POSIX® system types */
#include	<signal.h>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define	SIGHAND		struct sighand_head
#define	SIGHAND_HA	struct sighand_handle
#define	SIGHAND_MAGIC	0x66938271

#ifndef	TYPEDEF_SIGHANDF
#define	TYPEDEF_SIGHANDF
EXTERNC_begin
typedef void (*sighand_f)(int,siginfo_t *,void *) noex ;
EXTERNC_end
#endif /* TYPEDEF_SIGHANDF */

struct sighand_handle {
	SIGACTION	action ;
	int		sig ;
} ; /* end struct */

struct sighand_head {
	SIGHAND_HA	*handles ;
	sigset_t	osm ;
	uint		magval ;
	int		nhandles ;
	int		nblocks ;
} ; /* end struct */

typedef	SIGHAND_HA	sighand_ha ;

#ifdef	__cplusplus
enum sighandmems {
	sighandmem_finish,
	sighandmem_overlast
} ; /* end enum */
struct sighand ;
struct sighand_co {
	sighand		*op = nullptr ;
	int		w = -1 ;
	void operator () (sighand *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () (const int * = nullptr) noex ;
} ; /* end struct (sighand_co) */
struct sighand : sighand_head {
	sighand_co	finish ;
	sighand() noex {
	    finish	(this,sighandmem_finish) ;
	    magval = 0 ;
	} ; /* end ctor */
	int start(cint *,cint *,cint *,sighand_f) noex ;
	void dtor() noex ;
	destruct sighand() {
	    if (magval) dtor() ;
	} ; /* end destruct */
} ; /* end class (sighand) */
#else
typedef	SIGHAND		sighand ;
#endif /* __cplusplus */

EXTERNC_begin

extern int sighand_start(sighand *,cint *,cint *,cint *,sighand_f) noex ;
extern int sighand_finish(sighand *) noex ;

EXTERNC_end


#endif /* SIGHAND_INCLUDE */


