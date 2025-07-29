/* vecstr_ext SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* extra functions for string vector objects */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	vecstr_ext

	Description:
	Below are the so-called extended function for the VECSTR object.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<localmisc.h>

#include	"vecstr.h"
#include	"vecstrx.hh"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct vecstrer : vecstrx {
        vecstr		*op ;
	vecstrer() = delete ;
	vecstrer(vecstr *p) noex : op(p) { } ;
	int add(cchar *,int = -1)                   noex override final ;
	int adduniq(cchar *,int = -1)               noex override final ;
	int addkeyval(cchar *,int,cchar *,int = -1) noex override final ;
	int insert(int,cchar *,int = -1)            noex override final ;
	int get(int,cchar **)                       noex override final ;
	int getvec(mainv *)                         noex override final ;
	int finder(cchar *,vecstrx_f,cchar **)      noex override final ;
	int del(int = -1)                           noex override final ;
	int sort(vecstrx_f = nullptr)               noex override final ;
    } ; /* end struct (vecstrer) */
} /* end namespace */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int vecstr_addpathclean(vecstr *op,cchar *lp,int ll) noex {
    	vecstrer	vo(op) ;
	return vo.addpathclean(lp,ll) ;
}
/* end subroutine (vecstr_addpathclean) */

int vecstr_addpath(vecstr *op,cchar *lp,int ll) noex {
    	vecstrer	vo(op) ;
	return vo.addpath(lp,ll) ;
}
/* end subroutine (vecstr_addpath) */

int vecstr_addcspath(vecstr *op) noex {
    	vecstrer	vo(op) ;
	return vo.addcspath() ;
}
/* end subroutine (vecstr_addcspath) */

int vecstr_adds(vecstr *op,cchar *sp,int sl) noex {
    	vecstrer	vo(op) ;
	return vo.adds(sp,sl) ;
}
/* end subroutine (vecstr_adds) */

int vecstr_addsubdirs(vecstr *op,cchar *newsdname) noex {
    	vecstrer	vo(op) ;
	return vo.addsubdirs(newsdname) ;
}
/* end subroutine (vecstr_addsubdirs) */

int vecstr_addsyms(vecstr *op,cc *objn,mainv syms) noex {
    	vecstrer	vo(op) ;
	return vo.addsyms(objn,syms) ;
}
/* end subroutine (vecstr_addsymss) */

int vecstr_adduniqs(vecstr *op,cchar *sp,int sl) noex {
    	vecstrer	vo(op) ;
	return vo.adduniqs(sp,sl) ;
}
/* end subroutine (vecstr_adduniqs) */

int vecstr_envadd(vecstr *op,cchar *kp,cchar *vp,int vl) noex {
    	vecstrer	vo(op) ;
	return vo.envadd(kp,vp,vl) ;
}
/* end subroutine (vecstr_envadd) */

int vecstr_envadds(vecstr *op,cchar *sp,int sl) noex {
    	vecstrer	vo(op) ;
	return vo.envadds(sp,sl) ;
}
/* end subroutine (vecstr_envadds) */

int vecstr_envset(vecstr *op,cchar *kp,cchar *vp,int vl) noex {
    	vecstrer	vo(op) ;
	return vo.envset(kp,vp,vl) ;
}
/* end subroutine (vecstr_envset) */

int vecstr_envget(vecstr *op,cchar *kp,cchar **rpp) noex {
    	vecstrer	vo(op) ;
	return vo.envget(kp,rpp) ;
}
/* end subroutine (vecstr_envget) */

int vecstr_envfile(vecstr *op,cchar *fname) noex {
    	vecstrer	vo(op) ;
	return vo.envfile(fname) ;
}
/* end subroutine (vecstr_envfile) */

int vecstr_foilcmp(vecstr *op,vecstr *rnp) noex {
    	vecstrer	vo(op) ;
	vecstrer	voo(rnp) ;
	return vo.foilcmp(&voo) ;
}
/* end subroutine (vecstr_foilcmp) */

int vecstr_loadfile(vecstr *op,int fu,cchar *fname) noex {
    	vecstrer	vo(op) ;
	return vo.loadfile(fu,fname) ;
}
/* end subroutine (vecstr_loadfile) */

int vecstr_loadgrusers(vecstr *op,gid_t sgid) noex {
    	vecstrer	vo(op) ;
	return vo.loadgrusers(sgid) ;
}
/* end subroutine (vecstr_loadgrusers) */

int vecstr_loadpjusers(vecstr *op,cchar *pjn) noex {
    	vecstrer	vo(op) ;
	return vo.loadpjusers(pjn) ;
}
/* end subroutine (vecstr_loadpjusers) */

int vecstr_srvargs(vecstr *op,cchar *abuf) noex {
    	vecstrer	vo(op) ;
	return vo.srvargs(abuf) ;
}
/* end subroutine (vecstr_srvargs) */

int vecstr_svcargs(vecstr *op,int *fp,cchar *abuf) noex {
    	vecstrer	vo(op) ;
	return vo.svcargs(fp,abuf) ;
}
/* end subroutine (vecstr_svcargs) */


/* local subroutines */

int vecstrer::add(cchar *sp,int sl) noex {
    	return vecstr_add(op,sp,sl) ;
}

int vecstrer::adduniq(cchar *sp,int sl) noex {
    	return vecstr_adduniq(op,sp,sl) ;
}

int vecstrer::addkeyval(cchar *kp,int kl,cchar *valp,int vall) noex {
    	return vecstr_addkeyval(op,kp,kl,valp,vall) ;
}

int vecstrer::insert(int ii,cchar *sp,int sl) noex {
    	return vecstr_insert(op,ii,sp,sl) ;
}

int vecstrer::get(int i,cchar **rpp) noex {
    	return vecstr_get(op,i,rpp) ;
}

int vecstrer::getvec(mainv *mp) noex {
    	return vecstr_getvec(op,mp) ;
}

int vecstrer::finder(cchar *s,vecstr_f vcmp,cchar **rpp) noex {
    	return vecstr_finder(op,s,vcmp,rpp) ;
}

int vecstrer::del(int i) noex {
	return vecstr_del(op,i) ;
}

int vecstrer::sort(vecstr_f vcmp) noex {
	return vecstr_sort(op,vcmp) ;
}


