/* fdliner HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* file-read line hanlding */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	fdliner

	Description:
	This object is a line-reading helper manager.

*******************************************************************************/

#ifndef	FDLINER_INCLUDE
#define	FDLINER_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* |off_t| */
#include	<unistd.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysrets.h>
#include	<filer.h>


#define	FDLINER			struct fdliner_head
#define	FDLINER_FL		struct fdliner_flags
#define	FDLINER_MAGIC		0x53272856
#define	FDLINER_BUFSIZEDEF	1024
#define	FDLINER_BUFSIZEBLOCK	512


struct fdliner_flags {
	bool		fnet ;
} ;

struct fdliner_head {
	filer		*fbp ;		/* allocated */
	char		*lbuf ;
	off_t		poff ;		/* file-offset previous */
	off_t		foff ;		/* file-offset current */
	FDLINER_FL	fl ;
	int		llen ;
	int		fbo ;
	int		to ;		/* read time-out */
} ;

#ifdef	__cplusplus
enum fdlinermems {
    	fdlinermem_done,
	fdlinermem_adv,
	fdlinermem_finish,
	fdlinermem_overlast
} ;
struct fdliner ;
struct fdliner_co {
	fdliner		*op = nullptr ;
	int		w = -1 ;
	void operator () (fdliner *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	int operator () (int = 0) noex ;
	operator int () noex {
	    return operator () () ;
	} ;
} ; /* end struct (fdliner_co) */
struct fdliner : fdliner_head {
	fdliner_co	done ;
	fdliner_co	adv ;
	fdliner_co	finish ;
	fdliner() noex {
	    done(this,fdlinermem_done) ;
	    adv(this,fdlinermem_adv) ;
	    finish(this,fdlinermem_finish) ;
	} ;
	fdliner(const fdliner &) = delete ;
	fdliner &operator = (const fdliner &) = delete ;
	int start(int,off_t = 0z,int = -1) noex ;
	int getln(cchar **) noex ;
	void dtor() noex ;
	~fdliner() {
	    dtor() ;
	} ;
} ; /* end struct (fdliner) */
#else	/* __cplusplus */
typedef FDLINER		fdliner ;
#endif /* __cplusplus */

typedef FDLINER_FL	fdliner_fl ;

EXTERNC_begin

extern int fdliner_start(fdliner *,int,off_t,int) noex ;
extern int fdliner_getln(fdliner *,cchar **) noex ;
extern int fdliner_done(fdliner *) noex ;
extern int fdliner_adv(fdliner *,int) noex ;
extern int fdliner_finish(fdliner *) noex ;

EXTERNC_end


#endif /* FDLINER_INCLUDE */


