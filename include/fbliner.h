/* fbliner HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* file-buf (now FILER) file-read line handling */
/* version %I% last-modified %G% */


/* revision history:

	= 2009-01-10, David A­D­ Morano
        This is being written as a helper line-reading object
	for use in other objects.

*/

/* Copyright © 2009 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	fbliner

	Description:
	This object is a line-reading helper manager.

*******************************************************************************/

#ifndef	FBLINER_INCLUDE
#define	FBLINER_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* |off_t| */
#include	<unistd.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<filer.h>


#define	FBLINER		struct fbliner_head


struct fbliner_head {
	filer		*fbp ;		/* externally-owned */
	char		*lbuf ;		/* allocated */
	off_t		poff ;		/* file-offset previous */
	off_t		foff ;		/* file-offset current */
	int		to ;		/* read time-out */
	int		llen ;
} ;

#ifdef	__cplusplus
enum fblinermems {
    	fblinermem_done,
	fblinermem_adv,
	fblinermem_finish,
	fblinermem_overlast
} ;
struct fbliner ;
struct fbliner_co {
	fbliner		*op = nullptr ;
	int		w = -1 ;
	void operator () (fbliner *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	int operator () (int = 0) noex ;
	operator int () noex {
	    return operator () () ;
	} ;
} ; /* end struct (fbliner_co) */
struct fbliner : fbliner_head {
	fbliner_co	done ;
	fbliner_co	adv ;
	fbliner_co	finish ;
	fbliner() noex {
	    done(this,fblinermem_done) ;
	    adv(this,fblinermem_adv) ;
	    finish(this,fblinermem_finish) ;
	} ;
	fbliner(const fbliner &) = delete ;
	fbliner &operator = (const fbliner &) = delete ;
	int start(filer *,off_t = 0z,int = -1) noex ;
	int getln(cchar **) noex ;
	void dtor() noex ;
	~fbliner() {
	    dtor() ;
	} ;
} ; /* end struct (fbliner) */
#else	/* __cplusplus */
typedef FBLINER		fbliner ;
#endif /* __cplusplus */

EXTERNC_begin

extern int fbliner_start(fbliner *,filer *,off_t,int) noex ;
extern int fbliner_getln(fbliner *,cchar **) noex ;
extern int fbliner_done(fbliner *) noex ;
extern int fbliner_adv(fbliner *,int) noex ;
extern int fbliner_finish(fbliner *) noex ;

EXTERNC_end


#endif /* FBLINER_INCLUDE */


