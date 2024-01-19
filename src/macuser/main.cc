/* main (userhome) */
/* lang=C++20 */

/* generic front-end for SHELL built-ins */
/* version %I% last-modified %G% */


/* revision history:

	= 1989-03-01, David A.D. Morano
	This subroutine was originally written.

	= 2007-08-22, David A.D. Morano
	I refactored and enhanced this for use on MacOS.  MacOS is
	a brain-damaged system and needs some help.  But I could
	not easily port the full (featured) versions of these same
	utilities from Solaris® and other System V syhstems because
	MacOS does not support a moderately extended POSIX® development
	environment.

*/

/* Copyright © 1989,2007 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	username
	userhome
	usershell
	gid
	groupname
	uid
	sid
	shells

	Description:
	Print out the home directory (or some other user-related
	information) for the given user.

	Synopsis:
	$ username
	$ userhome
	$ usershell
	$ gid
	$ groupname
	$ uid
	$ sid
	$ shells

	Returns:
	EXIT_SUCCESS	user was found and processed successfully
	EXIT_FAILURE	some kind of error (there could be many)

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<climits>
#include	<cstdlib>
#include	<cstring>		/* for |strlen(3c)| + |strnlen(3c)| */
#include	<string>
#include	<fstream>
#include	<iostream>
#include	<usystem.h>
#include	<sfx.h>
#include	<matstr.h>
#include	<rmx.h>
#include	<strwcpy.h>
#include	<localmisc.h>
#include	<utmpx.h>
#include	<pwd.h>
#include	<grp.h>
#include	<ccfile.hh>
#include	<hasx.h>


/* local defines */

#define	MAXLINE		(4*1024)
#define	FNSHELLS	"/etc/shells"


/* local namespaces */

using std::string ;			/* type */
using std::ifstream ;			/* type */
using std::cout ;			/* variable */
using std::clog ;			/* variable */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

enum prognames {
	progmode_username,
	progmode_userhome,
	progmode_usershell,
	progmode_gid,
	progmode_groupname,
	progmode_uid,
	progmode_sid,
	progmode_sessionid,
	progmode_shells,
	progmode_overlast
} ;

static constexpr cchar	*prognames[] = {
	"username",
	"userhome",
	"usershell",
	"gid",
	"groupname",
	"uid",
	"sid",
	"sessionid",
	"shells",
	nullptr
} ;

static constexpr cchar	*envs[] = {
	"USERNAME",
	"USER",
	"LOGNAME",
	"HOME",
	"MAIL",
	nullptr
} ;

namespace {
    struct userinfo {
	string		un ;
	string		uh ;
	string		us ;
	gid_t		gid ;
	int find(uid_t) noex ;
	int findenv(uid_t) noex ;
	int findutmp(uid_t) noex ;
	int finduid(uid_t) noex ;
    } ; /* end struct (userinfo) */
}


/* forward references */

static int	getpn(int,mainv,mainv) noex ;
static int	procgroup(int,const userinfo *) noex ;
static int	printgroup(const userinfo *) noex ;
static int	printshells() noex ;


/* local variables */

constexpr gid_t		gidend = gid_t(-1) ;

constexpr cchar		fnshells[] = FNSHELLS ;


/* exported subroutines */

int main(int argc,mainv argv,mainv) noex {
	const uid_t	uid = getuid() ;
	int		ex = EXIT_FAILURE ;
	int		rs = 0 ;
	if ((rs = getpn(argc,argv,prognames)) >= 0) {
	    userinfo	ui ;
	    cint	pm = rs ;
	    switch (pm) {
	    case progmode_uid:
		cout << uid << eol ;
		break ;
	    case progmode_sid:
	    case progmode_sessionid:
		{
		    cint	sid = getsid(0) ;
		    cout << sid << eol ;
		}
		break ;
	    case progmode_shells:
		rs = printshells() ;
		break ;
	    default:
	        if ((rs = ui.find(uid)) > 0) {
		    switch (pm) {
		    case progmode_username:
		            cout << ui.un << eol ;
		            break ;
		    case progmode_userhome:
		            cout << ui.uh << eol ;
		            break ;
		    case progmode_usershell:
		            cout << ui.us << eol ;
		            break ;
		    case progmode_gid:
		    case progmode_groupname:
			    rs = procgroup(pm,&ui) ;
			    break ;
		    default:
		        rs = SR_BUGCHECK ;
		        break ;
		    } /* end switch */
		    if (rs >- 0) {
		        ex = EXIT_SUCCESS ;
		    }
	        } /* end if (userinfo::find) */
		break ;
	    } /* end switch */
	} /* end if (getpn) */
	if (rs < 0) ex = EXIT_FAILURE ;
	return ex ;
}
/* end subroutine (main) */


/* local subroutines */

static int getpn(int argc,mainv argv,mainv names) noex {
	int		rs = SR_FAULT ;
	if (argv) {
	    rs = SR_NOMSG ;
	    if ((argc > 0) && argv[0]) {
	        int	bl ;
	        cchar	*bp{} ;
	        if ((bl = sfbasename(argv[0],-1,&bp)) > 0) {
		    int		pl = rmchr(bp,bl,'.') ;
		    cchar	*pp = bp ;
		    if (pl > 0) {
			int	i ;
	                if ((i = matstr(names,pp,pl)) >= 0) {
		            rs = i ;
	                }
		    } /* end if (non-zero positive progname) */
		} /* end if (have base-name) */
	    } /* end if (have first argument) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (getpn) */

static int procgroup(int pm,const userinfo *uip) noex {
	int		rs = SR_INVALID ;
	if (uip->gid != gidend) {
	    switch (pm) {
	    case progmode_gid:
	        {
		    cout << uip->gid << eol ;
		    rs = SR_OK ;
	        }
	        break ;
	    case progmode_groupname:
	        rs = printgroup(uip) ;
	        break ;
	    } /* end switch */
	} /* end if (valid) */
	return rs ;
}
/* end subroutine (procgroup) */

static int printgroup(const userinfo *uip) noex {
	int		rs = SR_NOTFOUND ;
	const GROUP	*grp = getgrgid(uip->gid) ;
	if (grp) {
	    cout << grp->gr_name << eol ;
	    rs = SR_OK ;
	} /* end if */
	return rs ;
}
/* end subroutine (printgroup) */

static int printshells() noex {
	cint		llen = MAXLINE ;
	char		*lbuf ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	if ((lbuf = new(nothrow) char[llen+1]) != nullptr) {
	    try {
		ccfile		fis ;
		if ((rs = fis.open(fnshells)) >= 0) {
		    while ((rs = fis.readln(lbuf,llen)) > 0) {
			if (hasnotempty(lbuf,rs) && (lbuf[0] != '#')) {
			    cout << lbuf ;
			} /* end if (hasnotempty) */
		    } /* end while */
		    rs1 = fis.close ;
		    if (rs >= 0) rs = rs1 ;
 	        } /* end if (opened) */
	    } catch (...) {
		rs = SR_IO ;
	    }
	    delete [] lbuf ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end subroutine (printshells) */

static bool isourtype(const UTMPX *up) noex {
	bool	f = false ;
	f = f || (up->ut_type == INIT_PROCESS) ;
	f = f || (up->ut_type == LOGIN_PROCESS) ;
	f = f || (up->ut_type == USER_PROCESS) ;
	return f ;
}
/* end subroutine (isourtype) */

int userinfo::find(uid_t uid) noex {
	int		rs ;
	if ((rs = findenv(uid)) == 0) {
	    if ((rs = findutmp(uid)) == 0) {
	        rs = finduid(uid) ;
	    }
	}
	return rs ;
}
/* end method (userinfo::find) */

int userinfo::findenv(uid_t uid) noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	for (int i = 0 ; (rs == 0) && envs[i] ; i += 1) {
	    cchar	*rp ;
	    if ((rp = getenv(envs[i])) != nullptr) {
		int	cl ;
		cchar	*cp{} ;
		if ((cl = sfbasename(rp,-1,&cp)) > 0) {
		    PASSWD	*pwp ;
		    if ((pwp = getpwnam(rp)) != nullptr) {
		        if (pwp->pw_uid == uid) {
		            un = pwp->pw_name ;
		            uh = pwp->pw_dir ;
		            us = pwp->pw_shell ;
	    		    gid = pwp->pw_gid ;
			    len = un.length() ;
		        } /* end if (match) */
		    } /* end if (getpwnam) */
		} /* end if (sfbasename) */
	    } /* end if (getenv) */
	} /* end for */
	return (rs >= 0) ? len : rs ;
}
/* end method (userinfo::findenv) */

int userinfo::findutmp(uid_t) noex {
	const pid_t	sid = getsid(0) ;
	int		rs = SR_OK ;
	int		len = 0 ;
	utmpx		*up ;
	cchar		*unp{} ;
	setutxent() ;
	while ((up = getutxent()) != nullptr) {
	    if ((up->ut_pid == sid) && isourtype(up)) {
		unp = up->ut_user ;
		rs = strnlen(unp,sizeof(up->ut_user)) ;
	    } /* end if (match) */
	    if (rs != 0) break ;
	} /* end while */
	if ((rs > 0) && unp) {
	    PASSWD	*pwp ;
	    char	ubuf[rs+1] ;
	    strwcpy(ubuf,unp,rs) ;
	    if ((pwp = getpwnam(ubuf)) != nullptr) {
		un = pwp->pw_name ;
		uh = pwp->pw_dir ;
		us = pwp->pw_shell ;
	  	gid = pwp->pw_gid ;
		len = un.length() ;
	    } else {
		rs = 0 ;
	    }
	} /* end if (got one) */
	return (rs >= 0) ? len : rs ;
}
/* end method (userinfo::findutmp) */

int userinfo::finduid(uid_t uid) noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	PASSWD		*pwp ;
	if ((pwp = getpwuid(uid)) != nullptr) {
	    un = pwp->pw_name ;
	    uh = pwp->pw_dir ;
	    us = pwp->pw_shell ;
	    gid = pwp->pw_gid ;
	    len = un.length() ;
	} /* end if */
	return (rs >= 0) ? len : rs ;
}
/* end method (userinfo::finduid) */


