/* streamsync HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* data stream synchronization mechanism */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	STREAMSYNC_INCLUDE
#define	STREAMSYNC_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	STREAMSYNC_MAGIC	0x96817463
#define	STREAMSYNC		struct streamsync_head


struct streamsync_head {
	char		*st ;		/* sequence to test against */
	char		*data ;		/* the data stream */
	uint		magic ;
	int		stlen ;		/* sequence length needed */
	int		i ;		/* current length */
} ;

#ifdef	__cplusplus
enum streamsyncmems {
	streamsyncmem_finish,
	streamsyncmem_overlast
} ;
struct streamsync ;
struct streamsync_co {
	streamsync	*op = nullptr ;
	int		w = -1 ;
	void operator () (streamsync *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (streamsync_co) */
struct streamsync : streamsync_head {
	streamsync_co	finish ;
	streamsync() noex {
	    finish(this,streamsyncmem_finish) ;
	    magic = 0 ;
	} ;
	streamsync(const streamsync &) = delete ;
	streamsync &operator = (const streamsync &) = delete ;
	int start(cchar *,int = -1) noex ;
	int test(int) noex ;
	void dtor() noex ;
	operator int () noex ;
	destruct streamsync() {
	    if (magic) dtor() ;
	} ;
} ; /* end struct (streamsync) */
#else	/* __cplusplus */
typedef STREAMSYNC	streamsync ;
#endif /* __cplusplus */

EXTERNC_begin

extern int streamsync_start(streamsync *,cchar *,int) noex ;
extern int streamsync_test(streamsync *,int) noex ;
extern int streamsync_finish(streamsync *) noex ;

EXTERNC_end


#endif /* STREAMSYNC_INCLUDE */


