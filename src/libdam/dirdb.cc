/* dirdb SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* handle directory list operations */
/* version %I% last-modified %G% */

#define	CF_STATCMP	0		/* compare only parts of STAT */

/* revision history:

	= 1996-03-01, David A­D­ Morano
	The program was written from scratch to do what the previous
	program by the same name did.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object:
	dirdb

	Description:
	This object manages directory names in an in-memory database.
	Check out the methods to see what operations are available.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |lenstr(3c)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<mallocxx.h>
#include	<hdb.h>
#include	<sfx.h>
#include	<mkpathx.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"dirdb.h"

import libutil ;

/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef dirdb_ent *	entp ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static int dirdb_ctor(dirdb *op,Args ... args) noex {
	DIRDB		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->dlp = new(nothrow) vechand) != np) {
	        if ((op->dbp = new(nothrow) hdb) != np) {
		    rs = SR_OK ;
		} /* end if (new­hdb) */
		if (rs < 0) {
		    delete op->dlp ;
		    op->dlp = nullptr ;
		}
	    } /* end if (new-vechand) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (dirdb_ctor) */

static int dirdb_dtor(dirdb *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->dbp) {
		delete op->dbp ;
		op->dbp = nullptr ;
	    }
	    if (op->dlp) {
		delete op->dlp ;
		op->dlp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (dirdb_dtor) */

template<typename ... Args>
static inline int dirdb_magic(dirdb *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == DIRDB_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (dirdb_magic) */

static int	dirdb_alreadyentry(dirdb *,dirdb_ent *) noex ;
static int	dirdb_alreadyname(dirdb *,cchar *,int) noex ;
static int	dirdb_adding(dirdb *,ustat *,cchar *,int) noex ;

static int	entry_start(dirdb_ent *,cchar *,int,ustat *,int) noex ;
static int	entry_finish(dirdb_ent *) noex ;

#if	CF_STATCMP
extern "C" {
    static int	vcmpstat(cvoid **,cvoid **) noex ;
}
#endif


/* local variables */


/* exported variables */


/* exported subroutines */

int dirdb_start(dirdb *op,int n) noex {
	int		rs ;
	if ((rs = dirdb_ctor(op)) >= 0) {
	    op->count = 0 ;
	    if ((rs = vechand_start(op->dlp,n,0)) >= 0) {
	        if ((rs = hdb_start(op->dbp,n,1,nullptr,nullptr)) >= 0) {
	            op->magic = DIRDB_MAGIC ;
	        }
	        if (rs < 0) {
		    vechand_finish(op->dlp) ;
	        }
	    } /* end if (vechand_start) */
	    if (rs < 0) {
		dirdb_dtor(op) ;
	    }
	} /* end if (dirdb_ctor) */
	return rs ;
}
/* end subroutine (dirdb_start) */

int dirdb_finish(dirdb *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = dirdb_magic(op)) >= 0) {
	    if (op->dbp) {
		vechand	*dlp = op->dlp ;
	        void	*vp{} ;
	        for (int i = 0 ; dlp->get(i,&vp) >= 0 ; i += 1) {
		    dirdb_ent	*ep = entp(vp) ;
	            if (vp) {
		        {
	                    rs1 = entry_finish(ep) ;
	                    if (rs >= 0) rs = rs1 ;
		        }
		        {
	                    rs1 = uc_free(ep) ;
	                    if (rs >= 0) rs = rs1 ;
		        }
	            }
	        } /* end for */
		{
	            rs1 = hdb_finish(op->dbp) ;
	            if (rs >= 0) rs = rs1 ;
		}
	    }
	    if (op->dlp) {
	        rs1 = vechand_finish(op->dlp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = dirdb_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->count = 0 ;
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dirdb_finish) */

int dirdb_add(dirdb *op,cchar *dp,int dl) noex {
	int		rs ;
	int		rs1 ;
	int		f_new = false ;
	if ((rs = dirdb_magic(op,dp)) >= 0) {
	    rs = SR_INVALID ;
	    if (dp[0]) {
	        if (dl < 0) dl = lenstr(dp) ;
	        if (char *tbuf ; (rs = malloc_mp(&tbuf)) >= 0) {
	             if ((rs = mkpath1w(tbuf,dp,dl)) >= 0) {
	                 if (ustat sb ; (rs = uc_stat(tbuf,&sb)) >= 0) {
		             if (S_ISDIR(sb.st_mode)) {
		                 bool	f_add = true ;
		                 while ((dl > 0) && (dp[dl-1] == '/')) {
	    	                     dl -= 1 ;
		                 }
		                 cchar	*sp = dp ;
		                 int	sl = dl ;
		                 if ((rs = dirdb_alreadyname(op,dp,dl)) == 0) {
	    	                     int	cl ;
  	    		             cchar	*cp ;
			             while ((cl = sfdirname(dp,dl,&cp)) > 0) {
				         auto	al = dirdb_alreadyname ;
	    		                 if ((rs = al(op,cp,cl)) > 0) {
				             f_add = false ;
				             break ;
			                 }
	    		                 dp = cp ;
	    		                 dl = cl ;
		                     } /* end while */
			             /* enter this into the database list */
		                     if ((rs >= 0) && f_add) {
		                         f_new = true ;
	    	                         rs = dirdb_adding(op,&sb,sp,sl) ;
		                     } /* end if (ok) */
		                 } /* end if (dirdb_alreadyname) */
		             } /* end if (is-dir) */
	                 } else if (isNotPresent(rs)) {
		             rs = SR_OK ;
	                 } /* end if (stat) */
	            } /* end if (mkpath) */
		    rs1 = uc_free(tbuf) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? f_new : rs ;
}
/* end subroutine (dirdb_add) */

int dirdb_clean(dirdb *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = dirdb_magic(op)) >= 0) {
	    if (hdb_cur cur ; (rs = hdb_curbegin(op->dbp,&cur)) >= 0) {
	        hdb_dat		key{} ;
	        hdb_dat		val{} ;
	        while ((rs1 = hdb_curenum(op->dbp,&cur,&key,&val)) >= 0) {
	            dirdb_ent	*ep = entp(val.buf) ;
	            if ((rs = dirdb_alreadyentry(op,ep)) >= 0) {
		        {
	                    rs1 = hdb_curdel(op->dbp,&cur,0) ;
		            if (rs >= 0) rs = rs1 ;
		        }
		        {
	                    rs1 = entry_finish(ep) ;
		            if (rs >= 0) rs = rs1 ;
		        }
		        {
	                    rs1 = uc_free(ep) ;
		            if (rs >= 0) rs = rs1 ;
		        }
	            } else if (isNotPresent(rs)) {
		        rs = SR_OK ;
	            } /* end if */
	        } /* end while */
	        if ((rs >= 0) && (rs1 != SR_NOTFOUND)) rs = rs1 ;
	        rs1 = hdb_curend(op->dbp,&cur) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (kdb-cursor) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dirdb_clean) */

int dirdb_curbegin(dirdb *op,dirdb_cur *curp) noex {
	int		rs ;
	if ((rs = dirdb_magic(op,curp)) >= 0) {
	    curp->i = -1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dirdb_curbegin) */

int dirdb_curend(dirdb *op,dirdb_cur *curp) noex {
	int		rs ;
	if ((rs = dirdb_magic(op,curp)) >= 0) {
	    curp->i = -1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dirdb_curend) */

int dirdb_curenum(dirdb *op,dirdb_cur *curp,dirdb_ent **epp) noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = dirdb_magic(op,curp,epp)) >= 0) {
	    vechand	*dlp = op->dlp ;
	    int		i = (curp->i < 0) ? 0 : (curp->i + 1) ;
	    if (void *vp ; (rs = dlp->get(i,&vp)) >= 0) {
		*epp = entp(vp) ;
	        curp->i = i ;
	        len = lenstr((*epp)->name) ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (dirdb_enum) */


/* private subroutines */

static int dirdb_adding(dirdb *op,ustat *sbp,cchar *sp,int sl) noex {
	cint		sz = szof(dirdb_ent) ;
	int		rs ;
	if (void *vp{} ; (rs = uc_malloc(sz,&vp)) >= 0) {
	    dirdb_ent	*ep = entp(vp) ;
	    if ((rs = entry_start(ep,sp,sl,sbp,op->count)) >= 0) {
	        if ((rs = vechand_add(op->dlp,ep)) >= 0) {
		    hdb_dat	key ;
		    hdb_dat	val ;
	            int		dbi = rs ;
	            key.buf = &ep->fid ;
	            key.len = szof(dirdb_fid) ;
	            val.buf = ep ;
	            val.len = sz ;
	            if ((rs = hdb_store(op->dbp,key,val)) >= 0) {
	                op->count += 1 ;
	            } /* end if (hdb_store) */
	            if (rs < 0) {
		        vechand_del(op->dlp,dbi) ;
		    }
	        } /* end if (vechand_add) */
	        if (rs < 0) {
	            entry_finish(ep) ;
		}
	    } /* end if (entry_start) */
	    if (rs < 0) {
	        uc_free(ep) ;
	    }
	} /* end if (m-a) */
	return rs ;
}
/* end subroutine (dirdb_adding) */

static int dirdb_alreadyentry(dirdb *op,dirdb_ent *ep) noex {
	int		rs = SR_OK ;
	int		dnamel = lenstr(ep->name) ;
	int		f = false ; /* return-value */
	cchar		*dnamep = ep->name ;
	cchar		*cp ;
	for (int cl ; (cl = sfdirname(dnamep,dnamel,&cp)) > 0 ; ) {
	    if ((rs = dirdb_alreadyname(op,cp,cl)) > 0) {
		f = true ;
	        break ;
	    }
	    dnamep = cp ;
	    dnamel = cl ;
	    if (rs < 0) break ;
	} /* end for */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (dirdb_alreadyentry) */

static int dirdb_alreadyname(dirdb *op,cchar *name,int nlen) noex {
	int		rs ;
	int		rs1 ;
	int		f = false ;
	if (char *tbuf ; (rs = malloc_mp(&tbuf)) >= 0) {
	    if (nlen < 0) nlen = lenstr(name) ;
	    if ((rs = mkpath1w(tbuf,name,nlen)) >= 0) {
	        if (ustat sb ; (rs = uc_stat(tbuf,&sb)) >= 0) {
	            dirdb_fid	fid{} ;
	            hdb_dat	key ;
	            hdb_dat	val{} ;
	            fid.ino = sb.st_ino ;
	            fid.dev = sb.st_dev ;
	            key.buf = &fid ;
	            key.len = szof(dirdb_fid) ;
	            if ((rs = hdb_fetch(op->dbp,key,nullptr,&val)) >= 0) {
		        f = true ;
	            } else if (rs == SR_NOTFOUND) {
		        rs = SR_OK ;
	            }
	        } else if (isNotPresent(rs)) {
	            rs = SR_OK ;
	        }
	    } /* end if (mkpath) */
	    rs1 = uc_free(tbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (dirdb_alreadyname) */

static int entry_start(dirdb_ent *ep,cc *sp,int sl,ustat *sbp,int count) noex {
	int		rs = SR_FAULT ;
	if (ep) {
	    if (sl < 0) sl = lenstr(sp) ;
	    memclear(ep) ;
	    ep->fid.ino = sbp->st_ino ;
	    ep->fid.dev = sbp->st_dev ;
	    ep->count = count ;
	    if (cchar *cp ; (rs = uc_mallocstrw(sp,sl,&cp)) >= 0) {
	        ep->name = cp ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (entry_start) */

static int entry_finish(dirdb_ent *ep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (ep) {
	    rs = SR_OK ;
	    if (ep->name) {
	        rs1 = uc_free(ep->name) ;
	        if (rs >= 0) rs = rs1 ;
	        ep->name = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (entry_finish) */

#if	CF_STATCMP

static int cmpstat(const ustat *e1p,const ustat *e2p) noex {
	int		rc = (e1p->st_ino - e2p->st_ino) ;
	if (rc == 0) {
	    rc = (e1p->st_dev - e2p->st_dev) ;
	}
	return rc ;
}
/* end subroutine (cmpstat) */

static int vcmpstat(cvoid **v1pp,cvoid **v2pp) noex {
	const ustat	*e1p = (ustat *) *v1pp ;
	const ustat	*e2p = (ustat *) *v2pp ;
	int		rc = 0 ;
	if (e1p || e2p) {
	    rc = +1 ;
	    if (e1p) {
		rc = -1 ;
		if (e2p) {
		    rc = cmpstat(e1p,e2p) ;
		}
	    }
	}
	return rc ;
}
/* end subroutine (vcmpstat) */

#endif /* CF_STATCMP */


