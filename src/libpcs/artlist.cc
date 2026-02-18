/* artlist SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* article list handling */
/* version %I% last-modified %G% */


/* revision history:

	= 1995-05-01, David A­D­ Morano
	This code module was completely rewritten to replace any
	original garbage that came before.

	= 1998-11-22, David A­D­ Morano
        I did some clean-up.

	= 2017-10-24, David A­D­ Morano
	Some small refactoring.

	= 2026-02-01, David A­D­ Morano
	More minor refactoring.

*/

/* Copyright © 1995,1998,2017,2026 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	artlist

	Description:
	This object maintains a list of articles.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<sys/timeb.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<ucmem.h>
#include	<strn.h>
#include	<sfx.h>			/* |sfbracketval(3uc)| */
#include	<snwcpyx.h>		/* |sfbracketval(3uc)| */
#include	<vechand.h>
#include	<bfile.h>
#include	<mailmsg.h>
#include	<mailmsghdrs.h>
#include	<dater.h>
#include	<cfdec.h>
#include	<char.h>
#include	<localmisc.h>		/* |TIMEBUFLEN| */

#include	"artlist.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#ifndef	AL_NENT
#define	AL_NENT		10
#endif

#define	AL		artlist
#define	AL_ENT		artlist_ent


/* imports */

using libuc::mem ;			/* variable */
using std::nothrow ;			/* constant */


/* typedefs */

extern "C" {
    typedef int (*cmp_f)(AL_ENT **,AL_ENT **) noex ;
    typedef int	(*sortcmp_t)(cvoid *,cvoid *) noex ;
}

typedef time_t (*enttime_f)(AL_ENT *) noex ;

typedef artlist_ent *	entp ;


/* external subroutines */

extern int	mailmsg_loadfile(mailmsg *,bfile *) noex ;
extern int	mailmsg_envtimes(mailmsg *,dater *,time_t *,int) noex ;


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
local int artlist_ctor(artlist *op,Args ... args) noex {
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = SR_NOMEM ;
	    if ((op->datep = new(nothrow) dater) != np) {
	        if ((op->artp = new(nothrow) vechand) != np) {
		    rs = SR_OK ;
	        } /* end if (new-vechand) */
		if (rs < 0) {
		    delete op->datep ;
		    op->datep = np ;
		} /* end if (error) */
	    } /* end if (new-dater) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (artlist_ctor) */

local int artlist_dtor(artlist *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->artp) {
		delete op->artp ;
		op->artp = nullptr ;
	    }
	    if (op->datep) {
		delete op->datep ;
		op->datep = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (artlist_dtor) */

template<typename ... Args>
local inline int artlist_magic(artlist *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magval == ARTLIST_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (artlist_magic) */

local int cmpartforward(AL_ENT **e1pp,AL_ENT **e2pp) noex ;
local int cmpartreverse(AL_ENT **e1pp,AL_ENT **e2pp) noex ;

local int	cmpaf(AL_ENT **e1pp,AL_ENT **e2pp) noex ;
local int	cmpar(AL_ENT **e1pp,AL_ENT **e2pp) noex ;
local int	cmppf(AL_ENT **e1pp,AL_ENT **e2pp) noex ;
local int	cmppr(AL_ENT **e1pp,AL_ENT **e2pp) noex ;
local int	cmpcf(AL_ENT **e1pp,AL_ENT **e2pp) noex ;
local int	cmpcr(AL_ENT **e1pp,AL_ENT **e2pp) noex ;

local int	entry_start(AL_ENT *,dater *,cchar *,cchar *) noex ;
local int	entry_finish(AL_ENT *) noex ;
local int	entry_load(AL_ENT *,dater *,cchar *) noex ;

local int	timecmp(time_t *,time_t *) noex ;

local time_t	ent_ctime(AL_ENT *) noex ;
local time_t	ent_ptime(AL_ENT *) noex ;
local time_t	ent_atime(AL_ENT *) noex ;
local time_t	ent_mtime(AL_ENT *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int artlist_start(AL *op,TIMEB *nowp,cchar *zname) noex {
    	ARTLIST		*hop = op ;
	int		rs ;
	if ((rs = artlist_ctor(op,zname)) >= 0) {
	    dater *datep = op->datep ;
	    memclear(hop) ; /* dangerous */
	    if (nowp) {
	        cint sz = szof(TIMEB) ;
	        memcopy(&op->now,nowp,sz) ;
	    }
	    if ((rs = datep->start(nowp,zname,-1)) >= 0) {
		vechand *artp = op->artp ;
		cint	vn = 20 ;
	        cint	vo = vechandm.sorted ;
	        if ((rs = artp->start(vn,vo)) >= 0) {
	            op->magval = ARTLIST_MAGIC ;
	        }
	        if (rs < 0) {
	            datep->finish() ;
	        }
	    } /* end if */
	    if (rs < 0) {
		artlist_dtor(op) ;
	    } /* end if (error) */
	} /* end if (artlist_ctor) */
	return rs ;
}
/* end subroutine (artlist_start) */

/* free up this object */
int artlist_finish(AL *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if ((rs = artlist_magic(op)) >= 0) {
	    vechand *artp = op->artp ;
	    void *vp ; 
	    for (int i = 0 ; artp->get(i,&vp) >= 0 ; i += 1) {
	        AL_ENT	*ep = entp(vp) ;
	        if (vp) {
		    {
	                rs1 = entry_finish(ep) ;
	                if (rs >= 0) rs = rs1 ;
		    }
		    {
	                rs1 = mem.free(ep) ;
	                if (rs >= 0) rs = rs1 ;
		    }
	        } /* end if (non-null) */
	    } /* end for */
	    {
	        rs1 = artp->finish ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		dater *datep = op->datep ;
	        rs1 = datep->finish ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = artlist_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (artlist_finish) */

/* add another entry to this object */
int artlist_add(AL *op,cchar *ngdir,cchar *name) noex {
	int		rs ;
	if ((rs = artlist_magic(op,ngdir,name)) >= 0) {
	    vechand *artp = op->artp ;
	    rs = SR_INVALID ;
	    if (ngdir[0] && name[0]) {
	        AL_ENT	*aep ;
	        cint esz = szof(AL_ENT) ;
	        if ((rs = mem.mall(esz,&aep)) >= 0) {
	            if ((rs = entry_start(aep,op->datep,ngdir,name)) >= 0) {
	                rs = artp->add(aep) ;
	                if (rs < 0) {
	                    entry_finish(aep) ;
		        }
	            }
	            if (rs < 0) {
	                mem.free(aep) ;
	            }
	        } /* end if (m-a) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (artlist_add) */

/* sort the entries with given sorting mode and direction */
int artlist_sort(AL *op,int sortmode,int f_reverse) noex {
	int		rs ;
	if ((rs = artlist_magic(op)) >= 0) {
	    vechand *artp = op->artp ;
    	    cmp_f	cmpfunc ;
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
	    {
	        vechand_f vvcmp = vechand_f(cmpfunc) ;
	        rs = artp->sort(vvcmp) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (artlist_sort) */

/* get the basic information from the given entry */
int artlist_get(AL *op,int i,cchar **ngdpp,cchar **npp,time_t *mp) noex {
	int		rs ;
	if ((rs = artlist_magic(op)) >= 0) {
	    vechand *artp = op->artp ;
	    if (ngdpp) *ngdpp = nullptr ;
	    if (npp) *npp = nullptr ;
	    if (mp) *mp = 0 ;
	    if (void *vp ; (rs = artp->get(i,&vp)) >= 0) {
	        AL_ENT	*ep = entp(vp) ;
	        if (vp) {
	            if (ngdpp) {
	                *ngdpp = (cchar *) ep->ngdir ;
	            }
	            if (npp) {
	                *npp = (cchar *) ep->name ;
	            }
	            if (mp) {
	                *mp = ep->mtime ;
	            }
	        } /* end if (non-null) */
	    } /* end if (vechand-get) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (artlist_get) */

/* get the whole entry */
int artlist_getentry(AL *op,int i,AL_ENT **epp) noex {
	int		rs ;
	if ((rs = artlist_magic(op,epp)) >= 0) {
	    vechand *artp = op->artp ;
	    if (void *vp ; (rs = artp->get(i,&vp)) >= 0) {
	        AL_ENT	*ep = entp(vp) ;
	        *epp = ep ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (artlist_getentry) */


/* private subroutines */

local int entry_start(AL_ENT *ep,dater *dp,cc *ngdir,cc *name) noex {
	int		rs = SR_FAULT ;
	if (ep && ngdir && name) {
	    rs = SR_INVALID ;
	    memclear(ep) ;
	    if (ngdir[0] && name[0]) {
	        if (ustat sb ; (rs = u_stat(name,&sb)) >= 0) {
	            if (S_ISREG(sb.st_mode)) {
	                ep->mtime = sb.st_mtime ;
	                ep->ngdir = ngdir ;
	                if (cchar *cp ; (rs = mem.strw(name,-1,&cp)) >= 0) {
	                    ep->name = cp ;
	                    if ((rs = entry_load(ep,dp,name)) >= 0) {
	                        ep->magval = ARTLIST_CURMAGIC ;
	                    }
	                    if (rs < 0) {
	                        void *vp = voidp(ep->name) ;
	                        mem.free(vp) ;
	                        ep->name = nullptr ;
	                    } /* end if (error) */
	                } /* end if (memory-allocation) */
	            } else {
	                rs = SR_ISDIR ;
	            }
	        } /* end if (u_stat) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (entry_start) */

local int entry_finish(AL_ENT *ep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (ep) {
	    rs = SR_OK ;
	    if (ep->name) {
	        void *vp = voidp(ep->name) ;
	        rs1 = mem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        ep->name = nullptr ;
	    }
	    if (ep->subject) {
	        void *vp = voidp(ep->subject) ;
	        rs1 = mem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        ep->subject = nullptr ;
	    }
	    if (ep->replyto) {
	        void *vp = voidp(ep->replyto) ;
	        rs1 = mem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        ep->replyto = nullptr ;
	    }
	    if (ep->from) {
	        void *vp = voidp(ep->from) ;
	        rs1 = mem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        ep->from = nullptr ;
	    }
	    if (ep->newsgroups) {
	        void *vp = voidp(ep->newsgroups) ;
	        rs1 = mem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        ep->newsgroups = nullptr ;
	    }
	    if (ep->messageid) {
	        void *vp = voidp(ep->messageid) ;
	        rs1 = mem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        ep->messageid = nullptr ;
	    }
	    if (ep->articleid) {
	        void *vp = voidp(ep->articleid) ;
	        rs1 = mem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        ep->articleid = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (entry_finish) */

local int entry_load(AL_ENT *ep,dater *dp,cchar *name) noex {
	int		rs ;
	int		rs1 ;
	cmode		om = 0666 ;
	if (bfile afile ; (rs = bopen(&afile,name,"r",om)) >= 0) {
	    if (mailmsg am ; (rs = mailmsg_start(&am)) >= 0) {
	        if ((rs = mailmsg_loadfile(&am,&afile)) >= 0) {
	            time_t	ta[AL_NENT] ;
	            int		hl ;
	            int		n ;
	            int		v ;
	            cchar	*hn ;
	            cchar	*hp ;
		    /* get the envelope times (post & arrive) */
	            ep->ptime = ep->atime = 0 ;
	            n = mailmsg_envtimes(&am,dp,ta,AL_NENT) ;
	            if (n > 1) {
	                sortcmp_t	scmp = (sortcmp_t) timecmp ;
	                size_t		sortlen = n ;
	                cint		esz = szof(time_t) ;
	                qsort(ta,sortlen,esz,scmp) ;
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
	                    if ((rs = dp->setmsg(hp,hl)) >= 0) {
	                        time_t	t ;
	                        dp->gettime(&t) ;
	                        ep->ctime = t ;
	                    }
	                } /* end if (message date) */
	            } /* end if (ok) */

/* get the message id */

	            if (rs >= 0) {
	                hn = HN_MESSAGEID ;
	                if ((hl = mailmsg_hdrival(&am,hn,0,&hp)) >= 0) {
			    cchar *sp ;
	                    if (int sl ; (sl = sfbracketval(hp,hl,&sp)) > 0) {
				cchar *cp ;
	                        if ((rs = mem.strw(sp,sl,&cp)) >= 0) {
	                            ep->messageid = cp ;
	                        }
	                    }
	                } /* end if (message-id) */
	            }

/* get the article id */

	            if (rs >= 0) {
	                hn = HN_ARTICLEID ;
	                if ((hl = mailmsg_hdrival(&am,hn,0,&hp)) >= 0) {
			    cchar *sp ;
	                    if (int sl ; (sl = sfbracketval(hp,hl,&sp)) > 0) {
			        cchar *cp ;
	                        if ((rs = mem.strw(sp,sl,&cp)) >= 0) {
	                            ep->articleid = cp ;
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
	                    cchar	*cp ;
	                    if ((rs = mem.strw(hp,hl,&cp)) >= 0) {
	                        ep->replyto = cp ;
	                    }
	                } /* end if (from) */
	            }

/* get the FROM information */

	            if (rs >= 0) {
	                hn = HN_FROM ;
	                if ((hl = mailmsg_hdrval(&am,hn,&hp)) >= 0) {
	                    cchar	*cp ;
	                    if ((rs = mem.strw(hp,hl,&cp)) >= 0) {
	                        ep->from = cp ;
	                    }
	                } /* end if (from) */
	            }

/* get the newsgroups */

	            if (rs >= 0) {
	                hn = HN_NEWSGROUPS ;
	                if ((hl = mailmsg_hdrval(&am,hn,&hp)) >= 0) {
	                    cchar	*cpp ;
	                    if ((rs = mem.strw(hp,hl,&cpp)) >= 0) {
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
	                    int		bl = lenstr(hp,hl) ;
	                    char	*bp ;
	                    if ((rs = mem.mall((bl+1),&bp)) >= 0) {
	                        if ((rs = snwcpycompact(bp,bl,hp,hl)) >= 0) {
	                            ep->subject = bp ;
	                        }
	                        if (rs < 0) {
	                            mem.free(bp) ;
				}
	                    } /* end if (memory-allocation) */
	                } /* end if */
	            } /* end if (ok) */
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

/* compare article post times (reverse) */
local int cmpxy(enttime_f etf,AL_ENT **e1pp,AL_ENT **e2pp,bool f) noex {
	AL_ENT	*e1p = (AL_ENT *) *e1pp ;
	AL_ENT	*e2p = (AL_ENT *) *e2pp ;
	int		rc = 0 ;
	if (e1p || e2p) {
	    if (e1p) {
		if (e2p) {
		    custime et1 = etf(e1p) ;
		    custime et2 = etf(e2p) ;
		    if (f) {
		        rc = intsat(et1 - et2) ;
		    } else {
		        rc = intsat(et2 - et1) ;
		    }
		} else {
		    rc = -1 ;
		}
	    } else {
		rc = +1 ;
	    }
	}
	return rc ;
}
/* end subroutine (cmpxy) */

/* compare article compose times (forward) */
local int cmpcf(AL_ENT **e1pp,AL_ENT **e2pp) noex {
    	return cmpxy(ent_ctime,e1pp,e2pp,true) ;
}
/* end subroutine (cmpcf) */

/* compare article compose times (reverse) */
local int cmpcr(AL_ENT **e1pp,AL_ENT **e2pp) noex {
    	return cmpxy(ent_ctime,e1pp,e2pp,false) ;
}
/* end subroutine (cmpcr) */

/* compare article post times (forward) */
local int cmppf(AL_ENT **e1pp,AL_ENT **e2pp) noex {
    	return cmpxy(ent_ptime,e1pp,e2pp,true) ;
}
/* end subroutine (cmppf) */

/* compare article post times (reverse) */
local int cmppr(AL_ENT **e1pp,AL_ENT **e2pp) noex {
    	return cmpxy(ent_ptime,e1pp,e2pp,false) ;
}
/* end subroutine (cmppr) */

/* compare article post times (forward) */
local int cmpaf(AL_ENT **e1pp,AL_ENT **e2pp) noex {
    	return cmpxy(ent_atime,e1pp,e2pp,true) ;
}
/* end subroutine (cmpaf) */

/* compare article post times (reverse) */
local int cmpar(AL_ENT **e1pp,AL_ENT **e2pp) noex {
    	return cmpxy(ent_atime,e1pp,e2pp,false) ;
}
/* end subroutine (cmppf) */

local int cmpartforward(AL_ENT **e1pp,AL_ENT **e2pp) noex {
    	return cmpxy(ent_mtime,e1pp,e2pp,true) ;
}
/* end subroutine (cmpartforward) */

local int cmpartreverse(AL_ENT **e1pp,AL_ENT **e2pp) noex {
    	return cmpxy(ent_mtime,e1pp,e2pp,false) ;
}
/* end subroutine (cmpartreverse) */

/* compare UNIX times */
local int timecmp(time_t *t1p,time_t *t2p) noex {
    	int		rc = intconv(*t1p - *t2p) ;
	return rc ;
}
/* env subroutine (timecmp) */

local time_t ent_ctime(AL_ENT *ep) noex {
    	time_t	t ;
	if ((t = ep->ctime) == 0z) {
	    if ((t = ep->ptime) == 0z) {
	        if ((t = ep->atime) == 0z) {
	            t = ep->mtime ;
	        }
	    }
	}
	return t ;
} /* end subroutine (ent_ctime) */

local time_t ent_ptime(AL_ENT *ep) noex {
    	time_t	t ;
	    if ((t = ep->ptime) == 0z) {
	        if ((t = ep->atime) == 0z) {
	            t = ep->mtime ;
	        }
	    }
	return t ;
} /* end subroutine (ent_ptime) */

local time_t ent_atime(AL_ENT *ep) noex {
    	time_t	t ;
	        if ((t = ep->atime) == 0z) {
	            t = ep->mtime ;
	        }
	return t ;
} /* end subroutine (ent_atime) */

local time_t ent_mtime(AL_ENT *ep) noex {
	return ep->mtime ;
} /* end subroutine (ent_mtime) */


