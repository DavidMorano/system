/* dialtab SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* get additional machine dialing information */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was adopted for use from the DWD program.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	dialtab

	Description:
	This module provides a management object to access dialing
	information that is used by some PCS utilities to access
	remote machines.  This whole dialing information thing was
	a hack when accessing other machines because so problematic
	due to security considerations.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<mallocxx.h>
#include	<mallocstuff.h>
#include	<getpwd.h>
#include	<absfn.h>
#include	<bfile.h>
#include	<field.h>
#include	<vecobj.h>
#include	<mkpathx.h>
#include	<sfx.h>
#include	<matxstr.h>
#include	<localmisc.h>

#include	"dialtab.h"

import libutil ;

/* local defines */

#define	DT_MAGIC	31415926
#define	DT		dialtab
#define	DT_ENT		dialtab_ent
#define	DT_FI		dialtab_file


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct dialtab_file {
	cchar		*fname ;
	time_t		mti ;
	off_t		fsize ;
	ino_t		ino ;
	dev_t		dev ;
} ;


/* forward references */

template<typename ... Args>
static int dialtab_ctor(DT *op,Args ... args) noex {
    	DIALTAB		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    memclear(hop) ;
	    if ((op->flp = new(nothrow) vecobj) != np) {
	        if ((op->elp = new(nothrow) vecobj) != np) {
		    rs = SR_OK ;
	        } /* end if (new-vecobj) */
		if (rs < 0) {
		    delete op->flp ;
		    op->flp = nullptr ;
		}
	    } /* end if (new-vecobj) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (dialtab_ctor) */

static int dialtab_dtor(DT *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->elp) {
		delete op->elp ;
		op->elp = nullptr ;
	    }
	    if (op->flp) {
		delete op->flp ;
		op->flp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (dialtab_dtor) */

template<typename ... Args>
static inline int dialtab_magic(DT *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == DT_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (dialtab_magic) */

static int	dialtab_fileload(DT *,DT_FI *,cchar *,DT_FI **) noex ;
static int	dialtab_filedump(DT *,int) noex ;
static int	dialtab_filedel(DT *,int) noex ;

static int	file_start(DT_FI *,cchar *) noex ;
static int	file_finish(DT_FI *) noex ;

static int	entry_start(DT_ENT *,int,cchar *,int) noex ;
static int	entry_enough(DT_ENT *) noex ;
static int	entry_finish(DT_ENT *) noex ;

static void	freeit(cchar **) noex ;


/* local variables */

constexpr char 		fterms[32] = {
	0x00, 0x00, 0x00, 0x00,
	0x08, 0x10, 0x00, 0x24,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ;

enum dialkeys {
	dialkey_uucp,
	dialkey_inet,
	dialkey_username,
	dialkey_password,
	dialkey_overlast
} ;

constexpr cpcchar	dialkeys[] = {
	"uucp",
	"inet",
	"username",
	"password",
	nullptr
} ;


/* exported variables */


/* exported subroutines */

int dialtab_open(DT *op,cchar *dialfname) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = dialtab_ctor(op)) >= 0) {
	    int		vsz = szof(DT_FI) ;
	    int		vn = 10 ;
	    int		vo = vecobjm.reuse ;
	    if ((rs = vecobj_start(op->flp,vsz,vn,vo)) >= 0) {
		vsz = szof(DT_ENT) ;
		vn = 20 ;
	        if ((rs = vecobj_start(op->elp,vsz,vn,vo)) >= 0) {
		    op->magic = DT_MAGIC ;
	            if (dialfname) {
	                rs = dialtab_fileadd(op,dialfname) ;
			c = rs ;
	            } /* end if */
		    if (rs < 0) {
			op->magic = 0 ;
			vecobj_finish(op->elp) ;
		    }
		} /* end if (vecobj_start) */
		if (rs < 0) {
		    vecobj_finish(op->flp) ;
		}
	    } /* end if (vecobj_start) */
	    if (rs < 0) {
		dialtab_dtor(op) ;
	    }
	} /* end if (dialtab_ctor) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (dialtab_open) */

int dialtab_close(DT *op) noex {
    	int		rs ;
	int		rs1 ;
	if ((rs = dialtab_magic(op)) >= 0) {
	    void	*vp{} ;
	    if (op->elp) {
	        /* free up the dial entries */
	        for (int i = 0 ; vecobj_get(op->elp,i,&vp) >= 0 ; i += 1) {
	            DT_ENT	*dep = (DT_ENT *) vp ;
	            if (vp) {
	                rs1 = entry_finish(dep) ;
		        if (rs >= 0) rs = rs1 ;
	            }
	        } /* end for */
	        {
	            rs1 = vecobj_finish(op->elp) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	    } else {
	        rs = SR_BUGCHECK ;
	    } /* end if */
	    if (op->flp) {
	        /* free up the files */
	        for (int i = 0 ; vecobj_get(op->flp,i,&vp) >= 0 ; i += 1) {
	            DT_FI	*fep = (DT_FI *) vp ;
	            if (vp) {
	                rs1 = file_finish(fep) ;
		        if (rs >= 0) rs = rs1 ;
	            }
	        } /* end for */
	        {
	            rs1 = vecobj_finish(op->flp) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	    } else {
	        rs = SR_BUGCHECK ;
	    } /* end if */
	    {
	        rs1 = dialtab_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (dialtab_close) */

enum orgloccos {
	adderco_start,
	adderco_finish,
	adderco_overlast
} ;

namespace {
    struct adder ;
    struct adder_co {
	adder		*op{} ;
	int		w = -1 ;
	void operator () (adder *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
        operator int () noex ;
    } ; /* end struct (adder_co) */
    struct adder_fl {
	uint		ent:1 ;
    } ; /* end struct (adder_fl) */
    struct adder {
	DT		*op ;
	DT_ENT		de ;
	adder_co	start ;
	adder_co	finish ;
	adder_fl	flg{} ;
	adder(DT *p) noex : op(p) {
	    start(this,adderco_start) ;
	    finish(this,adderco_finish) ;
	} ;
	int operator () (cchar *) noex ;
	int istart() noex ;
	int ifinish() noex ;
	int reader(DT_FI *,cchar *,int) noex ;
	int procln(int,cchar *,int) noex ;
	int remainder() noex ;
    } ; /* end struct (adder) */
}

int adder::operator () (cchar *afn) noex {
    	int		rs ;
	int		rs1 ;
	int		c = 0 ;
        if ((rs = start) >= 0) {
            DT_FI   fe{} ;
            DT_FI   *fep{} ;
            if ((rs = dialtab_fileload(op,&fe,afn,&fep)) >= 0) {
                cint        fi = rs ;
                {
                    if ((rs = reader(fep,afn,fi)) >= 0) {
                        c += rs ;
                        rs = remainder() ;
                        c += rs ;
                    }
                }
                if (rs < 0) {
                    dialtab_filedump(op,fi) ;
                    dialtab_filedel(op,fi) ;
                }
            } /* end if (dialtab_fileload) */
            rs1 = finish ;
            if (rs >= 0) rs = rs1 ;
        } /* end if (start-finish) */
    	return (rs >= 0) ? c : rs ;
}
/* end method (adder::operator) */

int adder::reader(DT_FI *fep,cchar *fn,int fi) noex {
    	cnullptr	np{} ;
    	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (char *lbuf{} ; (rs = malloc_ml(&lbuf)) >= 0) {
	    cint	llen = rs ;
	    cchar	*os = "r" ;
	    cmode	om = 0 ;
	    if (bfile b ; (rs = bopen(&b,fn,os,om)) >= 0) {
		cint	cmd = BC_STAT ;
		if (USTAT sb ; (rs = bcontrol(&b,cmd,&sb)) >= 0) {
		    cint	to = -1 ;
		    fep->dev = sb.st_dev ;
		    fep->ino = sb.st_ino ;
		    fep->mti = sb.st_mtime ;
		    fep->fsize = sb.st_size ;
	            while ((rs = breadlns(&b,lbuf,llen,to,np)) > 0) {
		        cchar	*lp{} ;
		        if (int ll ; (ll = sfcontent(lbuf,rs,&lp)) > 0) {
		            rs = procln(fi,lp,ll) ;
			    c += rs ;
		        }
	            } /* end if (bfile_readln) */
		} /* end if (bcontrol) */
	        rs1 = bclose(&b) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (bfile) */
	    rs = rsfree(rs,lbuf) ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? c : rs ;
}
/* end method (adder::reader) */

int adder::procln(int fi,cchar *lp,int ll) noex {
    	int		rs ;
	int		rs1 ;
	int		c = 0 ;
        if (field fsb ; (rs = fsb.start(lp,ll)) >= 0) {
	    int		fl ;
            cchar	*fp{} ;
            if ((fl = fsb.get(fterms,&fp)) > 0) {
                if (fsb.term == ':') {
                    if (flg.ent) {
                        if (entry_enough(&de) > 0) {
                            rs = vecobj_add(op->elp,&de) ;
                            c += 1 ;
                            flg.ent = false ;
                        } else {
                            entry_finish(&de) ;
                            flg.ent = false ;
                        }
                    } /* end if (entry) */
		    if (rs >= 0) {
                        rs = entry_start(&de,fi,fp,fl) ;
                        flg.ent = (rs >= 0) ;
		    }
                } else {
                    if (int ki ; (ki = matostr(dialkeys,2,fp,fl)) >= 0) {
                        if ((fl = fsb.get(fterms,&fp)) > 0) {
                            switch (ki) {
                            case dialkey_uucp:
                                de.uucp = mallocstrw(fp,fl) ;
                                break ;
                            case dialkey_inet:
                                de.inet = mallocstrw(fp,fl) ;
                                break ;
                            case dialkey_username:
                                de.username = mallocstrw(fp,fl) ;
                                break ;
                            case dialkey_password:
                                de.password = mallocstrw(fp,fl) ;
                                break ;
                            } /* end switch */
                        } /* end if (got value for this key) */
                    } /* end if (got a valid key) */
                } /* end if */
            } /* end if (non-zero-length field) */
            rs1 = fsb.finish ;
            if (rs >= 0) rs = rs1 ;
        } /* end if (field) */
    	return (rs >= 0) ? c : rs ;
}
/* end method (adder::procln) */

int adder::remainder() noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	if (flg.ent) {
	    if (entry_enough(&de) > 0) {
	        if ((rs = vecobj_add(op->elp,&de)) >= 0) {
	            c += 1 ;
		    flg.ent = false ;
	        }
	    } else {
	        entry_finish(&de) ;
	        flg.ent = false ;
	    }
	} /* end if (had entry) */
	return (rs >= 0) ? c : rs ;
}
/* end method (adder::remainder) */

int dialtab_fileadd(DT *op,cchar *dfname) noex {
    	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = dialtab_magic(op,dfname)) >= 0) {
	    rs = SR_INVALID ;
	    if (dfname[0]) {
	        cchar	*afn{} ;
	        if (absfn af ; (rs = af.start(dfname,-1,&afn)) >= 0) {
		    if (adder ao(op) ; (rs = ao(afn)) >= 0) {
			c = rs ;
		    } /* end if (adder) */
	            rs1 = af.finish ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (absfn) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (dialtab_fileadd) */

/* search the dial table for a name match */
int dialtab_search(DT *op,cchar *name,DT_ENT **depp) noex {
    	int		rs ;
	int		idx = 0 ;
	if ((rs = dialtab_magic(op,name)) >= 0) {
    	    cnullptr	np{} ;
	    void	*vp{} ;
	    for (int i = 0 ; vecobj_get(op->elp,i,&vp) >= 0 ; i += 1) {
		DT_ENT	*ep = (DT_ENT *) vp ;
	        if (vp) {
		    bool	f = true ;
		    cchar	*sp = ep->name ;
		    cchar	*cp ;
	            if (depp) *depp = ep ;
	            f = f && ((cp = strchr(sp,'*')) != np) ;
		    f = f && (strchr(sp,'\\') == np) ;
		    if (f) {
	                if (strncmp(name,sp,(cp - sp)) == 0) {
			    cint	l1 = lenstr(name) ;
			    int		l2 = lenstr(sp) ;
			    int		l ;
	                    cp += 1 ;
	                    l = intconv(sp + l2 - cp) ;
	                    if (strncmp((name + l1 - l),cp,l) == 0) {
	                        idx = i ;
			        break ;
		            }
	                } /* end (strncmp) */
	            } else if (strcmp(name,sp) == 0) {
	                idx = i ;
		        break ;
	            }
	        } /* end if (non-null) */
	    } /* end for */
	} /* end if (magic) */
	return (rs >= 0) ? idx : rs ;
}
/* end subroutine (dialtab_search) */


/* private subroutines */

static int dialtab_fileload(DT *op,DT_FI *fep,cchar *fn,DT_FI **rpp) noex {
    	int		rs = SR_BUGCHECK ;
	int		fi = 0 ;
	if (fep && rpp) {
	    if ((rs = file_start(fep,fn)) >= 0) {
		vecobj	*flp = op->flp ;
	        if ((rs = flp->add(fep)) >= 0) {
	            fi = rs ;
    		    if (void *vp{} ; (rs = flp->get(fi,&vp)) >= 0) {
			DT_FI	*rp = (DT_FI *) vp ;
		        *rpp = rp ;
		    }
		    if (rs < 0) {
		        flp->del(fi) ;
		    }
	        } /* end fi (vecobj_add) */
	        if (rs < 0) {
		    file_finish(fep) ;
	        }
	    } /* end if (file_start) */
	} /* end if (bugcheck) */
	return (rs >= 0) ? fi : rs ;
}
/* end subroutine (dialtab_fileload) */

static int dialtab_filedump(DT *op,int fi) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    void	*vp{} ;
	    rs = SR_OK ;
	    for (int i = 0 ; vecobj_get(op->elp,i,&vp) >= 0 ; i += 1) {
	        DT_ENT	*ep = (DT_ENT *) vp ;
	        if (vp) {
	            if ((fi < 0) || (ep->fi == fi)) {
			{
	                    rs1 = entry_finish(ep) ;
			    if (rs >= 0) rs = rs1 ;
			}
			{
	                    rs1 = vecobj_del(op->elp,i--) ;
			    if (rs >= 0) rs = rs1 ;
			}
	            }
	        } /* end if (non-null) */
	    } /* end for */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (dialtab_filedump) */

static int dialtab_filedel(DT *op,int fi) noex {
	int		rs  SR_BUGCHECK ;
	int		rs1 ;
	if (op && (fi >= 0)) {
	    if (void *vp{} ; (rs = vecobj_get(op->flp,fi,&vp)) >= 0) {
	        DT_FI	*fep = (DT_FI *) vp ;
	        if (vp) {
	            {
	                rs1 = file_finish(fep) ;
		        if (rs >= 0) rs = rs1 ;
		    }
		    {
	                rs1 = vecobj_del(op->flp,fi) ;
		        if (rs >= 0) rs = rs1 ;
		    }
	        } /* end if (non-null) */
	    } /* end if (vecobj_get) */
	} /* end if (valid) */
	return rs ;
}
/* end subroutine (dialtab_filedel) */

static int file_start(DT_FI *fep,cchar *fname) noex {
	int		rs = SR_FAULT ;
	if (fep && fname) {
	    rs = memclear(fep) ;
	    if ((fep->fname = mallocstr(fname)) == nullptr) {
	        rs = SR_NOMEM ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (file_start) */

static int file_finish(DT_FI *fep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (fep) {
	    rs = SR_OK ;
	    if (fep->fname) {
	        rs1 = uc_free(fep->fname) ;
	        if (rs >= 0) rs = rs1 ;
	        fep->fname = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (file_finish) */

static int entry_start(DT_ENT *dep,int fi,cchar *sp,int sl) noex {
	int		rs = SR_BUGCHECK ;
	if (dep && sp && (fi >= 0)) {
	    rs = memclear(dep) ;
	    if ((dep->name = mallocstrw(sp,sl)) != nullptr) {
		dep->fi = fi ;
	    } else {
	        rs = SR_NOMEM ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (entry_start) */

static int entry_enough(DT_ENT *dep) noex {
    	int		rs = SR_FAULT ;
	int		fret = false ;
	if (dep) {
	    rs = SR_OK ;
	    if (dep->name && dep->name[0]) {
	        fret = fret || (dep->uucp && (dep->uucp[0] != '\0')) ;
	        fret = fret || (dep->inet && (dep->inet[0] != '\0')) ;
	        fret = fret || (dep->username && (dep->username[0] != '\0')) ;
	        fret = fret || (dep->password && (dep->password[0] != '\0')) ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? fret : rs ;
}
/* end subroutine (entry_enough) */

static int entry_finish(DT_ENT *dep) noex {
    	int		rs = SR_FAULT ;
	if (dep) {
	    rs = SR_BUGCHECK ;
	    if (dep->name) {
	        rs = SR_OK ;
		freeit(&dep->name) ;
		freeit(&dep->uucp) ;
		freeit(&dep->inet) ;
		freeit(&dep->username) ;
		freeit(&dep->password) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (entry_finish) */

static void freeit(cchar **pp) noex {
	if (*pp) {
	    uc_free(*pp) ;
	    *pp = nullptr ;
	}
}
/* end subroutine (freeit) */

int adder::istart() noex {
	return SR_OK ;
}

int adder::ifinish() noex {
	return SR_OK ;
}

adder_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case adderco_start:
		rs = op->istart() ;
		break ;
	    case adderco_finish:
		rs = op->ifinish() ;
		break ;
	    } /* end switch */
	}
	return rs ;
}
/* end method (adder_co::operator) */


