/* getpwentry SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* retrieve a PWENTRY given a name or UID */
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
#include	<sys/types.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucpwcache.h>		/* LIBUC */
#include	<ucentpw.h>		/* LIBUC */
#include	<getax.h>		/* LIBUC */
#include	<getpwx.h>		/* LIBUC */
#include	<getusername.h>		/* LIBUC */
#include	<gecos.h>		/* LIBUC */
#include	<storeitem.h>		/* LIBUC */
#include	<strn.h>		/* LIBUC */
#include	<snwcpy.h>		/* LIBUC */
#include	<isoneof.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"pwentry.h"
#include	"getpwentry.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

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

local int	getpwentry_load		(pwentry *,char *,int,ucentpw *) noex ;
local int	getpwentry_gecos	(pwentry *,SI *,cchar *) noex ;
local int	getpwentry_shadow	(pwentry *,SI *,ucentpw *) noex ;
local int	getpwentry_setnuls	(pwentry *,cchar *) noex ;

local bool	isNoEntry(int) noex ;

local int	checknul(cchar *,cchar **) noex ;


/* local variables */

constexpr int		rsents[] = {
	SR_NOTFOUND,
	SR_ACCESS,
	SR_NOSYS,
	0
} ; /* end array (rsents) */

constexpr bool		f_shadow = F_SHADOW ;


/* exported variables */


/* exported subroutines */

int getpwentry_name(pwentry *uep,char *ebuf,int elen,cchar *name) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rv = 0 ; /* return-value */
	if (uep && ebuf && name) ylikely {
	    if (char *pwbuf ; (rs = lm_pw(&pwbuf)) >= 0) ylikely {
	        ucentpw		pw ;
	        cint		pwlen = rs ;
	        if ((rs = getpwx_name(&pw,pwbuf,pwlen,name)) >= 0) ylikely {
	            rs = getpwentry_load(uep,ebuf,elen,&pw) ;
		    rv = rs ;
	        }
	        rs1 = lm_free(pwbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (non-null) */
	return (rs >= 0) ? rv : rs ;
} /* end subroutine (getpwentry_name) */

int getpwentry_uid(pwentry *uep,char *ebuf,int elen,uid_t uid) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rv = 0 ; /* return-value */
	if (uep && ebuf) ylikely {
	    if (char *pwbuf ; (rs = lm_pw(&pwbuf)) >= 0) ylikely {
	        ucentpw		pw ;
		cint		pwlen = rs ;
	        if ((rs = getpwusername(&pw,pwbuf,pwlen,uid)) >= 0) ylikely {
	            rs = getpwentry_load(uep,ebuf,elen,&pw) ;
		    rv = rs ;
	        }
	        rs1 = lm_free(pwbuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (non-null) */
	return (rs >= 0) ? rv : rs ;
} /* end subroutine (getpwentry_uid) */


/* local subroutines */

local int getpwentry_load(pwentry *uep,char *ebuf,int elen,ucentpw *pep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (uep && ebuf && pep) ylikely {
	    memclear(uep) ;		 /* noted potentially dangerous */
	    if (pep->pw_name) {
	        cchar	*emptyp = nullptr ;
	        if (SI ub ; (rs = ub.start(ebuf,elen)) >= 0) ylikely {
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
				cauto	gpws = getpwentry_shadow ;
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
} /* end subroutine (getpwentry_load) */

local int getpwentry_gecos(pwentry *uep,SI *sip,cchar *gecosdata) noex {
	int		rs ;
	int		rs1 ;
	if (gecos g ; (rs = gecos_start(&g,gecosdata,-1)) >= 0) ylikely {
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
	                if ((rs = lm_mall((vl+1),&mp)) >= 0) {
	                    char *nbuf = charp(mp) ;
	                    if (strnchr(vp,vl,'_') != nullptr) {
	                        rs = snwcpyhyphen(nbuf,-1,vp,vl) ;
	                        vp = nbuf ;
	                    }
	                    if (rs >= 0) {
	                        vpp = &uep->realname ;
	                        rs = sip->strw(vp,vl,vpp) ;
				vpp = nullptr ;
	                    } /* end if (ok) */
	                    rs1 = lm_free(mp) ;
			    if (rs >= 0) rs = rs1 ;
	                } /* end if (m-a-f) */
	                break ;
	            case gecosval_account:
	                vpp = &uep->account ;
	                break ;
	            case gecosval_prbin:
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
} /* end subroutine (getpwentry_gecos) */

local int getpwentry_shadow(pwentry *uep,SI *sip,ucentpw *pep) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if_constexpr (f_shadow) {
	    if (char *spbuf ; (rs = lm_sp(&spbuf)) >= 0) ylikely {
		cint	splen = rs ;
	        cchar	*pn = pep->pw_name ;
	        cchar	**vpp = &uep->password ;
	        if (ucentsp sd ; (rs = getsp_name(&sd,spbuf,splen,pn)) >= 0) {
	            uep->lstchg	= sd.sp_lstchg ;
	            uep->daymin	= sd.sp_min ;
	            uep->daymax	= sd.sp_max ;
	            uep->warn	= sd.sp_warn ;
	            uep->inact	= sd.sp_inact ;
	            uep->expire	= sd.sp_expire ;
	            uep->flag	= sd.sp_flag ;
	            if (pep->pw_passwd) {
			bool f = false ;
			cc *pp = pep->pw_passwd ;
	                f = f || (strcmp(pp,"*NP*") == 0) ;
			f = f || (strcmp(pp,"x") == 0) ;
			if (f) {
	                    sip->strw(sd.sp_pwdp,-1,vpp) ;
	                } else {
	                    sip->strw(pp,-1,vpp) ;
	                }
	            } else {
	                sip->strw(sd.sp_pwdp,-1,vpp) ;
	            }
	        } else if (rs == SR_ACCESS) {
	            rs = SR_OK ;
	            if (pep->pw_passwd) {
	                rs = sip->strw(pep->pw_passwd,-1,vpp) ;
	            }
	        } /* end if */
	        rs1 = lm_free(spbuf) ;
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
} /* end subroutine (getpwentry_shadow) */

local int getpwentry_setnuls(pwentry *uep,cchar *emptyp) noex {
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
} /* end subroutine (getpwentry_setnuls) */

local int checknul(cchar *emptyp,cchar **epp) noex {
	if (*epp == nullptr) {
	    *epp = emptyp ;
	}
	return 0 ;
} /* end subroutine (checknul) */

local bool isNoEntry(int rs) noex {
	return isOneOf(rsents,rs) ;
} /* end subroutine (isNoEntry) */


