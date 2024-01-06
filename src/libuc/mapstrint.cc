/* MSI SUPPORT */
/* lang=C++20 */

/* Map (database) for String-Integer pairs */
/* version %I% last-modified %G% */

#define	CF_STRNLEN	0		/* use 'strnlen(3dam)' */

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

#include	<envstandards.h>
#include	<sys/types.h>
#include	<climits>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<hdb.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"mapstrint.h"


/* local defines */

#define	MSI	mapstrint


/* local namespaces */


/* local typedefs */

typedef	mapstrint_cur	cur ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

#ifdef	COMMENT
static int	cmpentry(cchar *,cchar *,int) noex ;
#endif


/* local variables */


/* exported subroutines */

int mapstrint_start(MSI *dbp,int nitems) noex {
	int		rs ;

	rs = hdb_start(dbp,nitems,0,NULL,NULL) ;

	return rs ;
}
/* end subroutine (mapstrint_start) */

int mapstrint_count(MSI *dbp) noex {
	int		rs ;

	rs = hdb_count(dbp) ;

	return rs ;
}
/* end subroutine (mapstrint_count) */

int mapstrint_finish(MSI *dbp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		n = 0 ;
	{
	    mapstrint_cur	keycursor ;
	    if ((rs1 = hdb_curbegin(dbp,&keycursor)) >= 0) {
	        hdb_dat	key ;
	        hdb_dat	val ;
	        cchar	*ep ;
	        while (hdb_enum(dbp,&keycursor,&key,&val) >= 0) {
	            ep = charp(val.buf) ;
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
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (mapstrint_finish) */

int mapstrint_add(MSI *dbp,cchar *kstr,int klen,int ival) noex {
	int		rs ;
	int		size ;
	int		*ip ;

	if ((klen > 0) && (kstr == NULL))
	    return SR_INVALID ;

#if	CF_STRNLEN
	klen = strnlen(kstr,klen) ;
#else
	if (klen < 0)
	    klen = strlen(kstr) ;
#endif

	size = sizeof(int) + klen + 1 ;
	if ((rs = uc_malloc(size,&ip)) >= 0) {
	    hdb_dat	key, val ;
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

	return rs ;
}
/* end subroutine (mapstrint_add) */

int mapstrint_already(MSI *op,cchar *kstr,int klen) noex {
	int		rs ;
	int		v ;
	if ((rs = mapstrint_fetch(op,kstr,klen,NULL,&v)) >= 0) {
	    rs = (v & INT_MAX) ;
	}
	return rs ;
}
/* end if (mapstrint_already) */

int mapstrint_enum(MSI *dbp,cur *curp,cchar **kpp,int *vp) noex {
	hdb_dat		key ;
	hdb_dat		val ;
	int		rs ;
	int		klen = 0 ;

	if (kpp != NULL)
	    *kpp = NULL ;

	if ((rs = hdb_enum(dbp,curp,&key,&val)) >= 0) {
	    klen = key.len ;
	    if (kpp != NULL)
	        *kpp = (char *) key.buf ;
	    if (vp != NULL) {
	        int	*ip = (int *) val.buf ;
	        *vp = *ip ;
	    }
	} /* end if (hdb_enum) */

	return (rs >= 0) ? klen : rs ;
}
/* end subroutine (mapstrint_enum) */

int mapstrint_fetch(MSI *dbp,cchar *kstr,int klen,cur *curp,int *vp) noex {
	hdb_dat		key ;
	hdb_dat		val ;
	int		rs ;

#if	CF_STRNLEN
	klen = strnlen(kstr,klen) ;
#else
	if (klen < 0)
	    klen = strlen(kstr) ;
#endif

	key.buf = kstr ;
	key.len = klen ;
	if ((rs = hdb_fetch(dbp,key,curp,&val)) >= 0) {
	    if (vp != NULL) {
	        int	*ip = (int *) val.buf ;
	        *vp = *ip ;
	    }
	} /* end if (hdb_fetch) */

	return (rs >= 0) ? klen : rs ;
}
/* end subroutine (mapstrint_fetch) */

int mapstrint_fetchrec(MSI *dbp,cc *kstr,int klen,cur *curp,
		cc **kpp,int *vp) noex {
	hdb_dat		key ;
	hdb_dat		val ;
	hdb_dat		rkey ;
	int		rs ;

#if	CF_STRNLEN
	klen = strnlen(kstr,klen) ;
#else
	if (klen < 0)
	    klen = strlen(kstr) ;
#endif

	key.buf = kstr ;
	key.len = klen ;

	if (kpp != NULL)
	    *kpp = NULL ;

	if ((rs = hdb_fetchrec(dbp,key,curp,&rkey,&val)) >= 0) {
	    if (kpp != NULL) {
	        *kpp = (char *) rkey.buf ;
	    }
	    if (vp != NULL) {
	        int	*ip = (int *) val.buf ;
	        *vp = *ip ;
	    }
	} /* end if (hdb_fetchrec) */

	return (rs >= 0) ? klen : rs ;
}
/* end subroutine (mapstrint_fetchrec) */

int mapstrint_getrec(MSI *dbp,cur *curp,cchar **kpp,int *vp) noex {
	hdb_dat		key ;
	hdb_dat		val ;
	int		rs ;
	int		klen = 0 ;

	if (kpp != NULL)
	    *kpp = NULL ;

	if ((rs = hdb_getrec(dbp,curp,&key,&val)) >= 0) {
	    klen = key.len ;
	    if (kpp != NULL) {
	        *kpp = (char *) key.buf ;
	    }
	    if (vp != NULL) {
	        int	*ip = (int *) val.buf ;
	        *vp = *ip ;
	    }
	} /* end if (hdb_getrec) */

	return (rs >= 0) ? klen : rs ;
}
/* end subroutine (mapstrint_getrec) */

int mapstrint_next(MSI *dbp,mapstrint_cur *curp) noex {
	int		rs ;

	rs = hdb_next(dbp,curp) ;

	return rs ;
}
/* end subroutine (mapstrint_next) */

/* advance the cursor to the next entry with the given key */
int mapstrint_nextkey(MSI *dbp,cchar *kstr,int klen,mapstrint_cur *curp) noex {
	hdb_dat		key ;
	int		rs ;

	if (kstr == NULL) return SR_FAULT ;

#if	CF_STRNLEN
	klen = strnlen(kstr,klen) ;
#else
	if (klen < 0) klen = strlen(kstr) ;
#endif

	key.buf = kstr ;
	key.len = klen ;

	rs = hdb_nextrec(dbp,key,curp) ;

	return rs ;
}
/* end subroutine (mapstrint_nextkey) */

/* delete all of the entries that match a key */
int mapstrint_delkey(MSI *dbp,cchar *kstr,int klen) noex {
	hdb_dat		skey ;
	hdb_dat		key ;
	hdb_dat		val ;
	mapstrint_cur	keycursor ;
	int		rs ;
	int		rs1 ;
	char		*ep ;

#if	CF_STRNLEN
	klen = strnlen(kstr,klen) ;
#else
	if (klen < 0) klen = strlen(kstr) ;
#endif

	skey.buf = kstr ;
	skey.len = klen ;

/* delete all of the data associated with this key */

	if ((rs = hdb_curbegin(dbp,&keycursor)) >= 0) {
	    while (hdb_fetchrec(dbp,skey,&keycursor,&key,&val) >= 0) {
	        if (hdb_delcur(dbp,&keycursor,1) >= 0) {
	            ep = charp(val.buf) ;
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
	            if (hdb_delcur(dbp,&keycursor,0) >= 0) {
	                ep = charp(val.buf) ;
	                if (ep) {
	                    uc_free(ep) ;
			}
	            } /* end if */
	            rs1 = hdb_nextrec(dbp,skey,&keycursor) ;
	            if (rs1 == SR_NOTFOUND) break ;
	        } /* end while */
	    } /* end while */
	    hdb_curend(dbp,&keycursor) ;
	} /* end if (cursor) */

	return rs ;
}
/* end subroutine (mapstrint_delkey) */

int mapstrint_delcur(MSI *dbp,mapstrint_cur *curp,int f_adv) noex {
	hdb_dat		key ;
	hdb_dat		val ;
	int		rs ;

	if ((rs = hdb_getrec(dbp,curp,&key,&val)) >= 0) {
	    char	*ep = (char *) val.buf ;
	    rs = hdb_delcur(dbp,curp,f_adv)  ;
	    if ((rs >= 0) && (ep != NULL)) {
	        uc_free(ep) ;
	    }
	} /* end if */

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
	hdb_dat		key ;
	hdb_dat		val ;
	int		rs ;
	int		kl = 0 ;

	if (curp == NULL) return SR_INVALID ;

	if ((rs = hdb_getrec(dbp,curp,&key,&val)) >= 0) {
	    int	*ip = (int *) val.buf ;
	    *ip = ival ;
	    kl = key.len ;
	}

	return (rs >= 0) ? kl : rs ;
}
/* end subroutine (mapstrint_setval) */


/* private subroutines */


