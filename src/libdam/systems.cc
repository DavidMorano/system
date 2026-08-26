/* systems SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get machine dialing information from UUCP "Systems" DB */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was adopted for use from the DWD program.

	- 2003-11-04, David A­D­ Morano
	I do not know where all this has been (apparently "around").

*/

/* Copyright © 1998,2003 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	systems

	Description:
	This object is a container for "systems" listed in "systems"
	files.  These are files that are keyed by the name of a
	"system" (whatever that is) and has a composite value that
	is one or more strings.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<ctime>			/* CSTD */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<algorithm>		/* C++STD |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<bufsizeget.h>		/* LIBUC */
#include	<getpwd.h>		/* LIBUC */
#include	<absfn.h>		/* LIBUC */
#include	<mkpathx.h>		/* LIBUC */
#include	<vecobj.h>		/* LIBUC */
#include	<field.h>		/* LIBUC */
#include	<ismatstar.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<bfile.h>		/* LIBB */

#include	"systems.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	SYS_FILE	systems_file
#define	CUR		systems_cur
#define	ENT		systems_ent

#define	TI_FILECHECK	3

#define	FILELINEMULT	6
#define	FILEARGSMULT	6


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct vars {
	int		maxpathlen ;
	int		maxlinelen ;
	int		filelinelen ;
	int		fileargslen ;
	operator int () noex ;
    } ; /* end struct (vars) */
} /* end namespace */

struct systems_file {
	cchar		*fname ;
	time_t		timod ;
	size_t		fsize ;
	dev_t		dev ;
	ino_t		ino ;
} ; /* end struct */

typedef systems_file *	filep ;


/* forward references */

template<typename ... Args>
local int systems_ctor(systems *op,Args ... args) noex {
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = SR_NOMEM ;
	    if ((op->flp = new(nothrow) vecobj) != np) ylikely {
	        if ((op->flp = new(nothrow) vecobj) != np) ylikely {
		    rs = SR_OK ;
	        } /* end if (new-vecobj) */
		if (rs < 0) {
		    delete op->flp ;
		    op->flp = nullptr ;
		}
	    } /* end if (new-vecobj) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (systems_ctor) */

local int systems_dtor(systems *op) noex {
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
} /* end subroutine (systems_dtor) */

template<typename ... Args>
local inline int systems_magic(systems *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == SYSTEMS_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (systems_magic) */

local int systems_fileparse(systems *,int,SYS_FILE *) noex ;
local int systems_delfes(systems *,int) noex ;

local int file_start(SYS_FILE *,cchar *) noex ;
local int file_finish(SYS_FILE *) noex ;

local int entry_start(ENT *,int,cchar *,int) noex ;
local int entry_dialer(ENT *,cchar *,int) noex ;
local int entry_args(ENT *,cchar *,int) noex ;
local int entry_finish(ENT *) noex ;


/* local variables */

static vars		var ;


/* entry field terminators */
constexpr cchar		fterms[32] = {
	0x00, 0x00, 0x00, 0x00,
	0x08, 0x10, 0x00, 0x24,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ;

/* entry argument terminators (just '#' to provide "remainder" function) */
constexpr cchar		remterms[] = {
	0x00, 0x00, 0x00, 0x00,
	0x08, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ; /* end array */


/* exported variables */


/* exported subroutines */

int systems_open(systems *op,cchar *sysfname) noex {
	int		rs ;
	if ((rs = systems_ctor(op)) >= 0) ylikely {
	    static cint		rsv = var ;
	    if ((rs = rsv) >= 0) ylikely {
	        int	sz = szof(SYS_FILE) ;
	        int	vn = 10 ;
	        int	vo = vecobjm.reuse ;
	        if ((rs = vecobj_start(op->flp,sz,vn,vo)) >= 0) ylikely {
	            sz = szof(ENT) ;
		    vn = 20 ;
	            vo = 0 ;
	            if ((rs = vecobj_start(op->elp,sz,vn,vo)) >= 0) ylikely {
	                op->magval = SYSTEMS_MAGIC ;
	                if (sysfname) {
	                    rs = systems_fileadd(op,sysfname) ;
	                }
	                if (rs < 0) {
		            op->magval = 0 ;
		            vecobj_finish(op->elp) ;
	                } /* end if (error) */
	            }
	            if (rs < 0) {
	               vecobj_finish(op->flp) ;
	            } /* end if (error) */
	        } /* end if (vecobj_start) */
	    } /* end if (vars) */
	    if (rs < 0) {
		systems_dtor(op) ;
	    } /* end if (error) */
	} /* end if (systems_ctor) */
	return rs ;
} /* end subroutine (systems_open) */

int systems_close(systems *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = systems_magic(op)) >= 0) ylikely {
	    vecobj	*elp = op->elp ;
	    vecobj	*flp = op->flp ;
	    void	*vp{} ;
	    for (int i = 0 ; elp->get(i,&vp) >= 0 ; i += 1) {
	        ENT	*dep = (ENT *) vp ;
	        if (vp) {
	            rs1 = entry_finish(dep) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	    } /* end for */
	    {
	        rs1 = elp->finish ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    /* free up the files */
	    for (int i = 0 ; flp->get(i,&vp) >= 0 ; i += 1) {
	        SYS_FILE	*fep = filep(vp) ;
	        if (vp) {
	            if (fep->fname) {
	                vp = voidp(fep->fname) ;
	                rs1 = lm_free(vp) ;
		        if (rs >= 0) rs = rs1 ;
		        fep->fname = nullptr ;
	            }
	        }
	    } /* end for */
	    {
	        rs1 = flp->finish ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = systems_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (systems_close) */

int systems_fileadd(systems *op,cchar *sysfname) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = systems_magic(op,sysfname)) >= 0) ylikely {
	    cchar	*sp ;
	    if (absfn sfn ; (rs = sfn.start(sysfname,-1,&sp)) >= 0) ylikely {
	        if (SYS_FILE fe ; (rs = file_start(&fe,sp)) >= 0) ylikely {
		    vecobj	*flp = op->flp ;
	            if ((rs = flp->add(&fe)) >= 0) ylikely {
	                cint	fi = rs ;
		        if (void *vp{} ; (rs = flp->get(fi,&vp)) >= 0) {
			    SYS_FILE	*fep = filep(vp) ;
			    {
	                        rs = systems_fileparse(op,fi,fep) ;
	                        if ((rs < 0) && (rs != SR_EXIST)) {
		                    flp->del(fi) ;
	                        }
			    }
	                }
	                if (rs < 0) {
		            flp->del(fi) ;
		        } /* end if (error) */
	            } /* end if (vecobj_add) */
	            if (rs < 0) {
		        file_finish(&fe) ;
		    } /* end if (error) */
	        } /* end if (file_start) */
		rs1 = sfn.finish ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (ok) */
	} /* end if (magic) */
	return rs ;
} /* end subroutine (systems_fileadd) */

int systems_curbegin(systems *op,CUR *curp) noex {
    	int		rs ;
	if ((rs = systems_magic(op,curp)) >= 0) ylikely {
	    curp->i = -1 ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (systems_curbegin) */

int systems_curend(systems *op,CUR *curp) noex {
    	int		rs ;
	if ((rs = systems_magic(op)) >= 0) ylikely {
	    curp->i = -1 ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (systems_curend) */

int systems_curenum(systems *op,CUR *curp,ENT **depp) noex {
	int		rs ;
	int		ei = 0 ;
	if ((rs = systems_magic(op,curp,depp)) >= 0) ylikely {
	    vecobj	*elp = op->elp ;
	    ei = (curp->i < 0) ? 0 : (curp->i + 1) ;
	    if (void *vp{} ; (rs = elp->get(ei,&vp)) >= 0) {
	        *depp = (ENT *) vp ;
	        curp->i = ei ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? ei : rs ;
} /* end subroutine (systems_curenum) */

int systems_fetch(systems *op,cchar *name,CUR *curp,ENT **depp) noex {
	int		rs ;
	int		ei = 0 ;
	if ((rs = systems_magic(op,name,curp,depp)) >= 0) ylikely {
	    vecobj	*elp = op->elp ;
	    void	*vp{} ;
	    ei = (curp->i < 0) ? 0 : (curp->i + 1) ;
	    while ((rs = elp->get(ei,&vp)) >= 0) {
	        *depp = (ENT *) vp ;
	        if (vp) {
	            if (ismatstar((*depp)->sysname,name)) {
	                break ;
	            }
	            ei += 1 ;
	        }
	    } /* end while */
	    if (rs >= 0) {
	        curp->i = ei ;
	    } /* end if (ok) */
	} /* end if (magic) */
	return (rs >= 0) ? ei : rs ;
} /* end subroutine (systems_fetch) */

int systems_check(systems *op,time_t dt) noex {
	int		rs ;
	int		c = 0 ;
	if (dt <= 0) dt = getustime ;
	if ((rs = systems_magic(op)) >= 0) ylikely {
	    /* should we even check? */
	    if ((dt - op->checktime) > TI_FILECHECK) {
		vecobj	*flp = op->flp ;
	        ustat	sb ;
	        void	*vp{} ;
	        op->checktime = dt ;
	        for (int i = 0 ; flp->get(i,&vp) >= 0 ; i += 1) {
	            SYS_FILE	*fep = filep(vp) ;
	            if (vp) {
	        	bool	fdel = false ;
	                if ((rs = u_stat(fep->fname,&sb)) >= 0) {
		            csize	fsize = size_t(sb.st_size) ;
			    rs = SR_OK ;
			    fdel = fdel || (sb.st_mtime > fep->timod) ;
			    fdel = fdel || (fsize != fep->fsize) ;
			} else if (isNotPresent(rs)) {
			    rs = SR_OK ;
			    fdel = true ;
			}
	                if ((rs >= 0) && fdel) {
	                    c += 1 ;
	                    systems_delfes(op,i) ;
	                    if (rs >= 0) {
	                        systems_fileparse(op,i,fep) ;
		            }
	                } /* end if (file changed) */
		    } /* end if (non-null) */
	        } /* end for */
	    } /* end if (needed) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (systems_check) */


/* private subroutines */

namespace {
    struct parser {
	systems	*op ;
	char	*lbuf ;
	char	*abuf ;
	int	llen ;
	int	alen ;
	int	fi ; /* file-index */
	parser(systems *p,int i) noex : op(p), fi(i) { } ;
	int operator () (SYS_FILE *) noex ;
	int parse(SYS_FILE *) noex ;
	int parsealready(dev_t,ino_t) noex ;
	int parseln(field *) noex ;
    } ; /* end struct (parser) */
} /* end namespace */

int parser::operator () (SYS_FILE *fep) noex {
	cint		sz = (var.filelinelen + var.fileargslen + 2) ;
    	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (char *a ; (rs = lm_mall(sz,&a)) >= 0) ylikely {
	    lbuf = a ;
	    llen = var.filelinelen ;
	    abuf = (a + (var.filelinelen + 1)) ;
	    alen = var.fileargslen ;
	    {
		rs = parse(fep) ;
		c = rs ;
	    }
	    rs1 = lm_free(a) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? c : rs ;
} /* end method */

local int systems_fileparse(systems *op,int fi,SYS_FILE *fep) noex {
	parser	po(op,fi) ;
       	return po(fep) ;
} /* end subroutine */

int parser::parse(SYS_FILE *fep) noex {
    	cnullptr	np{} ;
	bfile		dialfile, *sfp = &dialfile ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = bopen(sfp,fep->fname,"r",0664)) >= 0) ylikely {
	    if (ustat sb ; (rs = bcontrol(sfp,BC_STAT,&sb)) >= 0) ylikely {
		const dev_t	dev = sb.st_dev ;
		const ino_t	ino = sb.st_ino ;
		if ((rs = parsealready(dev,ino)) == 0) {
                    field   fsb ;
                    cint    to = -1 ;
                    fep->timod = sb.st_mtime ;
                    fep->fsize = size_t(sb.st_size & UINT_MAX) ;
                    fep->dev = sb.st_dev ;
                    fep->ino = sb.st_ino ;
                    while ((rs = breadlns(sfp,lbuf,llen,to,np)) > 0) {
                        cint	len = rs ;
                        if ((len <= 1) || (lbuf[0] == '#')) continue ;
                        if ((rs = fsb.start(lbuf,len)) >= 0) {
                            cchar   *fp ;
                            if (int fl ; (fl = fsb.get(fterms,&fp)) >= 0) {
                                if (fsb.term != '#') {
                                    rs = parseln(&fsb) ;
                                    if (rs > 0) c += 1 ;
                                } /* end if */
                            }
                            rs1 = fsb.finish ;
                            if (rs >= 0) rs = rs1 ;
                        } /* end if (field) */
                        if (rs < 0) break ;
                    } /* end while (reading lines) */
	        } /* end if (not-already) */
	    } /* end if (bcontrol) */
	    rs1 = bclose(sfp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (file) */
	return (rs >= 0) ? c : rs ;
} /* end method (parser::parse) */

int parser::parsealready(dev_t dev,ino_t ino) noex {
	vecobj		*flp = op->flp ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		f = false ; /* return-value */
	void		*vp{} ;
	for (int i = 0 ; (rs1 = flp->get(i,&vp)) >= 0 ; i += 1) {
	    SYS_FILE	*fep = filep(vp) ;
	    if (vp) {
		f = ((fep->dev == dev) && (fep->ino == ino)) ;
		if (f) break ;
	    }
	} /* end for */
	if ((rs >= 0) && (rs1 != SR_NOTFOUND)) rs = rs1 ;
	return (rs >= 0) ? f : rs ;
} /* end method (parser::parsealready) */

int parser::parseln(field *fsp) noex {
	int		rs = SR_OK ;
	int		f = false ;
	if ((fsp->fl > 0) && (fsp->term != '#')) {
	    int		fl = fsp->fl ;
	    cchar	*fp = fsp->fp ;
	    if (ENT e ; (rs = entry_start(&e,fi,fp,fl)) >= 0) ylikely {
		bool	f_fin = true ;
	        if ((fl = fsp->get(fterms,&fp)) > 0) {
	            if ((rs = entry_dialer(&e,fp,fl)) >= 0) {
	                if (fsp->term != '#') {
			    cchar	*ft = remterms ;
	                    if ((rs = fsp->srvarg(ft,abuf,alen)) >= 0) {
	                	cint	al = rs ;
	                	if ((rs = entry_args(&e,abuf,al)) >= 0) {
				    vecobj	*elp = op->elp ;
	                    	    if ((rs = elp->add(&e)) >= 0) {
					f = true ;
					f_fin = false ;
				    }
			        }
	             	    } /* end if (field_srvarg) */
	                } /* end if (not a comment) */
	            } /* end if (entry_dialer) */
	        } /* end if (field_get) */
	        if ((rs < 0) || f_fin) {
		    entry_finish(&e) ;
		} /* end if (error) */
	    } /* end if (entry_start) */
	} /* end if (possible) */
	return (rs >= 0) ? f : rs ;
} /* end method (parser::parseln) */

local int systems_delfes(systems *op,int fi) noex {
	vecobj		*elp = op->elp ;
	int		rs = SR_OK ;
	int		rs1 ;
	void		*vp{} ;
	for (int i = 0 ; elp->get(i,&vp) >= 0 ; i += 1) {
	    ENT		*ep = (ENT *) vp ;
	    if (vp) {
	        if (ep->fi == fi) {
		    {
	                rs1 = entry_finish(ep) ;
		        if (rs >= 0) rs = rs1 ;
		    }
		    {
	                rs1 = elp->del(i--) ;
		        if (rs >= 0) rs = rs1 ;
		    }
	        }
	    }
	} /* end for */
	return rs ;
} /* end subroutine (systems_delfes) */

local int file_start(SYS_FILE *fep,cchar *fname) noex {
	int		rs = SR_FAULT ;
	if (fep && fname) ylikely {
	    memclear(fep) ;
	    if (cchar *cp ; (rs = lm_strw(fname,-1,&cp)) >= 0) ylikely {
	        fep->fname = cp ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (file_start) */

local int file_finish(SYS_FILE *fep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (fep) ylikely {
	    rs = SR_OK ;
	    if (fep->fname) ylikely {
		void *vp = voidp(fep->fname) ;
	        rs1 = lm_free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        fep->fname = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (file_finish) */

local int entry_start(ENT *ep,int fi,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (ep) ylikely {
	    memclear(ep) ;
	    ep->fi = fi ;
	    if (cchar *cp ; (rs = lm_strw(sp,sl,&cp)) >= 0) ylikely {
	        ep->sysnamelen = sl ;
	        ep->sysname = cp ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (entry_start) */

local int entry_dialer(ENT *ep,cchar *dp,int dl) noex {
	int		rs ;
	if (cchar *cp ; (rs = lm_strw(dp,dl,&cp)) >= 0) ylikely {
	    ep->dialernamelen = dl ;
	    ep->dialername = cp ;
	}
	return rs ;
} /* end subroutine (entry_dialer) */

local int entry_args(ENT *ep,cchar *argp,int argl) noex {
	int		rs = SR_OK ;
	if (argl > 0) {
	    ep->dialerargslen = argl ;
	    if (cchar *cp ; (rs = lm_strw(argp,argl,&cp)) >= 0) ylikely {
		ep->dialerargs = cp ;
	    }
	} /* end if */
	return (rs >= 0) ? argl : rs ;
} /* end subroutine (entry_args) */

local int entry_finish(ENT *ep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (ep) ylikely {
	    rs = SR_OK ;
	    if (ep->sysnamelen > 0) ylikely {
		void *vp ;
	        if (ep->dialerargs) ylikely {
	            vp = voidp(ep->dialerargs) ;
	            rs1 = lm_free(vp) ;
	            if (rs >= 0) rs = rs1 ;
		    ep->dialerargs = nullptr ;
	        } /* end if (memory-release) */
	        if (ep->dialername) ylikely {
	            vp = voidp(ep->dialername) ;
	            rs1 = lm_free(vp) ;
	            if (rs >= 0) rs = rs1 ;
	            ep->dialername = nullptr ;
	        } /* end if (memory-release) */
	        if (ep->sysname) ylikely {
	            vp = voidp(ep->sysname) ;
	            rs1 = lm_free(vp) ;
	            if (rs >= 0) rs = rs1 ;
	            ep->sysname = nullptr ;
	        } /* end if (memory-release) */
	        ep->sysnamelen = 0 ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (entry_finish) */

vars::operator int () noex {
    	int		rs ;
	if ((rs = bufsizeget(bufsize_mp)) >= 0) ylikely {
	    maxpathlen = rs ;
	    if ((rs = bufsizeget(bufsize_ml)) >= 0) ylikely {
		maxlinelen = rs ;
		filelinelen = (maxlinelen * FILELINEMULT) ;
		fileargslen = (maxpathlen * FILEARGSMULT) ;
	    }
	}
	return rs ;
} /* end method (vars::operator) */


