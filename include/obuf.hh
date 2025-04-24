/* obuf HEADER */
/* encoding=ISO8859-1 */
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
	obuf

	Description:
	See the source support file ('obuf.cc') for a description
	of this object.

*******************************************************************************/

#ifndef	OBUF_INCLUDE
#define	OBUF_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<string>		/* |string(3c++)| */
#include	<string_view>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<libutil.hh>		/* |xstrlen(3u)| */
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
	    csize	bsize = b.size() ;
	    int		rs ;
	    rs = intconv(bsize) - oi ;
	    return rs ;
	} ;
public:
	obuf_co		start ;
	obuf_co		finish ;
	obuf_co		count ;
	obuf_co		len ;
	obuf(const obuf &) = delete ;
	obuf &operator = (const obuf &) = delete ;
	obuf(cchar *sp = nullptr,int sl = -1) noex : oi(0) {
	    start(this,obufmem_start) ;
	    finish(this,obufmem_finish) ;
	    count(this,obufmem_count) ;
	    len(this,obufmem_len) ;
	    if (sp) {
	        if (sl < 0) {
		    sl = xstrlen(sp) ;
		}
		try {
		    std::string_view	sv(sp,sl) ;
		    b += sv ;
		} catch (...) {
		    (void) sp ;
		}
	    } /* end if (non-null) */
	} ; /* end ctor */
	int operator [] (int i) const noex {
	    cint	n = intconv(b.size()) ;
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
	    if (sl < 0) sl = xstrlen(sp) ;
	    try {
		std::string_view	sv(sp,sl) ;
		b += sv ;
		{
		    csize	bsize = b.size() ;
	            rs = (intconv(bsize) - oi) ;
		}
	    } catch (...) {
		rs = SR_NOMEM ;
	    }
	    return rs ;
	} ;
	int at(int i) const noex {
	    csize	bsize = b.size() ;
	    int		n ;
	    int		rch = 0 ;
	    n = intconv(bsize) ;
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
	operator int () noex {
	    return len() ;
	} ;
} ; /* end class (obuf) */


#endif /* __cplusplus */
#endif /* OBUF_INCLUDE */


