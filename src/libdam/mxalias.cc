/* mxalias SUPPORT (Mail-X Alies) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* manage a MXALIAS (Mail-X Alias) object */
/* version %I% last-modified %G% */

#define	CF_FILECHECK	0		/* enable file-checking */
#define	CF_FILEADD	1		/* files-add */
#define	CF_FILESYS	1		/* system-DB */
#define	CF_FILEUSER	1		/* user-DB */

/* revision history:

	= 2003-06-11, David A­D­ Morano
	I snarfed this code from some hardware research use since
	it seemed be a pretty good match for the present algorithm
	needs.  We will see how it works out! :-|

*/

/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	mxalias

	Description:
	This module is used to manage a MXALIAS (Mail-X Alias) object.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<getbufsize.h>
#include	<getax.h>
#include	<getusername.h>
#include	<getuserhome.h>
#include	<mallocxx.h>
#include	<ids.h>
#include	<vecstr.h>
#include	<vecobj.h>
#include	<bfile.h>
#include	<field.h>
#include	<nulstr.h>
#include	<sfx.h>
#include	<sncpyx.h>
#include	<mkpathx.h>
#include	<strwcpy.h>
#include	<matstr.h>
#include	<char.h>
#include	<isnot.h>
#include	<localmisc.h>		/* |REALNAMELEN| */

#include	"mxalias.h"

import libutil ;

/* local defines */

#define	MXALIAS_FILE	mxalias_file
#define	MXALIAS_SYSDB	"/etc/mail/mailx.rc"
#define	MXALIAS_USERDB	".mailrc"
#define	MXALIAS_KEYWORD	"alias"

#define	MA		mxalias
#define	MA_FI		mxalias_file
#define	MA_CUR		mxalias_cur

#define	BD		struct bufdesc

#define	TO_APROFILE	(1 * 60)
#define	TO_FILECOME	15		/* timeout for file to "come in" */
#define	TO_LOCK		(5 * 60)
#define	TO_OPEN		(60 * 60)
#define	TO_ACCESS	(1 * 60)
#define	TO_CHECK	5		/* minimum check interval */
#define	TO_FILECHANGED	5		/* DB file check */
#define	TO_FILEOLD	10		/* backing-store check */

#define	KEYBUFLEN	REALNAMELEN

#ifndef	CF_FILEADD
#define	CF_FILEADD	1		/* files-add */
#endif
#ifdef	CF_FILESYS
#define	CF_FILESYS	1		/* system-DB */
#endif
#ifndef	CF_FILEUSER
#define	CF_FILEUSER	1		/* user-DB */
#endif


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
	int		maxlinelen ;
	operator int () noex ;
    } ; /* end struct (vars) */
}

struct mxalias_file {
	cchar		*fname ;
	ino_t		ino ;
	time_t		timod ;
	dev_t		dev ;
	size_t		fsize ;
} ;

struct bufdesc {
	cchar		*a ;		/* memory allocation */
	char		*lbuf ;
	char		*fbuf ;
	char		*kbuf ;
	int		llen ;
	int		flen ;
	int		klen ;
} ;


/* forward references */

template<typename ... Args>
static int mxalias_ctor(mxalias *op,Args ... args) noex {
    	MXALIAS		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->flp = new(nothrow) vecobj) != np) {
	        if ((op->elp = new(nothrow) keyvals) != np) {
		    rs = SR_OK ;
		}
		if (rs < 0) {
		    delete op->flp ;
		    op->flp = nullptr ;
		}
	    } /* end if (new-vecobj) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mxalias_ctor) */

static int mxalias_dtor(mxalias *op) noex {
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
/* end subroutine (mxalias_dtor) */

template<typename ... Args>
static inline int mxalias_magic(mxalias *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == MXALIAS_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (mxalias_magic) */

static int	mxalias_username(MA *,cchar *) noex ;
static int	mxalias_userdname(MA *) noex ;
static int	mxalias_mkuserfname(MA *,char *) noex ;
static int	mxalias_curlooks(MA *,MA_CUR *,cc *,int) noex ;
static int	mxalias_filesadd(MA *,time_t) noex ;
static int	mxalias_fileadd(MA *,cchar *) noex ;
static int	mxalias_filereg(MA *,USTAT *,cchar *) noex ;
static int	mxalias_fileparse(MA *,int) noex ;
static int	mxalias_fileparser(MA *,int,bfile *) noex ;
static int	mxalias_fileparseln(MA *,int,BD *,cchar *,int) noex ;
static int	mxalias_filedump(MA *,int) noex ;
static int	mxalias_filedels(MA *) noex ;
static int	mxalias_filedel(MA *,int) noex ;
static int	mxalias_finents(MA *) noex ;
static int	mxalias_mkvals(MA *,MA_CUR *,vecstr *) noex ;
static int	mxalias_addvals(MA *,vecstr *,vecstr *,cchar *) noex ;
static int	mxalias_finallocs(MA *) noex ;

static int	mxalias_fileparseln_alias(MA *,int,BD *,field *) noex ;
static int	mxalias_fileparseln_unalias(MA *,int,BD *,field *) noex ;
static int	mxalias_fileparseln_source(MA *,int,BD *,field *) noex ;

#ifdef	COMMENT
static int	mxalias_filealready(MA *,dev_t,ino_t) noex ;
#endif

#if	CF_FILECHECK
static int	mxalias_filechecks(MA *,time_t) noex ;
#endif

static int	file_start(MA_FI *,USTAT *,cchar *) noex ;
static int	file_finish(MA_FI *) noex ;

static int	bufdesc_start(BD *,int) noex ;
static int	bufdesc_finish(BD *) noex ;

static int	cmpfe(MA_FI *,MA_FI *) noex ;

extern "C" {
    static int	vcmpfe(cvoid **,cvoid **) noex ;
}

static bool	isnotspecial(int) noex ;
static bool	isOurFileType(mode_t) noex ;


/* local variables */

constexpr char		kterms[] = {
	0x00, 0x26, 0x00, 0x00,
	0x09, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ;

/* all white space plus comma (',') */
constexpr char		vterms[] = {
	0x00, 0x26, 0x00, 0x00,
	0x09, 0x10, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ;

enum keywords {
	keyword_alias,
	keyword_group,
	keyword_unalias,
	keyword_ungroup,
	keyword_source,
	keyword_overlast
} ;

constexpr cpcchar	keywords[] = {
	"alias",
	"group",
	"unalias",
	"ungroup",
	"source",
	nullptr
} ;

static vars		var ;

constexpr bool		f_fileadd = CF_FILEADD ;
constexpr bool		f_filesys = CF_FILESYS ;
constexpr bool		f_fileuser = CF_FILEUSER ;
constexpr bool		f_comment = false ;


/* exported variables */


/* exported subroutines */

int mxalias_open(MA *op,cchar *pr,cchar *username) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = mxalias_ctor(op,pr,username)) >= 0) {
	    static cint		rsv = var ;
	    if ((rs = rsv) >= 0) {
	        if (cchar *cp{} ; (rs = uc_mallocstrw(pr,-1,&cp)) >= 0) {
	            op->pr = cp ;
	            if ((rs = mxalias_username(op,username)) >= 0) {
		        cint	vn = 5 ;
	                cint	vo = (VECOBJ_OSTATIONARY | VECOBJ_OREUSE) ;
	                cint	sz = szof(MA_FI) ;
	                if ((rs = vecobj_start(op->flp,sz,vn,vo)) >= 0) {
	                    if ((rs = keyvals_start(op->elp,vn)) >= 0) {
	                        custime		dt = getustime ;
	                        op->ti_access = dt ;
	                        op->ti_check = dt ;
	                        op->magic = MXALIAS_MAGIC ;
				if_constexpr (f_fileadd) {
	                            if ((rs = mxalias_filesadd(op,dt)) >= 0) {
	                                c = rs ;
	                            }
				} /* end if_constexpr (f_fileadd) */
	                        if (rs < 0) {
	                            mxalias_finents(op) ;
	                            keyvals_finish(op->elp) ;
	                            op->magic = 0 ;
	                        } /* end if (error handling) */
	                    } /* end if (entries) */
	                    if (rs < 0) {
	                        mxalias_filedels(op) ;
	                        vecobj_finish(op->flp) ;
	                    }
	                } /* end if (files) */
	            } /* end if (username) */
	            if (rs < 0) {
	                mxalias_finallocs(op) ;
		    }
	        } /* end if (program-root) */
	    } /* end if (vars) */
	    if (rs < 0) {
		mxalias_dtor(op) ;
	    }
	} /* end if (mxalias_ctor) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mxalias_open) */

int mxalias_close(MA *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = mxalias_magic(op)) >= 0) {
	    {
	        rs1 = mxalias_finents(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->elp) {
	        rs1 = keyvals_finish(op->elp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = mxalias_filedels(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->flp) {
	        rs1 = vecobj_finish(op->flp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = mxalias_finallocs(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = mxalias_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mxalias_close) */

int mxalias_audit(MA *op) noex {
	int		rs ;
	if ((rs = mxalias_magic(op)) >= 0) {
	    custime	dt = getustime ;
	    rs = keyvals_check(op->elp,dt) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mxalias_audit) */

int mxalias_count(MA *op) noex {
	int		rs ;
	if ((rs = mxalias_magic(op)) >= 0) {
	    rs = keyvals_count(op->elp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mxalias_count) */

int mxalias_curbegin(MA *op,MA_CUR *curp) noex {
	int		rs ;
	if ((rs = mxalias_magic(op,curp)) >= 0) {
	    rs = memclear(curp) ;
	    curp->i = -1 ;
	    curp->vbuf = nullptr ;
	    curp->vals = nullptr ;
	    curp->nvals = 0 ;
	    curp->magic = MXALIAS_MAGIC ;
	    op->ncursors += 1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mxalias_curbegin) */

int mxalias_curend(MA *op,MA_CUR *curp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = mxalias_magic(op,curp)) >= 0) {
	    if (op->fl.cursoracc) {
	        op->ti_access = getustime ;
	    }
	    curp->i = -1 ;
	    curp->nvals = 0 ;
	    if (curp->vbuf) {
	        rs1 = uc_free(curp->vbuf) ;
	        if (rs >= 0) rs = rs1 ;
	        curp->vbuf = nullptr ;
	    }
	    if (curp->vals) {
	        rs1 = uc_free(curp->vals) ;
	        if (rs >= 0) rs = rs1 ;
	        curp->vals = nullptr ;
	    }
	    if (curp->kvcp) {
	        {
	            rs1 = keyvals_curend(op->elp,curp->kvcp) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	        {
	            rs1 = uc_free(curp->kvcp) ;
	            if (rs >= 0) rs = rs1 ;
	            curp->kvcp = nullptr ;
	        }
	    }
	    op->ncursors -= 1 ;
	    curp->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mxalias_curend) */

int mxalias_curenum(MA *op,MA_CUR *curp,char *kbuf,int klen,
		char *vbuf,int vlen) noex {
	int		rs ;
	int		kl = 0 ;
	if ((rs = mxalias_magic(op,curp,kbuf)) >= 0) {
	    rs = SR_INVALID ;
	    if ((curp->magic == MXALIAS_MAGIC) && (op->ncursors > 0)) {
	        keyvals_cur	*kvcp = curp->kvcp ;
		rs = SR_OK ;
	        if (kvcp == nullptr) {
	            cint	sz = szof(keyvals_cur) ;
	            if ((rs = uc_malloc(sz,&kvcp)) >= 0) {
	                curp->kvcp = kvcp ;
	                rs = keyvals_curbegin(op->elp,kvcp) ;
	                if (rs < 0) {
	                    uc_free(kvcp) ;
	                    curp->kvcp = nullptr ;
	                } /* end if (error-handling) */
	            } /* end if (memory-allocation) */
	        } /* end if (was empty) */
	        if (rs >= 0) {
	            cchar	*kp{} ;
	            cchar	*vp{} ;
	            if ((rs = keyvals_curenum(op->elp,kvcp,&kp,&vp)) >= 0) {
	                kl = rs ;
			if (kp) {
	                    rs = sncpy1(kbuf,klen,kp) ;
			}
	                if ((rs >= 0) && vbuf && vp) {
	                    rs = sncpy1(vbuf,vlen,vp) ;
		        }
	            } /* end if */
	        } /* end if (ok) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? kl : rs ;
}
/* end subroutine (mxalias_curenum) */

/* lookup an entry by key-name */
int mxalias_curlook(MA *op,MA_CUR *curp,cchar *kbuf,int klen) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = mxalias_magic(op,curp,kbuf)) >= 0) {
	    rs = SR_INVALID ;
	    if ((curp->magic == MXALIAS_MAGIC) && (op->ncursors > 0)) {
	        if (kbuf[0]) {
		    rs = mxalias_curlooks(op,curp,kbuf,klen) ;
		    c = rs ;
		}
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mxalias_curlook) */

static int mxalias_curlooks(MA *op,MA_CUR *curp,cc *kbuf,int klen) noex {
    	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	cchar		*kp{} ;
	if (nulstr kstr ; (rs = kstr.start(kbuf,klen,&kp)) >= 0) {
	    cint	vn = 4 ;
	    int		vo = 0 ;
	    if (vecstr klist ; (rs = klist.start(vn,vo)) >= 0) {
	        vo = VECSTR_OSTATIONARY ;
	        if (vecstr vlist ; (rs = vlist.start(vn,vo)) >= 0) {
	            if ((rs = mxalias_addvals(op,&klist,&vlist,kp)) >= 0) {
			cchar	*cp{} ;
	                c += rs ;
	                for (int i = 0 ; vlist.get(i,&cp) >= 0 ; i += 1) {
	                    if ((cp != nullptr) && isnotspecial(*cp)) {
	                        rs = mxalias_addvals(op,&klist,&vlist,cp) ;
	                        c += rs ;
	                        if (rs > 0) {
	                            if (c > 0) c -= 1 ;
	                            vlist.del(i) ;
	                        }
	                    } /* end if */
	                    if (rs < 0) break ;
	                } /* end for */
	                if (rs >= 0) {
	                    rs = mxalias_mkvals(op,curp,&vlist) ;
	                    c = rs ;
	                }
	            } /* end if (addvals) */
	            rs1 = vlist.finish ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (vecstr) */
	        rs1 = klist.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (vecstr) */
	    rs1 = kstr.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (nulstr) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mxalias_curlooks) */

int mxalias_curread(MA *op,MA_CUR *curp,char *vbuf,int vlen) noex {
	int		rs ;
	int		vl = 0 ;
	if ((rs = mxalias_magic(op,curp)) >= 0) {
	    rs = SR_INVALID ;
	    if (op->ncursors > 0) {
	        cint	ni = (curp->i < 0) ? 0 : (curp->i + 1) ;
		rs = SR_OK ;
	        if (ni < curp->nvals) {
	            cchar	*vp = curp->vals[ni] ;
	            if (vbuf != nullptr) {
	                rs = sncpy1(vbuf,vlen,vp) ;
	                vl = rs ;
	            } else {
	                vl = lenstr(vp) ;
	            }
	            if (rs >= 0) {
	                curp->i = ni ;
	            }
	        } else {
	            rs = SR_NOTFOUND ;
	        }
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (mxalias_curread) */


/* private subroutines */

static int mxalias_username(MA *op,cchar *username) noex {
	int		rs ;
	if ((username == nullptr) || (username[0] == '\0')) {
	    username = "-" ;
	}
	if (cchar *cp{} ; (rs = uc_mallocstrw(username,-1,&cp)) >= 0) {
	    op->username = cp ;
	}
	return rs ;
}
/* end subroutine (mxalias_username) */

static int mxalias_userdname(MA *op) noex {
	int		rs = SR_OK ;
	int		hl = 0 ;
	if (op->userdname == nullptr) {
	    if (char *hbuf{} ; (rs = malloc_mp(&hbuf)) >= 0) {
	        cchar	*un = op->username ;
		cint	hlen = rs ;
	        if ((rs = getuserhome(hbuf,hlen,un)) >= 0) {
		    auto mall = uc_mallocstrw ;
		    hl = rs ;
	            if (cchar *cp{} ; (rs = mall(hbuf,rs,&cp)) >= 0) {
	                op->userdname = cp ;
		    }
	        } /* end if (getuserhome) */
		rs = rsfree(rs,hbuf) ;
	    } /* end if (m-a-f) */
	} else {
	    hl = lenstr(op->userdname) ;
	} /* end if (null) */
	return (rs >= 0) ? hl : rs ;
}
/* end subroutine (mxalias_userdname) */

static int mxalias_filesadd(MA *op,time_t dt) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	(void) dt ;
	if_constexpr (f_filesys || f_fileuser) {
	    if (char *fbuf{} ; (rs = malloc_mp(&fbuf)) >= 0) {
	        if_constexpr (f_filesys) {
	            if (rs >= 0) {
	                if ((rs = mkpath(fbuf,MXALIAS_SYSDB)) >= 0) {
	                    rs = mxalias_fileadd(op,fbuf) ;
	                    c += rs ;
	                }
	            } /* end if */
	        } /* end if_constexpr (f_filesys) */
	        if_constexpr (f_fileuser) {
	            if (rs >= 0) {
	                if ((rs = mxalias_mkuserfname(op,fbuf)) >= 0) {
	                    rs = mxalias_fileadd(op,fbuf) ;
	                    c += rs ;
	                }
	            } /* end if */
	        } /* end if_constexpr (f_fileuser) */
	        rs = rsfree(rs,fbuf) ;
	    } /* end if (m-a-f) */
	} /* end if_constexpr (f_filesys || f_fileuser) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mxalias_filesadd) */

/* add a file to the list of files */
int mxalias_fileadd(MA *op,cchar *atfname) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = mxalias_magic(op,atfname)) >= 0) {
	    rs = SR_INVALID ;
	    if (atfname[0]) {
	        cchar	*fn = atfname ;
	        char	*tbuf = nullptr ;
	        if (atfname[0] != '/') {
	            if ((rs = mxalias_userdname(op)) >= 0) {
	                rs = mkpath(tbuf,op->userdname,atfname) ;
	                fn = tbuf ;
	            }
	        } /* end if (added PWD) */
	        if (rs >= 0) {
	            if (USTAT sb ; (rs = u_stat(fn,&sb)) >= 0) {
		        if (isOurFileType(sb.st_mode)) {
	                    if ((rs = mxalias_filereg(op,&sb,fn)) >= 0) {
	                        cint	fi = rs ;
	                        if (rs < INT_MAX) {
	                            if ((rs = mxalias_fileparse(op,fi)) >= 0) {
	                                c = rs ;
	                            }
	                        } /* end if (needed registration) */
	                        if (rs < 0) {
	                            mxalias_filedel(op,fi) ;
		                }
	                    } /* end if (mxalias_filereg) */
		        } /* end if (allowed file-type) */
	            } else if (isNotPresent(rs)) {
		        rs = SR_OK ;
	            } /* end if (our file type) */
	        } /* end if (ok) */
		if (tbuf) {
		    rs = rsfree(rs,tbuf) ;
		}
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mxalias_fileadd) */

static int mxalias_filereg(MA *op,USTAT *sbp,cchar *fn) noex {
	int		rs ;
	int		fi = INT_MAX ;
	if (MA_FI fe ; (rs = file_start(&fe,sbp,fn)) >= 0) {
	    vecobj	*flp = op->flp ;
	    cint	rsn = SR_NOTFOUND ;
	    if ((rs = flp->search(&fe,vcmpfe,nullptr)) == rsn) {
	        rs = flp->add(&fe) ;
	        fi = rs ;
	    }
	    if (rs < 0) {
	        file_finish(&fe) ;
	    }
	} /* end if (file_start) */
	return (rs >= 0) ? fi : rs ;
}
/* end subroutine (mxalias_filereg) */

#if	CF_FILECHECK

/* check if files have changed */
static int mxalias_filechecks(MA *op,time_t dt) noex {
    	vecobj		*flp = op->flp ;
	int		rs = SR_OK ;
	int		c_changed = 0 ;
	void		*vp{} ;
	for (int i = 0 ; flp->get(i,&vp) >= 0 ; i += 1) {
	    MA_FI	*fep = (MA_FI *) vp ;
	    if (vp) {
	        if (USTAT sb ; (rs = uc_stat(fep->fname,&sb)) >= 0) {
		    if (sb.st_mtime > fep->timod) {
	        	c_changed += 1 ;
	        	mxalias_filedump(op,i) ;
	        	rs = mxalias_fileparse(op,i) ;
		    }
	        } else if (isNotPresent(rs)) {
		    rs = SR_OK ;{
	        } /* end if */
	    }
	    if (rs < 0) break ;
	} /* end for */
	op->ti_check = daytime ;
	return (rs >= 0) ? c_changed : rs ;
}
/* end subroutine (mxalias_filechecks) */

#endif /* CF_FILECHECK */

#ifdef	COMMENT
static int mxalias_filealready(MA *op,dev_t dev,ino_t ino) noex {
    	vecobj		*flp = op->flp ;
	int		rs = SR_OK ;
	int		rs1 ;
	void		*vp{} ;
	for (int i = 0 ; (rs1 = flp->get(i,&vp)) >= 0 ; i += 1) {
	    MA_FI	*fep = (MA_FI *) = vp ;
	    if (vp) {
	        if ((fep->dev == dev) && (fep->ino == ino)) break ;
	    }
	} /* end for */
	if ((rs >= 0) && (rs1 >= 0)) rs = SR_EXIST ;
	return rs ;
}
/* end subroutine (mxalias_filealready) */
#endif /* COMMENT */

static int mxalias_fileparse(MA *op,int fi) noex {
    	vecobj		*flp = op->flp ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	void		*vp{} ;
	if ((rs = flp->get(fi,&vp)) >= 0) {
	    MA_FI	*fep = (MA_FI *) vp ;
	    if (vp) {
	        bfile	mxfile, *lfp = &mxfile ;
	        if ((rs = bopen(lfp,fep->fname,"r",0664)) >= 0) {
	            if (USTAT sb ; (rs = bcontrol(lfp,BC_STAT,&sb)) >= 0) {
	                if (! S_ISDIR(sb.st_mode)) {
	                    if (fep->timod < sb.st_mtime) {
	                        fep->dev = sb.st_dev ;
	                        fep->ino = sb.st_ino ;
	                        fep->timod = sb.st_mtime ;
	                        fep->fsize = sb.st_size ;
	                        rs = mxalias_fileparser(op,fi,lfp) ;
	                        c += rs ;
	                    } /* end if (needed update) */
	                } else {
	                    rs = SR_ISDIR ;
			}
	            } /* end if (bcontrol) */
	            rs1 = bclose(lfp) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (bfile) */
		if (rs < 0) {
	    	    mxalias_filedump(op,fi) ;
		}
	    } else {
	        rs = SR_NOTFOUND ;
	    }
	} /* end if (vecobj_get) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mxalias_fileparse) */

static int mxalias_fileparser(MA *op,int fi,bfile *lfp) noex {
	cint		llen = var.maxlinelen ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	if (BD bd ; (rs = bufdesc_start(&bd,llen)) >= 0) {
	    char	*lbuf = bd.lbuf ;
	    while ((rs = breadln(lfp,lbuf,llen)) > 0) {
	        cchar	*cp ;
		if (int cl ; (cl = sfcontent(lbuf,rs,&cp)) > 0) {
	             rs = mxalias_fileparseln(op,fi,&bd,cp,cl) ;
	             c += rs ;
	        }
	        if (rs < 0) break ;
	    } /* end while (reading extended lines) */
	    rs1 = bufdesc_finish(&bd) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (bufdesc) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mxalias_fileparser) */

static int mxalias_fileparseln(MA *op,int fi,BD *bdp,cchar *lp,int ll) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	if (field fsb ; (rs = fsb.start(lp,ll)) >= 0) {
	    cchar	*fp ;
	    if (int fl ; (fl = fsb.get(kterms,&fp)) > 0) {
	        if (int ki ; (ki = matstr(keywords,fp,fl)) >= 0) {
	            switch (ki) {
	            case keyword_alias:
	            case keyword_group:
	                rs = mxalias_fileparseln_alias(op,fi,bdp,&fsb) ;
	                c += rs ;
	                break ;
	            case keyword_unalias:
	            case keyword_ungroup:
	                rs = mxalias_fileparseln_unalias(op,fi,bdp,&fsb) ;
	                break ;
	            case keyword_source:
	                rs = mxalias_fileparseln_source(op,fi,bdp,&fsb) ;
	                c += rs ;
	                break ;
	            } /* end switch */
	        } /* end if */
	    } /* end if (field_get) */
	    rs1 = fsb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (field) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mxalias_fileparseln) */

static int mxalias_fileparseln_alias(MA *op,int fi,BD *bdp,field *fsbp) noex {
	cint		flen = bdp->flen ;
	cint		klen = bdp->klen ;
	int		rs = SR_OK ;
	int		fl ;
	int		c_field = 0 ;
	int		c = 0 ; /* return-value */
	char		*kbuf = bdp->kbuf ;
	char		*fbuf = bdp->fbuf ;
	(void) fi ;
	kbuf[0] = '\0' ;
	while (rs >= 0) {
	    cchar	*fp = fbuf ;
	    fl = field_sharg(fsbp,vterms,fbuf,flen) ;
	    if ((fl < 0) || ((c_field == 0) && (fl == 0))) break ;
	    if (c_field++ == 0) {
	        strwcpy(kbuf,fp,min(fl,klen)) ;
	    } else if (kbuf[0] != '\0') {
	        c += 1 ;
	        rs = keyvals_add(op->elp,fi,kbuf,fp,fl) ;
	    } /* end if (handling record) */
	    if (fsbp->term == '#') break ;
	} /* end while (fields) */
	if ((rs >= 0) && (c == 0) && (kbuf[0] != '\0')) {
	    c += 1 ;
	    rs = keyvals_add(op->elp,fi,kbuf,fbuf,0) ;
	}
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mxalias_fileparseln_alias) */

static int mxalias_fileparseln_unalias(MA *op,int fi,BD *bdp,field *fsbp) noex {
	cint		flen = bdp->flen ;
	int		rs ;
	char		*fbuf = bdp->fbuf ;
	(void) fi ;
	if ((rs = fsbp->sharg(vterms,fbuf,flen)) > 0) {
	    rs = keyvals_delkey(op->elp,fbuf,rs) ;
	} else if (isNotPresent(rs)) {
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (mxalias_fileparseln_unalias) */

static int mxalias_fileparseln_source(MA *op,int fi,BD *bdp,field *fsbp) noex {
	cint		flen = bdp->flen ;
	int		rs ;
	int		c = 0 ; /* return-value */
	char		*fbuf = bdp->fbuf ;
	(void) fi ;
	if ((rs = fsbp->sharg(vterms,fbuf,flen)) > 0) {
	    rs = mxalias_fileadd(op,fbuf) ;
	    c = rs ;
	}
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mxalias_fileparseln_source) */

static int mxalias_filedump(MA *op,int fi) noex {
	int		rs = SR_OK ;
	if (fi >= 0) {
	    rs = keyvals_delset(op->elp,fi) ;
	} else {
	    vecobj	*flp = op->flp ;
	    if ((rs = flp->count) >= 0) {
	        cint	n = rs ;
	        for (int i = 0 ; (rs >= 0) && (i < n) ; i += 1) {
	            rs = keyvals_delset(op->elp,i) ;
	        }
	    }
	} /* end if */
	return rs ;
}
/* end subroutine (mxalias_filedump) */

static int mxalias_filedel(MA *op,int fi) noex {
    	vecobj		*flp = op->flp ;
	int		rs ;
	int		rs1 ;
	if (void *vp ; (rs = flp->get(fi,&vp)) >= 0) {
	    MA_FI	*fep = (MA_FI *) vp ;
	    if (vp) {
		{
	            rs1 = file_finish(fep) ;
	            if (rs >= 0) rs = rs1 ;
		}
		{
	            rs1 = vecobj_del(op->flp,fi) ;
	            if (rs >= 0) rs = rs1 ;
		}
	    }
	} /* end if (vecobj_get) */
	return rs ;
}
/* end subroutine (mxalias_filedel) */

static int mxalias_filedels(MA *op) noex {
    	vecobj		*flp = op->flp ;
	int		rs = SR_OK ;
	int		rs1 ;
	void		*vp{} ;
	for (int i = 0 ; flp->get(i,&vp) >= 0 ; i += 1) {
	    MA_FI	*fep = (MA_FI *) vp ;
	    if (vp) {
	        rs1 = file_finish(fep) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end for */
	return rs ;
}
/* end subroutine (mxalias_filedels) */

#if	CF_FILECHECK

static int mxalias_filechanged(MA *op,USTAT *sbp) noex {
    	int		rs = SR_FAULT ;
    	int		f = false ;
	if (sbp) {
	    f = f || (op->fi.size != sbp->st_size) ;
	    f = f || (op->fi.timod != sbp->st_mtime) ;
	    f = f || (op->fi.ino != sbp->st_ino) ;
	    f = f || (op->fi.dev != sbp->st_dev) ;
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (mxalias_filechanged) */

static int mxalias_fileold(MA *op,time_t daytime) noex {
	int		rs ;
	int		rs1 ;
	int		f = false ; /* return-value */
	if ((rs = mxalias_aprofile(op,daytime)) >= 0) {
	    if (char *tbuf{} ; (rs = malloc_mp(&tbuf)) >= 0) {
	        USTAT	sb ;
	        int	i ; /* used-afterwards */
	        for (i = 0 ; op->aprofile[i] != nullptr ; i += 1) {
	            cchar	*cp = charp(op->aprofile[i]) ;
	            if (*cp != '/') {
	                cp = tbuf ;
	                mkpath(tbuf,op->pr,op->aprofile[i]) ;
	            }
	            rs1 = u_stat(cp,&sb) ;
	            if ((rs1 >= 0) && (sb.st_mtime > op->fi.timod)) break ;
	        } /* end for */
	        f = (op->aprofile[i] != nullptr) ;
	    	rs = rsfree(rs,tbuf) ;
	    } /* end if (m-a-f) */
	} /* end if (mxalias_aprofile) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (mxalias_fileold) */

#endif /* CF_FILECHECK */

static int mxalias_mkuserfname(MA *op,char *fname) noex {
	int		rs ;
	fname[0] = '\0' ;
	if ((rs = mxalias_userdname(op)) >= 0) {
	    cchar	*homedname = op->userdname ;
	    rs = mkpath(fname,homedname,MXALIAS_USERDB) ;
	}
	return rs ;
}
/* end subroutine (mxalias_mkuserfname) */

static int mxalias_addvals(MA *op,vecstr *klp,vecstr *vlp,cchar *kp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ; /* return-value */
	if (kp[0] != '\0') {
	    keyvals_cur	kcur ;
	    cint	rsn = SR_NOTFOUND ;
	    if ((rs = keyvals_curbegin(op->elp,&kcur)) >= 0) {
	        while (rs >= 0) {
	            cchar *vp{} ;
	            int	vl = keyvals_fetch(op->elp,kp,&kcur,&vp) ;
	            if (vl == rsn) break ;
	            rs = vl ;
	            c += 1 ;
	            if ((rs >= 0) && (vl > 0)) {
	                bool	f = (kp[0] == vp[0]) ;
	                f = f && (strncmp(kp,vp,vl) == 0) ;
	                f = f && (kp[vl] == '\0') ;
	                if (! f) {
	                    if ((rs = klp->findn(vp,vl)) >= 0) {
	                        f = true ;
			    } else if (rs == rsn) {
				rs = SR_OK ;
			    }
	                }
	                if ((rs >= 0) && (! f)) {
	                    rs = vlp->adduniq(vp,vl) ;
			}
	            } /* end if (substantive value) */
	        } /* end while */
	        rs1 = keyvals_curend(op->elp,&kcur) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (cursor) */
	    if ((rs >= 0) && (c > 0)) {
	        rs = klp->adduniq(kp) ;
	    }
	} /* end if (non-nul) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mxalias_addvals) */

static int mxalias_mkvals(MA *op,MA_CUR *curp,vecstr *vlp) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ; /* return-value */
	if (op) {
	    if ((rs = vlp->count) > 0) {
	        int	sz = (rs + 1) * szof(char **) ;
	        if (void *p ; (rs = uc_malloc(sz,&p)) >= 0) {
	            cchar	*cp{} ;
	            curp->vals = charpp(p) ; /* store allocation here */
	            sz = 1 ;
	            for (int i = 0 ; vlp->get(i,&cp) >= 0 ; i += 1) {
	                if (cp) {
	                    sz += (lenstr(cp) + 1) ;
		        }
	            } /* end for */
	            if ((rs = uc_malloc(sz,&p)) >= 0) {
	                char	*bp = charp(p) ;
	                curp->vbuf = charp(p) ;
	                *bp++ = '\0' ; /* <- is this an ELF str-tab thing? */
	                for (int i = 0 ; vlp->get(i,&cp) >= 0 ; i += 1) {
	                    if (cp) {
	                        curp->vals[c++] = bp ;
	                        bp = strwcpy(bp,cp,-1) + 1 ;
			    }
	                } /* end for */
	                curp->vals[c] = nullptr ;
	                curp->nvals = c ;
	            } /* end if (memory-allocation) */
	            if (rs < 0) {
	                uc_free(curp->vals) ;
	                curp->vals = nullptr ;
	            }
	        } /* end if (m-a) */
	    } /* end if (non-zero positive) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mxalias_mkvals) */

static int mxalias_finallocs(MA *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->pwd) {
	    rs1 = uc_free(op->pwd) ;
	    if (rs >= 0) rs = rs1 ;
	    op->pwd = nullptr ;
	}
	if (op->userdname) {
	    rs1 = uc_free(op->userdname) ;
	    if (rs >= 0) rs = rs1 ;
	    op->userdname = nullptr ;
	}
	if (op->username) {
	    rs1 = uc_free(op->username) ;
	    if (rs >= 0) rs = rs1 ;
	    op->username = nullptr ;
	}
	if (op->pr) {
	    rs1 = uc_free(op->pr) ;
	    if (rs >= 0) rs = rs1 ;
	    op->pr = nullptr ;
	}
	return rs ;
}
/* end subroutine (mxalias_finallocs) */

static int mxalias_finents(MA *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (mxalias_finents) */

static int file_start(MA_FI *fep,USTAT *sbp,cchar *fname) noex {
	int		rs = SR_FAULT ;
	if (fep && sbp && fname) {
	    memclear(fep) ;
	    if_constexpr (f_comment) {
	        fep->timod = sbp->st_mtime ;
	        fep->fsize = sbp->st_size ;
	        fep->dev = sbp->st_dev ;
	        fep->ino = sbp->st_ino ;
	    } /* end if_constexpr (f_comment) */
	    if (cchar *cp{} ; (rs = uc_mallocstrw(fname,-1,&cp)) >= 0) {
	        fep->fname = cp ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (file_start) */

static int file_finish(MA_FI *fep) noex {
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

static int bufdesc_start(BD *bdp,int llen) noex {
	cint		klen = KEYBUFLEN ;
	int		sz ;
	int		rs ;
	if (llen < 0) llen = var.maxlinelen ;
	sz = ((2*(llen+1))+(klen+1)) ;
	if (char *bp{} ; (rs = uc_malloc(sz,&bp)) >= 0) {
	    bdp->a = bp ;
	    bdp->lbuf = bp ;
	    bdp->llen = llen ;
	    bp += (llen+1) ;
	    bdp->fbuf = bp ;
	    bdp->flen = llen ;
	    bp += (klen+1) ;
	    bdp->kbuf = bp ;
	    bdp->klen = klen ;
	}
	return rs ;
}
/* end subroutine (bufdesc_start) */

static int bufdesc_finish(BD *bdp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (bdp->a) {
	    rs1 = uc_free(bdp->a) ;
	    if (rs >= 0) rs = rs1 ;
	    bdp->a = nullptr ;
	}
	return rs ;
}
/* end subroutine (bufdesc_finish) */

vars::operator int () noex {
    	int		rs ;
	if ((rs = getbufsize(getbufsize_ml)) >= 0) {
	    maxlinelen = rs ;
	}
	return rs ;
}

static int cmpfe(MA_FI *e1p,MA_FI *e2p) noex {
	int		rc = 0 ;
	if (e1p || e2p) {
	    if (e1p) {
	        if (e2p) {
		    if ((e1p->dev - e2p->dev) == 0) {
		        if ((e1p->ino - e2p->ino) == 0) {
			    rc = 0 ;
		        } else {
			    rc = (e1p->ino > 0) ? 1 : -1 ;
		        }
		    } else {
			rc = (e1p->dev > 0) ? 1 : -1 ;
		    }
	        } else {
	            rc = -1 ;
		}
	    } else {
	        rc = +1 ;
	    }
	}
	return rc ;
}
/* end subroutine (cmpfe) */

static int vcmpfe(cvoid **v1pp,cvoid **v2pp) noex {
	MA_FI	*e1p = (MA_FI *) *v1pp ;
	MA_FI	*e2p = (MA_FI *) *v2pp ;
	return cmpfe(e1p,e2p) ;
}

static bool isnotspecial(int ch) noex {
	ch &= 255 ;
	return (ch != '/') && (ch != '|') ;
}
/* end subroutine (isnotspecial) */

static bool isOurFileType(mode_t m) noex {
	return S_ISREG(m) || S_ISSOCK(m) || S_ISFIFO(m) || S_ISCHR(m) ;
}
/* end subroutine (isOurFileType) */


