/* bbnews_main SUPPORT (bbnews) */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* BB-News reader */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* non-switchable */
#define	CF_DEBUG	0		/* run-time debug print-outs */
#define	CF_DEBUGMALL	1		/* debug memory-allocations */
#define	CF_FORCETERM	0		/* force a terminal for testing */
#define	CF_mkdirlist	1		/* enable mkdirlist */
#define	CF_ANSITERMS	0		/* ansiterms[] */

/* revision history:

	= 1995-05-01, David A­D­ Morano
	This code module was completely rewritten to replace any
	original garbage that was here before.

	= 1998-11-22, David A­D­ Morano
        I did some clean-up.

*/

/* Copyright © 1995,1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	main

	Description:
	This is the front-end subroutine (main) for the BB program.
	It is similar to most other PCS programs but may be a little
	different since it originated differently from the others.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<termios.h>
#include	<unistd.h>
#include	<ctime>
#include	<csignal>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<bufsizevar.hh>
#include	<sighand.h>
#include	<bits.h>
#include	<keyopt.h>
#include	<bfile.h>
#include	<userinfo.h>
#include	<field.h>
#include	<vecstr.h>
#include	<vecpstr.h>
#include	<dater.h>
#include	<tmctimeh>
#include	<pcsconf.h>
#include	<pcspoll.h>
#include	<mailmsghdrs.h>
#include	<initnow.h>
#include	<getourenv.h>
#include	<mkdirlist.h>
#include	<exitcodes.h>
#include	<localmisc.h>

#include	"artlist.h"
#include	"config.h"
#include	"defs.h"
#include	"dirshown.h"


/* local defines */

#define	PI	proginfo


/* external subroutines */

extern int	printhelp(bfile *,cchar *,cchar *,cchar *) ;
extern int	proginfo_setpiv(PI *,cchar *,const pivars *) ;

extern int	proglog_begin(PI *,userinfo *) ;
extern int	proglog_end(PI *) ;
extern int	proglog_print(PI *,cchar *,int) ;
extern int	proglog_printf(PI *,cchar *,...) ;
extern int	proglog_flush(PI *) ;

extern int	proguserlist_begin(PI *) ;
extern int	proguserlist_end(PI *) ;

extern int	progmsgenv_begin(PI *) ;
extern int	progmsgenv_envstr(PI *,char *,int) ;
extern int	progmsgenv_end(PI *) ;

extern int	progufname(PI *,cchar *) ;
extern int	prognewsrc(PI *,mkdirlist *,cchar *) ;
extern int	progmode(PI *,mkdirlist *,cchar *) ;

#ifdef	COMMENT
extern int	process(PI *,mkdirlist *) ;
#endif

#if	CF_DEBUGS || CF_DEBUG
extern int	debugopen(cchar *) ;
extern int	debugprintf(cchar *,...) ;
extern int	debugprinthexblock(cchar *,int,cchar *,int) ;
extern int	debugclose() ;
extern int	strlinelen(cchar *,int,int) ;
#endif


/* external variables */


/* external variables */


/* local structures */


/* forward references */

int		term_linecheck(PI *) ;

local int	usage(PI *) ;

local int	procopts(PI *,keyopt *) ;
local int	process(PI *,ARGINFO *,bits *,cchar *,cchar *,cchar *) ;
local int	procargs(PI *,ARGINFO *,bits *,cchar *,cchar *) ;
local int	procspecs(PI *,vecpstr *,cchar *,int) ;
local int	procvarnewsgroups(PI *,vecpstr *) ;
local int	procnewsdname(PI *pip) ;
local int	procuserboards(PI *,vecpstr *,cchar *) ;
local int	procboards(PI *,mkdirlist *,cchar *) ;

local int	procuserinfo_begin(PI *,userinfo *) ;
local int	procuserinfo_end(PI *) ;

local int	procpcsconf_begin(PI *,PCSCONF *) ;
local int	procpcsconf_end(PI *) ;

local void	main_sighand(int,siginfo_t *,void *) ;


/* local variables */

static volatile int	if_exit ;
static volatile int	if_int ;

constexpr int		sigblocks[] = {
	0
} ; /* end array */

constexpr int		sigignores[] = {
	SIGPIPE,
	SIGPOLL,
	SIGHUP,
	0
} ; /* end array */

constexpr int		sigints[] = {
	SIGUSR1,
	SIGUSR2,
	SIGINT,
	SIGTERM,
	0
} ; /* end array */

enum argopts {
	argopt_version,
	argopt_root,
	argopt_tmpdir,
	argopt_help,
	argopt_term,
	argopt_editor,
	argopt_sn,
	argopt_af,
	argopt_ef,
	argopt_of,
	argopt_if,
	argopt_nf,
	argopt_keyboard,
	argopt_kbd,
	argopt_titles,
	argopt_oldtitles,
	argopt_count,
	argopt_new,
	argopt_old,
	argopt_all,
	argopt_every,
	argopt_names, /* 18 */
	argopt_boards,
	argopt_interactive,
	argopt_nopage,
	argopt_articleid,
	argopt_mailbox,
	argopt_newsgroups, /* 24 */
	argopt_reverse,
	argopt_subject,
	argopt_date,
	argopt_from,
	argopt_msgid1,
	argopt_msgid2,
	argopt_mailer,
	argopt_metamail,
	argopt_catchup,
	argopt_query,
	argopt_subscribe,
	argopt_unsubscribe,
	argopt_description,
	argopt_bbpost,
	argopt_pager,
	argopt_sort,
	argopt_overlast
} ; /* end enum */

constexpr cpcchar	argopts[] = {
	"VERSION",
	"ROOT",
	"TMPDIR",
	"HELP",
	"TERM",
	"EDITOR",
	"sn",
	"af",
	"ef",
	"of",
	"if",
	"nf",
	"keyboard",
	"kbd",
	"titles",
	"old_titles",
	"count",
	"new",
	"old",
	"all",
	"every",
	"names", /* 18 */
	"boards",
	"interactive",
	"nopage",
	"article-id",
	"mailbox",
	"newsgroups", /* 24 */
	"reverse",
	"subjects",
	"date",
	"from",
	"message-id",
	"msgid",
	"mailer",
	"METAMAIL",
	"catchup",
	"query",
	"subscribe",
	"unsubscribe",
	"description",
	"BBPOST",
	"PAGER",
	"sort",
	nullptr
} ; /* end array */

constexpr pivars	initvars = {
	VARPROGRAMROOT1,
	VARPROGRAMROOT2,
	VARPROGRAMROOT3,
	PROGRAMROOT,
	VARPRNAME
} ; /* end array */

constexpr mapex		mapexs[] = {
	{ SR_NOENT, EX_NOUSER },
	{ SR_AGAIN, EX_TEMPFAIL },
	{ SR_DEADLK, EX_TEMPFAIL },
	{ SR_NOLCK, EX_TEMPFAIL },
	{ SR_TXTBSY, EX_TEMPFAIL },
	{ SR_ACCESS, EX_NOPERM },
	{ SR_REMOTE, EX_PROTOCOL },
	{ SR_NOSPC, EX_TEMPFAIL },
	{ 0, 0 }
} ; /* end array */

enum akonames {
	akoname_query,
	akoname_test,
	akoname_term,
	akoname_useclen,
	akoname_useclines,
	akoname_overlast
} ; /* end enum */

constexpr cpcchar	akonames[] = {
	"query",
	"test",
	"term",
	"useclen",
	"useclines",
	nullptr
} ; /* end array */

#if	CF_ANSITERMS
/* pop screen terminal types (this using prefix matching!) */
constexpr cpcchar	ansiterms[] = {
	"ansi",
	"vt100",
	"vt101",
	"vt102",
	"vt220",
	"vt320",
	"vt420",
	"vt520",
	"vt620",
	"xterm",
	"screen",
	nullptr
} ; /* end array */
#endif /* CF_ANSITERMS */

constexpr cpcchar	sortmodes[] = {
	"modify",
	"arrive",
	"post",
	"compose",
	"spool",
	"now",
	nullptr
} ; /* end array */

constexpr cpcchar	progmodes[] = {
	"read",
	"header",
	"names",
	"count",
	"subscription",
	"mailbox",
	nullptr
} ; /* end array */

#ifndef	PM_READ
#define	PM_READ		0
#define	PM_HEADER	1
#define	PM_NAMES	2
#define	PM_COUNT	3
#define	PM_SUBSCRIPTION	4
#define	PM_OVERLAST	5
#endif

constexpr char		aterms[] = {
	0x00, 0x2E, 0x00, 0x00,
	0x09, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ; /* end array */

static bufsizevar	zalen(bufsize_zn) ;


/* exported variables */


/* exported subroutines */

int main(int argc,mainv argv,mainv envv) noex {
	PROGINFO	pi, *pip = &pi ;
	ARGINFO		ainfo ;
	SIGHAND		sm ;
	bits		pargs ;
	keyopt		akopts ;
	bfile		errfile ;
	bfile		outfile, *ofp = &outfile ;
	bfile		infile, *ifp = &infile ;

#if	(CF_DEBUGS || CF_DEBUG) && CF_DEBUGMALL
	uint		mo_start = 0 ;
#endif

	int		argr, argl, aol, akl, avl, kwi ;
	int		ai, ai_max, ai_pos ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		cl ;
	int		ex = EX_INFO ;
	int		f_optminus, f_optplus, f_optequal ;
	int		f_usage = false ;
	int		f_version = false ;
	int		f_help = false ;

	cchar		*argp, *aop, *akp, *avp ;
	cchar		*argval = nullptr ;
	cchar		*pr = nullptr ;
	cchar		*sn = nullptr ;
	cchar		*afname = nullptr ;
	cchar		*efname = nullptr ;
	cchar		*ofname = nullptr ;
	cchar		*cfname = nullptr ;
	cchar		*ufname = nullptr ;
	cchar		*sortmode = nullptr ;
	cchar		*cp ;


	if_exit = 0 ;
	if_int = 0 ;

	rs = sighand_start(&sm,sigblocks,sigignores,sigints,main_sighand) ;
	if (rs < 0) goto badsighand ;

#if	CF_DEBUGS || CF_DEBUG
	if ((cp = getourenv(envv,VARDEBUGFNAME)) != nullptr) {
	    rs = debugopen(cp) ;
	    debugprintf("main: starting DFD=%d\n",rs) ;
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

	if ((cp = getenv(VARBANNER)) == nullptr) cp = BANNER ;
	proginfo_setbanner(pip,cp) ;

/* early things to initialize */

	pip->ofp = ofp ;
	pip->ifp = ifp ;

/* initialize some stuff before command line argument processing */

	if (pip->progname[0] == 'n')
	    pip->fl.newprogram = true ;

	pip->verboselevel = 1 ;
	pip->progmode = -1 ;
	pip->sortmode = 0 ;		/* default sort mode */
	pip->termlines = 0 ;
	pip->showlines = 0 ;
	pip->whichenvdate = SORTMODE_NOW ;
	pip->daytime = time(nullptr) ;

	pip->termtype = getenv(VARTERM) ;

	pip->prog_editor = PROG_EDITOR ;
	pip->prog_mailer = PROG_MAILER ;
	pip->prog_metamail = PROG_METAMAIL ;
	pip->prog_bbpost = PROG_PRT ;
	pip->prog_pager = nullptr ;

	pip->to_config = TO_CONFIG ;
	pip->to_read = TO_READ ;
	pip->to_info = TO_INFO ;
	pip->to_clock = TO_CLOCK ;

	{
	    const int	f = OPT_FASTSCAN ;
	    pip->fl.extrascan = (! f) ;
	    pip->fl.readtime = true ;
	    pip->fl.interactive = true ;
	    pip->fl.popscreen = true ;
	    pip->fl.logprog = true ;
	}

/* process program arguments */

	if (rs >= 0) rs = bits_start(&pargs,1) ;
	if (rs < 0) goto badpargs ;

	rs = keyopt_start(&akopts) ;
	pip->open.akopts = (rs >= 0) ;

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
		const int	ach = MKCHAR(argp[1]) ;

	        if (isdigitlatin(ach)) {

	            argval = (argp + 1) ;

	        } else if (ach == '-') {

	            ai_pos = ai ;
	            break ;

	        } else {

	            aop = argp + 1 ;
	            akp = aop ;
	            aol = argl - 1 ;
	            f_optequal = false ;
	            if ((avp = strchr(aop,'=')) != nullptr) {
	                akl = avp - aop ;
	                avp += 1 ;
	                avl = aop + argl - 1 - avp ;
	                aol = akl ;
	                f_optequal = true ;
	            } else {
	                avl = 0 ;
	                akl = aol ;
	            }

	            if ((kwi = matostr(argopts,2,akp,akl)) >= 0) {

	                switch (kwi) {

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

	                case argopt_tmpdir:
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl)
	                            pip->tmpdname = avp ;
	                    } else {
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                pip->tmpdname = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                    }
	                    break ;

/* version */
	                case argopt_version:
	                    f_version = true ;
	                    if (f_optequal)
	                        rs = SR_INVALID ;
	                    break ;

/* help */
	                case argopt_help:
	                    f_help = true ;
	                    break ;

	                case argopt_term:
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl)
	                            pip->termtype = avp ;
	                    } else {
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                pip->termtype = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                    }
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

/* argument list-file */
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

/* output-file */
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

/* user-news file */
	                case argopt_nf:
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl)
	                            ufname = avp ;
	                    } else {
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                ufname = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                    }
	                    break ;

	                case argopt_keyboard:
	                case argopt_kbd:
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl)
	                            pip->kbdtype = avp ;
	                    } else {
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                pip->kbdtype = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                    }
	                    break ;

/* "date" */
	                case argopt_date:
	                    if (pip->progmode < 0) {
	                        pip->progmode = PM_HEADER ;
	                        pip->header = HI_DATE ;
	                    }
	                    break ;

/* "from" */
	                case argopt_from:
	                    if (pip->progmode < 0) {
	                        pip->progmode = PM_HEADER ;
	                        pip->header = HI_FROM ;
	                    }
	                    break ;

/* "subject" and the old "titles" */
	                case argopt_titles:
	                case argopt_subject:
	                    if (pip->progmode < 0) {
	                        pip->progmode = PM_HEADER ;
	                        pip->header = HI_SUBJECT ;
	                    }
	                    break ;

	                case argopt_msgid1:
	                case argopt_msgid2:
	                    if (pip->progmode < 0) {
	                        pip->progmode = PM_HEADER ;
	                        pip->header = HI_MSGID ;
	                    }
	                    break ;

/* "old_titles" */
	                case argopt_oldtitles:
	                    if (f_optplus) {
	                        if (pip->progmode < 0) {
	                            pip->progmode = PM_HEADER ;
	                            pip->fl.all = true ;
	                        }
	                    } else
	                        pip->fl.old = true ;
	                    break ;

/* article-id */
	                case argopt_articleid:
	                    if (pip->progmode < 0) {
	                        pip->progmode = PM_HEADER ;
	                        pip->header = HI_ARTICLEID ;
	                    }
	                    break ;

/* "count" */
	                case argopt_count:
	                    if (pip->progmode < 0) {
	                        pip->progmode = PM_COUNT ;
	                    } else if ((pip->progmode == PM_NAMES) &&
	                        pip->fl.description)
	                        pip->progmode = PM_COUNT ;
	                    break ;

/* "query" */
	                case argopt_query:
	                    pip->fl.query = true ;
	                    if (pip->progmode < 0)
	                        pip->progmode = PM_COUNT ;
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl)
	                            pip->querytext = avp ;
	                    }
	                    break ;

/* "every_board" */
	                case argopt_new:
	                    pip->fl.new = true ;
	                    break ;

/* "old_board" */
	                case argopt_old:
	                    pip->fl.old = true ;
	                    break ;

/* "all_bulletins" */
	                case argopt_all:
	                    pip->fl.all = true ;
	                    break ;

/* "every_board" */
	                case argopt_every:
	                    pip->fl.every = true ;
	                    break ;

/* "names" */
/* "boards" */
	                case argopt_names:
	                case argopt_boards:
	                case argopt_newsgroups:
	                    if (pip->progmode < 0)
	                        pip->progmode = PM_NAMES ;
	                    break ;

/* "reverse" */
	                case argopt_reverse:
	                    pip->fl.reverse = true ;
	                    break ;

/* "interactive" */
	                case argopt_interactive:
	                    pip->fl.interactive = false ;
	                    if (f_optplus)
	                        pip->fl.interactive = true ;
	                    break ;

/* "nopage" */
	                case argopt_nopage:
	                    pip->fl.nopage = false ;
	                    if (f_optplus)
	                        pip->fl.nopage = true ;
	                    break ;

/* mailbox option for BBR (and smart users) */
	                case argopt_mailbox:
	                    pip->progmode = PM_MAILBOX ;
	                    pip->fl.mailbox = true ;
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl)
	                            pip->mailbox = avp ;
	                    }
	                    break ;

/* catchup option (really quite similar to the "mailbox" option) */
	                case argopt_catchup:
	                    pip->progmode = PM_READ ;
	                    pip->fl.catchup = true ;
	                    break ;

/* subscription changes */
	                case argopt_subscribe:
	                    pip->progmode = PM_SUBSCRIPTION ;
	                    pip->fl.subscribe = true ;
	                    break ;

/* subscription changes */
	                case argopt_unsubscribe:
	                    pip->progmode = PM_SUBSCRIPTION ;
	                    pip->fl.subscribe = false ;
	                    break ;

/* print out newsgroup descriptions */
	                case argopt_description:
	                    if (pip->progmode < 0)
	                        pip->progmode = PM_NAMES ;
	                    pip->fl.description = true ;
	                    break ;

/* editor program */
	                case argopt_editor:
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl)
	                            pip->prog_editor = avp ;
	                    } else {
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                pip->prog_editor = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                    }
	                    break ;

/* mailer program */
	                case argopt_mailer:
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl)
	                            pip->prog_mailer = avp ;
	                    } else {
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                pip->prog_mailer = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                    }
	                    break ;

/* metamail program */
	                case argopt_metamail:
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl)
	                            pip->prog_metamail = avp ;
	                    } else {
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                pip->prog_metamail = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                    }
	                    break ;

/* BBPOST program */
	                case argopt_bbpost:
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl)
	                            pip->prog_bbpost = avp ;
	                    } else {
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                pip->prog_bbpost = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                    }
	                    break ;

/* PAGER program */
	                case argopt_pager:
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl)
	                            pip->prog_pager = avp ;
	                    } else {
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                pip->prog_pager = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                    }
	                    break ;

/* sort mode */
	                case argopt_sort:
	                    if (f_optequal) {
	                        f_optequal = false ;
	                        if (avl)
	                            sortmode = avp ;
	                    } else {
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                sortmode = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                    }
	                    break ;

/* default action and user specified help */
	                default:
	                    rs = SR_INVALID ;
	                    break ;

	                } /* end switch (key words) */

	            } else {

	                while (akl--) {
	                    const int	kc = MKCHAR(*akp) ;

	                    switch (kc) {

	                    case 'C':
	                        break ;

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

/* editor program */
	                    case 'E':
	                        if (f_optequal) {
	                            f_optequal = false ;
	                            if (avl)
	                                pip->prog_editor = avp ;
	                        } else {
	                            if (argr > 0) {
	                                argp = argv[++ai] ;
	                                argr -= 1 ;
	                                argl = strlen(argp) ;
	                                if (argl)
	                                    pip->prog_editor = argp ;
	                            } else
	                                rs = SR_INVALID ;
	                        }
	                        break ;

/* terminal lines */
	                    case 'L':
	                        cp = nullptr ;
	                        cl = -1 ;
	                        if (f_optequal) {
	                            f_optequal = false ;
	                            cp = avp ;
	                            cl = avl ;
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
	                            rs = optvalue(cp,cl) ;
	                            pip->termlines = rs ;
	                        }
	                        break ;

/* mailer program */
	                    case 'M':
	                        if (f_optequal) {
	                            f_optequal = false ;
	                            if (avl)
	                                pip->prog_mailer = avp ;
	                        } else {
	                            if (argr > 0) {
	                                argp = argv[++ai] ;
	                                argr -= 1 ;
	                                argl = strlen(argp) ;
	                                if (argl)
	                                    pip->prog_mailer = argp ;
	                            } else
	                                rs = SR_INVALID ;
	                        }
	                        break ;

/* alternate newsgroup spool area */
	                    case 'N':
	                        if (f_optequal) {
	                            f_optequal = false ;
	                            if (avl)
	                                pip->newsdname = avp ;
	                        } else {
	                            if (argr > 0) {
	                                argp = argv[++ai] ;
	                                argr -= 1 ;
	                                argl = strlen(argp) ;
	                                if (argl)
	                                    pip->newsdname = argp ;
	                            } else
	                                rs = SR_INVALID ;
	                        }
	                        break ;

	                    case 'T':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                pip->termtype = argp ;
	                        } else
	                            rs = SR_INVALID ;
	                        break ;

	                    case 'V':
	                        f_version = true ;
	                        break ;

/* all articles (old and new) */
	                    case 'a':
	                        pip->fl.all = true ;
	                        break ;

/* count */
	                    case 'c':
	                        pip->fl.every = true ;
	                        break ;

/* every newsgroup */
	                    case 'e':
	                        pip->fl.every = true ;
	                        break ;

/* options */
	                    case 'o':
	                        if (argr > 0) {
	                            argp = argv[++ai] ;
	                            argr -= 1 ;
	                            argl = strlen(argp) ;
	                            if (argl)
	                                rs = keyopt_loads(&akopts,argp,argl) ;
	                        } else
	                            rs = SR_INVALID ;
	                        break ;

/* reverse */
	                    case 'r':
	                        pip->fl.reverse = true ;
	                        break ;

/* subject */
	                    case 's':
	                        pip->fl.every = true ;
	                        break ;

/* user's newsgroup list file */
	                    case 'u':
	                        if (f_optequal) {
	                            f_optequal = false ;
	                            if (avl)
	                                ufname = avp ;
	                        } else {
	                            if (argr > 0) {
	                                argp = argv[++ai] ;
	                                argr -= 1 ;
	                                argl = strlen(argp) ;
	                                if (argl)
	                                    ufname = argp ;
	                            } else
	                                rs = SR_INVALID ;
	                        }
	                        break ;

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

	        } /* end if (digit or not) */

	    } else {

	        rs = bits_set(&pargs,ai) ;
	        ai_max = ai ;

	    } /* end if (key letter/word or positional) */

	    ai_pos = ai ;

	} /* end while (all command line argument processing) */

	if (efname == nullptr) efname = getenv(VAREFNAME) ;
	if (efname == nullptr) efname = BFILE_STDERR ;
	if ((rs1 = bopen(&errfile,efname,"wca",0666)) >= 0) {
	    pip->efp = &errfile ;
	    pip->open.errfile = true ;
	    bcontrol(&errfile,BC_SETBUFLINE,true) ;
	} else if (! isFailOpen(rs1)) {
	    if (rs >= 0) rs = rs1 ;
	}

	if (rs < 0)
	    goto badarg ;

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("main: debuglevel=%u\n",pip->debuglevel) ;
#endif

	if (f_version) {
	    bprintf(pip->efp,"%s: version %s/%s\n",
	        pip->progname,
	        VERSION,(pip->fl.sysv_ct ? "SYSV" : "BSD")) ;
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

#if	CF_DEBUG
	if (DEBUGLEVEL(2)) {
	    debugprintf("main: pr=%s\n",pip->pr) ;
	    debugprintf("main: sn=%s\n",pip->searchname) ;
	}
#endif

	if (pip->debuglevel > 0) {
	    bprintf(pip->efp,"%s: pr=%s\n",pip->progname,pip->pr) ;
	    bprintf(pip->efp,"%s: sn=%s\n",pip->progname,pip->searchname) ;
	}

	if (f_usage)
	    usage(pip) ;

	if (f_help)
	    printhelp(nullptr,pip->pr,pip->searchname,HELPFNAME) ;

	if (f_version || f_help || f_usage)
	    goto retearly ;


	ex = EX_OK ;

/* some initialization */

	if ((rs >= 0) && (pip->n == 0) && (argval != nullptr)) {
	    rs = optvalue(argval,-1) ;
	    pip->n = rs ;
	}

	if (afname == nullptr) afname = getenv(VARAFNAME) ;

	if (ufname == nullptr) ufname = getenv(VARNEWSRC) ;
	if (ufname == nullptr) ufname = getenv(VARBBNEWSRC) ;

	if (pip->tmpdname == nullptr) pip->tmpdname = getenv(VARTMPDNAME) ;
	if (pip->tmpdname == nullptr) pip->tmpdname = TMPDNAME ;

	if (pip->prog_pager == nullptr) pip->prog_pager = getenv(VARPAGER) ;
	if (pip->prog_pager == nullptr) pip->prog_pager = PROG_PAGER ;

	if (pip->querytext == nullptr) pip->querytext = getenv(VARQUERYTEXT) ;
	if (pip->querytext == nullptr) pip->querytext = QUERYTEXT ;

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("main: progmode=%d\n",pip->progmode) ;
#endif

	if (rs >= 0) {
	    if ((rs = zalen) >= 0) {
	        if ((rs = initnow(&pip->now,pip->zname,rs)) >= 0) {
	            if (( rs = procopts(pip,&akopts)) >= 0) {
#if	CF_CHECKONC
		        rs = checkonc(pip->pr,nullptr,nullptr,nullptr) ;
		        pip->fl.onckey = (rs >= 0) ;
#else
		        rs = 1 ;
#endif /* CF_CHECKONC */
		    }
	        }
	    } /* end if (zalen) */
	} /* end if (ok) */

	if ((pip->progmode < 0) && pip->fl.test)
	    pip->progmode = PM_TEST ;

	if (pip->progmode < 0)
	    pip->progmode = PM_READ ;

	if (pip->debuglevel > 0) {
	    cint	pm = pip->progmode ;
	    cchar	*pn = pip->progname ;
	    cchar	*pms = "" ;
	    if (pm < PM_OVERLAST) pms = progmodes[pm] ;
	    bprintf(pip->efp,"%s: program mode=%s(%d)\n",pn,pms,pm) ;
	}

	if ((sortmode != nullptr) && (sortmode[0] != '\0')) {
	    if ((rs1 = matostr(sortmodes,2,sortmode,-1)) >= 0) {
	        if (rs1 >= 4) rs1 = SORTMODE_MTIME ;
	        pip->sortmode = rs1 ;
	    }
	} /* end if (sort mode) */

	if ((rs >= 0) && (! pip->have.mailcheck)) {
	    if ((cp = getenv(VARMAILCHECK)) == nullptr) {
	        cp = getenv(VARPMAILCHECK) ;
	    }
	    if (cp != nullptr) {
	        pip->have.mailcheck = true ;
	        rs = cfdecti(cp,-1,&pip->to_mailcheck) ;
	    }
	    if (pip->debuglevel > 0) {
	        cchar	*fmt = "%s: mailcheck=disabled\n" ;
	        if (pip->to_mailcheck > 0) fmt = "%s: mailcheck=%u\n" ;
	        bprintf(pip->efp,fmt,pip->progname,pip->to_mailcheck) ;
	    }
	}

/* go arguments */

	memset(&ainfo,0,sizeof(ARGINFO)) ;
	ainfo.argc = argc ;
	ainfo.argv = argv ;
	ainfo.ai_max = ai_max ;
	ainfo.ai_pos = ai_pos ;

	if (rs >= 0) {
	    userinfo	u ;
	    if ((rs = userinfo_start(&u,nullptr)) >= 0) {
	        if ((rs = procuserinfo_begin(pip,&u)) >= 0) {
	            PCSCONF	pc, *pcp = &pc ;
		    cchar	*pr = pip->pr ;
	            if (cfname != nullptr) {
	                if (pip->euid != pip->uid) u_seteuid(pip->uid) ;
	                if (pip->egid != pip->gid) u_setegid(pip->gid) ;
	            }
	            if ((rs = pcsconf_start(pcp,pr,envv,cfname)) >= 0) {
	                pip->pcsconf = pcp ;
	                pip->open.pcsconf = true ;
	                if ((rs = procpcsconf_begin(pip,pcp)) >= 0) {
	                    PCSPOLL	poll ;
			    cchar	*sn = pip->searchname ;
	                    if ((rs = pcspoll_start(&poll,pcp,sn)) >= 0) {
	                        if ((rs = proglog_begin(pip,&u)) >= 0) {
	                            if ((rs = proguserlist_begin(pip)) >= 0) {
				        ARGINFO	*aip = &ainfo ;
				        bits	*bop = &pargs ;
	                                cchar	*afn = afname ;
	                                cchar	*ofn = ofname ;
	                                cchar	*ufn = ufname ;
	                                rs = process(pip,aip,bop,afn,ofn,ufn) ;
	                                rs1 = proguserlist_end(pip) ;
	                                if (rs >= 0) rs = rs1 ;
	                            } /* end if (proguserlist) */
	                            rs1 = proglog_end(pip) ;
	                            if (rs >= 0) rs = rs1 ;
	                        } /* end if (proglog) */
	                        rs1 = pcspoll_finish(&poll) ;
	                        if (rs >= 0) rs = rs1 ;
	                    } /* end if (pcspoll) */
	                    rs1 = procpcsconf_end(pip) ;
	                    if (rs >= 0) rs = rs1 ;
	                } /* end if (procpcsconf) */
	                pip->open.pcsconf = false ;
	                pip->pcsconf = nullptr ;
	                rs1 = pcsconf_finish(pcp) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (pcsconf) */
	            rs1 = procuserinfo_end(pip) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (procuserinfo) */
	        rs1 = userinfo_finish(&u) ;
	        if (rs >= 0) rs = rs1 ;
	    } else {
		cchar	*pn = pip->progname ;
		cchar	*fmt ;
	        ex = EX_NOUSER ;
	        fmt = "%s: userinfo failure (%d)\n" ;
	        bprintf(pip->efp,fmt,pn,rs) ;
	    } /* end if (userinfo) */
	} else if (ex == EX_OK) {
	    cchar	*pn = pip->progname ;
	    cchar	*fmt = "%s: invalid argument or configuration (%d)\n" ;
	    ex = EX_USAGE ;
	    bprintf(pip->efp,fmt,pn,rs) ;
	    usage(pip) ;
	}

/* done */
	if ((rs < 0) && (ex == EX_OK)) {
	    cchar	*pn = pip->progname ;
	    switch (rs) {
	    case SR_PIPE:
		break ;
	    default:
	        if (! pip->fl.quiet) {
	            bprintf(pip->efp,"%s: could not process (%d)\n",pn,rs) ;
	        }
		break ;
	    } /* end switch */
	    ex = mapex(mapexs,rs) ;
	} /* end if */

retearly:
	if (pip->debuglevel > 0) {
	    bprintf(pip->efp,"%s: exiting ex=%u (%d)\n",pip->progname,ex,rs) ;
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	    debugprintf("main: exiting ex=%u (%d)\n",ex,rs) ;
#endif

	if (pip->efp != nullptr) {
	    pip->open.errfile = false ;
	    bclose(pip->efp) ;
	    pip->efp = nullptr ;
	}

	if (pip->open.akopts) {
	    pip->open.akopts = false ;
	    keyopt_finish(&akopts) ;
	}

	bits_finish(&pargs) ;

badpargs:
	proginfo_finish(pip) ;

badprogstart:

#if	(CF_DEBUGS || CF_DEBUG) && CF_DEBUGMALL
	{
	    uint	mi[12] ;
	    uint	mo ;
	    uint	mdiff ;
	    uc_mallout(&mo) ;
	    mdiff = (mo-mo_start) ;
	    debugprintf("main: final mallout=%u\n",mdiff) ;
	    if (mdiff > 0) {
	        UCMALLREG_CUR	cur ;
	        UCMALLREG_REG	reg ;
	        const int	size = (10*sizeof(uint)) ;
	        cchar		*ids = "main" ;
	        uc_mallinfo(mi,size) ;
	        debugprintf("main: MIoutnum=%u\n",mi[ucmallreg_outnum]) ;
	        debugprintf("main: MIoutnummax=%u\n",mi[ucmallreg_outnummax]) ;
	        debugprintf("main: MIoutsize=%u\n",mi[ucmallreg_outsize]) ;
	        debugprintf("main: MIoutsizemax=%u\n",
	            mi[ucmallreg_outsizemax]) ;
	        debugprintf("main: MIused=%u\n",mi[ucmallreg_used]) ;
	        debugprintf("main: MIusedmax=%u\n",mi[ucmallreg_usedmax]) ;
	        debugprintf("main: MIunder=%u\n",mi[ucmallreg_under]) ;
	        debugprintf("main: MIover=%u\n",mi[ucmallreg_over]) ;
	        debugprintf("main: MInotalloc=%u\n",mi[ucmallreg_notalloc]) ;
	        debugprintf("main: MInotfree=%u\n",mi[ucmallreg_notfree]) ;
	        ucmallreg_curbegin(&cur) ;
	        while (ucmallreg_curenum(&cur,&reg) >= 0) {
	            debugprintf("main: MIreg.addr=%p\n",reg.addr) ;
	            debugprintf("main: MIreg.size=%u\n",reg.size) ;
	            debugprinthexblock(ids,80,reg.addr,reg.size) ;
	        }
	        ucmallreg_curend(&cur) ;
	    }
	    uc_mallset(0) ;
	}
#endif /* CF_DEBUGMALL */

#if	(CF_DEBUGS || CF_DEBUG)
	debugclose() ;
#endif

	sighand_finish(&sm) ;

badsighand:
	return ex ;

/* argument errors */
badarg:
	ex = EX_USAGE ;
	bprintf(pip->efp,"%s: invalid argument specified (%d)\n",
	    pip->progname,rs) ;
	usage(pip) ;
	goto retearly ;

}
/* end subroutine (main) */

int progsig(PI *pip) noex {
	int		rs = SR_FAULT ;
	if (pip) {
	    if (if_exit) {
	        rs = SR_EXIT ;
	    } else if (if_int) {
	        rs = SR_INTR ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (progsig) */


/* local subroutines */

/* ARGSUSED */
local void main_sighand(int sn,siginfo_t *sip,void *vcp) noex {
	switch (sn) {
	case SIGINT:
	    if_int = true ;
	    break ;
	default:
	    if_exit = true ;
	    break ;
	} /* end switch */
}
/* end subroutine (main_sighand) */

local int usage(PI *pip) noex {
	int		rs = SR_OK ;
	int		wlen = 0 ;
	cchar		*pn = pip->progname ;
	cchar		*fmt ;

	fmt = "%s: USAGE> %s [<newsgroup(s)> ...] "
	    "[-newsgroups | -subject | -count |\n" ;
	if (rs >= 0) rs = bprintf(pip->efp,fmt,pn,pn) ;
	wlen += rs ;

	fmt = "  -description | -subscribe | -unsubscribe |\n" ;
	if (rs >= 0) rs = bprintf(pip->efp,fmt,pn) ;
	wlen += rs ;

	fmt = "  -from | -date | -catchup]\n" ;
	if (rs >= 0) rs = bprintf(pip->efp,fmt,pn) ;
	wlen += rs ;

	fmt = "  [-a | -o] [-e] [-interactive] [-mailbox]\n" ;
	if (rs >= 0) rs = bprintf(pip->efp,fmt,pn) ;
	wlen += rs ;

	fmt = "  [-L <termlines>] [-V?] [-N <newsgroup_spool_dir>]\n" ;
	if (rs >= 0) rs = bprintf(pip->efp,fmt,pn) ;
	wlen += rs ;

	fmt = "  [-u <user_newsgroup_list_file>]\n" ;
	if (rs >= 0) rs = bprintf(pip->efp,fmt,pn) ;
	wlen += rs ;

	return rs ;
}
/* end subroutine (usage) */

/* process the program ako-options */
local int procopts(PI *pip,keyopt *kop) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	cchar		*cp ;

	if ((cp = getourenv(pip->envv,VAROPTS)) != nullptr) {
	    rs = keyopt_loads(kop,cp,-1) ;
	}

	if (rs >= 0) {
	    keyopt_cur	kcur ;
	    if ((rs = keyopt_curbegin(kop,&kcur)) >= 0) {
	        int	oi ;
	        int	kl, vl ;
	        cchar	*kp, *vp ;

	        while ((kl = keyopt_enumkeys(kop,&kcur,&kp)) >= 0) {

	            if ((oi = matostr(akonames,2,kp,kl)) >= 0) {

	                vl = keyopt_fetch(kop,kp,nullptr,&vp) ;

	                switch (oi) {
	                case akoname_query:
	                    if (! pip->final.query) {
	                        pip->have.query = true ;
	                        pip->final.query = true ;
	                        pip->fl.query = true ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            pip->fl.query = (rs > 0) ;
	                        }
	                    }
	                    break ;
	                case akoname_test:
	                    if (! pip->final.test) {
	                        pip->have.test = true ;
	                        pip->final.test = true ;
	                        pip->fl.test = true ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            pip->fl.test = (rs > 0) ;
	                        }
	                    }
	                    break ;
	                case akoname_term:
	                    if (! pip->final.term) {
	                        pip->have.term = true ;
	                        pip->final.term = true ;
	                        pip->fl.term = true ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            pip->fl.term = (rs > 0) ;
	                        }
	                    }
	                    break ;
	                case akoname_useclen:
	                    if (! pip->final.useclen) {
	                        pip->have.useclen = true ;
	                        pip->final.useclen = true ;
	                        pip->fl.useclen = true ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            pip->fl.useclen = (rs > 0) ;
	                        }
	                    }
	                    break ;
	                case akoname_useclines:
	                    if (! pip->final.useclines) {
	                        pip->have.useclines = true ;
	                        pip->final.useclines = true ;
	                        pip->fl.useclines = true ;
	                        if (vl > 0) {
	                            rs = optbool(vp,vl) ;
	                            pip->fl.useclines = (rs > 0) ;
	                        }
	                    }
	                    break ;
	                default:
	                    rs = SR_INVALID ;
	                    break ;
	                } /* end switch */

	                c += 1 ;
	            } /* end if (valid option) */

	            if (rs < 0) break ;
	        } /* end while (looping through key options) */

	        keyopt_curend(kop,&kcur) ;
	    } /* end if (keyopt-cur) */
	} /* end if (ok) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (procopts) */

local int procuserinfo_begin(PI *pip,userinfo *uip) noex {
	int		rs = SR_OK ;

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	   debugprintf("procuserinfo_begin: ent\n") ;
#endif

	pip->usysname = uip->sysname ;
	pip->urelease = uip->release ;
	pip->uversion = uip->version ;
	pip->umachine = uip->machine ;
	pip->nodename = uip->nodename ;
	pip->domainname = uip->domainname ;
	pip->username = uip->username ;
	pip->gecosname = uip->gecosname ;
	pip->homedname = uip->homedname ;
	pip->realname = uip->realname ;
	pip->name = uip->name ;
	pip->fullname = uip->fullname ;
	pip->mailname = uip->mailname ;
	pip->org = uip->organization ;
	pip->tz = uip->tz ;
	pip->logid = uip->logid ;

	pip->pid = uip->pid ;
	pip->uid = uip->uid ;
	pip->euid = uip->euid ;
	pip->gid = uip->gid ;
	pip->egid = uip->egid ;

	if (rs >= 0) {
	    const int	hlen = MAXHOSTNAMELEN ;
	    char	hbuf[MAXHOSTNAMELEN+1] ;
	    cchar	*nn = pip->nodename ;
	    cchar	*dn = pip->domainname ;
	    if ((rs = snsds(hbuf,hlen,nn,dn)) >= 0) {
	        cchar	**vpp = &pip->hostname ;
	        rs = proginfo_setentry(pip,vpp,hbuf,rs) ;
	    }
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	   debugprintf("procuserinfo_begin: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (procuserinfo_begin) */

local int procuserinfo_end(PI *pip) noex {
	int		rs = SR_OK ;

	if (pip == nullptr) return SR_FAULT ;

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	   debugprintf("procuserinfo_end: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (procuserinfo_end) */

local int procpcsconf_begin(PI *pip,PCSCONF *pcp) noex {
	int		rs = SR_OK ;
	if (pip->open.pcsconf) {
	    rs = 1 ;

#if	CF_DEBUG
	    if (DEBUGLEVEL(3)) {
	        PCSCONF_CUR	cur ;
	        int		c = 0 ;
	        if ((rs = pcsconf_curbegin(pcp,&cur)) >= 0) {
	            const int	klen = KBUFLEN ;
	            const int	vlen = VBUFLEN ;
	            int		vl ;
	            char	kbuf[KBUFLEN+1] ;
	            char	vbuf[VBUFLEN+1] ;
	            while (rs >= 0) {
	                vl = pcsconf_enum(pcp,&cur,kbuf,klen,vbuf,vlen) ;
	                if (vl == SR_NOTFOUND) break ;
	                c += 1 ;
	                debugprintf("main/procpcsconf: pair> %s=%r\n",
	                    kbuf,vbuf,vl) ;
	            } /* end while */
	            pcsconf_curend(pcp,&cur) ;
	        } /* end if (cursor) */
	        debugprintf("main/procpcsconf: pairs=%u\n",c) ;
	    }
#endif /* CF_DEBUG */

	} /* end if (configured) */

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	   debugprintf("procpcsconf_begin: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (procpcsconf_begin) */

local int procpcsconf_end(PI *pip) noex {
	int		rs = SR_OK ;

	if (pip == nullptr) return SR_FAULT ;

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	   debugprintf("procpcsconf_end: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (procpcsconf_end) */

local int process(PI *pip,ARGINFO *aip,bits *bop,
		cchar *ofn,cchar *afn,cchar *ufn) noex {
	int		rs ;
	if ((rs = progufname(pip,ufn)) >= 0) {
	    rs = procargs(pip,aip,bop,afn,ofn) ;
	}
	return rs ;
}
/* end subroutine (process) */

local int procargs(PI *pip,ARGINFO *aip,bits *bop,cc *afn,cc *ofn) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (vecpstr as ; (rs = as.start(5,0,0)) >= 0) {
	    int		pan = 0 ;
	    int		cl ;
	    cchar	*cp ;

	    if (rs >= 0) {
	        int	ai ;
	        int	f ;
	        cchar	**argv = aip->argv ;
	        for (ai = 1 ; ai < aip->argc ; ai += 1) {

	            f = (ai <= aip->ai_max) && (bits_test(bop,ai) > 0) ;
	            f = f || ((ai > aip->ai_pos) && (argv[ai] != nullptr)) ;
	            if (f) {
	                cp = argv[ai] ;
	                if (cp[0] != '\0') {
	                    pan += 1 ;
	                    rs = vecpstr_adduniq(&as,cp,-1) ;
	                    if (rs < INT_MAX) c += 1 ;
	                }
	            }

	            if (rs < 0) break ;
	        } /* end for */
	    } /* end if (ok) */

	    if ((rs >= 0) && (afn != nullptr) && (afn[0] != '\0')) {
	        bfile	afile, *afp = &afile ;

	        if (strcmp(afn,"-") == 0) afn = BFILE_STDIN ;

	        if ((rs = bopen(afp,afn,"r",0666)) >= 0) {
	            const int	llen = LINEBUFLEN ;
	            int		len ;
	            char	lbuf[LINEBUFLEN + 1] ;

	            while ((rs = breadln(afp,lbuf,llen)) > 0) {
	                len = rs ;

	                if (lbuf[len - 1] == '\n') len -= 1 ;
	                lbuf[len] = '\0' ;

	                if ((cl = sfskipwhite(lbuf,len,&cp)) > 0) {
	                    if (cp[0] != '#') {
	                        pan += 1 ;
	                        rs = procspecs(pip,&as,cp,cl) ;
	                        c += rs ;
	                    }
	                }

	                if (rs < 0) break ;
	            } /* end while (reading lines) */

	            rs1 = bclose(afp) ;
	            if (rs >= 0) rs = rs1 ;
	        } else {
	            if (! pip->fl.quiet) {
			cchar	*pn = pip->progname ;
			cchar	*fmt ;
			fmt = "%s: inaccessible argument-list (%d)\n" ;
	                bprintf(pip->efp,fmt,pn,rs) ;
	                bprintf(pip->efp,"%s: afile=%s\n",pn,afn) ;
	            }
	        } /* end if */

	    } /* end if (processing file argument file list) */

#if	CF_DEBUG
	        if (DEBUGLEVEL(3))
	            debugprintf("main/procargs: 3 rs=%d\n",rs) ;
#endif

	    if ((rs >= 0) && (pan == 0)) {
	        rs = procvarnewsgroups(pip,&as) ;
	    }

#if	CF_DEBUG
	        if (DEBUGLEVEL(3))
	            debugprintf("main/procargs: 4 rs=%d\n",rs) ;
#endif

	    if (rs >= 0) {

#if	CF_DEBUG
	        if (DEBUGLEVEL(3))
	            debugprintf("main/procargs: procuserboards()\n") ;
#endif

	        rs = procuserboards(pip,&as,ofn) ;

#if	CF_DEBUG
	        if (DEBUGLEVEL(3))
	            debugprintf("main/procargs: "
	                "procuserboards() rs=%d\n",rs) ;
#endif

	    }

#if	CF_DEBUG
	        if (DEBUGLEVEL(3))
	            debugprintf("main/procargs: 5 rs=%d\n",rs) ;
#endif

	    rs1 = vecpstr_finish(&as) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (arguments) */

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("main/procargs: ret rs=%d\n",rs) ;
#endif

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (procargs) */

local int procspecs(PI *pip,vecpstr *asp,cchar *lbuf,int llen) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (field fsb ; (rs = field_start(&fsb,lbuf,llen)) >= 0) {
	    int		fl ;
	    cchar	*fp ;
	    while ((fl = field_get(&fsb,aterms,&fp)) >= 0) {
	        if (fl >= 0) {
	            rs = vecpstr_adduniq(asp,fp,fl) ;
	            if (rs < INT_MAX) c += 1 ;
	        }
	        if (fsb.term == '#') break ;
	        if (rs < 0) break ;
	    } /* end while */
	    rs1 = field_finish(&fsb) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (field) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (procspecs) */

local int procnewsdname(PI *pip) noex {
	int		rs = SR_OK ;
	int		rs1 ;

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("main/procnewsdname: ent\n") ;
#endif

	if (pip->newsdname == nullptr) pip->newsdname = getenv(VARNEWSDNAME) ;
	if (pip->newsdname == nullptr) pip->newsdname = getenv(VARBBNEWSDNAME) ;

	if ((pip->newsdname == nullptr) && pip->open.pcsconf) {
	    PCSCONF	*pcp = pip->pcsconf ;
	    PCSCONF_CUR	cur ;
	    if ((rs = pcsconf_curbegin(pcp,&cur)) >= 0) {
	        const int	vlen = VBUFLEN ;
	        cchar		*k = "bb:newsdir" ;
	        char		vbuf[VBUFLEN+1] ;
	        if ((rs1 = pcsconf_fetch(pcp,k,-1,&cur,vbuf,vlen)) >= 0) {
	            int		vl = rs1 ;
	            if (vl > 0) {
	                cchar	**vpp = &pip->newsdname ;
#if	CF_DEBUG
	                if (DEBUGLEVEL(3))
	                    debugprintf("main/procnewsdname: d=>%r<\n",
				vbuf,vl) ;
#endif
	                rs = proginfo_setentry(pip,vpp,vbuf,vl) ;
	            }
	        } /* end if */
		if ((rs >= 0) && (rs1 != SR_NOTFOUND)) rs = rs1 ;
#if	CF_DEBUG
	        if (DEBUGLEVEL(3))
	            debugprintf("main/procnewsdname: pcsconf_fetch() rs=%d\n",
	                rs1) ;
#endif
	        rs1 = pcsconf_curend(pcp,&cur) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (PCS-cursor) */
	} /* end if (pcsconf) */

	if (pip->newsdname == nullptr) pip->newsdname = NEWSDNAME ;

	if ((rs >= 0) && (pip->newsdname[0] != '/')) {
	    char	dname[MAXPATHLEN+1] ;
	    if ((rs = mkpath2(dname,pip->pr,pip->newsdname)) >= 0) {
	        cchar	**vpp = &pip->newsdname ;
	        rs = proginfo_setentry(pip,vpp,dname,rs) ;
	    }
	} /* end if (rooting) */

#if	CF_DEBUG
	if (DEBUGLEVEL(3)) {
	    debugprintf("main/procnewsdname: newsdname=%s\n",pip->newsdname) ;
	    debugprintf("main/procnewsdname: ret rs=%d\n",rs) ;
	}
#endif

	return rs ;
}
/* end subroutine (procnewsdname) */

local int procvarnewsgroups(PI *pip,vecpstr *asp) noex {
	int		rs = SR_OK ;
	cchar		*sp = getourenv(pip->envv,VARNEWSGROUPS) ;

	if (sp != nullptr) {
	    cchar	*tp ;
	    while ((tp = strchr(sp,':')) != nullptr) {
	        if ((tp-sp) > 0) {
	            rs = vecpstr_adduniq(asp,sp,(tp-sp)) ;
	        }
	        sp = (tp+1) ;
	    } /* end while */
	    if ((rs >= 0) && sp[0]) {
	        rs = vecpstr_adduniq(asp,sp,-1) ;
	    }
	} /* end if (VAR-NEWSGROUPS) */

	return rs ;
}
/* end subroutine (procvarnewsgroups) */

local int procuserboards(PI *pip,vecpstr *asp,cchar *ofn) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;

#if	CF_DEBUG
	if (DEBUGLEVEL(3)) {
	    int		i ;
	    cchar	*cp ;
	    debugprintf("main/procuserboards: ent of=%s\n",ofn) ;
	    for (i = 0 ; vecpstr_get(asp,i,&cp) >= 0 ; i += 1) {
	        debugprintf("main/procuserboards: cp=%s\n",cp) ;
	    }
	}
#endif /* CF_DEBUG */

	if ((rs = procnewsdname(pip)) >= 0) {
	    mkdirlist	dl ;
	    cchar	*pr = pip->pr ;
	    cchar	*nd = pip->newsdname ;

#if	CF_DEBUG
	    if (DEBUGLEVEL(3))
	        debugprintf("main/procuserboards: 1 \n") ;
#endif
#if	CF_mkdirlist
	    if ((rs = mkdirlist_start(&dl,pr,nd)) >= 0) {
	        cchar	*uf = pip->ufname ;

#if	CF_DEBUG
	        if (DEBUGLEVEL(3)) {
	            int	rs1 ;
	            debugprintf("main/procuserboards: 2 \n") ;
	            rs1 = mkdirlist_audit(&dl) ;
	            debugprintf("main/procuserboards: "
	                "mkdirlist_audit() rs=%d\n",rs1) ;
	        }
#endif

	        if ((rs = prognewsrc(pip,&dl,uf)) >= 0) {

#if	CF_DEBUG
	            if (DEBUGLEVEL(3))
	                debugprintf("main/procuserboards: 3 \n") ;
#endif

	            if ((rs = vecpstr_count(asp)) > 0) {
	                int	i ;
	                cchar	*ng ;
	                for (int i = 0 ; asp->get(i,&ng) >= 0 ; i += 1) {
	                    if (ng != nullptr) {
#if	CF_DEBUG
	                    if (DEBUGLEVEL(3))
	                        debugprintf("main/procuserboards: ng=%s\n",ng) ;
#endif
	                        rs = mkdirlist_show(&dl,ng,i) ;
	                        c += rs ;
			    }
	                    if (rs < 0) break ;
	                } /* end for */
	            } else {
	                rs = mkdirlist_showdef(&dl) ;
	                c += rs ;
	            } /* end if */
	            if (rs >= 0) {

#if	CF_DEBUG
	                if (DEBUGLEVEL(3))
	                    debugprintf("main/procuserboards: 10 c=%d\n",c) ;
#endif

	                if ((rs = mkdirlist_sort(&dl)) >= 0) {
	                    if ((rs = progmsgenv_begin(pip)) >= 0) {

	                        rs = procboards(pip,&dl,ofn) ;

	                        rs1 = progmsgenv_end(pip) ;
				if (rs >= 0) rs = rs1 ;
	                    } /* end if (progmsgenc) */
	                } /* end if (mkdirlist-sort) */

#if	CF_DEBUG
	                if (DEBUGLEVEL(3))
	                    debugprintf("main/procuserboards: "
	                        "procboards() rs=%d\n",rs) ;
#endif
	            } /* end if (ok) */
	        } /* end if (prognewsrc) */

	        rs1 = mkdirlist_finish(&dl) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (mkdirlist) */
#endif /* CF_mkdirlist */
	} /* end if (procnewsdname) */

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("main/procuserboards: ret rs=%d\n",rs) ;
#endif

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (procuserboards) */

local int procboards(PI *pip,mkdirlist *dlp,cchar *ofn) noex {
	int		rs ;
	int		rs1 ;
	int		rv = 0 ; /* return-value */
	if ((rs = ids_load(&pip->id)) >= 0) {
	    pip->open.id = true ;
	    {
	        rs = progmode(pip,dlp,ofn) ;
		rv = rs ;
	    }
	    pip->open.id = false ;
	    rs1 = ids_release(&pip->id) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ids) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (procboards) */


