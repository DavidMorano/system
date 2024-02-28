/* localgetorg SUPPORT */
/* lang=C++20 */

/* get the organization name (string) for a specified user-name */
/* version %I% last-modified %G% */

#define	CF_USERORG	0		/* try to access "user" org also */
#define	CF_UCPWCACHE	1		/* use |ugetpw(3uc)| */

/* revision history:

	= 1998-07-01, David A­D­ Morano
	This subroutine was originally written.

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
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<usupport.h>		/* |memclear(3u)| */
#include	<ucpwcache.h>
#include	<getbufsize.h>
#include	<char.h>
#include	<filebuf.h>
#include	<getax.h>
#include	<getxusername.h>
#include	<getuserhome.h>
#include	<gecos.h>
#include	<filereadln.h>
#include	<sfx.h>
#include	<sncpyx.h>
#include	<sncpyxw.h>
#include	<mkpathx.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"localget.h"


/* local defines */

#if	CF_UCPWCACHE
#define	GETPW_NAME	ucpwcache_name
#else
#define	GETPW_NAME	getpw_name
#endif /* CF_UGETPW */

#define	SUBINFO		struct subinfo

#define	SI		SUBINFO

#ifndef	LOCALUSERNAME
#define	LOCALUSERNAME	"local"
#endif

#ifndef	ETCDNAME
#define	ETCDNAME	"/etc"
#endif

#undef	ORGCNAME
#define	ORGCNAME	"organization"

#ifndef	VARUSERNAME
#define	VARUSERNAME	"USERNAME"
#endif

#ifndef	VARORGANIZATION
#define	VARORGANIZATION	"ORGANIZATION"
#endif


/* external namespaces */


/* external variables */


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


/* exported variables */


/* exported subroutines */

int localgetorg(cchar *pr,char *rbuf,int rlen,cchar *username) noex {
	SUBINFO		si, *sip = &si ;
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	cchar	*ofn = ORGCNAME ;

	if ((pr == NULL) || (rbuf == NULL) || (username == NULL))
	    return SR_FAULT ;

	rbuf[0] = '\0' ;
	if (username[0] == '\0')
	    return SR_INVALID ;

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

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (localgetorg) */


/* local subroutines */

static int subinfo_start(SI *sip,cc *pr,cc *ofn,cc *un,
		char *rbuf,int rlen) noex {
	int		rs = SR_OK ;
	int		cl ;
	cchar	*cp ;
	cchar	*ccp ;

	memset(sip,0,sizeof(SUBINFO)) ;
	sip->pr = pr ;
	sip->un = un ;
	sip->ofn = ofn ;
	sip->rbuf = rbuf ;
	sip->rlen = rlen ;
	cl = sfbasename(sip->pr,-1,&cp) ;

	if (cl > 0) {
	    while ((cl > 0) && (cp[cl-1] == '/')) cl -= 1 ;
	}
	if (cl <= 0) {
	    cp = LOCALUSERNAME ;
	    cl = -1 ;
	}
	rs = uc_mallocstrw(cp,cl,&ccp) ;
	if (rs >= 0) sip->prn = ccp ;

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
	        sip->prn = NULL ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (subinfo_finish) */

#if	CF_USERORG

static int localgetorg_var(SI *sip) noex {
	int		rs = SR_OK ;
	int		f ;
	int		len = 0 ;
	cchar		*un = sip->un ;
	f = (un[0] == '-') ;
	if (! f) {
	    cchar	*vun = getenv(VARUSERNAME) ;
	    if ((vun != NULL) && (vun[0] != '\0'))
	        f = (strcmp(vun,un) == 0) ;
	}
	if (f) {
	    cchar	*orgp = getenv(VARORGANIZATION) ;
	    if (orgp != NULL) {
		rs = sncpy1(sip->rbuf,sip->rlen,orgp) ;
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
	int		rs = SR_OK ;
	int		len = 0 ;
	char		orgfname[MAXPATHLEN+1] ;
	if ((rs = mkpath3(orgfname,sip->pr,ETCDNAME,sip->ofn)) >= 0) {
	    if ((rs = filereadln(orgfname,sip->rbuf,sip->rlen)) >= 0) {
	        len = rs ;
	    } else if (isNotPresent(rs)) {
		rs = SR_OK ;
	    }
	}
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (localgetorg_pretc) */

static int localgetorg_sys(SI *sip) noex {
	int		rs ;
	int		len = 0 ;
	char		orgfname[MAXPATHLEN+1] ;
	if ((rs = mkpath2(orgfname,ETCDNAME,sip->ofn)) >= 0) {
	    if ((rs = filereadln(orgfname,sip->rbuf,sip->rlen)) >= 0) {
	        len = rs ;
	    } else if (isNotPresent(rs)) {
		rs = SR_OK ;
	    }
	}
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (localgetorg_sys) */

static int subinfo_homer(SI *sip,cchar *homeuser) noex {
	int		rs ;
	int		len = 0 ;
	char		homedname[MAXPATHLEN+1] ;
	if ((rs = getuserhome(homedname,MAXPATHLEN,homeuser)) >= 0) {
	    char	orgname[MAXNAMELEN+1] ;
	    if ((rs = sncpy2(orgname,MAXNAMELEN,"/.",sip->ofn)) >= 0) {
		char	orgfname[MAXPATHLEN+1] ;
	        if ((rs = mkpath2(orgfname,homedname,orgname)) >= 0) {
		    cint	rlen = sip->rlen ;
		    char	*rbuf = sip->rbuf ;
	            if ((rs = filereadln(orgfname,rbuf,rlen)) >= 0) {
	                len = rs ;
	            } else if (isNotPresent(rs))
		        rs = SR_OK ;
		} /* end if (mkpath) */
	    } /* end if (sncpy) */
	} /* end if (getuserhome) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (subinfo_homer) */

static int subinfo_passwder(SI *sip,cchar *homeuser) noex {
	PASSWD		pw ;
	cint		pwlen = getbufsize(getbufsize_pw) ;
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	char		*pwbuf{} ;
	sip->rbuf[0] = '\0' ;
	if ((rs = uc_malloc((pwlen+1),&pwbuf)) >= 0) {
	    if ((rs = GETPW_NAME(&pw,pwbuf,pwlen,homeuser)) >= 0) {
		gecos	g ;
	        if ((rs = gecos_start(&g,pw.pw_gecos,-1)) >= 0) {
		    int		vl ;
		    cint	gi = gecosval_organization ;
		    cchar	*vp{} ;
	            if ((vl = gecos_getval(&g,gi,&vp)) > 0) {
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


