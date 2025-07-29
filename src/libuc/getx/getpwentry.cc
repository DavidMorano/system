/* getpwentry SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* subroutines to access the 'passwd' and 'group' databases */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-16, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	gepwentry

	Description:
	This subroutine was written so that we could use a single
	interface to access the 'passwd' database on all UNIX®
	platforms.  This code module provides a platform independent
	implementation of UNIX® 'passwd' database access subroutines.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<ucpwcache.h>
#include	<getbufsize.h>
#include	<getax.h>
#include	<getpwx.h>
#include	<gecos.h>
#include	<getusername.h>
#include	<mallocxx.h>
#include	<storeitem.h>
#include	<strn.h>
#include	<snwcpy.h>
#include	<isoneof.h>
#include	<localmisc.h>

#include	"pwentry.h"
#include	"getpwentry.h"

import libutil ;

/* local defines */

#if	defined(SYSHAS_SHADOW) && (SYSHAS_SHADOW > 0)
#define	F_SHADOW	1
#else
#define	F_SHADOW	0
#endif

#define	SI		storeitem


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward subroutines */

static int	getpwentry_load(pwentry *,char *,int,ucentpw *) noex ;
static int	getpwentry_gecos(pwentry *,SI *,cchar *) noex ;
static int	getpwentry_shadow(pwentry *,SI *,ucentpw *) noex ;
static int	getpwentry_setnuls(pwentry *,cchar *) noex ;

static int	isNoEntry(int) noex ;

static int	checknul(cchar *,cchar **) noex ;


/* local variables */

constexpr int		rsents[] = {
	SR_NOTFOUND,
	SR_ACCESS,
	SR_NOSYS,
	0
} ;

constexpr bool		f_shadow = F_SHADOW ;


/* exported variables */


/* exported subroutines */

int getpwentry_name(pwentry *uep,char *ebuf,int elen,cchar *name) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (uep && ebuf && name) {
	    if (char *pwbuf{} ; (rs = malloc_pw(&pwbuf)) >= 0) {
	        ucentpw		pw ;
	        cint		pwlen = rs ;
	        if ((rs = getpwx_name(&pw,pwbuf,pwlen,name)) >= 0) {
	            rs = getpwentry_load(uep,ebuf,elen,&pw) ;
	        }
	        rs1 = uc_free(pwbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (getpwentry_name) */

int getpwentry_uid(pwentry *uep,char *ebuf,int elen,uid_t uid) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (uep && ebuf) {
	    if (char *pwbuf{} ; (rs = malloc_pw(&pwbuf)) >= 0) {
	        ucentpw		pw ;
		cint		pwlen = rs ;
	        if ((rs = getpwusername(&pw,pwbuf,pwlen,uid)) >= 0) {
	            rs = getpwentry_load(uep,ebuf,elen,&pw) ;
	        }
	        rs1 = uc_free(pwbuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (getpwentry_uid) */


/* local subroutines */

static int getpwentry_load(pwentry *uep,char *ebuf,int elen,ucentpw *pep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (uep && ebuf && pep) {
	    memclear(uep) ;		 /* noted potentially dangerous */
	    if (pep->pw_name != nullptr) {
	        cchar		*emptyp = nullptr ;
	        if (SI ub ; (rs = ub.start(ebuf,elen)) >= 0) {
		    cchar	**vpp = &uep->username ;
/* fill in the stuff that we got from the system */
	            rs = ub.strw(pep->pw_name,-1,vpp) ;
	            emptyp = (uep->username + rs) ;
		    if_constexpr (f_shadow) {
	                if ((rs >= 0) && pep->pw_passwd) {
		            vpp = &uep->password ;
	                    ub.strw(pep->pw_passwd,-1,vpp) ;
	                }
		    } /* end if_constexpr (f_shadow) */
	            uep->uid = pep->pw_uid ;
	            uep->gid = pep->pw_gid ;
	            if ((rs >= 0) && (pep->pw_gecos != nullptr)) {
		        vpp = &uep->gecos ;
	                ub.strw(pep->pw_gecos,-1,vpp) ;
		    }
/* break up the gecos field further */
		    if (rs >= 0) {
		        cchar	*gecos = pep->pw_gecos ;
	                if ((rs = getpwentry_gecos(uep,&ub,gecos)) >= 0) {
	                    if (pep->pw_dir != nullptr) {
			        vpp = &uep->dir ;
	                        ub.strw(pep->pw_dir,-1,vpp) ;
		            }
	                    if (pep->pw_shell != nullptr) {
			        vpp = &uep->shell ;
	                        ub.strw(pep->pw_shell,-1,vpp) ;
		            }
			    if (rs >= 0) {
				auto	gpws = getpwentry_shadow ;
	                        if ((rs = gpws(uep,&ub,pep)) >= 0) {
	                            rs = 0 ;
	                        } else if (isNoEntry(rs)) {
	                            rs = SR_OK ;
	                        }
			    } /* end if (ok) */
	                } /* end if (gecos) */
		    } /* end if (ok) */
	            getpwentry_setnuls(uep,emptyp) ;
	            rs1 = ub.finish ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (storeitem) */
	    } else {
	        rs = SR_NOTFOUND ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (getpwentry_load) */

static int getpwentry_gecos(pwentry *uep,SI *sip,cchar *gecosdata) noex {
	int		rs ;
	int		rs1 ;
	if (gecos g ; (rs = gecos_start(&g,gecosdata,-1)) >= 0) {
	    for (int i = 0 ; i < gecosval_overlast ; i += 1) {
	        cchar	*vp{} ;	/* "Value-Pointer" */
	        if (int vl ; (vl = gecos_getval(&g,i,&vp)) >= 0) {
	            void	*mp{} ;	/* "Malloc-Pointer" */
		    cchar	**vpp = nullptr ;
	            switch (i) {
	            case gecosval_organization:
	                vpp = &uep->organization ;
	                break ;
	            case gecosval_realname:
	                if ((rs = uc_malloc((vl+1),&mp)) >= 0) {
	                    char	*nbuf = charp(mp) ;
	                    if (strnchr(vp,vl,'_') != nullptr) {
	                        rs = snwcpyhyphen(nbuf,-1,vp,vl) ;
	                        vp = nbuf ;
	                    }
	                    if (rs >= 0) {
	                        vpp = &uep->realname ;
	                        rs = sip->strw(vp,vl,vpp) ;
				vpp = nullptr ;
	                    }
	                    rs1 = uc_free(mp) ;
			    if (rs >= 0) rs = rs1 ;
	                } /* end if (m-a-f) */
	                break ;
	            case gecosval_account:
	                vpp = &uep->account ;
	                break ;
	            case gecosval_bin:
	                vpp = &uep->bin ;
	                break ;
	            case gecosval_office:
	                vpp = &uep->office ;
	                break ;
	            case gecosval_wphone:
	                vpp = &uep->wphone ;
	                break ;
	            case gecosval_hphone:
	                vpp = &uep->hphone ;
	                break ;
	            case gecosval_printer:
	                vpp = &uep->printer ;
	                break ;
	            } /* end switch */
		    if ((rs >= 0) && vpp) {
	                rs = sip->strw(vp,vl,vpp) ;
		    }
	        } /* end if (gecos_getval) */
	        if (rs < 0) break ;
	    } /* end for */
	    rs1 = gecos_finish(&g) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (gecos) */
	return rs ;
}
/* end subroutine (getpwentry_gecos) */

static int getpwentry_shadow(pwentry *uep,SI *sip,ucentpw *pep) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if_constexpr (f_shadow) {
	    if (char *spbuf{} ; (rs = malloc_sp(&spbuf)) >= 0) {
	        ucentsp		sd ;
		cint		splen = rs ;
	        cchar		*pn = pep->pw_name ;
	        cchar		**vpp = &uep->password ;
	        if ((rs = getsp_name(&sd,spbuf,splen,pn)) >= 0) {
	            uep->lstchg = sd.sp_lstchg ;
	            uep->daymin = sd.sp_min ;
	            uep->daymax = sd.sp_max ;
	            uep->warn = sd.sp_warn ;
	            uep->inact = sd.sp_inact ;
	            uep->expire = sd.sp_expire ;
	            uep->flag = sd.sp_flag ;
	            if (pep->pw_passwd != nullptr) {
	                if ((strcmp(pep->pw_passwd,"*NP*") == 0) ||
	                    (strcmp(pep->pw_passwd,"x") == 0)) {
	                    sip->strw(sd.sp_pwdp,-1,vpp) ;
	                } else {
	                    cchar	*pwp = pep->pw_passwd ;
	                    sip->strw(pwp,-1,vpp) ;
	                }
	            } else {
	                sip->strw(sd.sp_pwdp,-1,vpp) ;
	            }
	        } else if (rs == SR_ACCESS) {
	            rs = SR_OK ;
	            if (pep->pw_passwd != nullptr) {
	                rs = sip->strw(pep->pw_passwd,-1,vpp) ;
	            }
	        } /* end if */
	        rs1 = uc_free(spbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} else {
	    rs = SR_NOSYS ;
	    uep->lstchg = 1 ;
	    uep->daymin = -1 ;
	    uep->daymax = -1 ;
	    uep->warn = -1 ;
	    uep->inact = -1 ;
	    uep->expire = 0 ;
	    uep->flag = 0 ;
	} /* end if_constexpr (f_shadow) */
	return rs ;
}
/* end subroutine (getpwentry_shadow) */

static int getpwentry_setnuls(pwentry *uep,cchar *emptyp) noex {
	checknul(emptyp,&uep->username) ;
	checknul(emptyp,&uep->password) ;
	checknul(emptyp,&uep->gecos) ;
	checknul(emptyp,&uep->dir) ;
	checknul(emptyp,&uep->shell) ;
	checknul(emptyp,&uep->organization) ;
	checknul(emptyp,&uep->realname) ;
	checknul(emptyp,&uep->account) ;
	checknul(emptyp,&uep->bin) ;
	checknul(emptyp,&uep->name_m1) ;
	checknul(emptyp,&uep->name_m2) ;
	checknul(emptyp,&uep->name_l) ;
	checknul(emptyp,&uep->office) ;
	checknul(emptyp,&uep->wphone) ;
	checknul(emptyp,&uep->hphone) ;
	checknul(emptyp,&uep->printer) ;
	return SR_OK ;
}
/* end subroutine (getpwentry_setnuls) */

static int checknul(cchar *emptyp,cchar **epp) noex {
	if (*epp == nullptr) {
	    *epp = emptyp ;
	}
	return 0 ;
}
/* end subroutine (checknul) */

static int isNoEntry(int rs) noex {
	return isOneOf(rsents,rs) ;
}
/* end subroutine (isNoEntry) */


