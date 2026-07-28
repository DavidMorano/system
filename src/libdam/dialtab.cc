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
#include	<unistd.h>		/* POSIX */
#include	<fcntl.h>		/* POSIX */
#include	<ctime>			/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<getpwd.h>		/* LIBUC */
#include	<absfn.h>		/* LIBUC */
#include	<fieldterminit.hh>	/* LIBUC */
#include	<field.h>		/* LIBUC */
#include	<vecobj.h>		/* LIBUC */
#include	<mkpathx.h>		/* LIBUC */
#include	<sfx.h>			/* LIBUC */
#include	<matxstr.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<bfile.h>		/* LIBB */

#include	"dialtab.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;

/* local defines */

#define	DT		dialtab
#define	DT_ENT		dialtab_ent
#define	DT_FI		dialtab_file
#define	DT_MAGIC	0x31415926


/* imported namespaces */

using libuc::mem ;			/* variable */
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
} ; /* end struct */


/* forward references */

template<typename ... Args>
local int dialtab_ctor(DT *op,Args ... args) noex {
    	DIALTAB		*hop = op ;
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = SR_NOMEM ;
	    memclear(hop) ;
	    if ((op->flp = new(nothrow) vecobj) != np) ylikely {
	        if ((op->elp = new(nothrow) vecobj) != np) ylikely {
		    rs = SR_OK ;
	        } /* end if (new-vecobj) */
		if (rs < 0) {
		    delete op->flp ;
		    op->flp = nullptr ;
		} /* end if (error) */
	    } /* end if (new-vecobj) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (dialtab_ctor) */

local int dialtab_dtor(DT *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->elp) ylikely {
		delete op->elp ;
		op->elp = nullptr ;
	    }
	    if (op->flp) ylikely {
		delete op->flp ;
		op->flp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (dialtab_dtor) */

template<typename ... Args>
local inline int dialtab_magic(DT *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == DT_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (dialtab_magic) */

local int	dialtab_fileload(DT *,DT_FI *,cchar *,DT_FI **) noex ;
local int	dialtab_filedump(DT *,int) noex ;
local int	dialtab_filedel(DT *,int) noex ;

local int	file_start(DT_FI *,cchar *) noex ;
local int	file_finish(DT_FI *) noex ;

local int	entry_start(DT_ENT *,int,cchar *,int) noex ;
local int	entry_enough(DT_ENT *) noex ;
local int	entry_finish(DT_ENT *) noex ;


/* local variables */

constexpr fieldterminit		ft("#,:=") ;	/* # , : = */

enum dialkeys {
	dialkey_uucp,
	dialkey_inet,
	dialkey_username,
	dialkey_password,
	dialkey_overlast
} ; /* end enum */

constexpr cpcchar	dialnames[] = {
	"uucp",
	"inet",
	"username",
	"password",
	nullptr
} ; /* end array (dialnames) */


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
		    op->magval = DT_MAGIC ;
	            if (dialfname) {
	                rs = dialtab_fileadd(op,dialfname) ;
			c = rs ;
	            } /* end if */
		    if (rs < 0) {
			op->magval = 0 ;
			vecobj_finish(op->elp) ;
		    } /* end if (error) */
		} /* end if (vecobj_start) */
		if (rs < 0) {
		    vecobj_finish(op->flp) ;
		} /* end if (error) */
	    } /* end if (vecobj_start) */
	    if (rs < 0) {
		dialtab_dtor(op) ;
	    } /* end if (error) */
	} /* end if (dialtab_ctor) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (dialtab_open) */

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
	    op->magval = 0 ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (dialtab_close) */

enum orgloccos {
	adderco_start,
	adderco_finish,
	adderco_overlast
} ; /* end enum */

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
	int istart	() noex ;
	int ifinish	() noex ;
	int reader	(DT_FI *,cchar *,int) noex ;
	int procln	(int,cchar *,int) noex ;
	int remainder	() noex ;
    } ; /* end struct (adder) */
} /* end namespace */

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
                    } /* end if (reader) */
                } /* end block */
                if (rs < 0) {
                    dialtab_filedump(op,fi) ;
                    dialtab_filedel(op,fi) ;
                } /* end if (error) */
            } /* end if (dialtab_fileload) */
            rs1 = finish ;
            if (rs >= 0) rs = rs1 ;
        } /* end if (start-finish) */
    	return (rs >= 0) ? c : rs ;
} /* end method (adder::operator) */

int adder::reader(DT_FI *fep,cchar *fn,int fi) noex {
    	cnullptr	np{} ;
    	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (char *lbuf ; (rs = mem.ml(&lbuf)) >= 0) {
	    cint	llen = rs ;
	    cchar	*os = "r" ;
	    cmode	om = 0 ;
	    if (bfile b ; (rs = bopen(&b,fn,os,om)) >= 0) {
		cint	cmd = BC_STAT ;
		if (ustat sb ; (rs = bcontrol(&b,cmd,&sb)) >= 0) {
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
			if (rs < 0) break ;
	            } /* end while (bfile_readln) */
		} /* end if (bcontrol) */
	        rs1 = bclose(&b) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (bfile) */
	    rs1 = mem.free(lbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? c : rs ;
} /* end method (adder::reader) */

int adder::procln(int fi,cchar *lp,int ll) noex {
    	int		rs ;
	int		rs1 ;
	int		c = 0 ;
        if (field fsb ; (rs = fsb.start(lp,ll)) >= 0) {
	    int		fl ;
            if (cchar *fp{} ; (fl = fsb.get(ft.terms,&fp)) > 0) {
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
		    } /* end if (ok) */
                } else {
                    if (int ki ; (ki = matostr(dialnames,2,fp,fl)) >= 0) {
                        if ((fl = fsb.get(ft.terms,&fp)) > 0) {
			    cchar **rpp = nullptr ;
                            switch (ki) {
                            case dialkey_uucp:
                                rpp = &de.uucp ;
                                break ;
                            case dialkey_inet:
                                rpp = &de.inet ;
                                break ;
                            case dialkey_username:
                                rpp = &de.username ;
                                break ;
                            case dialkey_password:
                                rpp = &de.password ;
                                break ;
                            } /* end switch */
			    if (rpp) {
				cchar *cp ;
				if ((rs = mem.strw(fp,fl,&cp)) >= 0) {
				    *rpp = cp ;
				} /* end if (memory-acquire) */
			    } /* end if */
                        } /* end if (got value for this key) */
                    } /* end if (got a valid key) */
                } /* end if */
            } /* end if (non-zero-length field) */
            rs1 = fsb.finish ;
            if (rs >= 0) rs = rs1 ;
        } /* end if (field) */
    	return (rs >= 0) ? c : rs ;
} /* end method (adder::procln) */

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
} /* end method (adder::remainder) */

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
} /* end subroutine (dialtab_fileadd) */

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
} /* end subroutine (dialtab_search) */


/* private subroutines */

local int dialtab_fileload(DT *op,DT_FI *fep,cchar *fn,DT_FI **rpp) noex {
    	int		rs = SR_BUGCHECK ;
	int		fi = 0 ;
	if (fep && rpp) ylikely {
	    if ((rs = file_start(fep,fn)) >= 0) ylikely {
		vecobj	*flp = op->flp ;
	        if ((rs = flp->add(fep)) >= 0) ylikely {
	            fi = rs ;
    		    if (void *vp{} ; (rs = flp->get(fi,&vp)) >= 0) ylikely {
			DT_FI	*rp = (DT_FI *) vp ;
		        *rpp = rp ;
		    } /* end if */
		    if (rs < 0) {
		        flp->del(fi) ;
		    } /* end if (error) */
	        } /* end fi (vecobj_add) */
	        if (rs < 0) {
		    file_finish(fep) ;
	        } /* end if (error) */
	    } /* end if (file_start) */
	} /* end if (bugcheck) */
	return (rs >= 0) ? fi : rs ;
} /* end subroutine (dialtab_fileload) */

local int dialtab_filedump(DT *op,int fi) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) ylikely {
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
	            } /* end if */
	        } /* end if (non-null) */
	    } /* end for */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (dialtab_filedump) */

local int dialtab_filedel(DT *op,int fi) noex {
	int		rs  SR_BUGCHECK ;
	int		rs1 ;
	if (op && (fi >= 0)) ylikely {
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
} /* end subroutine (dialtab_filedel) */

local int file_start(DT_FI *fep,cchar *fname) noex {
	int		rs = SR_FAULT ;
	if (fep && fname) ylikely {
	    memclear(fep) ;
	    if (cchar *cp ; (rs = mem.strw(fname,-1,&cp)) >= 0) ylikely {
	        fep->fname = cp ;
	    } /* end if (memory-acquire) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (file_start) */

local int file_finish(DT_FI *fep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (fep) ylikely {
	    rs = SR_OK ;
	    if (fep->fname) ylikely {
	        voidp vp = voidp(fep->fname) ;
	        rs1 = mem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        fep->fname = nullptr ;
	    } /* end if (memory-acquire) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (file_finish) */

local int entry_start(DT_ENT *dep,int fi,cchar *sp,int sl) noex {
	int		rs = SR_BUGCHECK ;
	if (dep && sp && (fi >= 0)) {
	    memclear(dep) ;
	    if (cchar *cp ; (rs = mem.strw(sp,sl,&cp)) >= 0) {
	        dep->name = cp ;
		dep->fi = fi ;
	    } /* end if (memory-acquire) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (entry_start) */

local int entry_enough(DT_ENT *dep) noex {
    	int		rs = SR_FAULT ;
	int		fret = false ;
	if (dep) ylikely {
	    rs = SR_OK ;
	    if (dep->name && dep->name[0]) ylikely {
	        fret = fret || (dep->uucp && (dep->uucp[0] != '\0')) ;
	        fret = fret || (dep->inet && (dep->inet[0] != '\0')) ;
	        fret = fret || (dep->username && (dep->username[0] != '\0')) ;
	        fret = fret || (dep->password && (dep->password[0] != '\0')) ;
	    } /* end if */
	} /* end if (non-null) */
	return (rs >= 0) ? fret : rs ;
} /* end subroutine (entry_enough) */

namespace {
    struct memfreer {
	int	rs = SR_OK ;
	int	rs1 ;
	void operator () (ccharp &p) noex {
	    if (p) ylikely {
		voidp vp = voidp(p) ;
	        rs1 = mem.free(vp) ;
		if (rs >= 0) rs = rs1 ;
		p = nullptr ;
	    } /* end if (memory-release) */
	} ; /* end method */
	operator int () const noex {
	    return rs ;
	} ; /* end method */
    } ; /* end struct */
} /* end namespace */

local int entry_finish(DT_ENT *dep) noex {
    	int		rs = SR_FAULT ;
	if (dep) ylikely {
	    rs = SR_BUGCHECK ;
	    if (dep->name) ylikely {
		memfreer fo ;
		fo(dep->name) ;
		fo(dep->uucp) ;
		fo(dep->inet) ;
		fo(dep->username) ;
		fo(dep->password) ;
		rs = fo ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (entry_finish) */

int adder::istart() noex {
	return SR_OK ;
} /* end method */

int adder::ifinish() noex {
	return SR_OK ;
} /* end method */

adder_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case adderco_start:
		rs = op->istart() ;
		break ;
	    case adderco_finish:
		rs = op->ifinish() ;
		break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (adder_co::operator) */


