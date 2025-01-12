/* streamsync SUPPORT */
/* encoding=ISO8859-1 */
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

	This obeject is used to acquire synchronization on a data
	stream.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<localmisc.h>

#include	"streamsync.h"


/* local defines */

#ifndef	CF_MEMCMP
#define	CF_MEMCMP	0
#endif


/* external subroutines */


/* external variables */


/* forward references */


/* local variables */

constexpr bool		f_memcmp = CF_MEMCMP ;


/* exported variables */


/* exported subroutines */

int streamsync_start(streamsync *ssp,cchar *st,int stlen) noex {
	int		rs = SR_FAULT ;
	int		sz ;
	if (ssp && st) {
	    rs = SR_INVALID ;
	    if (stlen < 0) stlen = strlen(st) ;
	    memclear(ssp) ;
	    if (stlen >= 1) {
	        char	*p ;
	        ssp->i = 0 ;
	        ssp->stlen = stlen ;
	        sz = (2 * stlen * szof(char)) ;
	        if ((rs = uc_malloc(sz,&p)) >= 0) {
	            ssp->st = p + 0 ;
	            ssp->data = p + stlen ;
	            memcpy(ssp->st,st,stlen) ;
	            memset(ssp->data,0,stlen) ;
	            ssp->magic = STREAMSYNC_MAGIC ;
	        } /* end if (m-a) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (streamsync_start) */

int streamsync_finish(streamsync *ssp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (ssp) {
	    rs = SR_OK ;
	    if (ssp->st) {
	        rs1 = uc_free(ssp->st) ;
	        if (rs >= 0) rs = rs1 ;
	        ssp->st = nullptr ;
	    }
	    ssp->magic = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (streamsync_finish) */

int streamsync_test(streamsync *ssp,int ch) noex {
	int		rs = SR_FAULT ;
	int		f = false ;
	if (ssp) {
	    int		i ; /* used afterwards */
	    rs = SR_OK ;
	    /* shift the new byte in */
	    for (i = 0 ; i < (ssp->stlen - 1) ; i += 1) {
	        ssp->data[i] = ssp->data[i + 1] ;
	    }
	    ssp->data[i] = ch ;
	    /* now do the test */
	    if (ssp->data[i] == ssp->st[i]) {
		if_constexpr (f_memcmp) {
	    	    f = (memcmp(ssp->st,ssp->data,i) == 0) ;
		} else {
	            int	j ;
	            for (j = 0 ; j < i ; j += 1) {
	                if (ssp->st[j] != ssp->data[j]) break ;
	            } /* end for */
	            f = (j >= i) ;
		} /* end if_constexpr (f_memcmp) */
	    } /* end if (test good so far) */
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (streamsync_test) */


