/* getuserhome SUPPORT */
/* lang=C20 */

/* get the home directory of the specified user */
/* version %I% last-modified %G% */

#define	CF_UGETPW	1		/* use |ugetpw(3uc)| */

/* revision history:

	= 1998-06-01, David A�D� Morano
	This subroutine was originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getuserhome

	Description:
	We carry out some searches in order to find the home directory
	for a specified user (given by username).

	Synopsis:
	int getuserhome(char *rbuf,int rlen,cchar *username) noex

	Arguments:
	rbuf		buffer to hold result
	rlen		length of supplied result buffer
	username	username to check

	Returns:
	>=0		OK
	<0		some error (syhstem-return)

	Implementation note:
	Is "home-searching" faster than just asking the system for
	the home directory of a user?  It probably is not!  In
	reality, it is a matter of interacting with the Automount
	server probably much more than what would have happened if
	we just first interacted with the Name-Server-Cache and
	then the Automount server once after that.

	Note that we NEVER 'stat(2)' a file directly within any of
	the possible base directories for home-directories without
	first having a pretty good idea that it is actually there.
	This is done to avoid those stupid SYSLOG entries saying
	that a file could not be found within an indirect auto-mount
	point -- what the base directories usually are now-a-days!

	On second thought, avoiding asking the "system" may be a
	good course of action since even though we are going through
	the name-server cache, it more often than not has to go out
	to some weirdo-only-knows network NIS+ (whatever) server
	to get the PASSWD information.  This is not unusual in a
	large organization.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstdlib>
#include	<cstring>
#include	<pwd.h>
#include	<usystem.h>
#include	<usupport.h>
#include	<getbufsize.h>
#include	<mallocxx.h>
#include	<fsdir.h>
#include	<getax.h>
#include	<ugetpw.h>
#include	<getxusername.h>
#include	<sfx.h>
#include	<mkpathx.h>
#include	<hasx.h>
#include	<cfdec.h>
#include	<isnot.h>
#include	<strwcmp.h>
#include	<localmisc.h>


/* local defines */

#if	CF_UGETPW
#define	GETPW_NAME	ugetpw_name
#else
#define	GETPW_NAME	getpw_name
#endif /* CF_UGETPW */

#ifndef	VARHOME
#define	VARHOME		"HOME"
#endif

#ifndef	VARUSERNAME
#define	VARUSERNAME	"USERNAME"
#endif

#define	SUBINFO		struct subinfo
#define	SUBINFO_FL	struct subinfo_flags


/* external subroutines */

extern "C" {
    int		getuserhome(char *,int,cchar *) noex ;
}


/* external variables */


/* local structures */

struct subinfo_flags {
	uint		pw:1 ;
} ;

struct subinfo {
	cchar		*un ;
	char		*pwbuf ;
	PASSWD		pw ;
	uid_t		uid ;
	SUBINFO_FL	init ;
	int		pwlen ;
} ;


/* forward references */

static int	subinfo_start(SUBINFO *,cchar *) noex ;
static int	subinfo_getpw(SUBINFO *) noex ;
static int	subinfo_finish(SUBINFO *) noex ;

static int	subinfo_getvar(SUBINFO *,char *,int) noex ;
static int	subinfo_getdirsearch(SUBINFO *,char *,int) noex ;
static int	subinfo_getsysdb(SUBINFO *,char *,int) noex ;

static int	dirsearch(cchar *,cchar *) noex ;


/* local variables */

static constexpr cchar	*homednames[] = {
	"/home",
	"/usr/add-on",
	"/sysadm",
	nullptr
} ;

static constexpr int	(*gethomes[])(SUBINFO *,char *,int) = {
	subinfo_getvar,
	subinfo_getdirsearch,
	subinfo_getsysdb,
	nullptr
} ;


/* exported variables */


/* exported subroutines */

int getuserhome(char *rbuf,int rlen,cchar *un) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rl = 0 ;
	if (rbuf && un) {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if (un[0]) {
	        SUBINFO		si ;
	        if ((rs = subinfo_start(&si,un)) >= 0) {
	            for (int i = 0 ; gethomes[i] ; i += 1) {
	                rs = (*gethomes[i])(&si,rbuf,rlen) ;
	                rl = rs ;
		        if (rs != 0) break ;
	            } /* end for */
	            rs1 = subinfo_finish(&si) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (subinfo) */
	        if ((rs >= 0) && (rl == 0)) rs = SR_NOTFOUND ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (getuserhome) */


/* local subroutines */

static int subinfo_start(SUBINFO *sip,cchar *un) noex {
	int		rs ;
	memclear(sip) ;
	sip->un = un ;
	sip->uid = -1 ;
	if ((rs = getbufsize(getbufsize_pw)) >= 0) {
	    cint	pwlen = rs ;
	    char	*pwbuf ;
	    if ((rs = uc_malloc((pwlen+1),&pwbuf)) >= 0) {
	        sip->pwbuf = pwbuf ;
	        sip->pwlen = pwlen ;
	    }
	} /* end if (getbufsize) */
	return rs ;
}
/* end subroutine (subinfo_start) */

static int subinfo_finish(SUBINFO *sip) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (sip) {
	    rs = SR_OK ;
	    if (sip->pwbuf) {
	        rs1 = uc_free(sip->pwbuf) ;
	        if (rs >= 0) rs = rs1 ;
	        sip->pwbuf = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (subinfo_finish) */

static int subinfo_getpw(SUBINFO *sip) noex {
	int		rs = SR_OK ;
	cchar		*un = sip->un ;
	if (! sip->init.pw) {
	    PASSWD	*pwp = &sip->pw ;
	    cint	pwlen = sip->pwlen ;
	    sip->init.pw = true ;
	    if (un[0] != '-') {
	        if (hasalldig(un,-1)) {
	            uint	uv ;
	            if ((rs = cfdecui(un,-1,&uv)) >= 0) {
	                const uid_t	uid = uv ;
	                rs = getpwusername(pwp,sip->pwbuf,pwlen,uid) ;
	            }
	        } else {
	            rs = GETPW_NAME(pwp,sip->pwbuf,pwlen,un) ;
	        }
	    } else {
	        rs = getpwusername(pwp,sip->pwbuf,pwlen,-1) ;
	    }
	    if (rs >= 0) {
	        sip->uid = sip->pw.pw_uid ;
	    }
	} /* end if (needed initialization) */
	return rs ;
}
/* end subroutine (subinfo_getpw) */

static int subinfo_getvar(SUBINFO *sip,char *rbuf,int rlen) noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	cchar		*un = sip->un ;
	if (un[0] == '-') un = getenv(VARUSERNAME) ;
	if ((un != nullptr) && (un[0] != '\0')) {
	    cchar	*hd = getenv(VARHOME) ;
	    if ((hd != nullptr) && (hd[0] != '\0')) {
	        cchar	*bp ;
	        int	bl ;
	        if ((bl = sfbasename(hd,-1,&bp)) > 0) {
	            if (strwcmp(un,bp,bl) == 0) {
	                rs = mknpath1(rbuf,rlen,hd) ;
	                len = rs ;
	            }
	        } /* end if (sfbasename) */
	    } /* end if */
	} /* end if */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (subinfo_getvar) */

static int subinfo_getdirsearch(SUBINFO *sip,char *rbuf,int rlen) noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	cchar		*un = sip->un ;
	if (un[0] != '-') {
	    USTAT	sb ;
	    for (int i = 0 ; homednames[i] ; i += 1) {
	        cchar	*hdn = homednames[i] ;
	        if ((rs = uc_stat(hdn,&sb)) >= 0) {
		    if (S_ISDIR(sb.st_mode)) {
	                if ((rs = dirsearch(hdn,un)) > 0) {
	                    if ((rs = mknpath2(rbuf,rlen,hdn,un)) >= 0) {
	                        if ((rs = uc_stat(rbuf,&sb)) >= 0) {
	                            if (S_ISDIR(sb.st_mode)) {
	                        	len = rs ;
				    }
				} else if (isNotPresent(rs)) {
				    rs = SR_OK ;
	                        }
	                    } /* end if (mkpath) */
		        } /* end if (dirsearch) */
		    } /* end if (isdir) */
		} else if (isNotPresent(rs)) {
		    rs = SR_OK ;
		} /* end if (stat) */
	        if ((rs < 0) || (len > 0)) break ;
	    } /* end for (looping over login-directory root directories) */
	} /* end if (specified) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (subinfo_getdirsearch) */

static int subinfo_getsysdb(SUBINFO *sip,char *rbuf,int rlen) noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	if (! sip->init.pw) {
	    rs = subinfo_getpw(sip) ;
	}
	if (rs >= 0) {
	    rs = mknpath1(rbuf,rlen,sip->pw.pw_dir) ;
	    len = rs ;
	}
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (subinfo_getsysdb) */

static int dirsearch(cchar *basedname,cchar *un) noex {
	int		rs ;
	int		rs1 ;
	int		f_found = false ;
	char		*nbuf{} ;
	if ((rs = malloc_mn(&nbuf)) >= 0) {
	    fsdir	dir ;
	    fsdir_ent	ds ;
	    cint	nlen = rs ;
	    if ((rs = fsdir_open(&dir,basedname)) >= 0) {
	        while ((rs = fsdir_read(&dir,&ds,nbuf,nlen)) > 0) {
	            cchar	*fnp = ds.name ;
	            if (fnp[0] != '.') {
	                f_found = (fnp[0] == un[0]) && (strcmp(fnp,un) == 0) ;
	                if (f_found) break ;
	            }
	        } /* end while */
	        rs1 = fsdir_close(&dir) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (fsdir) */
	    rs1 = uc_free(nbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? f_found : rs ;
}
/* end subroutine (dirsearch) */


