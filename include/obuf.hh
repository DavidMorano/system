/* obuf HEADER */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* Output Buffer */
/* version %I% last-modified %G% */


/* revision history:

	= 2016-06-29, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2016 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	obuf

	Description:
	See the source support file ('obuf.cc') for a descruption
	of this object.

*******************************************************************************/

#ifndef	OBUF_INCLUDE
#define	OBUF_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* <- for |strlen(3c)| */
#include	<string>		/* <- C++ |string| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<mkchar.h>
#include	<localmisc.h>


enum obufmems {
	obufmem_start,
	obufmem_count,
	obufmem_len,
	obufmem_finish,
	obufmem_overlast
} ;
struct obuf ;
struct obuf_co {
	obuf		*op = nullptr ;
	int		w = -1 ;
	void operator () (obuf *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (obuf_co) */
class obuf {
    	friend		obuf_co ;
	std::string	b ;
	int		oi ;		/* output index */
	int push(int) noex ;
	int ilen() const noex {
	    return (b.size() - oi) ;
	} ;
public:
	obuf_co		start ;
	obuf_co		finish ;
	obuf_co		count ;
	obuf_co		len ;
	obuf(const obuf &) = delete ;
	obuf &operator = (const obuf &) = delete ;
	obuf(cchar *sp = nullptr,int sl = -1) noex : oi(0) {
	    obuf_co	start(this,obufmem_start) ;
	    obuf_co	finish(this,obufmem_finish) ;
	    obuf_co	count(this,obufmem_count) ;
	    obuf_co	len(this,obufmem_len) ;
	    if (sp) {
	        if (sl < 0) {
		    sl = strlen(sp) ;
		}
		try {
	            for (int i = 0 ; sp[i] && sl ; i += 1) {
		        b.push_back(sp[i]) ;
	            }
		} catch (...) {
		}
	    } /* end if (non-null) */
	} ; /* end ctor */
	int operator [] (int i) const noex {
	    cint	n = b.size() ;
	    int		rch = 0 ;
	    if ((oi+i) < n) {
		rch = mkchar(b[oi+i]) ;
	    }
	    return rch ;
	} ;
	obuf &operator += (int ch) noex {
	    push(ch) ;
	    return *this ;
	} ;
	int add(int ch) noex {
	    return push(ch) ;
	} ;
	int add(cchar *sp,int sl = -1) noex {
	    int		rs = SR_OK ;
	    if (sl < 0) sl = strlen(sp) ;
	    for (int i = 0 ; (rs >= 0) && (i < sl) ; i += 1) {
		rs = push(sp[i]) ;
	    }
	    if (rs >= 0) {
	        rs = (b.size() - oi) ;
	    }
	    return rs ;
	} ;
	int at(int i) const noex {
	    cint	n = b.size() ;
	    int		rch = 0 ;
	    if ((oi+i) < n) {
		rch = mkchar(b[oi+i]) ;
	    }
	    return rch ;
	} ;
	int adv(int) noex ;
	void dtor() noex ;
	~obuf() {
	    dtor() ;
	} ; /* end dtor */
	operator int () const noex {
	    return ilen() ;
	} ;
} ; /* end class (obuf) */


#endif /* __cplusplus */
#endif /* OBUF_INCLUDE */


