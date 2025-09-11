/* vecpstr_ext SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* extra functions for string vector objects */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-01, David A­D­ Morano
	This code was originally written.

	= 2023-10-27, David A­D­ Morano
	I changed to make the MAXPATHLEN value dynamic.

*/

/* Copyright © 1998,2023 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	vecpstr_ext

	Description:
	Below are the so-called extended function for the VECSTR object.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<localmisc.h>

#include	"vecpstr.h"
#include	"vecstrx.hh"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct vecpstrer : vecstrx {
        vecpstr		*op ;
	vecpstrer() = delete ;
	vecpstrer(vecpstr *p) noex : op(p) { } ;
	int add(cchar *,int = -1)                   noex override final ;
	int adduniq(cchar *,int = -1)               noex override final ;
	int addkeyval(cchar *,int,cchar *,int = -1) noex override final ;
	int insert(int,cchar *,int = -1)            noex override final ;
	int get(int,cchar **)                       noex override final ;
	int getvec(mainv *)                         noex override final ;
	int finder(cchar *,vecstrx_f,cchar **)      noex override final ;
	int del(int = -1)                           noex override final ;
	int sort(vecstrx_f = nullptr)               noex override final ;
    } ; /* end struct (vecpstrer) */
} /* end namespace */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int vecpstr_addpathclean(vecpstr *op,cchar *lp,int ll) noex {
    	vecpstrer	vo(op) ;
	return vo.addpathclean(lp,ll) ;
}
/* end subroutine (vecpstr_addpathclean) */

int vecpstr_addpath(vecpstr *op,cchar *lp,int ll) noex {
    	vecpstrer	vo(op) ;
	return vo.addpath(lp,ll) ;
}
/* end subroutine (vecpstr_addpath) */

int vecpstr_addcspath(vecpstr *op) noex {
    	vecpstrer	vo(op) ;
	return vo.addcspath() ;
}
/* end subroutine (vecpstr_addcspath) */

int vecpstr_adds(vecpstr *op,cchar *sp,int sl) noex {
    	vecpstrer	vo(op) ;
	return vo.adds(sp,sl) ;
}
/* end subroutine (vecpstr_adds) */

int vecpstr_addsubdirs(vecpstr *op,cchar *newsdname) noex {
    	vecpstrer	vo(op) ;
	return vo.addsubdirs(newsdname) ;
}
/* end subroutine (vecpstr_addsubdirs) */

int vecpstr_addsyms(vecpstr *op,cc *objn,mainv syms) noex {
    	vecpstrer	vo(op) ;
	return vo.addsyms(objn,syms) ;
}
/* end subroutine (vecpstr_addsymss) */

int vecpstr_adduniqs(vecpstr *op,cchar *sp,int sl) noex {
    	vecpstrer	vo(op) ;
	return vo.adduniqs(sp,sl) ;
}
/* end subroutine (vecpstr_adduniqs) */

int vecpstr_envadd(vecpstr *op,cchar *kp,cchar *vp,int vl) noex {
    	vecpstrer	vo(op) ;
	return vo.envadd(kp,vp,vl) ;
}
/* end subroutine (vecpstr_envadd) */

int vecpstr_envadds(vecpstr *op,cchar *sp,int sl) noex {
    	vecpstrer	vo(op) ;
	return vo.envadds(sp,sl) ;
}
/* end subroutine (vecpstr_envadds) */

int vecpstr_envset(vecpstr *op,cchar *kp,cchar *vp,int vl) noex {
    	vecpstrer	vo(op) ;
	return vo.envset(kp,vp,vl) ;
}
/* end subroutine (vecpstr_envset) */

int vecpstr_envget(vecpstr *op,cchar *kp,cchar **rpp) noex {
    	vecpstrer	vo(op) ;
	return vo.envget(kp,rpp) ;
}
/* end subroutine (vecpstr_envget) */

int vecpstr_envfile(vecpstr *op,cchar *fname) noex {
    	vecpstrer	vo(op) ;
	return vo.envfile(fname) ;
}
/* end subroutine (vecpstr_envfile) */

int vecpstr_foilcmp(vecpstr *op,vecpstr *rnp) noex {
    	vecpstrer	vo(op) ;
	vecpstrer	voo(rnp) ;
	return vo.foilcmp(&voo) ;
}
/* end subroutine (vecpstr_foilcmp) */

int vecpstr_loadfile(vecpstr *op,int fu,cchar *fname) noex {
    	vecpstrer	vo(op) ;
	return vo.loadfile(fu,fname) ;
}
/* end subroutine (vecpstr_loadfile) */

int vecpstr_loadgrusers(vecpstr *op,gid_t sgid) noex {
    	vecpstrer	vo(op) ;
	return vo.loadgrusers(sgid) ;
}
/* end subroutine (vecpstr_loadgrusers) */

int vecpstr_loadpjusers(vecpstr *op,cchar *pjn) noex {
    	vecpstrer	vo(op) ;
	return vo.loadpjusers(pjn) ;
}
/* end subroutine (vecpstr_loadpjusers) */

int vecpstr_srvargs(vecpstr *op,cchar *abuf) noex {
    	vecpstrer	vo(op) ;
	return vo.srvargs(abuf) ;
}
/* end subroutine (vecpstr_srvargs) */

int vecpstr_svcargs(vecpstr *op,int *fp,cchar *abuf) noex {
    	vecpstrer	vo(op) ;
	return vo.svcargs(fp,abuf) ;
}
/* end subroutine (vecpstr_svcargs) */


/* local subroutines */

int vecpstrer::add(cchar *sp,int sl) noex {
    	return vecpstr_add(op,sp,sl) ;
}

int vecpstrer::adduniq(cchar *sp,int sl) noex {
    	return vecpstr_adduniq(op,sp,sl) ;
}

int vecpstrer::addkeyval(cchar *kp,int kl,cchar *valp,int vall) noex {
    	return vecpstr_addkeyval(op,kp,kl,valp,vall) ;
}

int vecpstrer::insert(int ii,cchar *sp,int sl) noex {
    	return vecpstr_insert(op,ii,sp,sl) ;
}

int vecpstrer::get(int i,cchar **rpp) noex {
    	return vecpstr_get(op,i,rpp) ;
}

int vecpstrer::getvec(mainv *mp) noex {
    	return vecpstr_getvec(op,mp) ;
}

int vecpstrer::finder(cchar *s,vecpstr_f vcmp,cchar **rpp) noex {
    	return vecpstr_finder(op,s,vcmp,rpp) ;
}

int vecpstrer::del(int i) noex {
	return vecpstr_del(op,i) ;
}

int vecpstrer::sort(vecpstr_f vcmp) noex {
	return vecpstr_sort(op,vcmp) ;
}


