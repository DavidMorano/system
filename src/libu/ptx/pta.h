/* pta HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* POSIX Thread Attribute manipulation */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-07-28, David A-D- Morano
	Originally written for Rightcore Network Services.

*/
/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

#ifndef	PTA_INCLUDE
#define	PTA_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<pthread.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	PTA	pthread_attr_t


#ifdef	__cplusplus
enum ptamems {
	ptamem_create,
	ptamem_destroy,
	ptamem_setscope,
	ptamem_overlast
} ;
struct pta ;
struct pta_co {
        pta             *op = nullptr ;
        int             w = -1 ;
        constexpr void operator () (pta *p,int m) noex {
            op = p ;
            w = m ;
        } ;
        int operator () (int a = -1) noex ;
        operator int () noex {
	    return operator () (-1) ;
	}
} ; /* end struct (pta_co) */
struct pta : pthread_attr_t {
	pta_co		create ;
	pta_co		destroy ;
	pta_co		setscope ;
	constexpr pta() noex {
	    create(this,ptamem_create) ;
	    destroy(this,ptamem_destroy) ;
	    setscope(this,ptamem_setscope) ;
	} ;
	pta(const pta &) = delete ;
	pta &operator = (const pta &) = delete ;
	int setguardsize(size_t) noex ;
	int setstackaddr(void *) noex ;
	int setstacksize(size_t) noex ;
	void dtor() noex ;
	destruct pta() {
	    dtor() ;
	} ; /* end dtor (pta) */
} ; /* end class (pta) */
#else
typedef PTA	pta ;
#endif /* __cplusplus */

EXTERNC_begin

extern int	pta_create(pta *) noex ;
extern int	pta_destroy(pta *) noex ;
extern int	pta_setstacksize(pta *,size_t) noex ;
extern int	pta_getstacksize(pta *,size_t *) noex ;
extern int	pta_setguardsize(pta *,size_t) noex ;
extern int	pta_getguardsize(pta *,size_t *) noex ;
extern int	pta_setstackaddr(pta *,void *) noex ;
extern int	pta_getstackaddr(pta *,void **) noex ;
extern int	pta_setdetachstate(pta *,int) noex ;
extern int	pta_getdetachstate(pta *,int *) noex ;
extern int	pta_setscope(pta *,int) noex ;
extern int	pta_getscope(pta *,int *) noex ;
extern int	pta_setinheritsched(pta *,int) noex ;
extern int	pta_getinheritsched(pta *,int *) noex ;
extern int	pta_setschedpolicy(pta *,int) noex ;
extern int	pta_getschedpolicy(pta *,int *) noex ;
extern int	pta_setschedparam(pta *,const SCHEDPARAM *) noex ;
extern int	pta_getschedparam(pta *,SCHEDPARAM *) noex ;
extern int	pta_setstack(pta *,void *,size_t) noex ;

EXTERNC_end


#endif /* PTA_INCLUDE */


