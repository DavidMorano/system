/* ptrwlock HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* POSIX® Thread Read-Write Lock (PTRWLOCK) */
/* ** broken on SOLARIS®! ** */
/* version %I% last-modified %G% */


/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

#ifndef	PTRWLOCK_INCLUDE
#define	PTRWLOCK_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<pthread.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<ptrwa.h>


#define	PTRWLOCK	pthread_rwlock_t
#define	PTRWLOCK_MAGIC	0x32412527


#ifdef	__cplusplus
enum ptrwlockmems {
	ptrwlockmem_create,
	ptrwlockmem_lockrd,
	ptrwlockmem_lockrdtry,
	ptrwlockmem_lockwr,
	ptrwlockmem_lockwrtry,
	ptrwlockmem_lockend,
	ptrwlockmem_destroy,
	ptrwlockmem_overlast
} ; /* end enum (ptrwlockmems) */
struct ptrwlock ;
struct ptrwlock_cr {
	ptrwlock	*op = nullptr ;
	int		w = -1 ;
	constexpr void operator () (ptrwlock *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	int operator () (ptrwa * = nullptr) noex ;
	operator int () noex {
	    return (*this)() ;
	} ;
} ; /* end struct (ptrwlock_cr) */
struct ptrwlock_co {
        ptrwlock	*op = nullptr ;
        int             w = -1 ;
        constexpr void operator () (ptrwlock *p,int m) noex {
            op = p ;
            w = m ;
        } ;
        operator int () noex ;
} ; /* end struct (ptrwlock_co) */
struct ptrwlock : pthread_rwlock_t {
	ptrwlock_cr	create ;
	ptrwlock_co	lockrd ;
	ptrwlock_co	lockrdtry ;
	ptrwlock_co	lockwr ;
	ptrwlock_co	lockwrtry ;
	ptrwlock_co	lockend ;
	ptrwlock_co	destroy ;
	uint		magic ;
	constexpr ptrwlock() noex {
	    create	(this,ptrwlockmem_create) ;
	    lockrd	(this,ptrwlockmem_lockrd) ;
	    lockrdtry	(this,ptrwlockmem_lockrdtry) ;
	    lockwr	(this,ptrwlockmem_lockwr) ;
	    lockwrtry	(this,ptrwlockmem_lockwrtry) ;
	    lockend	(this,ptrwlockmem_lockend) ;
	    destroy	(this,ptrwlockmem_destroy) ;
	    magic = 0 ;
	} ; /* end ctor */
	ptrwlock(const ptrwlock &) = delete ;
	ptrwlock &operator = (const ptrwlock &) = delete ;
	int lockrdto	(int) noex ;
	int lockwrto	(int) noex ;
	void dtor() noex ;
	destruct ptrwlock() {
	    if (magic) dtor() ;
	} ; /* end dtor (ptrwlock) */
} ; /* end class (prwlock) */
#else
typedef PTRWLOCK	ptrwlock ;
#endif /* __cplusplus */


EXTERNC_begin

extern int	ptrwlock_create		(ptrwlock *,ptrwa *) noex ;
extern int	ptrwlock_lockrd		(ptrwlock *) noex ;
extern int	ptrwlock_lockrdtry	(ptrwlock *) noex ;
extern int	ptrwlock_lockrdto	(ptrwlock *,int) noex ;
extern int	ptrwlock_lockwr		(ptrwlock *) noex ;
extern int	ptrwlock_lockwrtry	(ptrwlock *) noex ;
extern int	ptrwlock_lockwrto	(ptrwlock *,int) noex ;
extern int	ptrwlock_lockend	(ptrwlock *) noex ;
extern int	ptrwlock_destroy	(ptrwlock *) noex ;

EXTERNC_end


#endif /* PTRWLOCK_INCLUDE */


