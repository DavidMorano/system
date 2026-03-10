/* kshlib SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* library initialization for KSH built-in command libraries */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time */
#define	CF_DEBUGN	0		/* extra-special debugging */
#define	CF_DEBUGENV	0		/* debug environment */
#define	CF_DEBUGHEXB	0		/* debug w/ |debugprinthexblock()| */
#define	CF_PLUGIN	1		/* define 'plugin_version()' */
#define	CF_LOCKMEMALLOC	1		/* call |lockmemalloc(3uc)| */
#define	CF_KSHRUN	1		/* run background under KSH */
#define	CF_MQ		0		/* need |kshlib_mq()| */
#define	CF_LOCMALSTRW	0		/* use local |mallocstrw()| */

/* revision history:

	= 2001-11-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	KSH built-in library support.

	------------------------------------------------------------------------
	Name:

	lib_init

	Description:
	This subroutine is called by KSH when it loads this shared
	library.  We use this call to initialize some things that
	are partuclar to when executing from within KSH.  One of
	these is to set the underlying memory management facility
	to implement a MUTEX lock around its operations.  This helps
	guard against a failure if the KSH-native version of the
	normal memory management subroutines are somehow linked in
	(loaded) rather than the standard default UNIX® system
	subroutines.

	Synopsis:
	void lib_init(int flags,void *cxp)

	Arguments:
	flags		flags set by KSH
	contexto	KSH context

	Returns:
	-


	------------------------------------------------------------------------
	Name:
	lib_initenviron

	Description:
	This subroutine is used to set the environment inside the
	builtin-command (CMD) link-group to the same environment
	of our caller (usually the SHELL itself). Depending on how
	this module is loaded, it may be in the link-group of its
	parent (not at all unusual) or it may be in its own link-group.
	In theory it could even be on its own link-map, but that
	is not at all a typical situation so we ignore that for our
	purposes. If this modeule is loaded into its own link-group,
	some means has to be provided to set the 'environ' variable
	(above) to the envionment of the calling parent (at least
	set to something). When the builtin commands are called by
	the SHELL, they (the builtin commands) are called directly
	from it. But the CMD link-group has its own copy of the
	'environ' variable which would not have yet been set at all
	by anybody (any subroutine anywhere). So when CMDs are
	called by the SHELL, the CMD subroutine itself calls
	'lib_initenviron()' in order to set the CMD link-group copy
	of the 'environ' variable to the same as what exists wihtin
	the SHELL itself.  When CMDs are not called by the SHELL,
	but rather by some other means, some other way to set the
	'environ' variable has to be established.  Possible other
	ways are:

	1. this subroutine is not linked in, so there is *no*
	separate copy of 'environ' in the first place (completely
	typical in regular programs)

	2. by the caller instead calling the CMD subroutine though
	an intermediate subroutine (like named 'lib_caller()') and
	which gets its internal 'environ' copy set with that
	subroutine before the CMD subroutine is called in turn.

	Synopsis:
	int lib_initenviron(void *cxp)

	Arguments:
	cxp		context pointer

	Returns:
	<0		error
	>=0		OK


	------------------------------------------------------------------------
	Name:
	lib_caller

	Description:
	What in the world does this subroutine do?  This subroutine
	lets us call a command (CMD) function, otherwise known as
	a command "builtin" (from the SHELL language on the subject)
	while giving it an arbitrary environment determined by the
	caller.  In the infinite (short-sighted) wisdom of the
	creators of the builtin command interface, it was neglected
	to provide the capability to pass an arbitrary environment
	(like what is possible -- but not often used) with regular
	UNIX® process calls (using |exec(2)| and friends).  Without
	this subroutine, and having to call the command function
	directory, there is no way to pass or to create a unique
	environment for the function since it is forced to simply
	inherit the environment of the caller.

	Synopsis:
	int lib_caller(func,int argc,mainv argv,mainv envv,void *cxp) noex
	int		(*func)(int,cchar **,void *) ;

	Arguments:
	func		function to call
	argc		ARGC
	argv		ARGV
	envv		ENVV
	cxp		KSH context

	Returns:
	ex		exit status (like a process exit status)


	------------------------------------------------------------------------
	Name:
	lib_initmemalloc

	Description:
	When CF_LOCKMEMALLOC is set (non-zero) above, the LOCKMEMALLOC
	facility is (possibly) made available for use. Actual use
	depends on whether the module (LOCKMEMALLOC) is available
	somewhere in the current link-map. If it is indeed available,
	we turn it on (with the proper command). We do this (turn
	it ON) because the KSH program does not provide mutex locks
	around its memory allocation subroutines (which emulate
	|malloc(3c)| and friends). Of course KSH does not use any
	of the standard system subroutines because, well, that would
	be way too easy wouldn't it? The KSH program thinks that
	it is better than everyone else and so it uses its own
	memory-allocation facility. One problem: it did not protect
	its own facility with mutex locks. It did not do this because
	the shell is single threaded throughout. But this causes
	problems (like program crashes) when some dynmically loaded
	code splits into a multi-threaded mode. Yes, bang, a mess
	of the underlying memory-allocation system and the expected
	program crash as a result. The use of the LOCKMEMALLOC
	facility places mutex locks around all calls to the underlying
	memory allocation subroutines. This can help but also might
	still not be enough (since some shell code can still be
	used even within multi-threaded code). But every little bit
	helps.  One would think that everything today is multi-thread
	safe, but NO.  There are still some hold-outs, and these
	hold-outs make it bad for everybody!  Well, there it is.

	Synospsis:
	int lib_initmemalloc(int f)

	Arguments:
	f		switch (0=OFF, 1=ON)

	Returns:
	<0		error
	>=0		OK


	Notes:

	= Forked
	We get forked, screwed, turned, rotated, twisted, yanked
	and pulled -- and probably a few other things. So we have
	to be very careful about knowing who we are (our PID) and
	if the state of our address space and threads are still
	valid.  This whole business is a real fork turner!

	= Aligned integer types?
	"aligned |int|s are already atomic"
	Well, yes, on almost every platform except for the old
	(original) DEC Alpha architecture.

*******************************************************************************/

#include	<envstandards.h>	/* must be ordered first to configure */

#if	(defined(KSHBUILTIN) && (KSHBUILTIN > 0))
#include	<shell.h>
#endif

#include	<sys/types.h>
#include	<sys/param.h>
#include	<dlfcn.h>
#include	<poll.h>
#include	<csignal>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<lockmemalloc.h>
#include	<upt.h>
#include	<ptm.h>
#include	<ptc.h>
#include	<sighand.h>
#include	<sockaddress.h>
#include	<raqhand.h>
#include	<utmpacc.h>
#include	<tmtime.hh>
#include	<strx.h>
#include	<dirempty.h>
#include	<char.h>
#include	<ischarx.h>
#include	<exitcodes.h>
#include	<localmisc.h>		/* |DIGBUFLEN| |TIMEBUFLEN| */
#include	<libdebug.h>		/* LIBDEBUG */

#include	"sesmsg.hh"
#include	"msgdata.h"
#include	"kshlib.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#if	(defined(KSHBUILTIN) && (KSHBUILTIN > 0))
#define	KSHLIB_MEMALLOC		1
#define	KSHLIB_WHERE		"embedded"
#define	KSHLIB_SYMSEARCH	RTLD_SELF
#else
#define	KSHLIB_MEMALLOC		1
#define	KSHLIB_WHERE		"standalone"
#define	KSHLIB_SYMSEARCH	RTLD_SELF
#endif

#define	VARKSHLIBRUN	"KSHLIB_RUNOPTS"

#define	NDF		"kshlib.deb"

#ifndef	SYMNAMELEN
#define	SYMNAMELEN	60
#endif

#define	TO_LOCKENV	10

#define	KSHLIB		kshlib
#define	KSHLIB_FL	kshlib_flags
#define	KSHLIB_SCOPE	PTHREAD_SCOPE_PROCESS

#define	STORENOTE	storenote
#define	STORENOTE_FL	storenote_flags


/* external subroutines */

#if	CF_LOCKMEMALLOC
extern int	lockmemalloc_set(int) ;
#endif


/* external variables */

extern mainv	environ ;


/* local structures */

extern "C" {
    typedef int (*subcmd_t)(int,cchar **,cchar **,void *) noex ;
}

#ifndef	TYPEDEF_TWORKER
#define	TYPEDEF_TWORKER
extern "C" {
    typedef	int (*tworker)(void *) noex ;
}
#endif

extern "C" {
    typedef	int (*cmdsub_t)(int,cchar **,cchar **,void *) noex ;
    typedef	int (*func_caller)(int,cchar **,void *) noex ;
}

struct kshlib_flags {
	uint		notes:1 ;
	uint		mq:1 ;
	uint		initrun:1 ;	/* at initialization time */
} ; /* end struct */

struct kshlib {
	ptm		mx ;		/* mutex data */
	ptc		cxv ;		/* condition variable */
	SIGHAND		sm ;
	SOCKADDRESS	servaddr ;	/* server address */
	raqhand		mq ;		/* message queue */
	KSHLIB_FL	f, open ;
	cchar		*sesdname ;	/* session directory-name */
	cchar		*reqfname ;	/* request file-name */
	pid_t		sid ;		/* session ID */
	pid_t		pid ;		/* process ID */
	pthread_t	tid ;		/* worker thread */
	time_t		ti_sescheck ;
	volatile int	f_initonce ;	/* aligned |int|s are already atomic */
	volatile int	f_init ;	/* aligned |int|s are already atomic */
	volatile int	f_initdone ;	/* aligned |int|s are already atomic */
	volatile int	f_running ;	/* aligned |int|s are already atomic */
	volatile int	f_capture ;	/* aligned |int|s are already atomic */
	volatile int	f_exiting ;	/* aligned |int|s are already atomic */
	volatile int	f_autorun ;	/* aligned |int|s are already atomic */
	volatile int	waiters ;	/* aligned |int|s are already atomic */
	sig_atomic_t	f_sigquit ;
	sig_atomic_t	f_sigterm ;
	sig_atomic_t	f_sigintr ;
	sig_atomic_t	f_sigwich ;
	sig_atomic_t	f_sigchild ;
	sig_atomic_t	f_sigsusp ;
	int		intpoll ;
	int		intsescheck ;
	int		seshour ;
	int		runmode ;
	int		serial ;
	int		sfd ;
	int		cdefs ;		/* defualt count */
	int		servlen ;	/* serv-addr length */
	int		pollcount ;
} ; /* end struct */

struct storenote_flags {
	uint		displayed:1 ;	/* displayed by KSH itself */
	uint		read:1 ;	/* marked as read by comment */
} ; /* end struct */

struct storenote {
	STORENOTE_FL	f ;
	time_t		stime ;
	cchar		*dbuf ;
	cchar		*user ;
	char		*a ;
	int		type ;
	int		dlen ;
} ; /* end struct */


/* forward references */

int		lib_initenviron(void *) noex ;
int		lib_callcmd(cchar *,int,cchar **,cchar **,void *) noex ;
int 		lib_callfunc(subcmd_t,int,cchar **,cchar **,void *) noex ;

local int	kshlib_init(void) noex ;
local void	kshlib_fini(void) noex ;

local void	kshlib_atforkbefore() noex ;
local void	kshlib_atforkparent() noex ;
local void	kshlib_atforkchild() noex ;
local void	kshlib_sighand(int,siginfo_t *,void *) noex ;

local int	kshlib_begin(KSHLIB *) noex ;
local int	kshlib_end(KSHLIB *) noex ;

local int	kshlib_autorun(KSHLIB *,cchar **) noex ;
local int	kshlib_autorunopt(KSHLIB *,cchar *,int) noex ;
local int	kshlib_autorunoptnotes(KSHLIB *,cchar *,int,int) noex ;
local int	kshlib_autorunopter(KSHLIB *) noex ;

local int	kshlib_runbegin(KSHLIB *) noex ;
local int	kshlib_runner(KSHLIB *) noex ;
local int	kshlib_runend(KSHLIB *) noex ;

local int	kshlib_sid(KSHLIB *) noex ;
local int	kshlib_sesdname(KSHLIB *) noex ;
local int	kshlib_reqfname(KSHLIB *) noex ;
local int	kshlib_worker(KSHLIB *) noex ;
local int	kshlib_worknoop(KSHLIB *,MSGDATA *) noex ;
local int	kshlib_workecho(KSHLIB *,MSGDATA *) noex ;
local int	kshlib_workbiff(KSHLIB *,MSGDATA *) noex ;
local int	kshlib_workbiffer(KSHLIB *,SESMSG_BIFF *) noex ;
local int	kshlib_workgen(KSHLIB *,MSGDATA *) noex ;
local int	kshlib_workgener(KSHLIB *,SESMSG_GEN *) noex ;
local int	kshlib_workdef(KSHLIB *,MSGDATA *) noex ;

local int	kshlib_msgenter(KSHLIB *,STORENOTE *) noex ;
local int	kshlib_reqopen(KSHLIB *) noex ;
local int	kshlib_reqopener(KSHLIB *) noex ;
local int	kshlib_reqsend(KSHLIB *,MSGDATA *,int,int) noex ;
local int	kshlib_reqrecv(KSHLIB *,MSGDATA *) noex ;
local int	kshlib_reqclose(KSHLIB *) noex ;
local int	kshlib_poll(KSHLIB *) noex ;
local int	kshlib_cmdsend(KSHLIB *,int) noex ;
local int	kshlib_capbegin(KSHLIB *,int) noex ;
local int	kshlib_capend(KSHLIB *) noex ;
local int	kshlib_sigbegin(KSHLIB *,cint *) noex ;
local int	kshlib_sigend(KSHLIB *) noex ;

local int	kshlib_notesbegin(KSHLIB *) noex ;
local int	kshlib_notesend(KSHLIB *) noex ;
local int	kshlib_notesactive(KSHLIB *) noex ;
local int	kshlib_notescount(KSHLIB *) noex ;

local int	kshlib_mqbegin(KSHLIB *) noex ;
local int	kshlib_mqend(KSHLIB *) noex ;
local int	kshlib_mqfins(KSHLIB *) noex ;
local int	kshlib_mqactive(KSHLIB *) noex ;
local int	kshlib_mqcount(KSHLIB *) noex ;

local int	kshlib_sesend(KSHLIB *) noex ;

#if	CF_MQ
local int	kshlib_mq(KSHLIB *) noex ;
#endif

#if	CF_LOCKMEMALLOC
int		lib_initmemalloc(int) noex ;
#endif /* CF_LOCKMEMALLOC */

local int	storenote_start(STORENOTE *,int,time_t,cc *,cc*,int) noex ;
local int	storenote_finish(STORENOTE *) noex ;

local int	sdir(cchar *,int) noex ;
local int	mksdir(cchar *,mode_t) noex ;
local int	mksdname(char *,cchar *,pid_t) noex ;

#if	CF_LOCMALSTRW
local int	mallocstrw(cchar *,int,cchar **) noex ;
#endif /* CF_LOCMALSTRW */


#if	CF_DEBUGENV && CF_DEBUGN
local int	ndebugenv(cchar *,cchar **) noex ;
#endif


/* local variables */

constexpr cpcchar	defenviron[] = {
	"_PROCSTATE=screwed",
	nullptr
} ; /* end array */

static KSHLIB		kshlib_data ; /* zero-initialized */

constexpr cint	sigblocks[] = {
	SIGUSR1,
	SIGUSR2,
	0
} ; /* end array */

constexpr int	sigigns[] = {
	SIGHUP,
	SIGPIPE,
	SIGPOLL,
#if	defined(SIGXFSZ)
	SIGXFSZ,
#endif
	0
} ; /* end array */

constexpr int	sigints[] = {
	SIGQUIT,
	SIGTERM,
	SIGINT,
	SIGWINCH,
	SIGCHLD,
	SIGTSTP,
	0
} ; /* end array */

enum runopts {
	runopt_notes,
	runopt_lognotes,
	runopt_overlast
} ; /* end enum */

constexpr cpcchar	runopts[] = {
	"notes",
	"lognotes",
	nullptr
} ; /* end array */


/* exported variables */


/* exported subroutines */

void lib_init(int flags,void *cxp) noex {
	KSHLIB		*uip = &kshlib_data ;
	if (! uip->f_initonce) {
	    uip->f_initonce = true ;

#if	CF_DEBUGN
	{
	    const uint	pid = getpid() ;
	    const uint	ppid = getppid() ;
	    nprintf(NDF,"lib_init: ent pid=%u ppid=%u\n",pid,ppid) ;
	}
#endif

#if	CF_DEBUGENV && CF_DEBUGN
	{
	    const pid_t	pid = getpid() ;
	    void 	*p = dlsym(RTLD_DEFAULT,"environ") ;
	    nprintf(NDF,"lib_init: ent pid=%u\n",pid) ;
	    nprintf(NDF,"lib_init: flags=%16ÃŸ (%u)\n",flags,flags) ;
	    if (p != nullptr) {
	        cchar	***evp = (cchar ***) p ;
	        cchar	**ev ;
	        ev = *evp ;
	        nprintf(NDF,"lib_init: p=%P\n",p) ;
	        nprintf(NDF,"lib_init: main-environ{%P}=%P\n",evp,ev) ;
	        nprintf(NDF,"lib_init: lib-environ{%P}=%P\n",
	            &environ,environ) ;
	        ndebugenv("lib_init-m",ev) ;
	    }
	    nprintf(NDF,"lib_init: lib-environ=%P\n",environ) ;
	    nprintf(NDF,"lib_init: cxp=%P\n",cxp) ;
	}
#endif /* CF_DEBUGN */

#if	CF_DEBUGENV && CF_DEBUGN && 0
	if (environ != nullptr) {
	    cchar	**ev = (cchar **) environ ;
	    ndebugenv("lib_init-l",ev) ;
	}
#endif

#if	CF_LOCKMEMALLOC
	{
	    cint	f = KSHLIB_MEMALLOC ;
	    (void) lib_initmemalloc(f) ;
	}
#endif /* CF_LOCKMEMALLOC */

#if	CF_KSHRUN
	{
	    int		rs ;
#if	CF_DEBUGN
	    nprintf(NDF,"lib_init: KSHRUN\n") ;
#endif
	    if ((rs = lib_initenviron(cxp)) >= 0) {
	        if ((rs = kshlib_init()) >= 0) {
	            KSHLIB	*uip = &kshlib_data ;
	            cchar	**envv = (cchar **) environ ;
	            rs = kshlib_autorun(uip,envv) ;
	        } /* end if (init) */
	    } /* end if (lib_initenviron) */
#if	CF_DEBUGN
	    nprintf(NDF,"lib_init: KSHRUN rs=%d\n",rs) ;
#endif
	}
#endif /* CF_KSHRUN */

#if	CF_DEBUGN
	{
	    const uint	pid = getpid() ;
	    nprintf(NDF,"lib_init: ret pid=%u\n",pid) ;
	}
#endif

	} else {
#if	CF_DEBUGN
	    const uint	pid = getpid() ;
	    nprintf(NDF,"lib_init: REPEAT pid=%u\n",pid) ;
#endif
	} /* end if (init-once) */
}
/* end subroutine (lib_init) */

void lib_fini(void) noex {
	kshlib_fini() ;
}
/* end subroutine (lib_fini) */

/* is this multi-thread safe or not? */
int lib_initenviron(void *cxp) noex {
	int		rs = SR_OK ;
	(void) cxp ;
	if (environ == nullptr) {
	    char ***eppp = dlsym(RTLD_DEFAULT,"environ") ;
	    if ((eppp != nullptr) && (eppp != &environ)) environ = *eppp ;
	    if (environ == nullptr) environ = (char **) defenviron ;
	} /* end if (environ) */
	return rs ;
}
/* end subroutine (lib_initenviron) */

int lib_mainbegin(cchar **envv,cint *catches) noex {
	int		rs ;

#if	CF_DEBUGN
	{
	    const uint	pid = getpid() ;
	    nprintf(NDF,"lib_mainbegin: ent pid=%u\n",pid) ;
	}
#endif

	if ((rs = kshlib_init()) >= 0) {
	    KSHLIB	*uip = &kshlib_data ;
	    if ((rs = kshlib_sigbegin(uip,catches)) >= 0) {
	        rs = kshlib_autorun(uip,envv) ;
	        if (rs < 0)
	            kshlib_sigend(uip) ;
	    } /* end if (kshlib_sigbegin) */
	} /* end if (kshlib_init) */

#if	CF_DEBUGN
	nprintf(NDF,"lib_mainbegin: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (lib_mainbegin) */

int lib_mainend(void) noex {
	KSHLIB		*uip = &kshlib_data ;
	int		rs = SR_OK ;
	int		rs1 ;

#if	CF_DEBUGN
	{
	    const uint	pid = getpid() ;
	    nprintf(NDF,"lib_mainend: ent pid=%u\n",pid) ;
	    nprintf(NDF,"lib_mainend: f_running=%u\n",uip->f_running) ;
	}
#endif

	if (uip->f_running) {
	    rs1 = kshlib_runend(uip) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (running) */

	rs1 = kshlib_sigend(uip) ;
	if (rs >= 0) rs = rs1 ;

#if	CF_DEBUGN
	nprintf(NDF,"lib_mainend: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (lib_mainend) */

int lib_kshbegin(void *cxp,cint *catches) noex {
	int		rs ;
#if	CF_DEBUGN
	nprintf(NDF,"lib_kshbegin: ent\n") ;
	nprintpid("lib_kshbegin") ;
#endif
	if ((rs = lib_initenviron(cxp)) >= 0) {
	    if ((rs = kshlib_init()) >= 0) {
	        KSHLIB	*kip = &kshlib_data ;
	        if ((rs = kshlib_sigbegin(kip,catches)) >= 0) {
	            kip->runmode |= KSHLIB_RMKSH ;
	        } /* end if (kshlib_sigbegin) */
	    } /* end if (kshlib_init) */
	} /* end if (lib_initenviron) */
	return rs ;
}
/* end subroutine (lib_kshbegin) */

int lib_kshend(void) noex {
	KSHLIB		*kip = &kshlib_data ;
	int		rs = SR_OK ;
	int		rs1 ;
	kip->serial += 1 ;
	kip->runmode &= (~ KSHLIB_RMKSH) ;
	rs1 = sighand_finish(&kip->sm) ;
	if (rs >= 0) rs = rs1 ;
#if	CF_DEBUGN
	nprintf(NDF,"lib_kshbegin: sighand_finish() rs=%d\n",rs) ;
#endif
	return rs ;
}
/* end subroutine (lib_kshend) */

int lib_runmode(void) noex {
	KSHLIB		*kip = &kshlib_data ;
	return kip->runmode ;
}
/* end subroutine (lib_runmode) */

int lib_serial(void) noex {
	KSHLIB		*kip = &kshlib_data ;
	int		s = kip->serial ;
	return s ;
}
/* end subroutine (lib_serial) */

int lib_sigreset(int sn) noex {
	KSHLIB		*kip = &kshlib_data ;
	int		rs = SR_OK ;
	switch (sn) {
	case SIGQUIT:
	    kip->f_sigquit = 0 ;
	    break ;
	case SIGTERM:
	    kip->f_sigterm = 0 ;
	    break ;
	case SIGINT:
	    kip->f_sigintr = 0 ;
	    break ;
	case SIGWINCH:
	    kip->f_sigwich = 0 ;
	    break ;
	case SIGCHLD:
	    kip->f_sigchild = 0 ;
	    break ;
	case SIGTSTP:
	    kip->f_sigsusp = 0 ;
	    break ;
	default:
	    rs = SR_INVALID ;
	    break ;
	} /* end switch */
	return rs ;
}
/* end subroutine (lib_sigreset) */

int lib_sigquit(void) noex {
	KSHLIB		*kip = &kshlib_data ;
	int		rs = SR_OK ;
	if (kip->f_sigquit) {
	    kip->f_sigquit = 0 ;
	    rs = SR_QUIT ;
	}
	return rs ;
}
/* end subroutine (lib_sigquit) */

int lib_sigterm(void) noex {
	KSHLIB		*kip = &kshlib_data ;
	int		rs = SR_OK ;
	if (kip->f_sigterm) {
	    kip->f_sigterm = 0 ;
	    rs = SR_EXIT ;
	}
	return rs ;
}
/* end subroutine (lib_sigterm) */

int lib_sigintr(void) noex {
	KSHLIB		*kip = &kshlib_data ;
	int		rs = SR_OK ;
	if (kip->f_sigintr) {
	    kip->f_sigintr = 0 ;
	    rs = SR_INTR ;
	}
	return rs ;
}
/* end subroutine (lib_sigintr) */

int lib_issig(int sn) noex {
	KSHLIB		*kip = &kshlib_data ;
	int		rs = SR_OK ;
	int		f = false ;
	switch (sn) {
	case SIGQUIT:
	    f = kip->f_sigquit ;
	    if (f) kip->f_sigquit = 0 ;
	    break ;
	case SIGTERM:
	    f = kip->f_sigterm ;
	    if (f) kip->f_sigterm = 0 ;
	    break ;
	case SIGINT:
	    f = kip->f_sigintr ;
	    if (f) kip->f_sigintr = 0 ;
	    break ;
	case SIGWINCH:
	    f = kip->f_sigwich ;
	    if (f) kip->f_sigwich = 0 ;
	    break ;
	case SIGCHLD:
	    f = kip->f_sigchild ;
	    if (f) kip->f_sigchild = 0 ;
	    break ;
	case SIGTSTP:
	    f = kip->f_sigsusp ;
	    if (f) kip->f_sigsusp = 0 ;
	    break ;
	default:
	    rs = SR_INVALID ;
	    break ;
	} /* end switch */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (lib_issig) */

int lib_initmemalloc(int f) noex {
	int		rs = SR_OK ;
#if	CF_DEBUGN
	nprintf(NDF,"lib_initmemalloc: ent f=%u\n",f) ;
#endif
	if (f) {
	    cchar	*sym = "lockmemalloc_set" ;
	    void	*sop = RTLD_SELF ;
	    void	*p ;
	    if ((p = dlsym(sop,sym)) != nullptr) {
	        int	(*fun)(int) = (int (*)(int)) p ;
	        rs = (*fun)(lockmemallocset_begin) ;
#if	CF_DEBUGN
	        nprintf(NDF,"lib_initmemalloc: LOCKMEMALLOC rs=%d\n",rs) ;
#endif
	    }
	} /* end if (enabled) */
#if	CF_DEBUGN
	nprintf(NDF,"lib_initmemalloc: ret rs=%d\n",rs) ;
#endif
	return rs ;
}
/* end subroutine (lib_initmemalloc) */

int lib_progaddr(cchar *name,void *app) noex {
	int		rs = SR_OK ;
#if	CF_DEBUGN
	nprintf(NDF,"lib_progaddr: ent name=>%s<\n",name) ;
#endif
	if ((name != nullptr) && (name[0] != '\0')) {
	    cint	symlen = SYMNAMELEN ;
	    char	symbuf[SYMNAMELEN+1] ;
	    if ((rs = sncpy2(symbuf,symlen,"p_",name)) >= 0) {
	        void	*sop = KSHLIB_SYMSEARCH ;
	        void	*p ;
#if	CF_DEBUGN
	        nprintf(NDF,"lib_progaddr: sym=%s\n",symbuf) ;
#endif
	        if ((p = dlsym(sop,symbuf)) != nullptr) {
	            if (app != nullptr) {
	                caddr_t	*sub = (caddr_t *) app ;
	                *sub = (caddr_t) p ;
	            }
	        } else {
	            rs = SR_NOENT ;
		}
	    } /* end if (sncpy) */
	} else {
	    rs = SR_NOENT ;
	}
#if	CF_DEBUGN
	nprintf(NDF,"lib_progaddr: ret rs=%d\n",rs) ;
#endif
	return rs ;
}
/* end subroutine (lib_progaddr) */

int lib_proghave(cchar *name) noex {
	return lib_progaddr(name,nullptr) ;
}
/* end subroutine (lib_proghave) */

int lib_progcall(cc *name,int argc,mainv argv,mainv envv,void *cxp) noex {
	cmdsub_t	addr ;
	int		rs ;
	int		ex = EX_OK ;

#if	CF_DEBUGN
	nprintf(NDF,"lib_progcall: ent name=%s\n",name) ;
	nprintf(NDF,"lib_progcall: from=%s\n",KSHLIB_WHERE) ;
#endif

	if ((rs = lib_progaddr(name,&addr)) >= 0) {
	    if ((rs = lib_initenviron(nullptr)) >= 0) {
	        if ((rs = kshlib_init()) >= 0) {
#if	CF_DEBUGN
	            nprintf(NDF,"lib_progcall: call()\n") ;
#endif
	            ex = (*addr)(argc,argv,envv,nullptr) ;
#if	CF_DEBUGN
	            nprintf(NDF,"lib_progcall: call() ex=%u\n",ex) ;
#endif
	        } else {
	            ex = EX_OSERR ;
		}
	    } else {
	        ex = EX_OSERR ;
	    }
	} else {
	    ex = EX_NOPROG ;
	}

#if	CF_DEBUGN
	nprintf(NDF,"lib_progcall: ret ex=%u (%d)\n",ex,rs) ;
#endif

	return ex ;
}
/* end subroutine (lib_progcall) */

int lib_progcalla(cvoid *func,int argc,mainv argv,mainv envv,void *cxp) noex {
	subcmd_t	f = (subcmd_t) func ;
	return lib_callfunc(f,argc,argv,envv,cxp) ;
}
/* end subroutine (lib_progcalla) */

int lib_caller(cvoid *fa,int argc,cchar **argv,cchar **envv,void *cxp) noex {
	func_caller	func = (func_caller) fa ;
	int		rs ;
	int		ex = EX_OK ;

#if	CF_DEBUGN
	nprintf(NDF,"lib_caller: &environ=%p\n",&environ) ;
	nprintf(NDF,"lib_caller: environ=%p\n",environ) ;
	nprintf(NDF,"lib_caller: envv=%p\n",envv) ;
#endif

#if	CF_DEBUGENV && CF_DEBUGN
	if (envv != nullptr)
	    ndebugenv("lib_caller",envv) ;
#endif

	if ((rs = lib_initenviron(cxp)) >= 0) {

#if	CF_DEBUGN
	    nprintf(NDF,"lib_caller: func()\n") ;
#endif

	    if (func != nullptr) {
	        ex = (*func)(argc,argv,cxp) ;
	    } else {
	        ex = EX_NOPROG ;
	    }

#if	CF_DEBUGN
	    nprintf(NDF,"lib_caller: func() ex=%u\n",ex) ;
#endif

	} /* end if (lib_initenviron) */

	if ((rs < 0) && (ex == EX_OK)) ex = EX_MUTEX ;

#if	CF_DEBUGN
	nprintf(NDF,"lib_caller: ret ex=%u (%d)\n",ex,rs) ;
#endif

	return ex ;
}
/* end subroutine (lib_caller) */

int lib_callfunc(subcmd_t func,int argc,mainv argv,mainv envv,void *cxp) noex {
	int		rs ;
	int		ex = EX_OK ;
	if ((rs = lib_initenviron(cxp)) >= 0) {
	    if ((rs = kshlib_init()) >= 0) {
	        if (func != nullptr) {
	            ex = (*func)(argc,argv,envv,cxp) ;
	        } else {
	            ex = EX_NOPROG ;
		}
	    } else {
	        ex = EX_OSERR ;
	    }
	} /* end if (lib_initenviron) */
	if ((rs < 0) && (ex == EX_OK)) ex = EX_MUTEX ;
	return ex ;
}
/* end subroutine (lib_callfunc) */

int lib_callcmd(cchar *name,int argc,mainv argv,mainv envv,void *cxp) noex {
	int		rs = SR_OK ;
	int		ex = EX_OK ;

#if	CF_DEBUGN
	nprintf(NDF,"lib_callcmd: ent name=%s\n",name) ;
	nprintf(NDF,"lib_callcmd: from=%s\n",KSHLIB_WHERE) ;
#endif

	if ((name != nullptr) && (name[0] != '\0')) {
	    char	symname[SYMNAMELEN+1] ;
	    if ((rs = sncpy2(symname,MAXNAMELEN,"p_",name)) >= 0) {
	        void	*sop = RTLD_SELF ;
	        void	*p ;
#if	CF_DEBUGN
		nprintf(NDF,"lib_callcmd: srch-sym=%s\n",symname) ;
#endif
	        if ((p = dlsym(sop,symname)) != nullptr) {
	            int (*cf)(int,cchar **,cchar **,void *) ;
	            cf = (int (*)(int,cchar **,cchar **,void *)) p ;
#if	CF_DEBUGN
		    nprintf(NDF,"lib_callcmd: call-before\n") ;
#endif
	            ex = (*cf)(argc,argv,envv,cxp) ;
#if	CF_DEBUGN
		    nprintf(NDF,"lib_callcmd: call-after ex=%u\n",ex) ;
#endif
	        } else {
#if	CF_DEBUGN
		    nprintf(NDF,"lib_callcmd: unavailable\n") ;
#endif
	            ex = EX_UNAVAILABLE ;
		}
	    } else {
	        ex = EX_NOPROG ;
	    }
	} else {
	    ex = EX_NOPROG ;
	}
	if ((rs < 0) && (ex == EX_OK)) ex = EX_OSERR ;

#if	CF_DEBUGN
	nprintf(NDF,"lib_callcmd: ret ex=%u (%d)\n",ex,rs) ;
#endif

	return ex ;
}
/* end subroutine (lib_callcmd) */

int lib_noteadm(int cmd,...) noex {
	int		rs ;
	int		rs1 ;
	int		rv = 0 ;
#if	CF_DEBUGN
	nprintf(NDF,"lib_noteadm: ent cmd=%u\n",cmd) ;
#endif
	if ((rs = kshlib_init()) >= 0) {
	    KSHLIB	*uip = &kshlib_data ;
	    if ((rs = kshlib_begin(uip)) >= 0) {
	        if ((rs = kshlib_capbegin(uip,-1)) >= 0) {
	            switch (cmd) {
	            case kshlibcmd_noteoff:
	                rs = kshlib_notesend(uip) ;
	                rv = rs ;
	                break ;
	            case kshlibcmd_noteon:
	                rs = kshlib_notesbegin(uip) ;
	                rv = rs ;
	                break ;
	            case kshlibcmd_notecount:
	                rs = kshlib_notescount(uip) ;
	                rv = rs ;
	                break ;
	            case kshlibcmd_notestate:
	                rs = kshlib_notesactive(uip) ;
			rv = rs ;
	                break ;
	            } /* end switch */
	            rs1 = kshlib_capend(uip) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (capture) */
	    } /* end if (kshlib_begin) */
	} /* end if (kshlib_init) */
#if	CF_DEBUGN
	nprintf(NDF,"lib_noteadm: ret rs=%d rv=%u\n",rs,rv) ;
#endif
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (lib_noteadm) */

int lib_noteread(KSHLIB_NOTE *rp,int mi) noex {
	int		rs ;
	int		rs1 ;
	int		rc = 0 ;
	if (rp == nullptr) return SR_FAULT ;
#if	CF_DEBUGN
	nprintf(NDF,"lib_noteread: ent mi=%u\n",mi) ;
#endif
	memclear(rp) ;
	if (mi < 0) return SR_INVALID ;
	if ((rs = kshlib_init()) >= 0) {
	    KSHLIB	*uip = &kshlib_data ;
	    if ((rs = kshlib_capbegin(uip,-1)) >= 0) {
	        if ((rs = kshlib_mqactive(uip)) > 0) {
	            STORENOTE	*ep ;
#if	CF_DEBUGN
	nprintf(NDF,"lib_noteread: raqhand_acc() mi=%u\n",mi) ;
#endif
	            if ((rs = raqhand_acc(&uip->mq,mi,&ep)) >= 0) {
#if	CF_DEBUGN
	nprintf(NDF,"lib_noteread: raqhand_acc() rs=%d ep{%p}\n",rs,ep) ;
#endif
	                if (ep != nullptr) {
	                    rp->stime = ep->stime ;
	                    rp->type = ep->type ;
	                    rp->nlen = ep->dlen ;
	                    strwcpy(rp->user,ep->user,SESMSG_USERLEN) ;
	                    strwcpy(rp->nbuf,ep->dbuf,SESMSG_NBUFLEN) ;
	                    rc = 1 ;
	                } /* end if (non-null) */
	            } else if (rs == SR_NOTFOUND) {
#if	CF_DEBUGN
	nprintf(NDF,"lib_noteread: raqhand_acc() rs=NOTFOUND\n") ;
#endif
	                rs = SR_OK ;
	            } /* end if (raqhand_acc) */
	        } /* end if (kshlib_mqactive) */
#if	CF_DEBUGN
	nprintf(NDF,"lib_noteread: _mqactive-out rs=%d\n",rs) ;
#endif
	        rs1 = kshlib_capend(uip) ;
	        if (rs >= 0) rs = rs1 ;
#if	CF_DEBUGN
	nprintf(NDF,"lib_noteread: _capend() rs=%d\n",rs) ;
#endif
	    } /* end if (capture) */
	} /* end if (kshlib_init) */
#if	CF_DEBUGN
	nprintf(NDF,"lib_noteread: ret rs=%d rc=%u\n",rs,rc) ;
#endif
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (lib_noteread) */


int lib_notedel(int ni)
{
	int		rs ;
	int		rs1 ;
	int		rc = 0 ;
	if (ni < 0) return SR_INVALID ;
	if ((rs = kshlib_init()) >= 0) {
	    KSHLIB	*uip = &kshlib_data ;
	    if ((rs = kshlib_capbegin(uip,-1)) >= 0) {
	        if ((rs = kshlib_mqactive(uip)) > 0) {
	            rs = raqhand_del(&uip->mq,ni) ;
	            rc = rs ;
	        } /* end if (kshlib_mqactive) */
	        rs1 = kshlib_capend(uip) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (capture) */
	} /* end if (kshlib_init) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (lib_notedel) */


#if	CF_PLUGIN
ulong plugin_version(void) {
	return 20131127UL ;
}
/* end subroutine (plugin_version) */
#endif /* CF_PLUGIN */


/* local subroutines */


local int kshlib_init(void)
{
	KSHLIB		*uip = &kshlib_data ;
	int		rs = SR_OK ;
#if	CF_DEBUGN
	nprintf(NDF,"kshlib_init: ent f_init=%u\n",uip->f_init) ;
#endif
	if (! uip->f_init) {
	    ptm *mxp = &uip->mx ;
	    uip->f_init = true ;
	    if ((rs = mxp->create) >= 0) {
	        ptc *cnp = &uip->cxv ;
	        if ((rs = cnp->create) >= 0) {
	            void	(*b)() = kshlib_atforkbefore ;
	            void	(*ap)() = kshlib_atforkparent ;
	            void	(*ac)() = kshlib_atforkchild ;
	            if ((rs = uc_atforkrec(b,ap,ac)) >= 0) {
	                if ((rs = uc_atexit(kshlib_fini)) >= 0) {
	                    uip->pid = getpid() ;
	                    uip->sfd = -1 ;
	                    rs = 1 ;
	                    uip->f_initdone = true ;
#if	CF_DEBUGN
	nprintf(NDF,"kshlib_init: done pid=%d\n",uip->pid) ;
#endif
	                }
	                if (rs < 0)
	                    uc_atforkexp(b,ap,ac) ;
	            } /* end if (uc_atfork) */
	            if (rs < 0) {
	                cnp->destroy() ;
		    }
	        } /* end if (ptc_create) */
	    } /* end if (ptm_create) */
	    if (rs < 0) {
	        uip->f_init = false ;
	    }
	} else {
	    while ((rs >= 0) && uip->f_init && (! uip->f_initdone)) {
	        rs = msleep(1) ;
	        if (rs == SR_INTR) rs = SR_OK ;
	    }
	    if ((rs >= 0) && (! uip->f_init)) rs = SR_LOCKLOST ;
	}
#if	CF_DEBUGN
	nprintf(NDF,"kshlib_init: ret rs=%d\n",rs) ;
#endif
	return rs ;
}
/* end subroutine (kshlib_init) */

local void kshlib_fini(void) noex {
	struct kshlib	*uip = &kshlib_data ;
#if	CF_DEBUGN
	nprintf(NDF,"kshlib_fini: ent\n") ;
#endif
	if (uip->f_initdone) {
	    uip->f_initdone = false ;
	    {
	        kshlib_runend(uip) ;
	        kshlib_end(uip) ;
	    }
	    {
	        void	(*b)() = kshlib_atforkbefore ;
	        void	(*ap)() = kshlib_atforkparent ;
	        void	(*ac)() = kshlib_atforkchild ;
	        uc_atforkexp(b,ap,ac) ;
	    }
	    {
	        ptc *cnp = &uip->cxv ;
	        cnp->destroy() ;
	    }
	    {
		ptm *mxp = &uip->mx ;
	        mxp->destroy() ;
	    }
	    memclear(uip) ;
	} /* end if (atexit registered) */
#if	CF_DEBUGN
	nprintf(NDF,"kshlib_fini: ret\n") ;
#endif
}
/* end subroutine (kshlib_fini) */

local int kshlib_begin(KSHLIB *uip) noex {
	if (uip == nullptr) return SR_FAULT ;
	uip->ti_sescheck = 0 ;
	uip->intpoll = KSHLIB_INTPOLL ;
	uip->intsescheck = KSHLIB_INTSESCHECK ;
	uip->seshour = KSHLIB_SESHOUR ;
	return SR_OK ;
}
/* end subroutine (kshlib_begin) */

local int kshlib_end(KSHLIB *uip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    rs1 = kshlib_notesend(uip) ;
	    if (rs >= 0) rs = rs1 ;
	}
	{
	    rs1 = kshlib_mqend(uip) ;
	    if (rs >= 0) rs = rs1 ;
	}
	{
	    rs1 = kshlib_sesend(uip) ;
	    if (rs >= 0) rs = rs1 ;
	}
	if (uip->sesdname != nullptr) {
	    rs1 = lm_free(uip->sesdname) ;
	    if (rs >= 0) rs = rs1 ;
	    uip->sesdname = nullptr ;
	}
	return rs ;
}
/* end subroutine (kshlib_end) */

local int kshlib_autorun(KSHLIB *uip,mainv envv) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
#if	CF_DEBUGN
	nprintf(NDF,"kshlib_autorun: ent\n") ;
#endif
	if (! uip->f_autorun) {
	    cchar	*vp ;
	    uip->f_autorun = true ;
	    if ((vp = getourenv(envv,VARKSHLIBRUN)) != nullptr) {
		cchar	*tp ;
		while ((tp = strbrk(vp," ,:")) != nullptr) {
		    if ((tp-vp) > 0) {
		        rs = kshlib_autorunopt(uip,vp,(tp-vp)) ;
			c += 1 ;
		    }
		    vp = (tp+1) ;
		    if (rs < 0) break ;
		} /* end while */
		if ((rs >= 0) && (vp[0] != '\0')) {
		    rs = kshlib_autorunopt(uip,vp,-1) ;
		    c += rs ;
		}
	    } /* end if (get-env) */
	} /* end if (needed auto-run check) */
#if	CF_DEBUGN
	nprintf(NDF,"kshlib_autorun: ret rs=%d c=%u\n",rs,c) ;
#endif
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (kshlib_autorun) */


local int kshlib_autorunopt(KSHLIB *uip,cchar *sp,int sl)
{
	int		rs = SR_OK ;
	int		si ;
	int		oi ;
	int		vl = 0 ;
	int		c = 0 ;
	cchar		*vp = nullptr ;
	cchar		*tp ;
	if ((si = siskipwhite(sp,sl)) > 0) {
	    sp += si ;
	    sl -= si ;
	}
	if ((tp = strnchr(sp,sl,'=')) != nullptr) {
	    vl = sfshrink((tp+1),((sp+sl)-(tp+1)),&vp) ;
	    sl = (tp-sp) ;
	    while (sl && CHAR_ISWHITE(sp[sl-1])) sl -= 1 ;
	}
#if	CF_DEBUGN
	nprintf(NDF,"kshlib_autorunopt: opt=%r\n",sp,sl) ;
#endif
	if ((oi = matostr(runopts,2,sp,sl)) >= 0) {
	    switch (oi) {
	    case runopt_notes:
		rs = kshlib_autorunoptnotes(uip,vp,vl,false) ;
		c += rs ;
		break ;
	    case runopt_lognotes:
		rs = kshlib_autorunoptnotes(uip,vp,vl,true) ;
		c += rs ;
		break ;
	    } /* end switch */
	} /* end if (match) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (kshlib_autorunopt) */


/* ARGSUSED */
local int kshlib_autorunoptnotes(KSHLIB *uip,cchar *vp,int vl,int f)
{
	int		rs = SR_OK ;
	int		rv = 0 ;
#if	CF_DEBUGN
	nprintf(NDF,"kshlib_autorunoptnotes: ent f=%u\n",f) ;
	nprintutmp("kshlib_autorunoptnotes") ;
#endif
	if (! uip->fl.initrun) {
	    int		f_go = true ;
	    if (f) {
		if ((rs = kshlib_sid(uip)) >= 0) {
		    UTMPACC_ENT	ue ;
		    const pid_t	pid = uip->pid ;
		    cint	rsn = SR_NOTFOUND ;
		    cint	ulen = UTMPACC_BUFLEN ;
		    char	ubuf[UTMPACC_BUFLEN+1] ;
		    if (uip->sid == pid) {
		        if ((rs = utmpacc_entsid(&ue,ubuf,ulen,pid)) == rsn) {
		            rs = SR_OK ;
		            f_go = false ;
		        }
		    } else {
		        f_go = false ;
		    }
		} /* end if (kshlib_sid) */
	    }
#if	CF_DEBUGN
	nprintf(NDF,"kshlib_autorunoptnotes: mid rs=%d f_go=%u\n",rs,f_go) ;
#endif
	    if ((rs >= 0) && f_go) {
	        uip->fl.initrun = true ;
		rs = kshlib_autorunopter(uip) ;
		rv = rs ;
#if	CF_DEBUGN
		nprintf(NDF,"kshlib_autorunoptnotes: _autorunopter() rs=%d\n",
			rs) ;
#endif
	    }
	} /* end if (need check) */
#if	CF_DEBUGN
	nprintf(NDF,"kshlib_autorunoptnotes: ret rs=%d rv=%u\n",rs,rv) ;
#endif
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (kshlib_autorunoptnotes) */


local int kshlib_autorunopter(KSHLIB *uip)
{
	int		rs ;
	int		rs1 ;
	int		rv = 0 ;
	if ((rs = kshlib_begin(uip)) >= 0) {
	    if ((rs = kshlib_capbegin(uip,-1)) >= 0) {
		if ((rs = kshlib_notesbegin(uip)) >= 0) {
		    rv = true ;
		}
	        rs1 = kshlib_capend(uip) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (capture) */
	} /* end if (kshlib_begin) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (kshlib_autorunopter) */


local int kshlib_runbegin(KSHLIB *uip)
{
	int		rs = SR_OK ;
	int		f = false ;

#if	CF_DEBUGN
	nprintf(NDF,"kshlib_runbegin: ent f_running=%u\n",uip->f_running) ;
#endif

	if (! uip->f_running) {
	    if ((rs = kshlib_reqopen(uip)) >= 0) {
		rs = kshlib_runner(uip) ;
		f = rs ;
	    } /* end if (kshlib_reqopen) */
	} /* end if (not-running) */

#if	CF_DEBUGN
	nprintf(NDF,"kshlib_runbegin: ret rs=%d f=%u\n",rs,f) ;
#endif

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (kshlib_runbegin) */


local int kshlib_runner(KSHLIB *uip)
{
	PTA		ta ;
	int		rs ;
	int		rs1 ;
	int		f = false ;

#if	CF_DEBUGN
	nprintf(NDF,"kshlib_runner: ent\n") ;
#endif

	if ((rs = pta_create(&ta)) >= 0) {
	    cint	scope = KSHLIB_SCOPE ;
	    if ((rs = pta_setscope(&ta,scope)) >= 0) {
	        pthread_t	tid ;
	        tworker		wt = (tworker) kshlib_worker ;
	        if ((rs = uptcreate(&tid,&ta,wt,uip)) >= 0) {
	            uip->f_running = true ;
	            uip->tid = tid ;
	            f = true ;
	        } /* end if (pthread-create) */
#if	CF_DEBUGN
	        nprintf(NDF,"kshlib_runner: pt-create rs=%d tid=%u\n",
	            rs,tid) ;
#endif
	    } /* end if (pta-setscope) */
	    rs1 = pta_destroy(&ta) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (pta) */

#if	CF_DEBUGN
	nprintf(NDF,"kshlib_runner: ret rs=%d f=%u\n",rs,f) ;
#endif

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (kshlib_runner) */


local int kshlib_runend(KSHLIB *uip)
{
	int		rs = SR_OK ;
	int		rs1 ;

#if	CF_DEBUGN
	nprintf(NDF,"kshlib_runend: ent running=%u\n",uip->f_running) ;
#endif

	if (uip->f_running) {
	    const pid_t		pid = getpid() ;
	    if (pid == uip->pid) {
	        cint	cmd = sesmsgtype_exit ;
	        if ((rs = kshlib_cmdsend(uip,cmd)) >= 0) {
	            pthread_t	tid = uip->tid ;
	            int		trs ;
#if	CF_DEBUGN
		    nprintpid("kshlib_runend") ;
		    nprintid("kshlib_runend") ;
	            nprintf(NDF,"kshlib_runend: pt-join tid=%u\n",tid) ;
#endif
	            if ((rs = uptjoin(tid,&trs)) >= 0) {
	                uip->f_running = false ;
	                rs = trs ;
	            } else if (rs == SR_SRCH) { /* should never happen */
#if	CF_DEBUGN
		    nprintpid("kshlib_runend") ;
		    nprintid("kshlib_runend") ;
	            nprintf(NDF,"kshlib_runend: SRCH uptjoin(%u) rs=%d\n",
		        tid,rs) ;
#endif
	                uip->f_running = false ;
		        rs = SR_OK ;
		    }
#if	CF_DEBUGN
		    nprintid("kshlib_runend") ;
	            nprintf(NDF,"kshlib_runend: pt-join tid=%u rs=%d\n",
	                tid,rs) ;
#endif
	            rs1 = kshlib_reqclose(uip) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (kshlib_cmdsend) */
	    } else {
		uip->f_running = false ;
		uip->f_exiting = false ;
		uip->pid = pid ;
	    }
	} /* end if (running) */

#if	CF_DEBUGN
	nprintf(NDF,"kshlib_runend: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (kshlib_runend) */


/* it always takes a good bit of code to make this part look easy! */
local int kshlib_worker(KSHLIB *uip)
{
	MSGDATA		m ;
	int		rs ;
	int		rs1 ;

#if	CF_DEBUGN
	{
	    const uint	tid = uptself(nullptr) ;
	    nprintf(NDF,"kshlib_worker: ent tid=%u\n",tid) ;
	}
#endif

	if ((rs = msgdata_init(&m,0)) >= 0) {
	    while ((rs = kshlib_reqrecv(uip,&m)) > 0) {
	        int	f_exit = false ;
#if	CF_DEBUGN
	        nprintf(NDF,"kshlib_worker: reqrecv mt=%u\n",rs) ;
#endif
	        switch (rs) {
	        case sesmsgtype_exit:
	            f_exit = true ;
	            break ;
	        case sesmsgtype_noop:
	            rs = kshlib_worknoop(uip,&m) ;
	            break ;
	        case sesmsgtype_echo:
	            rs = kshlib_workecho(uip,&m) ;
	            break ;
	        case sesmsgtype_gen:
	            rs = kshlib_workgen(uip,&m) ;
	            break ;
	        case sesmsgtype_biff:
	            rs = kshlib_workbiff(uip,&m) ;
	            break ;
	        default:
	            rs = kshlib_workdef(uip,&m) ;
	            break ;
	        } /* end switch */
	        if (f_exit) break ;
	        if (rs < 0) break ;
	    } /* end while (looping on commands) */
	    rs1 = msgdata_fini(&m) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (msgdata) */

#if	CF_DEBUGN
	nprintid("kshlib_worker") ;
	nprintf(NDF,"kshlib_worker: ret rs=%d\n",rs) ;
#endif

	uip->f_exiting = true ;
	return rs ;
}
/* end subroutine (kshlib_worker) */


local int kshlib_worknoop(KSHLIB *uip,MSGDATA *mip)
{
	int		rs ;
	if ((rs = msgdata_conpass(mip,false)) >= 0) {
	    rs = kshlib_reqsend(uip,mip,-1,0) ;
	} /* end if (msgdata_conpass) */
	return rs ;
}
/* end subroutine (kshlib_worknoop) */


local int kshlib_workecho(KSHLIB *uip,MSGDATA *mip)
{
	int		rs ;
	if ((rs = msgdata_conpass(mip,false)) >= 0) {
	    rs = kshlib_reqsend(uip,mip,-1,0) ;
	} /* end if (msgdata_conpass) */
	return rs ;
}
/* end subroutine (kshlib_workecho) */


local int kshlib_workgen(KSHLIB *uip,MSGDATA *mip)
{
	int		rs ;
	int		rs1 ;
#if	CF_DEBUGN
	nprintf(NDF,"kshlib_workgen: ent\n") ;
#endif
#if	CF_DEBUGS
	debugprintf("kshlib_workgen: ent\n") ;
#endif
#if	CF_DEBUGS && CF_DEBUGHEXB
	debugprinthexblock("kshlib_workgen: ",80,mip->mbuf,mip->ml) ;
#endif
	if ((rs = kshlib_capbegin(uip,-1)) >= 0) {
	    if ((rs = kshlib_notesactive(uip)) > 0) {
	        SESMSG_GEN	m2 ;
	        if ((rs = sesmsg_gen(&m2,1,mip->mbuf,mip->ml)) >= 0) {
	            rs = kshlib_workgener(uip,&m2) ;
	        } /* end if (sesmsg_gen) */
	    } /* end if (kshlib_notesactive) */
	    rs1 = kshlib_capend(uip) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (capture) */
#if	CF_DEBUGN
	nprintf(NDF,"kshlib_workgen: ret rs=%d\n",rs) ;
#endif
	return rs ;
}
/* end subroutine (kshlib_workgen) */

local int kshlib_workgener(KSHLIB *uip,SESMSG_GEN *mp) noex {
	STORENOTE	*ep ;
	cint	esz = szof(STORENOTE) ;
	int		rs ;
#if	CF_DEBUGN
	nprintf(NDF,"kshlib_workgener: ent\n") ;
	nprintf(NDF,"kshlib_workgener: m=>%r<\n",
	    mp->nbuf,strlinelen(mp->nbuf,-1,50)) ;
#endif
	if ((rs = lm_mall(esz,&ep)) >= 0) {
	    time_t	st = mp->stime ;
	    cint	mt = mp->msgtype ;
	    int		nl = rmeol(mp->nbuf,-1) ;
	    cchar	*nbuf = mp->nbuf ;
	    cchar	*un = mp->user ;
#if	CF_DEBUGN
	    {
		char	tbuf[TIMEBUFLEN+1] ;
	        nprintf(NDF,"kshlib_workgener: m=>%r<\n",
	            nbuf,strlinelen(nbuf,nl,50)) ;
	        timestr_logz(st,tbuf) ;
	        nprintf(NDF,"kshlib_workgener: t=%s\n",tbuf) ;
	    }
#endif /* CF_DEBUGN */
	    if ((rs = storenote_start(ep,mt,st,un,nbuf,nl)) >= 0) {
	        rs = kshlib_msgenter(uip,ep) ;
	        if (rs < 0)
	            storenote_finish(ep) ;
	    } /* end if (storenote_start) */
	    if (rs < 0)
	        lm_free(ep) ;
	} /* end if (m-a) */
#if	CF_DEBUGN
	nprintf(NDF,"kshlib_workgener: ret rs=%d\n",rs) ;
#endif
	return rs ;
}
/* end subroutine (kshlib_workgener) */


local int kshlib_workbiff(KSHLIB *uip,MSGDATA *mip)
{
	int		rs ;
	int		rs1 ;
	if ((rs = kshlib_capbegin(uip,-1)) >= 0) {
	    if ((rs = kshlib_notesactive(uip)) > 0) {
	        SESMSG_BIFF	m3 ;
	        if ((rs = sesmsg_biff(&m3,1,mip->mbuf,mip->ml)) >= 0) {
	            rs = kshlib_workbiffer(uip,&m3) ;
	        } /* end if (sesmsg_biff) */
	    } /* end if (kshlib_notesactive) */
	    rs1 = kshlib_capend(uip) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (capture) */
	return rs ;
}
/* end subroutine (kshlib_workbiff) */

local int kshlib_workbiffer(KSHLIB *uip,SESMSG_BIFF *mp) noex {
	STORENOTE	*ep ;
	cint	esz = szof(STORENOTE) ;
	int		rs ;
#if	CF_DEBUGN
	nprintf(NDF,"kshlib_workbiffer: m=>%r<\n",
	    mp->nbuf,strlinelen(mp->nbuf,-1,50)) ;
#endif
	if ((rs = lm_mall(esz,&ep)) >= 0) {
	    time_t	st = mp->stime ;
	    cint	mt = mp->msgtype ;
	    int		nl = rmeol(mp->nbuf,-1) ;
	    cchar	*un = mp->user ;
	    cchar	*nbuf = mp->nbuf ;
	    if ((rs = storenote_start(ep,mt,st,un,nbuf,nl)) >= 0) {
	        rs = kshlib_msgenter(uip,ep) ;
	        if (rs < 0)
	            storenote_finish(ep) ;
	    } /* end if (storenote_start) */
	    if (rs < 0)
	        lm_free(ep) ;
	} /* end if (m-a) */
	return rs ;
}
/* end subroutine (kshlib_workbiffer) */

local int kshlib_workdef(KSHLIB *uip,MSGDATA *mip) noex {
	ptm		*mxp = &uip->mx ;
	int		rs ;
	if (mip == nullptr) return SR_FAULT ;
	if ((rs = mxp->lockebgin) >= 0) {
	    uip->cdefs += 1 ;
	    mxp->lockend() ;
	} /* end if (mutex) */
	return rs ;
}
/* end subroutine (kshlib_workdef) */

local int kshlib_msgenter(KSHLIB *uip,STORENOTE *ep) noex {
	raqhand		*qlp = &uip->mq ;
	cint	rso = SR_OVERFLOW ;
	int		rs ;
	if ((rs = raqhand_ins(qlp,ep)) == rso) {
	    void	*dum ;
	    if ((rs = raqhand_rem(qlp,&dum)) >= 0) {
	        rs = raqhand_ins(qlp,ep) ;
	    }
	}
	return rs ;
}
/* end subroutine (kshlib_msgenter) */

local int kshlib_sid(KSHLIB *uip) noex {
	if (uip->sid == 0) {
	    uip->sid = getsid(0) ;
	}
	return SR_OK ;
}
/* end subroutine (kshlib_sid) */

local int kshlib_sesdname(KSHLIB *uip) noex {
	int		rs = SR_OK ;
	int		pl = 0 ;
	if (uip->sesdname == nullptr) {
	    cchar	*dname = KSHLIB_SESDNAME ;
	    if ((rs = sdir(dname,(W_OK|X_OK))) >= 0) {
		if ((rs = kshlib_sid(uip)) >= 0) {
	           char		pbuf[MAXPATHLEN+1] ;
	           if ((rs = mksdname(pbuf,dname,uip->sid)) >= 0) {
			cchar	*cp ;
			pl = rs ;
	                if ((rs = lm_strw(pbuf,pl,&cp)) >= 0) {
			    uip->sesdname = cp ;
			}
	    	    } /* end if (mksdname) */
		} /* end if (kshlib_sid) */
	    } /* end if (sdir) */
	} else {
	    pl = strlen(uip->sesdname) ;
	} /* end if (needed) */
	return (rs >= 0) ? pl : rs ;
}
/* end subroutine (kshlib_sesdname) */

local int kshlib_reqfname(KSHLIB *uip) noex {
	int		rs = SR_OK ;
	int		pl = 0 ;
	if (uip->reqfname == nullptr) {
	    if ((rs = kshlib_sesdname(uip)) >= 0) {
	        const uint	uv = (uint) uip->pid ;
	        cint		dlen = DIGBUFLEN ;
	        char		dbuf[DIGBUFLEN+1] = { 'p' } ;
	        if ((rs = ctdecui((dbuf+1),(dlen-1),uv)) >= 0) {
		    cchar	*sesdname = uip->sesdname ;
		    char	pbuf[MAXPATHLEN+1] ;
	            if ((rs = mkpath2(pbuf,sesdname,dbuf)) >= 0) {
	                cchar	*cp ;
		        pl = rs ;
	                if ((rs = lm_strw(pbuf,pl,&cp)) >= 0) {
	                    uip->reqfname = cp ;
	                }
	            } /* end if (mkpath) */
		} /* end if (ctdecui) */
	    } /* end if (kshlib_sesdname) */
	} else {
	    pl = strlen(uip->sesdname) ;
	}
	return (rs >= 0) ? pl : rs ;
}
/* end subroutine (kshlib_reqfname) */

local int kshlib_reqopen(KSHLIB *uip) noex {
	int		rs ;
#if	CF_DEBUGN
	nprintf(NDF,"kshlib_reqopen: ent\n") ;
#endif
	if ((rs = kshlib_reqfname(uip)) >= 0) {
	    rs = kshlib_reqopener(uip) ;
	} /* end if (kshlib_reqfname) */
#if	CF_DEBUGN
	nprintf(NDF,"kshlib_reqopen: ret rs=%d\n",rs) ;
#endif
	return rs ;
}
/* end subroutine (kshlib_reqopen) */

local int kshlib_reqopener(KSHLIB *uip) noex {
	cmode		om = 0666 ;
	cint	lo = 0 ; /* listen options */
	int		rs ;
	cchar		*req = uip->reqfname ;
#if	CF_DEBUGN
	nprintf(NDF,"kshlib_reqopener: ent req=%s\n",req) ;
#endif
	if ((rs = listenusd(req,om,lo)) >= 0) {
	    cint	fd = rs ;
	    if ((rs = uc_closeonexec(fd,true)) >= 0) {
	        SOCKADDRESS	*sap = &uip->servaddr ;
	        cint	af = AF_UNIX ;
	        if ((rs = sockaddress_start(sap,af,req,0,0)) >= 0) {
	            uip->servlen = rs ;
	            uip->sfd = fd ;
	        }
	    }
	    if (rs < 0)
	        u_close(fd) ;
	} /* end if (listenusd) */
#if	CF_DEBUGN
	nprintf(NDF,"kshlib_reqopener: ret rs=%d\n",rs) ;
#endif
	return rs ;
}
/* end subroutine (kshlib_reqopener) */

local int kshlib_reqclose(KSHLIB *uip) noex {
	int		rs = SR_OK ;
	int		rs1 ;

#if	CF_DEBUGN
	nprintf(NDF,"kshlib_reqclose: ent sfd=%d\n",uip->sfd) ;
#endif

	if (uip->sfd >= 0) {
	    rs1 = u_close(uip->sfd) ;
	    if (rs >= 0) rs = rs1 ;
	    uip->sfd = -1 ;
	    {
	        SOCKADDRESS	*sap = &uip->servaddr ;
	        rs1 = sockaddress_finish(sap) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (uip->reqfname != nullptr) {
#if	CF_DEBUGN
	        nprintf(NDF,"kshlib_reqclose: reqfname{%p}=Â¿\n",uip->reqfname) ;
	        nprintf(NDF,"kshlib_reqclose: reqfname=%s\n",uip->reqfname) ;
#endif
	        if (uip->reqfname[0] != '\0') {
	            u_unlink(uip->reqfname) ;
	        }
	        rs1 = lm_free(uip->reqfname) ;
	        if (rs >= 0) rs = rs1 ;
	        uip->reqfname = nullptr ;
	    } /* end if (reqfname) */
	} /* end if (server-open) */

#if	CF_DEBUGN
	nprintf(NDF,"kshlib_reqclose: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (kshlib_reqclose) */

local int kshlib_reqsend(KSHLIB *uip,MSGDATA *mip,int dl,int cl) noex {
	cint	fd = uip->sfd ;
	return msgdata_send(mip,fd,dl,cl) ;
}
/* end subroutine (kshlib_reqsend) */

local int kshlib_reqrecv(KSHLIB *uip,MSGDATA *mip) noex {
	POLLFD		fds[1] = {} ;
	cint	fd = uip->sfd ;
	cint	mto = (uip->intpoll*POLL_INTMULT) ;
	cint	nfds = 1 ;
	int		size ;
	int		rs ;
	int		rc = 0 ;

#if	CF_DEBUGN
	nprintf(NDF,"kshlib_reqrecv: ent\n") ;
#endif

	fds[0].fd = fd ;
	fds[0].events = (POLLIN | POLLPRI | POLLERR) ;
	fds[0].revents = 0 ;

	while ((rs = u_poll(fds,nfds,mto)) >= 0) {
	    int		f = false ;
	    if (rs > 0) {
	        cint	re = fds[0].revents ;
	        if (re & (POLLIN|POLLPRI)) {
	            if ((rs = msgdata_recv(mip,fd)) >= 0) {
	                f = true ;
	                if (rs > 0) {
	                    rc = MKCHAR(mip->mbuf[0]) ;
	                } else {
	                    rc = sesmsgtype_invalid ;
			}
	            } /* end if (msgdata_recv) */
	        } else if (re & POLLERR) {
	            rs = SR_IO ;
	        }
	    } else if (rs == SR_INTR) {
	        rs = SR_OK ;
	    }
	    if (f) break ;
	    if (rs >= 0) {
	        rs = kshlib_poll(uip) ;
	    }
	    if (rs < 0) break ;
	} /* end while (polling) */

#if	CF_DEBUGN
	nprintf(NDF,"kshlib_reqrecv: ret rs=%d rc=%u\n",rs,rc) ;
#endif

	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (kshlib_reqrecv) */


local int kshlib_poll(KSHLIB *uip)
{
	int		rs = SR_OK ;

	if (uip == nullptr) return SR_FAULT ;

#if	CF_DEBUGN
	nprintf(NDF,"kshlib_poll: ent\n") ;
#endif

	if ((uip->pollcount % 5) == 4) {
	    const time_t	dt = time(nullptr) ;
	    cint		intsescheck = uip->intsescheck ;
	    if ((dt - uip->ti_sescheck) >= intsescheck) {
	        TMTIME		m ;
	        uip->ti_sescheck = dt ;
	        if ((rs = tmtime_timelocal(&m,dt)) >= 0) {
		    if (m.hour >= uip->seshour) {
		        cchar	*sesdname = KSHLIB_SESDNAME ;
		        rs = rmsesfiles(sesdname) ;
#if	CF_DEBUGN
	nprintf(NDF,"kshlib_poll: rmsesfiles() rs=%d\n",rs) ;
#endif
		    }
		}
	    }
	}

	uip->pollcount += 1 ;

#if	CF_DEBUGN
	nprintf(NDF,"kshlib_poll: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (kshlib_poll) */


local int kshlib_cmdsend(KSHLIB *uip,int cmd)
{
	int		rs = SR_OK ;
	int		rs1 ;
	int		f = false ;
#if	CF_DEBUGN
	nprintf(NDF,"kshlib_cmdsend: ent cmd=%u\n",cmd) ;
	nprintf(NDF,"kshlib_cmdsend: f_running=%u\n",uip->f_running) ;
#endif
	if (uip->f_running && (uip->reqfname != nullptr)) {
	    f = true ;
	    switch (cmd) {
	    case sesmsgtype_exit:
	        {
	            MSGDATA	m ;
	            if ((rs = msgdata_init(&m,0)) >= 0) {
	                SESMSG_EXIT	m0{} ;
	                cint	sal = uip->servlen ;
	                cvoid	*sap = &uip->servaddr ;
	                msgdata_setaddr(&m,sap,sal) ;
	                if ((rs = sesmsg_exit(&m0,0,m.mbuf,m.mlen)) >= 0) {
	                    rs = kshlib_reqsend(uip,&m,rs,0) ;
#if	CF_DEBUGN
	                    nprintf(NDF,
	                        "kshlib_cmdsend: kshlib_reqsend() rs=%d\n",rs) ;
#endif
	                } /* end if (sesmsg_exit) */
#if	CF_DEBUGN
	                nprintf(NDF,
	                    "kshlib_cmdsend: sesmsg_exit-out rs=%d\n",rs) ;
#endif
	    		rs1 = msgdata_fini(&m) ;
	    		if (rs >= 0) rs = rs1 ;
	            } /* end if (msgdata) */
	        }
	        break ;
	    default:
	        rs = SR_INVALID ;
	        break ;
	    } /* end switch */
	} /* end if (running) */
#if	CF_DEBUGN
	nprintf(NDF,"kshlib_cmdsend: ret rs=%d f=%u\n",rs,f) ;
#endif
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (kshlib_cmdsend) */

local void kshlib_atforkbefore() noex {
	KSHLIB		*uip = &kshlib_data ;
	{
	    ptm *mxp = &uip->mx ;
	    mxp->lockbegin() ;
	}
}
/* end subroutine (kshlib_atforkbefore) */

local void kshlib_atforkparent() noex {
	KSHLIB		*uip = &kshlib_data ;
	{
	    ptm *mxp = &uip->mx ;
	    mxp->lockend() ;
	}
}
/* end subroutine (kshlib_atforkafter) */

local void kshlib_atforkchild() noex {
	KSHLIB		*uip = &kshlib_data ;
	uip->f_running = false ;
	uip->f_exiting = false ;
	uip->pid = getpid() ;
	{
	    ptm *mxp = &uip->mx ;
	    mxp->lockend() ;
	}
}
/* end subroutine (kshlib_atforkchild) */

/* ARGSUSED */
local void kshlib_sighand(int sn,siginfo_t *sip,void *vcp) noex {
	KSHLIB		*kip = &kshlib_data ;
	switch (sn) {
	case SIGQUIT:
	    kip->f_sigquit = true ;
	    break ;
	case SIGTERM:
	    kip->f_sigterm = true ;
	    break ;
	case SIGINT:
	    kip->f_sigintr = true ;
	    break ;
	case SIGWINCH:
	    kip->f_sigwich = true ;
	    break ;
	case SIGCHLD:
	    kip->f_sigchild = true ;
	    break ;
	case SIGTSTP:
	    kip->f_sigsusp = true ;
	    break ;
	} /* end switch */
}
/* end subroutine (kshlib_sighand) */

local int kshlib_capbegin(KSHLIB *uip,int to) noex {
	ptm		*mxp = &uip->mx ;
	int		rs ;
	int		rs1 ;
#if	CF_DEBUGN && 0
	nprintf(NDF,"kshlib_capbegin: ent to=%d\n",to) ;
#endif
	if ((rs = mxp->lockbegin(to)) >= 0) {
	    {
	        ptc *cnp = &uip->cxv ;
	        uip->waiters += 1 ;
	        while ((rs >= 0) && uip->f_capture) { /* busy */
	            rs = cnp->waiter(mxp,to) ;
	        } /* end while */
	        if (rs >= 0) {
	            uip->f_capture = true ;
	        }
	        uip->waiters -= 1 ;
	    } /* end block */
	    rs1 = mxp->lockend ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ptm) */
#if	CF_DEBUGN && 0
	nprintf(NDF,"kshlib_capbegin: ret rs=%d\n",rs) ;
#endif
	return rs ;
}
/* end subroutine (kshlib_capbegin) */

local int kshlib_capend(KSHLIB *uip) noex {
	ptm		*mxp = &uip->mx ;
	int		rs ;
	int		rs1 ;
#if	CF_DEBUGN && 0
	nprintf(NDF,"kshlib_capend: ent\n") ;
#endif
	if ((rs = mxp->lockbegin) >= 0) {
	    {
	        ptc *cnp = &uip->cxv ;
	        uip->f_capture = false ;
	        if (uip->waiters > 0) {
	            rs = cnp->signal ;
	        }
	    } /* end block */
	    rs1 = mxp->lockend ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ptm) */
#if	CF_DEBUGN && 0
	nprintf(NDF,"kshlib_capend: ret rs=%d\n",rs) ;
#endif
	return rs ;
}
/* end subroutine (kshlib_capend) */

/* ARGSUSED */
local int kshlib_sigbegin(KSHLIB *kip,cint *catches) noex {
	int		rs ;
	sighand_handler	sh = kshlib_sighand ;
#if	CF_DEBUGN
	nprintf(NDF,"kshlib_sigbegin: ent\n") ;
#endif
	kip->f_sigterm = 0 ;
	kip->f_sigintr = 0 ;
	rs = sighand_start(&kip->sm,sigblocks,sigigns,sigints,sh) ;
#if	CF_DEBUGN
	nprintf(NDF,"kshlib_sigbegin: ret rs=%d\n",rs) ;
#endif
	return rs ;
}
/* end subroutine (kshlib_sigbegin) */

local int kshlib_sigend(KSHLIB *kip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
#if	CF_DEBUGN
	nprintf(NDF,"kshlib_sigend: ent\n") ;
#endif
	rs1 = sighand_finish(&kip->sm) ;
	if (rs >= 0) rs = rs1 ;
#if	CF_DEBUGN
	nprintf(NDF,"kshlib_sigend: ret rs=%d\n",rs) ;
#endif
	return rs ;
}
/* end subroutine (kshlib_sigend) */

local int kshlib_notesbegin(KSHLIB *uip) noex {
	int		rs = SR_OK ;
	int		f = true ;
	if (! uip->open.notes) {
	    if ((rs = kshlib_mqbegin(uip)) >= 0) {
	        if ((rs = kshlib_runbegin(uip)) >= 0) {
	    	    uip->open.notes = true ;
		    f = false ;
		}
	    }
	}
#if	CF_DEBUGN
	nprintf(NDF,"kshlib_notesbegin: ret rs=%d\n",rs) ;
#endif
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (kshlib_notesbegin) */

local int kshlib_notesend(KSHLIB *uip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		f = false ;
	if (uip->open.notes) {
	    rs1 = kshlib_runend(uip) ;
	    if (rs >= 0) rs = rs1 ;
	    uip->open.notes = false ;
	    f = true ;
	}
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (kshlib_notesend) */

local int kshlib_notesactive(KSHLIB *uip) noex {
	return MKBOOL(uip->open.notes) ;
}
/* end subroutine (kshlib_notesactive) */

local int kshlib_notescount(KSHLIB *uip) noex {
	int		rs = SR_OK ;
	if (uip->open.notes) {
	    rs = kshlib_mqcount(uip) ;
	}
	return rs ;
}
/* end subroutine (kshlib_notescount) */

local int kshlib_mqbegin(KSHLIB *uip) noex {
	int		rs = SR_OK ;
	if (! uip->open.mq) {
	    cint	n = KSHLIB_NENTS ;
	    if ((rs = raqhand_start(&uip->mq,n,0)) >= 0) {
	        uip->open.mq = true ;
	    }
	}
	return rs ;
}
/* end subroutine (kshlib_mqbegin) */

local int kshlib_mqend(KSHLIB *uip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (uip->open.mq) {
	    {
	    rs1 = kshlib_mqfins(uip) ;
	    if (rs >= 0) rs = rs1 ;
	    }
	    {
	    rs1 = raqhand_finish(&uip->mq) ;
	    if (rs >= 0) rs = rs1 ;
	    uip->open.mq = false ;
	    }
	}
	return rs ;
}
/* end subroutine (kshlib_mqend) */

local int kshlib_mqfins(KSHLIB *uip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (uip->open.mq) {
	    raqhand	*qlp = &uip->mq ;
	    STORENOTE	*ep ;
	    int		i ;
	    for (i = 0 ; raqhand_get(qlp,i,&ep) >= 0 ; i += 1) {
	        if (ep != nullptr) {
	            rs1 = storenote_finish(ep) ;
	            if (rs >= 0) rs = rs1 ;
	            rs1 = lm_free(ep) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (non-null) */
	    } /* end for */
	} /* end if (open-mq) */
	return rs ;
}
/* end subroutine (kshlib_mqfins) */

local int kshlib_mqcount(KSHLIB *uip) noex {
	int		rs = SR_OK ;
	if (uip->open.mq) {
	    rs = raqhand_count(&uip->mq) ;
	}
#if	CF_DEBUGN
	nprintf(NDF,"kshlib_mqcount: ret rs=%d\n",rs) ;
#endif
	return rs ;
}
/* end subroutine (kshlib_mqcount) */

local int kshlib_mqactive(KSHLIB *uip) noex {
	int		rs = MKBOOL(uip->open.mq) ;
	return rs ;
}
/* end subroutine (kshlib_mqactive) */

#if	CF_MQ
/* ensure message-queue operations are initialized */
local int kshlib_mq(KSHLIB *uip) noex {
	int		rs = SR_OK ;
	if (! uip->open.mq) {
	    rs = kshlib_mqbegin(uip) ;
	}
	return rs ;
}
/* end subroutine (kshlib_mq) */
#endif /* CF_MQ */

local int kshlib_sesend(KSHLIB *uip) noex {
	int		rs = SR_OK ;
	if (uip->sesdname != nullptr) {
	    if ((rs = kshlib_sid(uip)) >= 0) {
		if (uip->sid == uip->pid) {
		    if ((rs = dirempty(uip->sesdname)) > 0) {
			rs = u_rmdir(uip->sesdname) ;
		    }
		}
	    }
	}
	return rs ;
}
/* end subroutine (kshlib_sesend) */

local int storenote_start(ep,mt,st,un,mdp,mdl)
STORENOTE	*ep ;
int		mt ;
time_t		st ;
cchar		*un ;
cchar		*mdp ;
int		mdl ;
{
	int		rs ;
	int		size = 0 ;
	char		*bp ;
	if (un == nullptr) return SR_FAULT ;
	if (mdp == nullptr) return SR_FAULT ;
	ep->stime = st ;
	ep->type = mt ;
	if (mdl < 0) mdl = strlen(mdp) ;
	size += (mdl+1) ;
	size += (strlen(un)+1) ;
	if ((rs = lm_mall(size,&bp)) >= 0) {
	    ep->a = bp ;
	    ep->user = bp ;
	    bp = (strwcpy(bp,un,-1)+1) ;
	    ep->dbuf = bp ;
	    bp = (strwcpy(bp,mdp,mdl)+1) ;
	    ep->dlen = mdl ;
	} /* end if (m-a) */
	return rs ;
}
/* end subroutine (storenote_start) */

local int storenote_finish(STORENOTE *ep) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (ep->a != nullptr) {
	    rs1 = lm_free(ep->a) ;
	    if (rs >= 0) rs = rs1 ;
	    ep->a = nullptr ;
	    ep->user = nullptr ;
	    ep->dbuf = nullptr ;
	    ep->dlen = 0 ;
	}
	ep->stime = 0 ;
	ep->type = 0 ;
	return rs ;
}
/* end subroutine (storenote_finish) */

#if	CF_LOCMALSTRW
local int mallocstrw(cchar *sp,int sl,cchar **rpp) noex {
	int		rs ;
	char		*bp ;
	if (rpp == nullptr) return SR_FAULT ;
	if (sl < 0) sl = strlen(sp) ;
	if ((rs = lm_mall((sl+1),&bp)) >= 0) {
	    *rpp = bp ;
	    strwcpy(bp,sp,sl) ;
	} /* end if (m-a) */
	return rs ;
}
/* end subroutine (mallocstrw) */
#endif /* CF_LOCMALSTRW */

local int sdir(cchar *dname,int am) noex {
	cmode		dm = 0777 ;
	cint	nrs = SR_NOTFOUND ;
	int		rs ;
	int		f = false ;

	if (ustat sb ; (rs = uc_stat(dname,&sb)) == nrs) {
	    f = true ;
	    rs = mksdir(dname,dm) ;
	} else {
	    rs = perm(dname,-1,-1,nullptr,am) ;
	} /* end if (stat) */

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (sdir) */

local int mksdir(cchar *dname,mode_t dm) noex {
	int		rs ;
	if ((rs = mkdirs(dname,dm)) >= 0) {
	    rs = uc_minmod(dname,dm) ;
	}
	return rs ;
}
/* end if (mksdir) */

local int mksdname(char *rbuf,cchar *dname,pid_t sid) noex {
	const uint	uv = (uint) sid ;
	cint	dlen = DIGBUFLEN ;
	int		rs ;
	int		rl = 0 ;
	char		dbuf[DIGBUFLEN+1] = { 's' } ;

#if	CF_DEBUGN
	nprintf(NDF,"kshlib/mksdname: sid=%d\n",sid) ;
#endif

	if ((rs = ctdecui((dbuf+1),(dlen-1),uv)) >= 0) {
	    if ((rs = mkpath2(rbuf,dname,dbuf)) >= 0) {
	        cmode	dm = 0777 ;
		rl = rs ;
	        if ((rs = mkdirs(rbuf,dm)) >= 0) {
	            rs = uc_minmod(rbuf,dm) ;
	        }
	    } /* end if (mkpath) */
	} /* end if (ctdecui) */

#if	CF_DEBUGN
	nprintf(NDF,"kshlib/mksdname: ret rs=%d\n",rs) ;
#endif

	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (mksdname) */

#if	CF_DEBUGENV && CF_DEBUGN
local int ndebugenv(cchar *s,mainv ev) noex {
	if (s != nullptr) {
	    if (ev != nullptr) {
	        int	i ;
	        cchar	*dfn = NDF ;
		cchar	*ep ;
	        cchar	*fmt = "%s: e%03u=>%r<\n" ;
	        nprintf(dfn,"%s: envÂ¬\n", s) ;
	        for (i = 0 ; ev[i] != nullptr ; i += 1) {
	            ep = ev[i] ;
	            nprintf(dfn,fmt,s,i,ep,strlinelen(ep,-1,50)) ;
	        }
	        nprintf(dfn,"%s: nenv=%u\n", s,i) ;
	    } else {
	        nprintf(dfn,"%s: environ=*null*\n",s) ;
	    }
	}
	return 0 ;
}
/* end subroutine (ndebugenv) */
#endif /* CF_DEBUGENV */

#if	CF_DEBUGN

local int nprintpid(cchar *s) noex {
	const uint	id = getpid() ;
	return nprintf(NDF,"%s: ent pid=%u\n",s,id) ;
}

local int nprintid(cchar *s) noex {
	const pthread_t	tid = uptself(nullptr) ;
	return nprintf(NDF,"%s: tid=%u\n",s,tid) ;
}
/* end subroutine (nprintid) */

local int nprintutmp(char *s) noex {
	KSHLIB		*uip = &kshlib_data ;
	int		rs ;
	nprintf(NDF,"%s: UTMPACC test-begin\n",s) ;
	{
		UTMPACC_ENT	ue ;
		const pid_t	sid = getsid(0) ;
		const pid_t	pid = uip->pid ;
		cint	ulen = UTMPACC_BUFLEN ;
		char		ubuf[UTMPACC_BUFLEN+1] ;

	    nprintf(NDF,"%s: sid=%d pid=%u\n",s,sid,pid) ;
	    rs = utmpacc_entsid(&ue,ubuf,ulen,pid) ;
	    nprintf(NDF,"%s: utmpacc() rs=%d\n",s,rs) ;

	}
	nprintf(NDF,"%s: UTMPACC test-end\n",s) ;
	return rs ;
}
/* end subroutine (nprintutmp) */

#endif /* CF_DEBUGN */


