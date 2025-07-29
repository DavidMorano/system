/* strlistmks SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* make a STRLIST database */
/* version %I% last-modified %G% */

#define	CF_MINMOD	1		/* ensure minimum file mode */
#define	CF_USESBUF	1		/* use |sbuf(3uc)| */

/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strlistmks

	Description:
	This subroutine creates a STRLIST database file.

	Synopsis:
	int strlistmks_open(strlistmks *op,cc *dbn,int of,mode_t om,int n) noex

	Arguments:
	op		object pointer
	dbn		name of (path-to) DB
	of		open-flags
	om		open-mode
	n		starting estimate of numbers of variables

	Returns:
	>=0		OK
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>		/* |INT_MAX| + |UINT_MAX| */
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<getbufsize.h>
#include	<sysval.hh>
#include	<bufsizevar.hh>
#include	<mallocxx.h>
#include	<endian.h>
#include	<vecobj.h>
#include	<strtab.h>
#include	<filer.h>
#include	<getpwd.h>
#include	<permx.h>
#include	<opentmp.h>
#include	<sbuf.h>
#include	<sfx.h>
#include	<snx.h>
#include	<sncpyx.h>
#include	<mkx.h>
#include	<mkpathx.h>
#include	<mknpathx.h>
#include	<mkfnamesuf.h>
#include	<nextpowtwo.h>
#include	<hash.h>
#include	<hashindex.h>
#include	<isnot.h>
#include	<localmisc.h>		/* |MODP2| */

#include	"strlistmks.h"
#include	"strlisthdr.h"

import libutil ;

/* local defines */

#define	STRLISTMKS_SIZEMULT	4
#define	STRLISTMKS_NSKIP	5
#define	STRLISTMKS_INDPERMS	0664
#define	STRLISTMKS_FSUF		STRLISTHDR_FSUF
#define	STRLISTMKS_MAGICSTR	STRLISTHDR_MAGICSTR
#define	STRLISTMKS_MAGICSIZE	STRLISTHDR_MAGICSIZE

#define	SLM		strlistmks
#define	SLM_OBJ		strlistmks_obj
#define	SLM_RT		strlistmks_rt
#define	SLM_FL		strlistmks_fl
#define	RT		strlistmks_rt
#define	VE		varentry

#ifndef	KEYBUFLEN
#define	KEYBUFLEN	120
#endif

#define	HDRBUFLEN	(szof(strlisthdr) + 128)

#define	TO_OLDFILE	(5 * 60)

#ifndef	CF_MINMOD
#define	CF_MINMOD	1		/* ensure minimum file mode */
#endif
#ifndef	CF_USESBUF
#define	CF_USESBUF	1		/* use |sbuf(3uc)| */
#endif


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef mode_t		om_t ;

typedef uint		*rectab_t ;


/* external subroutines */


/* external variables */


/* local structures */

struct varentry {
	uint	khash ;
	uint	ri ;
	uint	ki ;
	uint	hi ;
} ;

struct vars {
	int	maxpathlen ;
	int	maxnamelen ;
} ;


/* forward references */

template<typename ... Args>
static int strlistmks_ctor(strlistmks *op,Args ... args) noex {
    	STRLISTMKS	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    memclear(hop) ;
	    if ((op->stp = new(nothrow) strtab) != np) {
		if ((op->rtp = new(nothrow) srectab) != np) {
		    rs = SR_OK ;
		} /* end if (new-rectab) */
		if (rs < 0) {
		    delete op->stp ;
		    op->stp = nullptr ;
		}
	    } /* end if (new-strtab) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (strlistmks_ctor) */

static int strlistmks_dtor(strlistmks *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->rtp) {
		delete op->rtp ;
		op->rtp = nullptr ;
	    }
	    if (op->stp) {
		delete op->stp ;
		op->stp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (strlistmks_dtor) */

static int	strlistmks_filesbegin(SLM *) noex ;
static int	strlistmks_filesend(SLM *) noex ;
static int	strlistmks_fexists(SLM *,char *) noex ;
static int	strlistmks_dirok(SLM *,char *) noex ;

static int	strlistmks_listbegin(SLM *,int) noex ;
static int	strlistmks_listend(SLM *) noex ;

static int	strlistmks_nfcreate(SLM *,char *,int) noex ;
static int	strlistmks_nfdestroy(SLM *) noex ;
static int	strlistmks_mknfn(SLM *,char *,int) noex ;

#ifdef	COMMENT
static int	strlistmks_nfstore(SLM *,char *) noex ;
#endif /* COMMENT */

static int	strlistmks_mksfile(SLM *) noex ;
static int	strlistmks_wrsfile(SLM *) noex ;
static int	strlistmks_mkind(SLM *,char *,uint (*)[3],int) noex ;
static int	strlistmks_renamefiles(SLM *) noex ;

static int	indinsert(rectab_t,uint (*it)[3],int,VE *) noex ;

static int	mkvars() noex ;


/* local variables */

static sysval		pagesize(sysval_ps) ;

static vars		var ;

static cchar		*end = ENDIANSTR ;

constexpr gid_t		gidend = -1 ;

constexpr int		maglen = STRLISTMKS_MAGICSIZE ;

constexpr cchar		pre[] = STRLISTMKS_FSUF ;
constexpr cchar		pat[] = "XXXXXXXX" ;
constexpr cchar		suf[] = STRLISTMKS_FSUF ;
constexpr cchar		magstr[] = STRLISTMKS_MAGICSTR ;

constexpr bool		f_minmod = CF_MINMOD ;
constexpr bool		f_usesbuf = CF_USESBUF ;


/* exported variables */

strlistmks_obj	strlistmks_modinfo = {
	"strlistmks",
	szof(strlistmks),
	0
} ;


/* exported subroutines */

int strlistmks_open(SLM *op,cc *dbname,int of,mode_t om,int n) noex {
	int		rs ;
	if ((rs = strlistmks_ctor(op,dbname)) >= 0) {
	    rs = SR_INVALID ;
	    if (dbname[0]) {
		static cint	rsv = mkvars() ;
		if ((rs = rsv) >= 0) {
	            cchar	*cp ;
	            if (n < STRLISTMKS_NENTRIES) {
	                n = STRLISTMKS_NENTRIES ;
	            }
	            op->om = om ;
	            op->nfd = -1 ;
	            op->gid = -1 ;
	            op->f.ofcreat = !!(of & O_CREAT) ;
	            op->f.ofexcl = !!(of & O_EXCL) ;
	            op->f.none = (! op->f.ofcreat) && (! op->f.ofexcl) ;
	            if ((rs = uc_mallocstrw(dbname,-1,&cp)) >= 0) {
		        op->dbname = cp ;
		        if ((rs = strlistmks_filesbegin(op)) >= 0) {
		            if ((rs = strlistmks_listbegin(op,n)) >= 0) {
			        op->magic = STRLISTMKS_MAGIC ;
		            }
		            if (rs < 0) {
			        strlistmks_filesend(op) ;
		            }
		        } /* end if */
		        if (rs < 0) {
	    	            uc_free(op->dbname) ;
	    	            op->dbname = nullptr ;
		        }
	            } /* end if (memory-allocation) */
		} /* end if (mkvars) */
	    } /* end if (valid) */
	    if (rs < 0) {
		strlistmks_dtor(op) ;
	    }
	} /* end if (strlistmks_ctor) */
	return rs ;
}
/* end subroutine (strlistmks_open) */

int strlistmks_close(SLM *op) noex {
	int		rs ;
	int		rs1 ;
	int		nvars = 0 ;
	if ((rs = strlistmks_magic(op)) >= 0) {
	    nvars = op->nstrs ;
	    if (! op->f.abort) {
	        rs1 = strlistmks_mksfile(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->nfd >= 0) {
	        rs1 = u_close(op->nfd) ;
	        if (rs >= 0) rs = rs1 ;
	        op->nfd = -1 ;
	    }
	    {
	        rs1 = strlistmks_listend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if ((rs >= 0) && (! op->f.abort)) {
	        rs1 = strlistmks_renamefiles(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = strlistmks_filesend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->dbname) {
	        rs1 = uc_free(op->dbname) ;
	        if (rs >= 0) rs = rs1 ;
	        op->dbname = nullptr ;
	    }
	    {
		rs1 = strlistmks_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return (rs >= 0) ? nvars : rs ;
}
/* end subroutine (strlistmks_close) */

int strlistmks_addvar(SLM *op,cchar *sp,int sl) noex {
	int		rs ;
	if ((rs = strlistmks_magic(op,sp)) >= 0) {
	    if ((rs = strtab_add(op->stp,sp,sl)) >= 0) {
	        uint	ki = rs ;
	        if ((rs = srectab_add(op->rtp,ki)) >= 0) {
	            op->nstrs += 1 ;
	        }
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (strlistmks_addvar) */

int strlistmks_abort(SLM *op) noex {
	int		rs ;
	if ((rs = strlistmks_magic(op)) >= 0) {
	    op->f.abort = true ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (strlistmks_abort) */

int strlistmks_chgrp(SLM *op,gid_t gid) noex {
	int		rs ;
	if ((rs = strlistmks_magic(op)) >= 0) {
	    op->gid = gid ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (strlistmks_chgrp) */


/* private subroutines */

static int strlistmks_filesbegin(SLM *op) noex {
	int		rs ;
	int		rs1 ;
	char		*tbuf{} ;
	if ((rs = malloc_mp(&tbuf)) >= 0) {
	    cint	tlen = rs ;
	    if ((rs = strlistmks_fexists(op,tbuf)) >= 0) {
	        if ((rs = strlistmks_dirok(op,tbuf)) >= 0) {
		    rs = strlistmks_nfcreate(op,tbuf,tlen) ;
	            if (rs < 0) {
		        if (op->idname) { /* from |_dirok()| */
	    	            uc_free(op->idname) ;
	    	            op->idname = nullptr ;
		        }
	            } /* end if (error) */
	        } /* end if (strlistmks_dirok) */
	    } /* end if (strlistmks_fexists) */
	    rs1 = uc_free(tbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end subroutine (strlistmks_filesbegin) */

static int strlistmks_filesend(SLM *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    rs1 = strlistmks_nfdestroy(op) ;
	    if (rs >= 0) rs = rs1 ;
	}
	if (op->idname) {
	    rs1 = uc_free(op->idname) ;
	    if (rs >= 0) rs = rs1 ;
	    op->idname = nullptr ;
	}
	return rs ;
}
/* end subroutine (strlistmks_filesend) */

/* check that the index-directory is writable to us */
static int strlistmks_dirok(SLM *op,char *tbuf) noex {
	int		rs = SR_NOTDIR ;
	cchar		*dnp ;
	if (int dnl ; (dnl = sfdirname(op->dbname,-1,&dnp)) > 0) {
	    if (cchar *cp ; (rs = uc_mallocstrw(dnp,dnl,&cp)) >= 0) {
	        op->idname = cp ;
		if ((rs = mkpath1w(tbuf,dnp,dnl)) >= 0) {
		    cint	am = (X_OK | W_OK) ;
		    rs = perm(tbuf,-1,-1,nullptr,am) ;
		} /* end if (mkpath) */
	        if (rs < 0) {
	    	    uc_free(op->idname) ;
	    	    op->idname = nullptr ;
	        } /* end if (error) */
	    } /* end if (memory-allocation) */
	} /* end if (extracting directory) */
	return rs ;
}
/* end if (strlistmks_dirok) */

static int strlistmks_nfcreate(SLM *op,char *tbuf,int tlen) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = strlistmks_mknfn(op,tbuf,tlen)) >= 0) {
	    cint	nfl = rs ;
	    char	*rbuf{} ;
	    if ((rs = malloc_mp(&rbuf)) >= 0) {
		cint	of = (O_CREAT | O_EXCL | O_WRONLY) ;
		cmode	om = op->om ;
	        if ((rs = opentmpfile(tbuf,of,om,rbuf)) >= 0) {
	            if (cchar *cp ; (rs = uc_mallocstrw(rbuf,nfl,&cp)) >= 0) {
	                op->nfname = charp(cp) ;
		    } /* end if (memory-allocation of 'nfname') */
	        } /* end if (opentmpfile) */
		rs1 = uc_free(rbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (strlistmks_mknfn) */
	return rs ;
}
/* end subroutine (txindexmks_nfcreate) */

static int strlistmks_nfdestroy(SLM *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->nfd >= 0) {
	    rs1 = u_close(op->nfd) ;
	    if (rs >= 0) rs = rs1 ;
	    op->nfd = -1 ;
	}
	if (op->nfname) {
	    if (op->nfname[0] != '\0') {
		rs1 = u_unlink(op->nfname) ;
	        if (rs >= 0) rs = rs1 ;
		op->nfname[0] = '\0' ;
	    }
	    {
	        rs1 = uc_free(op->nfname) ;
	        if (rs >= 0) rs = rs1 ;
	        op->nfname = nullptr ;
	    }
	}
	return rs ;
}
/* end subroutine (strlistmks_nfdestroy) */

static int strlistmks_mknfn(SLM *op,char *tbuf,int tlen) noex {
	int		rs ;
	int		rs1 ;
	int		i = 0 ;
	if_constexpr (f_usesbuf) {
	    sbuf	b ;
	    if ((rs = b.start(tbuf,tlen)) >= 0) {
		{
		    b << op->idname ;
		    b << '/' ;
		    b << pre ;
		    b << pat ;
		    b << '.' ;
		    b << suf ;
		    b << end ;
		    b << 'n' ;
		} /* end block */
		rs1 = b.finish ;
		if (rs >= 0) rs = rs1 ;
		i = rs1 ;
	    } /* end if (sbuf) */
	} else {
	    rs = SR_OK ;
	    if (rs >= 0) {
	        rs = snadd(tbuf,tlen,i,op->idname) ;
	        i += rs ;
	    }
	    if (rs >= 0) {
	        rs = snadd(tbuf,tlen,i,"/") ;
	        i += rs ;
	    }
	    if (rs >= 0) {
	        rs = snadd(tbuf,tlen,i,pre) ;
	        i += rs ;
	    }
	    if (rs >= 0) {
	        rs = snadd(tbuf,tlen,i,pat) ;
	        i += rs ;
	    }
	    if (rs >= 0) {
	        rs = snadd(tbuf,tlen,i,".") ;
	        i += rs ;
	    }
	    if (rs >= 0) {
	        rs = snadd(tbuf,tlen,i,suf) ;
	        i += rs ;
	    }
	    if (rs >= 0) {
	        rs = snadd(tbuf,tlen,i,end) ;
	        i += rs ;
	    }
	    if (rs >= 0) {
	        rs = snadd(tbuf,tlen,i,"n") ;
	        i += rs ;
	    }
	} /* end if_constexpr (f_usesbuf) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (strlistmks_nfdestroy) */

#ifdef	COMMENT
static int strlistmks_nfstore(SLM *op,char *outfname) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->nfname) {
	    rs1 = uc_free(op->nfname) ;
	    if (rs >= 0) rs = rs1 ;
	    op->nfname = nullptr ;
	}
	if (rs >= 0) {
	    if (cchar *cp ; (rs = uc_mallocstrw(outfname,-1,&cp)) >= 0) {
		op->nfname = charp(cp) ;
	    }
	}
	return rs ;
}
/* end subroutine (strlistmks_nfstore) */
#endif /* COMMENT */

static int strlistmks_fexists(SLM *op,char *tbuf) noex {
	int		rs = SR_OK ;
	if (op->f.ofcreat && op->f.ofexcl) {
	    cchar	*dbn = op->dbname ;
	    if ((rs = mkfnamesuf2(tbuf,dbn,suf,end)) >= 0) {
		USTAT	sb ;
		if ((rs = u_stat(tbuf,&sb)) >= 0) {
	            rs = SR_EXIST ;
		} else if (isNotPresent(rs)) {
		    rs = SR_OK ;
	        }
	    } /* end if (mkfnamesuf) */
	} /* end if */
	return rs ;
}
/* end subroutine (strlistmks_fexists) */

static int strlistmks_listbegin(SLM *op,int n) noex {
	cint		sz = (n * STRLISTMKS_SIZEMULT) ;
	int		rs ;
	if ((rs = strtab_start(op->stp,sz)) >= 0) {
	    rs = srectab_start(op->rtp,n) ;
	    if (rs < 0) {
		strtab_finish(op->stp) ;
	    }
	} /* end if (strtab-keys) */
	return rs ;
}
/* end subroutine (strlistmks_listbegin) */

static int strlistmks_listend(SLM *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->rtp) {
	    rs1 = srectab_finish(op->rtp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	if (op->stp) {
	    rs1 = strtab_finish(op->stp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (strlistmks_listend) */

static int strlistmks_mksfile(SLM *op) noex {
	cint		rtl = srectab_done(op->rtp) ;
	int		rs = SR_BUGCHECK ;
	int		nstrs = 0 ;
	if (rtl == (op->nstrs + 1)) {
	    if ((rs = strlistmks_wrsfile(op)) >= 0) {
		nstrs = op->nstrs ;
	    }
	}
	return (rs >= 0) ? nstrs : rs ;
}
/* end subroutine (strlistmks_mksfile) */

namespace {
    struct sub_wrsfile {
	strlistmks	*op ;
	strlisthdr	hf{} ;
	custime		dt = time(nullptr) ;
	cint		hlen = HDRBUFLEN ;
	char		hbuf[HDRBUFLEN+1] ;
	uint		foff = 0 ;
	int		ps ;		/* pagesize */
	sub_wrsfile(strlistmks *p) noex : op(p) { } ;
	operator int () noex ;
	int mkfile(rectab_t,int) noex ;
	int mkkstab(filer *,int,int) noex ;
    } ;
}

static int strlistmks_wrsfile(SLM *op) noex {
	sub_wrsfile	wrf(op) ;
	return wrf ;
}
/* end subroutine (strlistmks_wrsfile) */

sub_wrsfile::operator int () noex {
	int		rs ;
	if ((rs = pagesize) >= 0) {
	    rectab_t	rt ;
	    ps = rs ;
	    if ((rs = srectab_getvec(op->rtp,&rt)) >= 0) {
	        cint	rtl = rs ;
		if ((rs = mkfile(rt,rtl)) >= 0) {
	    	    hf.fsize = foff ;
	  	    if ((rs = strlisthdr_rd(&hf,hbuf,hlen)) >= 0) {
			coff	moff = STRLISTHDR_MAGICSIZE ;
	        	cint	hl = rs ;
	        	if ((rs = u_pwrite(op->nfd,hbuf,hl,moff)) >= 0) {
			    if_constexpr (f_minmod) {
	    			rs = uc_fminmod(op->nfd,op->om) ;
			    } /* end if_constexpr (f_minmod) */
	    		    if ((rs >= 0) && (op->gid != gidend)) {
				rs = u_fchown(op->nfd,-1,op->gid) ;
	    		    }
	    		}
		    } /* end if (strlisthdr_rd) */
		} /* end if (mkfile) */
	    } /* end if (srectab_getvec) */
	} /* end if (pagesize) */
	return rs ;
}
/* end method (sub_wrsfile::operator) */ 

int sub_wrsfile::mkfile(rectab_t rt,int rtl) noex {
	filer		sfile, *sfp = &sfile ;
	int		rs ;
	int		rs1 ;
	int		sz = (ps * 4) ;
	if ((rs = filer_start(sfp,op->nfd,0,sz,0)) >= 0) {
	    cint	mlen = maglen ;
	    char	mbuf[maglen + 1] ;
	    if ((rs = mkmagic(mbuf,mlen,magstr)) >= 0) {
		foff += rs ;
		if ((rs = filer_write(sfp,mbuf,rs)) >= 0) {
	            /* prepare the file-header */
	            hf.vetu[0] = STRLISTMKS_VERSION ;
	            hf.vetu[1] = char(ENDIAN) ;
	            hf.vetu[2] = 0 ;
	            hf.vetu[3] = 0 ;
	            hf.wtime = uint(dt) ;
	            hf.nstrs = op->nstrs ;
	            hf.nskip = STRLISTMKS_NSKIP ;
	            if ((rs = strlisthdr_rd(&hf,hbuf,hlen)) >= 0) {
		        cint	hl = rs ;
	                if ((rs = filer_writefill(sfp,hbuf,hl)) >= 0) {
	    	            foff += rs ;
	                    /* write the record table */
	                    hf.rtoff = foff ;
	                    hf.rtlen = rtl ;
	                    sz = (rtl + 1) * szof(uint) ;
	                    if ((rs = filer_write(sfp,rt,sz)) >= 0) {
			        strtab	*ksp = op->stp ;
	                        foff += rs ;
		                /* make and write out key-string table */
			        if ((rs = strtab_strsize(ksp)) >= 0) {
			            sz = rs ;
			            hf.stoff = foff ;
			            hf.stlen = sz ;
			            rs = mkkstab(sfp,rtl,sz) ;
			        }
		            } /* end if (filer_write) */
		        } /* end if (filer_writefill) */
	            } /* end if (strlisthdr_rd) */
		} /* end if */
	    } /* end if (mkmagic) */
	    rs1 = filer_finish(sfp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (filer) */
	return rs ;
}
/* end subroutine (sub_wrsfile::mkfile) */

int sub_wrsfile::mkkstab(filer *vfp,int rtl,int sz) noex {
	int		rs ;
	int		rs1 ;
	char		*kstab{} ;
	if ((rs = uc_malloc(sz,&kstab)) >= 0) {
	    strtab	*ksp = op->stp ;
	    if ((rs = strtab_strmk(ksp,kstab,sz)) >= 0) {
		/* write out the key-string table */
		if ((rs = filer_write(vfp,kstab,sz)) >= 0) {
		    uint	(*indtab)[3] = nullptr ;
	            cint	itl = nextpowtwo(rtl) ;
	            foff += rs ;
		    /* make and write out the record-index table */
	            hf.itoff = foff ;
	            hf.itlen = itl ;
	            sz = (itl + 1) * 3 * szof(uint) ;
	            if ((rs = uc_malloc(sz,&indtab)) >= 0) {
			memset(indtab,0,sz) ;
	                if ((rs = strlistmks_mkind(op,kstab,indtab,itl)) >= 0) {
	                    rs = filer_write(vfp,indtab,sz) ;
	                    foff += rs ;
	                }
	                rs1 = uc_free(indtab) ;
			if (rs >= 0) rs = rs1 ;
	            } /* end if (m-a-f) */
	        } /* end if (record-index table) */
	        rs1 = uc_free(kstab) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (key-string table) */
	return rs ;
}
/* end subroutine (sub_wrsfile::mkkstab) */

int strlistmks_mkind(SLM *op,char *kst,uint (*it)[3], int il) noex {
	rectab_t	rt ;
	int		rs ;
	int		sc = 0 ;
	if ((rs = srectab_getvec(op->rtp,&rt)) >= 0) {
	    VE		ve ;
	    cint	rtl = rs ;
	    uint	hi ;
	    uint	ki ;
	    uint	khash ;
	    char	*kp ;
	        for (int ri = 1 ; ri < rtl ; ri += 1) {
	            ki = rt[ri] ;
	            kp = kst + ki ;
	            khash = hash_elf(kp,-1) ;
	            hi = hashindex(khash,il) ;
	            ve.ri = ri ;
	            ve.ki = ki ;
	            ve.khash = khash ;
	            ve.hi = hi ;
	            sc += indinsert(rt,it,il,&ve) ;
	        } /* end for */
	    it[il][0] = UINT_MAX ;
	    it[il][1] = 0 ;
	    it[il][2] = 0 ;
	    if (sc < 0) {
	        sc = 0 ;
	    }
	} /* end if (srectab_getvec) */
	return (rs >= 0) ? sc : rs ;
}
/* end subroutine (strlistmks_mkind) */

static int strlistmks_renamefiles(SLM *op) noex {
	int		rs ;
	int		rs1 ;
	char		*tbuf{} ;
	if ((rs = malloc_mp(&tbuf)) >= 0) {
	    if ((rs = mkfnamesuf2(tbuf,op->dbname,suf,end)) >= 0) {
	        if ((rs = u_rename(op->nfname,tbuf)) >= 0) {
	            op->nfname[0] = '\0' ;
		}
	        if (op->nfname[0] != '\0') {
	            u_unlink(op->nfname) ;
	            op->nfname[0] = '\0' ;
	        }
	    } /* end if (mkfnamesuf) */
	    rs1 = uc_free(tbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end subroutine (strlistmks_renamefiles) */

static int indinsert(rectab_t rt,uint (*it)[3],int il,VE *vep) noex {
	uint		nhash = vep->khash ;
	uint		chash = (vep->khash & INT_MAX) ;
	uint		hi = vep->hi ;
	int		c = 0 ;
	while (it[hi][0] != 0) {
	    uint	ri = it[hi][0] ;
	    uint	ki = rt[ri] ;
	    if (ki == vep->ki) break ;
	    it[hi][1] |= (~ INT_MAX) ;
	    nhash = hash_again(nhash,c++,STRLISTMKS_NSKIP) ;
	    hi = hashindex(nhash,il) ;
	} /* end while */
	if (it[hi][0] > 0) {
	    uint	lhi = hi ;
	    uint	nhi ;
	    while ((nhi = it[lhi][2]) > 0) {
	        lhi = nhi ;
	    }
	    hi = hashindex((lhi + 1),il) ;
	    while (it[hi][0] > 0) {
	        hi = hashindex((hi + 1),il) ;
	    }
	    it[lhi][2] = hi ;
	} /* end if (same-key continuation) */
	it[hi][0] = vep->ri ;
	it[hi][1] = chash ;
	it[hi][2] = 0 ;
	return c ;
}
/* end subroutine (indinsert) */

static int mkvars() noex {
	int		rs ;
	if ((rs = getbufsize(getbufsize_mp)) >= 0) {
	    var.maxpathlen = rs ;
	    if ((rs = getbufsize(getbufsize_mn)) >= 0) {
	        var.maxnamelen = rs ;
	    }
	}
	return rs ;
}
/* end subroutine (mkvars) */


