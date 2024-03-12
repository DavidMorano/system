/* getpwentry SUPPORT */
/* lang=C++20 */

/* subroutines to access the 'passwd' and 'group' databases */
/* version %I% last-modified %G% */

#define	CF_PWCACHE	1		/* use |ugetpw(3uc)| */

/* revision history:

	= 1998-06-16, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine was written so that we could use a single
	interface to access the 'passwd' database on all UNIX®
	platforms.  This code module provides a platform independent
	implementation of UNIX® 'passwd' database access subroutines.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<usupport.h>
#include	<ucpwcache.h>
#include	<getbufsize.h>
#include	<mallocxx.h>
#include	<getax.h>
#include	<gecos.h>
#include	<getusername.h>
#include	<storeitem.h>
#include	<strn.h>
#include	<snwcpy.h>
#include	<isoneof.h>
#include	<localmisc.h>

#include	"pwentry.h"
#include	"getpwentry.h"


/* local defines */

#if	CF_PWCACHE
#define	GETPW_NAME	ucpwcache_name
#define	GETPW_UID	ugpwcache_uid
#else
#define	GETPW_NAME	getpw_name
#define	GETPW_UID	getpw_uid
#endif /* CF_PWCACHE */

#if	defined(SYSHAS_SHADOW) && (SYSHAS_SHADOW > 0)
#define	F_SHADOW	1
#else
#define	F_SHADOW	0
#endif


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward subroutines */

static int	getpwentry_load(pwentry *,char *,int,PASSWD *) noex ;
static int	getpwentry_gecos(pwentry *,storeitem *,cchar *) noex ;
static int	getpwentry_shadow(pwentry *,storeitem *,PASSWD *) noex ;
static int	getpwentry_setnuls(pwentry *,cchar *) noex ;

static int	isNoEntry(int) noex ;

static int	checknul(cchar *,cchar **) noex ;


/* local variables */

static constexpr int	rsents[] = {
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
	    char	*pwbuf{} ;
	    if ((rs = malloc_pw(&pwbuf)) >= 0) {
	        PASSWD	pw ;
	        cint	pwlen = rs ;
	        if ((rs = GETPW_NAME(&pw,pwbuf,pwlen,name)) >= 0) {
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
	    char	*pwbuf{} ;
	    if ((rs = malloc_pw(&pwbuf)) >= 0) {
	        PASSWD	pw ;
		cint	pwlen = rs ;
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

static int getpwentry_load(pwentry *uep,char *ebuf,int elen,PASSWD *pep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (uep && ebuf && pep) {
	    memclear(uep) ;		 /* noted potentially dangerous */
	    if (pep->pw_name != nullptr) {
	        storeitem	ubuf ;
	        cchar	*emptyp = nullptr ;
	        if ((rs = storeitem_start(&ubuf,ebuf,elen)) >= 0) {
		    cchar	**vpp ;
/* fill in the stuff that we got from the system */
		    vpp = &uep->username ;
	            rs = storeitem_strw(&ubuf,pep->pw_name,-1,vpp) ;
	            emptyp = (uep->username + rs) ;
		    if constexpr (f_shadow) {
	                if ((rs >= 0) && pep->pw_passwd) {
		            vpp = &uep->password ;
	                    storeitem_strw(&ubuf,pep->pw_passwd,-1,vpp) ;
	                }
		    } /* end if-constexpr (f_shadow) */
	            uep->uid = pep->pw_uid ;
	            uep->gid = pep->pw_gid ;
	            if ((rs >= 0) && (pep->pw_gecos != nullptr)) {
		        vpp = &uep->gecos ;
	                storeitem_strw(&ubuf,pep->pw_gecos,-1,vpp) ;
		    }
/* break up the gecos field further */
		    if (rs >= 0) {
		        cchar	*gecos = pep->pw_gecos ;
	                if ((rs = getpwentry_gecos(uep,&ubuf,gecos)) >= 0) {
	                    if (pep->pw_dir != nullptr) {
			        vpp = &uep->dir ;
	                        storeitem_strw(&ubuf,pep->pw_dir,-1,vpp) ;
		            }
	                    if (pep->pw_shell != nullptr) {
			        vpp = &uep->shell ;
	                        storeitem_strw(&ubuf,pep->pw_shell,-1,vpp) ;
		            }
	                    if ((rs = getpwentry_shadow(uep,&ubuf,pep)) >= 0) {
	                        rs = 0 ;
	                    } else if (isNoEntry(rs)) {
	                        rs = SR_OK ;
	                    }
	                } /* end if (gecos) */
		    } /* end if (ok) */
	            getpwentry_setnuls(uep,emptyp) ;
	            rs1 = storeitem_finish(&ubuf) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (storeitem) */
	    } else {
	        rs = SR_NOTFOUND ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (getpwentry_load) */

static int getpwentry_gecos(pwentry *uep,storeitem *bp,cchar *gecosdata) noex {
	gecos		g ;
	int		rs ;
	int		rs1 ;
	if ((rs = gecos_start(&g,gecosdata,-1)) >= 0) {
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
	                        rs = storeitem_strw(bp,vp,vl,vpp) ;
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
	                rs = storeitem_strw(bp,vp,vl,vpp) ;
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

static int getpwentry_shadow(pwentry *uep,storeitem *sip,PASSWD *pep) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if constexpr (f_shadow) {
	    char	*spbuf{} ;
	    if ((rs = malloc_sp(&spbuf)) >= 0) {
	        ucentsp	sd ;
		cint	splen = rs ;
	        cchar	*pn = pep->pw_name ;
	        cchar	**vpp = &uep->password ;
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
	                    storeitem_strw(sip,sd.sp_pwdp,-1,vpp) ;
	                } else {
	                    cchar	*pwp = pep->pw_passwd ;
	                    storeitem_strw(sip,pwp,-1,vpp) ;
	                }
	            } else {
	                storeitem_strw(sip,sd.sp_pwdp,-1,vpp) ;
	            }
	        } else if (rs == SR_ACCESS) {
	            rs = SR_OK ;
	            if (pep->pw_passwd != nullptr) {
	                rs = storeitem_strw(sip,pep->pw_passwd,-1,vpp) ;
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
	} /* end if-constexpr (f_shadow) */
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


