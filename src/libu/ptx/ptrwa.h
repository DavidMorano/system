/* ptrwa HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* POSIX® Thread Read-Write lock Attribute manipulation */
/* version %I% last-modified %G% */


/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

#ifndef	PTRWA_INCLUDE
#define	PTRWA_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<pthread.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	PTRWA		pthread_rwlockattr_t
#define	PTRWA_MAGIC	0x32412528


#ifdef	__cplusplus
enum ptrwamems {
	ptrwamem_create,
	ptrwamem_destroy,
	ptrwamem_setpshared,
	ptrwamem_overlast
} ; /* end enum (ptrwamems) */
struct ptrwa ;
struct ptrwa_co {
        ptrwa		*op = nullptr ;
        int             w = -1 ;
        constexpr void operator () (ptrwa *p,int m) noex {
            op = p ;
            w = m ;
        } ;
        int operator () (int a = -1) noex ;
        operator int () noex {
	    return operator () (-1) ;
	}
} ; /* end struct (ptrwa_co) */
struct ptrwa : pthread_rwlockattr_t {
	ptrwa_co	create ;
	ptrwa_co	destroy ;
	ptrwa_co	setpshared ;
	uint		magic ;
	constexpr ptrwa() noex {
	    create	(this,ptrwamem_create) ;
	    destroy	(this,ptrwamem_destroy) ;
	    setpshared	(this,ptrwamem_setpshared) ;
	    magic = 0 ;
	} ; /* end ctor */
	ptrwa(const ptrwa &) = delete ;
	ptrwa &operator = (const ptrwa &) = delete ;
	int getpshared(int *) noex ;
	void dtor() noex ;
	destruct ptrwa() {
	    if (magic) dtor() ;
	} ; /* end dtor (ptrwa) */
} ; /* end class (ptrwa) */
#else
typedef	PTRWA		ptrwa ;
#endif /* __cplusplus */

EXTERNC_begin

extern int	ptrwa_create		(ptrwa *) noex ;
extern int	ptrwa_destroy		(ptrwa *) noex ;
extern int	ptrwa_setpshared	(ptrwa *,int) noex ;
extern int	ptrwa_getpshared	(ptrwa *,int *) noex ;

EXTERNC_end


#endif /* PTRWA_INCLUDE */


