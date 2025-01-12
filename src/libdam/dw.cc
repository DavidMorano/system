/* dw SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* directory watch */
/* version %I% last-modified %G% */

#define	CF_FNAMECMP	1		/* ? */

/* revision history:

	= 1998-07-01, David A­D­ Morano
	This subroutine was written for use in the 'rbbpost' daemon
	program.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This object is used to watch a directory path for additions
	or changes in the file objects under that path.  Subdirectories
	are allowed.  All files under the directory path (including
	those in subdirectories) are treated the same as an entry.

*******************************************************************************/

#include	<envstandards.h>	/* must be ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<bufsizevar.hh>
#include	<mallocxx.h>
#include	<vecobj.h>
#include	<vecstr.h>
#include	<fsdir.h>
#include	<sncpyx.h>
#include	<mkpathx.h>
#include	<strwcpy.h>
#include	<strdcpy.h>
#include	<localmisc.h>

#include	"dw.h"


/* local defines */

#define	IENTRY		struct dw_ientry

#ifndef	LINEBUFLEN
#ifdef	LINE_MAX
#define	LINEBUFLEN	MAX(LINE_MAX,2048)
#else
#define	LINEBUFLEN	2048
#endif
#endif

#define	BUFLEN		(2 * MAXPATHLEN)

#define	MAXOPENTIME	300		/* maximum FD cache time */
#define	MINCHECKTIME	2		/* minumun check interval (seconds) */
#define	MAXIDLETIME	240		/* maximum allowable idle time */


/* imported namespaces */


/* local typedefs */

extern "C" {
    typedef void (*dwcallback_f)(DW_ENT *,int,void *) noex ;
}


/* external subroutines */

extern "C" {
    int dw_callback(DW *,dwcallback_f,void *) noex ;
}


/* external variables */


/* forward references */

static int	dw_scanfull(DW *) noex ;
static int	dw_scansub(DW *,cchar *,time_t) noex ;
static int	dw_findi(DW *,cchar *,IENTRY **) noex ;

static int	ientry_start(IENTRY *,DW *,cchar *,USTAT *) noex ;
static int	ientry_finish(IENTRY *,DW *) noex ;

static int	entry_load(DW_ENT *,IENTRY *) noex ;

static int	fnamecmp(cvoid **,cvoid **) noex ;


/* local variables */

static bufsizevar		maxnamelen(getbufsize_mn) ;

constexpr int			rsn = SR_NOTFOUND ;


/* exported variables */


/* exported subroutines */

int dw_start(DW *dwp,cchar *dirname) noex {
	int		rs ;
	int		sz ;
	int		opts ;

	if (dwp == nullptr) return SR_FAULT ;
	if (dirname == nullptr) return SR_FAULT ;

	if (dirname[0] == '\0') return SR_INVALID ;

	memclear(dwp) ;
	dwp->fd = -1 ;
	dwp->count_new = 0 ;
	dwp->count_checkable = 0 ;
	dwp->checkint = DW_DEFCHECKTIME ;
	dwp->f.subdirs = false ;

/* initialize */

	sz = szof(DW_ENT) ;
	opts = (VECOBJ_OSTATIONARY | VECOBJ_OCONSERVE) ;
	if ((rs = vecobj_start(&dwp->e,sz,10,opts)) >= 0) {
	    custime	dt = getustime ;
	    cchar	*cp ;
	    dwp->mtime = 0 ;
	    dwp->opentime = 0 ;
	    dwp->checktime = dt ;
	    dwp->removetime = dt ;
	    if ((rs = uc_mallocstrw(dirname,-1,&cp)) >= 0) {
	        if ((rs-1) <= MAXPATHLEN) {
	            dwp->dirname = cp ;
	            rs = dw_scanfull(dwp) ;
	            if (rs == SR_NOENT) rs = SR_OK ;
	            dwp->magic = DW_MAGIC ;
	        } else {
	            rs = SR_TOOBIG ;
	        }
	        if (rs < 0)
	            uc_free(cp) ;
	    } /* end if (m-a) */
	    if (rs < 0)
	        vecobj_finish(&dwp->e) ;
	} /* end if (vecobj-start) */

	return rs ;
}
/* end subroutine (dw_start) */

int dw_finish(DW *dwp) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (dwp == nullptr) return SR_FAULT ;

	if (dwp->magic != DW_MAGIC) return SR_NOTOPEN ;

/* directory entries */

	void		*vp{} ;
	for (int i = 0 ; vecobj_get(&dwp->e,i,&vp) >= 0 ; i += 1) {
	    if (vp) {
		IENTRY	*iep = (IENTRY *) vp ;
	        rs1 = ientry_finish(iep,dwp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end for */
	{
	    rs1 = vecobj_finish(&dwp->e) ;
	    if (rs >= 0) rs = rs1 ;
	}
	if (dwp->dirname != nullptr) {
	    rs1 = uc_free(dwp->dirname) ;
	    if (rs >= 0) rs = rs1 ;
	    dwp->dirname = nullptr ;
	}

	if (dwp->fd >= 0) {
	    rs1 = u_close(dwp->fd) ;
	    if (rs >= 0) rs = rs1 ;
	    dwp->fd = -1 ;
	}

	if (dwp->f.subdirs) {
	    dwp->f.subdirs = false ;
	    rs1 = vecstr_finish(&dwp->subdirs) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if */

	dwp->fd = -1 ;
	dwp->magic = 0 ;
	return rs ;
}
/* end subroutine (dw_finish) */

int dw_curbegin(DW *dwp,DW_CUR *cp) noex {
	if (dwp == nullptr) return SR_FAULT ;

	if (dwp->magic != DW_MAGIC) return SR_NOTOPEN ;

	cp->i = -1 ;
	return SR_OK ;
}
/* end subroutine (dw_curbegin) */

int dw_curend(DW *dwp,DW_CUR *cp) noex {
	if (dwp == nullptr) return SR_FAULT ;

	if (dwp->magic != DW_MAGIC) return SR_NOTOPEN ;

	cp->i = -1 ;
	return SR_OK ;
}
/* end subroutine (dw_curend) */

int dw_enum(DW *dwp,DW_CUR *cp,DW_ENT *dep) noex {
	IENTRY		*iep = nullptr ;
	int		rs ;
	int		i ;
	int		nlen ;

	if (dwp == nullptr) return SR_FAULT ;

	if (dwp->magic != DW_MAGIC) return SR_NOTOPEN ;

/* get the next entry (any one) */

	if ((cp == nullptr) || (cp->i < 0)) {
	    i = 0 ;
	} else {
	    i = cp->i + 1 ;
	}

	void		*vp{} ;
	while ((rs = vecobj_get(&dwp->e,i,&vp)) >= 0) {
	    iep = (IENTRY *) vp ;
	    if (vp) break ;
	    i += 1 ;
	} /* end while */

	if ((rs >= 0) && iep) {
	    if (dep) {
	        rs = entry_load(dep,iep) ;
	    }
	    nlen = rs ;
	    if ((rs >= 0) && cp) {
	        cp->i = i ;
	    }
	} /* end if (ok) */

	if ((rs >= 0) && cp) {
	    cp->i = i ;
	}

	return (rs >= 0) ? nlen : rs ;
}
/* end subroutine (dw_enum) */

int dw_del(DW *dwp,DW_CUR *cp) noex {
	int		rs ;
	int		rs1 ;

	if (dwp == nullptr) return SR_FAULT ;
	if (cp == nullptr) return SR_FAULT ;

	if (dwp->magic != DW_MAGIC) return SR_NOTOPEN ;

	void		*vp{} ;
	if ((rs = vecobj_get(&dwp->e,cp->i,&vp)) >= 0) {
	    if (vp) {
		IENTRY	*iep = (IENTRY *) vp ;
		{
	            rs1 = ientry_finish(iep,dwp) ;
		    if (rs >= 0) rs = rs1 ;
		}
		{
	            rs1 = vecobj_del(&dwp->e,cp->i) ;
		    if (rs >= 0) rs = rs1 ;
		}
	    }
	} /* end if */

	return rs ;
}
/* end subroutine (dw_del) */

int dw_find(DW *dwp,cchar *name,DW_ENT *dep) noex {
	IENTRY		ie{} ;
	IENTRY		*iep = nullptr ;
	int		rs ;
	int		i = 0 ;

	if (dwp == nullptr) return SR_FAULT ;
	if (name == nullptr) return SR_FAULT ;

	if (dwp->magic != DW_MAGIC) return SR_NOTOPEN ;

	if (name[0] == '\0') return SR_NOTFOUND ;

	void		*vp{} ;
#if	CF_FNAMECMP
	ie.name = (char *) name ;
	if ((rs = vecobj_search(&dwp->e,&ie,fnamecmp,&vp)) >= 0) {
	    iep = (IENTRY *) vp ;
	}
#else /* CF_FNAMECMP */
	for (i = 0 ; (rs = vecobj_get(&dwp->e,i,&vp)) >= 0 ; i += 1) {
	    if (vp) {
		iep = (IENTRY *) vp ;
	        if (strcmp(name,iep->name) == 0) break ;

	    }
	} /* end for (looping through entries) */
#endif /* CF_FNAMECMP */

	if ((rs >= 0) && dep && iep) {
	    rs = entry_load(dep,iep) ;
	}

	return (rs >= 0) ? i : rs ;
}
/* end subroutine (dw_find) */

/* enumerate those entries that are "checkable" */
int dw_enumcheckable(DW *dwp,DW_CUR *cp,DW_ENT *dep) noex {
	IENTRY		*iep = nullptr ;
	int		rs ;
	int		i ;

	if (dwp == nullptr) return SR_FAULT ;

	if (dwp->magic != DW_MAGIC) return SR_NOTOPEN ;

/* get the next entry with a checkable file */

	if ((cp == nullptr) || (cp->i < 0)) {
	    i = 0 ;
	} else {
	    i = cp->i + 1 ;
	}

	void		*vp{} ;
	while ((rs = vecobj_get(&dwp->e,i,&vp)) >= 0) {
	    if (vp) {
		iep = (IENTRY *) vp ;
		if (iep->state == DW_SCHECK) break ;
	    }
	    i += 1 ;
	} /* end while */

	if (rs >= 0) {
	    if (dep && iep) {
	        rs = entry_load(dep,iep) ;
	    }
	    if ((rs >= 0) && cp) {
	        cp->i = i ;
	    }
	} /* end if */

	return (rs >= 0) ? i : rs ;
}
/* end subroutine (dw_enumcheckable) */

/* check if the directory (and any subdirectories) has changed */
int dw_check(DW *dwp,time_t daytime) noex {
	USTAT		sb ;
	int		rs = SR_OK ;
	int		i ;
	int		n = 0 ;

	if (dwp == nullptr) return SR_FAULT ;

	if (dwp->magic != DW_MAGIC) return SR_NOTOPEN ;

	if (daytime <= 0) daytime = getustime ;

/* should we even check? */

	if ((daytime - dwp->checktime) <= MINCHECKTIME)
	    goto ret0 ;

/* perform the directory time check */

	if (dwp->fd < 0) {

	    rs = u_open(dwp->dirname,O_RDONLY,0666) ;
	    dwp->fd = rs ;
	    if (rs >= 0) {
	        dwp->opentime = daytime ;
	        rs = uc_closeonexec(dwp->fd,true) ;
	    }

	} /* end if (opened FD for caching) */
	if (rs < 0) goto ret0 ;

	if ((rs = u_fstat(dwp->fd,&sb)) < 0) {
	    u_close(dwp->fd) ;
	    dwp->fd = -1 ;
	    goto done ;
	}

	if ((sb.st_mtime > dwp->mtime) ||
	    ((daytime - dwp->checktime) > MAXIDLETIME)) {

	    dwp->mtime = sb.st_mtime ;
	    rs = dw_scansub(dwp,"",daytime) ;
	    if (rs >= 0) n = rs ;

	} else if ((daytime - dwp->opentime) > MAXOPENTIME) {

	    u_close(dwp->fd) ;
	    dwp->fd = -1 ;

	} /* end if */

/* OK, look through the subdirectories and see if any of them need scanning */

	if (dwp->f.subdirs) {
	    cchar	*dnp ;
	    char	dnamebuf[MAXPATHLEN + 1], *dbp ;


	    dbp = dnamebuf ;
	    dbp = strwcpy(dbp,dwp->dirname,-1) ;

	    *dbp++ = '/' ;
	    for (i = 0 ; vecstr_get(&dwp->subdirs,i,&dnp) >= 0 ; i += 1) {
	        if (dnp != nullptr) {

	        {
	            int	rl = (MAXPATHLEN - (dbp-dnamebuf)) ;
	            strdcpy1(dbp,rl,dnp) ;
	        }

	        if ((u_stat(dnamebuf,&sb) >= 0) &&
	            ((sb.st_mtime > dwp->checktime) ||
	            ((daytime - dwp->checktime) > MAXIDLETIME))) {

	            rs = dw_scansub(dwp,dnp,daytime) ;
	            if (rs >= 0) n += 1 ;

	        } /* end if (scanning subdirectory) */

		}
	        if (rs < 0) break ;
	    } /* end for */

	} /* end if (subdirectories) */

/* OK, now check all files that are 'NEW' and see if they are older! */

	if ((rs >= 0) && (dwp->count_new > 0)) {
	    char	dnamebuf[MAXPATHLEN + 1], *dbp ;

	    dbp = dnamebuf ;
	    dbp = strwcpy(dbp,dwp->dirname,-1) ;

	    *dbp++ = '/' ;
	    void	*vp{} ;
	    for (i = 0 ; vecobj_get(&dwp->e,i,&vp) >= 0 ; i += 1) {
	        if (vp) {
		    IENTRY	*iep = (IENTRY *) vp ;
	            if (iep->state == DW_SNEW) {
	                if ((daytime - iep->itime) > (dwp->checkint / 4)) {
	                    {
	                        cint	rl = (MAXPATHLEN - (dbp-dnamebuf)) ;
	                        strdcpy1(dbp,rl,iep->name) ;
	                    }
	                    if ((u_stat(dnamebuf,&sb) >= 0) &&
	                        ((daytime - sb.st_mtime) > dwp->checkint)) {

	                        iep->state = DW_SCHECK ;
	                        dwp->count_new -= 1 ;
	                        dwp->count_checkable += 1 ;
	                        n += 1 ;

	                    } /* end if */
	                } /* end if (checkint) */
		    } /* end if (equal) */
		} /* end if (non-null) */
	    } /* end for */

	} /* end if */

/* OK, we are on a roll now !, check for files that have been removed ! */

	if ((rs >= 0) && ((daytime - dwp->removetime) > MAXIDLETIME) &&
	    (vecobj_count(&dwp->e) > 0)) {

	    char	dnamebuf[MAXPATHLEN + 1], *dbp ;

	    dwp->removetime = daytime ;
	    dbp = dnamebuf ;
	    dbp = strwcpy(dbp,dwp->dirname,-1) ;

	    *dbp++ = '/' ;
	    void	*vp{} ;
	    for (i = 0 ; vecobj_get(&dwp->e,i,&vp) >= 0 ; i += 1) {
		if (vp) {
		    IENTRY	*iep = (IENTRY *) vp ;
	            if ((daytime - iep->itime) >= MAXIDLETIME) {
	                iep->itime = daytime ;
	                {
	                    cint	rl = (MAXPATHLEN - (dbp-dnamebuf)) ;
	                    strdcpy1(dbp,rl,iep->name) ;
	                }
	                if (u_stat(dnamebuf,&sb) < 0) {
	                    ientry_finish(iep,dwp) ;
	                    vecobj_del(&dwp->e,i--) ;
	                } /* end if (could not 'stat') */
		    } /* end if (got time-out) */
		} /* end if (non-null) */
	    } /* end for */

	} /* end if (checking for removed files) */

done:
	dwp->checktime = daytime ;

ret0:

	return (rs >= 0) ? n : rs ;
}
/* end subroutine (dw_check) */

int dw_callback(DW *op,dwcallback_f func,void *argp) noex {
	if (op == nullptr) return SR_FAULT ;

	if (op->magic != DW_MAGIC) return SR_NOTOPEN ;

	op->callback = func ;
	op->argp = argp ;
	return SR_OK ;
}
/* end subroutine (dw_callback) */

extern int dw_state(DW *op,int i,int state) noex {
	int		rs ;
	int		state_prev = 0 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != DW_MAGIC) return SR_NOTOPEN ;

	if (state < 0) return SR_INVALID ;

	void		*vp{} ;
	if ((rs = vecobj_get(&op->e,i,&vp)) >= 0) {
	    IENTRY	*iep = (IENTRY *) vp ;
	    state_prev = iep->state ;
	    iep->state = state ;
	}

	return (rs >= 0) ? state_prev : rs ;
}
/* end subroutine (dw_state) */


/* private subroutines */

static int dw_scanfull(DW *dwp) noex {
	USTAT		sb ;
	IENTRY		ie, *iep ;
	fsdir		d ;
	fsdir_ent	ds ;
	time_t		daytime = getustime ;
	int		rs ;
	int		rs1 ;
	int		n = 0 ;
	char		dnamebuf[MAXPATHLEN + 1], *dbp ;

	if (dwp->dirname == nullptr) return SR_FAULT ;

	if (dwp->dirname[0] == '\0') return SR_INVALID ;

/* "do" the outer directory */

	if (char *nbuf{} ; (rs = malloc_mn(&nbuf)) >= 0) {
	    cint	nlen = 0 ;
	if ((rs = fsdir_open(&d,dwp->dirname)) >= 0) {
	    dbp = dnamebuf ;
	    dbp = strwcpy(dbp,dwp->dirname,-1) ;

	    *dbp++ = '/' ;
	    while ((rs = fsdir_read(&d,&ds,nbuf,nlen)) > 0) {
	    cint	nl = rs ;
	        if (ds.name[0] == '.') continue ;

	        {
	            int	rl = (MAXPATHLEN - (dbp-dnamebuf)) ;
	            strdcpy1(dbp,rl,ds.name) ;
	        }

	        if (u_stat(dnamebuf,&sb) < 0) continue ;

	        if (S_ISDIR(sb.st_mode)) {

	            if (! dwp->f.subdirs) {
			cint	vo = VECSTR_OCOMPACT ;
	                rs = vecstr_start(&dwp->subdirs,10,vo) ;
	                dwp->f.subdirs = (rs >= 0) ;
	            }

	            if (rs >= 0) {
			vecstr	*slp = &dwp->subdirs ;
	                if (vecstr_find(slp,dbp) == rsn) {
	                    rs = vecstr_add(slp,dbp,nl) ;
			}
	            }

	        } else if (dw_findi(dwp,dbp,&iep) < 0) {

	            dwp->count_new += 1 ;
	            ientry_start(&ie,dwp,dbp,&sb) ;

	            rs = vecobj_add(&dwp->e,&ie) ;

	        } else if ((iep->state == DW_SNEW) &&
	            ((daytime - sb.st_mtime) > dwp->checkint)) {

	            iep->state = DW_SCHECK ;
	            dwp->count_new -= 1 ;
	            dwp->count_checkable += 1 ;
	            n += 1 ;

	        } /* end if */

	    } /* end while (reading directory entries) */

	    rs1 = fsdir_close(&d) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (outer directory) */
	    rs1 = uc_free(nbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */

/* do the subdirectories */

	if ((rs >= 0) && dwp->f.subdirs) {
	    cchar	*dnp ;
	    for (int i = 0 ; vecstr_get(&dwp->subdirs,i,&dnp) >= 0 ; i += 1) {
	        if (dnp) {
	            rs = dw_scansub(dwp,dnp,daytime) ;
	            n += rs ;
	        }
	        if (rs < 0) break ;
	    } /* end for */
	} /* end if (subdirectories) */

	return (rs >= 0) ? n : rs ;
}
/* end subroutine (dw_scanfull) */

static int dw_scansub(DW *dwp,cchar *subdname,time_t daytime) noex {
	USTAT		sb ;
	IENTRY		ie, *iep ;
	fsdir		d ;
	fsdir_ent	ds ;
	int		rs ;
	int		rs1 ;
	int		dlen ;
	int		n = 0 ;
	char		dnamebuf[MAXPATHLEN + 1], *dbp ;
	char		*sdbp ;

	if (dwp->dirname == nullptr) return SR_FAULT ;

	if (dwp->dirname[0] == '\0') return SR_INVALID ;

/* put it together */

	dbp = dnamebuf ;
	dbp = strwcpy(dbp,dwp->dirname,-1) ;

	*dbp++ = '/' ;
	{
	    sdbp = dbp ;
	    if ((subdname != nullptr) && (subdname[0] != '\0')) {
	        sdbp = strwcpy(sdbp,subdname,-1) ;
	        *sdbp++ = '/' ;
	    }
	    *sdbp = '\0' ;
	}

/* "do" the subdirectory */

	if (char *nbuf{} ; (rs = malloc_mn(&nbuf)) >= 0) {
	    cint	nlen = 0 ;
	if ((rs = fsdir_open(&d,dnamebuf)) >= 0) {
	    cint	rsn = SR_NOENT ;

	    while ((rs = fsdir_read(&d,&ds,nbuf,nlen)) > 0) {
		cint	nl = rs ;
	        if (ds.name[0] == '.') continue ;

	        dlen = strwcpy(sdbp,ds.name,nl) - dbp ;

	        if (u_stat(dnamebuf,&sb) < 0) continue ;

	        if (S_ISDIR(sb.st_mode)) {

	            if (! dwp->f.subdirs) {
			cint	vo = VECSTR_OCOMPACT ;
	                rs = vecstr_start(&dwp->subdirs,10,vo) ;
	                dwp->f.subdirs = (rs >= 0) ;
	            }

	            if (rs >= 0) {
			vecstr	*slp = &dwp->subdirs ;
	                if ((rs = vecstr_find(slp,dbp)) == rsn) {
	                    rs = vecstr_add(slp,dbp,dlen) ;
			}
	            }

	        } else if (dw_findi(dwp,dbp,&iep) == rsn) {

	            dwp->count_new += 1 ;
	            ientry_start(&ie,dwp,dbp,&sb) ;

	            rs = vecobj_add(&dwp->e,&ie) ;

	        } else if (iep->state == DW_SNEW) {
	            iep->itime = daytime ;
	            if ((daytime - sb.st_mtime) > dwp->checkint) {
	                iep->state = DW_SCHECK ;
	                dwp->count_new -= 1 ;
	                dwp->count_checkable += 1 ;
	                n += 1 ;
	            } /* end if */
	        } else {
	            iep->itime = daytime ;
	        }

	    } /* end while (reading directory entries) */

	    rs1 = fsdir_close(&d) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (fsdir) */
	    rs1 = uc_free(nbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */

	return (rs >= 0) ? n : rs ;
}
/* end subroutine (dw_scansub) */

#ifdef	COMMENT

static int dw_finddelete(DW *dwp,cchar *name) noex {
	IENTRY		ie, *iep ;
	int		rs ;
	int		i ;

	ie.name = name ;
	rs = vecobj_search(&dwp->e,&ie,fnamecmp,&iep) ;
	i = rs ;
	if (rs >= 0)
	    rs = vecobj_del(&dwp->e,i) ;

	return (rs >= 0) ? i : rs ;
}
/* end subroutine (dw_finddelete) */

#endif /* COMMENT */

static int dw_findi(DW *dwp,cchar *name,IENTRY **iepp) noex {
	IENTRY		ie ;
	int		rs ;

	ie.name = (char *) name ;
	void		*vp{} ;
	if ((rs = vecobj_search(&dwp->e,&ie,fnamecmp,&vp)) >= 0) {
	    IENTRY	*iep = (IENTRY *) vp ;
	    *iepp = iep ;
	}

	return rs ;
}
/* end subroutine (dw_findi) */

/* initialize an entry */
static int ientry_start(IENTRY *iep,DW *dwp,cchar *name,USTAT *sbp) noex {
	USTAT		sb ;
	int		rs ;
	cchar		*cp ;

	iep->state = DW_SNEW ;
	if ((rs = uc_mallocstrw(name,-1,&cp)) >= 0) {
	    iep->name = cp ;

/* do we need to get some status on the file? */

	    if (sbp == nullptr) {
	        char	dnamebuf[MAXPATHLEN + 1] ;

	        if ((rs = mkpath2(dnamebuf,dwp->dirname,name)) >= 0) {
	            rs = u_stat(dnamebuf,&sb) ;
	            if (rs >= 0) sbp = &sb ;
	        }

	    } /* end if */

	    iep->itime = ((sbp != nullptr) ? sbp->st_mtime : 0) ;

	} /* end if */

	return rs ;
}
/* end subroutine (ientry_start) */

static int ientry_finish(IENTRY *iep,DW *dwp) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (iep->state == DW_SNEW) {
	    dwp->count_new -= 1 ;
	} else if (iep->state == DW_SCHECK) {
	    dwp->count_checkable -= 1 ;
	}

	if (iep->name != nullptr) {
	    rs1 = uc_free(iep->name) ;
	    if (rs >= 0) rs = rs1 ;
	    iep->name = nullptr ;
	}

	return rs ;
}
/* end subroutine (ientry_finish) */

static int entry_load(DW_ENT *dep,IENTRY *iep) noex {
	int		rs = SR_FAULT ;
	if (dep) {
	    dep->itime = iep->itime ;
	    dep->mtime = iep->mtime ;
	    dep->fsize = iep->fsize ;
	    dep->state = iep->state ;
	    rs = sncpy1(dep->name,MAXPATHLEN,iep->name) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (entry_load) */

static int fnamecmp(cvoid **v1pp,cvoid **v2pp) noex {
	IENTRY		**e1pp = (IENTRY **) v1pp ;
	IENTRY		**e2pp = (IENTRY **) v2pp ;
	int		rc = 0 ;
	{
	    IENTRY	*e1p = *e1pp ;
	    IENTRY	*e2p = *e2pp ;
	    if (e1p || e2p) {
	        if (e1p) {
	            if (e2p) {
	                rc = strcmp(e1p->name,e2p->name) ;
	            } else {
	                rc = -1 ;
		    }
	        } else {
	            rc = 1 ;
	        }
	    }
	} /* end block */
	return rc ;
}
/* end subroutine (fnamecmp) */


