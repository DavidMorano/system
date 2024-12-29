/* uc_opendialer (open-dialer-service) */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* interface component for UNIX® library-3c */
/* open a dialer */
/* version %I% last-modified %G% */

#define	CF_GETEXECNAME	1		/* use 'getexecname(3c)' */

/* revision history:

	= 1998-07-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	uc_opendialer

	Description:
	Dialer services are represented as files with names of the form:
		<dialer>¥<svc>[­<arg(s)>
	These sorts of file names are often actually stored in the
	filesystem as symbolic links.

	Synopsis:
	int uc_opendialer(prn,svc,of,om,argv,envv,to)
	cchar	prn[] ;
	cchar	svc[] ;
	int		of ;
	mode_t		om ;
	cchar	**argv[] ;
	cchar	**envv[] ;
	int		to ;

	Arguments:
	prn		facility name
	svc		service name
	of		open-flags
	om		open-mode
	argv		array of arguments
	envv		attay of environment
	to		time-out

	Returns:
	<0		error
	>=0		file-descriptor

	Dialer services are implemented with loadable shared-object
	files. Each service has a file of the same name as the
	service name itself. The file is a shared-object with a
	global symbol of a callable subroutine with the name
	'opendialer_<svc>' where <svc> is the service name. The
	subroutine looks like:

	int opendialer_<svc>(pr,prn,of,om,argv,envv,to)
	cchar	*pr ;
	cchar	*prn ;
	int		of ;
	mode_t		om ;
	cchar	*argv[] ;
	cchar	*envv[] ;

	Multiple services can be actually implemented in the same
	shared-object.  But the actual file of that object should
	be linked to other files, each with the filename of a service
	to be implemented. These links are required because this
	code only searches for services by searching for files with
	the names of the services.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<dlfcn.h>
#include	<stdlib.h>
#include	<string.h>
#include	<usystem.h>
#include	<getnodedomain.h>
#include	<ids.h>
#include	<localmisc.h>


/* local defines */

#define	SUBINFO		struct subinfo
#define	SUBINFO_FL	struct subinfo_flags

#define	SVCDNAME	"lib/opendialers"
#define	SVCSYMPREFIX	"opendialer_"

#ifndef	SVCLEN
#define	SVCLEN		MAXNAMELEN
#endif


/* external subroutines */

extern int	snwcpy(char *,int,cchar *,int) ;
extern int	sncpy2(char *,int,cchar *,cchar *) ;
extern int	mkpath1w(char *,cchar *,int) ;
extern int	mkpath2(char *,cchar *,cchar *) ;
extern int	mksofname(char *,cchar *,cchar *,cchar *) ;
extern int	pathadd(char *,int,cchar *) ;
extern int	sfdirname(cchar *,int,cchar **) ;
extern int	sfbasename(cchar *,int,cchar **) ;
extern int	sfprogroot(cchar *,int,cchar **) ;
extern int	sperm(IDS *,struct ustat *,int) ;
extern int	getpwd(char *,int) ;
extern int	getuserhome(char *,int,cchar *) ;
extern int	mkpr(char *,int,cchar *,cchar *) ;
extern int	isNotPresent(int) ;

extern char	*strwcpy(char *,cchar *,int) ;
extern char	*strnchr(cchar *,int,int) ;
extern char	*strnpbrk(cchar *,int,cchar *) ;
extern char	*strdcpy1(char *,int,cchar *) ;
extern char	*strdcpy1w(char *,int,cchar *,int) ;
extern char	*strdcpy2w(char *,int,cchar *,cchar *,int) ;


/* external variables */

extern char	**environ ;


/* local structures */

typedef int (*subdialer_t)(cchar *,cchar *,cchar *,
	    int,mode_t,cchar **,cchar **,int) ;

struct subinfo_flags {
	uint		dummy:1 ;
} ;

struct subinfo {
	IDS		id ;
	SUBINFO_FL	f ;
	cchar	*prn ;
	cchar	*svc ;
	cchar	*dialsym ;	/* memory-allocated */
	cchar	**argv ;
	cchar	**envv ;
	mode_t		om ;
	int		of ;
	int		to ;
	int		fd ;
} ;


/* forward references */

static int	subinfo_start(SUBINFO *,cchar *,cchar *,
			int,mode_t,cchar **,cchar **,int) ;
static int	subinfo_finish(SUBINFO *) ;
static int	subinfo_search(SUBINFO *) ;
static int	subinfo_exts(SUBINFO *,cchar *,cchar *,char *) ;
static int	subinfo_searchlib(SUBINFO *,cchar *,cchar *) ;
static int	subinfo_searchcall(SUBINFO *,cchar *,subdialer_t) ;
static int	subinfo_idbegin(SUBINFO *) ;
static int	subinfo_idend(SUBINFO *) ;


/* local variables */

static cchar	*prns[] = {
	"extra",
	"preroot",
	NULL
} ;

static cchar	*soexts[] = {
	"so",
	"o",
	"",
	NULL
} ;


/* exported subroutines */

int uc_opendialer(prn,svc,of,om,argv,envv,to)
cchar	prn[] ;
cchar	svc[] ;
int		of ;
mode_t		om ;
cchar	*argv[] ;
cchar	*envv[] ;
int		to ;
{
	SUBINFO		si, *sip = &si ;
	int		rs ;
	int		rs1 ;
	int		fd = -1 ;

	if ((prn == NULL) && (svc == NULL)) return SR_FAULT ;

	if ((prn[0] == '\0') && (svc[0] == '\0')) return SR_INVALID ;

	if ((rs = subinfo_start(&si,prn,svc,of,om,argv,envv,to)) >= 0) {
	    if ((rs = subinfo_search(&si)) > 0) { /* >0 means found */
		fd = sip->fd ;
	    } else if (rs == 0) {
		rs = SR_NOENT ;
	    }
	    rs1 = subinfo_finish(&si) ;
	    if (rs >= 0) rs = rs1 ;
	    if ((rs < 0) && (fd >= 0)) u_close(fd) ;
	} /* end if (subinfo) */

	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (uc_opendialer) */


/* local subroutines */

static int subinfo_start(sip,prn,svc,of,om,argv,envv,to)
SUBINFO		*sip ;
cchar	*prn ;
cchar	*svc ;
int		of ;
mode_t		om ;
cchar	**argv ;
cchar	**envv ;
int		to ;
{
	int		rs = SR_OK ;

	memset(sip,0,sizeof(SUBINFO)) ;
	sip->prn = prn ;
	sip->svc = svc ;
	sip->argv = argv ;
	sip->envv = (envv != NULL) ? envv : ((cchar **) environ) ;
	sip->of = of ;
	sip->om = om ;
	sip->to = to ;
	sip->fd = -1 ;

	{
	    cchar	*prefix = SVCSYMPREFIX ;
	    char	dialsym[MAXNAMELEN+1] ;
	    if ((rs = sncpy2(dialsym,MAXNAMELEN,prefix,sip->prn)) >= 0) {
	        cchar	*ccp ;
	        if ((rs = uc_libmallocstrw(dialsym,rs,&ccp)) >= 0) {
	            sip->dialsym = ccp ;
		}
	    }
	}

	return rs ;
}
/* end subroutine (subinfo_start) */

static int subinfo_finish(SUBINFO *sip) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (sip->dialsym != NULL) {
	    rs1 = uc_libfree(sip->dialsym) ;
	    if (rs >= 0) rs = rs1 ;
	    sip->dialsym = NULL ;
	}

	return rs ;
}
/* end subroutine (subinfo_finish) */

static int subinfo_search(SUBINFO *sip) noex {
	const int	plen = MAXPATHLEN ;
	int		rs ;
	int		rs1 ;
	int		f = FALSE ;

	if ((rs = subinfo_idbegin(sip)) >= 0) {
	    const int	size = ((plen+1)*3) ;
	    char	*abuf ;
	    if ((rs = uc_libmalloc(size,&abuf)) >= 0) {
	        char	dn[MAXHOSTNAMELEN+1] ;
	        char	*pdn = (abuf+(0*(plen+1))) ;
	        char	*sdn = (abuf+(1*(plen+1))) ;
	        char	*sfn = (abuf+(2*(plen+1))) ;
	        if ((rs = getnodedomain(NULL,dn)) >= 0) {
		    struct ustat	sb ;
		    int			i = 0 ;

	            for (i = 0 ; prns[i] != NULL ; i += 1) {
	                if ((rs = mkpr(pdn,plen,prns[i],dn)) > 0) {
		            if ((rs = mkpath2(sdn,pdn,SVCDNAME)) >= 0) {
	        		if ((rs = u_stat(sdn,&sb)) >= 0) {
				    if (S_ISDIR(sb.st_mode)) {
	                                rs = subinfo_exts(sip,pdn,sdn,sfn) ;
	                                f = rs ;
				    } /* end if */
				} else if (isNotPresent(rs)) {
		    		    rs = SR_OK ;
				}
			    } /* end if (mkpath) */
	                } /* end if (mkpr) */
			if (f) break ;
			if (rs < 0) break ;
	            } /* end for (prns) */

	        } /* end if */
	        rs1 = uc_libfree(abuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ma-a) */
	    rs1 = subinfo_idend(sip) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (subinfo-id) */

	if ((rs < 0) && (sip->fd >= 0)) {
	    u_close(sip->fd) ;
	    sip->fd = -1 ;
	}

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (subinfo_search) */

static int subinfo_exts(SUBINFO *sip,cchar *pr,cchar *sdn,char *sfn) noex {
	USTAT		sb ;
	const int	am = (R_OK|X_OK) ;
	int		rs = SR_OK ;
	int		i ;
	int		f = FALSE ;
	cchar		*prn = sip->prn ;
	for (i = 0 ; soexts[i] != NULL ; i += 1) {
	    if ((rs = mksofname(sfn,sdn,prn,soexts[i])) >= 0) {
	        if ((rs = u_stat(sfn,&sb)) >= 0) {
		    if (S_ISREG(sb.st_mode)) {
	                if ((rs = sperm(&sip->id,&sb,am)) >= 0) {
			    rs = subinfo_searchlib(sip,pr,sfn) ;
			    f = rs ;
			} else if (rs == SR_ACCESS) {
			    rs = SR_OK ;
			}
		    } /* end if (regular file) */
		} else if (isNotPresent(rs)) {
		    rs = SR_OK ;
		}
	    } /* end if (mksofname) */
	    if (f) break ;
	    if (rs < 0) break ;
	} /* end for (soexts) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (subinfo_exts) */

static int subinfo_searchlib(SUBINFO *sip,cchar *pr,cchar *sfn) noex {
	const int	dlmode = RTLD_LAZY ;
	int		rs = SR_OK ;
	int		f = FALSE ;
	void		*sop ;
	if ((sop = dlopen(sfn,dlmode)) != NULL) {
	    subdialer_t	symp ;
	    if ((symp = (subdialer_t) dlsym(sop,sip->dialsym)) != NULL) {
		rs = subinfo_searchcall(sip,pr,symp) ;
		f = rs ;
	    } /* end if (dlsym) */
	    dlclose(sop) ;
	} /* end if (dlopen) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (subinfo_searchlib) */

static int subinfo_searchcall(SUBINFO *sip,cchar *pr,subdialer_t symp) noex {
	mode_t		om = sip->om ;
	int		rs ;
	int		of = sip->of ;
	int		to = sip->to ;
	int		f = FALSE ;
	cchar	*prn = sip->prn ;
	cchar	*svc = sip->svc ;
	cchar	**argv = sip->argv ;
	cchar	**envv = sip->envv ;

	if ((rs = (*symp)(pr,prn,svc,of,om,argv,envv,to)) >= 0) {
	    sip->fd = rs ;
	    f = TRUE ;
	} /* end if (call) */

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (subinfo_searchcall) */

static int subinfo_idbegin(SUBINFO *sip) noex {
	int		rs = ids_load(&sip->id) ;
	return rs ;
}
/* end subroutine (subinfo_idbegin) */

static int subinfo_idend(SUBINFO *sip) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (sip) {
	    rs = SR_OK ;
	    {
	        rs1 = ids_release(&sip->id) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	}
	return rs ;
}
/* end subroutine (subinfo_idend) */


