/* main SUPPORT (OpenWeather) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* open a file-descriptor to a weather METAR */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	1		/* compile-time debugging */
#define	CF_DIRGROUP	1		/* set GID on directories */

/* revision history:

	= 1998-07-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

	Name:
	openweather

	Description:
	This subroutine opens a file that contains a weather METAR.
	Only the most current METAR is returned.

	Synopsis:
	int openweather(cchar *pr,cchar *ws,int oflags,int to)

	Arguments:
	pr		program-root
	ws		weather station identification (eg: "kbos")
	oflags		open flags (see 'open(2)')
	to		time-out

	Returns:
	>=0		file descriptor to program STDIN and STDOUT
	<0		error code (system-return)

	Note that all METARs are read-only.  Regardless of the type
	of open-flags supplied only a read-only file-descriptor (FD)
	is returned.

******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/mman.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<vecstr.h>
#include	<linefold.h>
#include	<logfile.h>
#include	<logsys.h>
#include	<localmisc.h>

#include	"ow.h"
#include	"owconfig.h"


/* local defines */

#ifndef	WSBUFLEN
#define	WSBUFLEN	16
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward reference */

local int	ow_avail(OW *) ;
local int	ow_loadsvars(OW *) ;
local int	ow_findconf(OW *) ;
local int	ow_setdefaults(OW *) ;
local int	ow_openweb(OW *) ;
local int	ow_procweb(OW *,int) ;
local int	ow_checkdirs(OW *) ;
local int	ow_checkdir(OW *,cchar *,mode_t) ;


/* local variables */

static cchar	*schedconf[] = {
	"%p/etc/%n/%n.%f",
	"%p/etc/%n/%f",
	"%p/etc/%n.%f",
	NULL
} ;


/* exported subroutines */


int openweather(pr,ws,of,to)
cchar	*pr ;
cchar	*ws ;
int		of ;
int		to ;
{
	OW		si, *sip = &si ;
	time_t		ti_now = time(NULL) ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		fd = -1 ;
	cchar	*sn = OW_SEARCHNAME ;
	cchar	*vd = OW_VDNAME ;

	if (pr == NULL) return SR_FAULT ;
	if (ws == NULL) return SR_FAULT ;

#if	CF_DEBUGS
	debugprintf("openweather: pr=%s\n",pr) ;
	debugprintf("openweather: ws=%s\n",ws) ;
#endif

	if ((rs = ow_start(sip,pr,sn,vd,ws,ti_now,of,to)) >= 0) {

	    if ((ws != NULL) && (ws[0] != '\0')) {
	        rs = ow_avail(sip) ;
	        fd = rs ;

#if	CF_DEBUGS
	debugprintf("openweather: ow_avail() rs=%d fd=%u\n",rs,fd) ;
#endif

	    }

	    if ((rs < 0) && (isNotPresent(rs) || (rs == SR_STALE))) {
	        cchar	*cf ;

	        rs = ow_loadsvars(sip) ;

#if	CF_DEBUGS
	debugprintf("openweather: ow_loadsvars() rs=%d\n",rs) ;
#endif
	        if (rs >= 0) {
	            rs = ow_findconf(sip) ; /* sets 'cfname' */
#if	CF_DEBUGS
	debugprintf("openweather: ow_findconf() rs=%d\n",rs) ;
#endif
		}

	        cf = sip->cfname ;
		if (rs >= 0) {
	        if ((rs = owconfig_start(&sip->c,sip,cf)) >= 0) {

#if	CF_DEBUGS
	debugprintf("openweather: owconfig_start() rs=%d\n",rs) ;
#endif
	            rs = ow_setdefaults(sip) ;

	            if (rs >= 0)
	                rs = ow_checkdirs(sip) ;

	            if (rs >= 0)
	                rs = ow_openweb(sip) ;

	            rs1 = owconfig_finish(&sip->c) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (owconfig) */
		} /* end if */

#if	CF_DEBUGS
	debugprintf("openweather: owconfig-out rs=%d\n",rs) ;
#endif
	    } /* end if (ow) */

	    rs1 = ow_finish(sip,(rs<0)) ;
	    if (rs >= 0) rs = rs1 ;
	    if ((rs < 0) && (fd >= 0)) u_close(fd) ;
	} /* end if (file not present or was stale) */

#if	CF_DEBUGS
	debugprintf("openweather: ret rs=%d fd=%u\n",rs,fd) ;
#endif

	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (openweather) */


/* local subroutines */


local int ow_avail(sip)
OW		*sip ;
{
	ustat	sb ;
	int		rs = SR_OK ;
	int		fd = -1 ;
	cchar	*pr = sip->pr ;
	cchar	*sn = sip->sn ;
	cchar	*vd = sip->vd ;
	cchar	*ws = sip->ws ;
	cchar	*mdname = OW_METARDNAME ;
	char		wsfname[MAXPATHLEN+1] ;

	if (ws == NULL) {
	    rs = SR_NOENT ;
	    goto ret0 ;
	}

/* we should not be called except before anything other attempts */

	if ((sip->ti_weather > 0) || (sip->wfd >= 0)) {
	    rs = SR_NOANODE ;
	    goto ret0 ;
	}

/* continue with this (first) attempt */

	rs = mkpath5(wsfname,pr,vd,sn,mdname,ws) ;
	if (rs < 0) goto ret0 ;

	rs = u_open(wsfname,O_RDONLY,0666) ;
	fd = rs ;
	if (rs >= 0) {
	    rs = u_fstat(fd,&sb) ;
	    if (rs >= 0) {
	        sip->wfd = fd ;
	        sip->ti_weather = sb.st_mtime ;
	        rs = ow_isvalid(sip,sb.st_mtime) ;
	        if (rs == 0) rs = SR_STALE ;
	    } else if (fd >= 0)
	        u_close(fd) ;
	}

ret0:

#if	CF_DEBUGS
	debugprintf("ow_avail: ret rs=%d fd=%u\n",rs,fd) ;
#endif

	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (ow_avail) */


local int ow_loadsvars(sip)
OW		*sip ;
{
	VECSTR	*slp = &sip->svars ;

	int	rs = SR_OK ;


	if (sip->nodename == NULL)
	    rs = ow_nodedomain(sip) ;

	if (rs >= 0) {
	    if ((rs = vecstr_count(slp)) == 0) {

	        if (rs >= 0)
	            rs = vecstr_envadd(slp,"p",sip->pr,-1) ;

	        if (rs >= 0)
	            rs = vecstr_envadd(slp,"n",sip->sn,-1) ;

	    } /* end if */
	} /* end if */

	return rs ;
}
/* end subroutine (ow_loadsvars) */


local int ow_findconf(sip)
OW		*sip ;
{
	int	rs = SR_OK ;
	int	rs1 ;
	int	pl = 0 ;

	cchar	*cf = OW_CFNAME ;

	char	tmpfname[MAXPATHLEN + 1] ;


	if (sip->cfname == NULL) {
	    vecstr	*slp = &sip->svars ;

	    rs1 = permsched(schedconf,slp,tmpfname,MAXPATHLEN,cf,R_OK) ;
	    if (rs1 >= 0) {
	        pl = rs1 ;
	        rs = ow_setentry(sip,&sip->cfname,tmpfname,pl) ;
	    }

	} /* end if */

	return (rs >= 0) ? pl : rs ;
}
/* end subroutine (ow_findconf) */


local int ow_setdefaults(sip)
OW		*sip ;
{
	int	rs = SR_OK ;


	if (sip->ws == NULL) {
	    if (sip->defws != NULL) sip->ws = sip->defws ;
	    if (sip->ws == NULL) sip->ws = OW_WS ;
	}

	if (sip->whost == NULL) {
	    sip->whost = OW_WHOST ;
	}

	if (sip->wprefix == NULL) {
	    sip->wprefix = OW_WPREFIX ;
	}

	if (sip->to == 0)
	    sip->to = OW_TO ;

	if (sip->logfacility == NULL)
	    sip->logfacility = OW_LOGFACILITY ;

	return rs ;
}
/* end subroutine (ow_setdefaults) */


local int ow_openweb(sip)
OW		*sip ;
{
	cint	af = AF_UNSPEC ;
	cint	opts = 0 ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		to = sip->to ;
	int		fd = -1 ;
	cchar	*wh = sip->whost ;
	cchar	*wps = OW_WEBPORT ;
	cchar	*wprefix = sip->wprefix ;
	cchar	*ws = sip->ws ;
	cchar	**wsvcargs = NULL ;
	char		wsbuf[WSBUFLEN+1], *wp = wsbuf ;
	char		svc[MAXNAMELEN+1] ;

	if (ws == NULL) return SR_FAULT ;

	if (ws[0] == '\0') return SR_INVALID ;

/* make the weather-station part upper-case */

	{
	    int	ml = (WSBUFLEN-4) ;
	    wp = strwcpyuc(wsbuf,ws,ml) ;
	    strwcpy(wp,".TXT",4) ;
	}

/* put it together with the prefix */

	rs = sncpy3(svc,MAXNAMELEN,wprefix,"/",wsbuf) ;
	if (rs < 0) goto ret0 ;

/* dial out */

	if (rs >= 0) {
	    rs1 = dialhttp(wh,wps,af,svc,wsvcargs,to,opts) ;
	    if (rs1 >= 0) {
	        int	wfd = rs ;
	        rs = ow_procweb(sip,wfd) ;
	        fd = rs ;
	        u_close(wfd) ;
	    }
	}

ret0:
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (ow_openweb) */


local int ow_procweb(sip,wfd)
OW		*sip ;
int		wfd ;
{
	int	rs = SR_OK ;



	return rs ;
}
/* end subroutine (ow_procweb) */


local int ow_checkdirs(sip)
OW		*sip ;
{
	int	rs = SR_OK ;

	cchar	*pr = sip->pr ;
	cchar	*vd = sip->vd ;
	cchar	*sn = sip->sn ;
	cchar	*md = OW_METARDNAME ;

	char	wdname[MAXPATHLEN + 1] ;
	char	mdname[MAXPATHLEN + 1] ;


	if (rs >= 0) {
	    rs = mkpath3(wdname,pr,vd,sn) ;
	    if (rs >= 0)
	        rs = ow_checkdir(sip,wdname,0775) ;
	}

	if (rs >= 0) {
	    rs = mkpath2(mdname,wdname,md) ;
	    if (rs >= 0)
	        rs = ow_checkdir(sip,mdname,0777) ;
	}

ret0:
	return rs ;
}
/* end subroutine (ow_checkdirs) */


local int ow_checkdir(sip,dname,m)
OW		*sip ;
cchar	dname[] ;
mode_t		m ;
{
	ustat	usb ;

	int	rs = SR_OK ;
	int	rs1 ;
	int	f_needmode = FALSE ;
	int	f_created = FALSE ;


	m &= S_IAMB ;
	rs1 = u_stat(dname,&usb) ;
	if (rs1 >= 0) {
	    if (S_ISDIR(usb.st_mode)) {
	        cint	am = (R_OK|W_OK|X_OK) ;
	        f_needmode = ((usb.st_mode & m) != m) ;
	        rs = u_access(dname,am) ;
	    } else
	        rs = SR_NOTDIR ;
	}

	if (rs < 0)
	    goto ret0 ;

	if ((rs >= 0) && (rs1 == SR_NOENT)) {

	    f_needmode = TRUE ;
	    f_created = TRUE ;
	    rs = mkdirs(dname,m) ;

	} /* end if (there was no-entry) */

	if ((rs >= 0) && f_needmode)
	    rs = uc_minmod(dname,m) ;

#if	CF_DIRGROUP
	if ((rs >= 0) && f_created) {
	    rs = ow_prid(sip) ;
	    if (rs >= 0) rs = u_chown(dname,-1,sip->prgid) ;
	}
#endif /* CF_DIRGROUP */

ret0:
	return (rs >= 0) ? f_created : rs ;
}
/* end subroutine (ow_checkdir) */



