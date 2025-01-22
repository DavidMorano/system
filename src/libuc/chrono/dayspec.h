/* dayspec HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* load a day-specification */
/* version %I% last-modified %G% */


/* revision history:

	= 2013-02-07, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2013 David A­D­ Morano.  All rights reserved. */

#ifndef	DAYSPEC_INCLUDE
#define	DAYSPEC_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	DAYSPEC		struct dayspec_head


struct dayspec_head {
	short		y ;
	schar		m, d ;
} ;

#ifdef	__cplusplus
enum dayspecmems {
	dayspecmem_def,
	dayspecmem_yday,
	dayspecmem_overlast
} ;
struct dayspec ;
struct dayspec_co {
	dayspec		*op = nullptr ;
	int		w = -1 ;
	void operator () (dayspec *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (dayspec_co) */
struct dayspec : dayspec_head {
	dayspec_co	def ;
	dayspec_co	yday ;
	dayspec() noex {
	    def(this,dayspecmem_def) ;
	    yday(this,dayspecmem_yday) ;
	} ;
	dayspec(const dayspec &) = delete ;
	dayspec &operator = (const dayspec &) = delete ;
	int load(cchar *,int = -1) noex ;
} ; /* end struct (dayspec) */
#else	/* __cplusplus */
typedef DAYSPEC		dayspec ;
#endif /* __cplusplus */

EXTERNC_begin

extern int dayspec_def(dayspec *) noex ;
extern int dayspec_load(dayspec *,cchar *,int) noex ;
extern int dayspec_yday(dayspec *) noex ;

EXTERNC_end


#endif /* DAYSPEC_INCLUDE */


