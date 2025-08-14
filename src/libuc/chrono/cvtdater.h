/* cvtdater HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* date conversion object */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CVTDATER_INCLUDE
#define	CVTDATER_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<time.h>		/* |time_t| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	CVTDATER	struct cvtdater_head


struct cvtdater_head {
	time_t		daytime ;
} ; /* end struct (cvtdater_head) */

#ifdef	__cplusplus
enum cvtdatermems {
	cvtdatermem_finish,
	cvtdatermem_overlast
} ;
struct cvtdater ;
struct cvtdater_co {
	cvtdater	*op = nullptr ;
	int		w = -1 ;
	void operator () (cvtdater *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (cvtdater_co) */
struct cvtdater : cvtdater_head {
	cvtdater_co	finish ;
	cvtdater() noex {
	    finish(this,cvtdatermem_finish) ;
	    daytime = 0 ;
	} ;
	cvtdater(const cvtdater &) = delete ;
	cvtdater &operator = (const cvtdater &) = delete ;
	int start(time_t = 0L) noex ;
	int load(time_t *,cchar *,int = -1) noex ;
	void dtor() noex ;
	destruct cvtdater() {
	    if (daytime) dtor() ;
	} ;
} ; /* end struct (cvtdater) */
#else	/* __cplusplus */
typedef CVTDATER	cvtdater ;
#endif /* __cplusplus */

EXTERNC_begin

extern int	cvtdater_finish(cvtdater *) noex ;

EXTERNC_end

#ifdef	__cplusplus

extern "C" {
    extern int	cvtdater_start(cvtdater *,time_t = 0L) noex ;
    extern int	cvtdater_load(cvtdater *,time_t *,cchar *,int = -1) noex ;
}

#else	/* __cplusplus */

extern int	cvtdater_start(cvtdater *,time_t) noex ;
extern int	cvtdater_load(cvtdater *,time_t *,cchar *,int) noex ;

#endif /* __cplusplus */


#endif /* CVTDATER_INCLUDE */


