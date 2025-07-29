/* localgetorg SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the organization name (string) for a specified user-name */
/* version %I% last-modified %G% */

#define	CF_USERORG	0		/* try to access "user" org also */

/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	localgetorg

	Description:
	This subroutine retrieves the organization name (string)
	for a specified user-name.

	Synopsis:
	int localgetorg(cc *pr,char *rbuf,int rlen,cc* username) noex

	Arguments:
	pr		program-root
	rbuf		user supplied buffer to hold result
	rlen		length of user supplied buffer
	username	username to look up

	Returns:
	>=0		length of return organization string
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strcmp(3c)| */
#include	<usystem.h>
#include	<ucpwcache.h>
#include	<getusername.h>
#include	<getuserhome.h>
#include	<getax.h>
#include	<getpwx.h>
#include	<gecos.h>
#include	<getbufsize.h>
#include	<mallocxx.h>
#include	<varnames.hh>
#include	<syswords.hh>
#include	<filereadln.h>
#include	<sfx.h>
#include	<sncpyx.h>
#include	<sncpyxw.h>
#include	<mkpathx.h>
#include	<strlibval.hh>
#include	<char.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"localget.h"

import libutil ;

/* local defines */

#define	SI		subinfo

#ifndef	LOCALUSERNAME
#define	LOCALUSERNAME	"local"
#endif

#undef	ORGCNAME
#define	ORGCNAME	"organization"


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct subinfo {
	cchar		*pr ;
	cchar		*prn ;		/* program-root name */
	cchar		*ofn ;		/* organization file-name */
	cchar		*un ;
	char		*rbuf ;		/* user-supplied buffer */
	int		rlen ;
} ;


/* forward references */

static int subinfo_start(SI *,cchar *,cchar *,cchar *,char *,int) noex ;
static int subinfo_homer(SI *,cchar *) noex ;
static int subinfo_passwder(SI *,cchar *) noex ;
static int subinfo_finish(SI *) noex ;

#if	CF_USERORG
static int	localgetorg_var(SI *) noex ;
static int	localgetorg_home(SI *) noex ;
static int	localgetorg_passwd(SI *) noex ;
#endif /* CF_USERORG */

static int	localgetorg_prhome(SI *) noex ;
static int	localgetorg_pretc(SI *) noex ;
static int	localgetorg_prpasswd(SI *) noex ;
static int	localgetorg_sys(SI *) noex ;


/* local variables */

static cchar		*etcdir = sysword.w_etcdir ;


/* exported variables */


/* exported subroutines */

int localgetorg(cchar *pr,char *rbuf,int rlen,cchar *username) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (pr && rbuf && username) {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if (username[0]) {
	        SI		si, *sip = &si ;
	        cchar		*ofn = ORGCNAME ;
	        if ((rs = subinfo_start(&si,pr,ofn,username,rbuf,rlen)) >= 0) {
	            for (int i = 0 ; i < 7 ; i += 1) {
		        rs = 0 ;
		        switch (i) {
#if	CF_USERORG
		        case 0:
		             rs = localgetorg_var(sip) ;
		             break ;
		        case 1:
		             rs = localgetorg_home(sip) ;
		             break ;
		        case 2:
		             rs = localgetorg_passwd(sip) ;
		             break ;
#endif /* CF_USERORG */
		        case 3:
		             rs = localgetorg_prhome(sip) ;
		             break ;
		        case 4:
		             rs = localgetorg_pretc(sip) ;
		             break ;
		        case 5:
		             rs = localgetorg_prpasswd(sip) ;
		             break ;
		        case 6:
		             rs = localgetorg_sys(sip) ;
		             break ;
		        } /* end switch */
		        len = rs ;
		        if ((rs < 0) && (! isNotPresent(rs))) break ;
		        if (len > 0) break ;
	            } /* end for */
	            rs1 = subinfo_finish(&si) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (subinfo) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (localgetorg) */


/* local subroutines */

static int subinfo_start(SI *sip,cc *pr,cc *ofn,cc *un,
		char *rbuf,int rlen) noex {
	int		rs = SR_OK ;
	int		cl ;
	cchar		*cp{} ;
	memclear(sip) ;
	sip->pr = pr ;
	sip->un = un ;
	sip->ofn = ofn ;
	sip->rbuf = rbuf ;
	sip->rlen = rlen ;
	if ((cl = sfbasename(sip->pr,-1,&cp)) > 0) {
	    while ((cl > 0) && (cp[cl-1] == '/')) cl -= 1 ;
	}
	if (cl <= 0) {
	    cp = LOCALUSERNAME ;
	    cl = -1 ;
	}
	if (cchar *zp{} ; (rs = uc_mallocstrw(cp,cl,&zp)) >= 0) {
	    sip->prn = zp ;
	}
	return rs ;
}
/* end subroutine (subinfo_start) */

static int subinfo_finish(SI *sip) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (sip) {
	    rs = SR_OK ;
	    if (sip->prn) {
	        rs1 = uc_free(sip->prn) ;
	        if (rs >= 0) rs = rs1 ;
	        sip->prn = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (subinfo_finish) */

#if	CF_USERORG

static int localgetorg_var(SI *sip) noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	cchar		*vnun = varname.username ;
	cchar		*vnorg = varname.organization ;
	cchar		*un = sip->un ;
	bool		f = (un[0] == '-') ;
	if (! f) {
	    static cchar	*vun = getenv(vnun) ;
	    if (vun && (vun[0] != '\0'))
	        f = (strcmp(vun,un) == 0) ;
	}
	if (f) {
	    static cchar	*vorg = getenv(vnorg) ;
	    if (vorg) {
		rs = sncpy1(sip->rbuf,sip->rlen,vorg) ;
		len = rs ;
	    }
	} /* end if */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (localgetorg_var) */

static int localgetorg_home(SI *sip) noex {
	return subinfo_homer(sip,sip->un) ;
}
/* end subroutine (localgetorg_home) */

static int localgetorg_passwd(SI *sip) noex {
	return subinfo_passwder(sip,sip->un) ;
}
/* end subroutine (localgetorg_passwd) */

#endif /* CF_USERORG */

static int localgetorg_prhome(SI *sip) noex {
	return subinfo_homer(sip,sip->prn) ;
}
/* end subroutine (localgetorg_prhome) */

static int localgetorg_prpasswd(SI *sip) noex {
	return subinfo_passwder(sip,sip->prn) ;
}
/* end subroutine (localgetorg_prpasswd) */

static int localgetorg_pretc(SI *sip) noex {
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	if (char *obuf{} ; (rs = malloc_mp(&obuf)) >= 0) {
	    if ((rs = mkpath(obuf,sip->pr,etcdir,sip->ofn)) >= 0) {
	        if ((rs = filereadln(obuf,sip->rbuf,sip->rlen)) >= 0) {
	            len = rs ;
	        } else if (isNotPresent(rs)) {
		    rs = SR_OK ;
	        }
	    }
	    rs1 = uc_free(obuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (localgetorg_pretc) */

static int localgetorg_sys(SI *sip) noex {
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	if (char *obuf{} ; (rs = malloc_mp(&obuf)) >= 0) {
	    if ((rs = mkpath(obuf,etcdir,sip->ofn)) >= 0) {
	        if ((rs = filereadln(obuf,sip->rbuf,sip->rlen)) >= 0) {
	            len = rs ;
	        } else if (isNotPresent(rs)) {
		    rs = SR_OK ;
	        }
	    }
	    rs1 = uc_free(obuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (localgetorg_sys) */

static int subinfo_homer(SI *sip,cchar *un) noex {
	int		rs ;
	int		len = 0 ;
	if (char *hbuf{} ; (rs = malloc_mp(&hbuf)) >= 0) {
	    cint	hlen = rs ;
	    if ((rs = getuserhome(hbuf,hlen,un)) >= 0) {
	        if (char *obuf{} ; (rs = malloc_mn(&obuf)) >= 0) {
		    cint	olen = rs ;
	            if ((rs = sncpy2(obuf,olen,"/.",sip->ofn)) >= 0) {
		        if (char *fbuf{} ; (rs = malloc_mp(&fbuf)) >= 0) {
	                    if ((rs = mkpath(fbuf,hbuf,obuf)) >= 0) {
		                cint	rlen = sip->rlen ;
		                char	*rbuf = sip->rbuf ;
	                        if ((rs = filereadln(fbuf,rbuf,rlen)) >= 0) {
	                            len = rs ;
	                        } else if (isNotPresent(rs)) {
		                    rs = SR_OK ;
				}
		            } /* end if (mkpath) */
			    rs = rsfree(rs,fbuf) ;
	                } /* end if (m-a-f) */
		    } /* end if (sncpy) */
		    rs = rsfree(rs,obuf) ;
	        } /* end if (m-a-f) */
	    } /* end if (getuserhome) */
	    rs = rsfree(rs,hbuf) ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (subinfo_homer) */

static int subinfo_passwder(SI *sip,cchar *un) noex {
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	sip->rbuf[0] = '\0' ;
	if (char *pwbuf ; (rs = malloc_pw(&pwbuf)) >= 0) {
	    cint	pwlen = rs ;
	    if (ucentpwx pw ; (rs = pw.nam(pwbuf,pwlen,un)) >= 0) {
		if (gecos g ; (rs = gecos_start(&g,pw.pw_gecos,-1)) >= 0) {
		    cint	gi = gecosval_organization ;
		    cchar	*vp{} ;
	            if (int vl ; (vl = gecos_getval(&g,gi,&vp)) > 0) {
	    	        rs = sncpy1w(sip->rbuf,sip->rlen,vp,vl) ;
		        len = rs ;
		    }
	            rs1 = gecos_finish(&g) ;
	    	    if (rs >= 0) rs = rs1 ;
	        } /* end if (gecos) */
	    } /* end if (get PW entry) */
	    rs1 = uc_free(pwbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (subinfo_passwder) */


