/* progdb SUPPORT */
/* lang=C++20 */

/* process the system password data and write out to the index file */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time debug print-outs */
#define	CF_DEBUG	0		/* run-time debugging */
#define	CF_DEBUGFILE	1		/* debug index file */
#define	CF_MKGECOSNAME	1		/* "should" be much faster? */
#define	CF_USEREALNAME	1		/* use realname from |pwfile| */

/* revision history:

	= 2002-04-29, David A�D� Morano
	This subroutine was inspired by something previous (I do
	not remember what it was).

	= 2017-09-12, David A�D� Morano
	Small enhancement to allocate the (on-stack) buffer for the
	index file magic string to be PWIHDR_MAGICSIZE instead of
	just 16 (what that define is anyway).

*/

/* Copyright � 2002,2017 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine reads the system PASSWD database and creates
	an inverse password database file (ipasswd) that maps real
	names to usernames.

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<climits>
#include	<cstring>
#include	<usystem.h>
#include	<endian.h>
#include	<getbufsize.h>
#include	<bfile.h>
#include	<pwfile.h>
#include	<setstr.h>
#include	<getax.h>
#include	<gecos.h>
#include	<realname.h>
#include	<strtab.h>
#include	<localmisc.h>

#include	"config.h"
#include	"defs.h"

#include	"recorder.h"
#include	"ipasswd.h"
#include	"pwihdr.h"


/* local defines */

#undef	USERNAMELEN
#ifdef	IPASSWD_USERNAME
#define	USERNAMELEN	IPASSWD_USERNAMELEN
#else
#define	USERNAMELEN	32
#endif

#ifndef	REALNAMELEN
#define	REALNAMELEN	100		/* real name length */
#endif

#define	NREC_GUESS	100		/* guess of number of records */

#ifndef	ITEMLEN
#define	ITEMLEN		100
#endif

#define	WRCACHE		struct wrcache

#define	PI		PROGINFO


/* external subroutines */

extern int	snsd(char *,int,cchar *,uint) ;
extern int	snsds(char *,int,cchar *,cchar *) ;
extern int	snrealname(char *,int,cchar **,int) ;
extern int	sncpy2(char *,int,cchar *,cchar *) ;
extern int	sncpy5(char *,int,cchar *,cchar *,cchar *,cchar *,cchar *) ;
extern int	snwcpy(char *,int,cchar *,int) ;
extern int	snwcpyhyphen(char *,int,cchar *,int) ;
extern int	mkpath1w(char *,cchar *,int) ;
extern int	mkpath1(char *,cchar *) ;
extern int	mkpath2(char *,cchar *,cchar *) ;
extern int	mkfnamesuf1(char *,cchar *,cchar *) ;
extern int	mkfnamesuf2(char *,cchar *,cchar *,cchar *) ;
extern int	mkmagic(char *,int,cchar *) ;
extern int	sfdirname(cchar *,int,cchar **) ;
extern int	cfdeci(cchar *,int,int *) ;
extern int	cfdecui(cchar *,int,uint *) ;
extern int	mktmpfile(char *,mode_t,cchar *) ;
extern int	mkgecosname(char *,int,cchar *) ;
extern int	getgecosname(cchar *,int,cchar **) ;
extern int	perm(cchar *,uid_t,gid_t,gid_t *,int) ;
extern int	iceil(int,int) ;

#if	CF_DEBUGS || CF_DEBUG
extern int	debugprintf(cchar *,...) ;
extern int	strlinelen(cchar *,int,int) ;
#endif

extern char	*strwcpy(char *,cchar *,int) ;
extern char	*strwcpylc(char *,cchar *,int) ;
extern char	*strnchr(cchar *,int,int) ;


/* external variables */


/* local structures */

struct wrcache {
	bfile		ifile ;
	STRTAB		*stp ;
	RECORDER	*rp ;
	RECORDER_ENT	*rectab ;
	uint		hdr[pwihdr_overlast] ;
	uint		(*recind)[2] ;
	char		*stab ;
	int		fto ;
	int		ro ;
} ;


/* forward references */

static int	progdber(PI *,bfile *,cchar *,cchar *,int,int) ;
static int	procpw(PI *,STRTAB *,RECORDER *) ;
static int	procpwfile(PI *,STRTAB *,RECORDER *) ;
static int	procpwfiler(PI *,STRTAB *,RECORDER *,PWFILE_ENT *) ;
static int	procpwsys(PI *,STRTAB *,RECORDER *) ;

static int	procentry(PI *,STRTAB *,RECORDER *,cchar *,cchar *,int) ;
static int	wrcache(PI *,STRTAB *,RECORDER *,cchar *,cchar *,int) ;
static int	wrfile(PI *,WRCACHE *,cchar *) ;
static int	wrsetup(PI *,WRCACHE *) ;
static int	wrstuff(PI *,WRCACHE *) ;
static int	wridx(PI *,WRCACHE *) ;
static int	wridxer(PI *,WRCACHE *) ;
static int	wrstats(PI *,bfile *,RECORDER *) ;

static int	mkourtmp(char *,cchar *,cchar *,cchar *,mode_t) ;
static int	ourchmod(cchar *) ;

static int	mkpwihdr(uint *,int) ;
static int	mkro(PI *) ;

#if	CF_DEBUG && CF_DEBUGFILE
static int	progdb_debugfile(PI *,cchar *) ;
#endif


/* local variables */


/* exported variables */


/* exported subroutines */

int progdb(PI *pip,bfile *ofp,cchar *dbname) noex {
	cint		n = NREC_GUESS ;
	int		rs = SR_OK ;
	int		dnl ;
	int		c = 0 ;
	cchar		*dnp ;

	if (dbname == NULL) return SR_FAULT ;

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("progdb: ent db=%s\n",dbname) ;
#endif

/* check that the parent directory is writable by us */

	if ((dnl = sfdirname(dbname,-1,&dnp)) > 0) {
	    char	dbuf[MAXPATHLEN + 1] ;
	    if ((rs = mkpath1w(dbuf,dnp,dnl)) >= 0) {
	        cint	am = (X_OK|W_OK) ;
	        if ((rs = perm(dbuf,-1,-1,NULL,am)) >= 0) {
	            if ((rs = mkro(pip)) >= 0) {
	                cint	ro = rs ;
	                rs = progdber(pip,ofp,dbuf,dbname,ro,n) ;
	                c = rs ;
	            } /* end if (mkro) */
	        }
	    }
	} /* end if (sfdirname) */

#if	CF_DEBUG && CF_DEBUGFILE
	if (rs >= 0) {
	    progdb_debugfile(pip,dbname) ;
	}
#endif /* CF_DEBUG && CF_DEBUGFILE */

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("progdb: ret rs=%d c=%u\n",rs,c) ;
#endif

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (progdb) */

static int progdber(PI *pip,bfile *ofp,cchar *dn,cchar *db,int ro,int n) noex {
	STRTAB		st ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
#if	CF_DEBUG
	if (DEBUGLEVEL(3)) {
	    debugprintf("progdber: ent dn=%s\n",dn) ;
	    debugprintf("progdber: db=%s\n",db) ;
	}
#endif
	if ((rs = strtab_start(&st,(n * 25))) >= 0) {
	    RECORDER	rec ;
	    if ((rs = recorder_start(&rec,n,ro)) >= 0) {
	        if ((rs = procpw(pip,&st,&rec)) >= 0) {
	            if ((rs = recorder_count(&rec)) >= 0) {
	                if ((rs = wrcache(pip,&st,&rec,dn,db,ro)) >= 0) {
	                    if (pip->verboselevel > 1) {
	                        if (pip->open.outfile) {
	                            rs = wrstats(pip,ofp,&rec) ;
	                        }
	                    }
	                } /* end if (wrstats) */
	            } /* end if (recorder_counts) */
	        } /* end if (ok) */
	        rs1 = recorder_finish(&rec) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (recorder) */
	    rs1 = strtab_finish(&st) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (strtab) */
#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("progdber: ret rs=%d c=%u\n",rs,c) ;
#endif
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (progdber) */

static int procpw(PI *pip,STRTAB *stp,RECORDER *rp) noex {
	int		rs ;
	int		c = 0 ;
	if ((pip->pwfname != NULL) && (pip->pwfname[0] != '-')) {
	    rs = procpwfile(pip,stp,rp) ;
	    c = rs ;
	} else {
	    rs = procpwsys(pip,stp,rp) ;
	    c = rs ;
	}
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (procpw) */

static int procpwfile(PI *pip,STRTAB *stp,RECORDER *rtp) noex {
	PWFILE		pf ;
	PWFILE_CUR	cur ;
	PWFILE_ENT	pw ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("progdb: ent\n") ;
#endif

	if ((rs = pwfile_open(&pf,pip->pwfname)) >= 0) {
	    if ((rs = pwfile_curbegin(&pf,&cur)) >= 0) {
	        cint	pwlen = getbufsize(getbufsize_pw) ;
	        char	*pwbuf ;
	        if ((rs = uc_malloc((pwlen+1),&pwbuf)) >= 0) {
	            while ((rs = pwfile_curenum(&pf,&cur,&pw,pwbuf,pwlen)) > 0) {

#if	CF_DEBUG
	                if (DEBUGLEVEL(4))
	                    debugprintf("progdb: username=%s\n",pw.username) ;
#endif

	                rs = procpwfiler(pip,stp,rtp,&pw) ;

#if	CF_DEBUG
	                if (DEBUGLEVEL(4))
	                    debugprintf("progdb/procpwfile: while-bot rs=%d\n",
	                        rs) ;
#endif

	                if (rs < 0) break ;
	            } /* end while (looping through entries) */
	            rs1 = uc_free(pwbuf) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	        rs1 = pwfile_curend(&pf,&cur) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (cursor) */
	    rs1 = pwfile_close(&pf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (pwfile) */

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("progdb/procpwfile: ret rs=%d c=%u\n",rs,c) ;
#endif

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (procpwfile) */

static int procpwfiler(PI *pip,STRTAB *stp,RECORDER *rtp,
		PWFILE_ENT *pwp) noex {
	int		rs = SR_OK ;
	int		nl = -1 ;
	int		c = 0 ;
	cchar		*un = pwp->username ;
	cchar		*np ;

#if	CF_USEREALNAME
	if (pwp->realname != NULL) {
	    np = pwp->realname ;
	    c += 1 ;
	    rs = procentry(pip,stp,rtp,un,np,nl) ;
	} else {
	    if ((nl = getgecosname(pwp->gecos,-1,&np)) > 0) {
	        char	nbuf[nl + 1] ;
	        if (strnchr(np,nl,'-') != NULL) {
	            rs = snwcpyhyphen(nbuf,-1,np,nl) ;
	            np = nbuf ;
	        }
	        if (rs >= 0) {
	            c += 1 ;
	            rs = procentry(pip,stp,rtp,un,np,nl) ;
	        }
	    }
	}
#else /* CF_USEREALNAME */
#if	CF_MKGECOSNAME
	{
	    if ((nl = getgecosname(pwp->gecos,-1,&np)) > 0) {
	        char	nbuf[nl + 1] ;
	        if (strnchr(np,nl,'-') != NULL) {
	            rs = snwcpyhyphen(nbuf,-1,np,nl) ;
	            np = nbuf ;
	        }
	        if (rs >= 0) {
	            c += 1 ;
	            rs = procentry(pip,stp,rtp,un,np,nl) ;
	        }
	    } /* end if */
	} /* end block */
#else /* CF_MKGECOSNAME */
	{
	    GECOS	ge ;
	    if ((rs = gecos_start(&ge,pwp->gecos,-1)) >= 0) {
	        cint	req = gecosval_realname ;
	        if ((nl = gecos_getval(&ge,req,&np)) > 0) {
	            char	nbuf[nl + 1] ;
	            if (strnchr(np,nl,'-') != NULL) {
	                rs = snwcpyhyphen(nbuf,-1,np,nl) ;
	                np = nbuf ;
	            }
	            if (rs >= 0) {
	                c += 1 ;
	                rs = procentry(pip,stp,rtp,un,np,nl) ;
	            }
	        } /* end block */
	        rs1 = gecos_finish(&ge) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (gecos) */
	} /* end block */
#endif /* CF_MKGECOSNAME */
#endif /* CF_USEREALNAME */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (procpwfiler) */

static int procpwsys(PI *pip,STRTAB *stp,RECORDER *rtp) noex {
	SETSTR		u ;
	cint		n = 100 ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;

	if ((rs = setstr_start(&u,n)) >= 0) {
	    PASSWD	pw ;
	    cint	pwlen = getbufsize(getbufsize_pw) ;
	    char	*pwbuf ;
	    if ((rs = uc_malloc((pwlen+1),&pwbuf)) >= 0) {
	        if ((rs = getpw_begin()) >= 0) {
	            while ((rs = getpw_ent(&pw,pwbuf,pwlen)) > 0) {
	                cchar	*un = pw.pw_name ;
	                if ((rs = setstr_add(&u,un,-1)) > 0) {
	                    GECOS	ge ;
	                    cchar	*gecos = pw.pw_gecos ;
	                    if ((rs = gecos_start(&ge,gecos,-1)) >= 0) {
	                        cint	req = gecosval_realname ;
	                        int		nl ;
	                        cchar		*np ;
	                        if ((nl = gecos_getval(&ge,req,&np)) > 0) {
	                            char	nbuf[nl+1] ;

	                            if (strnchr(np,nl,'-') != NULL) {
	                                rs = snwcpyhyphen(nbuf,-1,np,nl) ;
	                                np = nbuf ;
	                            }

	                            if (rs >= 0) {
	                                c += 1 ;
	                                rs = procentry(pip,stp,rtp,un,np,nl) ;
	                            }

#if	CF_DEBUG
	                            if (DEBUGLEVEL(4))
	                                debugprintf("progdb/procpwsys: "
	                                    "procentry() rs=%d\n",rs) ;
#endif

	                        } /* end if (gecos_getval) */
#if	CF_DEBUG
	                        if (DEBUGLEVEL(4))
	                            debugprintf("progdb/procpwsys: "
	                                "gecos_getval-out rs=%d\n",rs) ;
#endif
	                        rs1 = gecos_finish(&ge) ;
	                        if (rs >= 0) rs = rs1 ;
	                    } /* end if (gecos) */
	                } /* end if (setstr_add) */
#if	CF_DEBUG
	                if (DEBUGLEVEL(4))
	                    debugprintf("progdb/procpwsys: gecos-out rs=%d\n",
	                        rs) ;
#endif
	                if (rs < 0) break ;
	            } /* end while (looping through entries) */
	            rs1 = getpw_end() ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (getpw) */
	        rs1 = uc_free(pwbuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	    rs1 = setstr_finish(&u) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (setstr) */

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("progdb/procpwsys: ret rs=%d c=%u\n",rs,c) ;
#endif

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (procpwsys) */

static int procentry(PI *pip,STRTAB *stp,RECORDER *rtp,cchar *username,
		cchar *nbuf,int nlen) noex {
	REALNAME	rn ;
	int		rs ;
	int		rs1 ;

	if (pip == NULL) return SR_FAULT ;
	if (username == NULL) return SR_FAULT ;
	if (nbuf == NULL) return SR_FAULT ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4)) {
	    debugprintf("procentry: username=%s\n",username) ;
	    debugprintf("procentry: rname=>%r<\n",nbuf,nlen) ;
	}
#endif

	if ((rs = realname_start(&rn,nbuf,nlen)) >= 0) {
	    RECORDER_ENT	re ;
	    int			len ;
	    int			i_username ;
	    int			i_last, i_first, i_m1, i_m2 ;
	    char		buf[ITEMLEN + 1], *bp ;

/* username */

	    len = strnlen(username,USERNAMELEN) ;

	    i_username = strtab_already(stp,username,len) ;

#if	CF_DEBUG
	    if (DEBUGLEVEL(4)) {
	        debugprintf("procentry: un=%s\n",username) ;
	        debugprintf("procentry: strtab_already() i_username=%d\n",
	            i_username) ;
	    }
#endif

	    if (i_username < 0) {
	        rs = strtab_add(stp,username,len) ;
	        i_username = rs ;
#if	CF_DEBUG
	        if (DEBUGLEVEL(4)) {
	            debugprintf("procentry: strtab_add() rs=%d i_username=%d\n",
	                rs,i_username) ;
	            debugprintf("procentry: un=%s\n",username) ;
	        }
#endif
	    }

/* last */

#if	CF_DEBUG
	    if (DEBUGLEVEL(4))
	        debugprintf("procentry: last=%s\n",rn.last) ;
#endif

	    i_last = 0 ;
	    if ((rs >= 0) && (rn.last != NULL)) {

	        bp = strwcpylc(buf,rn.last,ITEMLEN) ;

#if	CF_DEBUG
	        if (DEBUGLEVEL(4))
	            debugprintf("procentry: lowered last=%s\n",buf) ;
#endif

	        i_last = strtab_already(stp,buf,(bp - buf)) ;

#if	CF_DEBUG
	        if (DEBUGLEVEL(4))
	            debugprintf("procentry: strtab_already() i_last=%d\n",
	                i_last) ;
#endif

	        if (i_last < 0) {
	            rs = strtab_add(stp,buf,(bp - buf)) ;
	            i_last = rs ;
#if	CF_DEBUG
	            if (DEBUGLEVEL(4))
	                debugprintf("procentry: strtab_add() rs=%d i_last=%d\n",
	                    rs,i_last) ;
#endif
	        }
	    }

/* first */

	    i_first = 0 ;
	    if ((rs >= 0) && (rn.first != NULL)) {
	        bp = strwcpylc(buf,rn.first,ITEMLEN) ;
	        if ((i_first = strtab_already(stp,buf,(bp - buf))) < 0) {
	            rs = strtab_add(stp,buf,(bp - buf)) ;
	            i_first = rs ;
	        }
	    }

/* middle-1 */

	    i_m1 = 0 ;
	    if ((rs >= 0) && (rn.m1 != NULL)) {
	        bp = strwcpylc(buf,rn.m1,ITEMLEN) ;
	        if ((i_m1 = strtab_already(stp,buf,(bp - buf))) < 0) {
	            rs = strtab_add(stp,buf,(bp - buf)) ;
	            i_m1 = rs ;
	        }
	    }

/* middle-2 */

	    i_m2 = 0 ;
	    if ((rs >= 0) && (rn.m2 != NULL)) {
	        bp = strwcpylc(buf,rn.m2,ITEMLEN) ;
	        if ((i_m2 = strtab_already(stp,buf,(bp - buf))) < 0) {
	            rs = strtab_add(stp,buf,(bp - buf)) ;
	            i_m2 = rs ;
	        }
	    }

/* put the record together */

	    if (rs >= 0) {
	        re.username = i_username ;
	        re.last = i_last ;
	        re.first = i_first ;
	        re.m1 = i_m1 ;
	        re.m2 = i_m2 ;
	        rs = recorder_add(rtp,&re) ;
	    } /* end if (adding record to DB) */

	    rs1 = realname_finish(&rn) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (realname) */

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("progdb/procentry: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (procentry) */

static int wrcache(PI *pip,STRTAB *stp,RECORDER *rp,
		cchar *dn,cchar *db,int ro) noex {
	cmode		fm = 0664 ;
	int		rs ;
	cchar		*suf = DBSUF ;
	cchar		*es = ENDIANSTR ;
	char		tbuf[MAXPATHLEN + 1] ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("wrcache: ent db=%s\n",db) ;
#endif

	if ((rs = mkourtmp(tbuf,dn,suf,es,fm)) >= 0) {
	    WRCACHE		wc ;
	    memset(&wc,0,sizeof(WRCACHE)) ;
	    wc.stp = stp ;
	    wc.rp = rp ;
	    wc.ro = ro ;
#if	CF_DEBUG
	    if (DEBUGLEVEL(4))
	        debugprintf("wrcache: tbuf=%s\n",tbuf) ;
#endif
	    if ((rs = wrfile(pip,&wc,tbuf)) >= 0) {
	        if ((rs = ourchmod(tbuf)) >= 0) {
	            char	nbuf[MAXPATHLEN+1] ;
	            if ((rs = mkfnamesuf2(nbuf,db,suf,es)) >= 0) {
	                rs = u_rename(tbuf,nbuf) ;
	            }
	        }
	        if (rs < 0) {
	            uc_unlink(tbuf) ;
	        }
	    } /* end if (wrfile) */
	} /* end if (mkourtmp) */

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("wrcache: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (wrcache) */

static int wrfile(PI *pip,WRCACHE *wcp,cchar *fn) noex {
	bfile		*ifp = &wcp->ifile ;
	int		rs ;
	int		rs1 ;
	if ((rs = bopen(ifp,fn,"wct",0664)) >= 0) {
	    cint	ml = PWIHDR_MAGICSIZE ;
	    cchar	*ms = PWIHDR_MAGICSTR ;
	    char	vetu[PWIHDR_MAGICSIZE+1] ;
	    if ((rs = mkmagic(vetu,ml,ms)) >= 0) {
#if	CF_DEBUG
	        if (DEBUGLEVEL(4))
	            debugprintf("wrfile: rs=%d magic=%s\n",rs,vetu) ;
#endif
	        if ((rs = bwrite(ifp,vetu,rs)) >= 0) {
	            wcp->fto += rs ;
	            vetu[0] = PWIHDR_VERSION ;
	            vetu[1] = ENDIAN ;
	            vetu[2] = wcp->ro ;
	            vetu[3] = 0 ;
	            if ((rs = bwrite(ifp,vetu,4)) >= 0) {
	                wcp->fto += rs ;
	                rs = wrsetup(pip,wcp) ;
	            }
	        }
	    } /* end if (mkmagic) */
	    rs1 = bclose(ifp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (bfile) */
#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("wrfile: ret rs=%d\n",rs) ;
#endif
	return rs ;
}
/* end subroutine (wrfile) */

static int wrsetup(PI *pip,WRCACHE *wcp) noex {
	RECORDER	*rp = wcp->rp ;
	RECORDER_ENT	*rectab ;
	int		rs ;
	int		rs1 ;
	if ((rs = recorder_gettab(rp,&rectab)) >= 0) {
	    STRTAB	*stp = wcp->stp ;
	    uint	*hdr = wcp->hdr ;
	    wcp->rectab = rectab ;
	    hdr[pwihdr_recsize] = rs ;
	    if ((rs = recorder_rtlen(rp)) >= 0) {
	        hdr[pwihdr_reclen] = rs ;
	        if ((rs = strtab_strsize(stp)) >= 0) {
	            cint	stsize = rs ;
	            char	*stab ;
	            if ((rs = uc_malloc(stsize,&stab)) >= 0) {
	                hdr[pwihdr_strsize] = stsize ;
	                wcp->stab = stab ;
	                if ((rs = strtab_strmk(stp,stab,stsize)) >= 0) {
	                    hdr[pwihdr_strlen] = rs ;
#if	CF_DEBUG
	                    if (DEBUGLEVEL(5)) {
	                        debugprintf("procdb/wrsetup: reclen=%u\n",
	                            hdr[pwihdr_reclen]) ;
	                        debugprintf("procdb/wrsetup: recsize=%u\n",
	                            hdr[pwihdr_recsize]) ;
	                        debugprintf("procdb/wrsetup: strlen=%u\n",
	                            hdr[pwihdr_strlen]) ;
	                        debugprintf("procdb/wrsetup: strsize=%u\n",
	                            hdr[pwihdr_strsize]) ;
	                    }
#endif
	                    if ((rs = recorder_indsize(rp)) >= 0) {
	                        hdr[pwihdr_idxsize] = rs ;
	                        if ((rs = recorder_indlen(rp)) >= 0) {
	                            hdr[pwihdr_idxlen] = rs ;
	                            rs = wrstuff(pip,wcp) ;
	                        }
	                    }
	                } /* end if (strtab_strmk) */
	                rs1 = uc_free(stab) ;
	                if (rs >= 0) rs = rs1 ;
	                wcp->stab = NULL ;
	            } /* end if (m-a-f) */
	        }
	    }
	} /* end if (recorder_gettab) */
	return rs ;
}
/* end subroutine (wrsetup) */

static int wrstuff(PI *pip,WRCACHE *wcp) noex {
	bfile		*ifp = &wcp->ifile ;
	cint		hdrsize = (pwihdr_overlast * sizeof(uint)) ;
	uint		*hdrp = wcp->hdr ;
	int		rs ;
	int		fto = wcp->fto ;
#if	CF_DEBUG
	if (DEBUGLEVEL(5))
	    debugprintf("procdb/wrstuff: ent fto=%u\n",fto) ;
#endif
	if ((rs = mkpwihdr(hdrp,(fto+hdrsize))) >= 0) {
#if	CF_DEBUG
	    if (DEBUGLEVEL(5)) {
	        debugprintf("procdb/wrstuff: rectab=%u\n",hdrp[pwihdr_rectab]) ;
	        debugprintf("procdb/wrstuff: strtab=%u\n",hdrp[pwihdr_strtab]) ;
	    }
#endif
	    if ((rs = bwrite(ifp,hdrp,hdrsize)) >= 0) {
	        RECORDER_ENT	*rectab = wcp->rectab ;
	        cint	rtsize = hdrp[pwihdr_recsize] ;
	        if ((rs = bwrite(ifp,rectab,rtsize)) >= 0) {
	            cint	stsize = hdrp[pwihdr_strsize] ;
	            char	*stab = wcp->stab ;
	            if ((rs = bwrite(ifp,stab,stsize)) >= 0) {
	                rs = wridx(pip,wcp) ;
	            }
	        }
	    }
	} /* end if (mkpwihdr) */
	return rs ;
}
/* end subroutine (wrstuff) */

static int wridx(PI *pip,WRCACHE *wcp) noex {
	bfile		*ifp = &wcp->ifile ;
	uint		(*recind)[2] ;
	uint		*hdrp = wcp->hdr ;
	int		idxsize ;
	int		rs ;
	int		rs1 ;
	idxsize = hdrp[pwihdr_idxsize] ;
	if ((rs = uc_malloc(idxsize,&recind)) >= 0) {
	    RECORDER	*rp = wcp->rp ;
	    char	*stab = wcp->stab ;
	    wcp->recind = recind ;
	    if ((rs = recorder_mkindl1(rp,stab,recind,idxsize)) >= 0) {
	        if ((rs = bwrite(ifp,recind,idxsize)) >= 0) {
	            if ((rs = recorder_mkindl3(rp,stab,recind,idxsize)) >= 0) {
	                if ((rs = bwrite(ifp,recind,idxsize)) >= 0) {
	                    rs = wridxer(pip,wcp) ;
	                }
	            }
	        }
	    }
	    rs1 = uc_free(recind) ;
	    if (rs >= 0) rs = rs1 ;
	    wcp->recind = NULL ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end subroutine (wridx) */

static int wridxer(PI *pip,WRCACHE *wcp) noex {
	RECORDER	*rp = wcp->rp ;
	bfile		*ifp = &wcp->ifile ;
	uint		(*recind)[2] = wcp->recind ;
	uint		*hdrp = wcp->hdr ;
	int		rs ;
	int		idxsize ;
	char		*stab = wcp->stab ;
	if (pip == NULL) return SR_FAULT ;
	idxsize = hdrp[pwihdr_idxsize] ;
	if ((rs = recorder_mkindf(rp,stab,recind,idxsize)) >= 0) {
	    if ((rs = bwrite(ifp,recind,idxsize)) >= 0) {
	        if ((rs = recorder_mkindfl3(rp,stab,recind,idxsize)) >= 0) {
	            if ((rs = bwrite(ifp,recind,idxsize)) >= 0) {
	                cint	ris = hdrp[pwihdr_idxsize] ;
	                if ((rs = recorder_mkindun(rp,stab,recind,ris)) >= 0) {
	                    rs = bwrite(ifp,recind,idxsize) ;
	                }
	            }
	        }
	    }
	}
	return rs ;
}
/* end subroutine (wridxer) */

static int wrstats(PI *pip,bfile *ofp,RECORDER *recp) noex {
	RECORDER_INFO	s ;
	int		rs ;

	if (pip == NULL) return SR_FAULT ;
	if ((rs = recorder_info(recp,&s)) >= 0) {
	    uint	sum ;
	    int		i, j ;

	    bprintf(ofp,"       records %6u\n",rs) ;
	    bprintf(ofp,"  index length %6u\n",s.ilen) ;
	    bprintf(ofp,"L1  collisions %6u\n",s.c_l1) ;
	    bprintf(ofp,"L3  collisions %6u\n",s.c_l3) ;
	    bprintf(ofp,"F   collisions %6u\n",s.c_f) ;
	    bprintf(ofp,"FL3 collisions %6u\n",s.c_fl3) ;
	    bprintf(ofp,"UN  collisions %6u\n",s.c_un) ;

	    sum = s.c_l1 + s.c_l3 + s.c_f + s.c_fl3 + s.c_un ;
	    bprintf(ofp,"T   collisions %6u\n",sum) ;

	    bprintf(ofp,"index\t %5s %5s %5s %5s %5s\n",
	        "l1","l3","f","fl3","un") ;

	    for (j = 0 ; j < RECORDER_NCOLLISIONS ; j += 1) {
	        bprintf(ofp,"\t") ;
	        for (i = 0 ; i < 5 ; i += 1) {
	            bprintf(ofp," %5u",s.cden[i][j]) ;
	        } /* end for */
	        bprintf(ofp,"\n") ;
	    } /* end for */

	} /* end if */

	return rs ;
}
/* end subroutine (wrstats) */

static int mkpwihdr(uint *hdrp,int fto) noex {
	hdrp[pwihdr_wrcount] = 0 ;
	hdrp[pwihdr_rectab] = fto ;
	fto += hdrp[pwihdr_recsize] ;
	hdrp[pwihdr_strtab] = fto ;
	fto += hdrp[pwihdr_strsize] ;
	hdrp[pwihdr_idxl1] = fto ;
	fto += hdrp[pwihdr_idxsize] ;
	hdrp[pwihdr_idxl3] = fto ;
	fto += hdrp[pwihdr_idxsize] ;
	hdrp[pwihdr_idxf] = fto ;
	fto += hdrp[pwihdr_idxsize] ;
	hdrp[pwihdr_idxfl3] = fto ;
	fto += hdrp[pwihdr_idxsize] ;
	hdrp[pwihdr_idxun] = fto ;
	fto += hdrp[pwihdr_idxsize] ;
	return fto ;
}
/* end subroutine (mkpwihdr) */

static int mkourtmp(char *rbuf,cc *dbuf,cc *suf,cc *es,mode_t fm) noex {
	cint		clen = MAXNAMELEN ;
	int		rs ;
	cchar		*xx = "progdbXX" ;
	char		cbuf[MAXNAMELEN+1] ;
	if ((rs = sncpy5(cbuf,clen,xx,".",suf,es,"n")) >= 0) {
	    char	tbuf[MAXPATHLEN+1] ;
	    if ((rs = mkpath2(tbuf,dbuf,cbuf)) >= 0) {
	        rs = mktmpfile(rbuf,fm,tbuf) ;
	    }
	}
	return rs ;
}
/* end subroutine (mkourtmp) */

static int mkro(PI *pip) noex {
	int		rs = SR_OK ;
	int		ro = (RECORDER_OSEC | RECORDER_ORANDLC) ;
	if ((pip->typespec != NULL) && (pip->typespec[0] != '\0')) {
	    int	ft = 0 ;
	    if ((rs = cfdeci(pip->typespec,-1,&ft)) >= 0) {
	        if (ft <= 2) {
	            switch (ft) {
	            case 1:
	                ro |= RECORDER_OSEC ;
	                break ;
	            case 2:
	                ro |= (RECORDER_OSEC | RECORDER_ORANDLC) ;
	                break ;
	            } /* end switch */
	        } else {
	            rs = SR_INVALID ;
	        }
	    } else {
	        rs = SR_INVALID ;
	    }
	} /* end if (file options) */
	return (rs >= 0) ? ro : rs ;
}
/* end subroutine (mkro) */

static int ourchmod(cchar *fn) noex {
	USTAT		sb ;
	int		rs ;
	if ((rs = uc_stat(fn,&sb)) >= 0) {
	    cmode	fm = (S_IREAD|S_IWRITE|S_IRGRP|S_IWGRP|S_IROTH) ;
	    if ((sb.st_mode & fm) != fm) {
	        rs = u_chmod(fn,(sb.st_mode | fm)) ;
	    }
	}
	return rs ;
}
/* end subroutine (ourchmod) */

#if	CF_DEBUG && CF_DEBUGFILE
static int progdb_debugfile(PI *pip,cchar *dbname) noex {
	int		rs = SR_OK ;
	if (DEBUGLEVEL(4)) {
	    IPASSWD	pwi ;
	    IPASSWD_CUR	cur ;
	    cint	rlen = REALNAMELEN ;
	    cint	nlen = REALNAMELEN ;
	    int		rs1 ;
	    int		rs2 ;
	    cchar	*sa[6] ;
	    char	ubuf[IPASSWD_USERNAMELEN + 1] ;
	    char	rbuf[REALNAMELEN+1] ;
	    char	nbuf[REALNAMELEN+1] ;
	    rs1 = ipasswd_open(&pwi,dbname) ;
	    debugprintf("progdb: ipasswd_open() rs=%d\n",rs1) ;
	    if ((rs2 = ipasswd_curbegin(&pwi,&cur)) >= 0) {
	        while (TRUE) {
	            rs2 = ipasswd_curenum(&pwi,&cur,
	                ubuf,sa,rbuf,rlen) ;
	            if (rs2 < 0) break ;
	            snrealname(nbuf,nlen,sa,-1) ;
	        }
	        ipasswd_curend(&pwi,&cur) ;
	    } /* end if (enumerating) */
	    ipasswd_close(&pwi) ;
	}
	return rs ;
}
/* end subroutine (procdb_debugfile) */
#endif /* CF_DEBUGFILE */


