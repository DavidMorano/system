/* mapstrint SUPPORT */
/* lang=C++20 */

/* Map (database) for String-Integer pairs */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-12, David A­D­ Morano
	This module was adapted from some previous code.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This little object maps a string to an integer value using
	a hash table.  The underlying hash table is implemented
	with a HDB object (currently).

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<hdb.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"mapstrint.h"


/* local defines */

#define	MSI	mapstrint


/* imported namespaces */

using std::nullptr_t ;			/* type */


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
	if (dbp) {
	    rs = hdb_start(dbp,nitems,0,nullptr,nullptr) ;
	}
	return rs ;
}
/* end subroutine (mapstrint_start) */

int mapstrint_count(MSI *dbp) noex {
	int		rs = SR_FAULT ;
	if (dbp) {
	    rs = hdb_count(dbp) ;
	}
	return rs ;
}
/* end subroutine (mapstrint_count) */

int mapstrint_finish(MSI *dbp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		n = 0 ;
	if (dbp) {
	    {
	        mapstrint_cur	keycursor ;
	        rs = SR_OK ;
	        if ((rs1 = hdb_curbegin(dbp,&keycursor)) >= 0) {
	            hdb_dat	key{} ;
	            hdb_dat	val{} ;
	            while (hdb_curenum(dbp,&keycursor,&key,&val) >= 0) {
	                cchar	*ep = charp(val.buf) ;
	                if (ep) {
	                    rs1 = uc_free(ep) ;
		            if (rs >= 0) rs = rs1 ;
	                }
	                n += 1 ;
	            } /* end while */
	            hdb_curend(dbp,&keycursor) ;
	        } /* end if */
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = hdb_finish(dbp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (mapstrint_finish) */

int mapstrint_add(MSI *dbp,cchar *kstr,int klen,int ival) noex {
	int		rs = SR_FAULT ;
	if (dbp && kstr) {
	    rs = SR_INVALID ;
	    if (klen > 0) {
	        int		sz ;
	        int		*ip{} ;
	        if (klen < 0) klen = strlen(kstr) ;
	        sz = sizeof(int) + klen + 1 ;
	        if ((rs = uc_malloc(sz,&ip)) >= 0) {
	            hdb_dat	key ;
	            hdb_dat	val ;
	            char	*bp ;
	            *ip = ival ;
	            bp = ((char *) ip) + sizeof(int) ;
	            strwcpy(bp,kstr,klen) ;
	            key.buf = bp ;
	            key.len = klen ;
	            val.buf = ip ;
	            val.len = sizeof(int) ;
	            rs = hdb_store(dbp,key,val) ;
	            if (rs < 0) {
		        uc_free(ip) ;
	            }
	        } /* end if (memory-allocation) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mapstrint_add) */

int mapstrint_already(MSI *op,cchar *kstr,int klen) noex {
	const nullptr_t	np{} ;
	int		rs = SR_FAULT ;
	if (op && kstr) {
	    if (int v{} ; (rs = mapstrint_fetch(op,kstr,klen,np,&v)) >= 0) {
	        rs = (v & INT_MAX) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end if (mapstrint_already) */

int mapstrint_enum(MSI *dbp,cur *curp,cchar **kpp,int *vp) noex {
	int		rs = SR_FAULT ;
	int		klen = 0 ;
	if (dbp && curp) {
	    hdb_dat	key{} ;
	    hdb_dat	val{} ;
	    if (kpp) *kpp = nullptr ;
	    if ((rs = hdb_curenum(dbp,curp,&key,&val)) >= 0) {
	        klen = key.len ;
	        if (kpp) {
	            *kpp = charp(key.buf) ;
	        }
	        if (vp) {
	            int		*ip = (int *) val.buf ;
	            *vp = *ip ;
	        }
	    } /* end if (hdb_curenum) */
	} /* end if (non-null) */
	return (rs >= 0) ? klen : rs ;
}
/* end subroutine (mapstrint_enum) */

int mapstrint_fetch(MSI *dbp,cchar *kstr,int klen,cur *curp,int *vp) noex {
	int		rs = SR_FAULT ;
	if (dbp && kstr) {
	    hdb_dat	key ;
	    hdb_dat	val{} ;
	    if (klen < 0) klen = strlen(kstr) ;
	    key.buf = kstr ;
	    key.len = klen ;
	    if ((rs = hdb_fetch(dbp,key,curp,&val)) >= 0) {
	        if (vp) {
	            int	*ip = (int *) val.buf ;
	            *vp = *ip ;
	        }
	    } /* end if (hdb_fetch) */
	} /* end if (non-null) */
	return (rs >= 0) ? klen : rs ;
}
/* end subroutine (mapstrint_fetch) */

int mapstrint_fetchrec(MSI *dbp,cc *kstr,int klen,cur *curp,
		cc **kpp,int *vp) noex {
	int		rs = SR_FAULT ;
	if (dbp && kstr && curp) {
	    hdb_dat	key ;
	    hdb_dat	val{} ;
	    hdb_dat	rkey{} ;
	    if (klen < 0) klen = strlen(kstr) ;
	    key.buf = kstr ;
	    key.len = klen ;
	    if (kpp) *kpp = nullptr ;
	    if ((rs = hdb_fetchrec(dbp,key,curp,&rkey,&val)) >= 0) {
	        if (kpp) {
	            *kpp = (char *) rkey.buf ;
	        }
	        if (vp) {
	            int		*ip = (int *) val.buf ;
	            *vp = *ip ;
	        }
	    } /* end if (hdb_fetchrec) */
	} /* end if (non-null) */
	return (rs >= 0) ? klen : rs ;
}
/* end subroutine (mapstrint_fetchrec) */

int mapstrint_getrec(MSI *dbp,cur *curp,cchar **kpp,int *vp) noex {
	int		rs = SR_FAULT ;
	int		klen = 0 ;
	if (dbp && curp) {
	    hdb_dat	key{} ;
	    hdb_dat	val{} ;
	    if (kpp) *kpp = nullptr ;
	    if ((rs = hdb_getrec(dbp,curp,&key,&val)) >= 0) {
	        klen = key.len ;
	        if (kpp) {
	            *kpp = (char *) key.buf ;
	        }
	        if (vp) {
	            int	*ip = (int *) val.buf ;
	            *vp = *ip ;
	        }
	    } /* end if (hdb_getrec) */
	} /* end if (non-null) */
	return (rs >= 0) ? klen : rs ;
}
/* end subroutine (mapstrint_getrec) */

int mapstrint_next(MSI *dbp,mapstrint_cur *curp) noex {
	int		rs = SR_FAULT ;
	if (dbp && curp) {
	    rs = hdb_next(dbp,curp) ;
	}
	return rs ;
}
/* end subroutine (mapstrint_next) */

/* advance the cursor to the next entry with the given key */
int mapstrint_nextkey(MSI *dbp,cchar *kstr,int klen,mapstrint_cur *curp) noex {
	int		rs = SR_FAULT ;
	if (dbp && kstr && curp) {
	    hdb_dat	key ;
	    if (klen < 0) klen = strlen(kstr) ;
	    key.buf = kstr ;
	    key.len = klen ;
	    rs = hdb_nextrec(dbp,key,curp) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mapstrint_nextkey) */

/* delete all of the entries that match a key */
int mapstrint_delkey(MSI *dbp,cchar *kstr,int klen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (dbp && kstr) {
	    mapstrint_cur	keycursor ;
	    hdb_dat		skey ;
	    if (klen < 0) klen = strlen(kstr) ;
	    skey.buf = kstr ;
	    skey.len = klen ;
/* delete all of the data associated with this key */
	    if ((rs = hdb_curbegin(dbp,&keycursor)) >= 0) {
	        hdb_dat		key{} ;
	        hdb_dat		val{} ;
	        while (hdb_fetchrec(dbp,skey,&keycursor,&key,&val) >= 0) {
	            if (hdb_curdel(dbp,&keycursor,1) >= 0) {
	                char	*ep = charp(val.buf) ;
	                if (ep) {
	                    uc_free(ep) ;
		        }
	            } /* end if */
	            while (hdb_getrec(dbp,&keycursor,&key,&val) >= 0) {
		        cchar	*kp = charp(key.buf) ;
		        cchar	*sp = charp(skey.buf) ;
		        bool	f = false ;
	                f = f || (skey.len != key.len) ;
		        f = f || (strncmp(sp,kp,skey.len) != 0) ;
		        if (f) break ;
	                if (hdb_curdel(dbp,&keycursor,0) >= 0) {
	                    char	*ep = charp(val.buf) ;
	                    if (ep) {
	                        uc_free(ep) ;
			    }
	                } /* end if */
	                rs1 = hdb_nextrec(dbp,skey,&keycursor) ;
	                if (rs1 == SR_NOTFOUND) break ;
	            } /* end while */
	        } /* end while */
	        rs1 = hdb_curend(dbp,&keycursor) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (cursor) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mapstrint_delkey) */

int mapstrint_delcur(MSI *dbp,mapstrint_cur *curp,int f_adv) noex {
	int		rs = SR_FAULT ;
	if (dbp && curp) {
	    hdb_dat	key{} ;
	    hdb_dat	val{} ;
	    if ((rs = hdb_getrec(dbp,curp,&key,&val)) >= 0) {
	        char	*ep = charp(val.buf) ;
	        rs = hdb_curdel(dbp,curp,f_adv)  ;
	        if ((rs >= 0) && ep) {
	            uc_free(ep) ;
	        }
	    } /* end if */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mapstrint_delcur) */

int mapstrint_curbegin(MSI *dbp,mapstrint_cur *curp) noex {
	return hdb_curbegin(dbp,curp) ;
}
/* end subroutine (mapstrint_curbegin) */

int mapstrint_curend(MSI *dbp,mapstrint_cur *curp) noex {
	return hdb_curend(dbp,curp) ;
}
/* end subroutine (mapstrint_curend) */

int mapstrint_setval(MSI *dbp,mapstrint_cur *curp,int ival) noex {
	int		rs = SR_FAULT ;
	int		kl = 0 ;
	if (dbp && curp) {
	    hdb_dat	key{} ;
	    hdb_dat	val{} ;
	    if ((rs = hdb_getrec(dbp,curp,&key,&val)) >= 0) {
	        int	*ip = (int *) val.buf ;
	        *ip = ival ;
	        kl = key.len ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? kl : rs ;
}
/* end subroutine (mapstrint_setval) */


/* private subroutines */


