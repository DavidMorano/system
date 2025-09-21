/* hdbstr_main SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* Key-Value Hash DataBase for Strings */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-12, David A­D­ Morano
	This module was adapted from some previous code.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	hdbstr

	Description:
	This module provides a hash access container object for
	c-strings.  Tis object stores the c-strings (in key-value
	c-string pairs) inside of the object (not just storing
	pointers to your own external storage spaec).  If you just
	want to store pointer to c-string that live on their own
	somewhere (or inside another object of some sort) then use
	the HDB object instead.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<hdb.h>
#include	<strwcpy.h>
#include	<strdcpyx.h>
#include	<localmisc.h>

#include	"hdbstr.h"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#ifndef	KBUFLEN
#define	KBUFLEN		120
#endif

#define	HS		hdbstr
#define	HS_CUR		hdbstr_cur


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */

typedef hdbstr_cur	hc ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static int hdbstr_ctor(hdbstr *op,Args ... args) noex {
    	HDBSTR		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = memclear(hop) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (hdbstr_ctor) */

static int hdbstr_dtor(hdbstr *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (hdbstr_dtor) */

template<typename ... Args>
static inline int hdbstr_magic(hdbstr *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = SR_OK ;
	}
	return rs ;
} /* end subroutine (hdbstr_magic) */

static int	hdbstr_finents(hdbstr *) noex ;


/* local variables */

constexpr char		objname[] = "hdbstr" ;


/* exported variables */


/* exported subroutines */

int hdbstr_start(hdbstr *op,int ne) noex {
	int		rs ;
	if ((rs = hdbstr_ctor(op)) >= 0) {
	    rs = hdb_start(op,ne,0,nullptr,nullptr) ;
	    if (rs < 0) {
		hdbstr_dtor(op) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (hdbstr_start) */

int hdbstr_finish(hdbstr *op) noex {
	int		rs ;
	int		rs1 ;
	int		n = 0 ;
	if ((rs = hdbstr_magic(op)) >= 0) {
	    {
		rs1 = hdbstr_finents(op) ;
		if (rs >= 0) rs = rs1 ;
		n = rs1 ;
	    }
	    {
		rs1 = hdb_finish(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = hdbstr_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (hdbstr_finish) */

static int hdbstr_finents(hdbstr *op) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	int		rs1 ;
	int		n = 0 ;
	if (hdb_cur cur ; (rs = hdb_curbegin(op,&cur)) >= 0) {
            hdb_dat	key{} ;
            hdb_dat	val{} ;
            while ((rs1 = hdb_curenum(op,&cur,&key,&val)) >= 0) {
                if (key.buf) {
		    void *vp = cast_const<voidp>(key.buf) ;
                    rs1 = libmem.free(vp) ;
                    if (rs >= 0) rs = rs1 ;
                }
                n += 1 ;
            } /* end while (enum) */
            if ((rs >= 0) && (rs1 != rsn)) rs = rs1 ;
            rs1 = hdb_curend(op,&cur) ;
            if (rs >= 0) rs = rs1 ;
	} /* end if (cursor) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (hdbstr_finents) */

/* add a string pair to the database */
int hdbstr_add(hdbstr *op,cchar *kstr,int klen,cchar *vstr,int vlen) noex {
	int		rs ;
	if ((rs = hdbstr_magic(op,kstr)) >= 0) {
	    int		sz ;
	    if (klen < 0) klen = lenstr(kstr) ;
	    if (vstr) {
	        if (vlen < 0) {
	            vlen = lenstr(vstr) ;
	        }
	    } else {
	        vlen = 0 ;
	    }
	    sz = (klen + vlen + 2) ;
	    if (char *bp ; (rs = libmem.mall(sz,&bp)) >= 0) {
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
	            libmem.free(bp) ;
	        }
	    } /* end if (memory-allocation) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (hdbstr_add) */

/* enumerate all of the entries */
int hdbstr_curenum(hdbstr *op,hc *curp,cc **kpp,cc **vpp,int *vlp) noex {
	int		rs ;
	int		kl = 0 ;
	if ((rs = hdbstr_magic(op,curp)) >= 0) {
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
	        if (vlp) {
	            *vlp = val.len ;
	        }
	    } /* end if (had entry) */
	} /* end if (magic) */
	return (rs >= 0) ? kl : rs ;
}
/* end subroutine (hdbstr_curenum) */

/* fetch the next entry value matching the given key */
int hdbstr_fetch(hdbstr *op,cc *kstr,int klen,hc *curp,cc **rpp) noex {
	int		rs ;
	int		vl = 0 ;
	if ((rs = hdbstr_magic(op,curp)) >= 0) {
	    hdb_dat	key ;
	    hdb_dat	val{} ;
	    key.buf = kstr ;
	    key.len = (klen < 0) ? lenstr(kstr) : klen ;
	    if (rpp) *rpp = nullptr ;
	    if ((rs = hdb_fetch(op,key,curp,&val)) >= 0) {
	        vl = val.len ;
	        if (rpp) {
	            *rpp = charp(val.buf) ;
		}
	    }
	} /* end if (magic) */
	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (hdbstr_fetch) */

/* fetch the next entry value matching the given key */
int hdbstr_fetchrec(hdbstr *op,cc *kstr,int klen,hc *curp,cc **kpp,cc **vpp,
		int *vlp) noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = hdbstr_magic(op,kstr,curp)) >= 0) {
	    hdb_dat	key ;
	    hdb_dat	val{} ;
	    hdb_dat	rkey{} ;
	    key.buf = kstr ;
	    key.len = (klen < 0) ? lenstr(kstr) : klen ;
	    if (kpp) *kpp = nullptr ;
	    if ((rs = hdb_fetchrec(op,key,curp,&rkey,&val)) >= 0) {
	        if (kpp) {
	            *kpp = charp(rkey.buf) ;
	        }
	        if (vpp) {
	            *vpp = charp(val.buf) ;
	        }
	        if (vlp) {
	            *vlp = val.len ;
	        }
	        len = key.len ;
	    } /* end if (had entry) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (hdbstr_fetchrec) */

/* get the current record under the cursor */
int hdbstr_curget(hdbstr *op,hc *curp,cc **kpp,cc **vpp,int *vlp) noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = hdbstr_magic(op,curp)) >= 0) {
	    hdb_dat	key{} ;
	    hdb_dat	val{} ;
	    if (kpp) *kpp = nullptr ;
	    if ((rs = hdb_curget(op,curp,&key,&val)) >= 0) {
	        if (kpp) {
	            *kpp = charp(key.buf) ;
	        }
	        if (vpp) {
	            *vpp = charp(val.buf) ;
	        }
	        if (vlp) {
	            *vlp = val.len ;
	        }
	        len = val.len ;
	    } /* end if (had entry) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (hdbstr_curget) */

/* advance the cursor to the next entry regardless of key */
int hdbstr_curnext(hdbstr *op,hc *curp) noex {
	int		rs ;
	if ((rs = hdbstr_magic(op,curp)) >= 0) {
	    rs = hdb_curnext(op,curp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (hdbstr_curnext) */

/* advance the cursor to the next entry with the given key */
int hdbstr_nextkey(hdbstr *op,cchar *kstr,int klen,hc *curp) noex {
	int		rs ;
	if ((rs = hdbstr_magic(op,kstr,curp)) >= 0) {
	     hdb_dat	key ;
	     key.buf = kstr ;
	     key.len = (klen < 0) ? lenstr(kstr) : klen ;
	     rs = hdb_nextrec(op,key,curp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (hdbstr_nextkey) */

/* delete all of the entries that match a key */
int hdbstr_delkey(hdbstr *op,cchar *kstr,int klen) noex {
	int		rs ;
	if ((rs = hdbstr_magic(op,kstr)) >= 0) {
	    hdb_dat	skey ;
	    skey.buf = kstr ;
	    skey.len = (klen < 0) ? lenstr(kstr) : klen ;
	    /* delete all of the data associated with this key */
	    int		rs1 = SR_OK ;
	    if (hdb_cur cur ; (rs = hdb_curbegin(op,&cur)) >= 0) {
	        hdb_dat		key{} ;
	        hdb_dat		val{} ;
	        while (rs1 >= 0) {
	            rs1 = hdb_fetchrec(op,skey,&cur,&key,&val) ;
	            if (rs1 == SR_NOTFOUND) break ;
		    rs = rs1 ;
		    if (rs < 0) break ;
	            if (hdb_curdel(op,&cur,1) >= 0) {
	                if (key.buf) {
		    	    void *vp = cast_const<voidp>(key.buf) ;
	                    libmem.free(vp) ;
	                    key.buf = nullptr ;
	                }
	            } /* end if */
	            while ((rs >= 0) && (rs1 >= 0)) {
			cchar	*s1 ;
			cchar	*s2 ;
			bool	f = false ;
	                rs1 = hdb_curget(op,&cur,&key,&val) ;
	                if (rs1 < 0) break ;
			s1 = charp(skey.buf) ;
			s2 = charp(key.buf) ;
	                f = f || (skey.len != key.len) ;
			f = f || (strncmp(s1,s2,skey.len) != 0) ;
			if (f) break ;
	                if (hdb_curdel(op,&cur,0) >= 0) {
	                    if (key.buf) {
		    		void *vp = cast_const<voidp>(key.buf) ;
	                        libmem.free(vp) ;
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
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (hdbstr_delkey) */

int hdbstr_curdel(hdbstr *op,hc *curp,int f_adv) noex {
	int		rs ;
	if ((rs = hdbstr_magic(op,curp)) >= 0) {
	    hdb_dat	key{} ;
	    hdb_dat	val{} ;
	    if ((rs = hdb_curget(op,curp,&key,&val)) >= 0) {
	        cchar	*kp = charp(key.buf) ;
	        if ((rs = hdb_curdel(op,curp,f_adv)) >= 0) {
	            if (kp) {
			void *vp = voidp(kp) ;
			libmem.free(vp) ;
		    }
	        }
	    } /* end if */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (hdbstr_curdel) */

int hdbstr_curbegin(hdbstr *op,hc *curp) noex {
	int		rs ;
	if ((rs = hdbstr_magic(op,curp)) >= 0) {
	    rs = hdb_curbegin(op,curp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (hdbstr_curbegin) */

int hdbstr_curend(hdbstr *op,hc *curp) noex {
	int		rs ;
	if ((rs = hdbstr_magic(op,curp)) >= 0) {
	    rs = hdb_curend(op,curp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (hdbstr_curend) */

int hdbstr_curdone(hdbstr *op,hc *curp) noex {
	int		rs ;
	if ((rs = hdbstr_magic(op,curp)) >= 0) {
	    rs = hdb_curdone(op,curp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (hdbstr_curdone) */

int hdbstr_curcopy(hdbstr *op,hc *curp,hc *othp) noex {
	int		rs ;
	if ((rs = hdbstr_magic(op,curp,othp)) >= 0) {
	    rs = hdb_curcopy(op,curp,othp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (hdbstr_curcopy) */

int hdbstr_delall(hdbstr *op) noex {
	int		rs ;
	if ((rs = hdbstr_magic(op)) >= 0) {
	    rs = hdb_delall(op) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (hdbstr_delall) */

int hdbstr_count(hdbstr *op) noex {
	int		rs ;
	if ((rs = hdbstr_magic(op)) >= 0) {
	    rs = hdb_count(op) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (hdbstr_count) */

hdbstr_iter hdbstr::begin() noex {
    	hdbstr_iter	it ;
	cint		csz = szof(hdbstr_cur) ;
    	int		rs ;
	if (void *vp ; (rs = libmem.mall(csz,&vp)) >= 0) {
	    it.curp = (hdb_cur *) vp ;
	    it.op = this ;
	    rs = hdbstr_curbegin(it.op,it.curp) ;
	    if (rs < 0) {
		libmem.free(it.curp) ;
		it.curp = nullptr ;
		it.op = nullptr ;
	    } /* end if (error handling) */
	} /* end if (memory-allocation) */
	if (rs < 0) {
	    ulogerror(objname,rs,"begin") ;
	}
	return it ;
}
/* end method (hdbstr_iter::begin) */

hdbstr_iter hdbstr::end() noex {
    	hdbstr_iter	it ;
	cint		csz = szof(hdbstr_cur) ;
    	int		rs = SR_OK ;
	if (void *vp ; (rs = libmem.mall(csz,&vp)) >= 0) {
	    it.curp = (hdb_cur *) vp ;
	    it.op = this ;
	    rs = hdbstr_curend(it.op,it.curp) ;
	    if (rs < 0) {
		libmem.free(it.curp) ;
		it.curp = nullptr ;
		it.op = nullptr ;
	    } /* end if (error handling) */
	} /* end if (memory-allocation) */
	if (rs < 0) {
	    ulogerror(objname,rs,"begin") ;
	}
	return it ;
}
/* end method (hdbstr_iter::end) */


