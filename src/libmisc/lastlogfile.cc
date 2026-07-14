/* LLF SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* manage reading or writing a LASTLOG file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-22, David A­D­ Morano
	This subroutine module was adopted for use from some previous
	code that performed the similar sorts of functions.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object:
	lastlogfile

	Description:
	This code is used to manage one LASTLOG type file.  This
	sort of file is usually used to track the last time that a
	person has logged in.  This function was implemented as
	part of PCS long before it was adopted as standard (or
	pseudo standard) practice in UNIX® proper.  This code
	represents a shift for PCS related software from using the
	older proprietary LASTLOG feature to the newer UNIX® standard
	(pseudo standard -- whatever) mechanism.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<ctime>			/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<sysval.hh>		/* LIBUC */
#include	<strwcpy.h>		/* LINUC */
#include	<ismisc.h>		/* LINUC */
#include	<utmpsizes.hh>		/* LINUC */
#include	<localmisc.h>		/* LINU */

#include	"lastlogfile.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	LLF		lastlogfile
#define	LLF_ENT		lastlogfile_ent
#define	LLF_CUR		lastlogfile_cur
#define	LLF_ENTSZ	szof(lastlogfile_ent)
#define	LLF_MAG		LASTLOGFILE_MAGIC
#define	LLF_FP		LASTLOGFILE_FILEPATH
#define	LLF_TIOPEN	30	/* seconds */
#define	LLF_LLINE	utmpsize.line
#define	LLF_LHOST	utmpsize.host


/* imported namespaces */

using libuc::mem ;		/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
local int lastlogfile_ctor(LLF *op,Args ... args) noex {
    	LASTLOGFILE	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = memclear(hop) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (lastlogfile_ctor) */

local int lastlogfile_dtor(LLF *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (lastlogfile_dtor) */

template<typename ... Args>
local inline int lastlogfile_magic(LLF *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magval == LLF_MAG) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (lastlogfile_magic) */

local int	lastlogfile_checkopen(LLF *) noex ;
local int	lastlogfile_fileclose(LLF *) noex ;


/* local variables */

static sysval		pagesz(sysval_ps) ;


/* exported variables */


/* exported subroutines */

int lastlogfile_open(LLF *op,cchar *fname,int oflags) noex {
	int		rs ;
	if (fname == nullptr) fname = LLF_FP ;
	if (fname[0] == '\0') fname = LLF_FP ;
	if (oflags == 0) oflags = O_RDONLY ;
	if ((rs = lastlogfile_ctor(op)) >= 0) {
	    op->oflags	= oflags ;
	    op->fd	= -1 ;
	    if ((rs = pagesz) >= 0) {
	        op->pagesz = rs ;
	        if (cchar *cp ; (rs = mem.strw(fname,-1,&cp)) >= 0) {
	            op->fname = cp ;
	            if ((rs = lastlogfile_checkopen(op)) >= 0) {
		        if (ustat sb ; (rs = u_fstat(op->fd,&sb)) >= 0) {
	                    op->fsize = sb.st_size ;
	                    op->mtime = sb.st_mtime ;
	                    op->magval = LLF_MAG ;
		        } /* end if (f_fstat) */
		        if (rs < 0) {
		            lastlogfile_fileclose(op) ;
		        } /* end if (error) */
	            } /* end if (file-open) */
	            if (rs < 0) {
			voidp vp = voidp(op->fname) ;
	                mem.free(vp) ;
	                op->fname = nullptr ;
	            } /* end if (error) */
	        } /* end if (memory-allocation) */
	    } /* end if (pagesz) */
	    if (rs < 0) {
		lastlogfile_dtor(op) ;
	    } /* end if (error) */
	} /* end if (lastlogfile_ctor) */
	return rs ;
} /* end subroutine (lastlogfile_open) */

int lastlogfile_close(LLF *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = lastlogfile_magic(op)) >= 0) {
	    if (op->fd >= 0) {
	        rs1 = u_close(op->fd) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->fname) {
	        voidp vp = voidp(op->fname) ;
	        rs1 = mem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->fname = nullptr ;
	    } /* end if (memory-release) */
	    {
	        rs1 = lastlogfile_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (lastlogfile_magic) */
	return rs ;
} /* end subroutine (lastlogfile_close) */

int lastlogfile_rdent(LLF *op,uid_t uid,LLF_ENT *ep) noex {
	int		rs ;
	if ((rs = lastlogfile_magic(op)) >= 0) {
	    cint esz = LLF_ENTSZ ;
	    rs = SR_ACCESS ;
	    if (isaccmode.rd(op->oflags)) {
		coff	loc = (uid * esz) ;
	        rs = SR_EOF ;
	        if (loc < op->fsize) {
		    LLF_ENT	e ;
		    if (ep == nullptr) ep = &e ;
	    	    if ((rs = lastlogfile_checkopen(op)) >= 0) {
	    		if ((rs = u_readp(op->fd,ep,esz,loc)) >= 0) {
			    if (ep->ll_tv.tv_sec == 0) rs = SR_BADSLT ;
			}
		    }
		} /* end if (within range) */
	    } /* end if (access-allowed) */
	} /* end if (lastlogfile_magic) */
	return rs ;
} /* end subroutine (lastlogfile_rdent) */

int lastlogfile_wrent(LLF *op,uid_t uid,LLF_ENT *ep) noex {
	int		rs ;
	if ((rs = lastlogfile_magic(op,ep)) >= 0) {
	    cint esz = LLF_ENTSZ ;
	    rs = SR_ACCESS ;
	    if (isaccmode.wr(op->oflags)) {
	        if ((rs = lastlogfile_checkopen(op)) >= 0) {
	    	    coff loc = (uid * esz) ;
	    	    rs = u_writep(op->fd,ep,esz,loc) ;
		}
	    } /* end if (access-allowed) */
	} /* end if (lastlogfile_magic) */
	return rs ;
} /* end subroutine (lastlogfile_wrent) */

int lastlogfile_rdinfo(LLF *op,uid_t uid,time_t *tp,
		char *lbuf,char *hbuf) noex {
	int		rs ;
	if ((rs = lastlogfile_magic(op)) >= 0) {
	    cint esz = LLF_ENTSZ ;
	    rs = SR_ACCESS ;
	    if (isaccmode.rd(op->oflags)) {
		coff loc = (uid * esz) ;
	        rs = SR_EOF ;
		if (loc >= op->fsize) {
	    	    if ((rs = lastlogfile_checkopen(op)) >= 0) {
		        LLF_ENT	e ;
	    	        if ((rs = u_readp(op->fd,&e,esz,loc)) >= 0) {
			    if ((rs > 0) && (e.ll_tv.tv_sec != 0)) {
	    	                if (tp) {
	                            *tp = e.ll_tv.tv_sec ;
		                }
	                        if (lbuf) {
	                            strwcpy(lbuf,e.ll_line,LLF_LLINE) ;
		                }
	                        if (hbuf) {
	                            strwcpy(hbuf,e.ll_host,LLF_LHOST) ;
		                }
	                    } else {
	                        rs = 0 ;
	                        if (tp) {
	                            *tp = 0 ;
		                }
	                        if (lbuf) {
	                            lbuf[0] = '\0' ;
		                }
	                        if (hbuf) {
	                            hbuf[0] = '\0' ;
		                }
	                    } /* end if */
	                } /* end if (u_readp) */
		    } /* end if (check-open) */
		} /* end if (file-size ok) */
	    } /* end if (access-allowed) */
	} /* end if (lastlogfile_magic) */
	return rs ;
} /* end subroutine (lastlogfile_rdinfo) */

int lastlogfile_wrinfo(LLF *op,uid_t uid,time_t t,cc *lbuf,cc *hbuf) noex {
	int		rs ;
	if ((rs = lastlogfile_magic(op)) >= 0) {
	    rs = SR_ACCESS ;
	    if (isaccmode.wr(op->oflags)) {
	        if ((rs = lastlogfile_checkopen(op)) >= 0) {
		    cint esz = LLF_ENTSZ ;
		    LLF_ENT	e = {} ;
	    	    if (t < 0) t = time(nullptr) ;
	    	    e.ll_tv.tv_sec = t ;
	    	    if (lbuf) {
	        	strncpy(e.ll_line,lbuf,LLF_LLINE) ;
	    	    }
	    	    if (hbuf) {
	        	strncpy(e.ll_host,hbuf,LLF_LHOST) ;
	    	    }
		    {
		        coff loc = (uid * esz) ;
	                rs = u_writep(op->fd,&e,esz,loc) ;
		    }
		} /* end if (check-open) */
	    } /* end if (access-allowed) */
	} /* end if (lastlogfile_magic) */
	return rs ;
} /* end subroutine (lastlogfile_wrinfo) */

int lastlogfile_check(LLF *op,time_t daytime) noex {
	int		rs ;
	if ((rs = lastlogfile_magic(op)) >= 0) {
	    if (op->fd >= 0) {
	        ustat	sb ;
	        bool	f_close = false ;
	        if ((u_fstat(op->fd,&sb) < 0) || (sb.st_mtime > op->mtime)) {
	            f_close = true ;
	        }
	        if (! f_close) {
	            if (daytime <= 0) daytime = time(nullptr) ;
	            if (daytime > (op->otime + LLF_TIOPEN)) {
	                f_close = true ;
	            }
	        }
	        if (f_close) {
	            rs = u_close(op->fd) ;
	            op->fd = -1 ;
	        }
	    } /* end if (file open) */
	} /* end if (magic) */
	return rs ;
} /* end subroutine (lastlogfile_check) */

int lastlogfile_curbegin(LLF *op,LLF_CUR *curp) noex {
	int		rs ;
	if ((rs = lastlogfile_magic(op,curp)) >= 0) {
	    curp->i = -1 ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (lastlogfile_curbegin) */

int lastlogfile_curend(LLF *op,LLF_CUR *curp) noex {
	int		rs ;
	if ((rs = lastlogfile_magic(op,curp)) >= 0) {
	    curp->i = -1 ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (lastlogfile_curend) */

int lastlogfile_curenum(LLF *op,LLF_CUR *curp,uid_t *up,
		time_t *tp,char *ln,char *hn) noex {
	int		rs ;
	if ((rs = lastlogfile_magic(op,curp)) >= 0) {
	    int		i = (curp->i < 0) ? 0 : (curp->i + 1) ;
	    while (rs >= 0) {
	        if ((rs = lastlogfile_rdinfo(op,i,tp,ln,hn)) > 0) break ;
	        i += 1 ;
	    } /* end while */
	    if (rs >= 0) {
	        if (up) *up = i ;
	        curp->i = i ;
	    } /* end if (ok) */
	} /* end if (magic) */
	return rs ;
} /* end subroutine (lastlogfile_curenum) */


/* private subroutines */

local int lastlogfile_checkopen(LLF *op) noex {
	int		rs = SR_OK ;
	if (op->fd < 0) {
	    rs = u_open(op->fname,op->oflags,0660) ;
	    op->fd = rs ;
	    op->otime = time(nullptr) ;
	} /* end if (needed open) */
	return (rs >= 0) ? op->fd : rs ;
} /* end subroutine (lastlogfile_checkopen) */

local int lastlogfile_fileclose(LLF *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->fd >= 0) {
	    rs1 = u_close(op->fd) ;
	    if (rs >= 0) rs = rs1 ;
	    op->fd = -1 ;
	}
	return rs ;
} /* end subroutine (lastlogfile_fileclose) */


