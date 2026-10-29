/* memo SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* memory allocation object */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */

/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object:
	memo

	Descriptor:
	This object allocates and de-allocates memory.  If explicit
	de-allocation is not called for, automatic de-allocation
	occurs anyway when the object goes out of scope.  It is the
	resppnsibility of the caller to not use the returned pointer
	to the allocated memory after this object goes out of scope
	(because the references allocated memory gets de-allocated).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ulogerror.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<bufsizes.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<dprint.hh>		/* LIBU |DPRINTF(3u)| */

#include	"memo.hh"		/* LIBU */

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

cbool		f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int memo::mall	(voidp vpp,int sz) noex {
	int rs ;
	if ((rs = mem.mall(sz,vpp)) >= 0) {
	    voidpp rpp = voidpp(vpp) ;
	    rp = *rpp ;
	    rl = rs ;
	} /* end */
	return rs ;
} /* end */
int memo::vall	(voidp vpp,int sz) noex {
	int rs ;
	if ((rs = mem.vall(sz,vpp)) >= 0) {
	    voidpp rpp = voidpp(vpp) ;
	    rp = *rpp ;
	    rl = rs ;
	} /* end */
	return rs ;
} /* end */
int memo::call	(voidp vpp,int ne,int sz) noex {
	int rs ;
	if ((rs = mem.call(ne,sz,vpp)) >= 0) {
	    voidpp rpp = voidpp(vpp) ;
	    rp = *rpp ;
	    rl = rs ;
	} /* end */
	return rs ;
} /* end */
int memo::rall	(voidp vpp,int sz,voidp cp) noex {
	int rs ;
	if ((rs = mem.rall(cp,sz,vpp)) >= 0) {
	    voidpp rpp = voidpp(vpp) ;
	    rp = *rpp ;
	    rl = rs ;
	} /* end */
	return rs ;
} /* end */
int memo::sysbuf(voidp vpp,int w) noex {
    	return allocbuf(vpp,w) ;
} /* end */
int memo::strw	(ccharpp rpp,ccharp sp,int sl) noex {
    	return mem.strw(sp,sl,rpp) ;
} /* end */
int memo::strw	(ccharpp rpp,con stringview &sv) noex {
	ccharp	sp = sv.data() ;
	cint	sl = (int) sv.length() ;
    	return mem.strw(sp,sl,rpp) ;
} /* end */
int memo::ml		(char **rpp) noex {
    	return allocstr(rpp,bufsize_ml) ;
} /* end */
int memo::mn		(char **rpp) noex {
    	return allocstr(rpp,bufsize_mn) ;
} /* end */
int memo::mp		(char **rpp) noex {
    	return allocstr(rpp,bufsize_mp) ;
} /* end */
int memo::nn		(char **rpp) noex {
    	return allocstr(rpp,bufsize_nn) ;
} /* end */
int memo::un		(char **rpp) noex {
    	return allocstr(rpp,bufsize_un) ;
} /* end */
int memo::gn		(char **rpp) noex {
    	return allocstr(rpp,bufsize_gn) ;
} /* end */
int memo::pn		(char **rpp) noex {
    	return allocstr(rpp,bufsize_pn) ;
} /* end */
int memo::protname	(char **rpp) noex {
    	return allocstr(rpp,bufsize_protname) ;
} /* end */
int memo::netwname	(char **rpp) noex {
    	return allocstr(rpp,bufsize_netwname) ;
} /* end */
int memo::hostname	(char **rpp) noex {
    	return allocstr(rpp,bufsize_hostname) ;
} /* end */
int memo::servname	(char **rpp) noex {
    	return allocstr(rpp,bufsize_servname) ;
} /* end */
int memo::mailaddr	(char **rpp) noex {
    	return allocstr(rpp,bufsize_mailaddr) ;
} /* end */
int memo::fs		(char **rpp) noex {
    	return allocstr(rpp,bufsize_fs) ;
} /* end */
int memo::sn		(char **rpp) noex {
    	return allocstr(rpp,bufsize_sn) ;
} /* end */
int memo::zn		(char **rpp) noex {
    	return allocstr(rpp,bufsize_zn) ;
} /* end */
int memo::zi		(char **rpp) noex {
    	return allocstr(rpp,bufsize_zi) ;
} /* end */
int memo::ad		(char **rpp) noex {
    	return allocstr(rpp,bufsize_mailaddr) ;
} /* end */
int memo::ps		(void *vpp) noex {
    	return allocbuf(vpp,bufsize_ps) ;
} /* end */
int memo::ma		(void *vpp) noex {
    	return allocbuf(vpp,bufsize_ma) ;
} /* end */
int memo::ut		(void *vpp) noex {
    	return allocbuf(vpp,bufsize_ut) ;
} /* end */
int memo::pw		(void *vpp) noex {
    	return allocbuf(vpp,bufsize_pw) ;
} /* end */
int memo::sp		(void *vpp) noex {
    	return allocbuf(vpp,bufsize_sp) ;
} /* end */
int memo::ua		(void *vpp) noex {
    	return allocbuf(vpp,bufsize_ua) ;
} /* end */
int memo::gr		(void *vpp) noex {
    	return allocbuf(vpp,bufsize_gr) ;
} /* end */
int memo::pj		(void *vpp) noex {
    	return allocbuf(vpp,bufsize_pj) ;
} /* end */
/* entry-protocol	|ucentpr(3uc)| */
int memo::pr		(void *vpp) noex {
    	return allocbuf(vpp,bufsize_pj) ;
} /* end */
/* entry-network	|ucentnw(3uc| */
int memo::nw		(void *vpp) noex {
    	return allocbuf(vpp,bufsize_nw) ;
} /* end */
/* entry-host		|ucentho(3uc)|  */
int memo::ho		(void *vpp) noex {
    	return allocbuf(vpp,bufsize_ho) ;
} /* end */
/* entry-service	|ucentsv(3uc)|  */
int memo::sv		(void *vpp) noex {
    	return allocbuf(vpp,bufsize_sv) ;
} /* end */

int memo::allocstr(charpp rpp,int w) noex {
	return allocbuf(rpp,w) ;
} /* end method (memo::allocstr) */

int memo::allocbuf(voidp vpp,int w) noex {
    	int rs ;
	if ((rs = mem.sysbuf(w,vpp)) >= 0) {
	    voidpp rpp = voidpp(vpp) ;
	    rp = *rpp ;
	    rl = rs ;
	} /* end */
	return rs ;
} /* end method (memo::allocbuf) */

int memo::free(void *ptr) noex {
    	int	rs = SR_FAULT ;
	if (ptr) ylikely {
	    rs = SR_NODATA ;
	    if (ptr == rp) {
	        rs = mem.free(ptr) ;
	        rp = nullptr ;
	        rl = -1 ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end method (memo::free) */

void memo::dtor() noex {
	if (rp) ylikely {
	    cint rs = mem.free(rp) ;
	    rp = nullptr ;
	    rl = -1 ;
	    if (rs < 0) {
		ulogerror("memo",rs,"dtor") ;
	    } /* end */
	} /* end */
} /* end method (memo::dtor) */

memo::operator int () const noex {
    	return getlen() ;
} /* end method (memo::operator) */

memo_len::operator int () const noex {
    	return op->getlen() ;
} /* end method (memo_len::operator) */

int memo::getlen() const noex {
    	int	rs = SR_NOTOPEN ;
	if (rp) ylikely {
	    if (rl >= 0) ylikely {
	        rs = rl ;
	    }
	} /* end if */
    	return rs ;
} /* end method (memo::getlen) */


