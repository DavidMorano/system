/* sysusers */
/* lang=C++20 */

/* system user-entry enumeration */
/* version %I% last-modified %G% */

#define	CF_UGETPW	1		/* use |ugetpw(3uc)| */

/* revision history:

	= 1998-03-24, David A­D­ Morano
	This object module was morphed from some previous one. I
	do not remember what the previous one was.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	We enumerate (reentrantly and thread safely) user names
	from the system PASSWD database.

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<limits.h>
#include	<unistd.h>
#include	<stdlib.h>
#include	<string.h>
#include	<usystem.h>
#include	<usupport.h>
#include	<getax.h>
#include	<ugetpw.h>
#include	<localmisc.h>
#include	<getbufsize.h>

#include	"sysusers.h"


/* local defines */

#if	CF_UGETPW
#define	GETPW_NAME	ugetpw_name
#else
#define	GETPW_NAME	getpw_name
#endif /* CF_UGETPW */
#undef	COMMENT


/* external subroutines */

extern "C" {
    extern char	*strdcpy1w(char *,int,cchar *,int) noex ;
}


/* local structures */


/* forward references */


/* local variables */

constexpr cchar		*defufname = SYSUSERS_FNAME ;


/* exported subroutines */

int sysusers_open(SYSUSERS *op,cchar *sufname) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    csize	max = INT_MAX ;
	    cint	of = O_RDONLY ;
	    if (sufname == nullptr) sufname = defufname ;
	    memclear(op,sizeof(SYSUSERS)) ;
	    if ((rs = filemap_open(&op->b,sufname,of,max)) >= 0) {
	        op->magic = SYSUSERS_MAGIC ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end if (sysusers_start) */

int sysusers_close(SYSUSERS *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->magic == SYSUSERS_MAGIC) {
		rs = SR_OK ;
		{
	            rs1 = filemap_close(&op->b) ;
	            if (rs >= 0) rs = rs1 ;
		}
	        op->magic = 0 ;
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
} 
/* end subroutine (sysusers_close) */

int sysusers_readent(SYSUSERS *op,PASSWD *pwp,char *pwbuf,int pwlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op && pwp && pwbuf) {
	    rs = SR_NOTOPEN ;
	    if (op->magic == SYSUSERS_MAGIC) {
		if ((rs = getbufsize(getbufsize_un)) >= 0) {
		    cint	ulen = rs ;
		    char	*ubuf ;
		    if ((rs = uc_libmalloc((ulen+1),&ubuf)) >= 0) {
		        cchar	*lp ;
		        while ((rs = filemap_getline(&op->b,&lp)) > 0) {
	    	            int		ll = rs ;
	    	            if (lp[ll-1] == '\n') ll -= 1 ;
	    	            if ((rs = (strdcpy1w(ubuf,ulen,lp,ll)-ubuf)) > 0) {
		                rs = GETPW_NAME(pwp,pwbuf,pwlen,ubuf) ;
		                if (rs == SR_NOTFOUND) rs = SR_OK ;
	    	            }
	    	            if (rs > 0) break ;
	    	            if (rs < 0) break ;
		        } /* end while */
			rs1 = uc_libfree(ubuf) ;
			if (rs >= 0) rs = rs1 ;
		    } /* end if (m-a-f) */
		} /* end if (getbufsize) */
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sysusers_readent) */

int sysusers_reset(SYSUSERS *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->magic == SYSUSERS_MAGIC) {
		rs = filemap_rewind(&op->b) ;
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sysusers_reset) */


