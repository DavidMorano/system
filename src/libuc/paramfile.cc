/* paramfile SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* read in a file of parameters */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-02-17, David A­D­ Morano
	Some code for this subroutine was taken from something that
	did something similar to what we are doing here.  The rest
	was originally written for LevoSim.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	paramfile

	Description:
	This object reads in the parameter file and makes the
	parameter pairs available thought a key search.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>		/* <- for |UCHAR_MAX| + |CHAR_BIT| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<ctime>
#include	<cstdlib>
#include	<cstring>		/* <- for |memcpy(3c)| */
#include	<new>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<netdb.h>
#include	<usystem.h>
#include	<ulogerror.h>
#include	<getpwd.h>
#include	<bufsizevar.hh>
#include	<mallocxx.h>
#include	<ascii.h>
#include	<bfile.h>
#include	<field.h>
#include	<fieldterms.h>
#include	<vecobj.h>
#include	<vecstr.h>
#include	<buffer.h>
#include	<sncpyx.h>
#include	<mkpathx.h>
#include	<mkpathrooted.h>
#include	<sfx.h>
#include	<strwcpy.h>
#include	<char.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"paramfile.h"

import libutil ;

/* local defines */

#define	PF			paramfile
#define	PF_F			paramfile_file
#define	PF_C			paramfile_cur
#define	PF_E			paramfile_ent
#define	VS			vecstr


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */

struct paramfile_file ;		/* <- needed for the typedef ("filep") below */

extern "C" {
    typedef int (*vog_f)(vecobj *,int,void **) noex ;
}

typedef PF_C	*cursorp ;
typedef PF_E	*entp ;
typedef PF_F	*filep ;


/* external subroutines */


/* external variables */


/* local structures */

struct paramfile_file {
	cchar		*fname ;
	time_t		mtime ;
	size_t		fsize ;
	ino_t		ino ;
	dev_t		dev ;
} ;


/* forward references */

static int	mkterms() noex ;

template<typename ... Args>
static inline int paramfile_ctor(paramfile *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    op->envv = nullptr ;
	    op->a = nullptr ;
	    op->lbuf = nullptr ;
	    op->fbuf = nullptr ;
	    op->fl = {} ;
	    op->ti_check = 0 ;
	    op->magic = 0 ;
	    op->llen = 0 ;
	    op->flen = 0 ;
	    op->intcheck = 0 ;
	    if ((op->filep = new(nothrow) vecobj) != np) {
	        if ((op->entsp = new(nothrow) vecobj) != np) {
	            if ((op->defp = new(nothrow) varsub) != np) {
	                if ((op->envp = new(nothrow) varsub) != np) {
			    rs = SR_OK ;
			} /* end if (new-varsub) */
		        if (rs < 0) {
		            delete op->defp ;
			    op->defp = nullptr ;
		        }
		    } /* end if (new-varsub) */
		    if (rs < 0) {
		        delete op->entsp ;
			op->entsp = nullptr ;
		    }
		} /* end if (new-vecobj) */
		if (rs < 0) {
		    delete op->filep ;
		    op->filep = nullptr ;
		}
	    } /* end if (new-vecobj) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (paramfile_ctor) */

static int paramfile_dtor(paramfile *op) noex {
	int		rs = SR_OK ;
	if (op->envp) {
	    delete op->envp ;
	    op->envp = nullptr ;
	}
	if (op->defp) {
	    delete op->defp ;
	    op->defp = nullptr ;
	}
	if (op->entsp) {
	    delete op->entsp ;
	    op->entsp = nullptr ;
	}
	if (op->filep) {
	    delete op->filep ;
	    op->filep = nullptr ;
	}
	return rs ;
}
/* end subroutine (paramfile_dtor) */

template<typename ... Args>
static inline int paramfile_magic(paramfile *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == PARAMFILE_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (paramfile_magic) */

static int	paramfile_fileadder(PF *,cchar *) noex ;

static int	paramfile_bufbegin(PF *) noex ;
static int	paramfile_bufend(PF *) noex ;

static int	paramfile_filefins(PF *) noex ;
static int	paramfile_fileparse(PF *,int) noex ;
static int	paramfile_fileparser(PF *,int,bfile *) noex ;
static int	paramfile_fileparseline(PF *,int,cchar *,int) noex ;
static int	paramfile_filedump(PF *,int) noex ;
static int	paramfile_filedel(PF *,int) noex ;
static int	paramfile_entadd(PF *,PF_E *) noex ;
static int	paramfile_entfins(PF *) noex ;
static int	paramfile_entsub(PF *,PF_E *,cchar **) noex ;

static int	paramfile_envbegin(PF *) noex ;
static int	paramfile_envload(PF *) noex ;
static int	paramfile_envend(PF *) noex ;

static int	paramfile_defbegin(PF *,vecstr *) noex ;
static int	paramfile_defload(PF *,vecstr *) noex ;
static int	paramfile_defend(PF *) noex ;

static int	paramfile_entrels(PF *) noex ;

static int	file_start(paramfile_file *,cchar *) noex ;
static int	file_finish(paramfile_file *) noex ;

static int	entry_start(PF_E *,int,cchar *,int,vecstr *,int) noex ;
static int	entry_release(PF_E *) noex ;
static int	entry_finish(PF_E *) noex ;

extern "C" {
    static int	vcmpentry(cvoid **,cvoid **) noex ;
}


/* local variables */

/* key field terminators (pound, equal, and all white-space) */
static char 		kterms[fieldterms_termsize] ;

/* argument field terminators (pound and all white-space) */
static char 		aterms[fieldterms_termsize] ;

static bufsizevar	maxlinelen(getbufsize_ml) ;


/* exported variables */


/* exported subroutines */

int paramfile_open(PF *op,mainv envv,cchar *fname) noex {
	static cint	srs = mkterms() ;
	int		rs ;
	if ((rs = srs) >= 0) {
	    if ((rs = paramfile_ctor(op)) >= 0) {
		vecobj	*flp = op->filep ;
	        int	esz = szof(PF_F) ;
	        int	vo = VECOBJ_OSTATIONARY ;
	        op->envv = envv ;
	        op->intcheck = PARAMFILE_INTCHECK ;
	        op->ti_check = time(nullptr) ;
	        if ((rs = vecobj_start(flp,esz,10,vo)) >= 0) {
		    vecobj	*elp = op->entsp ;
	            vo = (VECOBJ_OCOMPACT | VECOBJ_OORDERED) ;
	            esz = szof(PF_E) ;
	            if ((rs = vecobj_start(elp,esz,10,vo)) >= 0) {
	                if (op->envv) {
	                    rs = paramfile_envbegin(op) ;
	                }
	                if (rs >= 0) {
	                    op->magic = PARAMFILE_MAGIC ;
	                    if (fname) {
	                        rs = paramfile_fileadd(op,fname) ;
	                    }
	                }
	                if (rs < 0) {
	                    paramfile_envend(op) ;
	                    vecobj_finish(op->entsp) ;
	                }
	            }
	            if (rs < 0) {
	                vecobj_finish(op->filep) ;
		    }
	        } /* end if (had an initial file to load) */
		if (rs < 0) {
		    paramfile_dtor(op) ;
		}
	    } /* end if (paramfile_ctor) */
	} /* end if (mkterms) */
	return rs ;
}
/* end subroutine (paramfile_open) */

int paramfile_close(PF *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = paramfile_magic(op)) >= 0) {
	    {
	        rs1 = paramfile_entfins(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = paramfile_filefins(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->fl.definit) {
	        op->fl.definit = false ;
	        rs1 = varsub_finish(op->defp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = paramfile_envend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = vecobj_finish(op->entsp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = vecobj_finish(op->filep) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = paramfile_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (paramfile_close) */

int paramfile_setdefines(PF *op,vecstr *dvp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = paramfile_magic(op,dvp)) >= 0) {
	    {
	        rs1 = paramfile_defend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = paramfile_entrels(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = paramfile_defbegin(op,dvp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (paramfile_setdefines) */

int paramfile_fileadd(PF *op,cchar *fname) noex {
	int		rs ;
	int		rs1 ;
	int		rc = 0 ;
	if ((rs = paramfile_magic(op,fname)) >= 0) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
	        if (fname[0] != '/') {
		    char	*tbuf{} ;
		    if ((rs = malloc_mp(&tbuf)) >= 0) {
			if ((rs = mkpathrooted(tbuf,fname)) >= 0) {
		    	    rs = paramfile_fileadder(op,tbuf) ;
		    	    rc = rs ;
			} /* end if (mkpathrooted) */
			rs1 = uc_free(tbuf) ;
			if (rs >= 0) rs = rs1 ;
		    } /* end if (m-a-f) */
		} else {
		    rs = paramfile_fileadder(op,fname) ;
		    rc = rs ;
		}
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (paramfile_fileadd) */

int paramfile_curbegin(PF *op,PF_C *curp) noex {
	int		rs ;
	if ((rs = paramfile_magic(op,curp)) >= 0) {
	    curp->i = -1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (paramfile_curbegin) */

int paramfile_curend(PF *op,PF_C *curp) noex {
	int		rs ;
	if ((rs = paramfile_magic(op,curp)) >= 0) {
	    curp->i = -1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (paramfile_curend) */

int paramfile_fetch(PF *op,cchar *ks,PF_C *curp,char *vbuf,int vlen) noex {
	int		rs ;
	int		vl = 0 ;
	if ((rs = paramfile_magic(op,ks,curp,vbuf)) >= 0) {
	    rs = SR_INVALID ;
	    if (ks[0]) {
	        vecobj	*slp = op->entsp ;
	        PF_E	*pep{} ;
	        PF_E	ke ;
	        ke.fi = -1 ;
	        ke.key = ks ;
	        if (curp == nullptr) {
	            void	*vp{} ;
	            vecobj_vcf	vcf = vecobj_vcf(vcmpentry) ;
	            rs = vecobj_search(slp,&ke,vcf,&vp) ;
	            pep = entp(vp) ;
	        } else {
		    vog_f	vget = vecobj_get ;
	            int		j = (curp->i < 0) ? 0 : (curp->i + 1) ;
	            int		i ; /* <- used-afterwards */
	            void	*vp{} ;
	            for (i = j ; (rs = vget(slp,i,&vp)) >= 0 ; i += 1) {
	                pep = entp(vp) ;
		        if (vp) {
	                    if (pep->key) {
	                        if (strcmp(ks,pep->key) == 0) break ;
		            }
		        }
	            } /* end for (looping through entries) */
	            if (rs >= 0) {
	                curp->i = i ;
	            }
	        } /* end if */
	        if ((rs >= 0) && pep) {
		    if (cc *cp ; (rs = paramfile_entsub(op,pep,&cp)) >= 0) {
	                vl = rs ;
	                if (vbuf) {
	                    rs = sncpy1(vbuf,vlen,cp) ;
		        }
	            }
	        } /* end if */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (paramfile_fetch) */

int paramfile_curenum(PF *op,PF_C *curp,PF_E *ep,char *ebuf,int elen) noex {
	int		rs ;
	int		kl = 0 ;
	if ((rs = paramfile_magic(op,curp,ep,ebuf)) >= 0) {
	    vecobj	*slp = op->entsp ;
	    int		i = (curp->i < 0) ? 0 : (curp->i + 1) ;
	    int		j{} ;
	    void	*vp{} ;
	    memclear(ep) ;
	    for (j = i ; (rs = vecobj_get(slp,j,&vp)) >= 0 ; j += 1) {
	        if (vp) break ;
	    } /* end for */
	    if ((rs >= 0) && vp) {
	        PF_E	*pep = entp(vp) ;
	        char	*bp = ebuf ;
	        cchar	*cp{} ;
	        if ((rs = paramfile_entsub(op,pep,&cp)) >= 0) {
	            cint	cl = rs ;
	            kl = pep->klen ;
	            if ((elen < 0) || (elen >= (kl + cl + 2))) {
	                ep->key = bp ;
	                ep->klen = pep->klen ;
	                bp = strwcpy(bp,pep->key,pep->klen) + 1 ;
	                ep->value = bp ;
	                ep->vlen = cl ;
	                bp = strwcpy(bp,cp,cl) + 1 ;
	            } else {
	                rs = SR_OVERFLOW ;
		    }
	        } /* end if (entry-sub) */
	    } /* end if (got one) */
	    if (rs >= 0) {
	        curp->i = j ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? kl : rs ;
}
/* end subroutine (paramfile_curenum) */

int paramfile_checkint(PF *op,int intcheck) noex {
	int		rs ;
	if ((rs = paramfile_magic(op)) >= 0) {
	    if (intcheck < 1) intcheck = 1 ;
	    op->intcheck = intcheck ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (paramfile_checkint) */

int paramfile_check(PF *op,time_t dt) noex {
	cint		to = PARAMFILE_INTCHANGE ;
	int		rs ;
	int		rs1 ;
	int		c_changed = 0 ;
	if ((rs = paramfile_magic(op)) >= 0) {
	    if (dt == 0) dt = time(nullptr) ;
	    if ((dt - op->ti_check) > op->intcheck) {
	        vecobj	*flp = op->filep ;
		void	*vp{} ;
	        op->ti_check = dt ;
	        for (int i = 0 ; vecobj_get(flp,i,&vp) >= 0 ; i += 1) {
	            if (vp) {
	    	        USTAT	sb ;
	                PF_F	*fep = (PF_F *) vp ;
	                if ((rs1 = u_stat(fep->fname,&sb)) >= 0) {
		            bool	f = (sb.st_mtime > fep->mtime) ;
	                    f = f && ((dt - sb.st_mtime) >= to) ;
		            if (f) {
	                        paramfile_filedump(op,i) ;
	                        c_changed += 1 ;
	                        rs = paramfile_fileparse(op,i) ;
	                    } /* end if */
	                }
		    } /* end if (non-null) */
	            if (rs < 0) break ;
	        } /* end for */
	    } /* end if (needed) */
	} /* end if (magic) */
	return (rs >= 0) ? c_changed : rs ;
}
/* end subroutine (paramfile_check) */


/* private subroutines */

static int paramfile_fileadder(PF *op,cchar *fpath) noex {
	int		rs ;
	PF_F		fe{} ;
	if ((rs = file_start(&fe,fpath)) >= 0) {
	    if ((rs = vecobj_add(op->filep,&fe)) >= 0) {
	        cint	fi = rs ;
	        rs = paramfile_fileparse(op,fi) ;
	        if (rs < 0)
	            paramfile_filedel(op,fi) ;
	    }
	    if (rs < 0)
	        file_finish(&fe) ;
	} /* end if (file-start) */
	return rs ;
}
/* end subroutine (paramfile_fileadder) */

static int paramfile_bufbegin(PF *op) noex {
	cint		nlines = 2 ;
	int		rs ;
	int		llen = 0 ;
	if ((rs = maxlinelen) >= 0) {
	    cint	bsize = (nlines*(rs+1)) ;
	    void	*vp{} ;
	    llen = rs ;
	    if ((rs = uc_malloc(bsize,&vp)) >= 0) {
		char	*bp = charp(vp) ;
	        op->a = charp(vp) ;
	        op->lbuf = bp ;
	        op->llen = llen ;
	        bp += (llen+1) ;
	        op->fbuf = bp ;
	        op->flen = llen ;
	    } /* end if (m-a) */
	} /* end if (maxlinelen) */
	return (rs >= 0) ? llen : rs ;
}
/* end subroutine (paramfile_bufbegin) */

static int paramfile_bufend(PF *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->a) {
	    rs1 = uc_free(op->a) ;
	    if (rs >= 0) rs = rs1 ;
	    op->a = nullptr ;
	}
	return rs ;
}
/* end subroutine (paramfile_bufend) */

static int paramfile_fileparse(PF *op,int fi) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	void		*vp{} ;
	if ((rs = vecobj_get(op->filep,fi,&vp)) >= 0) {
	    if (vp) {
	        bfile	lfile, *lfp = &lfile ;
		PF_F	*fep = nullptr ;
	        if ((rs = bopen(lfp,fep->fname,"r",0664)) >= 0) {
	            USTAT	sb ;
	            if ((rs = bcontrol(lfp,BC_STAT,&sb)) >= 0) {
	                if (fep->mtime <  sb.st_mtime) {
	                    fep->dev = sb.st_dev ;
	                    fep->ino = sb.st_ino ;
	                    fep->mtime = sb.st_mtime ;
	                    fep->fsize = size_t(sb.st_size) ;
	                    rs = paramfile_fileparser(op,fi,lfp) ;
	                    c = rs ;
	                } /* end if (needed update) */
	            } /* end if (bcontrol) */
	            rs1 = bclose(lfp) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (bfile) */
	    } else {
	        rs = SR_NOTFOUND ;
	    }
	} /* end if (vecobj_get) */
	if (rs < 0) {
		rs = paramfile_filedump(op,fi) ;
	}
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (paramfile_fileparse) */

static int paramfile_fileparser(PF *op,int fi,bfile *lfp) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = paramfile_bufbegin(op)) >= 0) {
	    cint	llen = rs ;
	    int		cl ;
	    int		f_bol = true ;
	    int		f_eol ;
	    cchar	*cp ;
	    char	*lbuf = op->lbuf ;
	    while ((rs = breadlns(lfp,lbuf,llen,-1,nullptr)) > 0) {
	        int	len = rs ;
	        if (len > 1) {
	            f_eol = (lbuf[len-1] == '\n') ;
	            if (f_eol) len -= 1 ;
	            lbuf[len] = '\0' ;
		    if ((cl = sfskipwhite(lbuf,len,&cp)) > 0) {
	                if (f_bol && (*cp != '#')) {
	                    rs = paramfile_fileparseline(op,fi,cp,cl) ;
	                    c += rs ;
		        }
	            }
		} /* end if (non-empty line) */
	        f_bol = f_eol ;
	        if (rs < 0) break ;
	    } /* end while (reading lines) */
	    rs1 = paramfile_bufend(op) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (paramfile_buf) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (paramfile_fileparser) */

static int paramfile_fileparseline(PF *op,int fi,cchar *lp,int ll) noex {
	field		fsb ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = field_start(&fsb,lp,ll)) >= 0) {
	    int		kl ;
	    cchar	*kp{} ;
	    if ((kl = field_get(&fsb,kterms,&kp)) > 0) {
	        vecstr	vals ;
	        if ((rs = vecstr_start(&vals,10,0)) >= 0) {
	            cint	flen = op->flen ;
	            int		fl ;
	            int		vl = 0 ;
	            char	*fbuf = op->fbuf ;
	            while ((fl = field_sharg(&fsb,aterms,fbuf,flen)) >= 0) {
	                vl += (fl + 1) ;
	                rs = vecstr_add(&vals,fbuf,fl) ;
	                if (fsb.term == '#') break ;
	                if (rs < 0) break ;
	            } /* end while (fields) */
		    if (rs >= 0) {
	                PF_E	pe ;
	                if ((rs = entry_start(&pe,fi,kp,kl,&vals,vl)) >= 0) {
	                    c += 1 ;
	                    rs = paramfile_entadd(op,&pe) ;
	                    if (rs < 0)
	                        entry_finish(&pe) ;
	                } /* end if (entry_start) */
		    } /* end if (ok) */
	            rs1 = vecstr_finish(&vals) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (vecstr) */
	    } /* end if (non-zero key) */
	    rs1 = field_finish(&fsb) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (field) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (paramfile_fileparseline) */

static int paramfile_entadd(PF *op,PF_E *pep) noex {
	return vecobj_add(op->entsp,pep) ;
}
/* end subroutine (paramfile_entadd) */

static int paramfile_entsub(PF *op,PF_E *pep,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rvl = 0 ;
	cchar		*rvalp = nullptr ;
	if (op && pep) {
	    rs = SR_OK ;
	    if (pep->oval) {
		cint		vallen = PARAMFILE_VALLEN ;
	        rvl = pep->vlen ;
	        if (pep->value == nullptr) {
	            buffer	b ;
	            cint	start = max(pep->vlen,vallen) ;
	            bool	f_exptry = false ;
	            cchar	*rvp = nullptr ;
	            if ((rs = buffer_start(&b,start)) >= 0) {
	                if (op->fl.definit) {
		            varsub	*dlp = op->defp ;
		            int		sl = pep->olen ;
		            cchar	*sp = pep->oval ;
	                    f_exptry = true ;
	                    rs = varsub_expbuf(dlp,&b,sp,sl) ;
	                    rvl = rs ;
	                } /* end if */
	                if ((! f_exptry) || (rs == SR_NOTFOUND)) {
	                    rs = SR_OK ;
	                    if (! op->fl.envload) {
	                        rs = paramfile_envload(op) ;
		            }
	                    if (rs >= 0) {
		                varsub	*elp = op->envp ;
		                int	sl = pep->olen ;
		                cchar	*sp = pep->oval ;
	                        f_exptry = true ;
	                        rs = varsub_expbuf(elp,&b,sp,sl) ;
	                        rvl = rs ;
	                    }
	                } /* end if (second try) */
	                if ((rs >= 0) && (! f_exptry)) rs = SR_NOTFOUND ;
	                if (rs >= 0) {
	                    if ((rs = buffer_get(&b,&rvp)) >= 0) {
	                        bool	f ;
	                        f = (rvl != pep->olen) ;
	                        f = f || (strncmp(rvp,pep->oval,rvl) != 0) ;
	                        if (f) {
	                            void	*vp{} ;
	                            if ((rs = uc_malloc((rvl+1),&vp)) >= 0) {
				        char	*bp = charp(vp) ;
				        rvalp = bp ;
	                                strwcpy(bp,rvp,rvl) ;
	                                pep->value = bp ;
	                                pep->vlen = rvl ;
	                            }
	                        } else {
	                            pep->value = pep->oval ;
	                            pep->vlen = pep->olen ;
	                            rvalp = pep->value ;
	                        }
	                    } /* end if (buffer_get) */
	                } else {
	                    rvalp = pep->oval ;
		        }
	                rs1 = buffer_finish(&b) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (buffer) */
	        } else {
	            rvalp = pep->value ;
	        }
	    } /* end if (non-null) */
	    if ((rs >= 0) && rpp) {
	        *rpp = rvalp ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? rvl : rs ;
}
/* end subroutine (paramfile_entsub) */

static int paramfile_filefins(PF *op) noex {
	vecobj		*flp = op->filep ;
	int		rs = SR_OK ;
	int		rs1 ;
	void		*vp{} ;
	for (int i = 0 ; vecobj_get(flp,i,&vp) >= 0 ; i += 1) {
	    if (vp) {
	        PF_F	*fep = (PF_F *) vp ;
	        rs1 = file_finish(fep) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end for */
	return rs ;
}
/* end subroutine (paramfile_filefins) */

static int paramfile_entfins(PF *op) noex {
	vecobj		*elp = op->entsp ;
	int		rs = SR_OK ;
	int		rs1 ;
	void		*vp{} ;
	for (int i = 0 ; vecobj_get(elp,i,&vp) >= 0 ; i += 1) {
	    if (vp) {
	        PF_E	*pep = entp(vp) ;
	        rs1 = entry_finish(pep) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end for */
	return rs ;
}
/* end subroutine (paramfile_entfins) */

static int paramfile_filedump(PF *op,int fi) noex {
	vecobj		*slp = op->entsp ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ;
	void		*vp{} ;
	for (int i = 0 ; vecobj_get(slp,i,&vp) >= 0 ; i += 1) {
	    if (vp) {
	        PF_E	*pep = entp(vp) ;
	        if ((pep->fi == fi) || (fi < 0)) {
		    {
	                rs1 = entry_finish(pep) ;
	                if (rs >= 0) rs = rs1 ;
		    }
		    {
	                rs1 = vecobj_del(slp,i--) ;
	                if (rs >= 0) rs = rs1 ;
		    }
	            c += 1 ;
	        } /* end if (got one) */
	    } /* end if (non-null) */
	} /* end for */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (paramfile_filedump) */

static int paramfile_filedel(PF *op,int fi) noex {
	vecobj		*flp = op->filep ;
	int		rs ;
	int		rs1 ;
	void		*vp{} ;
	if ((rs = vecobj_get(flp,fi,&vp)) >= 0) {
	    if (vp) {
		{
		    PF_F	*fep = filep(vp) ;
	            rs1 = file_finish(fep) ;
	            if (rs >= 0) rs = rs1 ;
		}
		{
	            rs1 = vecobj_del(op->filep,fi) ;
	            if (rs >= 0) rs = rs1 ;
		}
	    }
	} /* end if (get) */
	return rs ;
}
/* end subroutine (paramfile_filedel) */

static int paramfile_envbegin(PF *op) noex {
	int		rs = SR_OK ;
	if (! op->fl.envinit) {
	    cint	vao = 0 ;
	    rs = varsub_start(op->envp,vao) ;
	    op->fl.envinit = (rs >= 0) ;
	} /* end if */
	return rs ;
}
/* end subroutine (paramfile_envbegin) */

static int paramfile_envend(PF *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->fl.envinit) {
	    op->fl.envinit = false ;
	    rs1 = varsub_finish(op->envp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (paramfile_envend) */

static int paramfile_envload(PF *op) noex {
	int		rs = SR_OK ;
	if ((op->envv != nullptr) && (! op->fl.envload)) {
	    op->fl.envload = true ;
	    if (! op->fl.envinit) {
	        rs = paramfile_envbegin(op) ;
	    }
	    if (rs >= 0) {
	        rs = varsub_addva(op->envp,op->envv) ;
	    }
	} /* end if */
	return rs ;
}
/* end subroutines (paramfile_envload) */

static int paramfile_defbegin(PF *op,vecstr *dvp) noex {
	int		rs = SR_OK ;
	if (! op->fl.definit) {
	    cint	vao = VARSUB_OBADNOKEY ;
	    rs = varsub_start(op->defp,vao) ;
	    op->fl.definit = (rs >= 0) ;
	}
	if ((rs >= 0) && (dvp != nullptr)) {
	    rs = paramfile_defload(op,dvp) ;
	}
	return rs ;
}
/* end subroutines (paramfile_defbegin) */

static int paramfile_defend(PF *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->fl.definit) {
	    op->fl.definit = false ;
	    rs1 = varsub_finish(op->defp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (paramfile_defend) */

static int paramfile_defload(PF *op,vecstr *dvp) noex {
	int		rs = SR_OK ;
	if (dvp != nullptr) {
	    cchar	*tp, *cp ;
	    for (int i = 0 ; vecstr_get(dvp,i,&cp) >= 0 ; i += 1) {
	        if (cp) {
	            if ((tp = strchr(cp,'=')) != nullptr) {
			cint tl = intconv(tp - cp) ;
	                rs = varsub_add(op->defp,cp,tl,(tp + 1),-1) ;
	            } else {
	                rs = varsub_add(op->defp,cp,-1,nullptr,0) ;
		    }
		}
	        if (rs < 0) break ;
	    } /* end for */
	} /* end if */
	return rs ;
}
/* end subroutine (paramfile_defload) */

static int paramfile_entrels(PF *op) noex {
	vecobj		*slp = op->entsp ;
	int		rs = SR_OK ;
	int		rs1 ;
	void		*vp{} ;
	for (int i = 0 ; slp->get(i,&vp) >= 0 ; i += 1) {
	    PF_E	*pep = entp(vp) ;
	    if (vp) {
	        rs1 = entry_release(pep) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end for */
	return rs ;
}
/* end subroutine (paramfile_entrels) */

int paramfile::open(mainv ev,cchar *fname) noex {
	return paramfile_open(this,ev,fname) ;
}

int paramfile::curbegin(PF_C *curp) noex {
	return paramfile_curbegin(this,curp) ;
}

int paramfile::curend(PF_C *curp) noex {
	return paramfile_curend(this,curp) ;
}

int paramfile::curenum(PF_C *curp,PF_E *ep,char *rbuf,int rlen) noex {
	return paramfile_curenum(this,curp,ep,rbuf,rlen) ;
}

int paramfile::check(time_t dt) noex {
	return paramfile_check(this,dt) ;
}

void paramfile::dtor() noex {
	if (cint rs = close ; rs < 0) {
	    ulogerror("paramfile",rs,"fini-close") ;
	}
} /* end method (paramfile::dtor) */

int paramfile_co::operator () (int a) noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case paramfilemem_checkint:
	        rs = paramfile_checkint(op,a) ;
	        break ;
	    case paramfilemem_close:
	        rs = paramfile_close(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (paramfile_co::operator) */

static int file_start(PF_F *fep,cchar *fname) noex {
	int		rs = SR_FAULT ;
	if (fep && fname) {
	    memclear(fep) ;
	    if (cchar *cp ; (rs = uc_mallocstrw(fname,-1,&cp)) >= 0) {
	        fep->fname = cp ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (file_start) */

static int file_finish(PF_F *fep) noex {
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

static int entry_start(PF_E *pep,int fi,cc *kp,int kl,VS *vsp,int olen) noex {
	int		rs = SR_FAULT ;
	int		bsize = 0 ;
	if (pep && kp) {
	    void	*vp{} ;
	    memclear(pep) ;
	    pep->fi = fi ;
	    if (kl < 0) kl = lenstr(kp) ;
	    pep->klen = kl ;
	    pep->olen = olen ;
	    bsize = (kl + 1 + olen + 1) + 10 ;
	    if ((rs = uc_malloc(bsize,&vp)) >= 0) {
	        char	*bp = charp(vp) ;
	        cchar	*cp{} ;
	        pep->key = bp ;
	        bp = strwcpy(bp,kp,kl) + 1 ;
	        pep->oval = bp ;
	        *bp = '\0' ;
	        for (int i = 0 ; vecstr_get(vsp,i,&cp) >= 0 ; i += 1) {
	            if (i > 0) bp[-1] = CH_FS ;
	            bp = strwcpy(bp,cp,-1) + 1 ;
	        } /* end for */
	    } /* end if (memory-allocation) */
	} /* end if (non-null) */
	return (rs >= 0) ? bsize : rs ;
}
/* end subroutine (entry_start) */

static int entry_finish(PF_E *pep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (pep) {
	    rs = SR_OK ;
	    pep->fi = -1 ;
	    if (pep->value) {
	        if (pep->oval != pep->value) {
	            rs1 = uc_free(pep->value) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	        pep->value = nullptr ;
	    }
	    if (pep->key) {
	        rs1 = uc_free(pep->key) ;
	        if (rs >= 0) rs = rs1 ;
	        pep->key = nullptr ;
	    }
	    pep->oval = nullptr ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (entry_finish) */

static int entry_release(PF_E *pep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (pep) {
	    rs = SR_OK ;
	    if ((pep->oval != pep->value) && pep->value) {
	        rs1 = uc_free(pep->value) ;
	        if (rs >= 0) rs = rs1 ;
	        pep->value = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (entry_release) */

static int vcmpentry(cvoid **v1pp,cvoid **v2pp) noex {
	PF_E		*e1p = (PF_E *) *v1pp ;
	PF_E		*e2p = (PF_E *) *v2pp ;
	int		rc = 0 ;
	if (e1p || e2p) {
	    rc = +1 ;
	    if (e1p) {
		rc = -1 ;
	        if (e2p) {
		    cint	ch1 = mkchar(e1p->key[0]) ;
		    cint	ch2 = mkchar(e2p->key[0]) ;
	            if ((rc = (ch1 - ch2)) == 0) {
	                rc = strcmp(e1p->key,e2p->key) ;
	            }
	        }
	    }
	}
	return rc ;
}
/* end subroutine (vcmpentry) */

static int mkterms() noex {
	int		rs ;
	if ((rs = fieldterms(aterms,false,'\b','\t','\v','\f',' ','#')) >= 0) {
	    rs = fieldterms(kterms,false,'\b','\t','\v','\f',' ','#','=') ;
	}
	return rs ;
}
/* end subroutine (mkterms) */


