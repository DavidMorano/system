/* densitydb SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* manage reading or writing of a density file */
/* version %I% last-modified %G% */

#define	CF_CREAT	0		/* always create the file? */

/* revision history:

	= 2004-02-17, David A­D­ Morano
	This code module was inspired from the mail-message-id
	database (which is used to eliminate repeated mail messages).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	densitydb

	Description:
	This module manages the reading and writing of entries in
	a density file.

*******************************************************************************/

#include	<envstandards.h>	/* must be ordered fist to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/statvfs.h>
#include	<sys/mman.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<inttypes.h>
#include	<netdb.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<new>			/* |nothrow(3c++)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<serialbuf.h>
#include	<endian.h>
#include	<mkfnamesuf.h>
#include	<mkx.h>
#include	<strwcpy.h>
#include	<isfiledesc.h>
#include	<isnot.h>
#include	<iserror.h>
#include	<funmode.hh>
#include	<localmisc.h>

#include	"densitydb.h"
#include	"densitydbe.h"

import densityx_filehead ;
import filemagic ;

/* local defines */

#define	DENSITYDB_FILEMAGICA	"DENSITYDBA"
#define	DENSITYDB_FILEMAGICB	"DENSITYDBB"

#define	DENSITYDB_FS		"densitydb"
#define	DENSITYDB_FSA		"densitydba"
#define	DENSITYDB_FSB		"densitydbb"

#define	DENSITYDB_FLID		(16 + 4)
#define	DENSITYDB_FLHEAD	filehead::bufsz
#define	DENSITYDB_FLTOP		(DENSITYDB_FLID + DENSITYDB_FLHEAD)

#define	DENSITYDB_FOID		0
#define	DENSITYDB_FOHEAD	(DENSITYDB_FOID + DENSITYDB_FLID)
#define	DENSITYDB_FOTAB		(DENSITYDB_FOHEAD + DENSITYDB_FLHEAD)

#define	DENSITYDB_EBS		((var.entsz + 3) & (~ 3))

#define	DENSITYDB_BUFSIZE	(64 * 1024)
#define	DENSITYDB_READSIZE	(16 * 1024)

#define	DENSITYDB_FBUFLEN	(DENSITYDB_FLTOP + 9)

#define	DD			densitydb
#define	DD_ENT			densitydb_ent
#define	FM			filemagic
#define	FH			filehead

#define	TO_OPEN		(60 * 60)	/* maximum file-open time */
#define	TO_ACCESS	(2 * 60)	/* maximum access idle time */
#define	TO_LOCK		10		/* seconds */

#ifndef	CF_CREAT
#define	CF_CREAT	0		/* always create the file? */
#endif


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef const funmode	cfunmode ;


/* external subroutines */


/* external variables */


/* local structures */

struct oldentry {
	time_t		utime ;
	int		ei ;
} ;

namespace {
    struct vars {
	int		entsz ;
	operator int () noex ;
    } ; /* end struct (vars) */
    struct buffer {
	char		*bup ;			/* fixed buffer pointer */
	int		bsz ;			/* fixed buffer size */
	int		off ;			/* file offset of valid area */
	int		len ;			/* length of valid area */
    } ; /* end struct (buffer) */
    struct filemagic {
	char		magic[16] ;
	uchar		vetu[4] ;
	int rd(char *) noex ;
	int wr(cchar *) noex ;
    } ; /* end struct filemagic) */
 } /* end namespace */


/* forward references */

template<typename ... Args>
static int densitydb_ctor(densitydb *op,Args ... args) noex {
    	DENSITYDB	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEMT ;
	    memclear(hop) ;
	    if (buffer *buf ; (bup = new(nothrow) buffer) != np) {
		op->bup = bup ;
	        if (filehead *fhp ; (fhp = new(nothrow) filehead) != np) {
		    op->fhp = fhp ;
		    rs = SR_OK ;
		} /* end if (new-filehead) */
		if (rs < 0) {
		    delete bup ;
		    op->buf = nullptr ;
		}
	    } /* end if (new-buffer) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (densitydb_ctor) */

static int densitydb_dtor(densitydb *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->fhp) {
		fileheader *fhp = (filehead *) op->fhp ;
		delete fhp ;
		op->fhp = nullptr ;
	    }
	    if (op->bup) {
		buffer *bup = (buffer *) op->bup ;
		delete bup ;
		op->bup = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (densitydb_dtor) */

template<typename ... Args>
static inline int densitydb_magic(densitydb *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == DENSITYDB_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (densitydb_magic) */

static int densitydb_openbegin(DD *,cchar *) noex ;
static int densitydb_opener(DD *,char *,cchar *,int) noex ;
static int densitydb_openend(DD *) noex ;

static int densitydb_fileopen(DD *,time_t) noex ;
static int densitydb_fileclose(DD *) noex ;
static int densitydb_fileinit(DD *,time_t) noex ;
static int densitydb_filechanged(DD *) noex ;
static int densitydb_filecheck(DD *,time_t,cfunmode) noex ;
static int densitydb_lockacq(DD *,time_t,cfunmode) noex ;
static int densitydb_lockrel(DD *) noex ;
static int densitydb_buf(DD *,uint,int,char **) noex ;
static int densitydb_bufupdate(DD *,uint,int,cchar *) noex ;
static int densitydb_bufbegin(DD *) noex ;
static int densitydb_bufend(DD *) noex ;
static int densitydb_writehead(DD *) noex ;

static int filemagic(FM *,char *,int) noex ;

static int matfield(cchar *,int,cchar *,int) noex ;


/* local variables */

constexpr cpcchar	localfs[] = {
	"ufs",
	"tmpfs",
	"lofs",
	"pcfs",
	"hfs",
	"vxfs",
	nullptr
} ;

cint		maglen		= DENSITYDB_MAGLEN ;

constexpr char	magstr[]	= DENSITYDB_MAGSTR ;

cbool		f_creat 	= CF_CREAT ;


/* exported variables */


/* exported subroutines */

static int densitydb_opens(DD *,cc *) noex ;

int densitydb_open(DD *op,cc *fname,int oflags,mode_t om,int maxent) noex {
	int		rs ;
	int		fcreate = false ;
	if ((rs = densitydb_ctor(op,fname)) >= 0) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
	        if_constexpr (f_creat) {
	            oflags |= O_CREAT ;
	        }
	        op->oflags = (oflags &= (~ O_TRUNC)) ;
		if ((rs = ucpagesize) >= 0) {
		    static cint		rsv = var ;
	            op->pagesz = rs ;
		    if ((rs = rsv) >= 0) {
	        	op->om = om ;
	        	op->maxent = maxent ;
	        	op->ebs = uceil(var.entsz,szof(int)) ;
		        rs = densitydb_opens(op,fname) ;
			fcreate = (rs > 0) ;
		    } /* end if (vars) */
	        } /* end if (ucpagesize) */
	    } /* end if (valid) */
	    if (rs < 0) {
		densitydb_dtor(op) ;
	    }
	} /* end if (densitydb_ctor) */
	return (rs >= 0) ? fcreate : rs ;
}
/* end subroutine (densitydb_open) */

static int densitydb_opens(DD *op,cc *fname) noex {
    	int		rs ;
	int		fcreate = false ;
        if ((rs = densitydb_bufbegin(op)) >= 0) {
            if ((rs = densitydb_openbegin(op,fname)) >= 0) {
		custime		dt = getustime ;
		fcreate = rs ;
                if (cchar *cp ; (rs = uc_malloc(fname,-1,&cp)) >= 0) {
                    cint	am = (op->oflags & O_ACCMODE) ;
                    op->fname = cp ;
                    op->fl.writable = ((am == O_WRONLY) || (am == O_RDWR)) ;
                    op->tiopen = dt ;
                    op->tiopen = dt ;
                    if (USTAT sb ; (rs = u_fstat(op->fd,&sb)) >= 0) {
                        op->timod = uint(sb.st_mtime) ;
                        op->filesz = intsat(sb.st_size) ;
                        if ((rs = isfsremote(op->fd)) >= 0) {
                            op->fl.remote = (rs > 0) ;
                            if ((rs = densitydb_fileinit(op,dt)) >= 0) {
                                op->magic = DENSITYDB_MAGIC ;
                            }
                        }
                    }
                    if (rs < 0) {
                        uc_free(op->fname) ;
                        op->fname = nullptr ;
                    }
                } /* end if (m-a) */
                if (rs < 0) {
		    densitydb_openend() ;
                }
            } /* end if (densitydb_openbegin) */
            if (rs < 0) {
                densitydb_bufend(op) ;
            }
        } /* end if (densitydb_bufbegin) */
	return (rs >= 0) ? fcreate : rs ;
}
/* end subroutine (densitydb_opens) */

int densitydb_close(DD *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = densitydb_magic(op)) >= 0) {
	    {
	        rs1 = desnitydb_openend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = densitydb_bufend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->fname) {
	        rs1 = uc_free(op->fname) ;
	        if (rs >= 0) rs = rs1 ;
	        op->fname = nullptr ;
	    }
	    {
	        rs1 = densitydb_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (densitydb_close) */

int densitydb_count(DD *op) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = densitydb_magic(op)) >= 0) {
	    c = (op->filesz - DENSITYDB_FOTAB) / var.entsz ;
	}
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (densitydb_count) */

int densitydb_curbegin(DD *op,DD_CUR *curp) noex {
    	int		rs ;
	if ((rs = densitydb_magic(op,curp)) >= 0) {
	    curp->i = -1 ;
	}
	return rs ;
}
/* end subroutine (densitydb_curbegin) */

int densitydb_curend(DD *op,DD_CUR *curp) noex {
    	int		rs ;
	if ((rs = densitydb_magic(op,curp)) >= 0) {
	    curp->i = -1 ;
	}
	return rs ;
}
/* end subroutine (densitydb_curend) */

static int densitydb_curenums(DD *,DD_CUR *,DD_ENT *) noex ;

int densitydb_curenum(DD *op,DD_CUR *curp,DD_ENT *ep) noex {
	int		rs ;
	int		ei = 0 ; /* return-value */
	if ((rs = density_magic(op,curp,ep)) >= 0) {
	    rs = SR_NOTFOUND ;
	    if (op->fl.fileinit) {
		rs = SR_LOCKLOST ;
		if (! op->fl.cursorlockbroken) {
		    rs = densitydb_curenums(op,curp,ep) ;
		    ei = rs ;
		} /* end if (cursor-lock not broken) */
	    } /* end if (file-initialized) */
	} /* end if (magic) */
	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (densitydb_curenum) */

static int densitydb_curenums(DD *op,DD_CUR *curp,DD_ENT *ep) noex {
	cfunmode	fc = funmode::rd ;
	custime		dt = getustime ;
	int		rs ;
	int		ei = 0 ;
	if ((rs = densitydb_filecheck(op,dt,fc)) >= 0) {
	    cint	fotab = DENSITYDB_FOTAB ;
	    ei = (curp->i < 0) ? 0 : curp->i + 1 ;
	    rs = SR_EOF ;
	    if (op->fl.fileinit) {
		cuint	eoff = uint(fotab + (ei * op->ebs)) ;
		rs = SR_NOTFOUND ;
		if ((eoff + op->ebs) <= op->filesz) {
		    char	*bp ;
		    if ((rs = densitydb_buf(op,eoff,op->ebs,&bp)) >= 0) {
			if (rs < op->ebs) {
			    rs = SR_EOF ;
			}
		        if ((rs >= 0 && ep) {
	    	            rs = densitydbe_wr(ep,bp,var.entsz)
		        } /* end if */
		        if (rs >= 0) {
		            curp->i = ei ;
		            op->fl.cursoracc = true ;
		        }
		    } /* end if (densitydb_buf) */
		} /* end if (valid) */
	    } /* end if (file-initialized) */
	} /* end if (densirydb_filecheck) */
	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (densitydb_curenums) */

int densitydb_update(DD *op,time_t dt,int idx,DD_ENT *ep) noex {
	int		rs ;
	int		ei = 0 ; /* return-value */
	if ((rs = density_magic(op,curp,ep)) >= 0) {
	    rs = SR_INVALID ;
	    id (idx >= 0) {
		rs = SR_NOTFOUND ;
		if (op->fl.fileinit) {
		    if (dt == 0) dt = getustime ;
	DENSITYDBE	m0 ;
	off_t	uoff ;
	uint		eoff ;
	int		wlen ;
	int		f_addition = false ;
	int		f_bufupdate = false ;
	char		ebuf[DENSITYDBE_SIZE + 4] ;
	char		*bep ;


/* do we have proper file access? */


	cfunmode	fc = funmode::rd ;
	rs = densitydb_filecheck(op,dt,fc) ;

	if ((rs < 0) || (! op->fl.fileinit))
	    goto ret0 ;

/* continue with the search */

	ei = index ;
	if (ei > (op->h.nents - 1))
	    ei = (op->h.nents - 1) ;

	if (ep != nullptr) {
	    densitydbe_all(bep,DENSITYDBE_SIZE,1,ep) ;
	    m0 = *ep ;
	} else {
	    densitydbe_all(bep,DENSITYDBE_SIZE,1,&m0) ;
	}

	    m0.count += 1 ;
	    m0.utime = dt ;
	    wlen = densitydbe_all(bep,DENSITYDBE_SIZE,0,&m0) ;

/* update the in-core file buffer as needed or as appropriate */

	    if ((rs >= 0) && f_bufupdate && op->fl.writable) {
	        eoff = DENSITYDB_FOTAB + (ei * op->ebs) ;
	        densitydb_bufupdate(op,eoff,wlen,ebuf) ;
	    }

	if ((rs >= 0) && op->fl.writable) {

/* write back this entry */

	    eoff = DENSITYDB_FOTAB + (ei * op->ebs) ;

	    uoff = eoff ;
	    rs = u_pwrite(op->fd,bep,wlen,uoff) ;

	    if (rs >= wlen) {

	        if (dt == 0)
	            dt = getustime ;

	        op->h.wcount += 1 ;
	        op->h.wtime = dt ;
	        if (f_addition) {
	            op->h.nents += 1 ;
	            op->filesz += wlen ;
	        }

	        rs = densitydb_writehead(op) ;

	        if ((rs >= 0) && op->fl.remote)
	            u_fsync(op->fd) ;

	    } /* end if (data write was successful) */

	} /* end if (writing updated entry to file) */

/* update access time as appropriate */

	if (dt == 0)
	    dt = getustime ;

	op->tiopen = dt ;

/* done */
ret0:
	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (densitydb_update) */

int densitydb_check(DD *op,time_t dt) noex {
	int		rs = SR_OK ;
	if (op->fd >= 0) {
	    if (dt == 0) dt = getustime ;
	    if (! (op->fl.readlocked || op->fl.writelocked)) {
		bool	f = false ;
	        f = f || ((dt - op->tiopen) > TO_ACCESS) ;
		f = f || ((dt - op->tiopen) > TO_OPEN) ;
		if (f) {
		    rs = densitydb_fileclose(op) ;
		}
	    } /* end if (not locked) */
	} /* end if (was open) */
	return rs ;
}
/* end subroutine (densitydb_check) */


/* private subroutines */

static int densitydb_openbegin(DD *op,cchar *fname) noex {
	cint		of = (op->oflags & (~ O_CREAT)) ;
	int		rs ;
	int		fcreate = false ;
	if (char *tbuf ; (rs = mallocxx_mp(&tbuf)) >= 0) {
	    if ((rs = densitydb_opener(op,tbuf,fname,of)) >= 0) {
		rs = 0 ;
	    } else if (isNotPresent(rs)) {
	        of = op->oflags ;
	        if (of & O_CREAT) {
		    fcreate = true ;
	            rs = densitydb_opener(op,tbuf,fname,of) ;
	        }
	    }
	    rs = rsfree(rs,tbuf) ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? fcreate : rs ;
}
/* end subroutine (densitydb_openbegin) */

static int densitydb_opener(DD *op,char *tbuf,cchar *fname,int of) noex {
	int		rs ;
	cchar		*suf = DENSITYDB_FS ;
	cmode		om = op->om ;
	if ((rs = mkfnamesuf(tbuf,fname,suf)) >= 0) {
	    if ((rs = u_open(tbuf,of,om)) >= 0) {
	        op->fd = rs ;
	    } else if (isNotPresent(rs)) {
		suf = DENSITYDB_FSA ;
	        if ((rs = mkfnamesuf(tbuf,fname,suf)) >= 0) {
	    	    if ((rs = u_open(tbuf,of,om)) >= 0) {
	                op->fd = rs ;
	    	    } else if (isNotPresent(rs)) {
		        suf = DENSITYDB_FSB ;
	                if ((rs = mkfnamesuf(tbuf,fname,suf)) >= 0) {
	    	    	    if ((rs = u_open(tbuf,of,om)) >= 0) {
	              		op->fd = rs ;
	    	    	    }
			}
		    }
		}
	    }
	}
	return rs ;
}
/* end subroutine (densitydb_opener) */

static int densitydb_openend(DD *op) noex {
    	int		rs = SR_OK ;
	int		rs1 ;
	if (op->fd >= 0) {
	    rs1 = u_close(op->fd) ;
	    if (rs >= 0) rs = rs1 ;
	    op->fd = -1 ;
	}
	return rs ;
}
/* end subroutine (densitydb_openend) */

static int densitydb_filecheck(DD *op,time_t dt,cfunmode fc) noex {
	int		rs = SR_OK ;
	int		fch = false ;
	if (dt == 0) dt = getustime ;
	if ((rs = densitydb_fileopen(op,dt)) >= 0) {
	    if ((! op->fl.readlocked) && (! op->fl.writelocked)) {
	        if ((rs = densitydb_lockacq(op,dt,fc)) >= 0) {
	    	    rs = densitydb_filechanged(op) ;
	    	    fch = (rs > 0) ;
		    if (rs < 0) {
			densitydb_lockrel(op) ;
		    }
		} /* end if (densitydb_lockacq) */
	    } /* end if (capture lock) */
	} /* end if (densitydb_fileopen) */
	return (rs >= 0) ? fch : rs ;
}
/* end subroutine (densitydb_filecheck) */

static int densitydb_fileinit(DD *op,time_t dt) noex {
	FM	fm ;
	int		rs ;
	int		bl ;
	int		f_locked = false ;
	char		fbuf[DENSITYDB_FBUFLEN + 1], *bp ;
	char		*cp ;

	if (op->filesz == 0) {

	    u_seek(op->fd,0L,SEEK_SET) ;

	    op->fl.fileinit = false ;
	    if (op->fl.writable) {

	        if (! op->fl.writelocked) {
		    const funmode	fwr = funmode:wr ;
	            rs = densitydb_lockacq(op,dt,fwr) ;

	            if (rs < 0)
	                goto ret0 ;

	            f_locked = true ;
	        }

/* write the file magic and header stuff */

/* file magic */

	        strwcpy(fm.magic,DENSITYDB_FILEMAGICB,14) ;

	        fm.vetu[0] = DENSITYDB_FILEVERSION ;
	        fm.vetu[1] = DENSITYDB_ENDIAN ;
	        fm.vetu[2] = 0 ;
	        fm.vetu[3] = 0 ;

	        bl = 0 ;
	        bl += filemagic((fbuf + bl),0,&fm) ;

/* file header */

	        memclear(&op->h) ;

	        bl += filehead(&op->h,(fbuf + bl),0) ;

/* write them to the file */

	        rs = u_pwrite(op->fd,fbuf,bl,0L) ;

	        if (rs > 0) {

	            op->filesz = rs ;
	            op->timod = dt ;
	            if (op->fl.remote)
	                u_fsync(op->fd) ;

	            densitydb_bufupdate(op,0,bl,fbuf) ;

	        }

	        op->fl.fileinit = (rs >= 0) ;

	    } /* end if (writing) */

	} else if (op->filesz >= DENSITYDB_FOTAB) {
	    int	f ;

/* read the file header */

	    if (! op->fl.readlocked) {
		const funmode	frd = funmode:rd ;
	        rs = densitydb_lockacq(op,dt,frd) ;

	        if (rs < 0)
	            goto ret0 ;

	        f_locked = true ;
	    }

	    rs = u_pread(op->fd,fbuf,DENSITYDB_FBUFLEN,0L) ;

	    if (rs >= DENSITYDB_FLTOP) {

	        bl = 0 ;
	        bl += filemagic((fbuf + bl),1,&fm) ;

	        filehead((fbuf + bl),1,&op->h) ;

	        f = (strcmp(fm.magic,DENSITYDB_FILEMAGICB) == 0) ;
	        f = f && (fm.vetu[0] <= DENSITYDB_FILEVERSION) ;
	        f = f && (fm.vetu[1] == DENSITYDB_ENDIAN) ;
	        if (! f)
	            rs = SR_BADFMT ;

	        op->fl.fileinit = f ;

	    }

	} /* end if */

/* if we locked, we unlock it, otherwise leave it ! */

	if (f_locked)
	    densitydb_lockrel(op) ;

/* we're out of here */
ret0:
	return rs ;
}
/* end subroutine (densitydb_fileinit) */

static int densitydb_filechanged(DD *op) noex {
	int		rs ;
	int		fch = false ;
	if (USTAT sb ; (rs = u_fstat(op->fd,&sb)) >= 0) {
	    cint	fsz = intsat(sb.st_size) ;
	    if (fsz >= DENSITYDB_FOTAB) {
	        fch = fch || (sb.st_size != op->filesz) ;
	        fch = fch || (sb.st_mtime != op->timod) ;
	    if ((! fch) && op->fl.fileinit) {
	        char	hbuf[DENSITYDB_FLTOP + 1] ;
	        id ((rs = u_pread(op->fd,hbuf,DENSITYDB_FLTOP,0z)) >= 0) {
	            if (rs < DENSITYDB_FLTOP) {
	                op->fl.fileinit = false ;
	            }
	            if (rs > 0) {
	                if (FH h ; (rs = h.wr(hbuf,rs)) >= 0) {

	        fch = (op->h.wtime != h.wtime) ||
	            (op->h.wcount != h.wcount) ||
	            (op->h.nents != h.nents) ;

	        if (fch)
	            op->h = h ;

	    }

	} /* end if (reading file header) */

	    } else {
	        op->fl.fileinit = false ;
	    } /* end if */
	} else if (isNotPresent(rs)) {
	    rs = SR_OK ;
	    fch = true ;
	} /* end if (stat) */
	if (fch) {
	    op->b.len = 0 ;
	    op->filesz = sb.st_size ;
	    op->timod = sb.st_mtime ;
	}
	return (rs >= 0) ? fch : rs ;
}
/* end subroutine (densitydb_filechanged) */

static int densitydb_lockacq(DD *op,time_t dt,cfunmode fc) noex {
	int		rs = SR_OK ;
	bool		falready = false ;
	if ((rs = densitydb_fileopen(op,dt)) >= 0) {
	    int		lockcmd = -1 ;
	    if ((fc == funmode::rd) || (! op->fl.writable)) {
	        f_already = op->fl.readlocked ;
	        op->fl.readlocked = true ;
	        op->fl.writelocked = false ;
	        lockcmd = F_RLOCK ;
	    } else {
	        f_already = op->fl.writelocked ;
	        op->fl.readlocked = false ;
	        op->fl.writelocked = true ;
	        lockcmd = F_WLOCK ;
	    } /* end if */
	    if ((! falready) && (lockcmd >= 0)) {
		cint	to = TO_LOCK ;
	        rs = lockfile(op->fd,lockcmd,0z,0z,to) ;
	    }
	} /* end if (ok) */
	return rs ;
}
/* end subroutine (densitydb_lockacq) */

static int densitydb_lockrel(DD *op) noex {
	int		rs = SR_OK ;
	if ((op->fl.readlocked || op->fl.writelocked)) {
	    if (op->fd >= 0) {
	        rs = lockfile(op->fd,F_ULOCK,0z,0z,TO_LOCK) ;
	    }
	    op->fl.readlocked = false ;
	    op->fl.writelocked = false ;
	}
	return rs ;
}
/* end subroutine (densitydb_lockrel) */

static int densitydb_fileopen(DD *op,time_t dt) noex {
	int		rs = SR_OK ;
	if (op->fd < 0) {
	    if ((rs = u_open(op->fname,op->oflags,op->om)) >= 0) {
	        op->fd = rs ;
	        uc_closeonexec(op->fd,true) ;
	        op->tiopen = dt ;
	    }
	}
	return (rs >= 0) ? op->fd : rs ;
}
/* end subroutine (densitydb_fileopen) */

int densitydb_fileclose(DD *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->fd >= 0) {
	    rs1 = u_close(op->fd) ;
	    if (rs >= 0) rs = rs1 ;
	    op->fd = -1 ;
	    op->tiopen = 0 ;
	}
	return rs ;
}
/* end subroutine (densitydb_fileclose) */

static int densitydb_bufbegin(DD *op) noex {
	cint		bsz = DENSITYDB_BUFSIZE ;
	int		rs ;
	op->b.off = 0 ;
	op->b.len = 0 ;
	op->b.bsz = 0 ;
	op->b.bup = nullptr ;
	if (char *bp ; (rs = uc_malloc(bsz,&bp)) >= 0) {
	    op->b.bup = bp ;
	    op->b.bsz = bsz ;
	}
	return rs ;
}
/* end subroutine (densitydb_bufbegin) */

static int densitydb_bufend(DD *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->b.bup) {
	    rs1 = uc_free(op->b.bup) ;
	    if (rs >= 0) rs = rs1 ;
	    op->b.bup = nullptr ;
	}
	op->b.bsz = 0 ;
	op->b.len = 0 ;
	return rs ;
}
/* end subroutine (densitydb_bufend) */

static int densitydb_buf(DD *op,uint roff,int rlen,char **rpp) noex {
	off_t		foff ;
	uint		bext, bend, fext, fend ;
	uint		rext = (roff + rlen), ext ;
	int		rs = SR_OK ;
	int		len = rlen ;
	char		*rbuf ;

	/* do we need to read in more data? */

	fext = op->b.off + op->b.len ;
	if ((roff < op->b.off) || (rext > fext)) {

	    /* can we do an "add-on" type read operation? */

	    bend = op->b.off + op->b.bsz ;
	    if ((roff >= op->b.off) && (rext <= bend)) {

	        foff = op->b.off + op->b.len ;
	        rbuf = op->b.bup + op->b.len ;

	        ext = roff + MAX(rlen,DENSITYDB_READSIZE) ;
	        fext = uceil(ext,op->pagesz) ;

	        if (fext > bend)
	            fext = bend ;

	        len = (fext - foff) ;

	        if ((rs = u_pread(op->fd,rbuf,len,foff)) >= 0) {
	            op->b.len += rs ;
	            len = MIN(((op->b.off + op->b.len) - roff),rlen) ;
	        }

	    } else {

	        op->b.off = roff ;
	        op->b.len = 0 ;

	        bend = roff + op->b.bsz ;

	        foff = roff ;
	        rbuf = op->b.bup ;

	        ext = roff + MAX(rlen,DENSITYDB_READSIZE) ;
	        fext = uceil(ext,op->pagesz) ;

	        if (fext > bend)
	            fext = bend ;

	        len = fext - foff ;
	        if ((rs = u_pread(op->fd,op->b.bup,len,foff)) >= 0) {
	            op->b.len = rs ;
	            len = MIN(rs,rlen) ;
	        }

	    } /* end if */

	} /* end if (needed to read more data) */

	if (rpp != nullptr) {
	    *rpp = op->b.bup + (roff - op->b.off) ;
	}

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (densitydb_buf) */

static int densitydb_bufupdate(DD *op,uint roff,int rbuflen,cchar *rbuf) noex {
	uint		boff, bext ;
	uint		rext = roff + rbuflen ;
	int		buflen, bdiff ;

	buflen = op->b.len ;
	boff = op->b.off ;
	bext = boff + buflen ;

	if (roff < boff) {

	    if (rext <= boff)
	        return 0 ;

	    rbuf += (boff - roff) ;
	    rbuflen -= (boff - roff) ;
	    roff = boff ;

	}

	if (rext > bext) {

	    if (roff >= bext)
	        return 0 ;

	    rbuflen -= (rext - bext) ;
	    rext = bext ;

	}

	if (rbuflen > 0) {
	    bdiff = roff - boff ;
	    memcpy((op->b.bup + bdiff),rbuf,rbuflen) ;
	}

	return rbuflen ;
}
/* end subroutine (densitydb_bufupdate) */

static int densitydb_writehead(DD *op) noex {
	int		rs ;
	char		fbuf[DENSITYDB_FBUFLEN + 1] ;
	if ((rs = filehead(fbuf,0,&op->h)) >= 0) {
	    coff	uoff = DENSITYDB_FOHEAD ;
	    cint	bl = rs ;
	    rs = u_pwrite(op->fd,fbuf,bl,uoff) ;
	}
	return rs ;
}
/* end subroutine (densitydb_writehead) */

vars::operator int () noex {
    	int		rs ;
	int		rs1 ;
	if (densitydbe de ; (rs = de.start) >= 0) {
	    {
	        rs = de.entsz ;
	        entsz = rs ;
	    }
	    rs1 = de.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (densitydbe) */
	return rs ;
}
/* end method (vars::operator) */


