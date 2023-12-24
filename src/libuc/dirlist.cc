/* dirlist */
/* lang=C++20 */

/* directory list manager */
/* version %I% last-modified %G% */

#define	CF_NULPATH	1		/* specify NUL-paths */

/* revision history:

	= 2000-09-10, David A­D­ Morano
	I created this modeled after something similar that was
	used for some of my PCS programs.

	= 2017-09-11, David A­D­ Morano
	Whew! Enhanced this module (subroutine |dirlist_add()| in
	particular) to make it more robust in the face of a bug in
	KSH. The KSH shell adds stuff to LD_LIBRARY_PATH to suit
	what it thinks are its own needs. It adds stuff at the front
	of that path variable.  But KSH is broken and adds an invalid
	path to the front of LD_LIBRARY_PATH.  We now try to ignore
	as much as possible invalid library-search paths. F*ck KSH!

*/

/* Copyright © 2000,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This object manages directory lists by:
	+ ensuring unique entries by name
	+ ensuring unique entries by dev-inode pair

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<cstdlib>
#include	<cstring>
#include	<new>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<sncpyx.h>
#include	<strwcpy.h>
#include	<pathclean.h>
#include	<strn.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"dirlist.h"


/* local defines */

#define	DIRLIST_ENT	struct dirlist_e
#define	DIRLIST_NDEF	10

#ifndef	CF_NULPATH
#define	CF_NULPATH	0
#endif


/* local namespaces */

using std::nothrow ;


/* local typedefs */

typedef DIRLIST_ENT	ent ;


/* external subroutines */


/* local structures */

struct dirlist_e {
	cchar		*np ;
	uino_t		ino ;
	dev_t		dev ;
	int		nl ;
} ;


/* typedefs */

typedef ent		*entp ;


/* forward references */

int		dirlist_add(dirlist *,cchar *,int) noex ;

template<typename ... Args>
static inline int dirlist_ctor(dirlist *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    nullptr_t	np{} ;
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
	if (op->dbp) {
	    delete op->dbp ;
	    op->dbp = nullptr ;
	}
	return rs ;
}
/* end subroutine (dirlist_dtor) */

template<typename ... Args>
static int dirlist_magic(dirlist *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == DIRLIST_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (dirlist_magic) */

extern "C" {
    static int	vcmpname(ent **,ent **) noex ;
    static int	vcmpdevino(ent **,ent **) noex ;
}

static int	entry_start(ent *,cchar *,int,dev_t,uino_t) noex ;
static int	entry_finish(ent *) noex ;


/* local variables */

constexpr bool		f_nulpath = CF_NULPATH ;


/* exported variables */


/* exported subroutines */

int dirlist_start(dirlist *op) noex {
	int		rs ;
	if ((rs = dirlist_ctor(op)) >= 0) {
	    cint	esize = sizeof(ent) ;
	    cint	ne = DIRLIST_NDEF ;
	    cint	vo = VECOBJ_OORDERED ;
	    if ((rs = vecobj_start(op->dbp,esize,ne,vo)) >= 0) {
	        op->magic = DIRLIST_MAGIC ;
	    }
	    if (rs < 0) {
		dirlist_dtor(op) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (dirlist_start) */

int dirlist_finish(dirlist *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = dirlist_magic(op)) >= 0) {
	    {
	        void	*vp{} ;
	        for (int i = 0 ; vecobj_get(op->dbp,i,&vp) >= 0 ; i += 1) {
	            if (vp) {
		        ent	*ep = entp(vp) ;
	                rs1 = entry_finish(ep) ;
	                if (rs >= 0) rs = rs1 ;
	            }
	        } /* end for */
	    }
	    {
	        rs1 = vecobj_finish(op->dbp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = dirlist_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dirlist_finish) */

int dirlist_semi(dirlist *op) noex {
	int		rs ;
	if ((rs = dirlist_magic(op)) >= 0) {
	    ent		e ;
	    if ((rs = entry_start(&e,";",1,0L,0L)) >= 0) {
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
	int		rs ;
	int		c = 0 ;
	if ((rs = dirlist_magic(op,sp)) >= 0) {
	    int		cl ;
	    cchar	*tp, *cp ;
	    if (sl < 0) sl = strlen(sp) ;
	    while ((tp = strnpbrk(sp,sl,":; \t,")) != nullptr) {
	        cp = sp ;
	        cl = (tp - sp) ;
	        if (rs >= 0) {
	            rs = dirlist_add(op,cp,cl) ;
	            c += rs ;
	        }
	        if ((rs >= 0) && (tp[0] == ';')) {
	            rs = dirlist_semi(op) ;
	        }
	        sl -= ((tp + 1) - sp) ;
	        sp = (tp + 1) ;
	        if (rs < 0) break ;
	    } /* end while */
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
	if ((rs = dirlist_magic(op,sp)) >= 0) {
	    char	*pbuf{} ;
	    if (sl < 0) sl = strlen(sp) ;
	    if ((rs = malloc_mp(&pbuf)) >= 0) {
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
	            e.np = pbuf ;
	            e.nl = pl ;
	            vecobj_vcmp	vcf = vecobj_vcmp(vcmpname) ;
	            void	*vp{} ;
	            if ((rs = vecobj_search(op->dbp,&e,vcf,&vp)) == rsn) {
	                USTAT	sb ;
/* now see if it is already in the list by DEV-INO */
	                if ((rs = u_stat(pbuf,&sb)) >= 0) {
	                    if (S_ISDIR(sb.st_mode)) {
	                        vecobj_vcmp	vcf = vecobj_vcmp(vcmpdevino) ;
				auto		vs = vecobj_search ;
	                        e.dev = sb.st_dev ;
	                        e.ino = sb.st_ino ;
	                        if ((rs = vs(op->dbp,&e,vcf,&vp)) == rsn) {
				    auto	es = entry_start ;
	                            dev_t	d = sb.st_dev ;
	                            uino_t	i = sb.st_ino ;
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

int dirlist_strsize(dirlist *op) noex {
	int		rs ;
	if ((rs = dirlist_magic(op)) >= 0) {
	    rs = op->tlen ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dirlist_strsize) */

int dirlist_curbegin(dirlist *op,dirlist_cur *curp) noex {
	int		rs ;
	if ((rs = dirlist_magic(op,curp)) >= 0) {
	    curp->i = -1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dirlist_curbegin) */

int dirlist_curend(dirlist *op,dirlist_cur *curp) noex {
	int		rs ;
	if ((rs = dirlist_magic(op,curp)) >= 0) {
	    curp->i = -1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dirlist_curend) */

int dirlist_enum(dirlist *op,dirlist_cur *curp,char *rbuf,int rlen) noex {
	int		rs ;
	if ((rs = dirlist_magic(op,curp,rbuf)) >= 0) {
	    int	i = (curp->i >= 0) ? (curp->i+1) : 0 ;
	    void	*vp{} ;
	    while ((rs = vecobj_get(op->dbp,i,&vp)) >= 0) {
	        if (vp) break ;
	        i += 1 ;
	    } /* end while */
	    if ((rs >= 0) && vp) {
	        ent		*ep = entp(vp) ;
	        if ((rs = sncpy1w(rbuf,rlen,ep->np,ep->nl)) >= 0) {
	            curp->i = i ;
	        }
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dirlist_enum) */

int dirlist_get(dirlist *op,dirlist_cur *curp,cchar **rpp) noex {
	int		rs ;
	if ((rs = dirlist_magic(op,curp)) >= 0) {
	    int		i = (curp->i >= 0) ? (curp->i+1) : 0 ;
	    void	*vp{} ;
	    while ((rs = vecobj_get(op->dbp,i,&vp)) >= 0) {
	        if (vp) break ;
	        i += 1 ;
	    } /* end while */
	    if (rpp) {
	        ent		*ep = entp(vp) ;
	        *rpp = (rs >= 0) ? ep->np : nullptr ;
	    }
	    if ((rs >= 0) && vp) {
	        ent		*ep = entp(vp) ;
	        curp->i = i ;
	        rs = ep->nl ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dirlist_get) */

int dirlist_joinsize(dirlist *op) noex {
	int		rs ;
	if ((rs = dirlist_magic(op)) >= 0) {
	    rs = op->tlen ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dirlist_joinsize) */

int dirlist_joinmk(dirlist *op,char *jbuf,int jlen) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = dirlist_magic(op,jbuf)) >= 0) {
	    int		djlen = op->tlen ;
	    if (jlen >= djlen) {
	        char	*bp = jbuf ;
	        cchar	*dp ;
	        int	dl ;
	        bool	f_semi = false ;
	        void	*vp{} ;
	        for (int i = 0 ; vecobj_get(op->dbp,i,&vp) >= 0 ; i += 1) {
	            ent	*ep = entp(vp) ;
	            if (vp == nullptr) break ;
	            dp = ep->np ;
	            dl = ep->nl ;
	            if (dp[0] != ';') {
	                if (c++ > 0) {
	                    if (f_semi) {
	                        f_semi = false ;
	                        *bp++ = ';' ;
	                    } else {
	                        *bp++ = ':' ;
			    }
	                } /* end if */
		        if constexpr (f_nulpath) {
	                    if ((dl > 0) && (! ((dp[0] == '.') && (dl == 1)))) {
	                        bp = strwcpy(bp,dp,dl) ;
	                    }
		        } else {
	                    if (dl > 0) {
	                        bp = strwcpy(bp,dp,dl) ;
	                    }
		        } /* end if-constexpr */
	                c += 1 ;
	            } else {
	                f_semi = true ;
		    }
	        } /* end for */
	        rs = (bp - jbuf) ;
	    } else {
	        rs = SR_OVERFLOW ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (dirlist_joinmk) */


/* private subroutines */

static int entry_start(ent *ep,cc *np,int nl,dev_t dev,uino_t ino) noex {
	int		rs = SR_OK ;
	memclear(ep) ;
	ep->dev = dev ;
	ep->ino = ino ;
	ep->nl = nl ;
	{
	    cchar		*cp{} ;
	    rs = uc_mallocstrw(np,nl,&cp) ;
	    if (rs >= 0) ep->np = cp ;
	} /* end block */
	return rs ;
}
/* end subroutine (entry_start) */

static int entry_finish(ent *ep) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (ep->np) {
	    rs1 = uc_free(ep->np) ;
	    if (rs >= 0) rs = rs1 ;
	    ep->np = nullptr ;
	    ep->nl = 0 ;
	}
	return rs ;
}
/* end subroutine (entry_finish) */

static int vcmpname(ent **e1pp,ent **e2pp) noex {
	int		rc = 0 ;
	if ((*e1pp != nullptr) || (*e2pp != nullptr)) {
	    if (*e1pp == nullptr) rc = 1 ;
	    else if (*e2pp == nullptr) rc = -1 ;
	    if (rc == 0) {
	        rc = strcmp((*e1pp)->np,(*e2pp)->np) ;
	    }
	}
	return rc ;
}
/* end subroutine (vcmpname) */

static int vcmpdevino(ent **e1pp,ent **e2pp) noex {
	int		rc = 0 ;
	if ((*e1pp != nullptr) || (*e2pp != nullptr)) {
	    if (*e1pp == nullptr) rc = 1 ;
	    else if (*e2pp == nullptr) rc = -1 ;
	    if (rc == 0) {
	        if ((rc = ((*e1pp)->ino - (*e2pp)->ino)) == 0) {
	            rc = (*e1pp)->dev - (*e2pp)->dev ;
	        }
	    }
	}
	return rc ;
}
/* end subroutine (vcmpdevino) */


