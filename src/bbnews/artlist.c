/* artlist */

/* article list handling */


#define	CF_DEBUGS	0		/* compile-time debugging */


/* revision history:

	= 1995-05-01, David A�D� Morano
        This code module was completely rewritten to replace any original
        garbage that was here before, if any.

	= 1998-11-22, David A�D� Morano
        I did some clean-up.

	= 2017-10-24, David A�D� Morano
	Some small refactoring.

*/

/* Copyright � 1995,1998,2017 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	This object maintains a list of articles.


*******************************************************************************/


#include	<envstandards.h>

#include	<sys/types.h>
#include	<sys/stat.h>
#include	<sys/timeb.h>
#include	<unistd.h>
#include	<time.h>
#include	<stdlib.h>
#include	<string.h>

#include	<usystem.h>
#include	<vechand.h>
#include	<bfile.h>
#include	<mailmsg.h>
#include	<mailmsghdrs.h>
#include	<char.h>
#include	<dater.h>
#include	<localmisc.h>

#include	"artlist.h"


/* local defines */

#ifndef	ARTLIST_NET
#define	ARTLIST_NET	10
#endif

#ifndef	TIMEBUFLEN
#define	TIMEBUFLEN	80
#endif


/* typedefs */

typedef int	(*sortcmp_t)(const void *,const void *) ;


/* external subroutines */

extern int	snwcpycompact(char *,int,cchar *,int) ;
extern int	sfbracketval(cchar *,int,cchar **) ;
extern int	cfdeci(const char *,int,int *) ;

extern int	mailmsg_loadfile(MAILMSG *,bfile *) ;
extern int	mailmsg_envtimes(MAILMSG *,DATER *,time_t *,int) ;

extern char	*strnchr(const char *,int,int) ;


/* external variables */


/* local structures */


/* forward references */

static int	cmpartforward(), cmpartreverse() ;
static int	cmpaf(), cmpar() ;
static int	cmppf(), cmppr() ;
static int	cmpcf(), cmpcr() ;

static int	entry_start(ARTLIST_ENT *,DATER *,cchar *,cchar *) ;
static int	entry_finish(ARTLIST_ENT *) ;
static int	entry_load(ARTLIST_ENT *,DATER *,cchar *) ;

static int	timecmp(time_t *,time_t *) ;


/* local variables */


/* exported subroutines */


int artlist_start(ARTLIST *alp,struct timeb *nowp,cchar *zname)
{
	int		rs ;

	if (alp == NULL) return SR_FAULT ;
	if (zname == NULL) return SR_FAULT ;

	memset(alp,0,sizeof(ARTLIST)) ;

	if (nowp != NULL) {
	    int size = sizeof(struct timeb) ;
	    memcpy(&alp->now,nowp,size) ;
	}

	if ((rs = dater_start(&alp->mdate,nowp,zname,-1)) >= 0) {
	    const int	vo = VECHAND_OSORTED ;
	    if ((rs = vechand_start(&alp->arts,100,vo)) >= 0) {
	        alp->magic = ARTLIST_MAGIC ;
	    }
	    if (rs < 0)
	        dater_finish(&alp->mdate) ;
	} /* end if */

	return rs ;
}
/* end subroutine (artlist_start) */


/* free up this object */
int artlist_finish(ARTLIST *alp)
{
	ARTLIST_ENT	*ep ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		i ;

	if (alp == NULL) return SR_FAULT ;
	if (alp->magic != ARTLIST_MAGIC) return SR_NOTOPEN ;

	for (i = 0 ; vechand_get(&alp->arts,i,&ep) >= 0 ; i += 1) {
	    if (ep != NULL) {
	        rs1 = entry_finish(ep) ;
	        if (rs >= 0) rs = rs1 ;
	        rs1 = uc_free(ep) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end for */

	rs1 = vechand_finish(&alp->arts) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = dater_finish(&alp->mdate) ;
	if (rs >= 0) rs = rs1 ;

	alp->magic = 0 ;
	return rs ;
}
/* end subroutine (artlist_finish) */


/* add another entry to this object */
int artlist_add(ARTLIST *alp,cchar *ngdir,cchar *name)
{
	ARTLIST_ENT	*aep ;
	int		rs ;
	int		size ;

	if (alp == NULL) return SR_FAULT ;
	if (ngdir == NULL) return SR_FAULT ;
	if (name == NULL) return SR_FAULT ;

	if (alp->magic != ARTLIST_MAGIC) return SR_NOTOPEN ;
	if (ngdir[0] == '\0') return SR_INVALID ;
	if (name[0] == '\0') return SR_INVALID ;

#if	CF_DEBUGS
	debugprintf("artlist_add: ent name=%s\n",name) ;
#endif

	size = sizeof(ARTLIST_ENT) ;
	if ((rs = uc_malloc(size,&aep)) >= 0) {
	    if ((rs = entry_start(aep,&alp->mdate,ngdir,name)) >= 0) {
	        rs = vechand_add(&alp->arts,aep) ;
	        if (rs < 0)
	            entry_finish(aep) ;
	    }
	    if (rs < 0)
	        uc_free(aep) ;
	} /* end if (m-a) */

	return rs ;
}
/* end subroutine (artlist_add) */


/* sort the entries with given sorting mode and direction */
int artlist_sort(ARTLIST *alp,int sortmode,int f_reverse)
{
	int		rs ;
	int		(*cmpfunc)(const void *,const void *) ;

	if (alp == NULL) return SR_FAULT ;

	if (alp->magic != ARTLIST_MAGIC) return SR_NOTOPEN ;

	switch (sortmode) {
/* modify time on file */
	default:
	case 0:
	    cmpfunc = (! f_reverse) ? cmpartforward : cmpartreverse ;
	    break ;
/* arrive */
	case 1:
	    cmpfunc = (! f_reverse) ? cmpaf : cmpar ;
	    break ;
/* post */
	case 2:
	    cmpfunc = (! f_reverse) ? cmppf : cmppr ;
	    break ;
/* compose */
	case 3:
	    cmpfunc = (! f_reverse) ? cmpcf : cmpcr ;
	    break ;
	} /* end switch */

	rs = vechand_sort(&alp->arts,cmpfunc) ;

	return rs ;
}
/* end subroutine (artlist_sort) */


/* get the basic information from the given entry */
int artlist_get(ARTLIST *alp,int i,cchar **ngdpp,cchar **npp,time_t *mp)
{
	ARTLIST_ENT	*ep ;
	int		rs ;

	if (alp == NULL) return SR_FAULT ;

	if (alp->magic != ARTLIST_MAGIC) return SR_NOTOPEN ;

#if	CF_DEBUGS
	debugprintf("artlist_get: ent, i=%d\n",i) ;
#endif

	if (ngdpp != NULL)
	    *ngdpp = NULL ;

	if (npp != NULL)
	    *npp = NULL ;

#if	CF_DEBUGS
	debugprintf("artlist_get: 2\n") ;
#endif

	if (mp != NULL)
	    *mp = 0 ;

#if	CF_DEBUGS
	debugprintf("artlist_get: i=%d\n",i) ;
#endif

	if ((rs = vechand_get(&alp->arts,i,&ep)) >= 0) {

#if	CF_DEBUGS
	    debugprintf("artlist_get: got one\n") ;
#endif

	    if (ngdpp != NULL)
	        *ngdpp = (const char *) ep->ngdir ;

	    if (npp != NULL)
	        *npp = (const char *) ep->name ;

	    if (mp != NULL)
	        *mp = ep->mtime ;

	} /* end if (vechand-get) */

#if	CF_DEBUGS
	debugprintf("artlist_get: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (artlist_get) */


/* get the whole entry */
int artlist_getentry(ARTLIST *alp,int i,ARTLIST_ENT **epp)
{
	int		rs ;

	if (alp == NULL) return SR_FAULT ;

	if (alp->magic != ARTLIST_MAGIC) return SR_NOTOPEN ;

#if	CF_DEBUGS
	debugprintf("artlist_getentry: ent i=%d\n",i) ;
#endif

	if (epp == NULL)
	    return SR_INVALID ;

	rs = vechand_get(&alp->arts,i,epp) ;

#if	CF_DEBUGS
	debugprintf("artlist_getentry: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (artlist_getentry) */


/* private subroutines */


/* initialize an article entry */
static int entry_start(ARTLIST_ENT *ep,DATER *dp,cchar *ngdir,cchar *name)
{
	struct ustat	sb ;
	int		rs ;
	const char	*cp ;

	if (ep == NULL) return SR_FAULT ;
	if (ngdir == NULL) return SR_FAULT ;
	if (name == NULL) return SR_FAULT ;

	if (ngdir[0] == '\0') return SR_INVALID ;
	if (name[0] == '\0') return SR_INVALID ;

	memset(ep,0,sizeof(ARTLIST_ENT)) ;

#if	CF_DEBUGS
	debugprintf("artlist/entry_start: ent\n") ;
#endif

	if ((rs = u_stat(name,&sb)) >= 0) {
	    if (S_ISREG(sb.st_mode)) {
	        ep->mtime = sb.st_mtime ;
	        ep->ngdir = ngdir ;
	        if ((rs = uc_mallocstrw(name,-1,&cp)) >= 0) {
	            ep->name = cp ;
	            if ((rs = entry_load(ep,dp,name)) >= 0) {
	                ep->magic = ARTLIST_CURMAGIC ;
	            }
	            if (rs < 0) {
	                uc_free(ep->name) ;
	                ep->name = NULL ;
	            }
	        } /* end if (memory-allocation) */
	    } else {
	        rs = SR_ISDIR ;
	    }
	} /* end if (stat) */

	return rs ;
}
/* end subroutine (entry_start) */


static int entry_finish(ARTLIST_ENT *ep)
{
	int		rs = SR_OK ;
	int		rs1 ;

	if (ep->name != NULL) {
	    rs1 = uc_free(ep->name) ;
	    if (rs >= 0) rs = rs1 ;
	    ep->name = NULL ;
	}

	if (ep->subject != NULL) {
	    rs1 = uc_free(ep->subject) ;
	    if (rs >= 0) rs = rs1 ;
	    ep->subject = NULL ;
	}

	if (ep->replyto != NULL) {
	    rs1 = uc_free(ep->replyto) ;
	    if (rs >= 0) rs = rs1 ;
	    ep->replyto = NULL ;
	}

	if (ep->from != NULL) {
	    rs1 = uc_free(ep->from) ;
	    if (rs >= 0) rs = rs1 ;
	    ep->from = NULL ;
	}

	if (ep->newsgroups != NULL) {
	    rs1 = uc_free(ep->newsgroups) ;
	    if (rs >= 0) rs = rs1 ;
	    ep->newsgroups = NULL ;
	}

	if (ep->messageid != NULL) {
	    rs1 = uc_free(ep->messageid) ;
	    if (rs >= 0) rs = rs1 ;
	    ep->messageid = NULL ;
	}

	if (ep->articleid != NULL) {
	    rs1 = uc_free(ep->articleid) ;
	    if (rs >= 0) rs = rs1 ;
	    ep->articleid = NULL ;
	}

	return rs ;
}
/* end subroutine (entry_finish) */


static int entry_load(ARTLIST_ENT *ep,DATER *dp,cchar *name)
{
	bfile		afile ;
	int		rs ;
	int		rs1 ;

	if ((rs = bopen(&afile,name,"r",0666)) >= 0) {
	    MAILMSG	am ;

#if	CF_DEBUGS
	    debugprintf("artlist/entry_start: mailmsg_start()\n") ;
#endif

	    if ((rs = mailmsg_start(&am)) >= 0) {
	        if ((rs = mailmsg_loadfile(&am,&afile)) >= 0) {
	            time_t	ta[ARTLIST_NET] ;
	            int		hl ;
	            int		cl ;
	            int		n ;
	            int		v ;
	            cchar	*hn ;
	            cchar	*hp ;
	            cchar	*cp ;

/* get the envelope times (post & arrive) if there are any */

#if	CF_DEBUGS
	            debugprintf("artlist/entry_start: mailmsg_envtimes()\n") ;
#endif

	            ep->ptime = ep->atime = 0 ;
	            n = mailmsg_envtimes(&am,dp,ta,ARTLIST_NET) ;

#if	CF_DEBUGS
	            debugprintf("artlist/entry_start: "
	                "mailmsg_envtimes() n=%d\n",n) ;
#endif

	            if (n > 1) {
	                sortcmp_t	scmp = (sortcmp_t) timecmp ;
	                size_t		sz = n ;
	                const int	esize = sizeof(time_t) ;

	                qsort(ta,sz,esize,scmp) ;
	                ep->ptime = ta[0] ;
	                ep->atime = ta[n - 1] ;

	            } else if (n == 1) {
	                ep->ptime = ep->atime = ta[0] ;

	            } else if (n == 0) {
	                ep->ptime = ep->atime = ep->mtime ;
	            }

/* get the message (composition) time (if there is one) */

	            if (rs >= 0) {
	                hn = HN_DATE ;
	                if ((hl = mailmsg_hdrival(&am,hn,0,&hp)) >= 0) {
	                    if ((rs = dater_setmsg(dp,hp,hl)) >= 0) {
	                        time_t	t ;
	                        dater_gettime(dp,&t) ;
	                        ep->ctime = t ;
	                    }
	                } /* end if (message date) */
	            }

/* get the message id */

	            if (rs >= 0) {
	                hn = HN_MESSAGEID ;
	                if ((hl = mailmsg_hdrival(&am,hn,0,&hp)) >= 0) {
	                    if ((cl = sfbracketval(hp,hl,&cp)) > 0) {
	                        cchar	*ccp ;
	                        if ((rs = uc_mallocstrw(cp,cl,&ccp)) >= 0) {
	                            ep->messageid = ccp ;
	                        }
	                    }
	                } /* end if (message-id) */
	            }

/* get the article id */

	            if (rs >= 0) {
	                hn = HN_ARTICLEID ;
	                if ((hl = mailmsg_hdrival(&am,hn,0,&hp)) >= 0) {
	                    if ((cl = sfbracketval(hp,hl,&cp)) > 0) {
	                        cchar	*ccp ;
	                        if ((rs = uc_mallocstrw(cp,cl,&ccp)) >= 0) {
	                            ep->articleid = ccp ;
	                        }
	                    }
	                } /* end if (article-id) */
	            }

/* get the content length if it is specified */

	            if (rs >= 0) {
	                hn = HN_CLEN ;
	                if ((hl = mailmsg_hdrival(&am,hn,0,&hp)) > 0) {
	                    rs1 = cfdeci(hp,hl,&v) ;
	                    ep->clen = v ;
	                } /* end if (article content length) */
	            }

/* get the number of lines in the article body (if present) */

	            if (rs >= 0) {
	                hn = HN_CLINES ;
	                hl = mailmsg_hdrival(&am,hn,0,&hp) ;
	                if (hl <= 0) {
	                    hn = HN_LINES ;
	                    hl = mailmsg_hdrival(&am,hn,0,&hp) ;
	                }
	                if (hl <= 0) {
	                    hn = HN_XLINES ;
	                    hl = mailmsg_hdrival(&am,hn,0,&hp) ;
	                }
	                if (hl > 0) {
	                    rs1 = cfdeci(hp,hl,&v) ;
	                    ep->lines = v ;
	                }
	            }

/* get the REPLYTO information */

	            if (rs >= 0) {
	                hn = HN_REPLYTO ;
	                if ((hl = mailmsg_hdrval(&am,hn,&hp)) >= 0) {
	                    cchar	*ccp ;
	                    if ((rs = uc_mallocstrw(hp,hl,&ccp)) >= 0) {
	                        ep->replyto = ccp ;
	                    }
	                } /* end if (from) */
	            }

/* get the FROM information */

	            if (rs >= 0) {
	                hn = HN_FROM ;
	                if ((hl = mailmsg_hdrval(&am,hn,&hp)) >= 0) {
	                    cchar	*ccp ;
	                    if ((rs = uc_mallocstrw(hp,hl,&ccp)) >= 0) {
	                        ep->from = ccp ;
	                    }
	                } /* end if (from) */
	            }

/* get the newsgroups */

	            if (rs >= 0) {
	                hn = HN_NEWSGROUPS ;
	                if ((hl = mailmsg_hdrval(&am,hn,&hp)) >= 0) {
	                    cchar	*cpp ;
	                    if ((rs = uc_mallocstrw(hp,hl,&cpp)) >= 0) {
	                        ep->newsgroups = cpp ;
	                    }
	                } /* end if (newsgroups) */
	            }

/* get the subject */

	            if (rs >= 0) {
	                hn = HN_SUBJECT ;
	                hl = mailmsg_hdrval(&am,"subject",&hp) ;
	                if (hl <= 0) {
	                    hn = HN_TITLE ;
	                    hl = mailmsg_hdrval(&am,hn,&hp) ;
	                }
	                if (hl <= 0) {
	                    hn = HN_SUBJ ;
	                    hl = mailmsg_hdrval(&am,hn,&hp) ;
	                }
	                if (hl >= 0) {
	                    int		bl = strnlen(hp,hl) ;
	                    char	*bp ;
	                    if ((rs = uc_malloc((bl+1),&bp)) >= 0) {
	                        if ((rs = snwcpycompact(bp,bl,hp,hl)) >= 0) {
	                            ep->subject = bp ;
	                        }
	                        if (rs < 0)
	                            uc_free(bp) ;
	                    } /* end if (memory-allocation) */
	                } /* end if */
	            }

/* done with extracting header values with this article */

	        } /* end if (loadfile) */
	        rs1 = mailmsg_finish(&am) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (opened MAILMSG object) */

	    rs1 = bclose(&afile) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (open-file) */

	return rs ;
}
/* end subroutine (entry_load) */


static int cmpartforward(e1pp,e2pp)
ARTLIST_ENT	**e1pp, **e2pp ;
{
	ARTLIST_ENT	*e1p = (ARTLIST_ENT *) *e1pp ;
	ARTLIST_ENT	*e2p = (ARTLIST_ENT *) *e2pp ;

#ifdef	OPTIONAL
	if ((*e1pp == NULL) && (*e2pp == NULL))
	    return 0 ;

	if (*e1pp == NULL)
	    return 1 ;

	if (*e2pp == NULL)
	    return -1 ;

	if (e1p->mtime < e2p->mtime)
	    return -1 ;

	if (e1p->mtime > e2p->mtime)
	    return 1 ;

	return 0 ;
#else
	return (e1p->mtime - e2p->mtime) ;
#endif

}
/* end subroutine (cmpartforward) */


static int cmpartreverse(e1pp,e2pp)
ARTLIST_ENT	**e1pp, **e2pp ;
{
	ARTLIST_ENT	*e1p = (ARTLIST_ENT *) *e1pp ;
	ARTLIST_ENT	*e2p = (ARTLIST_ENT *) *e2pp ;

#ifdef	OPTIONAL
	if ((*e1pp == NULL) && (*e2pp == NULL))
	    return 0 ;

	if (*e1pp == NULL)
	    return 1 ;

	if (*e2pp == NULL)
	    return -1 ;

	if (e1p->mtime > e2p->mtime)
	    return -1 ;

	if (e1p->mtime < e2p->mtime)
	    return 1 ;

	return 0 ;
#else
	return (e2p->mtime - e1p->mtime) ;
#endif

}
/* end subroutine (cmpartreverse) */


/* compare article arrival times (forward) */
static int cmpaf(e1pp,e2pp)
ARTLIST_ENT	**e1pp, **e2pp ;
{
	ARTLIST_ENT	*e1p = (ARTLIST_ENT *) *e1pp ;
	ARTLIST_ENT	*e2p = (ARTLIST_ENT *) *e2pp ;
	time_t		e1t, e2t ;

#if	OPTIONAL
	if ((*e1pp == NULL) && (*e2pp == NULL))
	    return 0 ;

	if (*e1pp == NULL)
	    return 1 ;

	if (*e2pp == NULL)
	    return -1 ;
#endif

	e1t = (e1p->atime != 0) ? e1p->atime : e1p->mtime ;
	e2t = (e2p->atime != 0) ? e2p->atime : e2p->mtime ;

	return (e1t - e2t) ;
}
/* end subroutine (cmpaf) */


/* compare article arrival times (reverse) */
static int cmpar(e1pp,e2pp)
ARTLIST_ENT	**e1pp, **e2pp ;
{
	ARTLIST_ENT	*e1p = (ARTLIST_ENT *) *e1pp ;
	ARTLIST_ENT	*e2p = (ARTLIST_ENT *) *e2pp ;
	time_t		e1t, e2t ;

#if	OPTIONAL
	if ((*e1pp == NULL) && (*e2pp == NULL))
	    return 0 ;

	if (*e1pp == NULL)
	    return 1 ;

	if (*e2pp == NULL)
	    return -1 ;
#endif

	e1t = (e1p->atime != 0) ? e1p->atime : e1p->mtime ;
	e2t = (e2p->atime != 0) ? e2p->atime : e2p->mtime ;

	return (e2t - e1t) ;
}
/* end subroutine (cmpar) */


/* compare article post times (forward) */
static int cmppf(e1pp,e2pp)
ARTLIST_ENT	**e1pp, **e2pp ;
{
	ARTLIST_ENT	*e1p = (ARTLIST_ENT *) *e1pp ;
	ARTLIST_ENT	*e2p = (ARTLIST_ENT *) *e2pp ;
	time_t		e1t, e2t ;

#if	OPTIONAL
	if ((*e1pp == NULL) && (*e2pp == NULL))
	    return 0 ;

	if (*e1pp == NULL)
	    return 1 ;

	if (*e2pp == NULL)
	    return -1 ;
#endif

	if ((e1t = e1p->ptime) == 0) {
	    if ((e1t = e1p->atime) == 0) {
	        e1t = e1p->mtime ;
	    }
	}

	if ((e2t = e2p->ptime) == 0) {
	    if ((e2t = e2p->atime) == 0) {
	        e2t = e2p->mtime ;
	    }
	}

	return (e1t - e2t) ;
}
/* end subroutine (cmppf) */


/* compare article post times (reverse) */
static int cmppr(e1pp,e2pp)
ARTLIST_ENT	**e1pp, **e2pp ;
{
	ARTLIST_ENT	*e1p = (ARTLIST_ENT *) *e1pp ;
	ARTLIST_ENT	*e2p = (ARTLIST_ENT *) *e2pp ;
	time_t		e1t, e2t ;

#if	OPTIONAL
	if ((*e1pp == NULL) && (*e2pp == NULL))
	    return 0 ;

	if (*e1pp == NULL)
	    return 1 ;

	if (*e2pp == NULL)
	    return -1 ;
#endif

	if ((e1t = e1p->ptime) == 0) {
	    if ((e1t = e1p->atime) == 0) {
	        e1t = e1p->mtime ;
	    }
	}
	if ((e2t = e2p->ptime) == 0) {
	    if ((e2t = e2p->atime) == 0) {
	        e2t = e2p->mtime ;
	    }
	}

	return (e2t - e1t) ;
}
/* end subroutine (cmppr) */


/* compare article compose times (forward) */
static int cmpcf(ARTLIST_ENT **e1pp,ARTLIST_ENT **e2pp)
{
	ARTLIST_ENT	*e1p = (ARTLIST_ENT *) *e1pp ;
	ARTLIST_ENT	*e2p = (ARTLIST_ENT *) *e2pp ;
	time_t		e1t, e2t ;

#if	OPTIONAL
	if ((*e1pp == NULL) && (*e2pp == NULL))
	    return 0 ;

	if (*e1pp == NULL)
	    return 1 ;

	if (*e2pp == NULL)
	    return -1 ;
#endif

	if ((e1t = e1p->ctime) == 0) {
	    if ((e1t = e1p->ptime) == 0) {
	        if ((e1t = e1p->atime) == 0) {
	            e1t = e1p->mtime ;
	        }
	    }
	}

	if ((e2t = e2p->ctime) == 0) {
	    if ((e2t = e2p->ptime) == 0) {
	        if ((e2t = e2p->atime) == 0) {
	            e2t = e2p->mtime ;
	        }
	    }
	}

	return (e1t - e2t) ;
}
/* end subroutine (cmpcf) */


/* compare article compose times (reverse) */
static int cmpcr(ARTLIST_ENT **e1pp,ARTLIST_ENT **e2pp)
{
	ARTLIST_ENT	*e1p = (ARTLIST_ENT *) *e1pp ;
	ARTLIST_ENT	*e2p = (ARTLIST_ENT *) *e2pp ;
	time_t		e1t, e2t ;

#if	OPTIONAL
	if ((*e1pp == NULL) && (*e2pp == NULL))
	    return 0 ;

	if (*e1pp == NULL)
	    return 1 ;

	if (*e2pp == NULL)
	    return -1 ;
#endif

	if ((e1t = e1p->ctime) == 0) {
	    if ((e1t = e1p->ptime) == 0) {
	        if ((e1t = e1p->atime) == 0) {
	            e1t = e1p->mtime ;
	        }
	    }
	}

	if ((e2t = e2p->ctime) == 0) {
	    if ((e2t = e2p->ptime) == 0) {
	        if ((e2t = e2p->atime) == 0) {
	            e2t = e2p->mtime ;
	        }
	    }
	}

	return (e2t - e1t) ;
}
/* end subroutine (cmpcr) */


/* compare UNIX times */
static int timecmp(time_t *t1p,time_t *t2p)
{

	return (*t1p - *t2p) ;
}
/* env subroutine (timecmp) */


