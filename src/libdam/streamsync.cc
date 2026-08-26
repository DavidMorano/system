/* streamsync SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* stream synchronization operations */
/* version %I% last-modified %G% */

#define	CF_MEMCMP	0		/* use 'memcmp(3c)' */

/* revision history:

	= 1998-12-01, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	streamsync

	Description:
	This obeject is used to acquire synchronization on a data
	stream.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"streamsync.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#ifndef	CF_MEMCMP
#define	CF_MEMCMP	0
#endif


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* forward references */

template<typename ... Args>
local inline int streamsync_magic(streamsync *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == STREAMSYNC_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (streamsync_magic) */


/* local variables */

constexpr bool		f_memcmp = CF_MEMCMP ;


/* exported variables */


/* exported subroutines */

int streamsync_start(streamsync *op,cchar *st,int stlen) noex {
	int		rs = SR_FAULT ;
	int		sz ;
	if (op && st) ylikely {
	    rs = SR_INVALID ;
	    if (stlen < 0) stlen = lenstr(st) ;
	    memclear(op) ;
	    if (stlen >= 1) {
	        op->i = 0 ;
	        op->stlen = stlen ;
	        sz = (2 * stlen * szof(char)) ;
	        if (char *p ; (rs = mem.mall(sz,&p)) >= 0) {
	            op->st = (p + 0) ;
	            op->data = (p + stlen) ;
	            memcopy(op->st,st,stlen) ;
	            memclear(op->data,stlen) ;
	            op->magval = STREAMSYNC_MAGIC ;
	        } /* end if (m-a) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (streamsync_start) */

int streamsync_finish(streamsync *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = streamsync_magic(op)) >= 0) ylikely {
	    if (op->st) ylikely {
	        rs1 = mem.free(op->st) ;
	        if (rs >= 0) rs = rs1 ;
	        op->st = nullptr ;
	    } /* end if (memory-release) */
	    op->magval = 0 ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (streamsync_finish) */

int streamsync_test(streamsync *op,int ch) noex {
	int		rs ;
	int		f = false ;
	if ((rs = streamsync_magic(op)) >= 0) {
	    int		i ; /* used-afterwards */
	    /* shift the new byte in */
	    for (i = 0 ; i < (op->stlen - 1) ; i += 1) {
	        op->data[i] = op->data[i + 1] ;
	    } /* end for */
	    op->data[i] = char(ch) ;
	    /* now do the test */
	    if (op->data[i] == op->st[i]) {
		if_constexpr (f_memcmp) {
	    	    f = (memcmp(op->st,op->data,i) == 0) ;
		} else {
	            int	j ; /* used-afterwards */
	            for (j = 0 ; j < i ; j += 1) {
	                if (op->st[j] != op->data[j]) break ;
	            } /* end for */
	            f = (j >= i) ;
		} /* end if_constexpr (f_memcmp) */
	    } /* end if (test good so far) */
	} /* end if (magic) */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (streamsync_test) */

int streamsync::start(cchar *stp,int stl) noex {
	return streamsync_start(this,stp,stl) ;
} /* emd method */

int streamsync::test(int ch) noex {
	return streamsync_test(this,ch) ;
} /* emd method */

void streamsync::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("streamsync",rs,"dtor-finish") ;
	}
} /* end method */

streamsync::operator int () noex {
    	int		rs = SR_NOTOPEN ;
	if (magval == STREAMSYNC_MAGIC) {
	    rs = i ;
	}
	return rs ;
} /* end method */

streamsync_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case streamsyncmem_finish:
	        rs = streamsync_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (streamsync_co::operator) */


