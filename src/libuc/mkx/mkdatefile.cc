/* mkdatefile SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* make a temporary date-type job file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mkdatefile

	Description:
	Make a file that can serve as a job submital file.  The
	file will have a name that has the current date in it and
	some pseudo-random characters to make it unique.

	Description:
	int mkdatefile(char *rbuf,cc *dname,cc *fs,mode_t fm) noex

	Arguments:
	= rbuf		result buffer - should be (MAXPATHLEN+1) in length
	- dname		directory to create file in
	- fs		file suffix for created file
	- fm		mode of created file

	Retruns:
	>=0		completed successfully - length of result
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstdlib>
#include	<cstdio>
#include	<cstring>
#include	<ctime>
#include	<usystem.h>
#include	<bufsizevar.hh>
#include	<gettmpdname.h>
#include	<mkpathx.h>
#include	<sncpyx.h>
#include	<snaddw.h>
#include	<snx.h>
#include	<ctdec.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"mkfile.h"


/* local defines */

#define	O_FLAGS		(O_CREAT | O_EXCL | O_RDWR)
#define	MAXNLOOP	100


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct tryer {
	TM		tms ;
	cchar		*dname ;
	cchar		*ext ;
	char		*rbuf ;
	int		rlen ;
	int		rl ;
	mode_t		fm ;
	tryer(char *r,cc *d,cc *e,mode_t m) noex {
	    rbuf = r ;
	    dname = d ;
	    ext = e ;
	    fm = m ;
	} ; /* end ctor */
	operator int () noex ;
	int looper() noex ;
} ; /* end struct (tryer) */


/* forward references */

static int	sntmymd(char *,int,TM *) noex ;

static int mker(char *rbuf,cc *dname,cc *fs,mode_t fm) noex {
	tryer	to(rbuf,dname,fs,fm) ;
	return to ;
}
/* end subroutine (mker) */


/* local variables */

static cchar		*defdname ;	/* <- thread-safe */

static bufsizevar	maxpathlen(getbufsize_mp) ;


/* exported variables */


/* exported subroutines */

int mkdatefile(char *rbuf,cchar *dname,cchar *fs,mode_t fm) noex {
	int		rs = SR_FAULT ;
	if (rbuf && fs) {
	    rbuf[0] = '\0' ;
	    rs = SR_INVALID ;
	    if (fs[0]) {
	        if ((dname == nullptr) || (dname[0] == '\0')) {
	            if ((dname = defdname) == nullptr) {
		        defdname = gettmpdname() ;
		        dname = defdname ;
	            }
	        } /* end if (null or empty) */
		rs = mker(rbuf,dname,fs,fm) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mkdatefile) */


/* local subroutines */

tryer::operator int () noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = maxpathlen) >= 0) {
	    time_t	dt = time(nullptr) ;
	    rlen = rs ;
	    if ((rs = uc_localtime(&dt,&tms)) >= 0) {
	        if ((rs = mkpath(rbuf,dname)) >= 0) {
		    rl = rs ;
		    if ((rs = snaddw(rbuf,rlen,rl,"/d",-1)) >= 0) {
			rl += rs ;
			if ((rs = sntmymd((rbuf+rl),(rlen-rl),&tms)) >= 0) {
			    rl += rs ;
		            rs = looper() ;
		            len = rs ;
			} /* end if (antmymd) */
		    } /* end if (snaddw) */
	        } /* end if (mkpath) */
	    } /* end if (localtime) */
	} /* end if (maxpathlen) */
	return (rs >= 0) ? len : rs ;
}
/* end method (tryer::operator) */

int tryer::looper() noex {
	cint		nalpha = 26 ;
	cint		ml = MAXNLOOP ;
	cmode		om = (fm & 0777) ;
	int		rs = SR_OK ;
	int		len = 0 ;
	for (int loop = 0 ; (loop < ml) && (rs == 0) ; loop += 1) {
	    char	*bp = (rbuf+rl) ;
	    int		bl = (rlen-rl) ;
	    if ((loop >= 0) && (loop < nalpha)) {
	        *bp++ = ('a' + loop) ;
		*bp = '\0' ;
		rs = 1 ;
		bl -= rs ;
	    } else if (loop >= nalpha) {
		rs = ctdecp(bp,bl,4,loop) ;
		bp += rs ;
		bl -= rs ;
	    } /* end if */
	    if (rs >= 0) {
	        cint	fl = (rl+rs) ;
		if ((rs = sncpy((rbuf+fl),(rlen-fl),".",ext)) >= 0) {
		    len = (fl+rs) ;
	            if ((rs = uc_open(rbuf,O_FLAGS,om)) >= 0) {
		        cint	fd = rs ;
		        uc_close(fd) ;
	            } else if (isNotAccess(rs)) {
		        rs = SR_OK ;
	            }
		} /* end if (sncpy) */
	    } /* end if (ok) */
	} /* end for (looping) */
	if ((rs >= 0) && (len == 0)) {
	    rs = SR_TIMEDOUT ;
	}
	return (rs >= 0) ? len : rs ;
}
/* end method (tryer::looper) */

static int sntmymd(char *rbuf,int rlen,TM *tmp) noex {
	cint		yr = tmp->tm_year ;
	cint		mo = (tmp->tm_mon + 1) ;
	cint		da = tmp->tm_mday ;
	int		rs = SR_INVALID ;
	if (rlen > 0) {
	    cchar	*fmt = "%02d%02d%02d" ;
	    if ((rs = snprintf(rbuf,rlen,fmt,yr,mo,da)) >= rlen) {
	        rs = SR_OVERFLOW ;
	    }
	} /* end if (valid) */
	return rs ;
}
/* end subroutine (sntmymd) */


