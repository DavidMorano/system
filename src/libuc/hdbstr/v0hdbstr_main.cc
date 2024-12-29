/* hdbstr_main SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* Key-Value Hash DataBase for Strings */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-12, David A­D­ Morano
	This module was adapted from some previous code.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This module provides a hash access container object for strings.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<hdb.h>
#include	<strwcpy.h>
#include	<strdcpyx.h>
#include	<localmisc.h>

#include	"hdbstr.h"


/* local defines */

#ifndef	KBUFLEN
#define	KBUFLEN		120
#endif

#define	HS		hdbstr
#define	HS_CUR		hdbstr_cur


/* imported namespaces */


/* local typedefs */

typedef hdbstr_cur	cur ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int hdbstr_start(hdbstr *op,int n) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = hdb_start(op,n,0,nullptr,nullptr) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (hdbstr_start) */

int hdbstr_finish(hdbstr *op) noex {
    	cint		rsn = SR_NOTFOUND ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		n = 0 ;
	if (op) {
	    if (hdb_cur cur{} ; (rs = hdb_curbegin(op,&cur)) >= 0) {
	        hdb_dat		key{} ;
	        hdb_dat		val{} ;
	        while ((rs1 = hdb_curenum(op,&cur,&key,&val)) >= 0) {
	            if (key.buf != nullptr) {
	                rs1 = uc_free(key.buf) ;
	                if (rs >= 0) rs = rs1 ;
	            }
	            n += 1 ;
	        } /* end while (enum) */
		if ((rs >= 0) && (rs1 != rsn)) rs = rs1 ;
	        rs1 = hdb_curend(op,&cur) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (cursor) */
	    rs1 = hdb_finish(op) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (non-null) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (hdbstr_finish) */

/* add a string pair to the database */
int hdbstr_add(hdbstr *op,cchar *kstr,int klen,cchar *vstr,int vlen) noex {
	int		rs = SR_FAULT ;
	if (op && kstr) {
	    int		sz ;
	    if (klen < 0) klen = strlen(kstr) ;
	    if (vstr) {
	        if (vlen < 0) {
	            vlen = strlen(vstr) ;
	        }
	    } else {
	        vlen = 0 ;
	    }
	    sz = (klen + vlen + 2) ;
	    if (char *bp{} ; (rs = uc_malloc(sz,&bp)) >= 0) {
	        hdb_dat		key ;
	        hdb_dat		val ;
	        key.buf = bp ;
	        key.len = klen ;
	        bp = strwcpy(bp,kstr,klen) + 1 ;
	        val.buf = bp ;
	        val.len = vlen ;
	        if (vstr) strwcpy(bp,vstr,vlen) ;
	        rs = hdb_store(op,key,val) ;
	        if (rs < 0) {
	            uc_free(bp) ;
	        }
	    } /* end if (memory-allocation) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (hdbstr_add) */

/* enumerate all of the entries */
int hdbstr_curenum(hdbstr *op,cur *curp,cc **kpp,
		cc **vpp,int *vlenp) noex {
	int		rs = SR_FAULT ;
	int		kl = 0 ;
	if (op && curp) {
	    hdb_dat	key ;
	    hdb_dat	val{} ;
	    if (kpp) *kpp = nullptr ;
	    if ((rs = hdb_curenum(op,curp,&key,&val)) >= 0) {
	        kl = key.len ;
	        if (kpp) {
	            *kpp = charp(key.buf) ;
	        }
	        if (vpp) {
	            *vpp = charp(val.buf) ;
	        }
	        if (vlenp) {
	            *vlenp = val.len ;
	        }
	    } /* end if (had entry) */
	} /* end if (non-null) */
	return (rs >= 0) ? kl : rs ;
}
/* end subroutine (hdbstr_curenum) */

/* fetch the next entry value matching the given key */
int hdbstr_fetch(hdbstr *op,cc *kstr,int klen,cur *curp,cc **rpp) noex {
	int		rs = SR_FAULT ;
	int		vl = 0 ;
	if (op && curp) {
	    hdb_dat	key ;
	    hdb_dat	val{} ;
	    key.buf = kstr ;
	    key.len = (klen < 0) ? strlen(kstr) : klen ;
	    if (rpp) *rpp = nullptr ;
	    if ((rs = hdb_fetch(op,key,curp,&val)) >= 0) {
	        vl = val.len ;
	        if (rpp) {
	            *rpp = charp(val.buf) ;
		}
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (hdbstr_fetch) */

/* fetch the next entry value matching the given key */
int hdbstr_fetchrec(hdbstr *op,cc *kstr,int klen,cur *curp,cc **kpp,cc **vpp,
		int *vlenp) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (op && curp) {
	    hdb_dat	key ;
	    hdb_dat	val{} ;
	    hdb_dat	rkey{} ;
	    key.buf = kstr ;
	    key.len = (klen < 0) ? strlen(kstr) : klen ;
	    if (kpp) *kpp = nullptr ;
	    if ((rs = hdb_fetchrec(op,key,curp,&rkey,&val)) >= 0) {
	        if (kpp) {
	            *kpp = charp(rkey.buf) ;
	        }
	        if (vpp) {
	            *vpp = charp(val.buf) ;
	        }
	        if (vlenp) {
	            *vlenp = val.len ;
	        }
	        len = key.len ;
	    } /* end if (had entry) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (hdbstr_fetchrec) */

/* get the current record under the cursor */
int hdbstr_getrec(hdbstr *op,cur *curp,cchar **kpp,
		cchar **vpp,int *vlenp) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (op) {
	    hdb_dat	key{} ;
	    hdb_dat	val{} ;
	    if (kpp) *kpp = nullptr ;
	    if ((rs = hdb_getrec(op,curp,&key,&val)) >= 0) {
	        if (kpp) {
	            *kpp = charp(key.buf) ;
	        }
	        if (vpp) {
	            *vpp = charp(val.buf) ;
	        }
	        if (vlenp) {
	            *vlenp = val.len ;
	        }
	        len = val.len ;
	    } /* end if (had entry) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (hdbstr_getrec) */

/* advance the cursor to the next entry regardless of key */
int hdbstr_next(hdbstr *op,cur *curp) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = hdb_next(op,curp) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (hdbstr_next) */

/* advance the cursor to the next entry with the given key */
int hdbstr_nextkey(hdbstr *op,cchar *kstr,int klen,cur *curp) noex {
	int		rs = SR_FAULT ;
	if (op && kstr && curp) {
	     hdb_dat	key ;
	     key.buf = kstr ;
	     key.len = (klen < 0) ? strlen(kstr) : klen ;
	     rs = hdb_nextrec(op,key,curp) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (hdbstr_nextkey) */

/* delete all of the entries that match a key */
int hdbstr_delkey(hdbstr *op,cchar *kstr,int klen) noex {
	int		rs = SR_FAULT ;
	if (op && kstr) {
	    hdb_cur	cur{} ;
	    hdb_dat	skey ;
	    skey.buf = kstr ;
	    skey.len = (klen < 0) ? strlen(kstr) : klen ;
/* delete all of the data associated with this key */
	    int		rs1 = SR_OK ;
	    if ((rs = hdb_curbegin(op,&cur)) >= 0) {
	        hdb_dat		key{} ;
	        hdb_dat		val{} ;
	        while (rs1 >= 0) {
	            rs1 = hdb_fetchrec(op,skey,&cur,&key,&val) ;
	            if (rs1 == SR_NOTFOUND) break ;
		    rs = rs1 ;
		    if (rs < 0) break ;
	            if (hdb_curdel(op,&cur,1) >= 0) {
	                if (key.buf != nullptr) {
	                    uc_free(key.buf) ;
	                    key.buf = nullptr ;
	                }
	            } /* end if */
	            while ((rs >= 0) && (rs1 >= 0)) {
			cchar	*s1 ;
			cchar	*s2 ;
			bool	f = false ;
	                rs1 = hdb_getrec(op,&cur,&key,&val) ;
	                if (rs1 < 0) break ;
			s1 = charp(skey.buf) ;
			s2 = charp(key.buf) ;
	                f = f || (skey.len != key.len) ;
			f = f || (strncmp(s1,s2,skey.len) != 0) ;
			if (f) break ;
	                if (hdb_curdel(op,&cur,0) >= 0) {
	                    if (key.buf != nullptr) {
	                        uc_free(key.buf) ;
	                        key.buf = nullptr ;
	                    }
	                } /* end if */
	                rs1 = hdb_nextrec(op,skey,&cur) ;
	                if (rs1 < 0) break ;
	            } /* end while */
	        } /* end while */
	        rs1 = hdb_curend(op,&cur) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (cursor) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (hdbstr_delkey) */

int hdbstr_delcur(hdbstr *op,cur *curp,int f_adv) noex {
	int		rs = SR_FAULT ;
	if (op && curp) {
	    hdb_dat	key{} ;
	    hdb_dat	val{} ;
	    if ((rs = hdb_getrec(op,curp,&key,&val)) >= 0) {
	        cchar	*kp = charp(key.buf) ;
	        if ((rs = hdb_curdel(op,curp,f_adv)) >= 0) {
	            if (kp) uc_free(kp) ;
	        }
	    } /* end if */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (hdbstr_delcur) */

int hdbstr_curbegin(hdbstr *op,cur *curp) noex {
	int		rs = SR_FAULT ;
	if (op && curp) {
	    rs = hdb_curbegin(op,curp) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (hdbstr_curbegin) */

int hdbstr_curend(hdbstr *op,cur *curp) noex {
	int		rs = SR_FAULT ;
	if (op && curp) {
	    rs = hdb_curend(op,curp) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (hdbstr_curend) */

int hdbstr_count(hdbstr *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = hdb_count(op) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (hdbstr_count) */


