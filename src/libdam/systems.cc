/* systems SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* get machine dialing information from UUCP "Systems" DB */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was adopted for use from the DWD program.

	- 2003-11-04, David A­D­ Morano
	I don't know where all this has been (apparently "around")
	but I grabbed it from the SYSTEMS object!

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
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<getbufsize.h>
#include	<getpwd.h>
#include	<absfn.h>
#include	<mkpathx.h>
#include	<vecobj.h>
#include	<bfile.h>
#include	<field.h>
#include	<ismatstar.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"systems.h"


/* local defines */

#define	SYS_FILE	systems_file
#define	CUR		systems_cur
#define	ENT		systems_ent

#define	TI_FILECHECK	3

#define	FILELINEMULT	6
#define	FILEARGSMULT	6


/* imported namespaces */

using std::nullptr_t ;			/* type */
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
}

struct systems_file {
	cchar		*fname ;
	dev_t		dev ;
	ino_t		ino ;
	time_t		timod ;
	size_t		fsize ;
} ;

typedef systems_file *	filep ;


/* forward references */

template<typename ... Args>
static int systems_ctor(systems *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    if ((op->flp = new(nothrow) vecobj) != np) {
	        if ((op->flp = new(nothrow) vecobj) != np) {
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
/* end subroutine (systems_ctor) */

static int systems_dtor(systems *op) noex {
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
/* end subroutine (systems_dtor) */

template<typename ... Args>
static inline int systems_magic(systems *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == SYSTEMS_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (systems_magic) */

static int systems_fileparse(systems *,int,SYS_FILE *) noex ;
static int systems_delfes(systems *,int) noex ;

static int file_start(SYS_FILE *,cchar *) noex ;
static int file_finish(SYS_FILE *) noex ;

static int entry_start(ENT *,int,cchar *,int) noex ;
static int entry_dialer(ENT *,cchar *,int) noex ;
static int entry_args(ENT *,cchar *,int) noex ;
static int entry_finish(ENT *) noex ;


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
constexpr cchar		remterms[32] = {
	0x00, 0x00, 0x00, 0x00,
	0x08, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ;


/* exported variables */


/* exported subroutines */

int systems_open(systems *op,cchar *sysfname) noex {
	int		rs ;
	if ((rs = systems_ctor(op)) >= 0) {
	    static cint		rsv = var ;
	    if ((rs = rsv) >= 0) {
	        int	sz = szof(SYS_FILE) ;
	        int	vn = 10 ;
	        int	vo = VECOBJ_OREUSE ;
	        if ((rs = vecobj_start(op->flp,sz,vn,vo)) >= 0) {
	            sz = szof(ENT) ;
		    vn = 20 ;
	            vo = 0 ;
	            if ((rs = vecobj_start(op->elp,sz,vn,vo)) >= 0) {
	                op->magic = SYSTEMS_MAGIC ;
	                if (sysfname) {
	                    rs = systems_fileadd(op,sysfname) ;
	                }
	                if (rs < 0) {
		            op->magic = 0 ;
		            vecobj_finish(op->elp) ;
	                }
	            }
	            if (rs < 0) {
	               vecobj_finish(op->flp) ;
	            }
	        } /* end if (vecobj_start) */
	    } /* end if (vars) */
	    if (rs < 0) {
		systems_dtor(op) ;
	    }
	} /* end if (systems_ctor) */
	return rs ;
}
/* end subroutine (systems_open) */

int systems_close(systems *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = systems_magic(op)) >= 0) {
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
	                rs1 = uc_free(fep->fname) ;
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
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (systems_close) */

int systems_fileadd(systems *op,cchar *sysfname) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = systems_magic(op,sysfname)) >= 0) {
	    cchar	*sp ;
	    if (absfn sfn ; (rs = sfn.start(sysfname,-1,&sp)) >= 0) {
	        if (SYS_FILE fe ; (rs = file_start(&fe,sp)) >= 0) {
		    vecobj	*flp = op->flp ;
	            if ((rs = flp->add(&fe)) >= 0) {
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
		        }
	            } /* end if (vecobj_add) */
	            if (rs < 0) {
		        file_finish(&fe) ;
		    }
	        } /* end if (file_start) */
		rs1 = sfn.finish ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (ok) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (systems_fileadd) */

int systems_curbegin(systems *op,CUR *curp) noex {
    	int		rs ;
	if ((rs = systems_magic(op,curp)) >= 0) {
	    curp->i = -1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (systems_curbegin) */

int systems_curend(systems *op,CUR *curp) noex {
    	int		rs ;
	if ((rs = systems_magic(op)) >= 0) {
	    curp->i = -1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (systems_curend) */

int systems_curenum(systems *op,CUR *curp,ENT **depp) noex {
	int		rs ;
	int		ei = 0 ;
	if ((rs = systems_magic(op,curp,depp)) >= 0) {
	    vecobj	*elp = op->elp ;
	    ei = (curp->i < 0) ? 0 : (curp->i + 1) ;
	    if (void *vp{} ; (rs = elp->get(ei,&vp)) >= 0) {
	        *depp = (ENT *) vp ;
	        curp->i = ei ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (systems_curenum) */

int systems_fetch(systems *op,cchar *name,CUR *curp,ENT **depp) noex {
	int		rs ;
	int		ei = 0 ;
	if ((rs = systems_magic(op,name,curp,depp)) >= 0) {
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
	    }
	} /* end if (magic) */
	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (systems_fetch) */

int systems_check(systems *op,time_t dt) noex {
	int		rs ;
	int		c = 0 ;
	if (dt <= 0) dt = getustime ;
	if ((rs = systems_magic(op)) >= 0) {
	    /* should we even check? */
	    if ((dt - op->checktime) > TI_FILECHECK) {
		vecobj	*flp = op->flp ;
	        USTAT	sb ;
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
}
/* end subroutine (systems_check) */


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
}

int parser::operator () (SYS_FILE *fep) noex {
	cint		sz = (var.filelinelen + var.fileargslen + 2) ;
    	int		rs ;
	int		c = 0 ;
	if (char *a{} ; (rs = uc_malloc(sz,&a)) >= 0) {
	    lbuf = a ;
	    llen = var.filelinelen ;
	    abuf = (a + (var.filelinelen + 1)) ;
	    alen = var.fileargslen ;
	    {
		rs = parse(fep) ;
		c = rs ;
	    }
	    rs = rsfree(rs,a) ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? c : rs ;
}

static int systems_fileparse(systems *op,int fi,SYS_FILE *fep) noex {
	parser po(op,fi) ;
       	return po(fep) ;
}

int parser::parse(SYS_FILE *fep) noex {
    	cnullptr	np{} ;
	bfile		dialfile, *sfp = &dialfile ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = bopen(sfp,fep->fname,"r",0664)) >= 0) {
	    if (USTAT sb ; (rs = bcontrol(sfp,BC_STAT,&sb)) >= 0) {
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
                            int     fl ;
                            cchar   *fp ;
                            if ((fl = fsb.get(fterms,&fp)) >= 0) {
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
}
/* end method (parser::parse) */

int parser::parsealready(dev_t dev,ino_t ino) noex {
	vecobj		*flp = op->flp ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		f = false ;
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
}
/* end method (parser::parsealready) */

int parser::parseln(field *fsp) noex {
	int		rs = SR_OK ;
	int		f = false ;
	if ((fsp->fl > 0) && (fsp->term != '#')) {
	    int		fl = fsp->fl ;
	    cchar	*fp = fsp->fp ;
	    if (ENT e ; (rs = entry_start(&e,fi,fp,fl)) >= 0) {
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
		}
	    } /* end if (entry_start) */
	} /* end if (possible) */
	return (rs >= 0) ? f : rs ;
}
/* end method (parser::parseln) */

static int systems_delfes(systems *op,int fi) noex {
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
}
/* end subroutine (systems_delfes) */

static int file_start(SYS_FILE *fep,cchar *fname) noex {
	int		rs = SR_FAULT ;
	if (fep && fname) {
	    memclear(fep) ;
	    if (cchar *cp{} ; (rs = uc_mallocstrw(fname,-1,&cp)) >= 0) {
	        fep->fname = cp ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (file_start) */

static int file_finish(SYS_FILE *fep) noex {
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

static int entry_start(ENT *ep,int fi,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (ep) {
	    memclear(ep) ;
	    ep->fi = fi ;
	    if (cchar *cp ; (rs = uc_mallocstrw(sp,sl,&cp)) >= 0) {
	        ep->sysnamelen = sl ;
	        ep->sysname = cp ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (entry_start) */

static int entry_dialer(ENT *ep,cchar *dp,int dl) noex {
	int		rs ;
	if (cchar *cp ; (rs = uc_mallocstrw(dp,dl,&cp)) >= 0) {
	    ep->dialernamelen = dl ;
	    ep->dialername = cp ;
	}
	return rs ;
}
/* end subroutine (entry_dialer) */

static int entry_args(ENT *ep,cchar *args,int argslen) noex {
	int		rs = SR_OK ;
	if (argslen > 0) {
	    ep->dialerargslen = argslen ;
	    if (cchar *cp ; (rs = uc_mallocstrw(args,argslen,&cp)) >= 0) {
		ep->dialerargs = cp ;
	    }
	} /* end if */
	return (rs >= 0) ? argslen : rs ;
}
/* end subroutine (entry_args) */

static int entry_finish(ENT *ep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (ep) {
	    rs = SR_OK ;
	    if (ep->sysnamelen > 0) {
	        if (ep->dialerargs) {
	            rs1 = uc_free(ep->dialerargs) ;
	            if (rs >= 0) rs = rs1 ;
		    ep->dialerargs = nullptr ;
	        }
	        if (ep->dialername) {
	            rs1 = uc_free(ep->dialername) ;
	            if (rs >= 0) rs = rs1 ;
	            ep->dialername = nullptr ;
	        }
	        if (ep->sysname) {
	            rs1 = uc_free(ep->sysname) ;
	            if (rs >= 0) rs = rs1 ;
	            ep->sysname = nullptr ;
	        }
	        ep->sysnamelen = 0 ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (entry_finish) */

vars::operator int () noex {
    	int		rs ;
	if ((rs = getbufsize(getbufsize_mp)) >= 0) {
	    maxpathlen = rs ;
	    if ((rs = getbufsize(getbufsize_ml)) >= 0) {
		maxlinelen = rs ;
		filelinelen = (maxlinelen * FILELINEMULT) ;
		fileargslen = (maxpathlen * FILEARGSMULT) ;
	    }
	}
	return rs ;
}
/* end method (vars::operator) */


