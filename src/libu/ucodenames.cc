/* ucodenames SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* return an abbreviation c-string given code-number */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	ucodenames

	Description:
	I search for and return (if found) a c-string of the
	abbreviation of the code-number given (simple).

	Synopsis:
	int ucodename_sr	(int n,cchar **rpp) noex
	int ucodename_sig	(int,n,cchar **rpp) noex

	Arguments:
	n		system-error return number to lookup
	rpp		pointer to pointer to result c-string

	Returns:
	>=0		length of returned c-string
	<0		not-found

	Notes:
	1. Of course, the code below might be more than expected
	beucase the search for the abbreviated c-string for a given
	code-number is done using a binary search.  That is the
	whole rub of this thing.  For a large number of code-name
	pairs, this is a big win.  For smaller numbers, not so much.
	2. The code below can only handle 256 entries in any of
	the data tables below.  If a table ever exceeds 256 entries,
	change the type of the sort-table from |uchar| to |ushort)
	to get more table-length range.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<algorithm>		/* C++STD |sort(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU |DECBUFLEN| */

#include	"ucodenames.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* local namespaces */

using std::sort ;			/* subroutine-template */
using std::partition_point ;		/* subroutine-template */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct codepair {
	int		n ;
	cchar		*s ;
} ; /* end struct */

namespace {
    struct getter {
	con codepair	*ns ;
	con uchar	*tab ;
	con uchar	*len ;
	int 		ne ;
	getter(con codepair *ªns,cuchar *ªtab,cuchar *ªlen,int n) noex {
	    ns = ªns ;
	    tab = ªtab ;
	    len = ªlen ;
	    ne = n ;
	} ; /* end ctor */
	int operator () (int n,cchar **rpp) const noex {
	    int		rs = SR_NOTFOUND ;
	    int		rl = 0 ; /* return-value */
	    cauto predf = [this,n] (uchar c) noex -> bool {
		cint i = int(c) ;
		return (ns[i].n < n) ;
	    } ; /* end lambda (predf) */
	    con uchar *itf = tab ;
	    con uchar *itl = (tab + ne) ;
	    if (cauto it = partition_point(itf,itl,predf) ; it != itl) {
		if (cint ii = *it ; ns[ii].n == n) {
		    rs = SR_OK ;
	    	    if (rpp) *rpp = ns[ii].s ;
	    	    rl = len[intconv(it - itf)] ;
		} /* end if (got a match) */
	    } /* end if (partition) */
	    return (rs >= 0) ? rl : rs ;
	} ; /* end method (operator) */
    } ; /* end struct (getter) */
} /* end namespace */


/* forward references */


/* local variables */

constexpr codepair		names_sr[] = {
	{ SR_PERM,		"PERM" },
	{ SR_NOENT,		"NOENT" },
	{ SR_SRCH,		"SRCH" },
	{ SR_INTR,		"INTR" },
	{ SR_IO,		"IO" },
	{ SR_NXIO,		"NXIO" },
	{ SR_2BIG,		"2BIG" },
	{ SR_NOEXEC,		"NOEXEC" },
	{ SR_BADF,		"BADF" },
	{ SR_CHILD,		"CHILD" },
	{ SR_AGAIN,		"AGAIN" },
	{ SR_NOMEM,		"NOMEM" },
	{ SR_ACCES,		"ACCES" },
	{ SR_FAULT,		"FAULT" },
	{ SR_NOTBLK,		"NOTBLK" },
	{ SR_BUSY,		"BUSY" },
	{ SR_EXIST,		"EXIST" },
	{ SR_XDEV,		"XDEV" },
	{ SR_NODEV,		"NODEV" },
	{ SR_NOTDIR,		"NOTDIR" },
	{ SR_ISDIR,		"ISDIR" },
	{ SR_INVAL,		"INVAL" },
	{ SR_NFILE,		"NFILE" },
	{ SR_MFILE,		"MFILE" },
	{ SR_NOTTY,		"NOTTY" },
	{ SR_TXTBSY,		"TXTBSY" },
	{ SR_FBIG,		"FBIG" },
	{ SR_NOSPC,		"NOSPC" },
	{ SR_SPIPE,		"NOTSEEK" },
	{ SR_ROFS,		"ROFS" },
	{ SR_MLINK,		"MLINK" },
	{ SR_PIPE,		"PIPE" },
	{ SR_DOM,		"DOM" },
	{ SR_RANGE,		"RANGE" },
	{ SR_NOMSG,		"NOMSG" },
	{ SR_IDRM,		"IDRM" },
	{ SR_CHRNG,		"CHRNG" },
	{ SR_L2NSYNC,		"L2NSYNC" },
	{ SR_L3HLT,		"L3HLT" },
	{ SR_L3RST,		"L3RST" },
	{ SR_LNRNG,		"LNRNG" },
	{ SR_UNATCH,		"UNATCH" },
	{ SR_NOCSI,		"NOCSI" },
	{ SR_L2HLT,		"L2HLT" },
	{ SR_DEADLK,		"DEADLK" },
	{ SR_NOLCK,		"NOLCK" },
	{ SR_CANCELED,		"CANCELED" },
	{ SR_NOTSUP,		"NOTSUP" },
	{ SR_DQUOT,		"DQUOT" },
	{ SR_BADE,		"BADE" },
	{ SR_BADR,		"BADR" },
	{ SR_XFULL,		"XFULL" },
	{ SR_NOANO,		"NOANO" },
	{ SR_BADRQC,		"BADRQC" },
	{ SR_BADSLT,		"BADSLT" },
	{ SR_DEADLOCK,		"DEADLOCK" },
	{ SR_BFONT,		"BFONT" },
	{ SR_OWNERDEAD,		"OWNERDEAD" },
	{ SR_NOTRECOVERABLE,	"NOTRECOVERABLE" },
	{ SR_NOSTR,		"NOSTR" },
	{ SR_NODATA,		"NODATA" },
	{ SR_TIME,		"TIME" },
	{ SR_NOSR,		"NOSR" },
	{ SR_NONET,		"NONET" },
	{ SR_NOPKG,		"NOPKG" },
	{ SR_REMOTE,		"REMOTE" },
	{ SR_NOLINK,		"NOLINK" },
	{ SR_NOATTR,		"NOATTR" },		/* Apple-Darwin */
	{ SR_ADV,		"ADV" },
	{ SR_SRMNT,		"SRMNT" },
	{ SR_COMM,		"COMM" },
	{ SR_PROTO,		"PROTO" },
	{ SR_MULTIHOP,		"MULTIHOP" },
	{ SR_BADMSG,		"BADMSG" },
	{ SR_NAMETOOLONG,	"NAMETOOLONG" },
	{ SR_OVERFLOW,		"OVERFLOW" },
	{ SR_NOTUNIQ,		"NOTUNIQ" },
	{ SR_BADFD,		"BADFD" },
	{ SR_REMCHG,		"REMCHG" },
	{ SR_LIBACC,		"LIBACC" },
	{ SR_LIBBAD,		"LIBBAD" },
	{ SR_LIBSCN,		"LIBSCN" },
	{ SR_LIBMAX,		"LIBMAX" },
	{ SR_LIBEXEC,		"LIBEXEC" },
	{ SR_ILSEQ,		"ILSEQ" },
	{ SR_NOSYS,		"NOSYS" },
	{ SR_LOOP,		"LOOP" },
	{ SR_RESTART,		"RESTART" },
	{ SR_STRPIPE,		"STRPIPE" },
	{ SR_NOTEMPTY,		"NOTEMPTY" },
	{ SR_USERS,		"USERS" },
	{ SR_NOTSOCK,		"NOTSOCK" },
	{ SR_DESTADDRREQ,	"DESTADDRREQ" },
	{ SR_MSGSIZE,		"MSGSIZE" },
	{ SR_PROTOTYPE,		"PROTOTYPE" },
	{ SR_NOPROTOOPT,	"NOPROTOOPT" },
	{ SR_PROTONOSUPPORT,	"PROTONOSUPPORT" },
	{ SR_SOCKTNOSUPPORT,	"SOCKTNOSUPPORT" },
	{ SR_OPNOTSUPP,		"OPNOTSUPP" },
	{ SR_PFNOSUPPORT,	"PFNOSUPPORT" },
	{ SR_AFNOSUPPORT,	"AFNOSUPPORT" },
	{ SR_ADDRINUSE,		"ADDRINUSE" },
	{ SR_ADDRNOTAVAIL,	"ADDRNOTAVAIL" },
	{ SR_NETDOWN,		"NETDOWN" },
	{ SR_NETUNREACH,	"NETUNREACH" },
	{ SR_NETRESET,		"NETRESET" },
	{ SR_CONNABORTED,	"CONNABORTED" },
	{ SR_CONNRESET,		"CONNRESET" },
	{ SR_NOBUFS,		"NOBUFS" },
	{ SR_ISCONN,		"ISCONN" },
	{ SR_NOTCONN,		"NOTCONN" },
	{ SR_SHUTDOWN,		"SHUTDOWN" },
	{ SR_TOOMANYREFS,	"TOOMANYREFS" },
	{ SR_TIMEDOUT,		"TIMEDOUT" },
	{ SR_CONNREFUSED,	"CONNREFUSED" },
	{ SR_HOSTDOWN,		"HOSTDOWN" },
	{ SR_HOSTUNREACH,	"HOSTUNREACH" },
	{ SR_WOULDBLOCK,	"WOULDBLOCK" },
	{ SR_ALREADY,		"ALREADY" },
	{ SR_INPROGRESS,	"INPROGRESS" },
	{ SR_STALE,		"STALE" },
	{ SR_BAD,		"BAD" },
	{ SR_EXIT,		"EXIT" },
	{ SR_NOENTRY,		"NOENTRY" },
	{ SR_NOTOPEN,		"NOTOPEN" },
	{ SR_WRONLY,		"WRONLY" },
	{ SR_RDONLY,		"RDONLY" },
	{ SR_NOTSEEK,		"NOTSEEK" },
	{ SR_ACCESS,		"ACCESS" },
	{ SR_INVALID,		"INVALID" },
	{ SR_EXISTS,		"EXISTS" },
	{ SR_LOCKED,		"LOCKED" },
	{ SR_INUSE,		"INUSE" },
	{ SR_LOCKFAIL,		"LOCKFAIL" },
	{ SR_HANGUP,		"HANGUP" },
	{ SR_POLLERR,		"POLLERR" },
	{ SR_TOOBIG,		"TOOBIG" },
	{ SR_BADFMT,		"BADFMT" },
	{ SR_FULL,		"FULL" },
	{ SR_EMPTY,		"EMPTY" },
	{ SR_EOF,		"EOF" },
	{ SR_NOEXIST,		"NOEXIST" },
	{ SR_NOTFOUND,		"NOTFOUND" },
	{ SR_BADREQUEST,	"BADREQUEST" },
	{ SR_NOTCONNECTED,	"NOTCONNECTED" },
	{ SR_OPEN,		"OPEN" },
	{ SR_OUT,		"OUT" },
	{ SR_NOTAVAIL,		"NOTAVAIL" },
	{ SR_BADSLOT,		"BADSLOT" },
	{ SR_SEARCH,		"SEARCH" },
	{ SR_NOANODE,		"NOANODE" },
	{ SR_BUGCHECK,		"BUGCHECK" },
	{ SR_LOOK,		"LOOK" },
	{ SR_DOWN,		"DOWN" },
	{ SR_UNAVAIL,		"UNAVAIL" },
	{ SR_BADRPC,		"BADRPC" },		/* Apple-Darwin */
	{ SR_RPCMISMATCH,	"RPCMISMATCH" },	/* Apple-Darwin */
	{ SR_PROGUNAVAIL,	"PROGUNAVAIL" },	/* Apple-Darwin */
	{ SR_PROGMISMATCH,	"PROGMISMATCH" },	/* Apple-Darwin */
	{ SR_PROCUNAVAIL,	"PROCUNAVAIL" },	/* Apple-Darwin */
	{ SR_FTYPE,		"FTYPE" },		/* Apple-Darwin */
	{ SR_AUTH,		"AUTH" },		/* Apple-Darwin */
	{ SR_NEEDAUTH,		"NEEDAUTH" },		/* Apple-Darwin */
	{ SR_PWROFF,		"PWROFF" },		/* Apple-Darwin */
	{ SR_DEVERR,		"DEVERR" },		/* Apple-Darwin */
	{ SR_BADEXEC,		"BADEXEC" },		/* Apple-Darwin */
	{ SR_BADARCH,		"BADARCH" },		/* Apple-Darwin */
	{ SR_SHLIBVERS,		"SHLIBVERS" },		/* Apple-Darwin */
	{ SR_BADMACHO,		"BADMACHO" },		/* Apple-Darwin */
	{ SR_PROCLIM,		"PROCLIM" },		/* Apple-Darwin */
	{ SR_NOPOLICY,		"NOPOLICY" },		/* Apple-Darwin */
	{ SR_QFULL,		"QFULL" },		/* Apple-Darwin */
	{ SR_TIMEOUT,		"TIMEOUT" },
	{ SR_CREATED,		"CREATED" },
	{ SR_OK,		"OK" }
} ; /* end array (names_sr) */

constexpr codepair		names_sig[] = {
	{ 0,			"TEST" },
	{ SIGHUP,		"HUP" },
	{ SIGINT,		"INT" },
	{ SIGQUIT,		"QUIT" },
	{ SIGILL,		"ILL" },
	{ SIGTRAP,		"TRAP" }, /* 5 */
	{ SIGABRT,		"ABRT" },
	{ SIGEMT,		"EMT" },
	{ SIGFPE,		"FPE" },
	{ SIGKILL,		"KILL" },
	{ SIGBUS,		"BUS" }, /* 10 */
	{ SIGSEGV,		"SEGV" },
	{ SIGSYS,		"SYS" },
	{ SIGPIPE,		"PIPE" },
	{ SIGALRM,		"ALRM" },
	{ SIGTERM,		"TERM" }, /* 15 */
	{ SIGUSR1,		"USR1" },
	{ SIGUSR2,		"USR2" },
	{ SIGCLD,		"CLD" },
	{ SIGCHLD,		"CHLD" },
	{ SIGPWR,		"PWR" }, /* 20 */
	{ SIGWINCH,		"WINCH" },
	{ SIGURG,		"URG" },
	{ SIGPOLL,		"POLL" },
	{ SIGSTOP,		"STOP" },
	{ SIGTSTP,		"TSTP" }, /* 25 */
	{ SIGCONT,		"CONT" },
	{ SIGTTIN,		"TTIN" },
	{ SIGTTOU,		"TTOU" },
	{ SIGVTALRM,		"VTALRM" },
	{ SIGPROF,		"PROF" }, /* 30 */
	{ SIGXCPU,		"XCPU" },
	{ SIGXFSZ,		"XFSZ" },
	{ SIGWAITING,		"WAITING" },
	{ SIGLWP,		"LWP" },
	{ SIGFREEZE,		"FREEZE" },
	{ SIGTHAW,		"THAW" },
	{ SIGCANCEL,		"CANCEL" },
	{ SIGLOST,		"LOST" },
	{ SIGRTMIN,		"RTMIN" },
	{ SIGRTMAX,		"RTMAX" }
} ; /* end array (names_sig) */

constexpr int	ne_sr	= nelem(names_sr) ;
constexpr int	ne_sig	= nelem(names_sig) ;

namespace {
    enum whichs {
	w_sr,
	w_sig,
	w_overlast
    } ; /* end enum */
    struct codemgr {
	uchar		tab_sr	[ne_sr] ;
	uchar		tab_sig	[ne_sig] ;
	uchar		len_sr	[ne_sr] ;
	uchar		len_sig	[ne_sig] ;
	consteval void tabload_x(mut uchar *tab,int n) noex {
	    for (int i = 0 ; i < n ; i += 1) {
		tab[i] = uchar(i) ;
	    } /* end for */
	} ; /* end method (tabload_x) */
	consteval void tabinit(int w,con codepair *pairs,int ne) noex {
	    uchar *tab{} ;
	    uchar *len{} ;
	    switch (w) {
	    case w_sr:
		tab = tab_sr ;
		len = len_sr ;
		break ;
	    case w_sig:
		tab = tab_sig ;
		len = len_sig ;
		break ;
	    } /* end switch */
	    cauto cmpf = [pairs] (con uchar &ia,con uchar &ib) noex -> bool {
    		return (pairs[ia].n < pairs[ib].n) ;
	    } ; /* end lambda */
	    tabload_x(tab,ne) ;
	    sort(tab,(tab+ne),cmpf) ;
	    for (int i = 0 ; i < ne ; i += 1) {
		len[i] = uchar(clenstr(pairs[tab[i]].s)) ;
	    } /* end for */
	} ; /* end method (tabinit) */
	consteval codemgr() noex {
	    tabinit(w_sr,	names_sr,	ne_sr) ;
	    tabinit(w_sig,	names_sig,	ne_sig) ;
	} /* end ctor (codemgr) */
    } ; /* end struct (codemgr) */
} /* end namespace */

constexpr codemgr	tabcode ;


/* exported variables */


/* exported subroutines */

int ucodename_sr(int n,ccharpp rpp) noex {
    	getter go(names_sr,tabcode.tab_sr,tabcode.len_sr,ne_sr) ;
    	return go(n,rpp) ;
} /* end subroutine */

int ucodename_sig(int n,ccharpp rpp) noex {
    	getter go(names_sig,tabcode.tab_sig,tabcode.len_sig,ne_sig) ;
    	return go(n,rpp) ;
} /* end subroutine */


/* local subroutines */


