/* strfilemk HEADER */
/* lang=C20 */

/* STRFILEMK management */
/* version %I% last-modified %G% */


/* revision history:

	- 1998-12-01, David A­D­ Morano
	This module was originally written for hardware CAD support.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strfilemk

	Description:
	This module implements an interface (a trivial one) that
	provides access to the STRFILEMK object (which is dynamically
	loaded).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>
#include	<dlfcn.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<vecstr.h>
#include	<modload.h>
#include	<localmisc.h>

#include	"strfilemk.h"
#include	"strfilemks.h"


/* local defines */

#define	STRFILEMK_MODBNAME	"strfilemks"
#define	STRFILEMK_OBJNAME	"strfilemks"
#define	STRFILEMK_PRLOCAL	"LOCAL"

#ifndef	SYMNAMELEN
#define	SYMNAMELEN	60
#endif


/* external subroutines */

extern int	sncpy3(char *,int,cchar *,cchar *,cchar *) ;
extern int	snwcpy(char *,int,cchar *,int) ;
extern int	mkpath1(char *,cchar *) ;
extern int	mkpath1w(char *,cchar *,int) ;
extern int	mkpath2(char *,cchar *,cchar *) ;
extern int	mkpath3(char *,cchar *,cchar *,cchar *) ;
extern int	mkpath4(char *,cchar *,cchar *,cchar *,
			cchar *) ;
extern int	mkfnamesuf1(char *,cchar *,cchar *) ;
extern int	mksofname(char *,cchar *,cchar *,cchar *) ;
extern int	nleadstr(cchar *,cchar *,int) ;
extern int	mkpr(char *,int,cchar *,cchar *) ;
extern int	pathclean(char *,cchar *,int) ;

extern char	*strwcpy(char *,cchar *,int) ;


/* external variables */


/* local structures */


/* forward references */

static int	strfilemk_objloadbegin(STRFILEMK *,cchar *,cchar *) noex ;
static int	strfilemk_objloadend(STRFILEMK *) noex ;
static int	strfilemk_loadcalls(STRFILEMK *,cchar *) noex ;

static bool	isrequired(int) noex ;


/* local variables */

enum subs {
	sub_open,
	sub_addfile,
	sub_abort,
	sub_chgrp,
	sub_close,
	sub_overlast
} ;

constexpr cpcchar	subs[] = {
	"open",
	"addfile",
	"abort",
	"chgrp",
	"close",
	nullptr
} ;


/* exported variables */


/* exported subroutines */

int strfilemk_open(op,pr,dbname,of,om,n)
STRFILEMK	*op ;
cchar	pr[] ;
cchar	dbname[] ;
int		of ;
mode_t		om ;
int		n ;
{
	int	rs ;

	cchar	*objname = STRFILEMK_OBJNAME ;


	if (op == nullptr) return SR_FAULT ;
	if (pr == nullptr) return SR_FAULT ;
	if (dbname == nullptr) return SR_FAULT ;

	if (dbname[0] == '\0') return SR_INVALID ;

	memset(op,0,sizeof(STRFILEMK)) ;

	if ((rs = strfilemk_objloadbegin(op,pr,objname)) >= 0) {
	    if ((rs = (*op->call.open)(op->obj,dbname,of,om,n)) >= 0) {
		op->magic = STRFILEMK_MAGIC ;
	    }
	    if (rs < 0)
		strfilemk_objloadend(op) ;
	} /* end if (objloadbegin) */

	return rs ;
}
/* end subroutine (strfilemk_open) */


/* free up the entire vector string data structure object */
int strfilemk_close(op)
STRFILEMK	*op ;
{
	int	rs = SR_OK ;
	int	rs1 ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magic != STRFILEMK_MAGIC)
	    return SR_NOTOPEN ;

	rs1 = (*op->call.close)(op->obj) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = strfilemk_objloadend(op) ;
	if (rs >= 0) rs = rs1 ;

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (strfilemk_close) */


int strfilemk_addfile(op,sp,sl)
STRFILEMK	*op ;
cchar	sp[] ;
int		sl ;
{
	int	rs ;


	if (op == nullptr) return SR_FAULT ;
	if (sp == nullptr) return SR_FAULT ;

	if (op->magic != STRFILEMK_MAGIC) return SR_NOTOPEN ;

	rs = (*op->call.addfile)(op->obj,sp,sl) ;

	return rs ;
}
/* end subroutine (strfilemk_addfile) */


int strfilemk_abort(op)
STRFILEMK	*op ;
{
	int	rs = SR_NOSYS ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magic != STRFILEMK_MAGIC)
	    return SR_NOTOPEN ;

	if (op->call.abort != nullptr)
	    rs = (*op->call.abort)(op->obj) ;

	return rs ;
}
/* end subroutine (strfilemk_abort) */


int strfilemk_chgrp(op,gid)
STRFILEMK	*op ;
gid_t		gid ;
{
	int	rs = SR_NOSYS ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (op->magic != STRFILEMK_MAGIC)
	    return SR_NOTOPEN ;

	if (op->call.chgrp != nullptr)
	    rs = (*op->call.chgrp)(op->obj,gid) ;

	return rs ;
}
/* end subroutine (strfilemk_chgrp) */


/* private subroutines */


/* find and load the DB-access object */
static int strfilemk_objloadbegin(op,pr,objname)
STRFILEMK	*op ;
cchar	*pr ;
cchar	*objname ;
{
	MODLOAD	*lp = &op->loader ;

	int	rs ;


	{
		VECSTR		syms ;
	        const int	n = nelem(subs) ;
		int		opts = VECSTR_OCOMPACT ;
	        if ((rs = vecstr_start(&syms,n,opts)) >= 0) {
		    const int	symlen = SYMNAMELEN ;
	            int		i ;
		    int		snl ;
		    cchar	**sv ;
		    char	symname[SYMNAMELEN + 1] ;

	            for (i = 0 ; (i < n) && (subs[i] != nullptr) ; i += 1) {
	                if (isrequired(i)) {
	                    rs = sncpy3(symname,symlen,objname,"_",subs[i]) ;
		            snl = rs ;
		            if (rs >= 0) 
			        rs = vecstr_add(&syms,symname,snl) ;
		        }
		        if (rs < 0) break ;
	            } /* end for */
        
	            if (rs >= 0)
	                rs = vecstr_getvec(&syms,&sv) ;
        
	            if (rs >= 0) {
	                cchar	*modbname = STRFILEMK_MODBNAME ;
			opts = 0 ;
	                opts |= MODLOAD_OLIBVAR ;
			opts |= MODLOAD_OPRS ;
			opts |= MODLOAD_OSDIRS ;
	                rs = modload_open(lp,pr,modbname,objname,opts,sv) ;
	            }

	            vecstr_finish(&syms) ;
	        } /* end if (allocation) */
		if (rs >= 0) {
		    int		mv[2] ;
		    if ((rs = modload_getmva(lp,mv,1)) >= 0) {
			void	*p ;
			op->objsize = mv[0] ;
			    if ((rs = uc_malloc(op->objsize,&p)) >= 0) {
			        op->obj = p ;
			        rs = strfilemk_loadcalls(op,objname) ;
			        if (rs < 0) {
	    			    uc_free(op->obj) ;
	    			    op->obj = nullptr ;
			        }
			    } /* end if (memory-allocation) */
		    } /* end if (modload_getmva) */
		    if (rs < 0)
			modload_close(lp) ;
		} /* end if (modload_open) */
	} /* end block */

	return rs ;
}
/* end subroutine (strfilemk_objloadbegin) */


static int strfilemk_objloadend(op)
STRFILEMK	*op ;
{
	int	rs = SR_OK ;
	int	rs1 ;


	if (op->obj != nullptr) {
	    rs1 = uc_free(op->obj) ;
	    if (rs >= 0) rs = rs1 ;
	    op->obj = nullptr ;
	}

	rs1 = modload_close(&op->loader) ;
	if (rs >= 0) rs = rs1 ;

	return rs ;
}
/* end subroutine (strfilemk_objloadend) */


static int strfilemk_loadcalls(op,soname)
STRFILEMK	*op ;
cchar	soname[] ;
{
	MODLOAD	*lp = &op->loader ;

	int	rs = SR_NOTFOUND ;
	int	i ;
	int	c = 0 ;

	char	symname[SYMNAMELEN + 1] ;

	const void	*snp ;


	for (i = 0 ; subs[i] != nullptr ; i += 1) {

	    if ((rs = sncpy3(symname,SYMNAMELEN,soname,"_",subs[i])) >= 0) {
	         if ((rs = modload_getsym(lp,symname,&snp)) == SR_NOTFOUND) {
		     snp = nullptr ;
		     if (! isrequired(i)) rs = SR_OK ;
		}
	    }

	    if (rs < 0) break ;

	    if (snp != nullptr) {
	        c += 1 ;
		switch (i) {
		case sub_open:
		    op->call.open = (int (*)(void *,cchar *,
			int,mode_t,int)) snp ;
		    break ;
		case sub_addfile:
		    op->call.addfile = (int (*)(void *,cchar *,int)) snp ;
		    break ;
		case sub_abort:
		    op->call.abort = (int (*)(void *)) snp ;
		    break ;
		case sub_chgrp:
		    op->call.chgrp = (int (*)(void *,gid_t)) snp ;
		    break ;
		case sub_close:
		    op->call.close = (int (*)(void *)) snp ;
		    break ;
		} /* end switch */
	    } /* end if (it had the call) */

	} /* end for (subs) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (strfilemk_loadcalls) */

static bool isrequired(int i) noex {
	bool		f = false ;
	switch (i) {
	case sub_open:
	case sub_addfile:
	case sub_close:
	    f = true ;
	    break ;
	} /* end switch */
	return f ;
}
/* end subroutine (isrequired) */


