/* ptm HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* POSIX Thread Mutex manipulation */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	PTM_INCLUDE
#define	PTM_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<pthread.h>		/* |pthread_mutex_t| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>

#include	"ptma.h"


#define	PTM	pthread_mutex_t

#ifdef	__cplusplus
enum ptmmems {
	ptmmem_create,
	ptmmem_destroy,
	ptmmem_lockbegin,
	ptmmem_lockend,
	ptmmem_overlast
} ;
struct ptm ;
struct ptm_creater {
	ptm		*op = nullptr ;
	int		w = -1 ;
	constexpr void operator () (ptm *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	int operator () (ptma * = nullptr) noex ;
	operator int () noex {
	    return (*this)() ;
	} ;
} ; /* end struct (ptm_creater) */
struct ptm_co {
        ptm             *op = nullptr ;
        int             w = -1 ;
        constexpr void operator () (ptm *p,int m) noex {
            op = p ;
            w = m ;
        } ;
        int operator () (int = -1) noex ;
        operator int () noex {
            return (*this)() ;
        } ;
} ; /* end struct (ptm_co) */
struct ptm : pthread_mutex_t {
	ptm_creater	create ;
	ptm_co		destroy ;
	ptm_co		lockbegin ;
	ptm_co		lockend ;
	constexpr ptm() noex {
	    create(this,ptmmem_create) ;
	    destroy(this,ptmmem_destroy) ;
	    lockbegin(this,ptmmem_lockbegin) ;
	    lockend(this,ptmmem_lockend) ;
	} ;
	ptm(const ptm &) = delete ;
	ptm &operator = (const ptm &) = delete ;
	void dtor() noex ;
	destruct ptm() {
	    dtor() ;
	} ; /* end dtor (ptm) */
} ; /* end class (ptm) */
#else
typedef PTM		ptm ;
#endif /* __cplusplus */

EXTERNC_begin

extern int	ptm_create(ptm *,ptma *) noex ;
extern int	ptm_destroy(ptm *) noex ;
extern int	ptm_lockbegin(ptm *) noex ;
extern int	ptm_lockbeginto(ptm *,int) noex ;
extern int	ptm_locktry(ptm *) noex ;
extern int	ptm_lockend(ptm *) noex ;
extern int	ptm_setprioceiling(ptm *,int,int *) noex ;
extern int	ptm_getprioceiling(ptm *,int *) noex ;

static inline int ptm_lock(ptm *op) noex {
    	return ptm_lockbegin(op) ;
}
static inline int ptm_lockto(ptm *op,int to) noex {
    	return ptm_lockbeginto(op,to) ;
}
static inline int ptm_unlock(ptm *op) noex {
    	return ptm_lockend(op) ;
}

EXTERNC_end


#endif /* PTM_INCLUDE */


