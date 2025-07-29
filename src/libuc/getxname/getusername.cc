/* getusername SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the best approximation of the user's username */
/* version %I% last-modified %G% */

#define	CF_UTMPACC	1		/* use |utmpacc(3uc)| */
#define	CF_GETUTMPNAME	1		/* use |getutmpname(3dam)| */

/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	getusername

	[Description:
	Get the username given a UID, the best that we can.  These things
	have a certain level of confusion about them.  What is a
	'username'? After all of these years, one would think that
	we know the answer to that question!

	Note:
	We try to find the proper username in the order in which
	the user would expect.  Happily for us, the cheaper and
	more likely methods come first, and the heavier and more
	expensive methods come later (pretty much).  Although, in
	all cases a lookup into the system UCENTPW database is
	required (itself not always a cheap or fast operation).  In
	fact, a lookup to the system UCENTPW database can be much
	more expensive than a lookup into the system UTMPX database
	(otherwise thought to be more expensive than a UCENTPW 
	lookup).  We guard against making multiple UCENTPW database
	requests for the same name (to save time).

	The following are available:

	= GETXUSERNAME

	Synopsis:
	int getxusername(getxuser *xup,uid_t uid) noex

	Arguments:
	xup		pointer to special (private) data structure
	uid		user-id

	Returns:
	>=0		length of resulting username
	<0		error (system-return)

	= GETUSERNAME

	Synopsis:
	int getusername(char *ubuf,int ulen,uid_t uid) noex

	Arguments:
	ubuf		buffer to receive username
	ulen		length of supplied buffer
	uid		user-id

	Returns:
	>=0		length of resulting username
	<0		error (system-return)

	= GETPWUSERNAME

	Synopsis:
	int getpwusername(ucentpw *pwp,char *pwbuf,int pwlen,uid_t uid) noex

	Arguments:
	pwp		pointer to UCENTPW structure (to receive results)
	pwbuf		supplied buffer to hold information
	pwlen		length of supplied buffer
	uid		user-id

	Returns:
	>=0		length of resulting username
	<0		error (system-return)

	Notes:

	Q. Is this module multi-thread safe?
	A. Duh!  Of course.

	Q. Where are the traditional locks protecting the common data?
	A. None are needed.

	Q. How can no locks be needed and still be multi-thread safe?
	A. We forgo locks by accepting the (very) slight risk of the
	   code running more than once (once in each of one or more
	   separate threads).

	Q. Is this little scheme really OK?
	A. Yes.

	Q. I still feel uncomfortable.
	A. Deal with it.

	Implementation notes:
	Forst we try to look up the name is the local program cache.
	Failing the cache lookup, we go through various ways of
	guessing what our username is. Each time we guess a name,
	we have to verify it by looking it up in the system UCENTPW 
	database. We do that by calling the subroutines
	|getxusername_lookup()| below. As soon as a guess of a name
	is verified, we return the guess as the answer. Finally,
	when we find am answer, we put it into the local program
	cache (if we had not retrieved it from there in the first
	place).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* <- for |uid_t| */
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<pwd.h>
#include	<usystem.h>
#include	<ucgetpid.h>
#include	<ucpwcache.h>		/* |ucpwcache_name(3uc)| */
#include	<ucproguser.h>
#include	<getutmpent.h>		/* <- for |getutmpname(3uc)| */
#include	<getax.h>
#include	<getpwx.h>
#include	<getbufsize.h>
#include	<mallocxx.h>
#include	<utmpacc.h>
#include	<varnames.hh>
#include	<vecstr.h>
#include	<strlibval.hh>
#include	<sfx.h>
#include	<snx.h>
#include	<sncpyx.h>
#include	<snwcpy.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"getusername.h"


/* local defines */

#define	GETXSTATE	getxusername_state

#define	DEBFNAME	"/var/tmp/debuguid.txt"

#ifndef	CF_UTMPACC
#define	CF_UTMPACC	0
#endif

#ifndef	CF_GETUTMPNAME
#define	CF_GETUTMPNAME	0
#endif


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef int (*getxuser_f)(getxuser *) noex ;


/* external subroutines */

extern "C" {
    int		getxusername(getxuser *) noex ;
}


/* external variables */


/* local structures */

struct mapent {
	cchar		*name ;
	uid_t		uid ;
} ;


/* forward references */

static int	getusernamer(char *,int,uid_t) noex ;

static int	getxusername_self(getxuser *) noex ;
static int	getxusername_varenv(getxuser *) noex ;
static int	getxusername_utmp(getxuser *) noex ;
static int	getxusername_map(getxuser *) noex ;
static int	getxusername_uid(getxuser *) noex ;

static int	getxusername_varbase(getxuser *,cchar *) noex ;
static int	getxusername_lookup(getxuser *,cchar *) noex ;


/* local variables */

constexpr uid_t		uidend = -1 ;

constexpr getxuser_f	getxusernames[] = {
	getxusername_self,
	getxusername_varenv,
	getxusername_utmp,
	getxusername_map,
	getxusername_uid,
	nullptr
} ;

constexpr mapent	mapents[] = {
	{ "root",	0 },
	{ "nobody",	60001 },
	{ "noaccess",	60002 },
	{ "nobody4",	65534 },
	{ nullptr,	uidend }
} ;

constexpr strlibvals	strusers[] = {
	strlibval_username,
	strlibval_user,
	strlibval_logname,
	strlibval_home,
	strlibval_mail,
	strlibval_overlast
} ;

static strlibval	var_username(strlibval_username) ;
static strlibval	var_user(strlibval_user) ;
static strlibval	var_logname(strlibval_logname) ;
static strlibval	var_home(strlibval_home) ;
static strlibval	var_mail(strlibval_mail) ;

constexpr bool		f_utmpacc = CF_UTMPACC ;
constexpr bool		f_getutmpname = CF_GETUTMPNAME ;


/* exported variables */


/* exported subroutines */

int getusername(char *ubuf,int ulen,uid_t uid) noex {
	int		rs = SR_FAULT ;
	if (ubuf) {
	    rs = SR_INVALID ;
	    ubuf[0] = '\0' ;
	    if (ulen > 0) {
	        if ((rs = ucproguser_nameget(ubuf,ulen,uid)) == 0) {
	            rs = getusernamer(ubuf,ulen,uid) ;
	        } /* end if (ucproguser_nameget) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (getusername) */

int getpwusername(ucentpw *pwp,char *pwbuf,int pwlen,uid_t uid) noex {
	int		rs = SR_FAULT ;
	if (pwp && pwbuf) {
	    rs = SR_INVALID ;
	    if (pwlen > 0) {
	        if ((rs = getbufsize(getbufsize_un)) >= 0) {
		    getxuser	xu{} ;
		    cint	ulen = rs ;
		    char	ubuf[rs + 1] ;	/* <- on the stack */
	            xu.pwp = pwp ;
	            xu.pwbuf = pwbuf ;
	            xu.pwlen = pwlen ;
	            xu.ubuf = ubuf ;
	            xu.ulen = ulen ;
	            xu.uid = uid ;
	            rs = getxusername(&xu) ;
	        } /* end if (getbufsize) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (getpwusername) */

int getxusername(getxuser *xup) noex {
	cint		ttl = GETXUSER_TTL ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		pwl = 0 ;
	if (xup) {
	    if (xup->pwp && xup->ubuf) {
		rs = SR_NOMEM ;
		if ((xup->nlp = new(nothrow) vecstr) != nullptr) {
	            xup->ubuf[0] = '\0' ;
	            if (xup->uid == uidend) {
	                xup->f_self = true ;
	                xup->uid = getuid() ;
	            } else {
	                const uid_t	suid = getuid() ;
	                xup->f_self = (xup->uid == suid) ;
	            }
	            if ((rs = vecstr_start(xup->nlp,10,0)) >= 0) {
	                for (int i = 0 ; getxusernames[i] ; i += 1) {
			    getxuser_f	f = getxusernames[i] ;
	                    rs = f(xup) ;
	                    if (rs != 0) break ;
	                } /* end for */
	                pwl = rs ;
	                if ((rs > 0) && xup->f_self) {
			    auto	upu = ucproguser_nameset ;
		            ucentpw	*pwp = xup->pwp ;
		            rs = upu(pwp->pw_name,-1,xup->uid,ttl) ;
	                } /* end if (cache store) */
	                rs1 = vecstr_finish(xup->nlp) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (vecstr) */
	            if ((rs >= 0) && (pwl == 0)) rs = SR_NOTFOUND ;
		    delete xup->nlp ;
		    xup->nlp = nullptr ;
		} /* end if (new-vecstr) */
	    } /* end if (non-null) */
	} /* end if (non-null) */
	return (rs >= 0) ? pwl : rs ;
}
/* end subroutine (getxusername) */


/* local subroutines */

static int getusernamer(char *ubuf,int ulen,uid_t uid) noex {
	int		rs ;
	int		rs1 ;
	if (char *pwbuf ; (rs = malloc_pw(&pwbuf)) >= 0) {
	    ucentpw	pw ;
	    cint	pwlen = rs ;
	    getxuser	xu{} ;
	    xu.pwp = &pw ;
	    xu.pwbuf = pwbuf ;
	    xu.pwlen = pwlen ;
	    xu.ubuf = ubuf ;
	    xu.ulen = ulen ;
	    xu.uid = uid ;
	    xu.f_tried = true ;
	    if ((rs = getxusername(&xu)) >= 0) {
	        rs = xu.unl ;
	        if (xu.unl <= 0) {
	            rs = sncpy1(ubuf,ulen,pw.pw_name) ;
	        }
	    } else if (rs == SR_NOTFOUND) {
	        uint	v = xu.uid ;
	        rs = snsd(ubuf,ulen,"U",v) ;
	    }
	    rs1 = uc_free(pwbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end subroutine (getusernamer) */

static int getxusername_self(getxuser *xup) noex {
	int		rs = SR_OK ;
	xup->unl = 0 ;
	if (xup->f_self && (! xup->f_tried)) {
	    if ((rs = ucproguser_nameget(xup->ubuf,xup->ulen,xup->uid)) > 0) {
		xup->unl = rs ;
		rs = getxusername_lookup(xup,xup->ubuf) ;
	    } /* end if (ucproguser_nameget) */
	} /* end if (self) */
	return rs ;
}
/* end subroutine (getxusername_self) */

static int getxusername_varenv(getxuser *xup) noex {
	int		rs = SR_OK ;
	for (int i = 0 ; strusers[i] < strlibval_overlast ; i += 1) {
	    strlibvals	sv = strusers[i] ;
	    cchar	*vv = nullptr ;
	    switch (sv) {
	    case strlibval_username:
		vv = var_username ;
		break ;
	    case strlibval_user:
		vv = var_user ;
		break ;
	    case strlibval_logname:
		vv = var_logname ;
		break ;
	    case strlibval_home:
		vv = var_home ;
		break ;
	    case strlibval_mail:
		vv = var_mail ;
		break ;
	    default:
		rs = SR_BUGCHECK ;
		break ;
	    } /* end switch */
	    if ((rs >= 0) && vv) {
		rs = getxusername_varbase(xup,vv) ;
	    }
	    if (rs != 0) break ;
	} /* end for */
	return rs ;
}
/* end subroutine (getxusername_varenv) */

static int getxusername_utmp(getxuser *xup) noex {
	int		rs = SR_OK ;
	if_constexpr (f_utmpacc) {
	    if ((rs = utmpacc_entbuflen) >= 0) {
	        utmpacc_ent	ue{} ; 
	        cint		uelen = rs ;
		if (char *uebuf ; (rs = uc_malloc((uelen+1),&uebuf)) >= 0) {
	            if ((rs = utmpacc_entsid(&ue,uebuf,uelen,0)) >= 0) {
		        if (ue.user != nullptr) {
		            rs = sncpy(xup->ubuf,xup->ulen,ue.user) ;
		        } else {
		            rs = SR_NOTFOUND ;
		            xup->ubuf[0] = '\0' ;
		        }
		    } /* end if (utmpacc_entsid) */
		    rs = rsfree(rs,uebuf) ;
	        } /* end if (m-a-f) */
	    } /* end if (utmpacc_entbuflen) */
	} else {
	    if_constexpr (f_getutmpname) {
	        rs = getutmpname(xup->ubuf,xup->ulen,0) ;
	    } else {
	        rs = uc_getlogin(xup->ubuf,xup->ulen) ;
	    } /* end if_constexpr (f_getutmpname) */
	} /* end if_constexpr (f_utmpacc) */
	xup->unl = rs ;
	if (rs >= 0) {
	    cchar	*sp = xup->ubuf ;
	    if (*sp != '\0') {
	        rs = getxusername_lookup(xup,sp) ;
	    } else {
	        rs = SR_OK ;
	    }
	} else if (isNotPresent(rs)) {
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (getxusername_utmp) */

static int getxusername_map(getxuser *xup) noex {
	const uid_t	uid = xup->uid ;
	int		rs = SR_OK ;
	int		i{} ; /* used-afterwards */
	int		f = false ;
	xup->unl = 0 ;
	for (i = 0 ; mapents[i].uid != uidend ; i += 1) {
	    f = (uid == mapents[i].uid) ;
	    if (f) break ;
	} /* end for */
	if (f) {
	    cchar	*sp = mapents[i].name ;
	    rs = getxusername_lookup(xup,sp) ;
	}
	return rs ;
}
/* end subroutine (getxusername_map) */

static int getxusername_uid(getxuser *xup) noex {
	int		rs ;
	xup->unl = 0 ;
	if ((rs = getpwx_uid(xup->pwp,xup->pwbuf,xup->pwlen,xup->uid)) >= 0) {
	    if (xup->pwp->pw_name[0] != '\0') {
		xup->pwl = rs ;
	    } else {
		rs = SR_OK ;
	    }
	} else if (isNotPresent(rs)) {
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (getxusername_uid) */

static int getxusername_varbase(getxuser *xup,cchar *vv) noex {
	int		rs = SR_OK ;
	if (vv) {
	    cchar	*sp{} ;
	    if (int sl ; (sl = sfbasename(vv,-1,&sp)) > 0) {
	        while ((sl > 0) && (sp[sl - 1] == '/')) {
	            sl -= 1 ;
	        }
	        if ((sl > 0) && (sp[0] != '/')) {
	            if ((rs = snwcpy(xup->ubuf,xup->ulen,sp,sl)) >= 0) {
	                xup->unl = rs ;
	                sp = xup->ubuf ;
	                rs = getxusername_lookup(xup,sp) ;
	            }
	        }
	    } /* end if */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (getxusername_varbase) */

static int getxusername_lookup(getxuser *xup,cchar *sp) noex {
	int		rs ;
	int		pwl = 0 ;
	if ((rs = vecstr_find(xup->nlp,sp)) == SR_NOTFOUND) {
	    cint	pwlen = xup->pwlen ;
	    char	*pwbuf = xup->pwbuf ;
	    if ((rs = getpwx_name(xup->pwp,pwbuf,pwlen,sp)) >= 0) {
	        if (xup->pwp->pw_uid == xup->uid) {
		    xup->pwl = rs ;
	            pwl = rs ;
		} else {
		    rs = SR_NOTFOUND ;
		}
	    } /* end if (getpwx_name) */
	    if (rs == SR_NOTFOUND) {
	        rs = vecstr_add(xup->nlp,sp,-1) ;
	    }
	} /* end if (search) */
	return (rs >= 0) ? pwl : rs ;
}
/* end subroutine (getxusername_lookup) */

#ifdef	COMMENT
static int logpop(uid_t uid) noex {
	time_t		daytime = time(nullptr) ;
	uid_t		pid = ugetpid() ;
	int		rs ;
	cchar		*pp = getexecname() ;
	cchar		*fmt ;
	char		timebuf[TIMEBUFLEN + 1] ;
	timestr_logz(daytime,timebuf),
	fmt = "%-23s p=%d u=%u ef=%s\n" ;
	rs = nprintf(DEBFNAME,fmt,timebuf,pid,uid,pp) ;
	return rs ;
}
/* end subroutine (logpop) */
#endif /* COMMENT */


