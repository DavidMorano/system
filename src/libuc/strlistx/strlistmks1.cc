/* strlistmks SUPPORT */
/* lang=C++20 */

/* make a STRLIST database */
/* version %I% last-modified %G% */

#define	CF_FIRSTHASH	0		/* arrange for first-attempt hashing */
#define	CF_MINMOD	1		/* ensure minimum file mode */
#define	CF_LATE		0		/* late open */

/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

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

			if DB exits	if NDB exists	returns
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
#include	<localmisc.h>

#include	"strlistmks.h"
#include	"strlisthdr.h"


/* local defines */

#define	STRLISTMKS_SIZEMULT	4
#define	STRLISTMKS_NSKIP	5
#define	STRLISTMKS_INDPERMS	0664

#define	SLM		strlistmks
#define	SLM_OBJ		strlistmks_obj
#define	SLM_REC		strlistmks_rec
#define	SLM_FL		strlistmks_fl
#define	RECTAB		struct strlistmks_rectab
#define	VE		struct varentry

#ifndef	KEYBUFLEN
#define	KEYBUFLEN	120
#endif

#define	BUFLEN		(sizeof(strlisthdr) + 128)

#define	FSUF_IND	"si"

#define	TO_OLDFILE	(5 * 60)

#ifndef	MODP2
#define	MODP2(v,n)	((v) & ((n) - 1))
#endif

#define	DEBFNAME	"strlistmks.deb"

#ifndef	CF_MINMOD
#define	CF_MINMOD	1		/* ensure minimum file mode */
#endif


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */


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
		rs = SR_OK ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (strlistmks_ctor) */

static int strlistmks_dtor(strlistmks *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->stp) {
		delete op->stp ;
		op->stp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (strlistmks_dtor) */

template<typename ... Args>
static inline int strlistmks_magic(strlistmks *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == STRLISTMKS_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (strlistmks_magic) */

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

static int	rectab_start(RECTAB *,int) noex ;
static int	rectab_add(RECTAB *,uint) noex ;
static int	rectab_done(RECTAB *) noex ;
static int	rectab_getvec(RECTAB *,uint (**)[1]) noex ;
static int	rectab_extend(RECTAB *) noex ;
static int	rectab_finish(RECTAB *) noex ;

#ifdef	COMMENT
static int	rectab_count(RECTAB *) noex ;
#endif

static int	filer_writefill(FILER *,char *,int) noex ;

static int	indinsert(uint (*rt)[1],uint (*it)[3],int,VE *) noex ;
static int	hashindex(uint,int) noex ;


/* local variables */

static cchar	zerobuf[4] = {
	0, 0, 0, 0 
} ;

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
	        op->f.creat = (of & O_CREAT) ;
	        op->f.excl = (of & O_EXCL) ;
	        op->f.none = (! op->f.creat) && (! op->f.excl) ;
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
	        if ((rs = rectab_add(&op->rectab,ki)) >= 0) {
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
	                    if (op->f.creat && op->f.excl) {
	                        rs = strlistmks_fexists(op) ;
	                    }
	                    if (rs < 0) {
		                strlistmks_nfdestroy(op) ;
		 	    }
	                } /* end if (nfcreate) */
	            }
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
	            op->nfname = charp(cp) ;

again:
	rs = u_open(op->nfname,of,op->om) ;
	op->nfd = rs ;

#if	CF_LATE
	if (rs >= 0) {
	    u_close(op->nfd) ;
	    op->nfd = -1 ;
	}
#endif /* CF_LATE */

	if (rs == SR_EXIST) {
	    time_t	daytime = time(nullptr) ;
	    int		f_inprogress ;
	    rs1 = u_stat(op->nfname,&sb) ;
	    if ((rs1 >= 0) && ((daytime - sb.st_mtime) > to_old)) {
		u_unlink(op->nfname) ;
		goto again ;
	    }
	    op->f.inprogress = true ;
	    f_inprogress = op->f.none ;
	    f_inprogress = f_inprogress || (op->f.creat && op->f.excl) ;
	    rs = (f_inprogress) ? SR_INPROGRESS : SR_OK ;
	} /* end if */

	if (rs >= 0) {
	    op->f.created = true ;
	} else {
	    if (op->nfname != nullptr) {
	        uc_free(op->nfname) ;
	        op->nfname = nullptr ;
	    }
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
		    op->f.created = (rs >= 0) ;
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
		op->f.created = (rs >= 0) ;
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
	if (op->f.creat && op->f.excl && op->f.inprogress) {
	    cchar	*suf = FSUF_IND ;
	    cchar	*end = ENDIANSTR ;
	    char	*tbuf{} ;
	    if ((rs = malloc_mp(&tbuf)) >= 0) {
	        if ((rs = mkfnamesuf2(tbuf,op->dbname,suf,end)) >= 0) {
		    USTAT	sb ;
	            int	rs1 = u_stat(tbuf,&sb) ;
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
	    rs = rectab_start(&op->rectab,n) ;
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
	{
	    rs1 = rectab_finish(&op->rectab) ;
	    if (rs >= 0) rs = rs1 ;
	}
	{
	    rs1 = strtab_finish(op->stp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (strlistmks_listend) */

static int strlistmks_mkvarfile(SLM *op) noex {
	cint		rtl = rectab_done(&op->rectab) ;
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

static int strlistmks_wrvarfile(SLM *op) noex {
	strlisthdr	hf{} ;
	filer		varfile ;
	strtab		*ksp = op->stp ;
	const time_t	daytime = time(nullptr) ;
	uint		fileoff = 0 ;
	uint		(*rt)[1] ;
	cint		pagesize = getpagesize() ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		rtl ;
	int		itl ;
	int		sz ;
	int		bl ;
	char		buf[BUFLEN + 1] ;

	rtl = rectab_getvec(&op->rectab,&rt) ;

/* open (create) the STRLIST file */

	rs = strlistmks_nfcreatecheck(op,"nv",FSUF_IND) ;
	if (rs < 0)
	    goto ret0 ;

	op->f.viopen = true ;
	sz = (pagesize * 4) ;
	rs = filer_start(&varfile,op->nfd,0,sz,0) ;
	if (rs < 0)
	    goto ret1 ;

/* prepare the file-header */

	hf.vetu[0] = STRLISTMKS_VERSION ;
	hf.vetu[1] = ENDIAN ;
	hf.vetu[2] = 0 ;
	hf.vetu[3] = 0 ;
	hf.wtime = (uint) daytime ;
	hf.nstrs = op->nstrs ;
	hf.nskip = STRLISTMKS_NSKIP ;

/* create the file-header */

	rs = strlisthdr_rd(&hf,buf,BUFLEN) ;
	bl = rs ;
	if (rs < 0)
	    goto ret2 ;

/* write header */

	if (rs >= 0) {
	    rs = filer_writefill(&varfile,buf,bl) ;
	    fileoff += rs ;
	}

	if (rs < 0)
	    goto ret2 ;

/* write the record table */

	hf.rtoff = fileoff ;
	hf.rtlen = rtl ;

	sz = (rtl + 1) * 2 * sizeof(uint) ;
	rs = filer_write(&varfile,rt,sz) ;
	fileoff += rs ;

/* make and write out key-string table */

	if (rs >= 0) {
	    char	*kstab = nullptr ;

	    sz = strtab_strsize(ksp) ;

	    hf.stoff = fileoff ;
	    hf.stlen = sz ;

	    if ((rs = uc_malloc(sz,&kstab)) >= 0) {

	        rs = strtab_strmk(ksp,kstab,sz) ;

/* write out the key-string table */

	        if (rs >= 0) {
	            rs = filer_write(&varfile,kstab,sz) ;
	            fileoff += rs ;
	        }

/* make and write out the record-index table */

	        if (rs >= 0) {
		    uint	(*indtab)[3] = nullptr ;

	            itl = nextpowtwo(rtl) ;

	            hf.itoff = fileoff ;
	            hf.itlen = itl ;

	            sz = (itl + 1) * 3 * sizeof(int) ;

	            if ((rs = uc_malloc(sz,&indtab)) >= 0) {
			memset(indtab,0,sz) ;
	                rs = strlistmks_mkind(op,kstab,indtab,itl) ;
	                if (rs >= 0) {
	                    rs = filer_write(&varfile,indtab,sz) ;
	                    fileoff += rs ;
	                }

	                uc_free(indtab) ;
	            } /* end if (memory allocation) */

	        } /* end if (record-index table) */

	        uc_free(kstab) ;
	    } /* end if (memory allocation) */

	} /* end if (key-string table) */

/* write out the header -- again! */
ret2:
	filer_finish(&varfile) ;

	if (rs >= 0) {

	    hf.fsize = fileoff ;

	    if ((rs = strlisthdr_rd(&hf,buf,BUFLEN)) >= 0) {
	        bl = rs ;
	        rs = u_pwrite(op->nfd,buf,bl,0L) ;
	    }

#if	CF_MINMOD
	if (rs >= 0) {
	    rs = uc_fminmod(op->nfd,op->om) ;
	}
#endif /* CF_MINMOD */

	    if ((rs >= 0) && (op->gid != gidend)) {
		rs = u_fchown(op->nfd,-1,op->gid) ;
	    }

	} /* end if (succeeded?) */

/* we're out of here */
ret1:
	op->f.viopen = false ;
	rs1 = u_close(op->nfd) ;
	if (rs >= 0) rs = rs1 ;
	op->nfd = -1 ;

	if ((rs < 0) && (op->nfname[0] != '\0')) {
	    u_unlink(op->nfname) ;
	    op->nfname[0] = '\0' ;
	}

ret0:
	return rs ;
}
/* end subroutine (strlistmks_wrvarfile) */

int strlistmks_mkind(SLM *op,char *kst,uint (*it)[3], int il) noex {
	VE		ve ;
	uint		ki, hi ;
	uint		khash ;
	uint		(*rt)[1] ;
	int		rs = SR_OK ;
	int		rtl ;
	int		sc = 0 ;
	char	*kp ;

	rtl = rectab_getvec(&op->rectab,&rt) ;

#if	CF_FIRSTHASH
	{
	    VE		*vep ;
	    vecobj	ves ;
	    cint	sz = sizeof(VE) ;
	    cint	vo = VECOBJ_OCOMPACT ;
	    int		i ;
	    if ((rs = vecobj_start(&ves,sz,rtl,vo)) >= 0) {

	    for (int ri = 1 ; ri < rtl ; ri += 1) {

	        ki = rt[ri][0] ;
	        kp = kst + ki ;
	        khash = hash_elf(kp,-1) ;

	        hi = hashindex(khash,il) ;

	        if (it[hi][0] == 0) {
	            it[hi][0] = ri ;
	            it[hi][1] = (khash & INT_MAX) ;
	            it[hi][2] = 0 ;
	            sc += 1 ;
	        } else {
		    ve.ri = ri ;
		    ve.ki = ki ;
	            ve.khash = chash ;
	            ve.hi = hi ;
	            rs = vecobj_add(&ves,&ve) ;
	        }

	        if (rs < 0) break ;
	    } /* end for */

	    if (rs >= 0) {
	    for (i = 0 ; vecobj_get(&ves,i,&vep) >= 0 ; i += 1) {
	        sc += indinsert(rt,it,il,vep) ;
	    } /* end for */
	    }

	    vecobj_finish(&ves) ;
	    } /* end if (ves) */

	}
#else /* CF_FIRSTHASH */
	{
	for (int ri = 1 ; ri < rtl ; ri += 1) {

	    ki = rt[ri][0] ;
	    kp = kst + ki ;

	    khash = hash_elf(kp,-1) ;

	    hi = hashindex(khash,il) ;

	    ve.ri = ri ;
	    ve.ki = ki ;
	    ve.khash = khash ;
	    ve.hi = hi ;
	    sc += indinsert(rt,it,il,&ve) ;

	} /* end for */
	}
#endif /* CF_FIRSTHASH */

	it[il][0] = UINT_MAX ;
	it[il][1] = 0 ;
	it[il][2] = 0 ;
	if (sc < 0) {
	    sc = 0 ;
	}

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

static int rectab_start(RECTAB *rtp,int n) noex {
	int		rs = SR_OK ;
	int		sz ;
	void		*p ;
	if (n < 10) n = 10 ;
	rtp->i = 0 ;
	rtp->n = n ;
	sz = (n + 1) * 1 * sizeof(int) ;
	if ((rs = uc_malloc(sz,&p)) >= 0) {
	    typedef uint (*rectab_t)[1] ;
	    rtp->rectab = rectab_t(p) ;
	    rtp->rectab[0][0] = 0 ;
	    rtp->i = 1 ;
	}
	return rs ;
}
/* end subroutine (rectab_start) */

static int rectab_finish(RECTAB *rtp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (rtp->rectab) {
	    rs1 = uc_free(rtp->rectab) ;
	    if (rs >= 0) rs = rs1 ;
	    rtp->rectab = nullptr ;
	}
	return rs ;
}
/* end subroutine (rectab_finish) */

static int rectab_add(RECTAB *rtp,uint ki) noex {
	int		rs = SR_OK ;
	int		i = rtp->i ;
	if ((i + 1) > rtp->n) {
	    rs = rectab_extend(rtp) ;
	}
	if (rs >= 0) {
	    rtp->rectab[i][0] = ki ;
	    rtp->i += 1 ;
	}
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (rectab_add) */

static int rectab_extend(RECTAB *rtp) noex {
	int		rs = SR_OK ;
	if ((rtp->i + 1) > rtp->n) {
	    uint	(*va)[1] ;
	    int		nn = (rtp->n + 1) * 1 ;
	    int		sz ;
	    sz = (nn + 1)  * sizeof(int) ;
	    if ((rs = uc_realloc(rtp->rectab,sz,&va)) >= 0) {
	        rtp->rectab = va ;
	        rtp->n = nn ;
	    }
	} /* end if */
	return rs ;
}
/* end subroutine (rectab_extend) */

static int rectab_done(RECTAB *rtp) noex {
	int		i = rtp->i ;
	rtp->rectab[i][0] = UINT_MAX ;
	return i ;
}
/* end subroutine (rectab_done) */

#ifdef	COMMENT
static int rectab_count(RECTAB *rtp) noex {
	return rtp->i ;
}
/* end subroutine (rectab_count) */
#endif /* COMMENT */

static int rectab_getvec(RECTAB *rtp,uint (**rpp)[1]) noex {
	*rpp = rtp->rectab ;
	return rtp->i ;
}
/* end subroutine (rectab_getvec) */

static int filer_writefill(filer *bp,char *wbuf,int wlen) noex {
	int		rs ;
	int		len = 0 ;
	if (wlen < 0) {
	    wlen = (strlen(wbuf) + 1) ;
	}
	if ((rs = filer_write(bp,wbuf,wlen)) >= 0) {
	    cint	r = (wlen & 3) ;
	    len += rs ;
	    if (r > 0) {
	    	cint	nzero = (4 - r) ;
	        if (nzero > 0) {
	            rs = filer_write(bp,zerobuf,nzero) ;
	            len += rs ;
	        }
	    }
	} /* end if (filer_writer) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (filer_writefill) */

static int indinsert(uint (*rt)[1], uint (*it)[3],int il,VE *vep) noex {
	uint		nhash, chash ;
	uint		ri, ki ;
	uint		lhi, nhi, hi ;
	int		c = 0 ;

	hi = vep->hi ;
	nhash = vep->khash ;
	chash = (nhash & INT_MAX) ;

/* CONSTCOND */
	forever {
	    if (it[hi][0] == 0) break ;
	    ri = it[hi][0] ;
	    ki = rt[ri][0] ;
	    if (ki == vep->ki) break ;
	    it[hi][1] |= (~ INT_MAX) ;
	    nhash = hash_again(nhash,c++,STRLISTMKS_NSKIP) ;
	    hi = hashindex(nhash,il) ;
	} /* end while */

	if (it[hi][0] > 0) {

	    lhi = hi ;
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


