/* getxusername SUPPORT */
/* lang=C20 */

/* get the best approximation of the user's username */
/* version %I% last-modified %G% */

#define	CF_UTMPACC	1		/* use |utmpacc(3uc)| */
#define	CF_GETUTMPNAME	1		/* use |getutmpname(3dam)| */
#define	CF_UGETPW	1		/* use |ugetpw(3uc)| */

/* revision history:

	= 1998-07-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Get the user's username, the best that we can. These things
	have a certain level of confusion about them. What is a
	'username'? After all of these years, one would think that
	we know the answer to that question!

	Note:

	We try to find the proper username in the order in which
	the user would expect.  Happily for us, the cheaper and
	more likely methods come first, and the heavier and more
	expensive methods come later (pretty much).  Although, in
	all cases a lookup into the system PASSWD database is
	required (itself not always a cheap or fast operation).  In
	fact, a lookup to the system PASSWD database can be much
	more expensive than a lookup into the system UTMPX database
	(otherwise thought to be more expensive than a PASSWD
	lookup).  We guard against making multiple PASSWD database
	requests for the same name (to save time).

	The following are available:

	= GETXUSERNAME

	Synopsis:
	int getxusername(GETXUSERNAME *xup,uid_t uid) noex

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
	int getpwusername(PASSWD *pwp,char *pwbuf,int pwlen,uid_t uid) noex

	Arguments:
	pwp		pointer to PASSWD structure (to receive results)
	pwbuf		supplied buffer to hold information
	pwlen		length of supplied buffer
	uid		user-id

	Returns:
	>=0		length of resulting username
	<0		error

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
	we have to verify it by looking it up in the system PASSWD
	database. We do that by calling the subroutines
	|getxusername_lookup()| below. As soon as a guess of a name
	is verified, we return the guess as the answer. Finally,
	when we find am answer, we put it into the local program
	cache (if we had not retrieved it from there in the first
	place).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstdlib>
#include	<cstring>
#include	<ctime>
#include	<pwd.h>
#include	<usystem.h>
#include	<usupport.h>
#include	<ugetpid.h>
#include	<getbufsize.h>
#include	<vecstr.h>
#include	<getax.h>
#include	<uproguser.h>
#include	<ugetpw.h>
#include	<utmpacc.h>
#include	<localmisc.h>

#include	"getxusername.h"


/* local defines */

#if	CF_UGETPW
#define	GETPW_NAME	ugetpw_name
#define	GETPW_UID	ugetpw_uid
#else
#define	GETPW_NAME	getpw_name
#define	GETPW_UID	getpw_uid
#endif /* CF_UGETPW */

#ifndef	TIMEBUFLEN
#define	TIMEBUFLEN	80
#endif

#ifndef	LOGNAMELEN
#ifdef	LOGNAME_MAX
#define	LOGNAMELEN	LOGNAME_MAX
#else
#define	LOGNAMELEN	32
#endif
#endif

#ifndef	USERNAMELEN
#ifdef	LOGNAME_MAX
#define	USERNAMELEN	LOGNAME_MAX
#else
#define	USERNAMELEN	32
#endif
#endif

#ifndef	VARHOME
#define	VARHOME		"HOME"
#endif

#ifndef	VARMAIL
#define	VARMAIL		"MAIL"
#endif

#ifndef	VARUSERNAME
#define	VARUSERNAME	"USERNAME"
#endif

#ifndef	VARUSER
#define	VARUSER		"USER"
#endif

#ifndef	VARLOGNAME
#define	VARLOGNAME	"LOGNAME"
#endif

#define	GETXSTATE	struct getxusername_state

#define	DEBFNAME	"/var/tmp/debuguid.txt"


/* local namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    int		getxusername(GETXUSERNAME *) noex ;
}


/* external variables */


/* local structures */

struct mapent {
	cchar		*name ;
	uid_t		uid ;
} ;


/* forward references */

static int	getusernamer(char *,int,uid_t) noex ;

static int	getxusername_self(GETXUSERNAME *) noex ;
static int	getxusername_varusername(GETXUSERNAME *) noex ;
static int	getxusername_varuser(GETXUSERNAME *) noex ;
static int	getxusername_home(GETXUSERNAME *) noex ;
static int	getxusername_mail(GETXUSERNAME *) noex ;
static int	getxusername_varlogname(GETXUSERNAME *) noex ;
static int	getxusername_utmp(GETXUSERNAME *) noex ;
static int	getxusername_map(GETXUSERNAME *) noex ;
static int	getxusername_uid(GETXUSERNAME *) noex ;

static int	getxusername_var(GETXUSERNAME *,cchar *) noex ;
static int	getxusername_varbase(GETXUSERNAME *,cchar *) noex ;
static int	getxusername_lookup(GETXUSERNAME *,cchar *) noex ;


/* local variables */

static int	(*getxusernames[])(GETXUSERNAME *) = {
	getxusername_self,
	getxusername_varusername,
	getxusername_varuser,
	getxusername_home,
	getxusername_mail,
	getxusername_varlogname,
	getxusername_utmp,
	getxusername_map,
	getxusername_uid,
	nullptr
} ;

static const struct mapent	mapents[] = {
	{ 0, "root" },
	{ 60001, "nobody" },
	{ 60002, "noaccess" },
	{ 65534, "nobody4" },
	{ -1, nullptr }
} ;


/* exported subroutines */

int getusername(char *ubuf,int ulen,uid_t uid) noex {
	int		rs = SR_FAULT ;
	if (ubuf) {
	    rs = SR_INVALID ;
	    if (ulen > 0) {
	        ubuf[0] = '\0' ;
	        if ((rs = uproguser_nameget(ubuf,ulen,uid)) == 0) {
	            rs = getusernamer(ubuf,ulen,uid) ;
	        } /* end if (uproguser_nameget) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (getusername) */

int getpwusername(PASSWD *pwp,char *pwbuf,int pwlen,uid_t uid) noex {
	GETXUSERNAME	xu ;
	int		rs ;
	char		ubuf[USERNAMELEN + 1] ;

	if (pwp == nullptr) return SR_FAULT ;
	if (pwbuf == nullptr) return SR_FAULT ;

	memclear(&xu) ;
	xu.pwp = pwp ;
	xu.pwbuf = pwbuf ;
	xu.pwlen = pwlen ;
	xu.ubuf = ubuf ;
	xu.ulen = USERNAMELEN ;
	xu.uid = uid ;

	rs = getxusername(&xu) ;

	return rs ;
}
/* end subroutine (getpwusername) */

int getxusername(GETXUSERNAME *xup) noex {
	cint	ttl = GETXUSERNAME_TTL ;
	int		rs ;
	int		rs1 ;
	int		pwl = 0 ;

	if (xup == nullptr) return SR_FAULT ;
	if (xup->pwp == nullptr) return SR_FAULT ;
	if (xup->ubuf == nullptr) return SR_FAULT ;

	xup->ubuf[0] = '\0' ;
	if (xup->uid < 0) {
	    xup->f_self = TRUE ;
	    xup->uid = getuid() ;
	} else {
	    const uid_t	suid = getuid() ;
	    xup->f_self = (xup->uid == suid) ;
	}

	if ((rs = vecstr_start(&xup->names,10,0)) >= 0) {
	    for (int i = 0 ; getxusernames[i] ; i += 1) {
	        rs = (*getxusernames[i])(xup) ;
	        if (rs != 0) break ;
	    } /* end for */
	    pwl = rs ;

	    if ((rs > 0) && xup->f_self) {
		PASSWD	*pwp = xup->pwp ;
		rs = uproguser_nameset(pwp->pw_name,-1,xup->uid,ttl) ;
	    } /* end if (cache store) */

	    rs1 = vecstr_finish(&xup->names) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (vecstr) */
	if ((rs >= 0) && (pwl == 0)) rs = SR_NOTFOUND ;

	return (rs >= 0) ? pwl : rs ;
}
/* end subroutine (getxusername) */


/* local subroutines */

static int getusernamer(char *ubuf,int ulen,uid_t uid) noex {
	int		rs ;
	int		rs1 ;

	if ((rs = getbufsize(getbufsize_pw)) >= 0) {
	    PASSWD	pw ;
	    cint	pwlen = rs ;
	    char	*pwbuf ;
	    if ((rs = uc_malloc((pwlen+1),&pwbuf)) >= 0) {
	        GETXUSERNAME	xu ;
	        memset(&xu,0,sizeof(struct getxusername)) ;
	        xu.pwp = &pw ;
	        xu.pwbuf = pwbuf ;
	        xu.pwlen = pwlen ;
	        xu.ubuf = ubuf ;
	        xu.ulen = ulen ;
	        xu.uid = uid ;
	        xu.f_tried = TRUE ;
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
	    } /* end if (m-a) */
	} /* end if (getbufsize) */

	return rs ;
}
/* end subroutine (getusernamer) */

static int getxusername_self(GETXUSERNAME *xup) noex {
	int		rs = SR_OK ;
	xup->unl = 0 ;
	if (xup->f_self && (! xup->f_tried)) {
	    if ((rs = uproguser_nameget(xup->ubuf,xup->ulen,xup->uid)) > 0) {
		xup->unl = rs ;
		rs = getxusername_lookup(xup,xup->ubuf) ;
	    } /* end if (uproguser_nameget) */
	} /* end if (self) */
	return rs ;
}
/* end subroutine (getxusername_self) */

static int getxusername_varusername(GETXUSERNAME *xup) noex {
	return getxusername_var(xup,VARUSERNAME) ;
}
/* end subroutine (getxusername_varusername) */

static int getxusername_varuser(GETXUSERNAME *xup) noex {
	return getxusername_var(xup,VARUSER) ;
}
/* end subroutine (getxusername_varuser) */

static int getxusername_varlogname(GETXUSERNAME *xup) noex {
	return getxusername_var(xup,VARLOGNAME) ;
}
/* end subroutine (getxusername_varlogname) */

static int getxusername_home(GETXUSERNAME *xup) noex {
	return getxusername_varbase(xup,VARHOME) ;
}
/* end subroutine (getxusername_home) */

static int getxusername_mail(GETXUSERNAME *xup) noex {
	return getxusername_varbase(xup,VARMAIL) ;
}
/* end subroutine (getxusername_mail) */

static int getxusername_utmp(GETXUSERNAME *xup) noex {
	int		rs ;
	cchar	*np ;

#if	CF_UTMPACC
	{
	    UTMPACC_ENT	ue ;
	    cint	uelen = UTMPACC_BUFLEN ;
	    char	uebuf[UTMPACC_BUFLEN+1] ;
	    if ((rs = utmpacc_entsid(&ue,uebuf,uelen,0)) >= 0) {
		if (ue.user != nullptr) {
		    rs = sncpy1(xup->ubuf,xup->ulen,ue.user) ;
		} else {
		    rs = SR_NOTFOUND ;
		    xup->ubuf[0] = '\0' ;
		}
	    } /* end if (utmpacc-entsid) */
	}
#else /* CF_UTMPACC */
#if	CF_GETUTMPNAME
	rs = getutmpname(xup->ubuf,xup->ulen,0) ;
#else
	rs = uc_getlogin(xup->ubuf,xup->ulen) ;
#endif
#endif /* CF_UTMPACC */

	xup->unl = rs ;
	if (rs >= 0) {
	    np = xup->ubuf ;
	    if (*np != '\0') {
	        rs = getxusername_lookup(xup,np) ;
	    } else {
	        rs = SR_OK ;
	    }
	} else if (isNotPresent(rs)) {
	    rs = SR_OK ;
	}

	return rs ;
}
/* end subroutine (getxusername_utmp) */


static int getxusername_map(GETXUSERNAME *xup) noex {
	uid_t		uid = xup->uid ;
	int		rs = SR_OK ;
	int		i ;
	int		f = FALSE ;

	xup->unl = 0 ;
	for (i = 0 ; mapents[i].uid >= 0 ; i += 1) {
	    f = (uid == mapents[i].uid) ;
	    if (f) break ;
	} /* end for */

	if (f) {
	    cchar	*np = mapents[i].name ;
	    rs = getxusername_lookup(xup,np) ;
	}

	return rs ;
}
/* end subroutine (getxusername_map) */

static int getxusername_uid(GETXUSERNAME *xup) noex {
	int		rs ;
	xup->unl = 0 ;
	if ((rs = GETPW_UID(xup->pwp,xup->pwbuf,xup->pwlen,xup->uid)) >= 0) {
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

static int getxusername_var(GETXUSERNAME *xup,cchar *varname) noex {
	int		rs = SR_OK ;
	cchar	*vp = getenv(varname) ;

	if (vp != nullptr) {
	    xup->unl = 0 ;
	    rs = getxusername_lookup(xup,vp) ;
	}

	return rs ;
}
/* end subroutine (getxusername_var) */

static int getxusername_varbase(GETXUSERNAME *xup,cchar *varname) noex {
	int		rs = SR_OK ;
	cchar	*vp = getenv(varname) ;

	if (vp != nullptr) {
	    int		nl ;
	    cchar	*np ;
	    if ((nl = sfbasename(vp,-1,&np)) > 0) {
	        while ((nl > 0) && (np[nl - 1] == '/')) {
	            nl -= 1 ;
	        }
	        if ((nl > 0) && (np[0] != '/')) {
	            if ((rs = snwcpy(xup->ubuf,xup->ulen,np,nl)) >= 0) {
	                xup->unl = rs ;
	                np = xup->ubuf ;
	                rs = getxusername_lookup(xup,np) ;
	            }
	        }
	    } /* end if */
	} /* end if (non-null) */

	return rs ;
}
/* end subroutine (getxusername_varbase) */

static int getxusername_lookup(GETXUSERNAME *xup,cchar *np) noex {
	int		rs ;
	int		pwl = 0 ;

	if ((rs = vecstr_find(&xup->names,np)) == SR_NOTFOUND) {
	    cint	pwlen = xup->pwlen ;
	    char	*pwbuf = xup->pwbuf ;
	    if ((rs = GETPW_NAME(xup->pwp,pwbuf,pwlen,np)) >= 0) {
	        if (xup->pwp->pw_uid == xup->uid) {
		    xup->pwl = rs ;
	            pwl = rs ;
		} else {
		    rs = SR_NOTFOUND ;
		}
	    } /* end if (GETPW_NAME) */
	    if (rs == SR_NOTFOUND) {
	        rs = vecstr_add(&xup->names,np,-1) ;
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


