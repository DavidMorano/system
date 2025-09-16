/* mapstrint SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* Map (database) for String-Integer pairs */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-12, David A­D­ Morano
	This module was adapted from some previous code.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	mapstrint

	[Description:
	This little object maps a string to an integer value using
	a hash table.  The underlying hash table is implemented
	with a HDB object (currently).

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<hdb.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"mapstrint.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |getlenstr(3u)| */

/* local defines */

#define	MSI	mapstrint


/* imported namespaces */

using libuc::libmem ;			/* variable */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef	mapstrint_cur	cur ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int mapstrint_start(MSI *dbp,int nitems) noex {
	int		rs = SR_FAULT ;
	if (dbp) ylikely {
	    rs = hdb_start(dbp,nitems,0,nullptr,nullptr) ;
	}
	return rs ;
}
/* end subroutine (mapstrint_start) */

int mapstrint_count(MSI *dbp) noex {
	int		rs = SR_FAULT ;
	if (dbp) ylikely {
	    rs = hdb_count(dbp) ;
	}
	return rs ;
}
/* end subroutine (mapstrint_count) */

int mapstrint_finish(MSI *dbp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		n = 0 ;
	if (dbp) ylikely {
	    rs = SR_OK ;
	    {
	        mapstrint_cur	kcur ;
	        if ((rs1 = hdb_curbegin(dbp,&kcur)) >= 0) {
	            hdb_dat	key{} ;
	            hdb_dat	val{} ;
	            while (hdb_curenum(dbp,&kcur,&key,&val) >= 0) {
	                cchar	*ep = charp(val.buf) ;
	                if (ep) {
			    void *vp = voidp(ep) ;
	                    rs1 = libmem.free(vp) ;
		            if (rs >= 0) rs = rs1 ;
	                }
	                n += 1 ;
	            } /* end while */
	            hdb_curend(dbp,&kcur) ;
	        } /* end if */
	        if (rs >= 0) rs = rs1 ;
	    } /* end block */
	    {
	        rs1 = hdb_finish(dbp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (mapstrint_finish) */

int mapstrint_add(MSI *dbp,cchar *kstr,int µklen,int ival) noex {
	int		rs = SR_FAULT ;
	if (dbp && kstr) ylikely {
	    rs = SR_INVALID ;
	    if (int klen ; (klen = getlenstr(kstr,µklen)) > 0) ylikely {
	        cint	sz = szof(int) + klen + 1 ;
	        if (int *ip ; (rs = libmem.mall(sz,&ip)) >= 0) ylikely {
	            hdb_dat	key ;
	            hdb_dat	val ;
	            char	*bp ;
	            *ip = ival ;
	            bp = charp(ip) + szof(int) ;
	            strwcpy(bp,kstr,klen) ;
	            key.buf = bp ;
	            key.len = klen ;
	            val.buf = ip ;
	            val.len = szof(int) ;
	            rs = hdb_store(dbp,key,val) ;
	            if (rs < 0) {
		        libmem.free(ip) ;
	            } /* end if (error) */
	        } /* end if (memory-allocation) */
	    } /* end if (getlenstr) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mapstrint_add) */

int mapstrint_already(MSI *op,cchar *kstr,int µklen) noex {
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op && kstr) ylikely {
	    rs = SR_INVALID ;
	    if (int klen ; (klen = getlenstr(kstr,µklen)) > 0) ylikely {
	        if (int v ; (rs = mapstrint_fetch(op,kstr,klen,np,&v)) >= 0) {
	            rs = v ;
	        }
	    } /* end if (getlenstr) */
	} /* end if (non-null) */
	return rs ;
}
/* end if (mapstrint_already) */

int mapstrint_curenum(MSI *dbp,cur *curp,cchar **kpp,int *vp) noex {
	int		rs = SR_FAULT ;
	int		klen = 0 ;
	if (dbp && curp) ylikely {
	    hdb_dat	key{} ;
	    hdb_dat	val{} ;
	    if (kpp) *kpp = nullptr ;
	    if ((rs = hdb_curenum(dbp,curp,&key,&val)) >= 0) {
	        klen = key.len ;
	        if (kpp) {
	            *kpp = charp(key.buf) ;
	        }
	        if (vp) {
	            int *ip = intp(val.buf) ;
	            *vp = *ip ;
	        }
	    } /* end if (hdb_curenum) */
	} /* end if (non-null) */
	return (rs >= 0) ? klen : rs ;
}
/* end subroutine (mapstrint_curenum) */

int mapstrint_fetch(MSI *dbp,cchar *kstr,int µklen,cur *curp,int *vp) noex {
	int		rs = SR_FAULT ;
	int		rv = 0 ;
	if (dbp && kstr) ylikely {
	    rs = SR_INVALID ;
	    if (int klen ; (klen = getlenstr(kstr,µklen)) > 0) ylikely {
	        hdb_dat		key ;
	        hdb_dat		val{} ;
	        key.buf = kstr ;
	        key.len = klen ;
	        if ((rs = hdb_fetch(dbp,key,curp,&val)) >= 0) {
	            int	*ip = intp(val.buf) ;
	            if (vp) {
	                *vp = *ip ;
	            }
		    rv = *ip ;
	        } /* end if (hdb_fetch) */
	    } /* end if (getlenstr) */
	} /* end if (non-null) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (mapstrint_fetch) */

int mapstrint_fetchrec(MSI *dbp,cc *kstr,int µklen,cur *curp,
		cc **kpp,int *vp) noex {
	int		rs = SR_FAULT ;
	int		rl = 0 ; /* return-value */
	if (dbp && kstr && curp) ylikely {
	    rs = SR_INVALID ;
	    if (int klen ; (klen = getlenstr(kstr,µklen)) > 0) ylikely {
	        hdb_dat		key ;
	        hdb_dat		val{} ;
	        hdb_dat		rkey{} ;
	        key.buf = kstr ;
	        key.len = klen ;
	        if (kpp) *kpp = nullptr ;
	        if ((rs = hdb_fetchrec(dbp,key,curp,&rkey,&val)) >= 0) {
	            if (kpp) {
	                *kpp = charp(rkey.buf) ;
			rl = klen ;
	            }
	            if (vp) {
	                int *ip = intp(val.buf) ;
	                *vp = *ip ;
	            }
	        } /* end if (hdb_fetchrec) */
	    } /* end if (getlenstr) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (mapstrint_fetchrec) */

int mapstrint_curget(MSI *dbp,cur *curp,cchar **kpp,int *vp) noex {
	int		rs = SR_FAULT ;
	int		klen = 0 ;
	if (dbp && curp) ylikely {
	    hdb_dat	key{} ;
	    hdb_dat	val{} ;
	    if (kpp) *kpp = nullptr ;
	    if ((rs = hdb_curget(dbp,curp,&key,&val)) >= 0) {
	        klen = key.len ;
	        if (kpp) {
	            *kpp = charp(key.buf) ;
	        }
	        if (vp) {
	            int	*ip = intp(val.buf) ;
	            *vp = *ip ;
	        }
	    } /* end if (hdb_curget) */
	} /* end if (non-null) */
	return (rs >= 0) ? klen : rs ;
}
/* end subroutine (mapstrint_curget) */

int mapstrint_curnext(MSI *dbp,mapstrint_cur *curp) noex {
	int		rs = SR_FAULT ;
	if (dbp && curp) ylikely {
	    rs = hdb_curnext(dbp,curp) ;
	}
	return rs ;
}
/* end subroutine (mapstrint_curnext) */

/* advance the cursor to the next entry with the given key */
int mapstrint_nextkey(MSI *dbp,cchar *kstr,int µklen,mapstrint_cur *curp) noex {
	int		rs = SR_FAULT ;
	if (dbp && kstr && curp) ylikely {
	    rs = SR_INVALID ;
	    if (int klen ; (klen = getlenstr(kstr,µklen)) > 0) ylikely {
	        hdb_dat		key ;
	        key.buf = kstr ;
	        key.len = klen ;
	        rs = hdb_nextrec(dbp,key,curp) ;
	    } /* end if (getlenstr) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mapstrint_nextkey) */

/* delete all of the entries that match a key */
int mapstrint_delkey(MSI *dbp,cchar *kstr,int µklen) noex {
    	cint		rsn = SR_NOTFOUND ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rs2 ;
	if (dbp && kstr) ylikely {
	    rs = SR_INVALID ;
	    if (int klen ; (klen = getlenstr(kstr,µklen)) > 0) ylikely {
	        /* delete all of the data associated with this key */
	        if (hdb_cur kcur ; (rs = hdb_curbegin(dbp,&kcur)) >= 0) {
		    cauto	fetrec = hdb_fetchrec ;
	            hdb_dat	skey ;
	            hdb_dat	key{} ;
	            hdb_dat	val{} ;
	            skey.buf = kstr ;
	            skey.len = klen ;
	            while ((rs2 = fetrec(dbp,skey,&kcur,&key,&val)) >= 0) {
	                if (hdb_curdel(dbp,&kcur,1) >= 0) {
	                    char	*ep = charp(val.buf) ;
	                    if (ep) {
	                        libmem.free(ep) ;
		            }
	                } /* end if */
	                while (hdb_curget(dbp,&kcur,&key,&val) >= 0) {
		            cchar	*kp = charp(key.buf) ;
		            cchar	*sp = charp(skey.buf) ;
		            bool	f = false ;
	                    f = f || (skey.len != key.len) ;
		            f = f || (strncmp(sp,kp,skey.len) != 0) ;
		            if (f) break ;
	                    if (hdb_curdel(dbp,&kcur,0) >= 0) {
	                        char	*ep = charp(val.buf) ;
	                        if (ep) {
	                            libmem.free(ep) ;
			        }
	                    } /* end if */
	                    rs1 = hdb_nextrec(dbp,skey,&kcur) ;
	                    if (rs1 == SR_NOTFOUND) break ;
	                } /* end while */
	            } /* end while */
		    if ((rs >= 0) && (rs2 != rsn)) rs = rs2 ;
	            rs1 = hdb_curend(dbp,&kcur) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (cursor) */
	    } /* end if (getlenstr) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mapstrint_delkey) */

int mapstrint_curdel(MSI *dbp,mapstrint_cur *curp,int f_adv) noex {
	int		rs = SR_FAULT ;
	if (dbp && curp) ylikely {
	    hdb_dat	key{} ;
	    hdb_dat	val{} ;
	    if ((rs = hdb_curget(dbp,curp,&key,&val)) >= 0) {
	        char	*ep = charp(val.buf) ;
	        rs = hdb_curdel(dbp,curp,f_adv) ;
	        if ((rs >= 0) && ep) {
	            libmem.free(ep) ;
	        }
	    } /* end if */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mapstrint_curdel) */

int mapstrint_curbegin(MSI *dbp,mapstrint_cur *curp) noex {
	return hdb_curbegin(dbp,curp) ;
}
/* end subroutine (mapstrint_curbegin) */

int mapstrint_curend(MSI *dbp,mapstrint_cur *curp) noex {
	return hdb_curend(dbp,curp) ;
}
/* end subroutine (mapstrint_curend) */

int mapstrint_cursetval(MSI *dbp,mapstrint_cur *curp,int ival) noex {
	int		rs = SR_FAULT ;
	int		kl = 0 ; /* return-value */
	if (dbp && curp) ylikely {
	    hdb_dat	key{} ;
	    hdb_dat	val{} ;
	    if ((rs = hdb_curget(dbp,curp,&key,&val)) >= 0) {
	        int	*ip = intp(val.buf) ;
	        *ip = ival ;
	        kl = key.len ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? kl : rs ;
}
/* end subroutine (mapstrint_cursetval) */


/* private subroutines */


