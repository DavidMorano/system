/* sysusernames */
/* lang=C++20 */

/* system user-name enumeration */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-24, David A­D­ Morano
	This object module was morphed from some previous one. I
	do not remember what the previous one was.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	We enumerate (reentrantly and thread safely) user-name
	entries from the system USER-NAME database.

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
#include	<localmisc.h>
#include	<snwcpy.h>

#include	"sysusernames.h"


/* local defines */


/* external subroutines */


/* local structures */


/* forward references */


/* local variables */

constexpr cchar		*defufname = SYSUSERNAMES_FNAME ;


/* exported subroutines */

int sysusernames_open(sysusernames *op,cchar *sufname) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    csize	max = INT_MAX ;
	    cint	of = O_RDONLY ;
	    if (sufname == nullptr) sufname = defufname ;
	    memclear(op,sizeof(SYSUSERNAMES)) ;
	    if ((rs = filemap_open(&op->b,sufname,of,max)) >= 0) {
	    	op->magic = SYSUSERNAMES_MAGIC ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end if (sysusernames_start) */

int sysusernames_close(sysusernames *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->magic == SYSUSERNAMES_MAGIC) {
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
/* end subroutine (sysusernames_close) */

int sysusernames_readent(sysusernames *op,char *ubuf,int ulen) noex {
	int		rs = SR_FAULT ;
	if (op && ubuf) {
	    rs = SR_NOTOPEN ;
	    ubuf[0] = '\0' ;
	    if (op->magic == SYSUSERNAMES_MAGIC) {
		cchar	*lp ;
	        while ((rs = filemap_getline(&op->b,&lp)) > 0) {
	            int		ll = rs ;
	            if (lp[ll-1] == '\n') ll -= 1 ;
	            rs = snwcpy(ubuf,ulen,lp,ll) ;
	            if (rs > 0) break ;
	            if (rs < 0) break ;
	        } /* end while */
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sysusernames_readent) */

int sysusernames_reset(sysusernames *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_NOTOPEN ;
	    if (op->magic == SYSUSERNAMES_MAGIC) {
		rs = filemap_rewind(&op->b) ;
	    } /* end if (open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sysusernames_reset) */


