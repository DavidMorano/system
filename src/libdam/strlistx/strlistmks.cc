/* strlistmks SUPPORT */
/* lang=C++20 */

/* make a STRLIST database */
/* version %I% last-modified %G% */

#define	CF_MINMOD	1		/* ensure minimum file mode */

/* revision history:

	= 1998-03-01, David A­D­ Morano
	This subroutine was originally written.

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

	Notes:

	= possible returns to an open attempt
	- OK (creating)
	- already exists
	- doesn't exist but is in progress
	- exists and is in progress

	= open-flags

	open-flags	if DB exits	if NDB exists	returns
	___________________________________________________________________

	-		no		no		SR_OK (created)
	-		no		yes		SR_INPROGRESS
	-		yes		no		SR_OK
	-		yes		yes		SR_INPROGRESS

	O_CREAT|O_EXCL	no		no		SR_OK (created)
	O_CREAT|O_EXCL	no		yes		SR_INPROGRESS
	O_CREAT|O_EXCL	yes		no		SR_EXIST
	O_CREAT|O_EXCL	yes		yes		SR_INPROGRESS

	O_CREAT		x		x		SR_OK (created)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
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
#include	<xperm.h>
#include	<opentmp.h>
#include	<hash.h>
#include	<sfx.h>
#include	<sncpyx.h>
#include	<mkx.h>
#include	<mkpathx.h>
#include	<mkfnamesuf.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"strlistmks.h"
#include	"strlisthdr.h"


/* local defines */

#define	STRLISTMKS_SIZEMULT	4
#define	STRLISTMKS_NSKIP	5
#define	STRLISTMKS_INDPERMS	0664

#define	SLM		strlistmks
#define	SLM_OBJ		strlistmks_obj
#define	SLM_RT		strlistmks_rt
#define	SLM_FL		strlistmks_fl
#define	RT		strlistmks_rt
#define	VE		varentry

#ifndef	KEYBUFLEN
#define	KEYBUFLEN	120
#endif

#define	HDRBUFLEN	(sizeof(strlisthdr) + 128)

#define	FSUF_IND	"si"

#define	TO_OLDFILE	(5 * 60)

#ifndef	MODP2
#define	MODP2(v,n)	((v) & ((n) - 1))
#endif

#ifndef	CF_MINMOD
#define	CF_MINMOD	1		/* ensure minimum file mode */
#endif


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef mode_t		om_t ;

typedef uint		*rectab_t ;


/* external subroutines */

extern "C" {
    extern uint	nextpowtwo(uint) noex ;
}


/* external variables */


/* exported variables */

STRLISTMKS_OBJ	strlistmks_mod = {
	"strlistmks",
	sizeof(STRLISTMKS)
} ;


/* local structures */

struct varentry {
	uint	khash ;
	uint	ri ;
	uint	ki ;
	uint	hi ;
} ;


/* forward references */

template<typename ... Args>
static int strlistmks_ctor(strlistmks *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    memclear(op) ;
	    if ((op->stp = new(nothrow) strtab) != np) {
		if ((op->rtp = new(nothrow) rectab) != np) {
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
static int	strlistmks_filesend(SLM *,int) noex ;

static int	strlistmks_listbegin(SLM *,int) noex ;
static int	strlistmks_listend(SLM *) noex ;

static int	strlistmks_nfcreate(SLM *,cchar *) noex ;
static int	strlistmks_nfcreatecheck(SLM *,cchar *,cchar *) noex ;
static int	strlistmks_nfdestroy(SLM *) noex ;
static int	strlistmks_nfstore(SLM *,char *) noex ;
static int	strlistmks_fexists(SLM *) noex ;

static int	strlistmks_mkvarfile(SLM *) noex ;
static int	strlistmks_wrvarfile(SLM *) noex ;
static int	strlistmks_mkind(SLM *,char *,uint (*)[3],int) noex ;
static int	strlistmks_renamefiles(SLM *) noex ;

static int	indinsert(rectab_t,uint (*it)[3],int,VE *) noex ;
static int	hashindex(uint,int) noex ;


/* local variables */

static sysval		pagesize(sysval_ps) ;

constexpr gid_t		gidend = -1 ;

constexpr bool		f_minmod = CF_MINMOD ;


/* exported variables */


/* exported subroutines */

int strlistmks_open(SLM *op,cc *dbname,int of,mode_t om,int n) noex {
	int		rs ;
	if ((rs = strlistmks_ctor(op,dbname)) >= 0) {
	    rs = SR_INVALID ;
	    if (dbname[0]) {
	        cchar	*cp ;
	        if (n < STRLISTMKS_NENTRIES) {
	            n = STRLISTMKS_NENTRIES ;
	        }
	        op->om = om ;
	        op->nfd = -1 ;
	        op->gid = -1 ;
	        op->f.ofcreat = (of & O_CREAT) ;
	        op->f.ofexcl = (of & O_EXCL) ;
	        op->f.none = (! op->f.ofcreat) && (! op->f.ofexcl) ;
	        if ((rs = uc_mallocstrw(dbname,-1,&cp)) >= 0) {
		    op->dbname = cp ;
		    if ((rs = strlistmks_filesbegin(op)) >= 0) {
		        if ((rs = strlistmks_listbegin(op,n)) >= 0) {
			    op->magic = STRLISTMKS_MAGIC ;
		        }
		        if (rs < 0) {
			    strlistmks_filesend(op,false) ;
		        }
		    } /* end if */
		    if (rs < 0) {
	    	        uc_free(op->dbname) ;
	    	        op->dbname = nullptr ;
		    }
	        } /* end if (memory-allocation) */
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
	    int		f_remove = true ;
	    nvars = op->nstrs ;
	    if (! op->f.abort) {
	        rs1 = strlistmks_mkvarfile(op) ;
	        f_remove = (rs1 < 0) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->nfd >= 0) {
	        rs1 = u_close(op->nfd) ;
	        if (rs >= 0) rs = rs1 ;
	        op->nfd = -1 ;
	    }
	    {
	        rs1 = strlistmks_listend(op) ;
	        if (! f_remove) f_remove = (rs1 < 0) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if ((rs >= 0) && (! op->f.abort)) {
	        rs1 = strlistmks_renamefiles(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = strlistmks_filesend(op,f_remove) ;
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
	        if ((rs = rectab_add(op->rtp,ki)) >= 0) {
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
	    cchar	*dnp ;
	    rs = SR_INVALID ;
	    /* check that the parent directory is writable to us */
	    if (int dnl ; (dnl = sfdirname(op->dbname,-1,&dnp)) > 0) {
	        cchar	*cp ;
	        if ((rs = uc_mallocstrw(dnp,dnl,&cp)) >= 0) {
	            op->idname = cp ;
	            if (dnl == 0) {
	                rs = getpwd(tbuf,tlen) ;
	                dnl = rs ;
	            } else {
	                rs = mkpath1w(tbuf,dnp,dnl) ;
		    }
	            if (rs >= 0) {
	                cint	am = (X_OK | W_OK) ;
	                rs = perm(tbuf,-1,-1,nullptr,am) ;
	            }
	            if (rs >= 0) {
	                if ((rs = strlistmks_nfcreate(op,FSUF_IND)) >= 0) {
	                    if (op->f.ofcreat && op->f.ofexcl) {
	                        rs = strlistmks_fexists(op) ;
	                    }
	                    if (rs < 0) {
		                strlistmks_nfdestroy(op) ;
		 	    }
	                } /* end if (nfcreate) */
	            } /* end if (ok) */
	            if (rs < 0) {
		        if (op->idname != nullptr) {
	    	            uc_free(op->idname) ;
	    	            op->idname = nullptr ;
		        }
	            }
	        } /* end if (memory-allocation) */
	    } /* end if (sfdirname) */
	    rs1 = uc_free(tbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end subroutine (strlistmks_filesbegin) */

static int strlistmks_filesend(SLM *op,int f) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->nfname) {
	    if (f && (op->nfname[0] != '\0')) {
	        u_unlink(op->nfname) ;
		op->nfname[0] = '\0' ;
	    }
	    {
	        rs1 = uc_free(op->nfname) ;
	        if (rs >= 0) rs = rs1 ;
	        op->nfname = nullptr ;
	    }
	}
	if (op->idname) {
	    rs1 = uc_free(op->idname) ;
	    if (rs >= 0) rs = rs1 ;
	    op->idname = nullptr ;
	}
	return rs ;
}
/* end subroutine (strlistmks_filesend) */

static int strlistmks_nfcreate(SLM *op,cchar *fsuf) noex {
	cint		rse = SR_EXIST ;
	int		rs ;
	int		rs1 ;
	char		*tbuf{} ;
	if ((rs = malloc_mp(&tbuf)) >= 0) {
	    cchar	*dbn = op->dbname ;
	    cchar	*end = ENDIANSTR ;
	    if ((rs = mkfnamesuf3(tbuf,dbn,fsuf,end,"n")) >= 0) {
	        cint	nfl = rs ;
	        cchar	*cp ;
	        if ((rs = uc_mallocstrw(tbuf,nfl,&cp)) >= 0) {
	            USTAT	sb ;
	            cint	to_old = TO_OLDFILE ;
		    cint	of = (O_CREAT | O_EXCL | O_WRONLY) ;
		    cmode	om = op->om ;
	            op->nfname = charp(cp) ;
		    op->f.fcreated = false ;
		    while ((rs >= 0) && (! op->f.fcreated)) {
		        if ((rs = u_open(op->nfname,of,om)) >= 0) {
			    op->nfd = rs ;
			    op->f.ofcreat = true ;
			} else if (rs == rse) {
	    		    custime	dt = time(nullptr) ;
	    		    if ((rs = u_stat(op->nfname,&sb)) >= 0) {
	    	                bool	f = false ;
	    			if ((dt - sb.st_mtime) > to_old) {
				    u_unlink(op->nfname) ;
				} else {
	    			    op->f.inprogress = true ;
	    			    f = op->f.none ;
	    			    f = f || (op->f.ofcreat && op->f.ofexcl) ;
	    			    rs = (f) ? SR_INPROGRESS : SR_OK ;
				} /* end if */
			    } else if (isNotPresent(rs)) {
				rs = SR_OK ;
			    } /* end if (u_stat) */
			} /* end if (u_open) */
		    } /* end while */
	    	    if ((rs < 0) && op->nfname) {
	        	uc_free(op->nfname) ;
	        	op->nfname = nullptr ;
	    	    }
		} /* end if (memory-allocation of 'nfname') */
	    } /* end if (mkfname) */
	    rs1 = uc_free(tbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end subroutine (txindexmks_nfcreate) */

static int strlistmks_nfcreatecheck(SLM *op,cchar *fpre,cchar *fsuf) noex {
	int		rs = SR_OK ;
	int		of ;

	if ((op->nfd < 0) || op->f.inprogress) {
	    if (op->nfd >= 0) {
		u_close(op->nfd) ;
		op->nfd = -1 ;
	    }
	    of = O_WRONLY | O_CREAT ;
	    if (op->f.inprogress) {
		char	cname[MAXNAMELEN + 1] ;
		char	infname[MAXPATHLEN + 1] ;
		char	outfname[MAXPATHLEN + 1] ;
		outfname[0] = '\0' ;
		rs = sncpy6(cname,MAXNAMELEN,
			fpre,"XXXXXXXX",".",fsuf,ENDIANSTR,"n") ;
		if (rs >= 0) {
		    if ((op->idname != nullptr) && (op->idname[0] != '\0')) {
		        rs = mkpath2(infname,op->idname,cname) ;
		    } else {
		        rs = mkpath1(infname,cname) ;
		    }
		}
		if (rs >= 0) {
		    rs = opentmpfile(infname,of,op->om,outfname) ;
	            op->nfd = rs ;
		    op->f.fcreated = (rs >= 0) ;
		}
		if (rs >= 0) {
		    rs = strlistmks_nfstore(op,outfname) ;
		}
		if (rs < 0) {
		    if (outfname[0] != '\0') {
			u_unlink(outfname) ;
		    }
		}
	    } else {
	        rs = u_open(op->nfname,of,op->om) ;
	        op->nfd = rs ;
		op->f.fcreated = (rs >= 0) ;
	    }
	    if (rs < 0) {
		if (op->nfd >= 0) {
		    u_close(op->nfd) ;
		    op->nfd = -1 ;
		}
	        if (op->nfname != nullptr) {
		    if (op->nfname[0] != '\0') {
			u_unlink(op->nfname) ;
			op->nfname[0] = '\0' ;
		    }
		    {
	                uc_free(op->nfname) ;
	                op->nfname = nullptr ;
		    }
		}
	    }
	} /* end if */

	return rs ;
}
/* end subroutine (strlistmks_nfcreatecheck) */

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

static int strlistmks_nfstore(SLM *op,char *outfname) noex {
	int		rs = SR_OK ;
	cchar		*cp ;
	if (op->nfname) {
	    uc_free(op->nfname) ;
	    op->nfname = nullptr ;
	}
	{
	    if ((rs = uc_mallocstrw(outfname,-1,&cp)) >= 0) {
		op->nfname = charp(cp) ;
	    }
	}
	return rs ;
}
/* end subroutine (strlistmks_nfstore) */

static int strlistmks_fexists(SLM *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->f.ofcreat && op->f.ofexcl && op->f.inprogress) {
	    cchar	*suf = FSUF_IND ;
	    cchar	*end = ENDIANSTR ;
	    char	*tbuf{} ;
	    if ((rs = malloc_mp(&tbuf)) >= 0) {
		cchar	*dbn = op->dbname ;
	        if ((rs = mkfnamesuf2(tbuf,dbn,suf,end)) >= 0) {
		    USTAT	sb ;
	            cint	rs1 = u_stat(tbuf,&sb) ;
	            if (rs1 >= 0) rs = SR_EXIST ;
	        }
		rs1 = uc_free(tbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if */
	return rs ;
}
/* end subroutine (strlistmks_fexists) */

static int strlistmks_listbegin(SLM *op,int n) noex {
	cint		sz = (n * STRLISTMKS_SIZEMULT) ;
	int		rs ;
	if ((rs = strtab_start(op->stp,sz)) >= 0) {
	    rs = rectab_start(op->rtp,n) ;
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
	    rs1 = rectab_finish(op->rtp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	if (op->stp) {
	    rs1 = strtab_finish(op->stp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (strlistmks_listend) */

static int strlistmks_mkvarfile(SLM *op) noex {
	cint		rtl = rectab_done(op->rtp) ;
	int		rs = SR_BUGCHECK ;
	int		nstrs = 0 ;
	if (rtl == (op->nstrs + 1)) {
	    if ((rs = strlistmks_wrvarfile(op)) >= 0) {
		nstrs = op->nstrs ;
	    }
	}
	return (rs >= 0) ? nstrs : rs ;
}
/* end subroutine (strlistmks_mkvarfile) */

namespace {
    struct sub_wrvarfile {
	strlistmks	*op ;
	strlisthdr	hf{} ;
	custime		dt = time(nullptr) ;
	cint		hlen = HDRBUFLEN ;
	char		hbuf[HDRBUFLEN+1] ;
	uint		foff = 0 ;
	int		ps ;
	sub_wrvarfile(strlistmks *p) noex : op(p) { } ;
	operator int () noex ;
	int mkfile(rectab_t,int) noex ;
	int mkkstab(filer *,int,int) noex ;
    } ;
}

static int strlistmks_wrvarfile(SLM *op) noex {
	sub_wrvarfile	wrf(op) ;
	return wrf ;
}
/* end subroutine (strlistmks_wrvarfile) */

sub_wrvarfile::operator int () noex {
	int		rs ;
	if ((rs = pagesize) >= 0) {
	    rectab_t	rt ;
	    ps = rs ;
	    if ((rs = rectab_getvec(op->rtp,&rt)) >= 0) {
	        cint	rtl = rs ;
	        cchar	*suf = FSUF_IND ;
	        if ((rs = strlistmks_nfcreatecheck(op,"nv",suf)) >= 0) {
		    if ((rs = mkfile(rt,rtl)) >= 0) {
	    	        hf.fsize = foff ;
	    		if ((rs = strlisthdr_rd(&hf,hbuf,hlen)) >= 0) {
	        	    cint	hl = rs ;
	        	    if ((rs = u_pwrite(op->nfd,hbuf,hl,0z)) >= 0) {
				if_constexpr (f_minmod) {
	    			    rs = uc_fminmod(op->nfd,op->om) ;
				} /* end if_constexpr (f_minmod) */
	    			if ((rs >= 0) && (op->gid != gidend)) {
				    rs = u_fchown(op->nfd,-1,op->gid) ;
	    			}
	    		    }
			}
		    }
		}
	    } /* end if (rectab_getvec) */
	} /* end if (pagesize) */
	return rs ;
}
/* end method (sub_wrvarfile::operator) */ 

int sub_wrvarfile::mkfile(rectab_t rt,int rtl) noex {
	filer		varfile ;
	int		rs ;
	int		rs1 ;
	int		sz = (ps * 4) ;
	op->f.viopen = true ;
	if ((rs = filer_start(&varfile,op->nfd,0,sz,0)) >= 0) {
	    /* prepare the file-header */
	    hf.vetu[0] = STRLISTMKS_VERSION ;
	    hf.vetu[1] = ENDIAN ;
	    hf.vetu[2] = 0 ;
	    hf.vetu[3] = 0 ;
	    hf.wtime = uint(dt) ;
	    hf.nstrs = op->nstrs ;
	    hf.nskip = STRLISTMKS_NSKIP ;
	    if ((rs = strlisthdr_rd(&hf,hbuf,hlen)) >= 0) {
		int	hl = rs ;
	        if ((rs = filer_writefill(&varfile,hbuf,hl)) >= 0) {
	    	    foff += rs ;
	            /* write the record table */
	            hf.rtoff = foff ;
	            hf.rtlen = rtl ;
	            sz = (rtl + 1) * sizeof(uint) ;
	            if ((rs = filer_write(&varfile,rt,sz)) >= 0) {
			strtab	*ksp = op->stp ;
	                foff += rs ;
		        /* make and write out key-string table */
			if ((rs = strtab_strsize(ksp)) >= 0) {
			    cint	sz = rs ;
			    hf.stoff = foff ;
			    hf.stlen = sz ;
			    rs = mkkstab(&varfile,rtl,sz) ;
			}
		    } /* end if (filer_write) */
		} /* end if (filer_writefill) */
	    } /* end if (strlisthdr_rd) */
	    rs1 = filer_finish(&varfile) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (filer) */
	return rs ;
}
/* end subroutine (sub_wrvarfile::mkfile) */

int sub_wrvarfile::mkkstab(filer *vfp,int rtl,int sz) noex {
	int		rs ;
	int		rs1 ;
	char		*kstab = nullptr ;
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
	            sz = (itl + 1) * 3 * sizeof(int) ;
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
/* end subroutine (sub_wrvarfile::mkkstab) */

int strlistmks_mkind(SLM *op,char *kst,uint (*it)[3], int il) noex {
	rectab_t	rt ;
	int		rs ;
	int		sc = 0 ;
	if ((rs = rectab_getvec(op->rtp,&rt)) >= 0) {
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
	} /* end if (rectab_getvec) */
	return (rs >= 0) ? sc : rs ;
}
/* end subroutine (strlistmks_mkind) */

static int strlistmks_renamefiles(SLM *op) noex {
	int		rs ;
	int		rs1 ;
	cchar		*end = ENDIANSTR ;
	char		*tbuf{} ;
	if ((rs = malloc_mp(&tbuf)) >= 0) {
	    if ((rs = mkfnamesuf2(tbuf,op->dbname,FSUF_IND,end)) >= 0) {
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

static int hashindex(uint i,int n) noex {
	int		hi = MODP2(i,n) ;
	if (hi == 0) hi = 1 ;
	return hi ;
}
/* end subroutine (hashindex) */


