/* txtindexmks SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* make a TXTINDEX database */
/* version %I% last-modified %G% */

#define	CF_SETINT	1		/* use |osetint(3dam)| */

/* revision history:

	= 2008-03-01, David A�D� Morano
	This code was originally written.

*/

/* Copyright � 2008 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	txtindexmks

	Description:
	This subroutine creates a TXTINDEX database (currently
	consisting of two files).

	Synopsis:
	int txtindexmks_open(txtindexmks *op,txtindexmks_pa *pp,
		cchar *dbname,int of,mode_t om) noex

	Arguments:
	- op		object pointer
	- pp		pointer to parameters
	- dbname	name of (path-to) DB
	- of		open-flags
	- om		open-permissions

	Returns:
	>=0		OK
	<0		error code

	Notes:
	= possible returns to an open attempt

	- OK (creating)
	- already exists
	- doesn't exist but is in progress
	- exists and is in progress

	= open-flags

			if DB exits	if NDB exists	returns
	___________________________________________________________________

	-		no		no		SR_OK (created)
	-		no		yes		SR_INPROGRESS
	-		yes		no		SR_OK
	-		yes		yes		SR_INPROGRESS

	O_CREAT|O_EXCL	no		no		SR_OK (created)
	O_CREAT|O_EXCL	no		yes		SR_INPROGRESS
	O_CREAT|O_EXCL	yes		no		SR_INPROGRESS
	O_CREAT|O_EXCL	yes		yes		SR_INPROGRESS

	O_CREAT		x		x		SR_OK (created)

*******************************************************************************/

#include	<envstandards.h>	/* must be before others */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/mman.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<getbufsize.h>
#include	<getpwd.h>
#include	<sysval.hh>
#include	<mallocxx.h>
#include	<endian.h>
#include	<estrings.h>
#include	<vecint.h>
#include	<osetint.h>
#include	<strtab.h>
#include	<bfile.h>
#include	<filer.h>
#include	<opentmp.h>
#include	<mktmp.h>
#include	<mkfname.h>
#include	<sfx.h>
#include	<strw.h>
#include	<strn.h>
#include	<hash.h>
#include	<xperm.h>
#include	<char.h>
#include	<hasx.h>
#include	<ischarx.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"txtindexmks.h"
#include	"txtindexhdr.h"
#include	"naturalwords.h"


/* local defines */

#define	TXTINDEXMKS_TABLEN	(16 * 1024)
#define	TXTINDEXMKS_EIGENSIZE	(2 * 1024)	/* usually sufficient! */
#define	TXTINDEXMKS_LISTLEN	6		/* default */
#define	TXTINDEXMKS_NSKIP	7		/* reasonable value */

#ifndef	KEYBUFLEN
#ifdef	NATURALWORDLEN
#define	KEYBUFLEN	NATURALWORDLEN
#else
#define	KEYBUFLEN	80
#endif
#endif

#define	HDRBUFLEN	(sizeof(HDR) + 128)
#define	BUFLEN		(sizeof(HDR) + 128)
#define	SUFBUFLEN	32
#define	TMPFNAMELEN	14

#define	FSUF_IND	"hash"
#define	FSUF_TAG	"tag"

#define	TO_OLDFILE	(5 * 60)

#define	MODP2(v,n)	((v) & ((n) - 1))

#if	CF_SETINT
#define	LISTOBJ				osetint
#define	LISTOBJ_OORDERED 		0
#define	LISTOBJ_CUR			osetint_cur
#define	LISTOBJ_START(op,n,opts)	osetint_start((op))
#define	LISTOBJ_FINISH(op)		osetint_finish((op))
#define	LISTOBJ_ADD(op,v)		osetint_addval((op),(v))
#define	LISTOBJ_COUNT(op)		osetint_count((op))
#define	LISTOBJ_MKVEC(op,va)		osetint_mkvec((op),(va))
#define	LISTOBJ_CURBEGIN(op,cp)		osetint_curbegin((op),(cp))
#define	LISTOBJ_ENUM(op,cp,rp)		osetint_curenum((op),(cp),(rp))
#define	LISTOBJ_CUREND(op,cp)		osetint_curend((op),(cp))
#else /* CF_SETINT */
#define	LISTOBJ				vecint
#define	LISTOBJ_OORDERED		VECINT_OORDERED
#define	LISTOBJ_CUR			vecint_cur
#define	LISTOBJ_START(op,n,opts)	vecint_start((op),(n),(opts))
#define	LISTOBJ_FINISH(op)		vecint_finish((op))
#define	LISTOBJ_ADD(op,v)		vecint_adduniq((op),(v))
#define	LISTOBJ_COUNT(op)		vecint_count((op))
#define	LISTOBJ_MKVEC(op,va)		vecint_mkvec((op),(va))
#define	LISTOBJ_CURBEGIN(op,cp)		vecint_curbegin((op),(cp))
#define	LISTOBJ_ENUM(op,cp,rp)		vecint_enum((op),(cp),(rp))
#define	LISTOBJ_CUREND(op,cp)		vecint_curend((op),(cp))
#endif /* CF_SETINT */

#define	TIM			txtindexmks
#define	TIM_PA			txtindexmks_pa
#define	TIM_TAG			txtindexmks_tag
#define	TIM_KEY			txtindexmks_key
#define	HDR			txtindexhdr


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef int (*idxtab_t)[3] ;
typedef int *	rectab_t ;


/* external subroutines */


/* external variables */


/* exported variables */

txtindexmks_obj	txtindexmks_modinfo = {
	"txtindexmks",
	sizeof(txtindexmks)
} ;


/* local structures */

struct vars {
	int		pagesize ;
	int		maxpathlen ;
} ;


/* forward references */

template<typename ... Args>
static int txtindexmks_ctor(txtindexmks *op,Args ... args) noex {
	TXTINDEXMKS	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->eigenp = new(nothrow) strtab) != np) {
	        if ((op->tagfilep = new(nothrow) bfile) != np) {
		    rs = SR_OK ;
		} /* end if (new-bfile) */
		if (rs < 0) {
		    delete op->eigenp ;
		    op->eigenp = nullptr ;
		}
	    } /* end if (new-strtab) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (txtindexmks_ctor) */

static int txtindexmks_dtor(txtindexmks *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->tagfilep) {
		delete op->tagfilep ;
		op->tagfilep = nullptr ;
	    }
	    if (op->eigenp) {
		delete op->eigenp ;
		op->eigenp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (txtindexmks_dtor) */

template<typename ... Args>
static inline int txtindexmks_magic(txtindexmks *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == TXTINDEXMKS_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (txtindexmks_magic) */

static int	txtindexmks_checkparams(TIM *) noex ;
static int	txtindexmks_idxdirbegin(TIM *) noex ;
static int	txtindexmks_idxdirend(TIM *) noex ;
static int	txtindexmks_filesbegin(TIM *) noex ;
static int	txtindexmks_filesbeginc(TIM *) noex ;
static int	txtindexmks_filesbeginwait(TIM *) noex ;
static int	txtindexmks_filesbeginopen(TIM *,cchar *) noex ;
static int	txtindexmks_filesend(TIM *) noex ;
static int	txtindexmks_listbegin(TIM *) noex ;
static int	txtindexmks_listend(TIM *) noex ;
static int	txtindexmks_addtag(TIM *,TIM_TAG *) noex ;
static int	txtindexmks_mkhash(TIM *) noex ;
static int	txtindexmks_mkhashwrmain(TIM *,HDR *) noex ;
static int	txtindexmks_mkhashwrhdr(TIM *,HDR *,
			filer *,int) noex ;
static int	txtindexmks_mkhashwrtab(TIM *,HDR *,
			filer *,int) noex ;
static int	txtindexmks_mkhashwreigen(TIM *,HDR *,
			filer *,int) noex ;
static int	txtindexmks_mkhashwrtabone(TIM *,HDR *,
			filer *,int,int *,int) noex ;
static int	txtindexmks_nhashopen(TIM *) noex ;
static int	txtindexmks_nhashclose(TIM *) noex ;
static int	txtindexmks_ntagclose(TIM *) noex ;
static int	txtindexmks_renamefiles(TIM *) noex ;

static int	mknewfname(char *,int,cchar *,cchar *) noex ;
static int	unlinkstale(cchar *,int) noex ;
static int	mkvars() noex ;


/* local variables */

static vars	var ;

constexpr char	zerobuf[4] = {
	0, 0, 0, 0 
} ;


/* exported variables */


/* exported subroutines */

int txtindexmks_open(TIM *op,TIM_PA *pp,cchar *db,int of,mode_t om) noex {
	int		rs ;
	int		f = false ;
	if ((rs = txtindexmks_ctor(op,pp,db)) >= 0) {
	    rs = SR_INVALID ;
	    if (db[0]) {
		static cint	rsv = mkvars() ;
		if ((rs = rsv) >= 0) {
	            op->om = (om | 0600) ;
	            op->nfd = -1 ;
	            op->pi = *pp ; /* copy the given parameters */
	            op->f.ofcreat = MKBOOL(of & O_CREAT) ;
	            op->f.ofexcl = MKBOOL(of & O_EXCL) ;
	            if (cchar *cp ; (rs = uc_mallocstrw(db,-1,&cp)) >= 0) {
	                op->dbname = cp ;
	                if ((rs = txtindexmks_checkparams(op)) >= 0) {
	                    if ((rs = txtindexmks_idxdirbegin(op)) >= 0) {
	                        if ((rs = txtindexmks_filesbegin(op)) >= 0) {
	                            f = rs ;
	                            if ((rs = txtindexmks_listbegin(op)) >= 0) {
	                                op->ti.maxtags = 0 ;
	                                op->magic = TXTINDEXMKS_MAGIC ;
	                            }
	                            if (rs < 0) {
	                                txtindexmks_filesend(op) ;
			            }
	                        } /* end if (files-begin) */
	                        if (rs < 0) {
	                            txtindexmks_idxdirend(op) ;
		                }
	                    } /* end if (txtindexmks_idxdirbegin) */
	                } /* end if (check-params) */
	                if (rs < 0) {
	                    if (op->dbname != nullptr) {
	                        uc_free(op->dbname) ;
	                        op->dbname = nullptr ;
	                    }
	                }
	            } /* end if (memory-allocation) */
		} /* end if (mkvars) */
	    } /* end if (valid) */
	    if (rs < 0) {
		txtindexmks_dtor(op) ;
	    }
	} /* end if (txtindexmks_ctor) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (txtindexmks_open) */

int txtindexmks_close(TIM *op) noex {
	int		rs ;
	int		rs1 ;
	int		clists = 0 ;
	if ((rs = txtindexmks_magic(op)) >= 0) {
	    bool	f_go  = (! op->f.abort) ;
	    {
	        rs1 = txtindexmks_ntagclose(op) ;
	        if (rs >= 0) rs = rs1 ;
	        if (f_go) f_go = (rs1 >= 0) ;
	    }
	    if ((op->ti.nkeys > 0) && f_go) {
	        rs1 = txtindexmks_mkhash(op) ;
	        if (rs >= 0) rs = rs1 ;
	        f_go = (rs1 >= 0) ;
	        clists = op->clists ;
	    }
	    {
	        rs1 = txtindexmks_listend(op) ;
	        if (rs >= 0) rs = rs1 ;
	        if (f_go) f_go = (rs1 >= 0) ;
	    }
	    if ((rs >= 0) && (op->ti.nkeys > 0) && f_go) {
	        rs1 = txtindexmks_renamefiles(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = txtindexmks_filesend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = txtindexmks_idxdirend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->dbname) {
	        rs1 = uc_free(op->dbname) ;
	        if (rs >= 0) rs = rs1 ;
	        op->dbname = nullptr ;
	    }
	    {
		rs1 = txtindexmks_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return (rs >= 0) ? clists : rs ;
}
/* end subroutine (txtindexmks_close) */

int txtindexmks_noop(TIM *op) noex {
	int		rs ;
	if ((rs = txtindexmks_magic(op)) >= 0) {
	    rs = SR_OK ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (txtindexmks_noop) */

int txtindexmks_abort(TIM *op) noex {
	int		rs ;
	if ((rs = txtindexmks_magic(op)) >= 0) {
	    op->f.abort = true ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (txtindexmks_abort) */

int txtindexmks_addeigens(TIM *op,TIM_KEY *eigens,int n) noex {
	int		rs ;
	if ((rs = txtindexmks_magic(op,eigens)) >= 0) {
	    rs = SR_INVALID ;
	    if (n >= 0) {
	        int	keylen = KEYBUFLEN ;
	        char	keybuf[KEYBUFLEN + 1] ;
		rs = SR_OK ;
	        for (int i = 0 ; i < n ; i += 1) {
	            cchar	*kp = charp(eigens[i].kp) ;
	            int		kl = eigens[i].kl ;
	            if (kl < 0) kl = strlen(kp) ;
	            if (hasuc(kp,kl)) {
	                if (kl > keylen) kl = keylen ;
	                strwcpylc(keybuf,kp,kl) ;
	                kp = keybuf ;
	            }
	            {
	                rs = strtab_add(op->eigenp,kp,kl) ;
	            }
	            if (rs < 0) break ;
	        } /* end for */
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (txtindexmks_addeigens) */

int txtindexmks_addtags(TIM *op,TIM_TAG *tags,int ntags) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = txtindexmks_magic(op,tags)) >= 0) {
	    for (int i = 0 ; i < ntags ; i += 1) {
	        rs = txtindexmks_addtag(op,(tags + i)) ;
	        c += rs ;
	        if (rs < 0) break ;
	    } /* end for */
	    if (rs >= 0) {
	        op->ti.ntags += ntags ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (txtindexmks_addtags) */


/* private subroutines */

static int txtindexmks_checkparams(TIM *op) noex {
	TIM_PA		*pp = &op->pi ;
	int		rs = SR_OK ;
	if (pp->tablen == 0) {
	    pp->tablen = TXTINDEXMKS_TABLEN ;
	}
	if (pp->minwlen == 0) {
	    pp->minwlen = TXTINDEXMKS_MINWLEN ;
	}
	if (pp->maxwlen == 0) {
	    pp->maxwlen = TXTINDEXMKS_MAXWLEN ;
	}
	if (pp->maxwlen > KEYBUFLEN) {
	    pp->maxwlen = KEYBUFLEN ;
	}
	return rs ;
}
/* end subroutine (txtindexmks_checkparams) */

static int txtindexmks_filesbegin(TIM *op) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	if (op->f.ofcreat) {
	    rs = txtindexmks_filesbeginc(op) ;
	} else {
	    rs = txtindexmks_filesbeginwait(op) ;
	    c = rs ;
	}
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (txtindexmks_filesbegin) */

static int txtindexmks_filesbeginc(TIM *op) noex {
	cint		type = (op->f.ofcreat && (! op->f.ofexcl)) ;
	cint		szinc = (var.maxpathlen + 1) ;
	int		rs ;
	int		rs1 ;
	int		ai = 0 ;
	cchar		*dbn = op->dbname ;
	cchar		*suf = FSUF_TAG	 ;
	char		*ap ;
	if ((rs = uc_malloc((szinc * 2),&ap)) >= 0) {
	    char	*tbuf = (ap + (szinc * ai++)) ;
	    if ((rs = mknewfname(tbuf,type,dbn,suf)) >= 0) {
	        cmode	om = op->om ;
	        cchar	*tfn = tbuf ;
	        char	*rbuf = (ap + (szinc * ai++)) ;
	        if (type) {
	            if ((rs = mktmpfile(rbuf,tbuf,om)) >= 0) {
	    	        op->f.created = true ;
	                tfn = rbuf ;
		    }
	        }
	        if (rs >= 0) {
	            bfile	*tfp = op->tagfilep ;
	            char	ostr[8] = "wc" ;
	            if (op->f.ofexcl) strcat(ostr,"e") ;
	            if ((rs = bopen(tfp,tfn,ostr,om)) >= 0) {
	                cchar	*cp ;
	    	        op->f.created = true ;
	                if ((rs = uc_mallocstrw(tfn,-1,&cp)) >= 0) {
	                    op->ntagfname = charp(cp) ;
	                }
	                if (rs < 0) {
	                    bclose(tfp) ;
		        }
	            } /* end if (bopen) */
		    if ((rs < 0) && type) {
		        uc_unlink(rbuf) ;
		    }
	        } /* end if (ok) */
	    } /* end if (mknewfname) */
	    rs1 = uc_free(ap) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end subroutine (txtindexmks_filesbeginc) */

static int txtindexmks_filesbeginwait(TIM *op) noex {
	int		rs ;
	int		c = 0 ;
	cchar		*dbn = op->dbname ;
	cchar		*suf = FSUF_TAG	 ;
	char		tbuf[MAXPATHLEN+1] ;
	if ((rs = mknewfname(tbuf,false,dbn,suf)) >= 0) {
		cint	to_stale = TXTINDEXMKS_INTSTALE ;
		cint	nrs = SR_EXISTS ;
		int	to = TXTINDEXMKS_INTOPEN ;
		while ((rs = txtindexmks_filesbeginopen(op,tbuf)) == nrs) {
		    c = 1 ;
		    sleep(1) ;
		    unlinkstale(tbuf,to_stale) ;
		    if (to-- == 0) break ;
		} /* end while (db exists) */
		if (rs == nrs) {
		    op->f.ofcreat = false ;
		    c = 0 ;
		    rs = txtindexmks_filesbeginc(op) ;
		}
	} /* end if (mknewfname) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (txtindexmks_filesbeginwait) */

static int txtindexmks_filesbeginopen(TIM *op,cchar *tfn) noex {
	bfile		*tfp = op->tagfilep ;
	cmode		om = op->om ;
	int		rs ;
	char		ostr[8] = "wce" ;
	if ((rs = bopen(tfp,tfn,ostr,om)) >= 0) {
	    cchar	*cp ;
	    op->f.created = true ;
	    if ((rs = uc_mallocstrw(tfn,-1,&cp)) >= 0) {
		op->ntagfname = (char *) cp ;
	    }
	    if (rs < 0) {
		bclose(tfp) ;
	    }
	} /* end if (bopen) */
	return rs ;
}
/* end subroutine (txtindexmks_filesbeginopen) */

static int txtindexmks_filesend(TIM *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->f.tagopen) {
	    op->f.tagopen = false ;
	    rs1 = bclose(op->tagfilep) ;
	    if (rs >= 0) rs = rs1 ;
	}
	if (op->ntagfname != nullptr) {
	    if (op->f.created && (op->ntagfname[0] != '\0')) {
	        u_unlink(op->ntagfname) ;
	    }
	    rs1 = uc_free(op->ntagfname) ;
	    if (rs >= 0) rs = rs1 ;
	    op->ntagfname = nullptr ;
	}
	if (op->nidxfname != nullptr) {
	    if (op->f.created && (op->nidxfname[0] != '\0')) {
	        u_unlink(op->nidxfname) ;
	    }
	    rs1 = uc_free(op->nidxfname) ;
	    if (rs >= 0) rs = rs1 ;
	    op->nidxfname = nullptr ;
	}
	return rs ;
}
/* end subroutine (txtindexmks_filesend) */

static int txtindexmks_idxdirbegin(TIM *op) noex {
	int		rs = SR_INVALID ;
	int		dnl ;
	cchar		*dnp ;
	if ((dnl = sfdirname(op->dbname,-1,&dnp)) >= 0) {
	    char	tbuf[MAXPATHLEN + 1] ;
	    if (dnl == 0) {
	        rs = getpwd(tbuf,MAXPATHLEN) ;
	        dnl = rs ;
	    } else {
	        rs = mkpath1w(tbuf,dnp,dnl) ;
	    }
	    if (rs >= 0) {
	        int	am = (X_OK | W_OK) ;
	        if ((rs = perm(tbuf,-1,-1,nullptr,am)) >= 0) {
	            cchar	*cp ;
	            if ((rs = uc_mallocstrw(tbuf,dnl,&cp)) >= 0) {
	                op->idname = cp ;
	            }
	        }
	    }
	}
	return rs ;
}
/* end subroutine (txtindexmks_idxdirbegin) */

static int txtindexmks_idxdirend(TIM *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->idname != nullptr) {
	    rs1 = uc_free(op->idname) ;
	    if (rs >= 0) rs = rs1 ;
	    op->idname = nullptr ;
	}
	return rs ;
}
/* end subroutine (txtindexmks_idxdirend) */

static int txtindexmks_listbegin(TIM *op) noex {
	int		sz = int(op->pi.tablen * sizeof(LISTOBJ)) ;
	int		rs ;
	void		*vp ;
	if ((rs = uc_malloc(sz,&vp)) >= 0) {
	    LISTOBJ	*lop = (LISTOBJ *) vp ;
	    cint	lo = LISTOBJ_OORDERED ;
	    int		n = 0 ;
	    int		idx = 0 ;
	    op->lists = vp ;
	    if ((lo >= 0) && (n >= 0)) {
		cint	tablen = int(op->pi.tablen) ;
	        for (int i = 0 ; (rs >= 0) && (i < tablen) ; i += 1) {
	            if ((rs = LISTOBJ_START((lop+i),n,lo)) >= 0) {
			idx += 1 ;
		    }
	        } /* end for */
	    } else {
	        rs = SR_BUGCHECK ;
	    }
	    if (rs >= 0) {
	        sz = TXTINDEXMKS_EIGENSIZE ;
	        rs = strtab_start(op->eigenp,sz) ;
	    }
	    if (rs < 0) {
	        for (int j = 0 ; j <= idx ; j += 1) {
	            LISTOBJ_FINISH(lop+j) ;
	        }
	    }
	    if (rs < 0) {
	        uc_free(op->lists) ;
	        op->lists = nullptr ;
	    }
	} /* end if (m-a) */
	return rs ;
}
/* end subroutine (txtindexmks_listbegin) */

static int txtindexmks_listend(TIM *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    rs1 = strtab_finish(op->eigenp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	{
	    LISTOBJ	*lop = (LISTOBJ *) op->lists ;
	    cint	tablen = int(op->pi.tablen) ;
	    for (int i = 0 ; i < tablen ; i += 1) {
	        rs1 = LISTOBJ_FINISH(lop+i) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	}
	if (op->lists != nullptr) {
	    rs1 = uc_free(op->lists) ;
	    if (rs >= 0) rs = rs1 ;
	    op->lists = nullptr ;
	}
	return rs ;
}
/* end subroutine (txtindexmks_listend) */

static int txtindexmks_addtag(TIM *op,TIM_TAG *tagp) noex {
	LISTOBJ		*lop = (LISTOBJ *) op->lists ;
	bfile		*tfp = op->tagfilep ;
	uint		tagoff ;
	int		rs = SR_OK ;
	int		roff = tagp->recoff ;
	int		rlen = tagp->reclen ;
	int		c = 0 ;
	cchar		*fmt = "%s:%u,%u\n" ;
	cchar		*fp ;

/* write out the tag to the tag-file */

	tagoff = op->tagoff ;
	fp = (tagp->fname != nullptr) ? tagp->fname : zerobuf ;
	if ((rs = bprintf(tfp,fmt,fp,roff,rlen)) >= 0) {
	    uint	hv ;
	    cint	maxkl = MIN(op->pi.maxwlen,KEYBUFLEN) ;
	    int		kl ;
	    int		hi ;
	    cchar	*kp ;
	    char	keybuf[KEYBUFLEN + 1] ;
	    op->tagoff += rs ;

/* process the keys associated with this tag */

	    cint	nkeys = int(tagp->nkeys) ;
	    for (int i = 0 ; i < nkeys ; i += 1) {
		cint	minwlen = int(op->pi.minwlen) ;
	        kp = tagp->keys[i].kp ;
	        kl = tagp->keys[i].kl ;

/* overflow and maximum word length management */

	        if (kl >= 0) {
	            if (kl > maxkl) {
	                kl = maxkl ;
		    }
	        } else {
	            kl = strnlen(kp,maxkl) ;
		}
	        if (kl < minwlen) continue ;

/* convert to lower-case as necessary */

	        if (hasuc(kp,kl)) {
	            strwcpylc(keybuf,kp,kl) ; /* can't overflow (see 'maxkl') */
	            kp = keybuf ;
	        }

/* hash it */

	        hv = hash_elf(kp,kl) ;

	        hi = (hv % op->pi.tablen) ;

	        if ((rs = LISTOBJ_ADD((lop+hi),tagoff)) >= 0) {
	            if (rs != INT_MAX) {
	                c += 1 ;
	                op->ti.nkeys += 1 ;
	            }
	        }

	        if (rs < 0) break ;
	    } /* end for (processing keys) */

	} /* end if (bprintf) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (txtindexmks_addtag) */

static int txtindexmks_mkhash(TIM *op) noex {
	int		rs ;
	int		rs1 ;
	int		clists = 0 ;
	if ((rs = txtindexmks_nhashopen(op)) >= 0) {
	    HDR		hdr{} ;
	    hdr.vetu[0] = TXTINDEXHDR_VERSION ;
	    hdr.vetu[1] = ENDIAN ;
	    hdr.vetu[2] = 0 ;
	    hdr.vetu[3] = 0 ;
	    hdr.wtime = (uint) time(nullptr) ;
	    hdr.tfsize = op->tagsize ;
	    hdr.taglen = op->ti.ntags ;
	    hdr.minwlen = op->pi.minwlen ;
	    hdr.maxwlen = op->pi.maxwlen ;
	    hdr.tablen = op->pi.tablen ;
	    if ((rs = txtindexmks_mkhashwrmain(op,&hdr)) >= 0) {
	        cint	hlen = HDRBUFLEN ;
	        char	hbuf[HDRBUFLEN+1] ;
	        hdr.hfsize = rs ;
	        if ((rs = txtindexhdr_rd(&hdr,hbuf,hlen)) >= 0) {
	            cint	bl = rs ;
	            if ((rs = u_pwrite(op->nfd,hbuf,bl,0L)) >= 0) {
	                cmode	om = op->om ;
	                rs = uc_fminmod(op->nfd,om) ;
	            }
	        }
	    } /* end if (txtindexmks_mkhashwrmain) */
	    rs1 = txtindexmks_nhashclose(op) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (txtindexmks_nhash) */
	return (rs >= 0) ? clists : rs ;
}
/* end subroutine (txtindexmks_mkhash) */

static int txtindexmks_mkhashwrmain(TIM *op,HDR *hdrp) noex {
	filer		hf, *hfp = &hf ;
	cint		nfd = op->nfd ;
	cint		ps = var.pagesize ;
	int		bsize ;
	int		rs ;
	int		rs1 ;
	int		off = 0 ;
	bsize = (ps * 4) ;
	if ((rs = filer_start(hfp,nfd,0,bsize,0)) >= 0) {
	    if ((rs = txtindexmks_mkhashwrhdr(op,hdrp,hfp,off)) >= 0) {
	        off += rs ;
/* write SDN string */
	        if ((rs >= 0) && (op->pi.sdn != nullptr)) {
	            hdrp->sdnoff = off ;
	            rs = filer_writefill(hfp,op->pi.sdn,-1) ;
	            off += rs ;
	        }
/* write SFN string */
	        if ((rs >= 0) && (op->pi.sfn != nullptr)) {
	            hdrp->sfnoff = off ;
	            rs = filer_writefill(hfp,op->pi.sfn,-1) ;
	            off += rs ;
	        }
/* write out the lists while creating the offset table */
	        if (rs >= 0) {
	            rs = txtindexmks_mkhashwrtab(op,hdrp,hfp,off) ;
	            off += rs ;
	        }
/* make and write out the eigen: string, record, and index tables */
	        if (rs >= 0) {
	            rs = txtindexmks_mkhashwreigen(op,hdrp,hfp,off) ;
	            off += rs ;
	        }
	    } /* end if (txtindexmks_mkhashwrhdr) */
	    rs1 = filer_finish(hfp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (filer) */
	return (rs >= 0) ? off : rs ;
}
/* end subroutine (txtindexmks_mkhashwrmain) */

static int txtindexmks_mkhashwrhdr(TIM *op,HDR *hdrp,filer *hfp,int off) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ;
	(void) off ;
	if (op) {
	    cint	hlen = HDRBUFLEN ;
	    char	hbuf[HDRBUFLEN+1] ;
	    if ((rs = txtindexhdr_rd(hdrp,hbuf,hlen)) >= 0) {
	        cint	bl = rs ;
	        rs = filer_writefill(hfp,hbuf,bl) ;
	        wlen += rs ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (txtindexmks_mkhashwrhdr) */

static int txtindexmks_mkhashwrtab(TIM *op,HDR *hdrp,filer *hfp,int off) noex {
	int		*table = nullptr ;
	cint		tsize = op->pi.tablen * sizeof(uint) ;
	int		rs ;
	int		wlen = 0 ;
	if ((rs = uc_malloc(tsize,&table)) >= 0) {
	    cint	tablen = int(op->pi.tablen) ;
	    hdrp->listoff = off ;
	    for (int i = 0 ; (rs >= 0) && (i < tablen) ; i += 1) {
	        rs = txtindexmks_mkhashwrtabone(op,hdrp,hfp,off,table,i) ;
	        off += rs ;
	        wlen += rs ;
	    } /* end for (table lists) */
	    hdrp->maxtags = op->ti.maxtags ;
	    hdrp->taboff = off ;
	    if (rs >= 0) {
	        cint	tsize = op->pi.tablen * sizeof(uint) ;
	        rs = filer_write(hfp,table,tsize) ;
	        off += rs ;
	        wlen += rs ;
	    }
	    uc_free(table) ;
	} /* end if (m-a) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (txtindexmks_mkhashwrtab) */

static int txtindexmks_mkhashwrtabone(TIM *op,HDR *hdrp,
		filer *hfp,int off,int *tab,int i) noex {
	LISTOBJ		*lop = (LISTOBJ *) op->lists ;
	int		rs ;
	int		wlen = 0 ;
	(void) hdrp ;
	lop += i ;
	if ((rs = LISTOBJ_COUNT(lop)) > 0) {
	    cint	c = rs ;
	    cint	asize = ((c+1)*sizeof(int)) ;
	    int		*va ;
	    if ((rs = uc_malloc(asize,&va)) >= 0) {
	        if ((rs = LISTOBJ_MKVEC(lop,va)) >= 0) {
		    cint	maxtags = int(op->ti.maxtags) ;
	            if (c > maxtags) {
			op->ti.maxtags = c ;
		    }
	            if (c > 0) {
	                cint	csize = sizeof(int) ;
	                op->clists += 1 ;
	                tab[i] = off ;
	                if ((rs = filer_write(hfp,&c,csize)) >= 0) {
	                    off += rs ;
	                    wlen += rs ;
	                    if (c > 0) {
	                        cint	vsize = (c*sizeof(uint)) ;
	                        rs = filer_write(hfp,va,vsize) ;
	                        off += rs ;
	                        wlen += rs ;
	                    }
	                } /* end if (filer_write) */
	            } /* end if (positive) */
	        } /* end if (LISTINT_MKVEC) */
	        uc_free(va) ;
	    } /* end if (m-a) */
	} else if (rs == 0) {
	    tab[i] = 0 ;
	} /* end if (LISTOBJ_COUNT) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (txtindexmks_mkhashwrtabone) */

static int txtindexmks_mkhashwreigen(TIM *op,HDR *hdrp,
		filer *hfp,int off) noex {
	strtab		*elp = op->eigenp ;
	int		rs ;
	int		wlen = 0 ;
	if ((rs = strtab_strsize(elp)) >= 0) {
	    cint	nskip = TXTINDEXMKS_NSKIP ;
	    int		essize = rs ;
	    char	*estab = nullptr ;
	    hdrp->eiskip = nskip ;
	    if (essize > 0) {
	        if ((rs = uc_malloc(essize,&estab)) >= 0) {
	            hdrp->esoff = off ;
	            hdrp->essize = essize ;
	            if ((rs = strtab_strmk(elp,estab,essize)) >= 0) {
	                rs = filer_write(hfp,estab,essize) ;
	                off += rs ;
	                wlen += rs ;
	            }
	            uc_free(estab) ;
	        } /* end if (eigen-string table) */

	        if (rs >= 0) {
	            int		ersize = 0 ;
	            int		n ;
	            int		erlen ;
	            int		*ertab = nullptr ;

	            n = strtab_count(op->eigenp) ;
	            erlen = (n+1) ;

	            if ((rs = strtab_recsize(op->eigenp)) >= 0) {
	                void	*vp ;
	                ersize = rs ;
	                if ((rs = uc_malloc(ersize,&vp)) >= 0) {
	                    ertab = rectab_t(vp) ;
	                    hdrp->ersize = ersize ;
	                    hdrp->eroff = off ;
	                    hdrp->erlen = erlen ;
	                    if ((rs = strtab_recmk(elp,ertab,ersize)) >= 0) {
	                        rs = filer_write(hfp,ertab,ersize) ;
	                        off += rs ;
	                        wlen += rs ;
	                    }
	                    uc_free(ertab) ;
	                } /* end if (memory-allocation) */
	            } /* end if (recsize) */

	        } /* end if (eigen-record table) */

	        if (rs >= 0) {
	            int		eisize = 0 ;
	            int		eilen ;
	            int		(*eitab)[3] = nullptr ;

	            eilen = strtab_indlen(elp) ;

	            if ((rs = strtab_indsize(elp)) >= 0) {
	                void	*vp ;
	                eisize = rs ;
	                if ((rs = uc_malloc(eisize,&vp)) >= 0) {
	                    eitab = idxtab_t(vp) ;

	                    hdrp->eisize = eisize ;
	                    hdrp->eioff = off ;
	                    hdrp->eilen = eilen ;
	                    rs = strtab_indmk(elp,eitab,eisize,nskip) ;

	                    if (rs >= 0) {
	                        rs = filer_write(hfp,eitab,eisize) ;
	                        off += rs ;
	                        wlen += rs ;
	                    }

	                    uc_free(eitab) ;
	                } /* end if (memory-allocation) */
	            } /* end if (indsize) */

	        } /* end if (eigen-index table) */

	    } /* end if (positive) */
	} /* end if (strtab_strsize) */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (txtindexmks_mkhashwreigen) */

static int txtindexmks_nhashopen(TIM *op) noex {
	cint		type = (op->f.ofcreat && (! op->f.ofexcl)) ;
	int		rs ;
	cchar		*dbn = op->dbname ;
	cchar		*suf = FSUF_IND ;
	char		tbuf[MAXPATHLEN+1] ;
	if ((rs = mknewfname(tbuf,type,dbn,suf)) >= 0) {
	    cmode	om = op->om ;
	    int		of = (O_CREAT|O_WRONLY) ; /* <- writable */
	    cchar	*tfn = tbuf ;
	    char	rbuf[MAXPATHLEN+1] ;
	    if (type) {
	        rs = opentmpfile(tbuf,of,om,rbuf) ;
	        op->nfd = rs ;
	        tfn = rbuf ;
	    } else {
	        if (op->f.ofexcl) of |= O_EXCL ;
	        rs = uc_open(tbuf,of,om) ;
	        op->nfd = rs ;
	    }
	    if (rs >= 0) {
	        cchar	*cp ;
	        if ((rs = uc_mallocstrw(tfn,-1,&cp)) >= 0) {
	            op->nidxfname = (char *) cp ;
	        }
	    } /* end if (ok) */
	} /* end if (mknewfname) */
	return rs ;
}
/* end subroutine (txtindexmks_nhashopen) */

static int txtindexmks_nhashclose(TIM *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->nfd >= 0) {
	    rs1 = u_close(op->nfd) ;
	    if (rs >= 0) rs = rs1 ;
	    op->nfd = -1 ;
	}
	return rs ;
}
/* end subroutine (txtindexmks_nhashclose) */

static int txtindexmks_renamefiles(TIM *op) noex {
	int		rs ;
	cchar		*ends = ENDIANSTR ;
	char		hashfname[MAXPATHLEN + 1] ;

	if ((rs = mkfnamesuf2(hashfname,op->dbname,FSUF_IND,ends)) >= 0) {
	    char	tagfname[MAXPATHLEN + 1] ;
	    if ((rs = mkfnamesuf1(tagfname,op->dbname,FSUF_TAG)) >= 0) {

	        if ((rs = u_rename(op->ntagfname,tagfname)) >= 0) {
	            op->ntagfname[0] = '\0' ;
	            if ((rs = u_rename(op->nidxfname,hashfname)) >= 0) {
	                op->nidxfname[0] = '\0' ;
		    }
		}
		if (rs < 0) {
	            if (op->ntagfname[0] != '\0') {
	                u_unlink(op->ntagfname) ;
	                op->ntagfname[0] = '\0' ;
	            }
	            if (op->nidxfname[0] != '\0') {
	                u_unlink(op->nidxfname) ;
	                op->nidxfname[0] = '\0' ;
	            }
		}

	    } /* end if (ok) */
	} /* end if (ok) */

	return rs ;
}
/* end subroutine (txtindexmks_renamefiles) */

int txtindexmks_ntagclose(TIM *op) noex {
	int		rs ;
	int		rs1 ;
	int		tagsize = 0 ;
	if ((rs = bsize(op->tagfilep)) >= 0) {
	    mode_t	om = op->om ;
	    op->tagsize = rs ;
	    tagsize = rs ;
	    rs = bcontrol(op->tagfilep,BC_MINMOD,om) ;
	} /* end if (bsize) */
	{
	    rs1 = bclose(op->tagfilep) ;
	    if (rs >= 0) rs = rs1 ;
	    op->f.tagopen = false ;
	}
	return (rs >= 0) ? tagsize : rs ;
}
/* end if (txtindexmks_ntagclose) */

#ifdef	COMMENT

static int txtindexmks_printeigen(TIM *op) noex {
	strtab		*edp = op->eigenp ;
	int		rs ;
	if ((rs = strtab_count(edp)) >= 0) {
	    int		erlen = (rs+1) ;
	    if ((rs = strtab_strsize(edp)) >= 0) {
		int	essize = rs ;
		char	*estab ;
		if ((rs = uc_malloc(essize,&estab)) >= 0) {
		    if ((rs = strtab_strmk(edp,estab,essize)) >= 0) {
			if ((rs = strtab_recsize(edp)) >= 0) {
			    int	ersize = rs ;
			    int	*ertab ;
			    if ((rs = uc_malloc(ersize,&ertab)) >= 0) {
				struct printeigen	a ;
				a.ertab = ertab ;
				a.estab = estab ;
				a.ersize = ersize ;
				a.erlen = erlen ;
				rs = txtindexmks_printeigener(op,&a) ;
				uc_free(ertab) ;
			    } /* end if (m-a) */
			}
		    }
		    uc_free(estab) ;
		} /* end if (m-a) */
	    }
	}
	return rs ;
}
/* end subroutine (txtindexmks_printeigen) */

static int txtindexmks_printeigener(TIM *op,printeigen *ap) noex {
	strtab		*edp = op->eigenp ;
	int		rs ;
	int		rs1 ;
	int		*ertab = ap->ertab ;
	char		*estab = ap->estab ;
	int		ersize = ap->ersize ;
	int		erlen = ap->erlen ;
	if ((rs = strtab_recmk(edp,ertab,ersize)) >= 0) {
	    if ((rs = strtab_indsize(edp)) >= 0) {
		int	(*eitab)[3] ;
		int	eisize = rs ;
		int	eilen = strtab_indlen(edp) ;
		if ((rs = uc_malloc(eisize,&eitab)) >= 0) {
		    int	ns = TXTINDEXMKS_NSKIP ;
		    if ((rs = strtab_indmk(edp,eitab,eisize,ns)) >= 0) {
			int	i ;
			int	si ;
			int	sl ;
			cchar	*sp ;
			cchar	*fmt ;
			fmt = "txtindexmks_printeigen: i=%u si=%u s=%s\n" ;
			for (i = 1 ; i < erlen ; i += 1) {
	    		    si = ertab[i] ;
	    		    if (si > 0) {
	        		debugprintf(fmt,i,si,(estab + si)) ;
			    }
	    		    sp = (estab + si) ;
	    		    sl = strlen(sp) ;
	    		    rs1 = strtabfind(estab,eitab,eilen,ns,sp,sl) ;
	    		    fmt = "txtindexmks_printeigen: stabfind() rs=%d\n";
	    		    debugprintf(fmt,rs1) ;
			} /* end for */
		    }
		    uc_free(eitab) ;
		} /* end if (m-a) */
	    }
	}
	return rs ;
}
/* end subroutine (txtindexmks_printeigener) */

#endif /* COMMENT */

static int mknewfname(char *tbuf,int type,cchar *dbn,cchar *suf) noex {
	cchar		*end = ENDIANSTR ;
	cchar		*fin = (type) ? "xXXXX" : "n" ;
	return mkfnamesuf3(tbuf,dbn,suf,end,fin) ;
}
/* end subroutine (mknewfname) */

static int unlinkstale(cchar *fn,int to) noex {
	USTAT		sb ;
	custime		dt = getustime ;
	int		rs ;
	if ((rs = uc_stat(fn,&sb)) >= 0) {
	    if ((dt-sb.st_mtime) >= to) {
		uc_unlink(fn) ;
		rs = 1 ;
	    } else {
	        rs = 0 ;
	    }
	} else if (isNotPresent(rs)) {
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (unlinkstale) */

static int mkvars() noex {
	int		rs ;
	if ((rs = uc_pagesize()) >= 0) {
	    var.pagesize = rs ;
	    if ((rs = getbufsize(getbufsize_mp)) >= 0) {
	        var.maxpathlen = rs ;
	    } /* end if (getbufsize) */
	} /* end if (uc_pagesize) */
	return rs ;
}
/* end subroutine (mkvars) */

#ifdef	COMMENT

static int checksize(cchar *s,int rs,int sz) noex {
	if (rs != sz) {
	    debugprintf("txtindexmks/checksize: %s mismatch rs=%d size=%d\n",
	        s,rs,sz) ;
	}
	return 0 ;
}
/* end subroutine (checksize) */

static int checkalign(cchar *s,uint off) noex {
	if ((off & 3) != 0) {
	    debugprintf("txtindexmks/checkalign: %s misalign off=%08lx\n",
	        s,off) ;
	}
	return 0 ;
}
/* end subroutine (checkalign) */

#endif /* COMMENT */


