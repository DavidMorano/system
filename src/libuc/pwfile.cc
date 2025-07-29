/* pwfile SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* open a 'passwd' formatted file for access */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-01, David A­D­ Morano
	This subroutine (object) module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	pwfile

	Description:
	This module manipulates passsword records in a 'passwd'
	type file.  No, it is not particularly pretty but such is
	life often times!

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>		/* open-flags */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<pwd.h>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<bfile.h>
#include	<hdb.h>
#include	<storeitem.h>
#include	<mallocstuff.h>
#include	<strn.h>
#include	<snwcpy.h>
#include	<rmx.h>
#include	<cfdec.h>
#include	<lockfile.h>
#include	<pwentry.h>
#include	<localmisc.h>

#include	"pwfile.h"

import libutil ;

/* local defines */

#define	DEFENTS	10

#define	PF		pwfile
#define	PF_CUR		pwfile_cur

#define	PWE		pwentry

#define	TO_LOCK		60

#ifndef	F_LOCK
#define	F_ULOCK	0
#define	F_LOCK	1
#define	F_TLOCK	2
#define	F_TEST	3
#endif


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static int pwfile_ctor(PF *op,Args ... args) noex {
	PWFILE		*hop = op ;
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->alp = new(nothrow) vecitem) != np) ylikely {
	        if ((op->ulp = new(nothrow) hdb) != np) ylikely {
		    rs = SR_OK ;
		} /* end if (new-hdb) */
		if (rs < 0) {
		    delete op->alp ;
		    op->alp = nullptr ;
		}
	    } /* end if (new-vecitem) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (pwfile_ctor) */

static int pwfile_dtor(PF *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->ulp) {
		delete op->ulp ;
		op->ulp = nullptr ;
	    }
	    if (op->alp) {
		delete op->alp ;
		op->alp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (pwfile_dtor) */

template<typename ... Args>
static int pwfile_magic(PF *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magic == PWFILE_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (pwfile_magic) */

static int	pwfile_loadbegin(PF *) noex ;
static int	pwfile_loadend(PF *) noex ;
static int	pwfile_filefront(PF *) noex ;
static int	pwfile_filefronter(PF *) noex ;
static int	pwfile_fileln(PF *,cchar *,int) noex ;
static int	pwfile_fileback(PF *) noex ;
static int	pwfile_checkopen(PF *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int pwfile_open(PF *op,cchar *pwfname) noex {
	int		rs ;
	if ((rs = pwfile_ctor(op,pwfname)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (pwfname[0]) ylikely {
	        if (cchar *cp ; (rs = uc_mallocstrw(pwfname,-1,&cp)) >= 0) {
	            op->fname = cp ;
	            op->lfd = -1 ;
	            if ((rs = pwfile_loadbegin(op)) >= 0) ylikely {
	                op->f = {} ;
	                op->magic = PWFILE_MAGIC ;
	            }
	            if (rs < 0) {
	                uc_free(op->fname) ;
	                op->fname = nullptr ;
	            }
	        } /* end if (m-a) */
	    } /* end if (valid) */
	    if (rs < 0) {
		pwfile_dtor(op) ;
	    }
	} /* end if (pwfile_ctor) */
	return rs ;
}
/* end subroutine (pwfile_open) */

int pwfile_close(PF *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = pwfile_magic(op)) >= 0) ylikely {
	    {
	        rs1 = pwfile_loadend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->lfd >= 0) ylikely {
	        rs1 = u_close(op->lfd) ;
	        if (rs >= 0) rs = rs1 ;
	        op->lfd = -1 ;
	    }
	    if (op->fname) ylikely {
	        rs1 = uc_free(op->fname) ;
	        if (rs >= 0) rs = rs1 ;
	        op->fname = nullptr ;
	    }
	    {
		rs1 = pwfile_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (pwfile_close) */

int pwfile_curenum(PF *op,PF_CUR *curp,PWE *uep,char *rbuf,int rlen) noex {
	int		rs ;
	if ((rs = pwfile_magic(op,curp,uep,rbuf)) >= 0) ylikely {
	    pwentry	*ep = nullptr ; /* used-afterwards */
	    void	*vp{} ;
	    while ((rs = vecitem_get(op->alp,curp->i,&vp)) >= 0) {
	        ep = (PWE *) vp ;
	        curp->i += 1 ;
	        if (vp) break ;
	    } /* end while */
/* load up the user's structure with the information from our internal one */
	    if ((rs >= 0) && ep) {
	        rs = ep->mkcopy(uep,rbuf,rlen) ;
	    } else if (rs == SR_NOTFOUND) {
	        rs = SR_OK ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (pwfile_curenum) */

/* fetch the next entry that matches the specified username */
int pwfile_fetchuser(PF *op,cc *un,PF_CUR *curp,PWE *uep,char *rb,int rl) noex {
	int		rs ;
	if ((rs = pwfile_magic(op,un,curp,uep,rb)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (un[0]) ylikely {
	        ustat	sb ;
	        if (op->lfd < 0) {
	            rs = uc_stat(op->fname,&sb) ;
	        } else {
	            rs = uc_fstat(op->lfd,&sb) ;
	        }
	        if (rs >= 0) ylikely {
	            hdb_dat	key ;
	            hdb_dat	val{} ;
	            hdb_cur	*hcurp = ((curp) ? curp->hcp : nullptr) ;
	            if (sb.st_mtime > op->readtime) {
	                pwfile_loadend(op) ;
	                pwfile_loadbegin(op) ;
	            } /* end if */
		    /* continue with the query */
	            key.buf = un ;
	            key.len = lenstr(un) ;
	            if ((rs = hdb_fetch(op->ulp,key,hcurp,&val)) >= 0) {
	                if (uep) {
	                    pwentry	*ep = (PWE *) val.buf ;
	                    rs = ep->mkcopy(uep,rb,rl) ;
	                }
	            } /* end if (hdb_fetch) */
	        } /* end if (ok) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (pwfile_fetchuser) */

int pwfile_curbegin(PF *op,PF_CUR *curp) noex {
	int		rs ;
	if ((rs = pwfile_magic(op,curp)) >= 0) ylikely {
	    if ((rs = pwfile_checkopen(op)) >= 0) ylikely {
		int	cmd ; /* used in two blocks below */
	        bool	f_locked = false ;
	        if (! op->f.locked) {
		    cint	to = TO_LOCK ;
		    cmd = F_RLOCK ;
	            if ((rs = lockfile(op->lfd,cmd,0z,0z,to)) < 0) {
	                u_close(op->lfd) ;
	                op->lfd = -1 ;
	                return rs ;
	            }
	            op->f.locked = op->f.locked_cur = true ;
	            f_locked = true ;
	        } /* end if (not locked) */
	        curp->i = 0 ;
	        if (rs >= 0) ylikely {
		    rs = hdb_curbegin(op->ulp,curp->hcp) ;
		}
	        if ((rs < 0) && f_locked) {
	            op->f.locked = op->f.locked_cur = false ;
#ifdef	COMMENT
		    cmd = F_ULOCK ;
	            lockfile(op->lfd,cmd,0z,0z,to) ;
#else
	            u_close(op->lfd) ;
	            op->lfd = -1 ;
#endif
	        } /* end if (cleanup on error) */
	    } /* end if (pwfile_checkopen) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (pwfile_curbegin) */

int pwfile_curend(PF *op,PF_CUR *curp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = pwfile_magic(op,curp)) >= 0) ylikely {
	    if (op->f.locked_cur && (! op->f.locked_explicit)) {
		cint	to = TO_LOCK ;
	        op->f.locked = false ;
	        rs1 = lockfile(op->lfd,F_ULOCK,0z,0z,to) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->f.locked_cur = false ;
	    curp->i = 0 ;
	    rs1 = hdb_curend(op->ulp,curp->hcp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (pwfile_curend) */

int pwfile_lock(PF *op,int type,int to_lock) noex {
	int		rs ;
	int		f_opened = false ;
	if ((rs = pwfile_magic(op)) >= 0) ylikely {
	    if ((rs = pwfile_checkopen(op)) >= 0) ylikely {
	        f_opened = (rs > 0) ;
	        switch (type) {
	        case F_ULOCK:
	            if (op->f.locked_explicit) {
	                op->f.locked = op->f.locked_explicit = false ;
	                rs = lockfile(op->lfd,type,0z,0z,to_lock) ;
	                if (rs < 0) {
	                    u_close(op->lfd) ;
	                    op->lfd = -1 ;
	                }
	            } else {
	                rs = SR_INVALID ;
	            }
	            break ;
	        default:
	        case F_RTEST:
	        case F_WTEST:
	            rs = SR_LOCKED ;
	            if (! op->f.locked) {
	                rs = lockfile(op->lfd,type,0z,0z,to_lock) ;
#ifdef	COMMENT
	                if (f_opened) {
	                    u_close(op->lfd) ;
	                    op->lfd = -1 ;
	                }
#endif /* COMMENT */
	            } /* end if (not locked already) */
	            break ;
	        case F_RLOCK:
	        case F_WLOCK:
	        case F_TRLOCK:
	        case F_TWLOCK:
	            if (! op->f.locked) {
			cint	to = to_lock ;
	                if ((rs = lockfile(op->lfd,type,0z,0z,to)) >= 0) {
	                    op->f.locked = op->f.locked_explicit = true ;
	                }
	            } else {
	                rs = SR_INVALID ;
	            }
	            break ;
	        } /* end switch */
	    } /* end if (pwfile_checkopen) */
	} /* end if (magic) */
	return (rs >= 0) ? f_opened : rs ;
}
/* end subroutine (pwfile_lock) */


/* private subroutines */

static int pwfile_loadbegin(PF *op) noex {
	int		rs ;
	int		n = 0 ;
	if ((rs = pwfile_filefront(op)) >= 0) ylikely {
	    cnullptr	np{} ;
	    n = rs ;
	    if ((rs = hdb_start(op->ulp,n,0,np,np)) >= 0) ylikely {
	        vecitem		*alp = op->alp ;
	        void		*vp{} ;
	        for (int i = 0 ; vecitem_get(alp,i,&vp) >= 0 ; i += 1) {
	            pwentry	*ep = (pwentry *) vp ;
	            if (vp) {
	        	hdb_dat		key ;
	        	hdb_dat		val ;
	                key.buf = ep->username ;
	                key.len = lenstr(ep->username) ;
	                val.buf = ep ;
	                val.len = szof(pwentry) ;
	                rs = hdb_store(op->ulp,key,val) ;
	            }
	            if (rs < 0) break ;
	        } /* end for */
	        if (rs < 0) {
	            hdb_finish(op->ulp) ;
	        }
	    } /* end if (hdb_start) */
	    if (rs < 0) {
	        pwfile_fileback(op) ;
	    }
	} /* end if (pwfile_filefront) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (pwfile_loadbegin) */

static int pwfile_loadend(PF *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->ulp) ylikely {
	    rs1 = hdb_finish(op->ulp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	{
	    rs1 = pwfile_fileback(op) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (pwfile_loadend) */

static int pwfile_filefront(PF *op) noex {
	int		rs = SR_NOENTRY ;
	if (op->fname[0]) ylikely {
	    if (ustat sb ; (rs = uc_stat(op->fname,&sb)) >= 0) ylikely {
	        vecitem		*alp = op->alp ;
	        cint		vo = VECITEM_OCOMPACT ;
	        int		vn = intconv((sb.st_size / 60) + 5) ;
	        if (vn < DEFENTS) vn = DEFENTS ;
	        if ((rs = alp->start(vn,vo)) >= 0) ylikely {
	            op->readtime = sb.st_mtime ;
	            rs = pwfile_filefronter(op) ;
	            if (rs < 0) {
	                alp->finish() ;
		    }
	        }
	    } /* end if (uc_stat) */
	} /* end if (valid) */
	return rs ;
}
/* end subroutine (pwfile_filefront) */

static int pwfile_filefronter(PF *op) noex {
	int		rs ;
	int		rs1 ;
	int		n = 0 ;
	if (char *lbuf ; (rs = malloc_ml(&lbuf)) >= 0) ylikely {
	    bfile	pf, *fp = &pf ;
	    cint	llen = rs ;
	    cmode	om = 0644 ;
	    if ((rs = bopen(fp,op->fname,"rc",om)) >= 0) ylikely {
	        if (! op->f.locked) {
	            rs = bcontrol(fp,BC_LOCKREAD,TO_LOCK) ;
	        }
	        if (rs >= 0) ylikely {
	            while ((rs = breadln(fp,lbuf,llen)) > 0) ylikely {
			cint	ll = rmeol(lbuf,rs) ;
			rs = pwfile_fileln(op,lbuf,ll) ;
			n += rs ;
	                if (rs < 0) break ;
	            } /* end while (reading file entries) */
	            if ((rs >= 0) && (! op->f.locked)) {
	                rs = bcontrol(fp,BC_UNLOCK,0) ;
	            }
	        } /* end if (ok) */
	        rs1 = bclose(fp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (bfile) */
	    rs1 = uc_free(lbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (pwfile_filefronter) */

static int pwfile_fileln(PF *op,cchar *lbuf,int ll) noex {
    	cnullptr	np{} ;
    	int		rs ;
    	int		n = 0 ;
        if (pwentry ent ; (rs = ent.start) >= 0) ylikely {
            int         fn = 0 ;
            int         cl = ll ;
	    cchar	*cp = lbuf ; /* used-afterwards */
            for (cc *tp ; (tp = strnchr(cp,cl,':')) != np ; ) {
		cint tl = intconv(tp - cp) ;
                rs = ent.fieldpw(fn,cp,tl) ;
                cl -= intconv((tp + 1) - cp) ;
                cp = (tp + 1) ;
                fn += 1 ;
                if (rs < 0) break ;
            } /* end for */
            if ((rs >= 0) && (cl > 0) && cp[0]) {
                rs = ent.fieldpw(fn,cp,cl) ;
            }
            /* make any extras fields that we want */
            if (rs >= 0) {
                rs = ent.mkextras ;
            }
            /* add the entry to our list */
            if (rs >= 0) {
                cint    esz = szof(pwentry) ;
                n += 1 ;
                rs = vecitem_add(op->alp,&ent,esz) ;
            }
            if (rs < 0) {
                ent.finish() ;
            }
        } /* end if (initialized new entry) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (pwfile_fileln) */

static int pwfile_fileback(PF *op) noex {
	vecitem		*alp = op->alp ;
	int		rs = SR_OK ;
	int		rs1 ;
	void		*vp{} ;
	for (int i = 0 ; alp->get(i,&vp) >= 0 ; i += 1) {
	    pwentry	*ep = (PWE *) vp ;
	    if (vp) {
	        rs1 = ep->finish ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end for */
	{
	    rs1 = alp->finish ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (pwfile_fileback) */

static int pwfile_checkopen(PF *op) noex {
	int		rs = SR_OK ;
	int		f = false ;
	if (op->lfd < 0) {
	    cint	of = O_RDONLY ;
	    cmode	om = 0666 ;
	    rs = u_open(op->fname,of,om) ;
	    op->lfd = rs ;
	    f = true ;
	} /* end if (it wasn not open) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (pwfile_checkopen) */


