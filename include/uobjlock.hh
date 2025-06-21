/* uobjlock SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® Object-Lock-Management */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	uobjlock

	Description:
	This module provides some object locking management.

*******************************************************************************/

#ifndef	INCLUDE_UOBJLOCK
#define	INCLUDE_UOBJLOCK
#ifdef	__cplusplus /* C++ only! */


#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<aflag.hh>
#include	<ptm.h>


/* local defines */


/* imported namespaces */


/* local typedefs */


/* exported variables */


/* external subroutines */


/* local structures */

enum uobjlockmems {
    uobjlockmem_init,
    uobjlockmem_fini,
    uobjlockmem_callbegin,
    uobjlockmem_callend
} ; /* end enum (uobjlockmems) */
class uobjlock ;
struct uobjlock_co {
        uobjlock	*op = nullptr ;
        int             w = -1 ;
        void operator () (uobjlock *p,int m) noex {
            op = p ;
            w = m ;
        } ;
        int operator () (int = -1) noex ;
        operator int () noex {
            return operator () () ;
        } ;
} ; /* end struct (uobjlock) */
class uobjlock {
    	friend		uobjlock_co ;
	ptm		mx ;		/* data mutex */
	aflag		fvoid ;
	aflag		finit ;
	aflag		finitdone ;
	void_f		afbefore_f{}, afparent_f{}, afchild_f{} ;
	void_f		aefunc_f{} ;
	uobjlock_co	init ;
	uobjlock_co	fini ;
	uobjlock_co	callbegin ;
	uobjlock_co	callend ;
    public:
	uobjlock() noex {
	    init(this,uobjlockmem_init) ;
	    fini(this,uobjlockmem_fini) ;
	    callbegin(this,uobjlockmem_callbegin) ;
	    callend(this,uobjlockmem_callend) ;
	} ;
	void afload(void_f fbef,void_f fpar,void_f fchi) noex {
	    afbefore_f = fbef ;
	    afparent_f = fpar ;
	    afchild_f = fchi ;
	} ;
	void aeload(void_f ef) noex {
	    aefunc_f = ef ;
	} ;
	virtual int initonce() noex {
	    return SR_OK ;
	} ;
	virtual int finionce() noex {
	    return SR_OK ;
	} ;
	virtual int setupbegin() noex {
	    return SR_OK ;
	} ;
	virtual int setupend() noex {
	    return SR_OK ;
	} ;
	void dtor() noex ;
	virtual destruct uobjlock() {
	    dtor() ;
	} ; /* end dtor */
    private:
	void forkbefore() noex {
	    mx.lockbegin() ;
	} ;
	void forkafter() noex {
	    mx.lockend() ;
	} ;
	void exitnow() noex ;
	int iinit() noex ;
	int ifini() noex ;
	int icallbegin(int) noex ;
	int icallend() noex ;
	int isetexit() noex ;		/* internal helper */
} ; /* end struct (uobjlock) */


#endif /* __cplusplus (C++ only) */
#endif /* INCLUDE_UOBJLOCK */


