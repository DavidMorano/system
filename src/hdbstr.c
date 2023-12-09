/* hdbstr */
/* lang=C20 */

/* Key-Value Hash DataBase for Strings */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time debugging */
#define	CF_SAFE		1		/* safety */
#define	CF_STRNLEN	0		/* user 'strnlen(3dam)' */

/* revision history:

	= 1998-03-12, David A­D­ Morano
	This module was adapted from some previous code.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This module provides a hash access container object for strings.

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<stdlib.h>
#include	<string.h>
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


/* external subroutines */

#if	CF_DEBUGS
extern int	debugprintf(cchar *,...) ;
extern int	strlinelen(cchar *,int,int) ;
#endif


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int hdbstr_start(HDBSTR *op,int n) noex {
	int		rs ;

#if	CF_SAFE
	if (op == NULL) return SR_FAULT ;
#endif

	rs = hdb_start(op,n,0,NULL,NULL) ;

	return rs ;
}
/* end subroutine (hdbstr_start) */

int hdbstr_finish(HDBSTR *op) noex {
	HDB_DATUM	key, val ;
	HDB_CUR		cur ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		n = 0 ;

#if	CF_SAFE
	if (op == NULL) return SR_FAULT ;
#endif

	if ((rs1 = hdb_curbegin(op,&cur)) >= 0) {
	    while (hdb_enum(op,&cur,&key,&val) >= 0) {
	        if (key.buf != NULL) {
	            rs1 = uc_free(key.buf) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	        n += 1 ;
	    } /* end while (enum) */
	    hdb_curend(op,&cur) ;
	} /* end if (cursor) */
	if (rs >= 0) rs = rs1 ;

	rs1 = hdb_finish(op) ;
	if (rs >= 0) rs = rs1 ;

	return (rs >= 0) ? n : rs ;
}
/* end subroutine (hdbstr_finish) */

/* add a string pair to the database */
int hdbstr_add(HDBSTR *op,cchar *kstr,int klen,cchar *vstr,int vlen) noex {
	HDB_DATUM	key, val ;
	int		rs ;
	int		size ;
	char		*bp ;

#if	CF_SAFE
	if (op == NULL) return SR_FAULT ;
#endif

#if	CF_DEBUGS
	debugprintf("hdbstr_add: kl=%d k=>%t<\n",klen,kstr,klen) ;
#endif

	if (kstr == NULL) return SR_FAULT ;

#if	CF_STRNLEN
	klen = strnlen(kstr,klen) ;
#else
	if (klen < 0) klen = strlen(kstr) ;
#endif

	if (vstr == NULL) vlen = 0 ;

#if	CF_STRNLEN
	vlen = strnlen(vstr,vlen) ;
#else
	if (vlen < 0) vlen = strlen(vstr) ;
#endif

#if	CF_DEBUGS
	debugprintf("hdbstr_add: vl=%d v=>%t<\n",vlen,vstr,vlen) ;
#endif

	size = (klen + vlen + 2) ;
	if ((rs = uc_malloc(size,&bp)) >= 0) {
	    key.buf = bp ;
	    key.len = klen ;
	    bp = strwcpy(bp,kstr,klen) + 1 ;
	    val.buf = bp ;
	    val.len = vlen ;
	    if (vstr != NULL) strwcpy(bp,vstr,vlen) ;
	    rs = hdb_store(op,key,val) ;
	    if (rs < 0)
	        uc_free(bp) ;
	} /* end if (memory-allocation) */

#if	CF_DEBUGS
	debugprintf("hdbstr_add: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (hdbstr_add) */

/* enumerate all of the entries */
int hdbstr_enum(HDBSTR *op,HDBSTR_CUR *curp,cc **kpp,
		cc **vpp,int *vlenp) noex {
	HDB_DATUM	key, val ;
	int		rs ;
	int		kl = 0 ;

#if	CF_SAFE
	if (op == NULL) return SR_FAULT ;
#endif

	if (kpp != NULL) *kpp = NULL ;

	if ((rs = hdb_enum(op,curp,&key,&val)) >= 0) {

	    kl = key.len ;
	    if (kpp != NULL)
	        *kpp = (cchar *) key.buf ;

	    if (vpp != NULL)
	        *vpp = (cchar *) val.buf ;

	    if (vlenp != NULL)
	        *vlenp = val.len ;

	} /* end if (had entry) */

	return (rs >= 0) ? kl : rs ;
}
/* end subroutine (hdbstr_enum) */


/* fetch the next entry value matching the given key */
int hdbstr_fetch(HDBSTR *op,cc *kstr,int klen,HDBSTR_CUR *curp,cc **rpp)
{
	HDB_DATUM	key, val ;
	int		rs ;
	int		vl = 0 ;

#if	CF_SAFE
	if (op == NULL) return SR_FAULT ;
#endif

#if	CF_DEBUGS
	debugprintf("hdbstr_fetch: ent kl=%d k=>%t<\n",klen,kstr,klen) ;
#endif

#if	CF_STRNLEN
	key.buf = kstr ;
	key.len = strnlen(kstr,klen) ;
#else
	key.buf = kstr ;
	key.len = (klen < 0) ? strlen(kstr) : klen ;
#endif

	if (rpp != NULL)
	    *rpp = NULL ;

#if	CF_DEBUGS
	debugprintf("hdbstr_fetch: hdb_fetch key=%t\n",kstr,klen) ;
#endif

	if ((rs = hdb_fetch(op,key,curp,&val)) >= 0) {
	    vl = val.len ;
	    if (rpp != NULL)
	        *rpp = (cchar *) val.buf ;
	}

#if	CF_DEBUGS
	debugprintf("hdbstr_fetch: ret rs=%d vl=%d v=>%t<\n",
	    rs,val.len,val.buf,val.len) ;
#endif

	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (hdbstr_fetch) */


/* fetch the next entry value matching the given key */
int hdbstr_fetchrec(op,kstr,klen,curp,kpp,vpp,vlenp)
HDBSTR		*op ;
cchar	kstr[] ;
int		klen ;
HDBSTR_CUR	*curp ;
cchar	**kpp ;
cchar	**vpp ;
int		*vlenp ;
{
	HDB_DATUM	key, val ;
	HDB_DATUM	rkey ;
	int		rs ;

#if	CF_SAFE
	if (op == NULL) return SR_FAULT ;
#endif

#if	CF_STRNLEN
	key.buf = kstr ;
	key.len = strnlen(kstr,klen) ;
#else
	key.buf = kstr ;
	key.len = (klen < 0) ? strlen(kstr) : klen ;
#endif

	if (kpp != NULL)
	    *kpp = NULL ;

	if ((rs = hdb_fetchrec(op,key,curp,&rkey,&val)) >= 0) {

	    if (kpp != NULL)
	        *kpp = (cchar *) rkey.buf ;

	    if (vpp != NULL)
	        *vpp = (cchar *) val.buf ;

	    if (vlenp != NULL)
	        *vlenp = val.len ;

	} /* end if (had entry) */

	return (rs >= 0) ? key.len : rs ;
}
/* end subroutine (hdbstr_fetchrec) */


/* get the current record under the cursor */
int hdbstr_getrec(HDBSTR *op,HDBSTR_CUR *curp,cchar **kpp,
		cchar **vpp,int *vlenp)
{
	HDB_DATUM	key, val ;
	int		rs ;

#if	CF_SAFE
	if (op == NULL) return SR_FAULT ;
#endif

	if (kpp != NULL)
	    *kpp = NULL ;

	if ((rs = hdb_getrec(op,curp,&key,&val)) >= 0) {

	    if (kpp != NULL)
	        *kpp = (cchar *) key.buf ;

	    if (vpp != NULL)
	        *vpp = (cchar *) val.buf ;

	    if (vlenp != NULL)
	        *vlenp = (int) val.len ;

	} /* end if (had entry) */

	return (rs >= 0) ? key.len : rs ;
}
/* end subroutine (hdbstr_getrec) */


/* advance the cursor to the next entry regardless of key */
int hdbstr_next(HDBSTR *op,HDBSTR_CUR *curp)
{
	int		rs ;

#if	CF_SAFE
	if (op == NULL) return SR_FAULT ;
#endif

	rs = hdb_next(op,curp) ;

	return rs ;
}
/* end subroutine (hdbstr_next) */


/* advance the cursor to the next entry with the given key */
int hdbstr_nextkey(HDBSTR *op,cchar *kstr,int klen,HDBSTR_CUR *curp)
{
	HDB_DATUM	key ;
	int		rs ;

#if	CF_SAFE
	if (op == NULL) return SR_FAULT ;
#endif

	if (kstr == NULL) return SR_FAULT ;

#if	CF_STRNLEN
	key.buf = kstr ;
	key.len = strnlen(kstr,klen) ;
#else
	key.buf = kstr ;
	key.len = (klen < 0) ? strlen(kstr) : klen ;
#endif

	rs = hdb_nextrec(op,key,curp) ;

	return rs ;
}
/* end subroutine (hdbstr_nextkey) */


/* delete all of the entries that match a key */
int hdbstr_delkey(HDBSTR *op,cchar *kstr,int klen)
{
	HDB_DATUM	skey ;
	int		rs = SR_OK ;

#if	CF_SAFE
	if (op == NULL) return SR_FAULT ;
#endif

#if	CF_DEBUGS
	debugprintf("hdbstr_delkey: k=>%t<\n",
	    kstr,strlinelen(kstr,klen,50)) ;
#endif

#if	CF_STRNLEN
	skey.buf = kstr ;
	skey.len = strnlen(kstr,klen) ;
#else
	skey.buf = kstr ;
	skey.len = (klen < 0) ? strlen(kstr) : klen ;
#endif

/* delete all of the data associated with this key */

	{
	    HDB_DATUM	key, val ;
	    HDB_CUR	cur ;
	    int		rs1 = SR_OK ;

	    if ((rs = hdb_curbegin(op,&cur)) >= 0) {

	        while (rs1 >= 0) {
	            rs1 = hdb_fetchrec(op,skey,&cur,&key,&val) ;
	            if (rs1 == SR_NOTFOUND) break ;
		    rs = rs1 ;
		    if (rs < 0) break ;

	            if (hdb_delcur(op,&cur,1) >= 0) {
	                if (key.buf != NULL) {
	                    uc_free(key.buf) ;
	                    key.buf = NULL ;
	                }
	            } /* end if */

	            while ((rs >= 0) && (rs1 >= 0)) {
	                rs1 = hdb_getrec(op,&cur,&key,&val) ;
	                if (rs1 < 0) break ;

	                if ((skey.len != key.len) ||
	                    (strncmp(skey.buf,key.buf,skey.len) != 0))
	                    break ;

	                if (hdb_delcur(op,&cur,0) >= 0) {
	                    if (key.buf != NULL) {
	                        uc_free(key.buf) ;
	                        key.buf = NULL ;
	                    }
	                } /* end if */

	                rs1 = hdb_nextrec(op,skey,&cur) ;

	                if (rs1 < 0) break ;
	            } /* end while */

	        } /* end while */

	        rs1 = hdb_curend(op,&cur) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (cursor) */
	} /* end block */

#if	CF_DEBUGS
	debugprintf("hdbstr_delkey: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (hdbstr_delkey) */


/* delete the item under the cursor */
int hdbstr_delcur(HDBSTR *op,HDBSTR_CUR *curp,int f_adv)
{
	HDB_DATUM	key, val ;
	int		rs ;

#if	CF_SAFE
	if (op == NULL) return SR_FAULT ;
#endif

	if ((rs = hdb_getrec(op,curp,&key,&val)) >= 0) {
	    cchar	*kp = (cchar *) key.buf ;
	    if ((rs = hdb_delcur(op,curp,f_adv)) >= 0) {
	        if (kp != NULL) uc_free(kp) ;
	    }
	} /* end if */

	return rs ;
}
/* end subroutine (hdbstr_delcur) */


int hdbstr_curbegin(HDBSTR *op,HDBSTR_CUR *curp)
{

#if	CF_SAFE
	if (op == NULL) return SR_FAULT ;
#endif

	return hdb_curbegin(op,curp) ;
}
/* end subroutine (hdbstr_curbegin) */


int hdbstr_curend(HDBSTR *op,HDBSTR_CUR *curp)
{

#if	CF_SAFE
	if (op == NULL) return SR_FAULT ;
#endif

	return hdb_curend(op,curp) ;
}
/* end subroutine (hdbstr_curend) */


int hdbstr_count(HDBSTR *op)
{
	int		rs ;

#if	CF_SAFE
	if (op == NULL) return SR_FAULT ;
#endif

	rs = hdb_count(op) ;

	return rs ;
}
/* end subroutine (hdbstr_count) */


