/* pcsmkconf */

/* make the PCS CONF index file */


#define	CF_DEBUGS	0		/* non-switchable debug print-outs */


/* revision history:

	- 2008-10-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine makes the PCS CONF index file.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<stdlib.h>
#include	<string.h>
#include	<usystem.h>
#include	<mktmp.h>
#include	<ids.h>
#include	<vecstr.h>
#include	<paramfile.h>
#include	<varmk.h>
#include	<localmisc.h>


/* local defines */

#define	SUBINFO		struct subinfo
#define	SUBINFO_FL	struct subinfo_flags

#define	PRCONF		"conf"

#ifndef	PARAMBUFLEN
#define	PARAMBUFLEN	256
#endif


/* external subroutines */

extern int	sncpy3(char *,int,cchar *,cchar *,cchar *) ;
extern int	snwcpy(char *,int,cchar *,int) ;
extern int	mkpath1(char *,cchar *) ;
extern int	mkpath1w(char *,cchar *,int) ;
extern int	mkpath2(char *,cchar *,cchar *) ;
extern int	mkpath3(char *,cchar *,cchar *,cchar *) ;
extern int	mkfnamesuf1(char *,cchar *,cchar *) ;
extern int	nleadstr(cchar *,cchar *,int) ;
extern int	sfbasename(cchar *,int,cchar **) ;
extern int	pathclean(char *,cchar *,int) ;
extern int	perm(cchar *,uid_t,gid_t,gid_t *,int) ;
extern int	fperm(int,uid_t,gid_t,gid_t *,int) ;
extern int	sperm(IDS *,struct ustat *,int) ;
extern int	permsched(cchar **,vecstr *,char *,int,cchar *,int) ;
extern int	vecstr_adduniq(vecstr *,cchar *,int) ;
extern int	vecstr_envadd(vecstr *,cchar *,cchar *,int) ;
extern int	vecstr_envset(vecstr *,cchar *,cchar *,int) ;
extern int	vecstr_loadfile(vecstr *,int,cchar *) ;

extern int	prmktmpdir(cchar *,char *,cchar *,mode_t) ;

#if	CF_DEBUGS
extern int	debugprintf(cchar *,...) ;
#endif

extern char	*strwcpy(char *,cchar *,int) ;


/* external variables */

extern char	**environ ;


/* local structures */

struct subinfo_flags {
	uint		localconf:1 ;
} ;

struct subinfo {
	SUBINFO_FL	f ;
	PARAMFILE	pf ;
	VARMK		v ;
	cchar	**envv ;
	cchar	*prconf ;
	cchar	*pr ;
	cchar	*cfname ;
} ;


/* forward references */

static int	subinfo_start(SUBINFO *,cchar *,cchar **,cchar *) ;
static int	subinfo_finish(SUBINFO *) ;
static int	subinfo_startsubs(SUBINFO *,VECSTR *) ;
static int	subinfo_confglobal(SUBINFO *,char *) ;
static int	subinfo_conflocal(SUBINFO *,char *) ;
static int	subinfo_proc(SUBINFO *) ;


/* global variables */


/* local variables */

static cchar	*schedconf[] = {
	"%p/etc/%n.%f",
	"%p/etc/%f",
	"%p/%n.%f",
	NULL
} ;


/* exported subroutines */


int pcsmkconf(pr,envv,cfname)
cchar	pr[] ;
cchar	*envv[] ;
cchar	cfname[] ;
{
	SUBINFO		si ;
	int		rs ;
	int		rs1 ;

	if (pr == NULL) return SR_FAULT ;

#if	CF_DEBUGS
	debugprintf("pcsmkconf: pr=%s\n",pr) ;
	debugprintf("pcsmkconf: cfname=%s\n",cfname) ;
#endif

	if (pr[0] == '\0')
	    return SR_INVALID ;

	if ((cfname != NULL) && (cfname[0] == '\0')) return SR_INVALID ;

	if ((rs = subinfo_start(&si,pr,envv,cfname)) >= 0) {

	    rs = subinfo_proc(&si) ;

	    rs1 = subinfo_finish(&si) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (subinfo) */

	if (rs == SR_EXIST) rs = SR_OK ;

#if	CF_DEBUGS
	debugprintf("pcsmkconf: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (pcsmkconf) */


/* local subroutines */


static int subinfo_start(sip,pr,envv,cfname)
SUBINFO		*sip ;
cchar	pr[] ;
cchar	*envv[] ;
cchar	cfname[] ;
{
	VECSTR		subs ;
	const mode_t	vm = 0444 ;
	cint	of = O_CREAT ;
	cint	n = 20 ;
	cint	f_global = (cfname == NULL) ;
	int		rs = SR_OK ;
	char		dbname[MAXPATHLEN+1] ;

	if (envv == NULL) envv = (cchar **) environ ;

	memset(sip,0,sizeof(SUBINFO)) ;
	sip->envv = envv ;
	sip->prconf = PRCONF ;
	sip->pr = pr ;
	sip->cfname = cfname ;

/* open the PCS-configuration file (if it exists) */

	if ((rs = vecstr_start(&subs,4,0)) >= 0) {
	    char	tmpfname[MAXPATHLEN+1] ;

	    rs = subinfo_startsubs(sip,&subs) ;

	    if ((rs >= 0) && (cfname == NULL)) {
	        mode_t	m = R_OK ;
	        int	tlen = MAXPATHLEN ;
	        char	*tbuf = tmpfname ;
	        cfname = tmpfname ;
	        rs = permsched(schedconf,&subs, tbuf,tlen, sip->prconf,m) ;
	    }

	    vecstr_finish(&subs) ;
	} /* end if (subs) */

	if (rs < 0) goto bad0 ;

	if (f_global) {
	    rs = subinfo_confglobal(sip,dbname) ;
	} else {
	    rs = subinfo_conflocal(sip,dbname) ;
	}
	if (rs < 0) goto bad1 ;

/* see if we can create a new VAR DB */

	rs = varmk_open(&sip->v,dbname,of,vm,n) ;
	if (rs < 0) goto bad1 ; /* can return SR_EXIST */

/* done */
ret0:
	return rs ;

/* bad stuff */
bad1:
bad0:
	goto ret0 ;
}
/* end subroutine (subinfo_start) */


static int subinfo_finish(sip)
SUBINFO		*sip ;
{
	int		rs = SR_OK ;
	int		rs1 ;

	rs1 = varmk_close(&sip->v) ;
	if (rs >= 0) rs = rs1 ;

	return rs ;
}
/* end subroutine (subinfo_finish) */


static int subinfo_startsubs(SUBINFO *sip,VECSTR *slp)
{
	int		rs = SR_OK ;
	int		cl ;
	cchar	*cp ;

	cl = sfbasename(sip->pr,-1,&cp) ;
	if (cl <= 0) rs = SR_INVALID ;

	if (rs >= 0)
	    rs = vecstr_envadd(slp,"p",sip->pr,-1) ;

	if (rs >= 0)
	    rs = vecstr_envadd(slp,"n",cp,cl) ;

	return rs ;
}
/* end subroutine (subinfo_startsubs) */


static int subinfo_confglobal(SUBINFO *sip,char *dbname)
{
	const mode_t	dm = 0777 ;
	int		rs ;
	cchar	*cdname = "pcsconf" ;
	char		tmpdname[MAXPATHLEN+1] ;

	if ((rs = prmktmpdir(sip->pr,tmpdname,cdname,dm)) >= 0) {
	    rs = mkpath2(dbname,tmpdname,sip->prconf) ;
	}

	return rs ;
}
/* end subroutine (subinfo_confglobal) */


static int subinfo_conflocal(SUBINFO *sip,char *dbname)
{
	const mode_t	dm = 0775 ;
	int		rs ;
	cchar		*cdname = "pcsconf" ;
	char		tmpdname[MAXPATHLEN+1] ;

	if ((rs = mktmpuserdir(tmpdname,"-",cdname,dm)) >= 0) {
	    rs = mkpath2(dbname,tmpdname,sip->prconf) ;
	}

	return rs ;
}
/* end subroutine (subinfo_conflocal) */


static int subinfo_proc(SUBINFO *sip)
{
	PARAMFILE	*pfp = &sip->pf ;
	PARAMFILE_CUR	cur ;
	PARAMFILE_ENT	pe ;
	int		rs ;
	int		rs1 ;
	if ((rs = paramfile_open(pfp,sip->envv,sip->cfname)) >= 0) {
	    if ((rs = paramfile_curbegin(pfp,&cur)) >= 0) {
	        cint	plen = PARAMBUFLEN ;
	        int	kl ;
	        char	pbuf[PARAMBUFLEN+1] ;
	        while (rs >= 0) {
	            kl = paramfile_enum(pfp,&cur,&pe,pbuf,plen) ;
	            if (kl == SR_NOTFOUND) break ;
	            rs = kl ;
		    if (rs >= 0) {
	                rs = varmk_addvar(&sip->v,pe.key,pe.value,pe.vlen) ;
		    }
	        } /* end while (reading parameters) */
	        paramfile_curend(pfp,&cur) ;
	    } /* end if (cursor) */
	    rs1 = paramfile_close(pfp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (paramfile) */
	return rs ;
}
/* end subroutine (subinfo_proc) */


