/* setstr SUPPORT */
/* lang=C99 */

/* set of strings */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This object provides am ordered set of integers. No two
	strings can be the same (desired for these purposes).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<climits>
#include	<string>
#include	<usystem.h>
#include	<localmisc.h>

#include	"setstr.h"


/* local defines */


/* default name spaces */


/* external subroutines */


/* forward references */


/* local variables */


/* exported subroutines */

int setstr_start(setstr *op,int n) noex {
	cint		at = true ;
	return hdb_start(op,n,at,nullptr,nullptr) ;
}
/* end subroutine (setstr_start) */

int setstr_finish(setstr *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ;
	if (op) {
	    rs = SR_OK ;
	    {
	        hdb_cur		cur ;
	        hdb_dat		key ;
	        hdb_dat		val ;
	        cint		rsn = SR_NOTFOUND ;
	        if ((rs1 = hdb_curbegin(op,&cur)) >= 0) {
	            int		rs2 ;
	            while ((rs2 = hdb_enum(op,&cur,&key,&val)) >= 0) {
	                if (key.buf != nullptr) {
		            c += 1 ;
	                    rs1 = uc_free(key.buf) ;
	                    if (rs >= 0) rs = rs1 ;
	                }
	            } /* end while (enum) */
	            if ((rs >= 0) && (rs2 != rsn)) rs = rs2 ;
	            rs2 = hdb_curend(op,&cur) ;
	            if (rs >= 0) rs = rs2 ;
	        } /* end if (cursor) */
	        if ((rs >= 0) && (rs1 != rsn)) rs = rs1 ;
	    }
	    {
	        rs1 = hdb_finish(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (setstr_finish) */

int setstr_already(setstr *op,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		f = true ;
	if (op && sp) {
	    hdb_dat	key, val ;
	    const int	rsn = SR_NOTFOUND ;
	    if (sl < 0) sl = strlen(sp) ;
	    key.buf = sp ;
	    key.len = sl ;
	    val.buf = sp ;
	    val.len = sl ;
	    if ((rs = hdb_fetch(op,key,nullptr,&val)) == rsn) {
	        f = false ;
	        rs = SR_OK ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (setstr_already) */

int setstr_add(setstr *op,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		f = false ;
	if (op && sp) {
	    hdb_dat	key, val ;
	    cint	rsn = SR_NOTFOUND ;
	    if (sl < 0) sl = strlen(sp) ;
	    key.buf = sp ;
	    key.len = sl ;
	    val.buf = sp ;
	    val.len = sl ;
	    if ((rs = hdb_fetch(op,key,nullptr,&val)) == rsn) {
	        cchar	*asp ;
	        if ((rs = uc_mallocstrw(sp,sl,&asp)) >= 0) {
		    key.buf = asp ;
		    val.buf = asp ;
	            f = true ;
	            rs = hdb_store(op,key,val) ;
		    if (rs < 0) {
		        uc_free(asp) ;
		    }
	        } /* end if (m-a) */
	    } /* end if (not already present) */
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (setstr_add) */

int setstr_del(setstr *op,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		f = false ;
	if (op && sp) {
	    hdb_dat	key, val ;
	    cint	rsn = SR_NOTFOUND ;
	    if (sl < 0) sl = strlen(sp) ;
	    key.buf = sp ;
	    key.len = sl ;
	    if ((rs = hdb_fetch(op,key,nullptr,&val)) >= 0) {
	        cchar	*asp = (cchar *) val.buf ;
		{
	            rs1 = hdb_delkey(op,key) ;
	            if (rs >= 0) rs = rs1 ;
		}
		{
	            rs1 = uc_free(asp) ;
	            if (rs >= 0) rs = rs1 ;
		}
	        f = true ;
	    } else if (rs == rsn) {
	        rs = SR_OK ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (setstr_del) */

int setstr_count(setstr *op) noex {
	return hdb_count(op) ;
}
/* end subroutine (setstr_count) */

int setstr_curbegin(setstr *op,setstr_cur *curp) noex {
	return hdb_curbegin(op,curp) ;
}
/* end subroutine (setstr_curbegin) */

int setstr_curend(setstr *op,setstr_cur *curp) noex {
	return hdb_curend(op,curp) ;
}
/* end subroutine (setstr_curend) */

int setstr_enum(setstr *op,setstr_cur *curp,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	int		rl = 0 ;
	if (op && curp && rpp) {
	    hdb_dat	key ;
	    hdb_dat	val ;
	    if ((rs = hdb_enum(op,curp,&key,&val)) >= 0) {
	        rl = val.len ;
	        if (rpp) {
	            *rpp = charp(val.buf) ;
	        }
	    } else {
	        if (rpp) *rpp = nullptr ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (setstr_enum) */


