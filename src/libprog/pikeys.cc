/* keys SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* handle the keys while processing a file */
/* version %I% last-modified %G% */


/* revision history:

	= 1994-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	keys

	Description:
	This is a set of subroutines for managing keys while
	processing an input file (of them).

*******************************************************************************/

#include	<envstandards.h>	/* must be before others */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<bfile.h>
#include	<hdb.h>
#include	<ptm.h>
#include	<localmisc.h>

#include	"config.h"
#include	"defs.h"


/* local defines */

#define	PI	proginfo


/* external subroutines */


/* external variables */


/* forward references */

extern "C" {
    int	keys_ender(PI *,hdb *,bfile *,ptm *,cchar *,off_t,int) noex ;
}


/* local variables */


/* exported variables */


/* exported subroutines */

int keys_begin(PI *pip,hdb *dbp,int hashsize) noex {
	int		rs = SR_FAULT ;
	if (pip && dbp) {
	    cint	f = false ;
	    rs = hdb_start(dbp,hashsize,f,nullptr,nullptr) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (keys_begin) */

int keys_add(PI *pip,hdb *dbp,cchar *sp,int sl) noex {
	hdb_dat		key ;
	hdb_dat		value ;
	hdb_dat		dumbvalue ;
	cint		nrs = SR_NOTFOUND ;
	int		rs = SR_OK ;
	int		f = false ;

	if (pip == nullptr) return SR_FAULT ;

	if (sl < 0) sl = strlen(sp) ;

	key.len = sl ;
	key.buf = sp ;			/* prepare for check */
	value.len = 0 ;
	value.buf = nullptr ;
	/* if it is already present, we're done, return */
	if ((rs = hdb_fetch(dbp,key,nullptr,&dumbvalue)) == nrs) {
	    cchar	*cp ;
	    if ((rs = uc_mallocstrw(sp,sl,&cp)) >= 0) {
	        f = true ;
	        key.buf = (void *) cp ;
	        rs = hdb_store(dbp,key,value) ;
	    }
	} /* end if */

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (keys_add) */

int keys_end(PI *pip,hdb *dbp,bfile *ofp,ptm *omp,cc *fn,
		off_t roff,int rlen) noex {
	int		rs ;
	int		rs1 ;
	int		rv = 0 ;
	if ((rs = ptm_lock(omp)) >= 0) {
	    {
	        rs = keys_ender(pip,dbp,ofp,omp,fn,roff,rlen) ;
		rv = rs ;
	    }
	    rs1 = ptm_unlock(omp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ptm) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (keys_end) */


/* local subroutines */

int keys_ender(PI *pip,hdb *dbp,bfile *ofp,ptm omp,cc *fn,
		off_t roff,int rlen) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		n = 0 ;

	if (roff <= INT_MAX) {
	    dhB_cur	keycursor ;
	    hdb_dat	key ;
	    hdb_dat	value ;
	    uint	tagoff = roff ;
	    uint	taglen = rlen ;

	    if ((rs = hdb_curbegin(dbp,&keycursor)) >= 0) {

	        if (hdb_curenum(dbp,&keycursor,&key,&value) >= 0) {

	            if (! pip->fl.removelabel) {
	                bprintf(ofp,"%s:%u,%u\t",fn,tagoff,taglen) ;
		    }

	            rs = bwrite(ofp,key.buf,key.len) ;

	            n = 1 ;
	            while ((rs >= 0) && 
	                ((pip->maxkeys < 0) || (n < pip->maxkeys)) && 
	                (hdb_curenum(dbp,&keycursor,&key,&value) >= 0)) {

	                if ((rs = bputc(ofp,' ')) >= 0) {
	                    rs = bwrite(ofp, (void *) key.buf,key.len) ;
			}

	                n += 1 ;
	            } /* end while */

	            if (rs >= 0)
	                rs = bputc(ofp,'\n') ;

	        } /* end if */

	        hdb_curend(dbp,&keycursor) ;
	    } /* end if (cursor) */

/* delete this whole DB */

	    if ((rs1 = hdb_curbegin(dbp,&keycursor)) >= 0) {
	        while (hdb_curenum(dbp,&keycursor,&key,&value) >= 0) {
	            if (key.buf) {
	                rs1 = uc_free((void *) key.buf) ;
			if (rs >= 0) rs = rs1 ;
	            }
	        } /* end while */
	        rs1 = hdb_curend(dbp,&keycursor) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (cursor) */
	    if (rs >= 0) rs = rs1 ;

	    rs1 = hdb_finish(dbp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (within range) */

	return (rs >= 0) ? n : rs ;
}
/* end subroutine (keys_ender) */


