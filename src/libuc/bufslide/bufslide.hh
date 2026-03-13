/* bufslide HEADER */
/* charset=ISO8859-1 */
/* lang=C++98 */

/* Output Buffer */
/* version %I% last-modified %G% */


/* revision history:

	= 2016-06-29, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2016 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	bufslide

	Description:
	See the source support file ('bufslide.cc') for a description
	of this object.

*******************************************************************************/

#ifndef	BUFSLIDE_INCLUDE
#define	BUFSLIDE_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<string>		/* |string(3c++)| */
#include	<clanguage.h>
#include	<usysbase.h>

enum bufslidemems {
	bufslidemem_start,
	bufslidemem_count,
	bufslidemem_len,
	bufslidemem_finish,
	bufslidemem_overlast
} ; /* end enum (bufslidemems) */

struct bufslide_fl {
    	uint		open:1 ;
} ; /* end struct (bufslide_fl) */

struct bufslide ;

struct bufslide_co {
	bufslide		*op = nullptr ;
	int		w = -1 ;
	void operator () (bufslide *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (bufslide_co) */

class bufslide {
    	friend		bufslide_co ;
	std::string	b ;
	int		oi ;		/* output index */
	int push(int) noex ;
	int istart() noex ;
	int ifinish() noex ;
	int ilen() const noex ;
public:
	bufslide_co		start ;
	bufslide_co		finish ;
	bufslide_co		count ;
	bufslide_co		len ;
	bufslide_fl		fl{} ;
	bufslide(const bufslide &) = delete ;
	bufslide &operator = (const bufslide &) = delete ;
	bufslide(cchar * = nullptr,int = -1) noex ;
	int add		(cchar *,int = -1) noex ;
	int add		(int) noex ;
	int at		(int) const noex ;
	int readat	(int,char *,int) const noex ;
	int read	(char *,int) const noex ;
	int adv		(int) noex ;
	void dtor() noex ;
	int operator [] (int) const noex ;
	operator int () const noex {
	    return ilen() ;
	} ;
	destruct bufslide() {
	    if (fl.open) dtor() ;
	} ; /* end dtor */
} ; /* end class (bufslide) */


#endif /* __cplusplus */
#endif /* BUFSLIDE_INCLUDE */


