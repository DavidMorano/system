/* b_mailnew SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* update the machine status for the current machine */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* non-switchable debug print-outs */
#define	CF_DEBUG	0		/* switchable at invocation */
#define	CF_DEBUGMALL	1		/* debug memory-allocations */
#define	CF_LOCSETENT	0		/* |locinfo_setentry()| */
#define	CF_LOCEPRINTF	0		/* |locinfo_eprintf()| */
#define	CF_CONFIGCHECK	0		/* |config_check()| */

/* revision history:

	= 1989-03-01, David A�D� Morano
	This code was originally written.  

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	b_mailnew

	Description:
	This is a built-in command to the KSH shell.  It should
	also be able to be made into a stand-alone program without
	much (if almost any) difficulty, but I have not done that
	yet (we already have a MSU program out there).  Note that
	special care needed to be taken with the child processes
	because we cannot let them ever return normally!  They
	cannot return since they would be returning to a KSH program
	that thinks it is alive (!) and that geneally causes some
	sort of problem or another.  That is just some weird thing
	asking for trouble.  So we have to take care to force child
	processes to exit explicitly.  Child processes are only
	created when run in "daemon" mode.

	Synopsis:
	$ mailnew [-u <user>]

	Notes:

	= Command options:
	Dates available right now:
	- default	hh:mm			(5 bytes)
	- long		CC-Mmm-DD hh:mm		(15 bytes)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */

#if	defined(SFIO) && (SFIO > 0)
#define	CF_SFIO	1
#else
#define	CF_SFIO	0
#endif

#if	(defined(KSHBUILTIN) && (KSHBUILTIN > 0))
#include	<shell.h>
#endif

#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
#include	<climits>
#include	<cstdlib>
#include	<cstddef>
#include	<cstring>
#include	<netdb.h>
#include	<usystem.h>
#include	<getourenv.h>
#include	<tmtime.hh>
#include	<sntmtime.h>
#include	<bits.h>
#include	<keyopt.h>
#include	<paramopt.h>
#include	<field.h>
#include	<vecstr.h>
#include	<vechand.h>
#include	<userinfo.h>
#include	<paramfile.h>
#include	<expcook.h>
#include	<logfile.h>
#include	<storebuf.h>
#include	<mailbox.h>
#include	<mbcache.h>
#include	<hdrdecode.h>
#include	<prmkfname.h>
#include	<strn.h>
#include	<strwcpy.h>
#include	<strx.h>
#include	<char.h>
#include	<exitcodes.h>
#include	<localmisc.h>

#include	"shio.h"
#include	"kshlib.h"
#include	"b_mailnew.h"
#include	"defs.h"
#include	"proglog.h"


/* local defines */

#ifndef	VBUFLEN
#define	VBUFLEN		(2 * MAXPATHLEN)
#endif

#ifndef	EBUFLEN
#define	EBUFLEN		(3 * MAXPATHLEN)
#endif

#ifndef	SUBJBUFLEN
#define	SUBJBUFLEN	(2*COLUMNS)
#endif

#ifndef	NOTEBUFLEN
#define	NOTEBUFLEN	COLUMNS
#endif

#define	DSBUFLEN	15		/* large enough for "CC-MMM-DD hh:mm" */
#define	MAXOVERLEN	22		/* standard date overhead (in bytes) */
#define	MAXFROMLEN	35

#define	DEBUGFNAME	"/tmp/mailnew.deb"

#define	LOCINFO		struct locinfo
#define	LOCINFO_FL	struct locinfo_flags

#define	CONFIG		struct config

#define	MSGENTRY	struct msgentry

#define	OUTINFO		struct outinfo

#define	PO_MAILDIRS	"maildirs"
#define	PO_MAILUSERS	"mailusers"


/* external subroutines */

extern int	snsd(char *,int,cchar *,uint) ;
extern int	snsds(char *,int,cchar *,cchar *) ;
extern int	snwcpycompact(char *,int,cchar *,int) ;
extern int	sncpy1(char *,int,cchar *) ;
extern int	sncpy2(char *,int,cchar *,cchar *) ;
extern int	sncpy3(char *,int,cchar *,cchar *,cchar *) ;
extern int	snwcpywidehdr(char *,int,const wchar_t *,int) ;
extern int	mkpath1w(char *,cchar *,int) ;
extern int	mkpath1(char *,cchar *) ;
extern int	mkpath2(char *,cchar *,cchar *) ;
extern int	mkpath3(char *,cchar *,cchar *,cchar *) ;
extern int	mkfnamesuf1(char *,cchar *,cchar *) ;
extern int	sfdirname(cchar *,int,cchar **) ;
extern int	sfshrink(cchar *,int,cchar **) ;
extern int	sfskipwhite(cchar *,int,cchar **) ;
extern int	wsfnext(cint *,int,cint **) ;
extern int	matostr(cchar **,int,cchar *,int) ;
extern int	matpstr(cchar **,int,cchar *,int) ;
extern int	cfdeci(cchar *,int,int *) ;
extern int	cfdecui(cchar *,int,uint *) ;
extern int	cfdecti(cchar *,int,int *) ;
extern int	cfdecmfi(cchar *,int,int *) ;
extern int	optbool(cchar *,int) ;
extern int	optvalue(cchar *,int) ;
extern int	vecstr_envadd(vecstr *,cchar *,cchar *,int) ;
extern int	vecstr_envset(vecstr *,cchar *,cchar *,int) ;
extern int	permsched(cchar **,vecstr *,char *,int,cchar *,int) ;
extern int	mklogidpre(char *,int,cchar *,int) ;
extern int	mklogidsub(char *,int,cchar *,int) ;
extern int	mkaddrdisp(char *,int,cchar *,int) ;
extern int	bufprintf(char *,int,cchar *,...) ;
extern int	tolc(int) ;
extern int	isdigitlatin(int) ;
extern int	isFailOpen(int) ;
extern int	isNotPresent(int) ;

extern int	printhelp(void *,cchar *,cchar *,cchar *) ;
extern int	proginfo_setpiv(PROGINFO *,cchar *,const struct pivars *) ;

extern int	proguserlist_begin(PROGINFO *) ;
extern int	proguserlist_end(PROGINFO *) ;

#if	CF_DEBUGS || CF_DEBUG
extern int	debugopen(cchar *) ;
extern int	debugprintf(cchar *,...) ;
extern int	debugclose() ;
extern int	strlinelen(cchar *,int,int) ;
#endif

extern char	*strdcpycompact(char *,int,cchar *,int) ;
extern char	*timestr_log(time_t,char *) ;
extern char	*timestr_logz(time_t,char *) ;
extern char	*timestr_elapsed(time_t,char *) ;


/* external variables */

extern char	**environ ;		/* definition required by AT&T AST */


/* local structures */

struct locinfo_flags {
	uint		stores:1 ;
	uint		md:1 ;
	uint		mailusers:1 ;
	uint		msgs:1 ;
	uint		sort:1 ;
	uint		sortrev:1 ;
	uint		nshow:1 ;
	uint		linelen:1 ;
	uint		datelong:1 ;
} ;

struct locinfo {
	PROGINFO	*pip ;
	LOCINFO_FL	have, f, final, changed ;
	LOCINFO_FL	open ;
	vecstr		stores ;
	vecstr		mailusers ;
	VECHAND		msgs ;
	int		nshow ;
	int		linelen ;
} ;

struct config {
	PROGINFO	*pip ;
	PARAMOPT	*app ;
	PARAMFILE	p ;
	EXPCOOK		cooks ;
	uint		f_p:1 ;
	uint		f_cooks:1 ;
} ;

struct msgentry {
	cchar	*user ;
	cchar	*date ;		/* just "HH:MM" */
	cchar	*from ;
	cchar	*subj ;
	cchar	*a ;		/* memory-allocation */
	time_t		mdate ;
	int		lines ;
} ;

struct outinfo {
	MSGENTRY	*mep ;
	char		*fbuf ;
	char		*sbuf ;
	int		ll ;
	int		dl, ul ;
	int		flen, fl ;
	int		slen, sl ;
} ;


/* forward references */

static int	mainsub(int,cchar **,cchar **,void *) ;

static int	usage(PROGINFO *) ;

static int	procopts(PROGINFO *,KEYOPT *,PARAMOPT *) ;
static int	procargs(PROGINFO *,ARGINFO *,BITS *,PARAMOPT *,cchar *) ;
static int	procnames(PROGINFO *,PARAMOPT *,cchar *,cchar *,int) ;
static int	procmaildirs(PROGINFO *,PARAMOPT *) ;
static int	procmaildir(PROGINFO *,PARAMOPT *,cchar *,int) ;

static int	procuserinfo_begin(PROGINFO *,USERINFO *) ;
static int	procuserinfo_end(PROGINFO *) ;
static int	procuserinfo_logid(PROGINFO *) ;

static int	procourconf_begin(PROGINFO *,PARAMOPT *,cchar *) ;
static int	procourconf_end(PROGINFO *) ;

static int	procmailusers(PROGINFO *,PARAMOPT *) ;
static int	procmailusers_env(PROGINFO *,cchar *) ;
static int	procmailusers_arg(PROGINFO *,PARAMOPT *) ;
static int	procmailusers_def(PROGINFO *) ;
static int	procmailusers_add(PROGINFO *,cchar *,int) ;

static int	process(PROGINFO *pip,PARAMOPT *,cchar *) ;
static int	procmailboxes(PROGINFO *,PARAMOPT *) ;
static int	procmailbox(PROGINFO *,cchar *,cchar *) ;
static int	procmailmsg(PROGINFO *,cchar *,MBCACHE *,int) ;
static int	procout(PROGINFO *,void *) ;
static int	procouter(PROGINFO *,void *,HDRDECODE *,MSGENTRY *) ;

static int	sfnormfrom(cchar *,int) ;

static int	locinfo_start(LOCINFO *,PROGINFO *) ;
static int	locinfo_finish(LOCINFO *) ;
static int	locinfo_nshow(LOCINFO *,cchar *) ;
static int	locinfo_logprintf(LOCINFO *,cchar *,...) ;
static int	locinfo_optdate(LOCINFO *,cchar *,int) ;

#if	CF_LOCEPRINTF
static int	locinfo_eprintf(LOCINFO *,cchar *,...) ;
#endif /* CF_LOCEPRINTF */

#if	CF_LOCSETENT
static int	locinfo_setentry(LOCINFO *,cchar **,cchar *,int) ;
#endif /* CF_LOCSETENT */

static int	config_start(CONFIG *,PROGINFO *,PARAMOPT *,cchar *) ;
static int	config_findfile(CONFIG *,char *,cchar *) ;
static int	config_cookbegin(CONFIG *) ;
static int	config_cookend(CONFIG *) ;
static int	config_read(CONFIG *) ;
static int	config_finish(CONFIG *) ;
static int	config_setlfname(CONFIG *,cchar *,int) ;

#if	CF_CONFIGCHECK
static int	config_check(CONFIG *) ;
#endif /* CF_CONFIGCHECK */

static int	msgentry_start(MSGENTRY *,cchar *,time_t,cchar *,
			cchar **,cint *) ;
static int	msgentry_finish(MSGENTRY *) ;
static int	msgentry_from(MSGENTRY *,cchar **) ;
static int	msgentry_subj(MSGENTRY *,cchar **) ;

static int	outinfo_start(OUTINFO *,MSGENTRY *,int,int) ;
static int	outinfo_finish(OUTINFO *) ;
static int	outinfo_trans(OUTINFO *,HDRDECODE *) ;
static int	outinfo_cvt(OUTINFO *,HDRDECODE *,char *,int,cchar *,int,int) ;
static int	outinfo_calc(OUTINFO *) ;
static int	outinfo_total(OUTINFO *) ;
static int	outinfo_print(OUTINFO *,PROGINFO *,void *,int) ;

static int	mkmsfname(char *,cchar *,int,cchar *) ;

static int	vcmpfor(cvoid *,cvoid *) ;
static int	vcmprev(cvoid *,cvoid *) ;


/* local variables */

enum argopts {
	argopt_version,
	argopt_verbose,
	argopt_root,
	argopt_help,
	argopt_logfile,
	argopt_md,
	argopt_sn,
	argopt_af,
	argopt_ef,
	argopt_of,
	argopt_cf,
	argopt_lf,
	argopt_overlast
} ;

constexpr cpcchar	argopts[] = {
	"VERSION",
	"VERBOSE",
	"ROOT",
	"HELP",
	"LOGFILE",
	"md",
	"sn",
	"af",
	"ef",
	"of",
	"cf",
	"lf",
	nullptr
} ;

constexpr PIVARS	initvars = {
	VARPROGRAMROOT1,
	VARPROGRAMROOT2,
	VARPROGRAMROOT3,
	PROGRAMROOT,
	VARPRNAME
} ;

constexpr MAPEX		mapexs[] = {
	{ SR_NOENT, EX_NOUSER },
	{ SR_AGAIN, EX_TEMPFAIL },
	{ SR_DEADLK, EX_TEMPFAIL },
	{ SR_NOLCK, EX_TEMPFAIL },
	{ SR_TXTBSY, EX_TEMPFAIL },
	{ SR_ACCESS, EX_NOPERM },
	{ SR_REMOTE, EX_PROTOCOL },
	{ SR_NOSPC, EX_TEMPFAIL },
	{ SR_INTR, EX_INTR },
	{ SR_EXIT, EX_TERM },
	{ 0, 0 }
} ;

enum akonames {
	akoname_md,
	akoname_sort,
	akoname_nshow,
	akoname_date,
	akoname_overlast
} ;

constexpr cpcchar	akonames[] = {
	"md",
	"sort",
	"nshow",
	"date",
	nullptr
} ;

constexpr cpcchar	sched1[] = {
	"%p/%e/%n/%n.%f",
	"%p/%e/%n/%f",
	"%p/%e/%n.%f",
	"%p/%n.%f",
	nullptr
} ;

enum cparams {
	cparam_maildir,
	cparam_logsize,
	cparam_logfile,
	cparam_overlast
} ;

constexpr cpcchar	cparams[] = {
	"maildir",
	"logsize",
	"logfile",
	nullptr
} ;

constexpr cpcchar	varmaildirs[] = {
	VARMAILDNAMESP,
	VARMAILDNAMES,
	VARMAILDNAME,
	nullptr
} ;

constexpr cpcchar	varmailusers[] = {
	VARMAILUSERSP,
	VARMAILUSERS,
	nullptr
} ;

constexpr char		aterms[] = {
	0x00, 0x2E, 0x00, 0x00,
	0x09, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ;

enum datetypes {
	datetype_long,
	datetype_overlast
} ;

constexpr cpcchar	datetypes[] = {
	"long",
	nullptr
} ;


/* exported variables */


/* exported subroutines */

int b_mailnew(int argc,mainv argv,void *contextp) noex {
	int		rs ;
	int		rs1 ;
	int		ex = EX_OK ;

	if ((rs = lib_kshbegin(contextp,nullptr)) >= 0) {
	    cchar	**envv = (cchar **) environ ;
	    ex = mainsub(argc,argv,envv,contextp) ;
	    rs1 = lib_kshend() ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ksh) */

	if ((rs < 0) && (ex == EX_OK)) ex = EX_DATAERR ;

	return ex ;
}
/* end subroutine (b_mailnew) */

int p_mailnew(int argc,mainv argv,mainv envv,void *contextp) noex {
	return mainsub(argc,argv,envv,contextp) ;
}
/* end subroutine (p_mailnew) */


/* local subroutines */

/* ARGSUSED */
static int mainsub(int argc,mainv argv,mainv envv,void *contextp) noex {
	PROGINFO	pi, *pip = &pi ;
	LOCINFO		li, *lip = &li ;
	ARGINFO		ainfo ;
	BITS		pargs ;
	KEYOPT		akopts ;
	PARAMOPT	aparams ;
	SHIO		errfile ;

#if	(CF_DEBUGS || CF_DEBUG) && CF_DEBUGMALL
	uint		mo_start = 0 ;
#endif

	int		argr, argl, aol, akl, avl, kwi ;
	int		ai, ai_max, ai_pos ;
	int		rs, rs1 ;
	int		cl ;
	int		ex = EX_INFO ;
	int		f_optminus, f_optplus, f_optequal ;
	int		f_version = false ;
	int		f_usage = false ;
	int		f_help = false ;

	cchar	*argp, *aop, *akp, *avp ;
	cchar	*argval = nullptr ;
	cchar	*pr = nullptr ;
	cchar	*sn = nullptr ;
	cchar	*cfname = nullptr ;
	cchar	*afname = nullptr ;
	cchar	*efname = nullptr ;
	cchar	*ofname = nullptr ;
	cchar	*cp ;


#if	CF_DEBUGS || CF_DEBUG
	if ((cp = getourenv(envv,VARDEBUGFNAME)) != nullptr) {
	    rs = debugopen(cp) ;
	    debugprintf("b_mailnew: starting DFD=%d\n",rs) ;
	}
#endif /* CF_DEBUGS */

#if	(CF_DEBUGS || CF_DEBUG) && CF_DEBUGMALL
	uc_mallset(1) ;
	uc_mallout(&mo_start) ;
#endif

	rs = proginfo_start(pip,envv,argv[0],VERSION) ;
	if (rs < 0) {
	    ex = EX_OSERR ;
	    goto badprogstart ;
	}

	if ((cp = getourenv(envv,VARBANNER)) == nullptr) cp = BANNER ;
	rs = proginfo_setbanner(pip,cp) ;

/* initialize */

	pip->verboselevel = 1 ;
	pip->daytime = time(nullptr) ;

	pip->fl.logprog = true ;

	pip->lip = lip ;
	if (rs >= 0) rs = locinfo_start(lip,pip) ;
	if (rs < 0) {
	    ex = EX_OSERR ;
	    goto badlocstart ;
	}

/* start parsing the arguments */

	if (rs >= 0) rs = bits_start(&pargs,0) ;
	if (rs < 0) goto badpargs ;

	rs = keyopt_start(&akopts) ;
	pip->open.akopts = (rs >= 0) ;

	if (rs >= 0) {
	    rs = paramopt_start(&aparams) ;
	    pip->open.aparams = (rs >= 0) ;
	}

	ai_max = 0 ;
	ai_pos = 0 ;
	argr = argc ;
	for (ai = 0 ; (ai < argc) && (argv[ai] != nullptr) ; ai += 1) {
	    if (rs < 0) break ;
	    argr -= 1 ;
	    if (ai == 0) continue ;

	    argp = argv[ai] ;
	    argl = strlen(argp) ;

	    f_optminus = (*argp == '-') ;
	    f_optplus = (*argp == '+') ;
	    if ((argl > 1) && (f_optminus || f_optplus)) {
	        cint	ach = MKCHAR(argp[1]) ;

	        if (isdigitlatin(ach)) {

	            argval = (argp+1) ;

	        } else if (ach == '-') {

	            ai_pos = ai ;
	            break ;

	        } else {

	            aop = argp + 1 ;
	            akp = aop ;
	            aol = argl - 1 ;
	            f_optequal = false ;
	            if ((avp = strchr(aop,'=')) != nullptr) {
	                f_optequal = true ;
	                akl = avp - aop ;
	                avp += 1 ;
	                avl = aop + argl - 1 - avp ;
	                aol = akl ;
	            } else {
	                avp = nullptr ;
	                avl = 0 ;
	                akl = aol ;
	            }

	            if ((kwi = matostr(argopts,2,akp,akl)) >= 0) {

	                switch (kwi) {

/* version */
	                case argopt_version:
	                    f_version = true ;
	                    if (f_optequal)
	                        rs = SR_INVALID ;
	                    break ;

/* verbose mode */
	                case argopt_verbose:
	                    pip->verboselevel = 2 ;
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl) {
	                            rs = optvalue(avp,avl) ;
	                            pip->verboselevel = rs ;
	                        }
	                    }
	                    break ;

/* program root */
	                case argopt_root:
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl)
	                            pr = avp ;
	                    } else {
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                pr = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                    }
	                    break ;

	                case argopt_logfile:
	                    pip->have.logprog = true ;
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl) {
	                            pip->lfname = avp ;
	                            pip->final.logprog = true ;
	                        }
	                    }
	                    break ;

/* mail directory */
	                case argopt_md:
	                    cp = nullptr ;
	                    cl = -1 ;
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl) {
	                            cp = avp ;
	                            cl = avl ;
	                        }
	                    } else {
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl) {
	                                cp = argp ;
	                                cl = argl ;
	                            }
	                        } else
	                            rs = SR_INVALID ;
	                    }
	                    if ((rs >= 0) && (cp != nullptr)) {
				PARAMOPT	*pop = &aparams ;
	                        cchar		*po = PO_MAILDIRS ;
	                        rs = paramopt_loads(pop,po,cp,cl) ;
	                    }
	                    break ;

	                case argopt_help:
	                    f_help = true ;
	                    break ;

/* program search-name */
	                case argopt_sn:
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl)
	                            sn = avp ;
	                    } else {
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                sn = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                    }
	                    break ;

/* argument file */
	                case argopt_af:
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl)
	                            afname = avp ;
	                    } else {
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                afname = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                    }
	                    break ;

/* error file name */
	                case argopt_ef:
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl)
	                            efname = avp ;
	                    } else {
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                efname = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                    }
	                    break ;

/* output file-name */
	                case argopt_of:
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl)
	                            ofname = avp ;
	                    } else {
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                ofname = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                    }
	                    break ;

/* configuration file-name */
	                case argopt_cf:
	                    cp = nullptr ;
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl)
	                            cp = avp ;
	                    } else {
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                cp = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                    }
	                    if ((rs >= 0) && (cp != nullptr)) {
	                        pip->have.cfname = true ;
	                        pip->final.cfname = true ;
	                        cfname = cp ;
	                    }
	                    break ;

/* log file name */
	                case argopt_lf:
	                    cp = nullptr ;
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl)
	                            cp = avp ;
	                    } else {
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                cp = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                    }
	                    if ((rs >= 0) && (cp != nullptr)) {
	                        pip->have.logprog = true ;
	                        pip->lfname = cp ;
	                    }
	                    break ;

/* handle all keyword defaults */
	                default:
	                    rs = SR_INVALID ;
	                    break ;

	                } /* end switch */

	            } else {

	                while (akl--) {
	                    cint	kc = MKCHAR(*akp) ;

	                    switch (kc) {

	                    case 'C':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl) {
	                                pip->have.cfname = true ;
	                                pip->final.cfname = true ;
	                                cfname = argp ;
	                            }
	                        } else
	                            rs = SR_INVALID ;
	                        break ;

/* debug */
	                    case 'D':
	                        pip->debuglevel = 1 ;
	                        if (f_optequal) {
	                            f_optequal = false ;
	                            if (avl) {
	                                rs = optvalue(avp,avl) ;
	                                pip->debuglevel = rs ;
	                            }
	                        }
	                        break ;

	                    case 'Q':
	                        pip->fl.quiet = true ;
	                        break ;

/* version */
	                    case 'V':
	                        f_version = true ;
	                        break ;

/* long dates */
	                    case 'l':
	                        lip->fl.datelong = true ;
	                        if (f_optequal) {
	                            f_optequal = false ;
	                            if (avl) {
	                                rs = optbool(avp,avl) ;
	                        	lip->fl.datelong = (rs > 0) ;
	                            }
	                        }
	                        break ;

/* options */
	                    case 'o':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl) {
					KEYOPT	*kop = &akopts ;
	                                rs = keyopt_loads(kop,argp,argl) ;
				    }
	                        } else
	                            rs = SR_INVALID ;
	                        break ;

/* quiet mode */
	                    case 'q':
	                        pip->verboselevel = 0 ;
	                        break ;

/* alternate users */
	                    case 'u':
	                        cp = nullptr ;
	                        cl = 0 ;
	                        if (f_optequal) {
	                            f_optequal = false ;
	                            if (avl) {
	                                cp = avp ;
	                                cl = avl ;
	                            }
	                        } else {
	                            if (argr > 0) {
	                                argp = argv[++ai] ;
	                                argr -= 1 ;
	                                argl = strlen(argp) ;
	                                if (argl) {
	                                    cp = argp ;
	                                    cl = argl ;
	                                }
	                            } else
	                                rs = SR_INVALID ;
	                        }
	                        if ((rs >= 0) && (cp != nullptr)) {
				    PARAMOPT	*pop = &aparams ;
	                            cchar	*po = PO_MAILUSERS ;
	                            rs = paramopt_loads(pop,po,cp,cl) ;
	                        }
	                        break ;

/* verbose mode */
	                    case 'v':
	                        pip->verboselevel = 2 ;
	                        if (f_optequal) {
	                            f_optequal = false ;
	                            if (avl) {
	                                rs = optvalue(avp,avl) ;
	                                pip->verboselevel = rs ;
	                            }
	                        }
	                        break ;

/* width (columns) */
	                    case 'w':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl) {
	                                lip->have.linelen = true ;
	                                lip->final.linelen = true ;
	                                rs = optvalue(argp,argl) ;
	                                lip->linelen = rs ;
	                            }
	                        } else
	                            rs = SR_INVALID ;
	                        break ;

	                    case '?':
	                        f_usage = true ;
	                        break ;

	                    default:
	                        rs = SR_INVALID ;
	                        break ;

	                    } /* end switch */
	                    akp += 1 ;

	                    if (rs < 0) break ;
	                } /* end while */

	            } /* end if (individual option key letters) */

	        } /* end if (digits as argument or not) */

	    } else {

	        rs = bits_set(&pargs,ai) ;
	        ai_max = ai ;

	    } /* end if (key letter/word or positional) */

	    ai_pos = ai ;

	} /* end while (all command line argument processing) */

	if (efname == nullptr) efname = getourenv(envv,VAREFNAME) ;
	if (efname == nullptr) efname = STDFNERR ;
	if ((rs1 = shio_open(&errfile,efname,"wca",0666)) >= 0) {
	    pip->efp = &errfile ;
	    pip->open.errfile = true ;
	    shio_control(&errfile,SHIO_CSETBUFLINE,true) ;
	} else if (! isFailOpen(rs1)) {
	    if (rs >= 0) rs = rs1 ;
	}

	if (rs < 0)
	    goto badarg ;

#if	CF_DEBUGS
	debugprintf("main: finished parsing arguments\n") ;
#endif

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("main: debuglevel=%u\n",pip->debuglevel) ;
#endif

	if (f_version) {
	    shio_printf(pip->efp,"%s: version %s\n",pip->progname,VERSION) ;
	}

/* get the program root */

	if (rs >= 0) {
	    if ((rs = proginfo_setpiv(pip,pr,&initvars)) >= 0) {
	        rs = proginfo_setsearchname(pip,VARSEARCHNAME,sn) ;
	    }
	}

	if (rs < 0) {
	    ex = EX_OSERR ;
	    goto retearly ;
	}

	if (pip->debuglevel > 0) {
	    shio_printf(pip->efp, "%s: pr=%s\n",pip->progname,pip->pr) ;
	    shio_printf(pip->efp, "%s: sn=%s\n",pip->progname,pip->searchname) ;
	}

	if (f_usage)
	    usage(pip) ;

/* help file */

	if (f_help) {
#if	CF_SFIO
	    printhelp(sfstdout,pip->pr,pip->searchname,HELPFNAME) ;
#else
	    printhelp(nullptr,pip->pr,pip->searchname,HELPFNAME) ;
#endif
	} /* end if */

	if (f_help || f_usage || f_version)
	    goto retearly ;


	ex = EX_OK ;

/* cotinue intialization */

	if (argval == nullptr) argval = getourenv(pip->envv,VARNSHOW) ;

	if ((rs = locinfo_nshow(lip,argval)) >= 0) {
	    rs = procopts(pip,&akopts,&aparams) ;
	}

	if ((rs >= 0) && (lip->linelen == 0)) {
	    cp = nullptr ;
	    if (cp == nullptr) cp = getourenv(pip->envv,VARLINELEN) ;
	    if (cp == nullptr) cp = getourenv(pip->envv,VARCOLUMNS) ;
	    if (cp != nullptr) {
	        rs = optvalue(cp,-1) ;
	        lip->linelen = rs ;
	    }
	}

	if (lip->linelen == 0) lip->linelen = COLUMNS ;

	if (afname == nullptr) afname = getourenv(pip->envv,VARAFNAME) ;

	if (cfname == nullptr) cfname = getourenv(pip->envv,VARCFNAME) ;

	if (pip->lfname == nullptr) pip->lfname = getourenv(pip->envv,VARLFNAME) ;

	if (cfname != nullptr) pip->final.cfname = true ;

/* get some mail-users */

	memset(&ainfo,0,sizeof(ARGINFO)) ;
	ainfo.argc = argc ;
	ainfo.ai = ai ;
	ainfo.argv = argv ;
	ainfo.ai_max = ai_max ;
	ainfo.ai_pos = ai_pos ;

	if (rs >= 0) {
	    ARGINFO	*aip = &ainfo ;
	    if ((rs = procargs(pip,aip,&pargs,&aparams,afname)) >= 0) {
	        USERINFO	u ;
	        if ((rs = userinfo_start(&u,nullptr)) >= 0) {
	            if ((rs = procuserinfo_begin(pip,&u)) >= 0) {
			PARAMOPT	*pop = &aparams ;
		        cchar		*cfn = cfname ;
	                if (cfname != nullptr) {
	                    if (pip->euid != pip->uid) u_seteuid(pip->uid) ;
	                    if (pip->egid != pip->gid) u_setegid(pip->gid) ;
	                }
	                if ((rs = procourconf_begin(pip,pop,cfn)) >= 0) {
	                    if ((rs = proglog_begin(pip,&u)) >= 0) {
	                        if ((rs = proguserlist_begin(pip)) >= 0) {

	                            if (rs >= 0)
	                                rs = procmaildirs(pip,pop) ;

	                            if (rs >= 0)
	                                rs = procmailusers(pip,pop) ;

	                            if (pip->debuglevel > 0) {
	                                cchar	*pn = pip->progname ;
	                                cchar	*s = "off" ;
				        cchar	*fmt ;
	                                if (lip->fl.sort) {
					    int f_rev = lip->fl.sortrev ;
	                                    s = (f_rev) ? "rev" : "for" ;
	                                }
				        fmt = "%s: sort=%s\n" ;
	                                shio_printf(pip->efp,fmt,pn,s) ;
	                            }

	                            if (rs >= 0) {
	                                rs = process(pip,pop,ofname) ;
	                            }

	                            rs1 = proguserlist_end(pip) ;
				    if (rs >= 0) rs = rs1 ;
	                        } /* end if (proguserlist) */
	                        rs1 = proglog_end(pip) ;
			        if (rs >= 0) rs = rs1 ;
	                    } /* end if (proglogfile) */
	                    rs1 = procourconf_end(pip) ;
			    if (rs >= 0) rs = rs1 ;
	                } /* end if (procourconf) */
	                rs1 = procuserinfo_end(pip) ;
		        if (rs >= 0) rs = rs1 ;
	            } /* end if (procuserinfo) */
	            rs1 = userinfo_finish(&u) ;
		    if (rs >= 0) rs = rs1 ;
	        } else {
		    cchar	*pn = pip->progname ;
		    cchar	*fmt = "%s: userinfo failure (%d)\n" ;
	            ex = EX_NOUSER ;
	            shio_printf(pip->efp,fmt,pn,rs) ;
	        }
	    } /* end if (procargs) */
	} else if (ex == EX_OK) {
	    cchar	*pn = pip->progname ;
	    cchar	*fmt = "%s: invalid argument or configuration (%d)\n" ;
	    ex = EX_USAGE ;
	    shio_printf(pip->efp,fmt,pn,rs) ;
	    usage(pip) ;
	} /* end if (ok) */

/* done */
	if ((rs < 0) && (ex == EX_OK)) {
	    switch (rs) {
	    case SR_INVALID:
	        ex = EX_USAGE ;
	        if (! pip->fl.quiet) {
	            shio_printf(pip->efp,"%s: invalid query (%d)\n",
	                pip->progname,rs) ;
	        }
	        break ;
	    case SR_NOENT:
	        ex = EX_CANTCREAT ;
	        break ;
	    case SR_AGAIN:
	        ex = EX_TEMPFAIL ;
	        break ;
	    default:
	        ex = mapex(mapexs,rs) ;
	        break ;
	    } /* end switch */
	} else if ((rs >= 0) && (ex == EX_OK)) {
	    if ((rs = lib_sigterm()) < 0) {
	        ex = EX_TERM ;
	    } else if ((rs = lib_sigintr()) < 0) {
	        ex = EX_INTR ;
	    }
	} /* end if */

/* early return thing */
retearly:
	if ((pip->debuglevel > 0) && (pip->efp != nullptr)) {
	    shio_printf(pip->efp,"%s: exiting ex=%u (%d)\n",
	        pip->progname,ex,rs) ;
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("b_mailnew: exiting ex=%u (%d)\n",ex,rs) ;
#endif

	if (pip->efp != nullptr) {
	    pip->open.errfile = false ;
	    shio_close(pip->efp) ;
	    pip->efp = nullptr ;
	}

	if (pip->open.aparams) {
	    pip->open.aparams = false ;
	    paramopt_finish(&aparams) ;
	}

	if (pip->open.akopts) {
	    pip->open.akopts = false ;
	    keyopt_finish(&akopts) ;
	}

	bits_finish(&pargs) ;

badpargs:
	locinfo_finish(lip) ;

badlocstart:
	proginfo_finish(pip) ;

badprogstart:

#if	(CF_DEBUGS || CF_DEBUG) && CF_DEBUGMALL
	{
	    uint	mo ;
	    uc_mallout(&mo) ;
	    debugprintf("main: final mallout=%u\n",(mo-mo_start)) ;
	    uc_mallset(0) ;
	}
#endif /* CF_DEBUGMALL */

#if	(CF_DEBUGS || CF_DEBUG)
	debugclose() ;
#endif

	return ex ;

/* the bad things */
badarg:
	ex = EX_USAGE ;
	shio_printf(pip->efp,"%s: invalid argument specified (%d)\n",
	    pip->progname,rs) ;
	usage(pip) ;
	goto retearly ;

}
/* end subroutine (mainsub) */


static int usage(PROGINFO *pip)
{
	int		rs = SR_OK ;
	int		wlen = 0 ;
	cchar	*pn = pip->progname ;
	cchar	*fmt ;

	fmt = "%s: USAGE> %s [<user(s)>] [-o <opt(s)>] [-md <maildir(s)>]\n" ;
	if (rs >= 0) rs = shio_printf(pip->efp,fmt,pn,pn) ;
	wlen += rs ;

	fmt = "%s:  [-Q] [-D] [-v[=<n>]] [-HELP] [-V]\n" ;
	if (rs >= 0) rs = shio_printf(pip->efp,fmt,pn) ;
	wlen += rs ;

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (usage) */


/* process the program ako-options */
static int procopts(PROGINFO *pip,KEYOPT *kop,PARAMOPT *app)
{
	LOCINFO		*lip = pip->lip ;
	int		rs = SR_OK ;
	int		c = 0 ;
	cchar	*cp ;

	if ((cp = getourenv(pip->envv,VAROPTS)) != nullptr) {
	    rs = keyopt_loads(kop,cp,-1) ;
	}

	if (rs >= 0) {
	    KEYOPT_CUR	kcur ;
	    if ((rs = keyopt_curbegin(kop,&kcur)) >= 0) {
	        int	oi ;
	        int	kl, vl ;
	        cchar	*kp, *vp ;

	        while ((kl = keyopt_enumkeys(kop,&kcur,&kp)) >= 0) {

	            if ((oi = matostr(akonames,2,kp,kl)) >= 0) {

	                vl = keyopt_fetch(kop,kp,nullptr,&vp) ;

	                switch (oi) {
	                case akoname_md:
	                    if (vl > 0) {
	                        cchar	*po = PO_MAILDIRS ;
	                        lip->have.md = true ;
	                        rs = paramopt_loads(app,po,vp,vl) ;
	                    }
	                    break ;
	                case akoname_sort:
	                    if (! lip->final.sort) {
	                        lip->final.sort = true ;
	                        lip->have.sort = true ;
	                        lip->fl.sort = true ;
	                        if (vl > 0) {
	                            int	ch = tolc(*vp & 0xff) ;
	                            switch (ch) {
	                            case '0':
	                            case 'n':
	                                lip->fl.sort = false ;
	                                break ;
	                            case 'r':
	                                lip->fl.sortrev = true ;
	                                break ;
	                            } /* end switch */
	                        }
	                    }
	                    break ;
	                case akoname_nshow:
	                    if (! lip->final.nshow) {
	                        if (vl) {
	                            lip->final.nshow = true ;
	                            lip->have.nshow = true ;
	                            rs = optvalue(vp,vl) ;
	                            lip->nshow = rs ;
	                        }
	                    }
	                    break ;
	                case akoname_date:
	                    if (! lip->final.datelong) {
	                        lip->final.datelong = true ;
	                        lip->have.datelong = true ;
	                        lip->fl.datelong = true ;
	                        if (vl) {
				    rs = locinfo_optdate(lip,vp,vl) ;
	                        }
	                    }
	                    break ;
	                } /* end switch */

	                c += 1 ;
	            } else
	                rs = SR_INVALID ;

	            if (rs < 0) break ;
	        } /* end while (looping through key options) */

	        keyopt_curend(kop,&kcur) ;
	    } /* end if (keyopt-cur) */
	} /* end if (ok) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (procopts) */


static int procuserinfo_begin(PROGINFO *pip,USERINFO *uip)
{
	int		rs = SR_OK ;

	pip->nodename = uip->nodename ;
	pip->domainname = uip->domainname ;
	pip->username = uip->username ;
	pip->gecosname = uip->gecosname ;
	pip->realname = uip->realname ;
	pip->name = uip->name ;
	pip->fullname = uip->fullname ;
	pip->mailname = uip->mailname ;
	pip->org = uip->organization ;
	pip->logid = uip->logid ;
	pip->pid = uip->pid ;
	pip->uid = uip->uid ;
	pip->euid = uip->euid ;
	pip->gid = uip->gid ;
	pip->egid = uip->egid ;

	if (rs >= 0) {
	    cint	hlen = MAXHOSTNAMELEN ;
	    char	hbuf[MAXHOSTNAMELEN+1] ;
	    cchar	*nn = pip->nodename ;
	    cchar	*dn = pip->domainname ;
	    if ((rs = snsds(hbuf,hlen,nn,dn)) >= 0) {
	        cchar	**vpp = &pip->hostname ;
	        rs = proginfo_setentry(pip,vpp,hbuf,rs) ;
	    }
	}

	if (rs >= 0) {
	    rs = procuserinfo_logid(pip) ;
	} /* end if (ok) */

	return rs ;
}
/* end subroutine (procuserinfo_begin) */


static int procuserinfo_end(PROGINFO *pip)
{
	int		rs = SR_OK ;

	if (pip == nullptr) return SR_FAULT ;

	return rs ;
}
/* end subroutine (procuserinfo_end) */


static int procuserinfo_logid(PROGINFO *pip)
{
	int		rs ;
	if ((rs = lib_runmode()) >= 0) {
	    if (rs & KSHLIB_RMKSH) {
	        if ((rs = lib_serial()) >= 0) {
	            cint	s = rs ;
	            cint	plen = LOGIDLEN ;
	            cint	pv = pip->pid ;
	            cchar	*nn = pip->nodename ;
	            char	pbuf[LOGIDLEN+1] ;
	            if ((rs = mklogidpre(pbuf,plen,nn,pv)) >= 0) {
	                cint	slen = LOGIDLEN ;
	                char		sbuf[LOGIDLEN+1] ;
	                if ((rs = mklogidsub(sbuf,slen,pbuf,s)) >= 0) {
	                    cchar	**vpp = &pip->logid ;
	                    rs = proginfo_setentry(pip,vpp,sbuf,rs) ;
	                }
	            }
	        } /* end if (lib_serial) */
	    } /* end if (runmode-KSH) */
	} /* end if (lib_runmode) */
	return rs ;
}
/* end subroutine (procuserinfo_logid) */


static int procourconf_begin(PROGINFO *pip,PARAMOPT *app,cchar cfname[])
{
	cint	csize = sizeof(CONFIG) ;
	int		rs = SR_OK ;
	void		*p ;

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("b_mailnew/procourconf_begin: ent\n") ;
#endif

	if ((rs = uc_malloc(csize,&p)) >= 0) {
	    CONFIG	*csp = p ;
	    pip->config = csp ;
	    if ((rs = config_start(csp,pip,app,cfname)) >= 0) {
	        if ((rs = config_read(csp)) >= 0) {
	            rs = 1 ;
	        }
	        if (rs < 0)
	            config_finish(csp) ;
	    } /* end if (config) */
	    if (rs < 0) {
	        uc_free(p) ;
	        pip->config = nullptr ;
	    }
	} /* end if (memory-allocation) */

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("b_mailnew/procourconf_begin: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (procourconf_begin) */


static int procourconf_end(PROGINFO *pip)
{
	int		rs = SR_OK ;
	int		rs1 ;

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("b_mailnew/procourconf_end: config=%u\n",
	        (pip->config != nullptr)) ;
#endif

	if (pip->config != nullptr) {
	    CONFIG	*csp = pip->config ;
	    rs1 = config_finish(csp) ;
	    if (rs >= 0) rs = rs1 ;
	    rs1 = uc_free(pip->config) ;
	    if (rs >= 0) rs = rs1 ;
	    pip->config = nullptr ;
	}

	return rs ;
}
/* end subroutine (procourconf_end) */


static int procargs(PROGINFO *pip,ARGINFO *aip,BITS *bop,PARAMOPT *app,
		cchar *afn)
{
	int		rs = SR_OK ;
	int		rs1 ;
	int		cl ;
	int		pan = 0 ;
	cchar	*po = PO_MAILUSERS ;
	cchar	*cp ;

	if (rs >= 0) {
	    int	ai ;
	    int	f ;
	    for (ai = 1 ; (rs >= 0) && (ai < aip->argc) ; ai += 1) {

	        f = (ai <= aip->ai_max) && (bits_test(bop,ai) > 0) ;
	        f = f || ((ai > aip->ai_pos) && (aip->argv[ai] != nullptr)) ;
	        if (f) {
	            cp = aip->argv[ai] ;
	            if (cp[0] != '\0') {
	                pan += 1 ;
	                rs = paramopt_loads(app,po,cp,-1) ;
	            }
	        }

	    } /* end for */
	} /* end if (ok) */

	if ((rs >= 0) && (afn != nullptr) && (afn[0] != '\0')) {
	    SHIO	afile, *afp = &afile ;

	    if (strcmp(afn,"-") == 0)
	        afn = STDFNIN ;

	    if ((rs = shio_open(afp,afn,"r",0666)) >= 0) {
	        cint	llen = LINEBUFLEN ;
	        int		len ;
	        char		lbuf[LINEBUFLEN + 1] ;

	        while ((rs = shio_readline(afp,lbuf,llen)) > 0) {
	            len = rs ;

	            if (lbuf[len - 1] == '\n') len -= 1 ;
	            lbuf[len] = '\0' ;

	                if ((cl = sfskipwhite(lbuf,len,&cp)) > 0) {
	                    if (cp[0] != '#') {
	                        pan += 1 ;
	                        rs = procnames(pip,app,po,cp,cl) ;
	                    }
	                }

	            if (rs < 0) break ;
	        } /* end while (reading lines) */

	        rs1 = shio_close(afp) ;
		if (rs >= 0) rs = rs1 ;
	    } else {
		cchar	*pn = pip->progname ;
		cchar	*fmt ;
		fmt = "%s: inaccessible argument-list (%d)\n" ;
	        shio_printf(pip->efp,fmt,pn,rs) ;
	        shio_printf(pip->efp,"%s: afile=%s\n",pn,afn) ;
	    } /* end if */

	} /* end if (processing file argument file list) */

	return rs ;
}
/* end subroutine (procargs) */


static int procnames(PROGINFO *pip,PARAMOPT *app,cchar *po,cchar *lbuf,int llen)
{
	FIELD		fsb ;
	int		rs ;
	int		c = 0 ;
	if (pip == nullptr) return SR_FAULT ;
	if ((rs = field_start(&fsb,lbuf,llen)) >= 0) {
	    int		fl ;
	    cchar	*fp ;
	    while ((fl = field_get(&fsb,aterms,&fp)) >= 0) {
	        if (fl > 0) {
	     	    rs = paramopt_loads(app,po,fp,fl) ;
	            c += rs ;
	        }
	        if (fsb.term == '#') break ;
	        if (rs < 0) break ;
	    } /* end while */
	    field_finish(&fsb) ;
	} /* end if (field) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (procnames) */


static int process(PROGINFO *pip,PARAMOPT *app,cchar *ofn)
{
	SHIO		ofile, *ofp = &ofile ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;

	if ((ofn == nullptr) || (ofn[0] == '\0') || (ofn[0] == '-'))
	    ofn = STDFNOUT ;

	if ((rs = shio_open(ofp,ofn,"wct",0666)) >= 0) {

	    rs = procmailboxes(pip,app) ;

#if	CF_DEBUG
	    if (DEBUGLEVEL(2))
	        debugprintf("b_mailnew: procmailboxes() rs=%d\n",rs) ;
#endif

	    if (pip->debuglevel > 0) {
	        shio_printf(pip->efp,"%s: nmsgs=%d\n",pip->progname,rs) ;
	    }

	    if (rs >= 0) {
	        rs = procout(pip,ofp) ;
	        wlen += rs ;
	    }

	    rs1 = shio_close(ofp) ;
	    if (rs >= 0) rs = rs1 ;
	} else {
	    cchar	*pn = pip->progname ;
	    cchar	*fmt ;
	    fmt = "%s: inaccessible output (%d)\n" ;
	    shio_printf(pip->efp,fmt,pn,rs) ;
	    shio_printf(pip->efp,"%s: ofile=%s\n",pn,ofn) ;
	}

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (process) */


/* process the mailbox */
static int procmailboxes(PROGINFO *pip,PARAMOPT *app)
{
	LOCINFO		*lip = pip->lip ;
	PARAMOPT_CUR	cur ;
	int		rs ;
	int		c = 0 ;

	if ((rs = paramopt_curbegin(app,&cur)) >= 0) {
	    vecstr	*ulp = &lip->mailusers ;
	    int		ml ;
	    int		i ;
	    cchar	*pn = pip->progname ;
	    cchar	*fmt ;
	    cchar	*po = PO_MAILDIRS ;
	    cchar	*mp ;
	    cchar	*mup ;
	    char	msfname[MAXPATHLEN+1] ;

	    while ((ml = paramopt_curfetch(app,po,&cur,&mp)) >= 0) {
	        int	mc = 0 ;

	        if (pip->debuglevel > 0) {
	            fmt = "%s: maildir=%s\n" ;
	            shio_printf(pip->efp,fmt,pn,mp) ;
		}

	        if (ml > 0) {
	            for (i = 0 ; vecstr_get(ulp,i,&mup) >= 0 ; i += 1) {
	                if (mup != nullptr) {
	                    if ((rs = mkmsfname(msfname,mp,ml,mup)) >= 0) {
	                        rs = procmailbox(pip,mup,msfname) ;
	                        mc += rs ;
	                    }
			}
	                if (rs < 0) break ;
	            } /* end for (mail-users) */
	        } /* end if (non-zero) */

	        if (pip->debuglevel > 0) {
	            fmt = "%s: msgs=%u\n" ;
	            shio_printf(pip->efp,fmt,pn,mc) ;
		}

	        c += mc ;
	        if (rs < 0) break ;
	    } /* end while */

	    paramopt_curend(app,&cur) ;
	} /* end if (paramopt-cur) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (procmailboxes) */


static int procmailbox(PROGINFO *pip,cchar *un,cchar *msfname)
{
	MAILBOX		mb ;
	cint	mbopts = MAILBOX_ORDONLY ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4)) {
	    debugprintf("b_mailnew/procmailbox: un=%s\n",un) ;
	    debugprintf("b_mailnew/procmailbox: msfname=%s\n",msfname) ;
	}
#endif

	if ((rs = mailbox_open(&mb,msfname,mbopts)) >= 0) {
	    MBCACHE	mc ;
	    if ((rs = mbcache_start(&mc,msfname,0,&mb)) >= 0) {
	        if ((rs = mbcache_count(&mc)) > 0) {
	            int		mn = rs ;
	            int		mi ;

#if	CF_DEBUG
	            if (DEBUGLEVEL(4))
	                debugprintf("b_mailnew/procmailbox: mn=%u\n",mn) ;
#endif

	            for (mi = 0 ; mi < mn ; mi += 1) {

	                c += 1 ;
	                rs = procmailmsg(pip,un,&mc,mi) ;

	                if (rs < 0) break ;
	            } /* end for */

	        } /* end if (n-mesgs) */
	        rs1 = mbcache_finish(&mc) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (mbcache) */
	    rs1 = mailbox_close(&mb) ;
	    if (rs >= 0) rs = rs1 ;
	} else {
#if	CF_DEBUG
	    if (DEBUGLEVEL(4))
	        debugprintf("b_mailnew/procmailbox: mailbox() rs=%d\n",rs) ;
#endif
	    if (isNotPresent(rs)) rs = SR_OK ;
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("b_mailnew/procmailbox: ret rs=%d c=%u\n",rs,c) ;
#endif

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (procmailbox) */


static int procmailmsg(PROGINFO *pip,cchar *un,MBCACHE *mcp,int mi)
{
	LOCINFO		*lip = pip->lip ;
	MBCACHE_SCAN	*msp ;
	int		rs ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("b_mailnew/procmailmsg: un=%s mi=%u\n",un,mi) ;
#endif

	if ((rs = mbcache_msgscan(mcp,mi,&msp)) >= 0) {
	    MSGENTRY	*mep ;
	    cint	size = sizeof(MSGENTRY) ;
	    if ((rs = uc_malloc(size,&mep)) >= 0) {
	        time_t		t = msp->htime ;
	        cint	*vl = msp->vl ;
	        cchar	**vs = msp->vs ;
	        cchar	*dp ;
	        char		ds[DSBUFLEN+1] = { 0 } ;

	        dp = vs[mbcachemf_scandate] ;

	        if (t == 0) t = msp->etime ;

		if (lip->fl.datelong || (dp == nullptr) || (dp[0] == '\0')) {
		    TMTIME	m ;
		    cint	dl = DSBUFLEN ;
		    if ((rs = tmtime_localtime(&m,t)) >= 0) {
			cchar	*fmt = "%Y%b%d %R" ;
		        if ((rs = sntmtime(ds,dl,&m,fmt)) >= 0) {
			    ds[4] = CHAR_TOLC(ds[4]) ;
			}
		    }
	        } else {
	            if ((dp != nullptr) && (dp[0] != '\0')) {
	                strwcpy(ds,(dp+7),5) ;
	            } else {
	                strwcpy(ds,"��",2) ;
		    }
		}

#if	CF_DEBUG
	        if (DEBUGLEVEL(4)) {
	            debugprintf("b_mailnew/procmailmsg: f=>%s<\n",
	                vs[mbcachemf_scanfrom]) ;
	            debugprintf("b_mailnew/procmailmsg: s=>%s<\n",
	                vs[mbcachemf_hdrsubject]) ;
	        }
#endif /* CF_DEBUG */

		if (rs >= 0) {
	            if ((rs = msgentry_start(mep,un,t,ds,vs,vl)) >= 0) {
	                rs = vechand_add(&lip->msgs,mep) ;
	                if (rs < 0)
	                    msgentry_finish(mep) ;
		    } /* end if (msgentry) */
	        } /* end if (ok) */

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("b_mailnew/procmailmsg: msgentry rs=%d\n",rs) ;
#endif

	        if (rs < 0)
	            uc_free(mep) ;
	    } /* end if (memory-allocation) */
	} /* end if (mbcache_msgscan) */

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("b_mailnew/procmailmsg: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (procmailmsg) */


static int procmaildirs(PROGINFO *pip,PARAMOPT *pop)
{
	int		rs ;
	int		c = 0 ;
	cchar		*po = PO_MAILDIRS ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	debugprintf("procmaildirs: ent\n") ;
#endif

	if ((rs = paramopt_countvals(pop,po)) == 0) {
	    int		i ;
	    cchar	*dns ;
	    cchar	*tp ;

	    for (i = 0 ; varmaildirs[i] != nullptr ; i += 1) {
	        if ((dns = getourenv(pip->envv,varmaildirs[i])) != nullptr) {
	            while ((tp = strbrk(dns," :,\t\n")) != nullptr) {
	                rs = procmaildir(pip,pop,dns,(tp-dns)) ;
	                c += rs ;
	                dns = (tp+1) ;
	                if (rs < 0) break ;
	            } /* end while */
	            if ((rs >= 0) && (dns[0] != '\0')) {
	                rs = procmaildir(pip,pop,dns,-1) ;
	                c += rs ;
	            } /* end if */
	        }
	        if (rs < 0) break ;
	    } /* end for */

	    if (rs >= 0) {
	        cchar	*dns = MAILDNAME ;
	        rs = procmaildir(pip,pop,dns,-1) ;
	        c += rs ;
	    }

	} /* end if (do not already have) */

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	debugprintf("procmaildirs: ret rs=%d c=%u\n",rs,c) ;
#endif

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (procmaildirs) */


static int procmaildir(PROGINFO *pip,PARAMOPT *pop,cchar *dp,int dl)
{
	int		rs ;
	int		c = 0 ;
	cchar		*po = PO_MAILDIRS ;

	if (pip == nullptr) return SR_FAULT ;

	if (dl < 0) dl = strlen(dp) ;

	if ((rs = paramopt_haveval(pop,po,dp,dl)) == 0) {
	    char	dname[MAXPATHLEN+1] ;
	    if ((rs = mkpath1w(dname,dp,dl)) > 0) {
	        ustat	sb ;
	        if ((rs = u_stat(dname,&sb)) >= 0) {
	            rs = paramopt_loads(pop,po,dp,dl) ;
	            c += rs ;
		} else if (isNotPresent(rs)) {
		    rs = SR_OK ;
	        }
	    } /* end if */
	} /* end if (have-val?) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (procmaildir) */


static int procmailusers(PROGINFO *pip,PARAMOPT *app)
{
	int		rs = SR_OK ;
	int		i ;
	int		c = 0 ;
	cchar		*pn = pip->progname ;
	cchar	*cp ;

	if (rs >= 0) {
	    rs = procmailusers_arg(pip,app) ;
	    c += rs ;
	}

	if ((rs >= 0) && (c == 0)) {
	    for (i = 0 ; (rs >= 0) && (varmailusers[i] != nullptr) ; i += 1) {
	        rs = procmailusers_env(pip,varmailusers[i]) ;
	        c += rs ;
	    } /* end for */
	}

	if (rs >= 0) {
	    rs = procmailusers_def(pip) ;
	    c += rs ;
	}

	if ((rs >= 0) && (pip->debuglevel > 0)) {
	    LOCINFO	*lip = pip->lip ;
	    vecstr	*mlp ;
	    mlp = &lip->mailusers ;
	    for (i = 0 ; vecstr_get(mlp,i,&cp) >= 0 ; i += 1) {
	        if (cp != nullptr) {
	            shio_printf(pip->efp,"%s: mailuser=%s\n",pn,cp) ;
		}
	    } /* end for */
	}

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (procmailusers) */


static int procmailusers_env(PROGINFO *pip,cchar *var)
{
	int		rs = SR_OK ;
	int		sl, cl ;
	int		c = 0 ;
	cchar	*tp, *sp, *cp ;

	if (var == nullptr) return SR_FAULT ;

	if ((sp = getourenv(pip->envv,var)) != nullptr) {

	    sl = strlen(sp) ;

	    while ((tp = strnbrk(sp,sl," :,\t\n")) != nullptr) {

	        if ((cl = sfshrink(sp,(tp - sp),&cp)) > 0) {
	            if (cl > USERNAMELEN) cl = USERNAMELEN ;
	            rs = procmailusers_add(pip,cp,cl) ;
	            c += rs ;
	        }

	        sl -= ((tp + 1) - sp) ;
	        sp = (tp + 1) ;

	        if (rs < 0) break ;
	    } /* end while */

	    if ((rs >= 0) && (sl > 0)) {

	        if ((cl = sfshrink(sp,sl,&cp)) > 0) {
	            rs = procmailusers_add(pip,cp,cl) ;
	            c += rs ;
	        }

	    } /* end if */

	} /* end if */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (procmailusers_env) */


static int procmailusers_arg(PROGINFO *pip,PARAMOPT *app)
{
	int		rs ;
	int		c = 0 ;
	cchar	*po = PO_MAILUSERS ;

	if ((rs = paramopt_havekey(app,po)) > 0) {
	    PARAMOPT_CUR	cur ;
	    int			cl ;
	    cchar		*cp ;

	    if ((rs = paramopt_curbegin(app,&cur)) >= 0) {

	        while ((cl = paramopt_curenumval(app,po,&cur,&cp)) >= 0) {
	            if (cp != nullptr) {

	            if ((cp[0] == '-') || (cp[0] == '+')) {
	                cp = pip->username ;
	                cl = -1 ;
	            }
	            rs = procmailusers_add(pip,cp,cl) ;
	            c += rs ;

		    }
	            if (rs < 0) break ;
	        } /* end while */

	        paramopt_curend(app,&cur) ;
	    } /* end if (cursor) */

	} /* end if (mailuser arguments) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (procmailusers_arg) */


static int procmailusers_def(PROGINFO *pip)
{
	LOCINFO		*lip = pip->lip ;
	vecstr		*mlp ;
	int		rs ;
	int		c = 0 ;
	int		f = true ;
	cchar	*username = pip->username ;

	mlp = &lip->mailusers ;
	if ((rs = vecstr_count(mlp)) > 0) {
	    int		i ;
	    cchar	*cp ;
	    c = rs ;

	    f = false ;
	    for (i = 0 ; vecstr_get(mlp,i,&cp) >= 0 ; i += 1) {
	        if (cp != nullptr) {
	            if (strcmp(cp,"+") == 0) {
	                f = true ;
	                c -= 1 ;
	                vecstr_del(mlp,i) ;
	            } else if (strcmp(cp,"-") == 0) {
	                c -= 1 ;
	                vecstr_del(mlp,i) ;
	            } /* end if */
		}
	    } /* end for */

	} /* end if (non-zero) */

	if ((rs >= 0) && (f || (c == 0))) {
	    rs = procmailusers_add(pip,username,-1) ;
	    c += rs ;
	} /* end if */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (procmailusers_def) */


static int procmailusers_add(PROGINFO *pip,cchar *dp,int dl)
{
	LOCINFO		*lip = pip->lip ;
	vecstr		*mlp = &lip->mailusers ;
	int		rs = SR_OK ;
	int		c = 0 ;

	if (dp == nullptr) return SR_FAULT ;

	if (dl < 0) dl = strlen(dp) ;

	if ((dl > 0) && (dp[0] != '\0')) {
	    if ((rs = vecstr_findn(mlp,dp,dl)) == SR_NOTFOUND) {
	        c += 1 ;
	        rs = vecstr_add(mlp,dp,dl) ;
	    } /* end if (not already) */
	} /* end if (non-zero) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (procmailusers_add) */


static int procout(PROGINFO *pip,void *ofp)
{
	LOCINFO		*lip = pip->lip ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		nshow ;
	int		wlen = 0 ;

	if (pip == nullptr) return SR_FAULT ;

	nshow = lip->nshow ;
	if (lip->fl.sort) {
	    int (*fn)() = (lip->fl.sortrev) ? vcmprev : vcmpfor ;
	    rs = vechand_sort(&lip->msgs,fn) ;
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("b_mailnew/procout: nshow=%u\n",nshow) ;
#endif

	if (nshow == 0) nshow = INT_MAX ;

	if (rs >= 0) {
	    HDRDECODE	d ;
	    cchar	*pr = pip->pr ;
	    if ((rs = hdrdecode_start(&d,pr)) >= 0) {
	        VECHAND		*mlp = &lip->msgs ;
	        MSGENTRY	*mep ;
	        int		i ;
	        int		c = 0 ;
	        for (i = 0 ; vechand_get(mlp,i,&mep) >= 0 ; i += 1) {
	            if (mep != nullptr) {
	                c += 1 ;
	                rs = procouter(pip,ofp,&d,mep) ;
	                wlen += rs ;
   	            }
	            if (c >= nshow) break ;
	            if (rs < 0) break ;
	        } /* end for */
		rs1 = hdrdecode_finish(&d) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (hdrdecode) */
	} /* end if (ok) */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (procout) */


static int procouter(PROGINFO *pip,void *ofp,HDRDECODE *hdp,MSGENTRY *mep)
{
	LOCINFO		*lip = pip->lip ;
	OUTINFO		oi ;
	int		rs ;
	int		rs1 ;
	int		ll, dl ;
	int		wlen = 0 ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("b_mailnew/procouter: ent\n") ;
#endif

	ll = lip->linelen ;
	dl = (lip->fl.datelong) ? 15 : 5 ;
	if ((rs = outinfo_start(&oi,mep,ll,dl)) >= 0) {
	    if ((rs = outinfo_trans(&oi,hdp)) >= 0) {
	        if ((rs = outinfo_calc(&oi)) >= 0) {
		    rs = outinfo_print(&oi,pip,ofp,rs) ;
		    wlen += rs ;
#if	CF_DEBUG
	if (DEBUGLEVEL(5))
	    debugprintf("b_mailnew/procouter: outinfo_print() rs=%d\n",rs) ;
#endif
		} /* end if (outinfo-calc) */
#if	CF_DEBUG
	if (DEBUGLEVEL(5))
	    debugprintf("b_mailnew/procouter: outinfo_trans() rs=%d\n",rs) ;
#endif
	    } /* end if (outinfo-trans) */
	    rs1 = outinfo_finish(&oi) ;
	    if (rs >= 0) rs = rs1 ;
#if	CF_DEBUG
	if (DEBUGLEVEL(5))
	    debugprintf("b_mailnew/procouter: outinfo_finish() rs=%d\n",rs1) ;
#endif
	} /* end if (outinfo) */

#if	CF_DEBUG
	if (DEBUGLEVEL(5))
	    debugprintf("b_mailnew/procouter: ret rs=%d wlen=%u\n",rs,wlen) ;
#endif

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (procouter) */


static int outinfo_start(OUTINFO *oip,MSGENTRY *mep,int ll,int dl)
{
	int		rs = SR_OK ;

	memset(oip,0,sizeof(OUTINFO)) ;
	oip->mep = mep ;
	oip->ll = ll ;
	oip->dl = dl ;

	return rs ;
}
/* end subroutine (outinfo_start) */


static int outinfo_finish(OUTINFO *oip)
{
	int		rs = SR_OK ;
	int		rs1 ;

#if	CF_DEBUGS
	debugprintf("b_mailnew/outinfo_finish: ent\n") ;
	debugprintf("b_mailnew/outinfo_finish: fbuf{%p}\n",oip->fbuf) ;
#endif

	if (oip->fbuf != nullptr) {
	    rs1 = uc_free(oip->fbuf) ;
	    if (rs >= 0) rs = rs1 ;
	    oip->fbuf = nullptr ;
	}

#if	CF_DEBUGS
	debugprintf("b_mailnew/outinfo_finish: mid1 rs=%d\n",rs) ;
#endif

	if (oip->sbuf != nullptr) {
	    rs1 = uc_free(oip->sbuf) ;
	    if (rs >= 0) rs = rs1 ;
	    oip->sbuf = nullptr ;
	}

#if	CF_DEBUGS
	debugprintf("b_mailnew/outinfo_finish: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (outinfo_finish) */


static int outinfo_trans(OUTINFO *oip,HDRDECODE *hdp)
{
	MSGENTRY	*mep = oip->mep ;
	int		rs = SR_OK ;
	int		cl ;
	int		bl ;
	cchar		*cp ;
	char		*bp ;

#if	CF_DEBUGS
	debugprintf("b_mailnew/outinfo_trans: ent\n") ;
#endif

/* FROM header value */

	if (rs >= 0) {
	    if ((cl = msgentry_from(mep,&cp)) > 0) {
		cint	c = MIN(cl,MAILADDRLEN) ; /* desired */
		bl = (c*2) ;
		oip->flen = bl ;
		if ((rs = uc_malloc((bl+1),&bp)) >= 0) {
		    oip->fbuf = bp ;
		    if ((rs = outinfo_cvt(oip,hdp,bp,bl,cp,cl,c)) >= 0) {
			oip->fl = rs ;
		    }
		} /* end if (m-a) */
	    } /* end if (msgentry_from) */
	} /* end if (ok) */

#if	CF_DEBUGS
	{
	    debugprintf("b_mailnew/outinfo_trans: from rs=%d\n",rs) ;
	    if (oip->fbuf != nullptr) {
		int	rs1 = uc_mallpresent(oip->fbuf) ;
	        debugprintf("b_mailnew/outinfo_trans: mp{fbuf}=%d\n",rs1) ;
	    }
	}
#endif /* CF_DEBUGS */

/* SUBJ header value */

	if (rs >= 0) {
	    if ((cl = msgentry_subj(mep,&cp)) > 0) {
	        cint	c = MIN(cl,SUBJBUFLEN) ; /* desired */
		bl = (c*2) ;
		oip->slen = bl ;
	        if ((rs = uc_malloc((bl+1),&bp)) >= 0) {
		    oip->sbuf = bp ;
	            if ((rs = outinfo_cvt(oip,hdp,bp,bl,cp,cl,c)) >= 0) {
			oip->sl = rs ;
		    }
	        } /* end if (m-a) */
	    } /* end if (msgentry_subj) */
	} /* end if (ok) */

#if	CF_DEBUGS
	{
	    if (oip->fbuf != nullptr) {
		int	rs1 = uc_mallpresent(oip->fbuf) ;
	        debugprintf("b_mailnew/outinfo_trans: mp{fbuf}=%d\n",rs1) ;
	    }
	}
	debugprintf("b_mailnew/outinfo_trans: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (outinfo_trans) */


static int outinfo_cvt(OUTINFO *oip,HDRDECODE *hdp,
		char *rbuf,int rlen,cchar *sp,int sl,int c) 
{
	cint	wsize = ((sl+1) * sizeof(wchar_t)) ;
	cint	wlen = sl ;
	int		rs ;
	int		rl = 0 ;
	wchar_t		*wbuf ;
#if	CF_DEBUGS
	debugprintf("b_mailnew/outinfo_cvt: ent sl=%d c=%u\n",sl,c) ;
#endif
	if (oip == nullptr) return SR_FAULT ;
	if ((rs = uc_malloc(wsize,&wbuf)) >= 0) {
	    if ((rs = hdrdecode_proc(hdp,wbuf,wlen,sp,sl)) >= 0) {
		cint	n = MIN(c,rs) ;
		int		tlen ;
		char		*tbuf ;
		tlen = (n*2) ;
		if ((rs = uc_malloc((tlen+1),&tbuf)) >= 0) {
		    if ((rs = snwcpywidehdr(tbuf,tlen,wbuf,n)) >= 0) {
			rs = mkaddrdisp(rbuf,rlen,tbuf,rs) ;
		        rl = rs ;
		    }
		    uc_free(tbuf) ;
		} /* end if (m-a-f) */
	    } /* end if (hdrdecode_proc) */
	    uc_free(wbuf) ;
	} /* end if (m-a-f) */
#if	CF_DEBUGS
	debugprintf("b_mailnew/outinfo_cvt: ret rs=%d rl=%d\n",rs,rl) ;
#endif
	return (rs >= 0) ? rl : rs ;
}
/* end if (outinfo_cvt) */


static int outinfo_calc(OUTINFO *oip)
{
	MSGENTRY	*mep = oip->mep ;
	int		rs = SR_OK ;
	int		rl = 0 ;
	int		tl ;

#if	CF_DEBUGS
	debugprintf("outinfo_calc: ent\n") ;
#endif

/* setup */

	oip->ul = strlen(mep->user) ;

	if ((rl = sfnormfrom(oip->fbuf,oip->fl)) > 0) {
	    oip->fl = rl ;
	}

/* reductions */

	tl = outinfo_total(oip) ;

#if	CF_DEBUGS
	debugprintf("outinfo_calc: tl=%d\n",tl) ;
#endif

	if (tl > oip->ll) {
	    rl = (tl - oip->ll) ;
	    if ((oip->fl - rl) >= MAXFROMLEN) {
		oip->fl -= rl ;
	        tl = outinfo_total(oip) ;
	    }
	}

#if	CF_DEBUGS
	debugprintf("outinfo_calc: tl=%d\n",tl) ;
#endif

	if (tl > oip->ll) {
	    if (oip->fl > MAXFROMLEN) {
		oip->fl = MAXFROMLEN ;
	        tl = outinfo_total(oip) ;
	    }
	}

#if	CF_DEBUGS
	debugprintf("outinfo_calc: tl=%d\n",tl) ;
#endif

	if (tl > oip->ll) {
	    rl = (tl - oip->ll) ;
	    oip->sl -= rl ;
	    tl = outinfo_total(oip) ;
	}

#if	CF_DEBUGS
	debugprintf("outinfo_calc: ret rs=%d tl=%d\n",rs,tl) ;
#endif

	return (rs >= 0) ? tl : rs ;
}
/* end if (outinfo_calc) */


static int outinfo_total(OUTINFO *oip)
{
	int	len = (1+3+3) ; /* non-field columns in output string */
	len += (oip->dl+oip->ul+oip->fl+oip->sl) ;
	return len ;
}
/* end if (outinfo_total) */


static int outinfo_print(OUTINFO *oip,PROGINFO *pip,void *ofp,int olen)
{
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	char		*obuf ;

#if	CF_DEBUG
	if (DEBUGLEVEL(5))
	debugprintf("b_mailnew/outinfo_print: ent olen=%d\n",olen) ;
#endif

	if ((rs = uc_malloc((olen+1),&obuf)) >= 0) {
	    MSGENTRY	*mep = oip->mep ;
	    LOCINFO	*lip = pip->lip ;
	    cint	fl = oip->fl ;
	    cint	sl = oip->sl ;
	    cchar	*pn = pip->progname ;
	    cchar	*db = mep->date ;
	    cchar	*un = mep->user ;
	    cchar	*fb = oip->fbuf ;
	    cchar	*sb = oip->sbuf ;
	    cchar 	*fmt = "%s %s � %r � %r" ;
	    char	tbuf[TIMEBUFLEN+1] ;

#if	CF_DEBUG
	    if (DEBUGLEVEL(5)) {
	        debugprintf("b_mailnew/outinfo_print: db=>%r<\n",
		    db,strlinelen(db,-1,40)) ;
	        debugprintf("b_mailnew/outinfo_print: un=%r\n",
		    un,strlinelen(un,-1,40)) ;
	        debugprintf("b_mailnew/outinfo_print: fl=%d sl=%d\n",fl,sl) ;
	        debugprintf("b_mailnew/outinfo_print: sb=>%r<\n",
		    sb,strlinelen(sb,-1,40)) ;
	    }
#endif /* CF_DEBUGS */

	    if ((rs = bufprintf(obuf,olen,fmt,db,un,fb,fl,sb,sl)) >= 0) {
	        rs = shio_print(ofp,obuf,rs) ;
	        wlen += rs ;
	    }

#if	CF_DEBUG
	    if (DEBUGLEVEL(5))
	        debugprintf("b_mailnew/outinfo_print: "
			"shio_print() rs=%d\n",rs) ;
#endif

	        if (pip->debuglevel > 0) {
		    fmt = "%s: %s u=%s date=%s\n" ;
	            timestr_logz(pip->daytime,tbuf) ;
	            shio_printf(pip->efp,fmt,pn,tbuf,un,db) ;
	        }

	        timestr_logz(pip->daytime,tbuf) ;
	        locinfo_logprintf(lip,"%s u=%s time=%s",tbuf,un,db) ;
	        locinfo_logprintf(lip,"  from=�%r�",fb,fl) ;
	        locinfo_logprintf(lip,"  subj=�%r�",sb,sl) ;

	    rs1 = uc_free(obuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (memory-allocation) */

#if	CF_DEBUG
	if (DEBUGLEVEL(5))
	    debugprintf("b_mailnew/outinfo_print: ret rs=%d wlen=%d\n",
		rs,wlen) ;
#endif

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (outinfo_print) */


static int locinfo_start(LOCINFO *lip,PROGINFO *pip)
{
	int		rs ;

	memset(lip,0,sizeof(LOCINFO)) ;
	lip->pip = pip ;

	if ((rs = vecstr_start(&lip->stores,0,0)) >= 0) {
	    lip->open.stores = true ;
	    if ((rs = vecstr_start(&lip->mailusers,1,0)) >= 0) {
	        lip->open.mailusers = true ;
	        if ((rs = vechand_start(&lip->msgs,5,0)) >= 0) {
	            lip->open.msgs = true ;
	        }
	        if (rs < 0) {
	            lip->open.mailusers = false ;
	            vecstr_finish(&lip->mailusers) ;
	        }
	    } /* end if (mailusers) */
	    if (rs < 0) {
	        lip->open.stores = false ;
	        vecstr_finish(&lip->stores) ;
	    }
	} /* end if (stores) */

	return rs ;
}
/* end subroutine (locinfo_start) */


static int locinfo_finish(LOCINFO *lip)
{
	int		rs = SR_OK ;
	int		rs1 ;

	{
	    MSGENTRY	*mep ;
	    vechand	*mlp = &lip->msgs ;
	    int		i ;
	    for (i = 0 ; vechand_get(mlp,i,&mep) >= 0 ; i += 1) {
	        if (mep != nullptr) {
	            rs1 = msgentry_finish(mep) ;
	            if (rs >= 0) rs = rs1 ;
	            rs1 = uc_free(mep) ;
	            if (rs >= 0) rs = rs1 ;
		}
	    } /* end for */
	} /* end block */

	if (lip->open.msgs) {
	    lip->open.msgs = false ;
	    rs1 = vechand_finish(&lip->msgs) ;
	    if (rs >= 0) rs = rs1 ;
	}

	if (lip->open.mailusers) {
	    lip->open.mailusers = false ;
	    rs1 = vecstr_finish(&lip->mailusers) ;
	    if (rs >= 0) rs = rs1 ;
	}

	if (lip->open.stores) {
	    lip->open.stores = false ;
	    rs1 = vecstr_finish(&lip->stores) ;
	    if (rs >= 0) rs = rs1 ;
	}

	return rs ;
}
/* end subroutine (locinfo_finish) */


#if	CF_LOCSETENT
static int locinfo_setentry(LOCINFO *lip,cchar **epp,cchar *vp,int vl)
{
	vecstr		*slp ;
	int		rs = SR_OK ;
	int		len = 0 ;

	if (lip == nullptr) return SR_FAULT ;
	if (epp == nullptr) return SR_FAULT ;

	slp = &lip->stores ;
	if (! lip->open.stores) {
	    rs = vecstr_start(slp,4,0) ;
	    lip->open.stores = (rs >= 0) ;
	}

	if (rs >= 0) {
	    int	oi = -1 ;
	    if (*epp != nullptr) {
		oi = vecstr_findaddr(slp,*epp) ;
	    }
	    if (vp != nullptr) {
	        len = strnlen(vp,vl) ;
	        rs = vecstr_store(slp,vp,len,epp) ;
	    } else {
	        *epp = nullptr ;
	    }
	    if ((rs >= 0) && (oi >= 0)) {
	        vecstr_del(slp,oi) ;
	    }
	} /* end if (ok) */

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (locinfo_setentry) */
#endif /* CF_LOCSETENT */


int locinfo_nshow(LOCINFO *lip,cchar *sp)
{
	int		rs = SR_OK ;

	if (! lip->final.nshow) {
	    if (sp != nullptr) {
	        int	v ;
	        lip->final.nshow = true ;
	        lip->have.nshow = true ;
	        rs = cfdeci(sp,-1,&v) ;
	        lip->nshow = v ;
	    }
	} /* end if (needed 'nshow') */

	return rs ;
}
/* end subroutine (locinfo_nshow) */


static int locinfo_optdate(LOCINFO *lip,cchar *vp,int vl)
{
	int		rs = SR_OK ;
	if (vl > 0) {
	    int	oi ;
	    if ((oi = matpstr(datetypes,1,vp,vl)) >= 0) {
		switch (oi) {
		case datetype_long:
		    lip->fl.datelong = true ;
		    break ;
		} /* end switch */
	    } /* end if (matpstr) */
	} /* end if (have option-value) */
	return rs ;
}
/* end subroutine (locinfo_optdate) */


#if	CF_LOCEPRINTF
static int locinfo_eprintf(LOCINFO *lip,cchar *fmt,...)
{
	PROGINFO	*pip = lip->pip ;
	int		rs = SR_OK ;
	if (pip->debuglevel > 0) {
	    va_list	ap ;
	    va_begin(ap,fmt) ;
	    rs = shio_vprintf(pip->efp,fmt,ap) ;
	    va_end(ap) ;
	}
	return rs ;
}
/* end subroutine (locinfo_eprintf) */
#endif /* CF_LOCEPRINTF */


static int locinfo_logprintf(LOCINFO *lip,cchar *fmt,...)
{
	PROGINFO	*pip = lip->pip ;
	int		rs = SR_OK ;
	if (pip->open.logprog) {
	    va_list	ap ;
	    va_begin(ap,fmt) ;
	    rs = logfile_vprintf(&pip->lh,fmt,ap) ;
	    va_end(ap) ;
	}
	return rs ;
}
/* end subroutine (locinfo_logprintf) */


/* configuration management */
static int config_start(CONFIG *csp,PROGINFO *pip,PARAMOPT *app,cchar *cfname)
{
	int		rs = SR_OK ;
	char		tbuf[MAXPATHLEN+1] = { 0 } ;

	if (csp == nullptr) return SR_FAULT ;

	if (cfname == nullptr) cfname = CONFIGFNAME ;

	memset(csp,0,sizeof(CONFIG)) ;
	csp->pip = pip ;
	csp->app = app ;

	if (strchr(cfname,'/') == nullptr) {
	    rs = config_findfile(csp,tbuf,cfname) ;
	    if (rs > 0) cfname = tbuf ;
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("config_start: mid rs=%d cfname=%s\n",rs,cfname) ;
#endif

	if ((rs >= 0) && (pip->debuglevel > 0)) {
	    shio_printf(pip->efp,"%s: conf=%s\n",pip->progname,cfname) ;
	}

	if (rs >= 0) {
	    if ((rs = paramfile_open(&csp->p,pip->envv,cfname)) >= 0) {
	        if ((rs = config_cookbegin(csp)) >= 0) {
	            csp->f_p = (rs >= 0) ;
	        }
	        if (rs < 0)
	            paramfile_close(&csp->p) ;
	    } else if (isNotPresent(rs)) {
	        rs = SR_OK ;
	    }
	} else if (isNotPresent(rs)) {
	    rs = SR_OK ;
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("config_start: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (config_start) */


static int config_findfile(CONFIG *op,char tbuf[],cchar cfname[])
{
	PROGINFO	*pip = op->pip ;
	int		rs = SR_OK ;
	int		tl = 0 ;

	tbuf[0] = '\0' ;
	if ((cfname != nullptr) && (cfname[0] != '\0')) {
	    vecstr	sv ;
	    if ((rs = vecstr_start(&sv,6,0)) >= 0) {
	        cint	tlen = MAXPATHLEN ;
	        int		i ;
	        int		vl ;
	        int		kch ;
	        cchar	*ks = "pen" ;
	        cchar	*vp ;
	        char	kbuf[2] ;
	        kbuf[1] = '\0' ;
	        for (i = 0 ; (rs >= 0) && (ks[i] != '\0') ; i += 1) {
	            kch = (ks[i] & 0xff) ;
	            kbuf[0] = kch ;
	            vp = nullptr ;
	            vl = -1 ;
	            switch (kch) {
	            case 'p':
	                vp = pip->pr ;
	                break ;
	            case 'e':
	                vp = "etc" ;
	                break ;
	            case 'n':
	                vp = pip->searchname ;
	                break ;
	            } /* end switch */
	            if ((rs >= 0) && (vp != nullptr)) {
	                rs = vecstr_envset(&sv,kbuf,vp,vl) ;
	            }
	        } /* end for */
	        if (rs >= 0) {
	            rs = permsched(sched1,&sv,tbuf,tlen,cfname,R_OK) ;
	            tl = rs ;
	        }
	        vecstr_finish(&sv) ;
	    } /* end if (finding file) */
	} /* end if (non-null) */

	return (rs >= 0) ? tl : rs ;
}
/* end subroutine (config_findfile) */


static int config_cookbegin(CONFIG *op)
{
	PROGINFO	*pip = op->pip ;
	int		rs ;

	if ((rs = expcook_start(&op->cooks)) >= 0) {
	    cint	hlen = MAXHOSTNAMELEN ;
	    int		i ;
	    int		kch ;
	    int		vl ;
	    cchar	*ks = "PSNDHRU" ;
	    cchar	*vp ;
	    char	hbuf[MAXHOSTNAMELEN+1] ;
	    char	kbuf[2] ;

	    kbuf[1] = '\0' ;
	    for (i = 0 ; (rs >= 0) && (ks[i] != '\0') ; i += 1) {
	        kch = MKCHAR(ks[i]) ;
	        vp = nullptr ;
	        vl = -1 ;
	        switch (kch) {
	        case 'P':
	            vp = pip->progname ;
	            break ;
	        case 'S':
	            vp = pip->searchname ;
	            break ;
	        case 'N':
	            vp = pip->nodename ;
	            break ;
	        case 'D':
	            vp = pip->domainname ;
	            break ;
	        case 'H':
	            {
	                cchar	*nn = pip->nodename ;
	                cchar	*dn = pip->domainname ;
	                rs = snsds(hbuf,hlen,nn,dn) ;
	                vl = rs ;
	                if (rs >= 0) vp = hbuf ;
	            }
	            break ;
	        case 'R':
	            vp = pip->pr ;
	            break ;
	        case 'U':
	            vp = pip->username ;
	            break ;
	        } /* end switch */
	        if ((rs >= 0) && (vp != nullptr)) {
	            kbuf[0] = kch ;
	            rs = expcook_add(&op->cooks,kbuf,vp,vl) ;
	        }
	    } /* end for */

	    if (rs >= 0) {
	        op->f_cooks = true ;
	    } else {
	        expcook_finish(&op->cooks) ;
	    }
	} /* end if (expcook_start) */

	return rs ;
}
/* end subroutine (config_cookbegin) */


static int config_cookend(CONFIG *op)
{
	int		rs = SR_OK ;
	int		rs1 ;

	if (op->f_cooks) {
	    op->f_cooks = false ;
	    rs1 = expcook_finish(&op->cooks) ;
	    if (rs >= 0) rs = rs1 ;
	}

	return rs ;
}
/* end subroutine (config_cookend) */


static int config_finish(CONFIG *csp)
{
	PROGINFO	*pip ;
	int		rs = SR_OK ;
	int		rs1 ;

	if (csp == nullptr) return SR_FAULT ;
	pip = csp->pip ;
	if (pip == nullptr) return SR_FAULT ;

	if (csp->f_p) {

	    if (csp->f_cooks) {
	        rs1 = config_cookend(csp) ;
	        if (rs >= 0) rs = rs1 ;
	    }

	    rs1 = paramfile_close(&csp->p) ;
	    if (rs >= 0) rs = rs1 ;

	    csp->f_p = false ;
	} /* end if */

	return rs ;
}
/* end subroutine (config_finish) */


#if	CF_CONFIGCHECK
static int config_check(CONFIG *op)
{
	PROGINFO	*pip = op->pip ;
	int		rs = SR_OK ;

	if (op->f_p) {
	    if ((rs = paramfile_check(&op->p,pip->daytime)) > 0) {
	        rs = config_read(op) ;
	    }
	}

	return rs ;
}
/* end subroutine (config_check) */
#endif /* CF_CONFIGCHECK */


static int config_read(CONFIG *op)
{
	PROGINFO	*pip = op->pip ;
	LOCINFO		*lip ;
	int		rs = SR_OK ;

	if (pip == nullptr) return SR_FAULT ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("config_read: f_p=%u\n",op->f_p) ;
#endif

	lip = pip->lip ;
	if (lip == nullptr) return SR_FAULT ;

	if (op->f_p) {
	    PARAMOPT		*app = op->app ;
	    PARAMFILE		*pfp = &op->p ;
	    PARAMFILE_CUR	cur ;
	    cint		vlen = VBUFLEN ;
	    cint		elen = EBUFLEN ;
	    int			i ;
	    int			ml, vl, el ;
	    int			v ;
	    cchar		*pr = pip->pr ;
	    char		vbuf[VBUFLEN + 1] ;
	    char		ebuf[EBUFLEN + 1] ;
	    for (i = 0 ; cparams[i] != nullptr ; i += 1) {

	        if ((rs = paramfile_curbegin(pfp,&cur)) >= 0) {
	            char	tbuf[MAXPATHLEN + 1] ;

	            while (rs >= 0) {
	                vl = paramfile_fetch(pfp,cparams[i],&cur,vbuf,vlen) ;
	                if (vl == SR_NOTFOUND) break ;
	                rs = vl ;
	                if (rs < 0) break ;

#if	CF_DEBUG
	                if (DEBUGLEVEL(4))
	                    debugprintf("config_read: vbuf=>%r<\n",vbuf,vl) ;
#endif

	                ebuf[0] = '\0' ;
	                el = 0 ;
	                if (vl > 0) {
	                    el = expcook_exp(&op->cooks,0,ebuf,elen,vbuf,vl) ;
	                    if (el >= 0) ebuf[el] = '\0' ;
	                }

#if	CF_DEBUG
	                if (DEBUGLEVEL(4))
	                    debugprintf("config_read: ebuf=>%r<\n",ebuf,el) ;
#endif

	                switch (i) {
	                case cparam_logsize:
	                    if (el > 0) {
	                        if (cfdecmfi(ebuf,el,&v) >= 0) {
	                            if (v >= 0) {
	                                switch (i) {
	                                case cparam_logsize:
	                                    pip->logsize = v ;
	                                    break ;
	                                } /* end switch */
	                            }
	                        } /* end if (valid number) */
	                    }
	                    break ;
	                case cparam_maildir:
	                    ml = prsetfname(pr,tbuf,ebuf,el,true,
	                        nullptr,MAILDNAME,"") ;
	                    if (ml > 0) {
	                        rs = procmaildir(pip,app,tbuf,ml) ;
			    }
	                    break ;
	                case cparam_logfile:
	                    if (el > 0) {
	                        if (! pip->final.lfname) {
	                            pip->final.lfname = true ;
	                            pip->have.lfname = true ;
	                            rs = config_setlfname(op,ebuf,el) ;
	                        }
	                    }
	                    break ;
	                } /* end switch */

	            } /* end while (fetching) */

	            paramfile_curend(pfp,&cur) ;
	        } /* end if (cursor) */

	        if (rs < 0) break ;
	    } /* end for (parameters) */
	} /* end if (active) */

	return rs ;
}
/* end subroutine (config_read) */


static int config_setlfname(CONFIG *cfp,cchar *vp,int vl)
{
	PROGINFO	*pip = cfp->pip ;
	cchar	*pr ;
	cchar	*sn ;
	cchar	*lfn ;
	int		rs ;
	char		tbuf[MAXPATHLEN+1] ;

	pr = pip->pr ;
	sn = pip->searchname ;
	lfn = pip->lfname ;
	if ((rs = prsetfname(pr,tbuf,vp,vl,true,LOGCNAME,sn,"")) >= 0) {
	    cint	tl = rs ;
	    if ((lfn == nullptr) || (strcmp(lfn,tbuf) != 0)) {
	        cchar	**vpp = &pip->lfname ;
	        pip->changed.lfname = true ;
	        rs = proginfo_setentry(pip,vpp,tbuf,tl) ;
	    }
	}

	return rs ;
}
/* end subroutine (config_setlfname) */


static int msgentry_start(mep,userp,t,datep,vs,vl)
MSGENTRY	*mep ;
cchar	*userp ;
time_t		t ;
cchar	*datep ;
cchar	*vs[] ;
cint	vl[] ;
{
	int		rs = SR_OK ;
	int		ul ;
	int		dl = 0 ;
	int		vi ;
	int		size = 0 ;
	char		*bp ;

	if (mep == nullptr) return SR_FAULT ;
	if (userp == nullptr) return SR_FAULT ;
	if (vs == nullptr) return SR_FAULT ;
	if (vl == nullptr) return SR_FAULT ;

	memset(mep,0,sizeof(MSGENTRY)) ;
	mep->mdate = t ;

	size += ((ul = strlen(userp)) + 1) ;
	if (datep != nullptr) size += ((dl = strlen(datep)) + 1) ;
	vi = mbcachemf_scanfrom ;
	if (vs[vi] != nullptr) size += (vl[vi] + 1) ;
	vi = mbcachemf_hdrsubject ;
	if (vs[vi] != nullptr) size += (vl[vi] + 1) ;

	if ((rs = uc_malloc(size,&bp)) >= 0) {
	    mep->a = bp ;

	    if (datep != nullptr) {
	        mep->date = bp ;
	        bp = (strwcpy(bp,datep,dl) + 1) ;
	    }

	    mep->user = bp ;
	    bp = (strwcpy(bp,userp,ul) + 1) ;

	    vi = mbcachemf_scanfrom ;
	    if (vs[vi] != nullptr) {
	        mep->from = bp ;
	        bp = (strwcpy(bp,vs[vi],vl[vi]) + 1) ;
	    } else {
	        mep->from = (bp-1) ;
	    }

	    vi = mbcachemf_hdrsubject ;
	    if (vs[vi] != nullptr) {
	        mep->subj = bp ;
	        bp = (strwcpy(bp,vs[vi],vl[vi]) + 1) ;
	    } else {
	        mep->subj = (bp-1) ;
	    }

	} /* end if (memory-allocation) */

	return rs ;
}
/* end subroutine (msgentry_start) */


static int msgentry_finish(MSGENTRY *mep)
{
	int		rs = SR_OK ;
	int		rs1 ;

	if (mep->a != nullptr) {
	    rs1 = uc_free(mep->a) ;
	    if (rs >= 0) rs = rs1 ;
	    mep->a = nullptr ;
	}

	return rs ;
}
/* end subroutine (msgentry_finish) */


static int msgentry_from(MSGENTRY *mep,cchar **rpp)
{
	int		rs = SR_OK ;
	int		cl ;
	cchar		*s = mep->from ;
	cchar		*cp ;

	if (mep->a == nullptr) rs = SR_NOTFOUND ;

	cl = sfshrink(s,-1,&cp) ;

	if (rpp != nullptr) {
	    *rpp = (rs >= 0) ? cp : nullptr ;
	}

	return (rs >= 0) ? cl : rs ;
}
/* end subroutine (msgentry_from) */


static int msgentry_subj(MSGENTRY *mep,cchar **rpp)
{
	int		rs = SR_OK ;
	int		cl ;
	cchar		*s = mep->subj ;
	cchar		*cp ;

	if (mep->a == nullptr) rs = SR_NOTFOUND ;

	cl = sfshrink(s,-1,&cp) ;

	if (rpp != nullptr) {
	    *rpp = (rs >= 0) ? cp : nullptr ;
	}

	return (rs >= 0) ? cl : rs ;
}
/* end subroutine (msgentry_subj) */


static int mkmsfname(char rbuf[],cchar mbuf[],int mlen,cchar *mup)
{
	cint	rlen = MAXPATHLEN ;
	int		rs = SR_OK ;
	int		i = 0 ;

	if (rs >= 0) {
	    rs = storebuf_strw(rbuf,rlen,i,mbuf,mlen) ;
	    i += rs ;
	}

	if ((rs >= 0) && (i > 0) && (rbuf[i-1] != '/')) {
	    rs = storebuf_chr(rbuf,rlen,i,'/') ;
	    i += rs ;
	}

	if (rs >= 0) {
	    rs = storebuf_strw(rbuf,rlen,i,mup,-1) ;
	    i += rs ;
	}

	return (rs >= 0) ? i : rs ;
}
/* end subroutine (mkmsfname) */


static int sfnormfrom(cchar *fp,int fl)
{
	cchar	*tp ;
	if ((tp = strnchr(fp,fl,',')) != nullptr) {
	    fl = (tp-fp) ;
	    while (fl && CHAR_ISWHITE(fp[fl-1])) fl -= 1 ;
	}
	return fl ;
}
/* end subroutine (sfnormfrom) */


static int vcmpfor(cvoid *v1pp,cvoid *v2pp)
{
	MSGENTRY	**e1pp = (MSGENTRY **) v1pp ;
	MSGENTRY	**e2pp = (MSGENTRY **) v2pp ;
	int		rc = 0 ;
	if ((*e1pp != nullptr) || (*e2pp != nullptr)) {
	    if (*e1pp != nullptr) {
	        if (*e2pp != nullptr) {
	            rc = ((*e1pp)->mdate - (*e2pp)->mdate) ;
	        } else
	            rc = -1 ;
	    } else
		rc = 1 ;
	}
	return rc ;
}
/* end subroutine (vcmpfor) */


static int vcmprev(cvoid *v1pp,cvoid *v2pp)
{
	MSGENTRY	**e1pp = (MSGENTRY **) v1pp ;
	MSGENTRY	**e2pp = (MSGENTRY **) v2pp ;
	int		rc = 0 ;
	if ((*e1pp != nullptr) || (*e2pp != nullptr)) {
	    if (*e1pp != nullptr) {
	        if (*e2pp != nullptr) {
	            rc = - ((*e1pp)->mdate - (*e2pp)->mdate) ;
	        } else
	            rc = -1 ;
	    } else
		rc = 1 ;
	}
	return rc ;
}
/* end subroutine (vcmprev) */


