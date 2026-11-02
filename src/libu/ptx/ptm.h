/* ptm HEADER (POSIX®-Thread-Mutex) */
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
#include	<pthread.h>		/* POSIX® |pthread_mutex_t| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */

#include	"ptma.h"


#define	PTM		pthread_mutex_t
#define	PTM_MAGIC	0x32412522


#ifdef	__cplusplus
enum ptmsubs {
	ptmsub_create,
	ptmsub_destroy,
	ptmsub_lockbeg,
	ptmsub_lockend,
	ptmsub_overlast
} ; /* end enum (ptmsubs) */
struct ptm ;
struct ptm_creater {
	ptm		*op = nullptr ;
	int		w = -1 ;
	constexpr void operator () (ptm *p,int m) noex {
	    op = p ;
	    w = m ;
	} ; /* end */
	int operator () (ptma * = nullptr) noex ;
	operator int () noex {
	    return (*this)() ;
	} ; /* end */
} ; /* end struct (ptm_creater) */
struct ptm_co {
        ptm             *op = nullptr ;
        int             w = -1 ;
        constexpr void operator () (ptm *p,int m) noex {
            op = p ;
            w = m ;
        } ; /* end */
        int operator () (int = -1) noex ;
        operator int () noex {
            return (*this)() ;
        } ; /* end */
} ; /* end struct (ptm_co) */
struct ptm : pthread_mutex_t {
	ptm_creater	create ;
	ptm_co		destroy ;
	ptm_co		lockbeg ;
	ptm_co		lockend ;
	uint		magval ;
	constexpr ptm() noex {
	    create	(this,ptmsub_create) ;
	    destroy	(this,ptmsub_destroy) ;
	    lockbeg	(this,ptmsub_lockbeg) ;
	    lockend	(this,ptmsub_lockend) ;
	    magval	= 0 ;
	} ; /* end ctor */
	ptm(const ptm &) = delete ;
	ptm &operator = (con ptm &) = delete ;
	void dtor	() noex ;
	destruct ptm() {
	    if (magval) dtor() ;
	} ; /* end dtor (ptm) */
} ; /* end class (ptm) */
#else
typedef PTM		ptm ;
#endif /* __cplusplus */

EXTERNC_begin
extern int ptm_create		(ptm *,ptma *) noex ;
extern int ptm_destroy		(ptm *) noex ;
extern int ptm_lockbeg		(ptm *) noex ;
extern int ptm_lockbegto	(ptm *,int) noex ;
extern int ptm_locktry		(ptm *) noex ;
extern int ptm_lockend		(ptm *) noex ;
extern int ptm_setprioceiling	(ptm *,int,int *) noex ;
extern int ptm_getprioceiling	(ptm *,int *) noex ;
EXTERNC_end

EXTERNC_begin
local inline int ptm_lock	(ptm *op) noex {
    	return ptm_lockbeg(op) ;
} /* end */
local inline int ptm_lockto	(ptm *op,int to) noex {
    	return ptm_lockbegto(op,to) ;
} /* end */
local inline int ptm_unlock	(ptm *op) noex {
    	return ptm_lockend(op) ;
} /* end */
EXTERNC_end


#endif /* PTM_INCLUDE */


