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
	'username'?  After all of these years, one would think that
	we know the answer to that question!

	Note:
	I try to find the proper username in the order in which the
	user would expect.  Happily for everyone, the cheaper and
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
	Forst I try to look up the name is the local program cache.
	Failing the cache lookup, I go through various ways of
	guessing what our username is.  Each time I guess a name,
	I have to verify it by looking it up in the system UCENTPW 
	database.  I do that by calling the subroutine
	|getxusername_lookup()| below.  As soon as a guess of a name
	is verified, I return the guess as the answer.  Finally,
	when I find am answer, I put it into the local program
	cache (if I had not retrieved it from there in the first
	place).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* <- for |uid_t| */
#include	<unistd.h>
#include	<fcntl.h>
#include	<pwd.h>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<uclibmem.h>
#include	<ucgetpid.h>
#include	<ucproguser.h>
#include	<ucentpw.h>
#include	<ucpwcache.h>		/* |ucpwcache_name(3uc)| */
#include	<getax.h>
#include	<getpwx.h>
#include	<getbufsize.h>
#include	<getutmpent.h>		/* <- for |getutmpname(3uc)| */
#include	<aflag.hh>
#include	<utmpacc.h>
#include	<strlibval.hh>
#include	<sfx.h>
#include	<snx.h>
#include	<sncpyx.h>
#include	<snwcpy.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"getusername.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/ureserve.ccm"

import libutil ;			/* |lenstr(3u)| */
import ureserve ;			/* |vecstr(3u)| */

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

using std::nothrow ;			/* constant */


/* local typedefs */

typedef int (*getxuser_f)(getxuser *) noex ;
typedef vecstr *		vecstrp ;


/* external subroutines */

extern "C" {
    extern int uc_getlogin(char *,int) noex ;
}

extern "C" {
    int		getxusername(getxuser *) noex ;
}


/* external variables */


/* local structures */

struct mapent {
	cchar		*name ;
	uid_t		uid ;
} ; /* end struct (mapent) */


/* forward references */

local int	getusernamer(char *,int,uid_t) noex ;

local int	getxusername_self(getxuser *) noex ;
local int	getxusername_varenv(getxuser *) noex ;
local int	getxusername_utmp(getxuser *) noex ;
local int	getxusername_map(getxuser *) noex ;
local int	getxusername_uid(getxuser *) noex ;

local int	getxusername_varbase(getxuser *,cchar *) noex ;
local int	getxusername_lookup(getxuser *,cchar *) noex ;


/* local variables */

constexpr uid_t		uidend = -1 ;

constexpr getxuser_f	getxusernames[] = {
	getxusername_self,
	getxusername_varenv,
	getxusername_utmp,
	getxusername_map,
	getxusername_uid,
	nullptr
} ; /* end array (getxusernames) */

constexpr mapent	mapents[] = {
	{ "root",	0 },
	{ "nobody",	60001 },
	{ "noaccess",	60002 },
	{ "nobody4",	65534 },
	{ nullptr,	uidend }
} ; /* end array (mapents) */

constexpr strlibvals	strusers[] = {
	strlibval_username,
	strlibval_user,
	strlibval_logname,
	strlibval_home,
	strlibval_mail,
	strlibval_overlast
} ; /* end array (strusers) */

static strlibval	var_username	(strlibval_username) ;
static strlibval	var_user	(strlibval_user) ;
static strlibval	var_logname	(strlibval_logname) ;
static strlibval	var_home	(strlibval_home) ;
static strlibval	var_mail	(strlibval_mail) ;

constexpr bool		f_utmpacc	= CF_UTMPACC ;
constexpr bool		f_getutmpname	= CF_GETUTMPNAME ;


/* exported variables */


/* exported subroutines */

int getusername(char *ubuf,int ulen,uid_t uid) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ; /* return-value */
	if (ubuf) ylikely {
	    rs = SR_INVALID ;
	    if (ulen > 0) ylikely {
	        ubuf[0] = '\0' ;
	        if ((rs = ucproguser_nameget(ubuf,ulen,uid)) == 0) {
	            rs = getusernamer(ubuf,ulen,uid) ;
		    len = rs ;
	        } /* end if (ucproguser_nameget) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (getusername) */

int getpwusername(ucentpw *pwp,char *pwbuf,int pwlen,uid_t uid) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ; /* return-value */
	if (pwp && pwbuf) ylikely {
	    rs = SR_INVALID ;
	    if (pwlen > 0) ylikely {
	        if ((rs = getbufsize(bufsize_un)) >= 0) ylikely {
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
		    len = rs ;
	        } /* end if (getbufsize) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (getpwusername) */

local int getxusername_µprep(getxuser *) noex ;
local int getxusername_µtryer(getxuser *) noex ;
local int getxusername_µcache(getxuser *) noex ;

int getxusername(getxuser *xup) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ; /* return-value */
	if (xup) ylikely {
	    if (xup->pwp && xup->ubuf) ylikely {
		if ((rs = getxusername_µprep(xup)) >= 0) ylikely {
		    if ((rs = getxusername_µtryer(xup)) > 0) ylikely {
		        len = rs ;
		        rs = getxusername_µcache(xup) ;
		    } /* end if (got one) */
		} /* end if (preparation) */
	    } /* end if (non-null) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (getxusername) */


/* local subroutines */

local int getusernamer(char *ubuf,int ulen,uid_t uid) noex {
	int		rs ;
	int		rs1 ;
	int		len = 0 ; /* return-value */
	if (char *pwbuf ; (rs = lm_pw(&pwbuf)) >= 0) ylikely {
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
		len = rs ;
	        rs = xu.unl ;
	        if (xu.unl <= 0) {
	            rs = sncpy(ubuf,ulen,pw.pw_name) ;
		    len = rs ;
	        }
	    } else if (rs == SR_NOTFOUND) {
	        uint	v = xu.uid ;
	        rs = snsd(ubuf,ulen,"U",v) ;
		len = rs ;
	    } /* end if */
	    rs1 = lm_free(pwbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (getusernamer) */

local int getxusername_µprep(getxuser *xup) noex {
	int		rs = SR_OK ;
	xup->ubuf[0] = '\0' ;
	if (xup->uid == uidend) {
	    xup->f_self = true ;
	    xup->uid = getuid() ;
	} else {
	    const uid_t	suid = getuid() ;
	    xup->f_self = (xup->uid == suid) ;
	}
	return rs ;
} /* end subroutine (getxusername_µprep) */

local int getxusername_µtryer(getxuser *xup) noex {
    	cnullptr	np{} ;
	cnothrow	nt{} ;
    	int		rs = SR_NOMEM ;
	int		rs1 ;
	int		len = 0 ; /* return-value */
        if (vecstr *nlp ; (nlp = new(nt) vecstr) != np) ylikely {
	    cint	vn = 10 ;
	    xup->nlp = nlp ;
            if ((rs = nlp->start(vn)) >= 0) ylikely {
		{
                    for (int i = 0 ; getxusernames[i] ; i += 1) {
                        getxuser_f  fun = getxusernames[i] ;
                        rs = fun(xup) ;
                        if (rs != 0) break ;
                    } /* end for */
                    len = rs ;
		} /* end block */
                rs1 = nlp->finish ;
                if (rs >= 0) rs = rs1 ;
            } /* end if (vecstr) */
            if ((rs >= 0) && (len == 0)) {
		rs = SR_NOTFOUND ;
	    }
            delete nlp ;
            xup->nlp = nullptr ;
        } /* end if (new-vecstr) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (getxusername_µtryer) */

local int getxusername_µcache(getxuser *xup) noex {
    	int		rs = SR_OK ;
        if (xup->f_self) {
	    cint	ttl = GETXUSER_TTL ;
            cauto       upu = ucproguser_nameset ;
            ucentpw     *pwp = xup->pwp ;
            rs = upu(pwp->pw_name,-1,xup->uid,ttl) ;
        } /* end if (cache store) */
	return rs ;
} /* end subroutine (getxusername_µcache) */

local int getxusername_self(getxuser *xup) noex {
	int		rs = SR_OK ;
	int		len = 0 ; /* return-value */
	xup->unl = 0 ;
	if (xup->f_self && (! xup->f_tried)) {
	    char	*ubuf = xup->ubuf ;
	    cint	ulen = xup->ulen ;
	    const uid_t	uid = xup->uid ;
	    if ((rs = ucproguser_nameget(ubuf,ulen,uid)) > 0) {
		xup->unl = rs ;
		len = rs ;
		if ((rs = getxusername_lookup(xup,xup->ubuf)) == 0) {
		    len = 0 ;
		}
	    } /* end if (ucproguser_nameget) */
	} /* end if (self) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (getxusername_self) */

local int getxusername_varenv(getxuser *xup) noex {
	int		rs = SR_OK ;
	int		len = 0 ; /* return-value */
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
		len = rs ;
	    }
	    if (rs != 0) break ;
	} /* end for */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (getxusername_varenv) */

local int getxusername_utmp(getxuser *xup) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		len = 0 ; /* return-value */
	if_constexpr (f_utmpacc) {
	    if ((rs = utmpacc_entbuflen) >= 0) {
	        utmpacc_ent	ue{} ; 
	        cint		uelen = rs ;
		if (char *uebuf ; (rs = lm_mall((uelen+1),&uebuf)) >= 0) {
	            if ((rs = utmpacc_entsid(&ue,uebuf,uelen,0)) >= 0) {
		        if (ue.user != nullptr) {
		            rs = sncpy(xup->ubuf,xup->ulen,ue.user) ;
			    len = rs ;
		        } else {
		            rs = SR_NOTFOUND ;
		            xup->ubuf[0] = '\0' ;
		        }
		    } /* end if (utmpacc_entsid) */
		    rs1 = lm_free(uebuf) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (utmpacc_entbuflen) */
	} else {
	    if_constexpr (f_getutmpname) {
	        rs = getutmpname(xup->ubuf,xup->ulen,0) ;
		len = rs ;
	    } else {
	        rs = uc_getlogin(xup->ubuf,xup->ulen) ;
		len = rs ;
	    } /* end if_constexpr (f_getutmpname) */
	} /* end if_constexpr (f_utmpacc) */
	xup->unl = rs ;
	if (rs >= 0) ylikely {
	    cchar	*sp = xup->ubuf ;
	    if (*sp != '\0') {
	        if ((rs = getxusername_lookup(xup,sp)) == 0) {
		    len = 0 ;
		}
	    } else {
		len = 0 ;
	        rs = SR_OK ;
	    }
	} else if (isNotPresent(rs)) {
	    len = 0 ;
	    rs = SR_OK ;
	}
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (getxusername_utmp) */

local int getxusername_map(getxuser *xup) noex {
	const uid_t	uid = xup->uid ;
	int		rs = SR_OK ;
	int		len = 0 ; /* return-value */
	int		i{} ; /* used-afterwards */
	int		f = false ;
	xup->unl = 0 ;
	for (i = 0 ; mapents[i].uid != uidend ; i += 1) {
	    f = (uid == mapents[i].uid) ;
	    if (f) break ;
	} /* end for */
	if (f) {
	    cchar	*sp = mapents[i].name ;
	    if ((rs = getxusername_lookup(xup,sp)) > 0) {
		len = lenstr(sp) ;
	    }
	} /* end if (match) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (getxusername_map) */

local int getxusername_uid(getxuser *xup) noex {
    	char		*pwbuf = xup->pwbuf ;
    	cint		pwlen = xup->pwlen ;
	const uid_t	uid = xup->uid ;
	int		rs ;
	int		len = 0 ; /* return-value */
	xup->unl = 0 ;
	if ((rs = getpwx_uid(xup->pwp,pwbuf,pwlen,uid)) >= 0) {
	    if (xup->pwp->pw_name[0] != '\0') {
		xup->pwl = rs ;
		len = lenstr(xup->pwp->pw_name) ;
	    } else {
		rs = SR_OK ;
	    }
	} else if (isNotPresent(rs)) {
	    rs = SR_OK ;
	}
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (getxusername_uid) */

local int getxusername_varbase(getxuser *xup,cchar *vv) noex {
	int		rs = SR_OK ;
	int		len = 0 ; /* return-value */
	if (vv) {
	    cchar	*sp{} ;
	    if (int sl ; (sl = sfbasename(vv,-1,&sp)) > 0) {
	        while ((sl > 0) && (sp[sl - 1] == '/')) {
	            sl -= 1 ;
	        }
	        if ((sl > 0) && (sp[0] != '/')) {
	            if ((rs = snwcpy(xup->ubuf,xup->ulen,sp,sl)) >= 0) {
			len = rs ;
	                xup->unl = rs ;
	                sp = xup->ubuf ;
	                if ((rs = getxusername_lookup(xup,sp)) == 0) {
			    len = 0 ;
			}
	            } /* end if (possible) */
	        } /* end if (possible base-name) */
	    } /* end if */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (getxusername_varbase) */

local int getxusername_lookup(getxuser *xup,cchar *sp) noex {
    	vecstr		*nlp = vecstrp(xup->nlp) ;
	int		rs ;
	int		rv = 0 ; /* return-value */
	if ((rs = nlp->find(sp)) == SR_NOTFOUND) {
	    cint	pwlen = xup->pwlen ;
	    char	*pwbuf = xup->pwbuf ;
	    if ((rs = getpwx_name(xup->pwp,pwbuf,pwlen,sp)) >= 0) {
	        if (xup->pwp->pw_uid == xup->uid) {
		    xup->pwl = rs ;
	            rv = rs ;
		} else {
		    rs = SR_NOTFOUND ;
		}
	    } /* end if (getpwx_name) */
	    if (rs == SR_NOTFOUND) {
	        rs = nlp->add(sp) ;
	    }
	} /* end if (search) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (getxusername_lookup) */

#ifdef	COMMENT
local int logpop(uid_t uid) noex {
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


