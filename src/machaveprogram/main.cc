/* main (haveprogram) */
/* lang=C++20 */

/* determine if a given name is an executable program */
/* version %I% last-modified %G% */


/* revision history:

	= 1989-03-01, David A.D. Morano
	This subroutine was originally written.

*/

/* Copyright © 1989 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	haveprogram

	Description:
	Print out the home directory for the given user.

	Synopsis:
	$ pathenum
	$ pathto
	$ haveprogram

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<cstdlib>
#include	<cstring>
#include	<string>
#include	<vector>
#include	<iostream>
#include	<usystem.h>
#include	<sfx.h>
#include	<matstr.h>
#include	<rmx.h>
#include	<strwcpy.h>
#include	<varnames.h>
#include	<localmisc.h>


/* local defines */


/* local namespaces */

using std::cout ;
using std::clog ;
using std::string ;
using std::vector ;


/* local typedefs */

typedef cchar *const 	*mainv ;


/* external subroutines */


/* external variables */


/* local structures */

enum progmodes {
	progmode_pathenum,
	progmode_pathto,
	progmode_haveprogram,
	progmode_overlast
} ;

static constexpr cchar	*progmodes[] = {
	"pathenum",
	"pathto",
	"haveprogram",
	nullptr
} ;

namespace {
    struct userinfo {
	string		us ;
	cchar		*un{} ;
	cchar		*uh{} ;
	int find(uid_t) noex ;
	int findenv(uid_t) noex ;
	int findutmp(uid_t) noex ;
	int finduid(uid_t) noex ;
    } ; /* end struct (userinfo) */
}


/* forward references */

static int	getpn(int,mainv,cchar **) noexcept ;


/* local variables */


/* exported subroutines */

int main(int argc,mainv argv,mainv) noexcept {
	const uid_t	uid = getuid() ;
	int		ex = EXIT_FAILURE ;
	int		rs = 0 ;
	cchar		*pn{} ;
	if ((rs = getpn(argc,argv,&pn)) >= 0) {
	    userinfo	ui ;
	    cint	pl = rs ;
	    if ((rs = ui.find(uid)) > 0) {
	        int	pm ;
	        if ((pm = matstr(progmodes,pn,pl)) >= 0) {
		    switch (pm) {
		    case progmode_username:
		        cout << ui.un << '\n' ;
		        break ;
		    case progmode_userhome:
		        cout << ui.uh << '\n' ;
		        break ;
		    } /* end switch */
		    ex = EXIT_SUCCESS ;
	        } /* end if (matstr) */
	    } /* end if (findenv) */
	} /* end if (getpn) */
	if (rs < 0) ex = EXIT_FAILURE ;
	return ex ;
}
/* end subroutine (main) */


/* local subroutines */

static int getpn(int argc,mainv argv,cchar **rpp) noexcept {
	int		rs = SR_FAULT ;
	if (argv && rpp) {
	    int		bl ;
	    cchar	*bp{} ;
	    rs = SR_INVALID ;
	    if ((argc > 0) && (argv[0] != nullptr)) {
	        if ((bl = sfbasename(argv[0],-1,&bp)) > 0) {
		    int		pl = rmchr(bp,bl,'.') ;
		    cchar	*pp = bp ;
		    if (pl > 0) {
			int	i ;
	                if ((i = matstr(progmodes,pp,pl)) >= 0) {
		            *rpp = progmodes[i] ;
		            rs = strlen(progmodes[i]) ;
	                }
		    } /* end if (non-zero positive progname) */
		} /* end if (have base-name) */
	    } /* end if (have first argument) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (getpn) */

static bool isourtype(UTMPX *up) noexcept {
	bool	f = false ;
	f = f || (up->ut_type == INIT_PROCESS) ;
	f = f || (up->ut_type == LOGIN_PROCESS) ;
	f = f || (up->ut_type == USER_PROCESS) ;
	return f ;
}

int userinfo::find(uid_t uid) noex {
	int		rs ;
	if ((rs = findenv(uid)) == 0) {
	    if ((rs = findutmp(uid)) == 0) {
	        rs = finduid(uid) ;
	    }
	}
	return rs ;
}

int userinfo::findenv(uid_t uid) noex {
	int		rs = 0 ;
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
			    rs = strlen(un) ;
		        }
		    } /* end if (getpwnam) */
		} /* end if (sfbasename) */
	    } /* end if (getenv) */
	} /* end for */
	return rs ;
}
/* end subroutine (userinfo::findenv) */

int userinfo::findutmp(uid_t) noex {
	const pid_t	sid = getsid(0) ;
	int		rs = SR_OK ;
	utmpx		*up ;
	setutxent() ;
	while ((up = getutxent()) != nullptr) {
	    if ((up->ut_pid == sid) && isourtype(up)) {
		un = up->ut_user ;
		rs = strnlen(un,sizeof(up->ut_user)) ;
	    } /* end if (match) */
	    if (rs != 0) break ;
	} /* end while */
	if (rs > 0) {
	    PASSWD	*pwp ;
	    char	ubuf[rs+1] ;
	    strwcpy(ubuf,un,rs) ;
	    if ((pwp = getpwnam(ubuf)) != nullptr) {
		us = ubuf ;
		un = us.c_str() ;
		uh = pwp->pw_name ;
	    } else {
		rs = 0 ;
	    }
	} /* end if (got one) */
	return rs ;
}
/* end subroutine (userinfo::findutmp) */

int userinfo::finduid(uid_t uid) noex {
	int		rs = SR_OK ;
	PASSWD		*pwp ;
	if ((pwp = getpwuid(uid)) != nullptr) {
	    un = pwp->pw_name ;
	    uh = pwp->pw_dir ;
	    rs = strlen(un) ;
	} /* end if */
	return rs ;
}
/* end subroutine (userinfo::finduid) */


