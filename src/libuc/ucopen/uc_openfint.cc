/* uc_openfint SUPPORT (open-facility-intercept) */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* open a facility-intercept */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	uc_openfint

	Description:
	This subroutine opens what is referred to as a
	"facility-intercept." Facility intercepts provide a way for
	opens on files to be intercepted by a facility manager
	specified by the intercept name.  Facilities are normally
	or usually really a software distribution that has its own
	source-root or program-root.  Examples of software distributions
	within AT&T are:

	NCMP
	PCS (Personal Communication Services)
	EXPTOOLS (Experimental Tools)
	LOCAL
	REXEC (Remote Execution)
	POST
	EXTRA
	AST (Advanced Software Technologies)
	GNU

	Facilities are stored in the filesystem hierarchy rooted
	at a certain place.  Facilities are usually rooted at one
	of the following locations in the system filesystem hierarchy:

	/usr
	/usr/add-on
	/opt

	Facility services are represented in the filesystem as files
	with names of the form:

	<facility>º<int>[­<arg(s)>

	These sorts of file names are often actually stored in the
	filesystem as symbolic links.

	Synopsis:
	int uc_openfint(pr,dn,bn,prn,svc,of,om,argv,envv,to)
	cchar	pr[] ;
	cchar	dn[] ;
	cchar	bn[] ;
	cchar	prn[] ;
	cchar	svc[] ;
	int		of ;
	mode_t		om ;
	cchar	**argv[] ;
	cchar	**envv[] ;
	int		to ;

	Arguments:
	pr		program-root
	dn		directory name (containing the file)
	dn		base name of the file
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


	Facility intercepts are implemented with loadable shared-object
	files.  Each service has a file of the same name as the
	intercept name itself.  The file is a shared-object with a
	global symbol of a callable subroutine with the name
	'openint_<int>' where <int> is the intercept name.  The
	subroutine looks like:

	int openint_<int>(pr,dn,bn,prn,of,om,argv,envv,to)
	cchar	*pr ;
	cchar	*dn ;
	cchar	*bn ;
	cchar	*prn ;
	int		of ;
	mode_t		om ;
	cchar	*argv[] ;
	cchar	*envv[] ;

	Notes:
	Multiple intercepts can be actually implemented in the same
	shared-object.  But the actual file of that object should
	be linked to other files, each with the filename of an
	intercept to be implemented.  These links are required
	because this code only searches for intercepts by searching
	for files with the names of the intercepts.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<dlfcn.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<ids.h>
#include	<localmisc.h>


/* local defines */

#define	SUBINFO		struct subinfo
#define	SUBINFO_FL	struct subinfo_flags

#define	INTDNAME	"lib/openints"
#define	INTSYMPREFIX	"openint_"

#ifndef	INTLEN
#define	INTLEN		MAXNAMELEN
#endif

#undef	SR_LIBISNOTLOCAL
#define	SR_LIBISNOTLOCAL	SR_LIBSCN


/* external subroutines */

extern int	snwcpy(char *,int,cchar *,int) ;
extern int	sncpy2(char *,int,cchar *,cchar *) ;
extern int	mkpath1w(char *,cchar *,int) ;
extern int	mkpath2(char *,cchar *,cchar *) ;
extern int	mksofname(char *,cchar *,cchar *,cchar *) ;
extern int	sperm(IDS *,struct ustat *,int) ;

extern char	*strwcpy(char *,cchar *,int) ;
extern char	*strnchr(cchar *,int,int) ;
extern char	*strnpbrk(cchar *,int,cchar *) ;
extern char	*strdcpy1(char *,int,cchar *) ;
extern char	*strdcpy1w(char *,int,cchar *,int) ;
extern char	*strdcpy2w(char *,int,cchar *,cchar *,int) ;


/* external variables */

extern char	**environ ;


/* local structures */

struct subinfo_flags {
	uint		dummy:1 ;
} ;

struct subinfo {
	cchar	*pr ;
	cchar	*dn ;
	cchar	*bn ;
	cchar	*prn ;
	cchar	*svc ;
	cchar	*svcdname ;	/* memory-allocated */
	cchar	*svcsym ;	/* memory-allocated */
	cchar	**argv ;
	cchar	**envv ;
	SUBINFO_FL	f ;
	mode_t		om ;
	int		of ;
	int		to ;
} ;


/* forward references */

static int	subinfo_start(SUBINFO *,cchar *,cchar *,cchar *,
			cchar *,cchar *,
			int,mode_t,cchar **,cchar **,int) ;
static int	subinfo_finish(SUBINFO *) ;
static int	subinfo_search(SUBINFO *) ;


/* local variables */

static cchar	*soexts[] = {
	"so",
	"o",
	"",
	NULL
} ;


/* exported subroutines */


int uc_openfint(pr,dn,bn,prn,svc,of,om,av,ev,to)
cchar	pr[] ;
cchar	dn[] ;
cchar	bn[] ;
cchar	prn[] ;
cchar	svc[] ;
int		of ;
mode_t		om ;
cchar	*av[] ;
cchar	*ev[] ;
int		to ;
{
	SUBINFO		si ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		fd = -1 ;

	if ((pr == NULL) || (prn == NULL) || (svc == NULL))
	    return SR_FAULT ;

	if ((pr[0] == '\0') || (prn[0] == '\0') || (svc[0] == '\0'))
	    return SR_INVALID ;

	if ((rs = subinfo_start(&si,pr,dn,bn,prn,svc,of,om,av,ev,to)) >= 0) {

	        rs = subinfo_search(&si) ;
	        fd = rs ;

	    rs1 = subinfo_finish(&si) ;
	    if (rs >= 0) rs = rs1 ;
	    if ((rs < 0) && (fd >= 0)) u_close(fd) ;
	} /* end if (subinfo) */

	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (uc_openfint) */


/* local subroutines */


static int subinfo_start(sip,pr,dn,bn,prn,svc,of,om,argv,envv,to)
SUBINFO		*sip ;
cchar	*pr ;
cchar	*dn ;
cchar	*bn ;
cchar	*prn ;
cchar	*svc ;
int		of ;
mode_t		om ;
cchar	**argv ;
cchar	**envv ;
int		to ;
{
	USTAT		sb ;
	int		rs = SR_OK ;
	int		pl ;
	cchar	*ccp ;
	char		svcdname[MAXPATHLEN+1] ;

	memclear(sip) ;
	sip->pr = pr ;
	sip->dn = dn ;
	sip->bn = bn ;
	sip->prn = prn ;
	sip->svc = svc ;
	sip->argv = argv ;
	sip->envv = (envv != NULL) ? envv : ((cchar **) environ) ;
	sip->of = of ;
	sip->om = om ;
	sip->to = to ;

/* make the directory where the service shared-objects are */

	rs = mkpath2(svcdname,pr,INTDNAME) ;
	pl = rs ;
	if (rs < 0) goto bad0 ;

	rs = u_stat(svcdname,&sb) ;
	if ((rs >= 0) && (! S_ISDIR(sb.st_mode)))
	    rs = SR_NOTDIR ;

/* otherwise the 'stat' will return OK or mostly SR_NOENT */

	if (rs < 0) goto bad0 ;

	rs = uc_libmallocstrw(svcdname,pl,&ccp) ;
	if (rs < 0) goto bad0 ;
	sip->svcdname = ccp ;

	{
	    char	svcsym[MAXNAMELEN+1] ;
	    int		sl ;
	    rs = sncpy2(svcsym,MAXNAMELEN,INTSYMPREFIX,sip->svc) ;
	    sl = rs ;
	    if (rs >= 0) {
	        rs = uc_libmallocstrw(svcsym,sl,&ccp) ;
	        if (rs >= 0) sip->svcsym = ccp ;
	    }
	}

	if (rs < 0) goto bad1 ;

ret0:
bad0:
	return rs ;

/* bad stuff */
bad1:
	if (sip->svcdname != NULL) {
	    uc_libfree(sip->svcdname) ;
	    sip->svcdname = NULL ;
	}

	goto bad0 ;
}
/* end subroutine (subinfo_start) */


static int subinfo_finish(sip)
SUBINFO		*sip ;
{
	int		rs = SR_OK ;
	int		rs1 ;

	if (sip->svcsym != NULL) {
	    rs1 = uc_libfree(sip->svcsym) ;
	    if (rs >= 0) rs = rs1 ;
	    sip->svcsym = NULL ;
	}

	if (sip->svcdname != NULL) {
	    rs1 = uc_libfree(sip->svcdname) ;
	    if (rs >= 0) rs = rs1 ;
	    sip->svcdname = NULL ;
	}

	return rs ;
}
/* end subroutine (subinfo_finish) */


static int subinfo_search(sip)
SUBINFO		*sip ;
{
	struct ustat	sb ;
	IDS		id ;
	const int	dlmode = (RTLD_LAZY | RTLD_LOCAL) ;
	const int	soperm = (R_OK | X_OK) ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		j ;
	int		fd = -1 ;
	int		(*symp)(cchar *,cchar *,cchar *,
			cchar *,
			int,mode_t,
			cchar **,cchar **,int) ;
	void		*sop = NULL ;
	char		sofname[MAXPATHLEN + 1] ;

	symp = NULL ;
	rs = ids_load(&id) ;
	if (rs < 0) goto ret0 ;

	rs1 = SR_NOENT ;
	for (j = 0 ; (soexts[j] != NULL) ; j += 1) {

	    rs1 = mksofname(sofname,sip->svcdname,sip->svc,soexts[j]) ;

	    if (rs1 >= 0) {

	        rs1 = u_stat(sofname,&sb) ;
	        if ((rs1 >= 0) && (! S_ISREG(sb.st_mode)))
	            rs1 = SR_ISDIR ;

	        if (rs1 >= 0)
	            rs1 = sperm(&id,&sb,soperm) ;

	        if (rs1 >= 0) {

	            sop = dlopen(sofname,dlmode) ;

#if	CF_DEBUGS
	            debugprintf("uc_openfint/subinfo_search: "
	                "dlopen() sop=%p >%s<\n",
	                sop,
	                ((sop != NULL) ? "ok" : dlerror() )) ;
#endif

	            if (sop != NULL) {
	                symp = dlsym(sop,sip->svcsym) ;

	                if (symp != NULL) {
	                    cchar	*pr = sip->pr ;
	                    cchar	*dn = sip->dn ;
	                    cchar	*bn = sip->bn ;
			    cchar	*prn = sip->prn ;
	                    cchar	**argv = sip->argv ;
	                    cchar	**envv = sip->envv ;
			    mode_t	om = sip->om ;
	                    int		of = sip->of ;
	                    int		to = sip->to ;
#if	CF_DEBUGS
		{
			int	i ;
	            debugprintf("uc_openfint/subinfo_search: "
			"calling symp=%p\n",symp) ;
			if (argv != NULL) {
			for (i = 0 ; argv[i] != NULL; i += 1)
	            debugprintf("uc_openfint/subinfo_search: "
			"argv[%u]=%s\n",i,argv[i]) ;
			}
		}
#endif /* CF_DEBUGS */
	                    rs = (*symp)(pr,dn,bn,prn,of,om,argv,envv,to) ;
	                    fd = rs ;
#if	CF_DEBUGS
	        debugprintf("uc_openfint/subinfo_search: sym() rs=%d\n",rs) ;
#endif
	                } /* end if */

	                dlclose(sop) ;
	                sop = NULL ;
	            } else
	                rs1 = SR_NOENT ;

	        } /* end if (have file) */

	    } /* end if (created SO filename) */

#if	CF_DEBUGS
	        debugprintf("uc_openfint/subinfo_search: "
		"loop-bottom rs=%d symp=%p\n",rs,symp) ;
#endif

	    if (rs < 0)
	        break ;

	    if (symp != NULL)
	        break ;

	} /* end for (exts) */

	if (rs >= 0) rs = rs1 ;

	if ((rs >= 0) && (symp == NULL)) rs = SR_LIBACC ;

ret1:
	ids_release(&id) ;

ret0:

#if	CF_DEBUGS
	debugprintf("uc_openfint/subinfo_sochecklib: "
	    "ret rs=%d fd=%u\n",rs,fd) ;
#endif

	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (subinfo_search) */


