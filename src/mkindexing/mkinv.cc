/* proginv SUPPORT */
/* charset=ISO8859-1 */
/* lang=C20 */

/* invert the data (keys to pointers) */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	proginv

	Description:
	This subroutine processes a single file.

	Synopsis:
	int proginv(PROGINFO *pip,ARGINFO *aip,cchar *dbname) noex

	Arguments:
	pip		program information pointer
	aip		argument-information-pointer
	dbname		DB name

	Returns:
	>=0		OK
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* must be before others */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<baops.h>
#include	<bfile.h>
#include	<vecobj.h>
#include	<vecstr.h>
#include	<vecpstr.h>
#include	<txtindexmk.h>
#include	<dirseen.h>
#include	<nulstr.h>
#include	<char.h>
#include	<ascii.h>
#include	<ischarx.h>
#include	<localmisc.h>		/* |NATURALWORDLEN| */

#include	"config.h"
#include	"defs.h"
#include	"taginfo.h"
#include	"xwords.h"
#include	"mkcmds.h"
#include	"progids.h"
#include	"progeigen.h"


/* local defines */

#ifndef	KEYBUFLEN
#define	KEYBUFLEN	NATURALWORDLEN
#endif

#define	MKINV_DIRPERM	0777
#define	MKINV_INDPERM	0664

#define	TO_MKWAIT	20

#define	SUBINFO		struct subinfo
#define	SUBINFO_FL	struct subinfo_flags

#define	KTAG		struct ktag_head
#define	KTAG_PARAMS	struct ktag_params
#define	KTAG_KEY	TXTINDEXMK_KEY


/* external subroutines */


/* external variables */


/* local structures */

struct subinfo_flags {
	uint		tl:1 ;
	uint		sdn:1 ;
	uint		sfn:1 ;
	uint		txtopen:1 ;
} ;

struct subinfo {
	PROGINFO	*pip ;
	SUBINFO_FL	f ;
	VECPSTR		eigenwords ;
	TXTINDEXMK	tm ;
	uint		minwlen ;
	uint		maxwlen ;
	uint		maxkeys ;
	uint		tablen ;
	cchar		*sdn ;
	cchar		*sfn ;
	cchar		*dbname ;
} ;

struct ktag_params {
	int		minwlen ;
	int		f_eigen ;
	int		f_nofile ;
} ;

struct ktag_head {
	cchar		*fname ;
	KTAG_PARAMS	*kap ;
	TXTINDEXMK_KEY	*tkeys ;	/* storage for TXTMKINDEXMK_ADDTAGS */
	VECPSTR		store ;
	vecobj		keys ;
	ulong		recoff ;
	ulong		reclen ;
	int		f_store ;
} ;


/* forward references */

static int	procmk(PROGINFO *,ARGINFO *) ;
static int	procfiles(PROGINFO *,ARGINFO *) ;
static int	procfile(PROGINFO *,cchar *) ;
static int	procmkeigen(PROGINFO *) ;

static int	procdbcheck(PROGINFO *,cchar *,mode_t) ;
static int	procdnamecheck(PROGINFO *,cchar *,mode_t) ;

static int	proccmd(PROGINFO *,cchar *,int) ;
static int	proccmd_eigen(PROGINFO *,cchar *,int) ;

static int	loadsubstr(PROGINFO *,cchar **,cchar *,int) ;

static int	subinfo_start(SUBINFO *,PROGINFO *,cchar *) ;
static int	subinfo_finish(SUBINFO *) ;
static int	subinfo_eigendefs(SUBINFO *) ;
static int	subinfo_txtopen(SUBINFO *) ;
static int	subinfo_txtclose(SUBINFO *) ;
static int	subinfo_addtags(SUBINFO *,TXTINDEXMK_TAG *,int) ;

#ifdef	COMMENT
static int	subinfo_minwlen(SUBINFO *) ;
#endif

static int	ktag_start(KTAG *,KTAG_PARAMS *,TAGINFO *,cchar *,cchar *,int) ;
static int	ktag_add(KTAG *,cchar *,int) ;
static int	ktag_procline(KTAG *,cchar *,int) ;
static int	ktag_mktag(KTAG *,TXTINDEXMK_TAG *) ;
static int	ktag_finish(KTAG *) ;
static int	ktag_procword(KTAG *,cchar *,int) ;


/* local variables */


/* exported subroutines */

int proginv(PROGINFO *pip,ARGINFO *aip,cchar *dbname) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		pan = 0 ;
	cchar		*pn = pip->progname ;
	cchar		*fmt ;

	if (aip == NULL) return SR_FAULT ;

	if (dbname == NULL) {
	    rs = SR_FAULT ;
	    fmt = "%s: no database specified (%d)\n" ;
	    bprintf(pip->efp,fmt,pn,rs) ;
	}

	if ((rs >= 0) && (dbname[0] == '\0')) {
	    rs = SR_INVALID ;
	    fmt = "%s: no database specified (%d)\n" ;
	    bprintf(pip->efp,fmt,pn,rs) ;
	}

	if ((rs >= 0) && (pip->debuglevel > 0)) {
	    fmt = "%s: db=%s\n" ;
	    bprintf(pip->efp,fmt,pn,dbname) ;
	}

	if (rs >= 0) {
	    if ((rs = progids_begin(pip)) >= 0) {
	        const mode_t	m = MKINV_DIRPERM ;
	        if ((rs = procdbcheck(pip,dbname,m)) >= 0) {
	            SUBINFO	si, *sip = &si ;
	            pip->sip = sip ;
	            if ((rs = subinfo_start(sip,pip,dbname)) >= 0) {
    		        {
	                    rs = procmk(pip,aip) ;
	                    pan = rs ;
    		        }
	                rs1 = subinfo_finish(sip) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (subinfo) */
	        } /* end if (db-check) */
	    } /* end if (progids_end) */
	} /* end if (ok) */

	return (rs >= 0) ? pan : rs ;
}
/* end subroutine (proginv) */


/* local subroutines */

static int procmk(PROGINFO *pip,ARGINFO *aip) noex {
	int		rs ;
	int		c = 0 ;

	if ((rs = procfiles(pip,aip)) >= 0) {
	    SUBINFO	*sip = pip->sip ;
	    c += rs ;
	    if ((rs = subinfo_eigendefs(sip)) >= 0) {
	    	rs = procmkeigen(pip) ;
	    }
	}

	if (pip->debuglevel > 0) {
	    bprintf(pip->efp,"%s: tags=%u\n",pip->progname,c) ;
	}

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (procmk) */

static int procmkeigen(PROGINFO *pip) noex {
	SUBINFO		*sip = pip->sip ;
	TXTINDEXMK_KEY	*keys = NULL ;
	cint		nkeys = pip->eigenwords ;
	int		rs ;
	int		c = 0 ;

	if ((rs = subinfo_txtopen(sip)) >= 0) {
	    cint	size = (nkeys + 1) * szof(TXTINDEXMK_KEY) ;
	    if ((rs = uc_malloc(size,&keys)) >= 0) {
		VECPSTR		*esp = &sip->eigenwords ;
		TXTINDEXMK	*tmp = &sip->tm ;
		int		wl ;
	        int		i ;
	        int		j = 0 ;
		cchar		*wp ;
	        for (i = 0 ; vecpstr_get(esp,i,&wp) >= 0 ; i += 1) {
	            if (wp != NULL) {
		        wl = strlen(wp) ;
	                if (i >= nkeys) {
	                    c += j ;
	                    rs = txtindexmk_addeigens(tmp,keys,j) ;
	                    j = 0 ;
	                }
	                keys[j].kp = wp ;
	                keys[j].kl = wl ;
	                j += 1 ;
		    }
	            if (rs < 0) break ;
	        } /* end for */

	        if ((rs >= 0) && (j > 0)) {
	            c += j ;
	            rs = txtindexmk_addeigens(tmp,keys,j) ;
	            j = 0 ;
	        }

	        uc_free(keys) ;
	    } /* end if (memory allocation) */
	} /* end if (subinfo_txtopen) */

	if (pip->debuglevel > 0) {
	    bprintf(pip->efp,"%s: eigenwords=%u\n",
	        pip->progname,c) ;
	}

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (procmkeigen) */

static int procdbcheck(PROGINFO *pip,cchar *db,mode_t m) noex {
	int		rs = SR_OK ;
	int		dl ;
	cchar		*dp ;
	if ((dl = sfdirname(db,-1,&dp)) > 0) {
	    char	tbuf[MAXPATHLEN + 1] ;
	    if ((rs = mkpath1w(tbuf,dp,dl)) >= 0) {
	        rs = procdnamecheck(pip,tbuf,m) ;
	    }
	}
	return rs ;
}
/* end subroutine (procdbcheck) */

static int procdnamecheck(PROGINFO *pip,cchar *dname,mode_t m) noex {
	USTAT		sb ;
	int		rs ;

	if (pip == NULL) return SR_FAULT ;

	if ((rs = u_stat(dname,&sb)) >= 0) {
	    if (S_ISDIR(sb.st_mode)) {
	        const int	nrs = SR_NOENT ;
		const int	am = (X_OK|W_OK) ;
	        if ((rs = progids_permid(pip,&sb,am)) == nrs) {
	            rs = mkdirs(dname,m) ;
	        }
	    } else {
	        rs = SR_NOTDIR ;
	    }
	} /* end if (stat) */

	return rs ;
}
/* end subroutine (procdnamecheck) */

static int procfiles(PROGINFO *pip,ARGINFO *aip) noex {
	vecstr		*alp = &aip->args ;
	int		rs = SR_OK ;
	int		pan = 0 ;
	int		i ;
	int		c_tagref = 0 ;
	cchar		*cp ;
	cchar		*pn = pip->progname ;
	cchar		*fmt ;

/* process the arguments */

	for (i = 0 ; vecstr_get(alp,i,&cp) >= 0 ; i += 1) {
	    if (cp != NULL) {
	        pan += 1 ;
	        if ((rs = procfile(pip,cp)) >= 0) {
	            c_tagref += rs ;
		} else {

	            if (*cp == '-') cp = "*stdinput*" ;
	            if (pip->quietlevel <= 1) {
	                fmt = "%s: error processing file (%d)\n" ;
	                bprintf(pip->efp,fmt,pn,rs) ;
	                bprintf(pip->efp,"%s: file=%s\n",pn,cp) ;
	            }  /* end if (not quiet) */

	        } /* end if (error) */

	    }
	    if (rs < 0) break ;
	} /* end for (processing arguments) */

	return (rs >= 0) ? pan : rs ;
}
/* end subroutine (procfiles) */

static int procfile(PROGINFO *pip,cchar *fname) noex {
	SUBINFO		*sip = pip->sip ;
	KTAG		e ;
	KTAG_PARAMS	ka{} ;
	bfile		infile, *ifp = &infile ;
	int		rs = SR_OK ;
	int		recoff = 0 ;
	int		c = 0 ;

	if (fname == NULL) return SR_FAULT ;

	if (fname[0] == '\0') return SR_INVALID ;

	ka.minwlen = pip->minwordlen ;
	ka.f_eigen = pip->open.eigendb ;
	ka.f_nofile = pip->fl.optnofile ;

/* open the file that we are supposed to process */

	if (fname[0] == '-') fname = BFILE_STDIN ;

	if ((rs = bopen(ifp,fname,"r",0666)) >= 0) {
	    TXTINDEXMK_TAG	t ;
	    TAGINFO		ti ;
	    cint		llen = LINEBUFLEN ;
	    int			rlen ;
	    int			si ;
	    int			len ;
	    int			sl, cl ;
	    int			lo = 0 ;
	    int			f_bol = TRUE ;
	    int			f_eol ;
	    int			f_ent = FALSE ;
	    char		lbuf[LINEBUFLEN + 1] ;
	    cchar		*sp, *cp ;
	    cchar		*tp ;

/* go to it, read the file line by line */

	    while (rs >= 0) {

	        rs = breadln(ifp,(lbuf + lo),(llen - lo)) ;
	        rlen = rs ;
	        if (rs < 0) break ;

	        len = (lo + rlen) ;
	        if (len == 0) break ;

	        f_eol = FALSE ;
	        if (lbuf[len - 1] == '\n')  {
	            f_eol = TRUE ;
	            lbuf[--len] = '\0' ;
	        }
	        f_eol = f_eol || (rlen == 0) ;

	        sp = lbuf ;
	        sl = len ;

/* do this little ditty for wrap-around action later */

	        cl = 0 ;
	        if (! f_eol) {
	            if ((tp = strnrchr(sp,sl,' ')) != NULL) {
	                cp = (tp + 1) ;
	                cl = (sl - ((tp + 1) - sp)) ;
	                sl = (tp - sp) ;
	            }
	        }

	        if ((sl > 0) && (sp[0] != '#')) {

	            if (f_bol) {

	                if (sp[0] == '-') {
	                    rs = proccmd(pip,sp,sl) ;
	                } else if (! CHAR_ISWHITE(sp[0])) {
	                    si = taginfo_parse(&ti,sp,sl) ;
	                    if (f_ent) {
	                        c += 1 ;
	                        rs = ktag_mktag(&e,&t) ;
	                        if (rs >= 0) {
	                            rs = subinfo_addtags(sip,&t,1) ;
	                        }
	                        f_ent = FALSE ;
	                        ktag_finish(&e) ;
	                    } /* end if (entry) */

	                    if ((rs >= 0) && (si > 0)) {
	                        cchar	*sip = (sp+si) ;
	                        int	sil = (sl-si) ;
	                        rs = ktag_start(&e,&ka,&ti,sp,sip,sil) ;
	                        if (rs >= 0) {
	                            f_ent = TRUE ;
				}
	                    } /* end if */

	                } else {
	                    if (f_ent && (sl > 0)) {
	                        rs = ktag_procline(&e,sp,sl) ;
	                    }
	                } /* end if */

	            } else {

	                if (f_ent) {
	                    rs = ktag_add(&e,sp,sl) ;
	                }

	            } /* end if (entry start of add) */

	        } /* end if */

	        if (rs < 0) break ;

	        recoff += rlen ;
	        if (rlen == 0) break ;

	        lo = 0 ;
	        if (cl > 0) {
	            lo = cl ;
	            memmove(lbuf,cp,cl) ;
	            lbuf[lo++] = ' ' ;
	        }

	        f_bol = f_eol ;

	    } /* end while (reading lines) */

	    if ((rs >= 0) && f_ent) {
	        c += 1 ;
	        rs = ktag_mktag(&e,&t) ;

	        if (rs >= 0) {
	            rs = subinfo_addtags(sip,&t,1) ;
	        }
	        f_ent = FALSE ;
	        ktag_finish(&e) ;
	    } /* end if (entry) */

	    if (f_ent) {
	        f_ent = FALSE ;
	        ktag_finish(&e) ;
	    }

	    bclose(ifp) ;
	} /* end if (file-open) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (procfile) */

static int proccmd(PROGINFO *pip,cchar *sp,int sl) noex {
	SUBINFO		*sip = pip->sip ;
	int		rs = SR_OK ;

	if (sp[0] == '-') {
	int		cl ;
	cchar		*cp ;

/* skip the minus sign */

	sp += 1 ;
	sl -= 1 ;

/* get the command */

	if ((cl = nextfield(sp,sl,&cp)) > 0) {
            int		ci ;

	    sl -= ((cp + cl) - sp) ;
	    sp = (cp + cl) ;

	    if ((ci = matostr(mkcmds,2,cp,cl)) >= 0) {
	        switch (ci) {
	        case mkcmd_minwordlen:
	        case mkcmd_maxwordlen:
	        case mkcmd_nkeys:
	        case mkcmd_tablen:
	            if ((cl = nextfield(sp,sl,&cp)) > 0) {
			uint	v ;
	                rs = cfdecui(cp,cl,&v) ;
	                switch (ci) {
	                case mkcmd_minwordlen:
	                    sip->minwlen = v ;
	                    break ;
	                case mkcmd_maxwordlen:
	                    sip->maxwlen = v ;
	                    break ;
	                case mkcmd_nkeys:
	                    sip->maxkeys = v ;
	                    break ;
	                case mkcmd_tablen:
	                    sip->tablen = v ;
	                    break ;
	                } /* end switch */
	            } /* end if */
	            break ;
	        case mkcmd_eigenwords:
	            rs = proccmd_eigen(pip,sp,sl) ;
	            break ;
	        case mkcmd_sdn:
	            rs = loadsubstr(pip,&sip->sdn,sp,sl) ;
	            break ;
	        case mkcmd_sfn:
	            rs = loadsubstr(pip,&sip->sfn,sp,sl) ;
	            break ;
	        } /* end switch */
	    } else
	        rs = SR_BADFMT ;

	} /* end if (non-zero thing) */
	} else
	    rs = SR_BADFMT ;

	return rs ;
}
/* end subroutine (proccmd) */

static int proccmd_eigen(PROGINFO *pip,cchar *sp,int sl) noex {
	SUBINFO		*sip = pip->sip ;
	vecpstr		*slp ;
	int		rs = SR_OK ;
	int		cl ;
	cchar		*cp ;

	slp = &sip->eigenwords ;
	while ((cl = nextfield(sp,sl,&cp)) > 0) {
	    rs = vecpstr_adduniq(slp,cp,cl) ;
	    sl -= ((cp + cl) - sp) ;
	    sp = (cp + cl) ;
	    if (rs < 0) break ;
	} /* end while */

	return rs ;
}
/* end subroutine (proccmd_eigen) */

static int loadsubstr(PROGINFO *pip,cchar **rpp,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		cl ;
	cchar		*cp ;

	if (pip == NULL) return SR_FAULT ;

	if ((cl = nextfield(sp,sl,&cp)) > 0) {
	    cchar	*ccp ;
	    if (*rpp != NULL) {
	        uc_free(*rpp) ;
	        *rpp = NULL ;
	    }
	    if ((rs = uc_mallocstrw(cp,cl,&ccp)) >= 0) {
	        *rpp = ccp ;
	    }
	}

	return rs ;
}
/* end subroutine (loadsubstr) */

static int subinfo_start(SUBINFO *sip,PROGINFO *pip,cchar *dbname) noex {
	int		rs ;
	cchar		*cp ;

	memclear(sip) ;
	sip->pip = pip ;
	sip->tablen = pip->tablen ;
	sip->minwlen = pip->minwordlen ;
	sip->maxwlen = pip->maxwordlen ;

	if ((rs = uc_mallocstrw(dbname,-1,&cp)) >= 0) {
	    sip->dbname = cp ;
	    if ((rs = vecpstr_start(&sip->eigenwords,50,0,0)) >= 0) {
	        if (pip->sdn != NULL) {
	            rs = loadsubstr(pip,&sip->sdn,pip->sdn,-1) ;
	            sip->fl.sdn = (rs >= 0) ;
	            if (rs < 0) {
	                if (sip->fl.sdn && (sip->sdn != NULL)) {
	                    sip->fl.sdn = FALSE ;
	                    uc_free(sip->sdn) ;
	                    sip->sdn = NULL ;
	                }
	            }
	        }
	        if (rs < 0)
	            vecpstr_finish(&sip->eigenwords) ;
	    } /* end if (vecpstr-start) */
	    if (rs < 0) {
	        uc_free(sip->dbname) ;
	        sip->dbname = NULL ;
	    }
	} /* end if (memory-allocation) */

	return rs ;
}
/* end subroutine (subinfo_start) */

static int subinfo_finish(SUBINFO *sip) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	rs1 = subinfo_txtclose(sip) ;
	if (rs >= 0) rs = rs1 ;

	if (sip->fl.sfn && (sip->sfn != NULL)) {
	    sip->fl.sfn = FALSE ;
	    rs1 = uc_free(sip->sfn) ;
	    if (rs >= 0) rs = rs1 ;
	    sip->sfn = NULL ;
	}

	if (sip->fl.sdn && (sip->sdn != NULL)) {
	    sip->fl.sdn = FALSE ;
	    rs1 = uc_free(sip->sdn) ;
	    if (rs >= 0) rs = rs1 ;
	    sip->sdn = NULL ;
	}

	rs1 = vecpstr_finish(&sip->eigenwords) ;
	if (rs >= 0) rs = rs1 ;

	if (sip->dbname != NULL) {
	    rs1 = uc_free(sip->dbname) ;
	    if (rs >= 0) rs = rs1 ;
	    sip->dbname = NULL ;
	}

	return rs ;
}
/* end subroutine (subinfo_finish) */

static int subinfo_eigendefs(SUBINFO *sip) noex {
	PROGINFO	*pip = sip->pip ;
	VECPSTR		*esp = &sip->eigenwords ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;

	if ((rs = vecpstr_count(esp)) == 0) {
	    int		nkeys = pip->eigenwords ;
	    if (nkeys < 10) nkeys = 10 ;
	    if ((rs = progeigen_begin(pip)) >= 0) {
		PROGEIGEN_CUR	ecur ;
	        if ((rs = progeigen_curbegin(pip,&ecur)) >= 0) {
		    int		wl ;
		    cchar	*wp ;
		    while ((wl = progeigen_enum(pip,&ecur,&wp)) >= 0) {
			if (wl > 0) {
			    rs = vecpstr_add(esp,wp,wl) ;
			}
			if (rs < 0) break ;
	            } /* end while */
	            rs1 = progeigen_curend(pip,&ecur) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (progeigen-cur) */
		rs1 = progeigen_end(pip) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (progeigen-cur) */
	} /* end if (zero) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (subinfo_eigendefs) */

static int subinfo_txtopen(SUBINFO *sip) noex {
	PROGINFO	*pip = sip->pip ;
	TXTINDEXMK_PA	ta{} ;
	int		rs = SR_OK ;

	if (pip == NULL) return SR_FAULT ;

	ta.tablen = sip->tablen ;
	ta.minwlen = sip->minwlen ;
	ta.maxwlen = sip->maxwlen ;
	ta.sdn = sip->sdn ;
	ta.sfn = sip->sfn ;

	if (! sip->fl.txtopen) {
	    if (sip->dbname != NULL) {
		const mode_t	om = MKINV_INDPERM ;
		cint		of = O_CREAT ;
		cchar		*db = sip->dbname ;
	        if ((rs = txtindexmk_open(&sip->tm,&ta,db,of,om)) >= 0) {
	            sip->fl.txtopen = TRUE ;
		} else if (isNotPresent(rs)) {
		    cchar	*pn = pip->progname ;
		    cchar	*fmt = "%s: failed-open db=%s (%d)\n" ;
		    bprintf(pip->efp,fmt,pn,db,rs) ;
		}
	    } else {
	        rs = SR_FAULT ;
	    }
	}

	return rs ;
}
/* end subroutine (subinfo_txtopen) */

static int subinfo_txtclose(SUBINFO *sip) noex {
	PROGINFO	*pip = sip->pip ;
	int		rs = SR_OK ;
	int		rs1 ;

	if (pip == NULL) return SR_FAULT ;

	if (sip->fl.txtopen) {
	    sip->fl.txtopen = FALSE ;
	    rs1 = txtindexmk_close(&sip->tm) ;
	    if (rs >= 0) rs = rs1 ;
	}

	return rs ;
}
/* end subroutine (subinfo_txtclose) */

static int subinfo_addtags(SUBINFO *sip,TXTINDEXMK_TAG *tagp,int tagn) noex {
	PROGINFO	*pip = sip->pip ;
	int		rs = SR_OK ;

	if (pip == NULL) return SR_FAULT ;

	if (! sip->fl.txtopen) {
	    rs = subinfo_txtopen(sip) ;
	}

	if (rs >= 0) {
	    rs = txtindexmk_addtags(&sip->tm,tagp,tagn) ;
	}

	return rs ;
}
/* end subroutine (subinfo_addtags) */


static int ktag_start(KTAG *kop,KTAG_PARAMS *kap,TAGINFO *tip,
		cchar *fname,cchar *lp,int ll) noex {
	int		rs ;
	int		c = 0 ;
	cchar		*cp ;

	memclear(kop) ;
	kop->kap = kap ;
	kop->recoff = tip->recoff ;
	kop->reclen = tip->reclen ;

	if ((rs = uc_mallocstrw(fname,tip->fnamelen,&cp)) >= 0) {
	    cint	size = szof(KTAG_KEY) ;
	    cint	vopts = VECOBJ_OCOMPACT ;
	    kop->fname = cp ;
	    if ((rs = vecobj_start(&kop->keys,size,0,vopts)) >= 0) {
	        if ((rs = vecpstr_start(&kop->store,10,80,0)) >= 0) {
	            rs = ktag_procline(kop,lp,ll) ;
	            c = rs ;
	            if (rs < 0)
	                vecpstr_finish(&kop->store) ;
	        }
	        if (rs < 0)
	            vecobj_finish(&kop->keys) ;
	    } /* end if */
	    if (rs < 0) {
	        uc_free(kop->fname) ;
	        kop->fname = NULL ;
	    }
	} /* end if (memory-allocation) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (ktag_start) */

static int ktag_finish(KTAG *kop) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (kop->tkeys != NULL) {
	    rs1 = uc_free(kop->tkeys) ;
	    if (rs >= 0) rs = rs1 ;
	    kop->tkeys = NULL ;
	}

	rs1 = vecpstr_finish(&kop->store) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = vecobj_finish(&kop->keys) ;
	if (rs >= 0) rs = rs1 ;

	if (kop->fname != NULL) {
	    rs1 = uc_free(kop->fname) ;
	    if (rs >= 0) rs = rs1 ;
	    kop->fname = NULL ;
	}

	return rs ;
}
/* end subroutine (ktag_finish) */

static int ktag_add(KTAG *kop,cchar *lp,int ll) noex {
	return ktag_procline(kop,lp,ll) ;
}
/* end subroutine (ktag_add) */

static int ktag_mktag(KTAG *kop,TXTINDEXMK_TAG *tagp) noex {
	KTAG_PARAMS	*kap = kop->kap ;
	int		rs ;

	memclear(tagp) ;
	tagp->recoff = kop->recoff ;
	tagp->reclen = kop->reclen ;
	if (! kap->f_nofile) {
	    tagp->fname = kop->fname ;
	}

	KTAG_KEY	**va ;
	if ((rs = vecobj_getvec(&kop->keys,&va)) >= 0) {
	    TXTINDEXMK_KEY	*kea = NULL ;
	    int			size ;

	    tagp->nkeys = rs ;

	size = (tagp->nkeys * szof(TXTINDEXMK_KEY)) ;
	if ((rs = uc_malloc(size,&kea)) >= 0) {
	    kop->tkeys = kea ;		/* kea: save for us */
	    for (int i = 0 ; i < tagp->nkeys ; i += 1) {
	        kea[i] = *(va[i]) ;
	    } /* end for */
	    tagp->keys = kea ;		/* kea: store in the tag */
	    } /* end if (memory-allocation) */
	} /* end if (m-a) */

	return rs ;
}
/* end subroutine (ktag_mktag) */

static int ktag_procline(KTAG *kop,cchar *lp,int ll) noex {
	KTAG_PARAMS	*kap = kop->kap ;
	XWORDS		w ;
	int		rs = SR_OK ;
	int		i ;
	int		sl, cl ;
	int		c = 0 ;
	cchar		*sp, *cp ;

	while ((sl = nextfield(lp,ll,&sp)) > 0) {

	    if (sl >= kap->minwlen) {
	        if ((rs = xwords_start(&w,sp,sl)) >= 0) {

	            c += 1 ;
	            i = 0 ;
	            while ((rs >= 0) &&
	                ((cl = xwords_get(&w,i++,&cp)) > 0)) {

	                if (cl >= kap->minwlen) {
	                    rs = ktag_procword(kop,cp,cl) ;
	                }

	            } /* end while */

	            xwords_finish(&w) ;
	        } /* end if (xwords) */
	    } /* end if (meets min-key-length restrictions) */

	    ll -= ((sp + sl) - lp) ;
	    lp = (sp + sl) ;

	    if (rs < 0) break ;
	} /* end while */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (ktag_procline) */

static int ktag_procword(KTAG *kop,cchar *cp,int cl) noex {
	int		rs ;
	cchar		*sp ;

	if (cl > KEYBUFLEN)
	    cl = KEYBUFLEN ;

	if ((rs = vecpstr_store(&kop->store,cp,cl,&sp)) >= 0) {
	    KTAG_KEY	key ;
	    key.kp = sp ; /* the stored version of the string */
	    key.kl = cl ; /* both are the same length */
	    rs = vecobj_add(&kop->keys,&key) ;
	}

	return rs ;
}
/* end subroutine (ktag_procword) */


