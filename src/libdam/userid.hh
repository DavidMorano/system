/* userid HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* some sort of identification-object for the MOTD object */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USERID_INCLUDE
#define	USERID_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* system types */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


enum useridmems {
    	useridmem_finish,
    	useridmem_overlast
} ;
struct userid ;
struct userid_st {
	userid		*op = nullptr ;
	void init(userid *p) noex {
	    op = p ;
	} ;
	int operator () (cc * = nullptr,cc * = nullptr,
		uid_t = -1,gid_t = -1) noex ;
	operator int () noex {
	    return operator () () ;
	} ;
} ; /* end struct (userid_st) */
struct userid_co {
	userid		*op = nullptr ;
	int		w = -1 ;
	void operator () (userid *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (userid_co) */
struct userid {
	userid_st	start ;
	userid_co	finish ;
	cchar		*username ;
	cchar		*groupname ;
	uid_t		uid ;
	gid_t		gid ;
	userid() noex {
	    start.init(this) ;
	    finish(this,useridmem_finish) ;
	} ;
	destruct userid() {
	    if (username) dtor() ;
	} ;
    private :
	void dtor() noex ;
} ; /* end struct (userid) */

EXTERNC_begin

extern int	userid_star(userid *,cchar * = nullptr) noex ;
extern int	userid_finish(userid *) noex ;

EXTERNC_end


#endif /* __cplusplus */
#endif /* USERID_INCLUDE */


