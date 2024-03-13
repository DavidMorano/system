/* main (appleterminal) */
/* lang=C++20 */

/* print out a character sequence to send a command to Apple-Terminal */
/* version %I% last-modified %G% */


/* revision history:

	= 1989-03-01, David A.D. Morano
	This subroutine was originally written.

*/

/* Copyright © 1989 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	appleterminal_pwd
	appleterminal_doc

	Description:
	Print out the home directory for the given user.

	Synopsis:
	$ appleterminal_pwd <pwd> [<hostname>]
	$ appleterminal_doc <doc>

	Returns:
	EXIT_SUCCESS	success
	EXIT_FAILURE	failure

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<cerrno>
#include	<climits>
#include	<cstdlib>
#include	<cstdio>		/* for |snprintf(3c)| */
#include	<cstring>		/* for |strcmp(3c)| */
#include	<iostream>		/* unuesed */
#include	<string>
#include	<usystem.h>
#include	<varnames.hh>
#include	<sfx.h>
#include	<matstr.h>
#include	<rmx.h>
#include	<strwcpy.h>
#include	<getfdfile.h>		/* for |FD_STDIN| */
#include	<getourenv.h>
#include	<localmisc.h>


/* local defines */

#define	ENCBUFLEN	10		/* the buffer length for encoding */


/* imported namespaces */

using std::cout ;
using std::string ;


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

enum prognames {
	progmode_apptermdoc,
	progmode_apptermpwd,
	progmode_overlast
} ;

static constexpr cchar	*prognames[] = {
	"appleterminal_doc",
	"appleterminal_pwd",
	nullptr
} ;

namespace {
    struct proginfo {
	mainv		argv ;
	mainv		envv ;
	cchar		*pn ;
	int		argc ;
	int		pm ;
	proginfo(int c,mainv a,mainv e) noex : argc(c), argv(a), envv(e) { 
	    pn = nullptr ;
	    pm = 0 ;
	} ;
	int start(mainv) noex ;
	int finish() noex ;
	int getpn(mainv) noex ;
	int termout() noex ;
    } ; /* end struct (proginfo) */
}


/* forward references */

static int	getps(int) noex ;
static int	hexchar(int) noex ;
static int	perenc(char *,int,int) noex ;
static bool	isnotbadchar(int) noex ;


/* local variables */

static constexpr char		badchars[] = R"xx(%:\'"&)xx" ;
static constexpr char		appleterminal[] = "Apple_Terminal" ;


/* exported variables */


/* exported subroutines */

int main(int argc,mainv argv,mainv envv) noex {
	proginfo	pi(argc,argv,envv) ;
	int		ex = EXIT_FAILURE ;
	int		rs ;
	int		rs1 ;
	if ((rs = pi.start(prognames)) >= 0) {
	    if ((rs = pi.termout()) >= 0) {
		ex = EXIT_SUCCESS ;
	    } /* end if (proginfo::termout) */
	    rs1 = pi.finish() ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (proginfo) */
	if (rs < 0) ex = EXIT_FAILURE ;
	return ex ;
}
/* end subroutine (main) */


/* local subroutines */

int proginfo::start(mainv names) noex {
	return getpn(names) ;
}
/* end method (proginfo::start) */

int proginfo::finish() noex {
	int		rs = SR_FAULT ;
	if (this) {
	    pm = 0 ;
	    pn = nullptr ;
	}
	return rs ;
}
/* end method (proginfo::finish) */

int proginfo::getpn(mainv names) noex {
	int		rs = SR_FAULT ;
	if (argv) {
	    rs = SR_INVALID ;
	    if ((argc > 0) && (argv[0] != nullptr)) {
	        cchar	*bp{} ;
	        if (int bl ; (bl = sfbasename(argv[0],-1,&bp)) > 0) {
		    int		pl = rmchr(bp,bl,'.') ;
		    cchar	*pp = bp ;
		    if (pl > 0) {
	                if ((pm = matstr(names,pp,pl)) >= 0) {
			    pn = names[pm] ;
		            rs = pm ;
	                }
		    } /* end if (non-zero positive progname) */
		} /* end if (have base-name) */
	    } /* end if (have first argument) */
	} /* end if (non-null) */
	return rs ;
}
/* end method (proginfo::getpn) */

int proginfo::termout() noex {
	cchar		*vn = varname.termprogram ;
	int		rs = SR_OK ;
	if (cchar *tn ; (tn = getourenv(envv,vn)) != nullptr) {
            if ((strcmp(tn,appleterminal) == 0) && (argc >= 2)) {
                string	s ;
                cint	fd = FD_STDIN ;
		cint	ps = getps(pm) ;
                int	vl = 0 ;
		rs = SR_INVALID ;
                if (ps >= 0) {
                    cint        ilen = ENCBUFLEN ;
		    cchar	*fmt = "\033]%u;" ;
                    char        ibuf[ENCBUFLEN+1] ;
                    if ((rs = snprintf(ibuf,ilen,fmt,ps)) >= ilen) {
                        rs = SR_OVERFLOW ;
                    } else {
                        s += ibuf ;
                    }
                } /* end if (ok) */
                if (rs >= 0) {
                    cchar   prefix[] = "file://" ;
                    cchar   *fn = argv[1] ;
                    s += prefix ;
		    vl += strlen(prefix) ;
                    if (argc >= 3) { /* <- hostname is available */
                        cchar       *hn = argv[2] ;
                        if (hn[0]) {
                            s += hn ;
                            vl = strlen(hn) ;
                        }
                    } /* end if (possible hostname) */
                    if (fn[0]) {
                        cint        elen = ENCBUFLEN ;
                        char        ebuf[ENCBUFLEN+1] = {} ;
                        cint        fl = rmtrailchr(fn,-1,'/') ;
                        for (int i = 0 ; (i < fl) && fn[i] ; i += 1) {
                            vl += perenc(ebuf,elen,fn[i]) ;
                            s += ebuf ;
                        } /* end for */
                    } /* end if (non-zero positive) */
                } /* end if (ok) */
                if ((rs >= 0) && (vl > 0)) {
                    s += '\a' ;
                }
                if ((rs >= 0) && (vl > 0)) {
                    cchar   *sp = s.c_str() ;
                    cint    sl = strlen(sp) ;
                    if ((rs = write(fd,sp,sl)) < 0) {
                        rs = (-errno) ;
                    }
                }
            } /* end if (at least one argument) */
	} /* end if (getourenv) */
	return rs ;
}
/* end method (proginfo::termout) */

static int getps(int pm) noex {
	int         ps = -1 ;       /* <- what Apple calls this */
	switch (pm) {
	case progmode_apptermdoc:
	    ps = 6 ;                /* <- Apple specified for 'doc' */
	    break ;
	case progmode_apptermpwd:
	    ps = 7 ;                /* <- Apple specified for 'pwd' */
	    break ;
	} /* end switch */
	return ps ;
}
/* end subroutine (getps) */

static int perenc(char *ebuf,int,int ch) noex {
	char		*ep = ebuf ;
	if (isprint(ch) && isnotbadchar(ch)) {
	    *ep++ = ch ;
	    *ep = '\0' ;
	} else {
	    *ep++ = '%' ;
	    *ep++ = hexchar(ch >> 4) ;
	    *ep++ = hexchar(ch >> 0) ;
	    *ep = '\0' ;
	}
	return (ep - ebuf) ;
}
/* end subroutine (perenc) */

static int hexchar(int ch) noex {
	int	xch = 0 ;
	ch &= 0x0F ;
	if ((ch >= 0) && (ch <= 9)) {
	    xch = ('0' + ch) ;
	} else {
	    xch = ('A' + (ch - 10)) ;
	}
	return xch ;
}
/* end subroutine (hexchar) */

static bool isnotbadchar(int ch) noex {
	return (strchr(badchars,ch) == nullptr) ;
}
/* end subroutine (isnotbadchar) */


