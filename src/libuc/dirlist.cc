/* dirlist SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* directory list manager */
/* version %I% last-modified %G% */

#define	CF_NULPATH	1		/* specify NUL-paths */

/* revision history:

	= 2000-09-10, David A­D­ Morano
	I created this modeled after something similar that was
	used for some of my PCS programs.

	= 2017-09-11, David A­D­ Morano
	Whew!  Enhanced this module (subroutine |dirlist_add()| in
	particular) to make it more robust in the face of a bug in
	KSH.  The KSH shell adds stuff to LD_LIBRARY_PATH to suit
	what it thinks are its own needs.  It adds stuff at the
	front of that path variable.  But KSH is broken and adds
	an invalid path to the front of LD_LIBRARY_PATH.  We now
	try to ignore as much as possible invalid library-search
	paths.  F*ck KSH!

*/

/* Copyright © 2000,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	dirlist

	Description:
	This object manages directory lists by:
	+ ensuring unique entries by name
	+ ensuring unique entries by dev-inode pair

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>		/* |USTAT| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstddef>
#include	<cstdlib>
#include	<cstring>		/* <- for |strlen(3c)| */
#include	<new>			/* |nothrow(3c++)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<mallocxx.h>
#include	<sncpyx.h>
#include	<strwcpy.h>
#include	<pathclean.h>
#include	<strn.h>
#include	<mkchar.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"dirlist.h"

import libutil ;

/* local defines */

#define	DIRLIST_NDEF	10

#ifndef	CF_NULPATH
#define	CF_NULPATH	0
#endif


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */

struct dirlist_ent ;

typedef dirlist_ent	ent ;
typedef dirlist_ent	*entp ;


/* external subroutines */

extern "C" {
    int		dirlist_add(dirlist *,cchar *,int) noex ;
}


/* local structures */

struct dirlist_ent {
	cchar		*sp ;
	ino_t		ino ;
	dev_t		dev ;
	int		sl ;
} ;


/* forward references */

template<typename ... Args>
static inline int dirlist_ctor(dirlist *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    op->magic = 0 ;
	    op->tlen = 0 ;
	    if ((op->dbp = new(nothrow) vecobj) != np) {
		rs = SR_OK ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (dirlist_ctor) */

static int dirlist_dtor(dirlist *op) noex {
	int		rs = SR_OK ;
	if (op->dbp) ylikely {
	    delete op->dbp ;
	    op->dbp = nullptr ;
	}
	return rs ;
}
/* end subroutine (dirlist_dtor) */

template<typename ... Args>
static int dirlist_magic(dirlist *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magic == DIRLIST_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (dirlist_magic) */

extern "C" {
    static int	vcmpname(cvoid **,cvoid **) noex ;
    static int	vcmpdevino(cvoid **,cvoid **) noex ;
}

static int	entry_start(ent *,cchar *,int,dev_t,ino_t) noex ;
static int	entry_finish(ent *) noex ;


/* local variables */

constexpr bool		f_nulpath = CF_NULPATH ;


/* exported variables */


/* exported subroutines */

int dirlist_start(dirlist *op) noex {
	int		rs ;
	if ((rs = dirlist_ctor(op)) >= 0) ylikely {
	    cint	esz = szof(ent) ;
	    cint	ne = DIRLIST_NDEF ;
	    cint	vo = VECOBJ_OORDERED ;
	    if ((rs = vecobj_start(op->dbp,esz,ne,vo)) >= 0) {
	        op->magic = DIRLIST_MAGIC ;
	    }
	    if (rs < 0) {
		dirlist_dtor(op) ;
	    }
	} /* end if (dirlist_ctor) */
	return rs ;
}
/* end subroutine (dirlist_start) */

int dirlist_finish(dirlist *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = dirlist_magic(op)) >= 0) ylikely {
	    if (op->dbp) {
		vecobj	*dbp = op->dbp ;
	        {
	            void	*vp{} ;
	            for (int i = 0 ; dbp->get(i,&vp) >= 0 ; i += 1) {
		        ent	*ep = cast_static<entp>(vp) ;
	                if (vp) {
	                    rs1 = entry_finish(ep) ;
	                    if (rs >= 0) rs = rs1 ;
	                }
	            } /* end for */
	        }
	        {
	            rs1 = dbp->finish ;
	            if (rs >= 0) rs = rs1 ;
	        }
	    } /* end if (op->dbp) */
	    {
	        rs1 = dirlist_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dirlist_finish) */

int dirlist_semi(dirlist *op) noex { /* add a semicolon as an entry */
	int		rs ;
	if ((rs = dirlist_magic(op)) >= 0) ylikely {
	    if (ent e ; (rs = entry_start(&e,";",1,0L,0L)) >= 0) {
	        op->tlen += (rs+1) ;
	        rs = vecobj_add(op->dbp,&e) ;
	        if (rs < 0) {
	            entry_finish(&e) ;
	        }
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dirlist_semi) */

int dirlist_adds(dirlist *op,cchar *sp,int sl) noex {
    	cnullptr	np{} ;
	int		rs ;
	int		c = 0 ;
	if ((rs = dirlist_magic(op,sp)) >= 0) ylikely {
	    if (sl < 0) sl = lenstr(sp) ;
	    for (cc *tp ; (tp = strnbrk(sp,sl,":; \t,")) != np ; ) {
	        cchar	*cp = sp ;
	        cint	cl = intconv(tp - sp) ;
	        if (rs >= 0) {
	            rs = dirlist_add(op,cp,cl) ;
	            c += rs ;
	        }
	        if ((rs >= 0) && (tp[0] == ';')) {
	            rs = dirlist_semi(op) ;
	        }
	        sl -= intconv((tp + 1) - sp) ;
	        sp = (tp + 1) ;
	        if (rs < 0) break ;
	    } /* end for */
	    if ((rs >= 0) && (sl > 0)) {
	        rs = dirlist_add(op,sp,sl) ;
	        c += rs ;
	    } /* end if */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (dirlist_adds) */

int dirlist_add(dirlist *op,cchar *sp,int sl) noex {
	int		rs ;
	int		rs1 ;
	int		f_added = false ;
	if ((rs = dirlist_magic(op,sp)) >= 0) ylikely {
	    if (sl < 0) sl = lenstr(sp) ;
	    if (char *pbuf ; (rs = malloc_mp(&pbuf)) >= 0) {
	        int	pl{} ;
	        if ((sl == 0) || ((sl == 1) && (sp[0] == '.'))) {
	            pbuf[0] = '.' ;
	            pbuf[1] = '\0' ;
	            pl = 1 ;
	        } else {
	            rs = pathclean(pbuf,sp,sl) ;
	            pl = rs ;
	        }
	        if (rs >= 0) {
	            ent		e ;
	            cint	rsn = SR_NOTFOUND ;
		    /* any NUL (blank) paths need to be converted to "." */
	            if (pl == 0) {
	                pbuf[0] = '.' ;
	                pbuf[1] = '\0' ;
	                pl = 1 ;
	            }
		    /* now see if it is already in the list by NAME */
	            e.sp = pbuf ;
	            e.sl = pl ;
	            vecobj_vcf	vcf = vecobj_vcf(vcmpname) ;
	            void	*vp{} ;
	            if ((rs = vecobj_search(op->dbp,&e,vcf,&vp)) == rsn) {
			/* now see if it is already in the list by DEV-INO */
	                if (USTAT sb ; (rs = u_stat(pbuf,&sb)) >= 0) {
	                    if (S_ISDIR(sb.st_mode)) {
	                        vecobj_vcf	vdf = vecobj_vcf(vcmpdevino) ;
				auto		vs = vecobj_search ;
	                        e.dev = sb.st_dev ;
	                        e.ino = sb.st_ino ;
	                        if ((rs = vs(op->dbp,&e,vdf,&vp)) == rsn) {
				    auto	es = entry_start ;
	                            dev_t	d = sb.st_dev ;
	                            ino_t	i = sb.st_ino ;
	                            f_added = true ;
	                            if ((rs = es(&e,pbuf,pl,d,i)) >= 0) {
	                                op->tlen += (rs+1) ;
	                                rs = vecobj_add(op->dbp,&e) ;
	                                if (rs < 0) {
	                                    entry_finish(&e) ;
				        }
	                            } /* end if (entry_start) */
	                        } /* end if (was not found) */
	                    } else {
	                        rs = SR_NOENT ;
	                    } /* end if (isDir) */
		        } else if (isNotPresent(rs)) {
		            rs = SR_OK ;
	                } /* end if (stat) */
	            } /* end if (was not found) */
	        } /* end if (ok) */
		rs1 = uc_free(pbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (magic) */
	return (rs >= 0) ? f_added : rs ;
}
/* end subroutine (dirlist_add) */

int dirlist_count(dirlist *op) noex {
	int		rs ;
	if ((rs = dirlist_magic(op)) >= 0) ylikely {
	    rs = vecobj_count(op->dbp) ;
	} /* end if (magic) */
	return rs ;
}

int dirlist_strsize(dirlist *op) noex {
	int		rs ;
	if ((rs = dirlist_magic(op)) >= 0) ylikely {
	    rs = op->tlen ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dirlist_strsize) */

int dirlist_curbegin(dirlist *op,dirlist_cur *curp) noex {
	int		rs ;
	if ((rs = dirlist_magic(op,curp)) >= 0) ylikely {
	    curp->i = -1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dirlist_curbegin) */

int dirlist_curend(dirlist *op,dirlist_cur *curp) noex {
	int		rs ;
	if ((rs = dirlist_magic(op,curp)) >= 0) ylikely {
	    curp->i = -1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dirlist_curend) */

int dirlist_curenum(dirlist *op,dirlist_cur *curp,char *rbuf,int rlen) noex {
	int		rs ;
	if ((rs = dirlist_magic(op,curp,rbuf)) >= 0) ylikely {
	    int		i = (curp->i >= 0) ? (curp->i+1) : 0 ;
	    void	*vp{} ;
	    while ((rs = vecobj_get(op->dbp,i,&vp)) >= 0) {
	        if (vp) break ;
	        i += 1 ;
	    } /* end while */
	    if ((rs >= 0) && vp) {
		ent	*ep = cast_static<entp>(vp) ;
	        if ((rs = sncpy1w(rbuf,rlen,ep->sp,ep->sl)) >= 0) {
	            curp->i = i ;
	        }
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dirlist_curenum) */

int dirlist_curget(dirlist *op,dirlist_cur *curp,cchar **rpp) noex {
	int		rs ;
	if ((rs = dirlist_magic(op,curp)) >= 0) ylikely {
	    int		i = (curp->i >= 0) ? (curp->i+1) : 0 ;
	    void	*vp{} ;
	    while ((rs = vecobj_get(op->dbp,i,&vp)) >= 0) {
	        if (vp) break ;
	        i += 1 ;
	    } /* end while */
	    if (rpp) {
		ent	*ep = cast_static<entp>(vp) ;
	        *rpp = (rs >= 0) ? ep->sp : nullptr ;
	    }
	    if ((rs >= 0) && vp) {
	        ent	*ep = entp(vp) ;
	        curp->i = i ;
	        rs = ep->sl ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dirlist_curget) */

int dirlist_joinsize(dirlist *op) noex {
	int		rs ;
	if ((rs = dirlist_magic(op)) >= 0) ylikely {
	    rs = op->tlen ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dirlist_joinsize) */

int dirlist_joinmk(dirlist *op,char *jbuf,int jlen) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = dirlist_magic(op,jbuf)) >= 0) ylikely {
	    int		djlen = op->tlen ;
	    if (jlen >= djlen) {
		vecobj	*dbp = op->dbp ;
	        char	*bp = jbuf ;
	        cchar	*dp ;
	        int	dl ;
	        bool	f_semi = false ;
	        void	*vp{} ;
	        for (int i = 0 ; dbp->get(i,&vp) >= 0 ; i += 1) {
		    ent	*ep = cast_static<entp>(vp) ;
	            if (vp == nullptr) break ;
	            dp = ep->sp ;
	            dl = ep->sl ;
	            if (dp[0] != ';') {
	                if (c++ > 0) {
	                    if (f_semi) {
	                        f_semi = false ;
	                        *bp++ = ';' ;
	                    } else {
	                        *bp++ = ':' ;
			    }
	                } /* end if */
		        if_constexpr (f_nulpath) {
			    bool	f = (dl > 0) ;
			    f = f && (! ((dp[0] == '.') && (dl == 1))) ;
	                    if (f) {
	                        bp = strwcpy(bp,dp,dl) ;
	                    }
		        } else {
	                    if (dl > 0) {
	                        bp = strwcpy(bp,dp,dl) ;
	                    }
		        } /* end if_constexpr (f_nulpath) */
	            } else {
	                f_semi = true ;
		    }
	        } /* end for */
	        rs = intconv(bp - jbuf) ;
	    } else {
	        rs = SR_OVERFLOW ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (dirlist_joinmk) */


/* private subroutines */

int dirlist::adds(cchar *sp,int sl) noex {
	return dirlist_adds(this,sp,sl) ;
}

int dirlist::add(cchar *sp,int sl) noex {
	return dirlist_add(this,sp,sl) ;
}

int dirlist::curbegin(dirlist_cur *curp) noex {
	return dirlist_curbegin(this,curp) ;
}

int dirlist::curend(dirlist_cur *curp) noex {
	return dirlist_curend(this,curp) ;
}

int dirlist::curenum(dirlist_cur *curp,char *rbuf,int rlen) noex {
	return dirlist_curenum(this,curp,rbuf,rlen) ;
}

int dirlist::curget(dirlist_cur *curp,cchar **rpp) noex {
	return dirlist_curget(this,curp,rpp) ;
}

int dirlist::joinmk(char *jbuf,int jlen) noex {
	return dirlist_joinmk(this,jbuf,jlen) ;
}

void dirlist::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("dirlist",rs,"fini-finish") ;
	}
}

dirlist::operator int () noex {
    	return dirlist_count(this) ;
}

dirlist_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case dirlistmem_start:
	        rs = dirlist_start(op) ;
	        break ;
	    case dirlistmem_semi:
	        rs = dirlist_semi(op) ;
	        break ;
	    case dirlistmem_count:
	        rs = dirlist_count(op) ;
	        break ;
	    case dirlistmem_strsize:
	        rs = dirlist_strsize(op) ;
	        break ;
	    case dirlistmem_join:
	        rs = dirlist_join(op) ;
	        break ;
	    case dirlistmem_finish:
	        rs = dirlist_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (dirlist_co::operator) */

static int entry_start(ent *ep,cc *sp,int sl,dev_t dev,ino_t ino) noex {
	int		rs = SR_FAULT ;
	if (ep && sp) ylikely {
	    memclear(ep) ;
	    ep->dev = dev ;
	    ep->ino = ino ;
	    if (cchar *cp ; (rs = uc_mallocstrw(sp,sl,&cp)) >= 0) {
	        ep->sl = rs ;
	        ep->sp = cp ;
	    } /* end if */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (entry_start) */

static int entry_finish(ent *ep) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (ep->sp) ylikely {
	    rs1 = uc_free(ep->sp) ;
	    if (rs >= 0) rs = rs1 ;
	    ep->sp = nullptr ;
	    ep->sl = 0 ;
	}
	return rs ;
}
/* end subroutine (entry_finish) */

static int cmpname(const entp e1p,const entp e2p) noex {
	cint        ml = min(e1p->sl,e2p->sl) ;
	int         rc ;
	cchar       *s1 = e1p->sp ;
	cchar       *s2 = e2p->sp ;
	{
            cint        ch1 = mkchar(*s1) ;
            cint        ch2 = mkchar(*s2) ;
            if ((rc = (ch1 - ch2)) == 0) {
                rc = strncmp(s1,s2,ml) ;
            }
	} /* end block */
	return rc ;
} /* end subroutine (cmpname) */

static int vcmpname(cvoid **v1pp,cvoid **v2pp) noex {
	ent		*e1p = entp(*v1pp) ;
	ent		*e2p = entp(*v2pp) ;
	int		rc = 0 ;
	if (e1p || e2p) ylikely {
	    rc = +1 ;
	    if (e1p) {
		rc = -1 ;
	        if (e2p) {
		    rc = cmpname(e1p,e2p) ;
		}
	    }
	} /* end if */
	return rc ;
}
/* end subroutine (vcmpname) */

static int vcmpdevino(cvoid **v1pp,cvoid **v2pp) noex {
	ent		*e1p = entp(*v1pp) ;
	ent		*e2p = entp(*v2pp) ;
	int		rc = 0 ;
	if (e1p || e2p) ylikely {
	    rc = +1 ;
	    if (e1p) {
		rc = -1 ;
	        if (e2p) {
	            if ((rc = intconv(e1p->dev - e2p->dev)) == 0) {
		        rc = intconv(e1p->ino - e2p->ino) ;
		    }
	        }
	    }
	} /* end if */
	return rc ;
}
/* end subroutine (vcmpdevino) */


