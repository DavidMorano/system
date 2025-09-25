/* mailalias_obj SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* manage a MAILALIAS object */
/* version %I% last-modified %G% */

#define	CF_DEFPROFILE	0		/* always use default MA-profile */

/* revision history:

	= 2003-06-11, David A­D­ Morano
	I snarfed this file from some hardware research I was doing
	since it seemed be a pretty good match for the present
	algorithm needs.  We will see how it works out!

*/

/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	mailalias

	Description:
	This module is used to manage a MAILALIAS object.  We always
	check if your alias key is the string "Postmater" (in a
	case-insensitive way) and if it is, we convert it to LOWER
	case before doing the |mailalias_fetch()|.  Also, unlike
	other fetching (with other DBs), there is no concept of
	"prefix" match fetching!  We use TWO string tables in this
	DB (unlike some other similar DBs).  We use one string table
	for keys and another for values.  I sort of did this for
	fun (so far).  This is actually potentially less space-efficient
	than using just one string table since strings that are
	both a key and a value are not combined in one table but
	rather appear separately in each of the two tables.  However,
	the up side is that:

	a. making a record table of only keys is done by just taking
	advantage of the strtab method for that

	b. we easily can provide an interface to transverse or
	otherwise just query the keys if we want to later

	c. we can do more space-efficient building of the DB file
	since we only need to keep the smaller key string table
	around for the main indexing!

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/mman.h>		/* Memory Management */
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<bit>			/* |rotl(3c++)| + |rotr(3c++)| */
#include	<strings.h>		/* |strncasecmp(3c)| */
#include	<usystem.h>
#include	<getbufsize.h>
#include	<sysval.hh>
#include	<bufsizevar.hh>
#include	<mallocxx.h>
#include	<endian.h>
#include	<vecobj.h>
#include	<vecstr.h>
#include	<linebuffer.h>
#include	<bfile.h>
#include	<opentmp.h>
#include	<field.h>
#include	<kvsfile.h>
#include	<strtab.h>
#include	<intceil.h>
#include	<hash.h>
#include	<strn.h>
#include	<sfx.h>
#include	<snx.h>
#include	<sncpyx.h>
#include	<mkx.h>
#include	<mkpathx.h>
#include	<mkpathxw.h>
#include	<mkpathx.h>
#include	<strwcpy.h>
#include	<mkdirs.h>
#include	<ids.h>
#include	<permx.h>
#include	<hashindex.h>
#include	<randlc.h>
#include	<matxstr.h>
#include	<char.h>
#include	<mkchar.h>
#include	<mailvalues.hh>
#include	<hasx.h>
#include	<isfiledesc.h>
#include	<isnot.h>
#include	<iserror.h>
#include	<ischarx.h>
#include	<localmisc.h>		/* |MODP2| */

#include	"mailalias.h"
#include	"mailaliashdr.h"
#include	"dbmake.hh"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |getlenstr(3u)| */

/* local defines */

#define	MAILALIAS_DNAME		"var/mailalias"
#define	MAILALIAS_DIRMODE	0775

#define	MAILALIAS_IDOFF		0
#define	MAILALIAS_IDLEN		(MAILALIAS_MAGICSIZE + szof(uint))

#define	MAILALIAS_HEADOFF	MAILALIAS_IDLEN
#define	MAILALIAS_HEADLEN	(mailaliashdr_overlast * szof(uint))

#define	MAILALIAS_TOPLEN	(MAILALIAS_IDLEN + MAILALIAS_HEADLEN)
#define	MAILALIAS_BUFOFF	(MAILALIAS_HEADOFF + MAILALIAS_HEADLEN)

#define	MA			mailalias
#define	MA_CUR			mailalias_cur
#define	MA_INFO			mailalias_info

#define	TO_APROFILE	(1 * 60)
#define	TO_FILECOME	15		/* timeout for file to "come in" */
#define	TO_LOCK		(5 * 60)
#define	TO_OPEN		(60 * 60)
#define	TO_ACCESS	(1 * 60)
#define	TO_CHECK	5		/* minimum check interval */
#define	TO_FILECHECK	5		/* DB file check */
#define	TO_FILEOLD	10		/* backing-store check */

#define	NREC_GUESS	100		/* guess of number of records */

#ifndef	CF_DEFPROFILE
#define	CF_DEFPROFILE	0		/* always use default MA-profile */
#endif


/* imported namespaces */

using namespace	mailutils ;		/* namespace */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::rotl ;			/* subroutine-template */
using std::rotr ;			/* subroutine-template */
using libuc::libmem ;			/* variable */
using std::nothrow ;			/* constant */


/* local typedefs */

#ifndef	TYPEDEF_MT
#define	TYPEDEF_MT
typedef mode_t		m_t ;
#endif


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct vars {
	int		mailaddrlen ;
	int		mailaliaslen ;
	operator int () noex ;
    } ; /* end struct (vars) */
}


/* forward references */

template<typename ... Args>
static int mailalias_ctor(mailalias *op,Args ... args) noex {
    	MAILALIAS	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = SR_NOMEM ;
	    memclear(hop) ;
	    if ((op->afp = new(nothrow) vecstr) != nullptr) {
	        if ((op->idp = new(nothrow) ids) != nullptr) {
		    rs = SR_OK ;
	        } /* end if (new-ids) */
		if (rs < 0) {
		    delete op->afp ;
		    op->afp = nullptr ;
		}
	    } /* end if (new-vecstr) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mailalias_ctor) */

static int mailalias_dtor(mailalias *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->idp) {
		delete op->idp ;
		op->idp = nullptr ;
	    }
	    if (op->afp) {
		delete op->afp ;
		op->afp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mailalias_dtor) */

template<typename ... Args>
static inline int mailalias_magic(mailalias *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == MAILALIAS_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (mailalias_magic) */

static int	mailalias_opener(MA *op) noex ;
static int	mailalias_hdrload(MA *) noex ;
static int	mailalias_hdrloader(MA *) noex ;
static int	mailalias_enterbegin(MA *,time_t) noex ;
static int	mailalias_enterend(MA *,time_t) noex ;

static int	mailalias_fileopen(MA *,time_t) noex ;
static int	mailalias_fileclose(MA *) noex ;
static int	mailalias_filechanged(MA *,USTAT *) noex ;
static int	mailalias_mapbegin(MA *,time_t dt) noex ;
static int	mailalias_mapend(MA *) noex ;
static int	mailalias_keymatch(MA *,int,cchar *) noex ;
static int	mailalias_dbopen(MA *,time_t) noex ;
static int	mailalias_dbopenfile(MA *,time_t) noex ;
static int	mailalias_dbopenmake(MA *,time_t) noex ;
static int	mailalias_dbopenwait(MA *) noex ;
static int	mailalias_isremote(MA *) noex ;
static int	mailalias_dbclose(MA *) noex ;
static int	mailalias_dbmake(MA *,time_t) noex ;
static int	mailalias_dbmaker(MA *,time_t,cchar *) noex ;
static int	mailalias_dbmaking(MA *,int,time_t,int) noex ;
static int	mailalias_fileold(MA *,time_t) noex ;
static int	mailalias_aprofile(MA *,time_t) noex ;
static int	mailalias_aprofiler(MA *,char *,int,cchar *) noex ;

static int	mailalias_checkchanged(MA *,time_t) noex ;
static int	mailalias_checkold(MA *,time_t) noex ;

static int	mailalias_mapcheck(MA *,time_t) noex ;


/* local variables */

constexpr cpcchar	defprofile[] = {
	"/etc/mail/aliases",
	"etc/mail/aliases",
	"var/mail/nis.aliases",
	nullptr
} ;

constexpr cpcchar	aptabsched[] = {
	"etc/mail/mail.aptab",
	"etc/mail/aptab",
	"etc/mail.aptab",
	"etc/aptab",
	nullptr
} ;

static sysval		pagesize(sysval_ps) ;
static bufsizevar	maxpathlen(getbufsize_mp) ;
static vars		var ;

constexpr bool		f_defprofile = CF_DEFPROFILE ;

constexpr cchar		pm[] = "Postmaster" ;


/* exported variables */


/* exported subroutines */

int mailalias_open(MA *op,cc *pr,cc *pname,int of,m_t om,int ot) noex {
	int		rs ;
	int		f_create = false ;
	if ((rs = mailalias_ctor(op,pr,pname)) >= 0) {
	    static cint		rsv = var ;
	    if ((rs = rsv) >= 0) {
	        op->fd = -1 ;
	        op->oflags = of ;
	        op->operm = om ;
	        op->otype = ot ;
	        op->fl.ocreate = ((of & O_CREAT) == O_CREAT) ;
		/* XXX GCC conversion complaint */
	        op->fl.owrite = op->fl.owrite || ((of & O_WRONLY) == O_WRONLY) ;
		/* XXX GCC conversion complaint */
	        op->fl.owrite = op->fl.owrite || ((of & O_RDWR) == O_RDWR) ;
	        op->aprofile = defprofile ;
	        if ((rs = ids_load(op->idp)) >= 0) {
	            if (cchar *cp ; (rs = libmem.strw(pr,-1,&cp)) >= 0) {
	                op->pr = cp ;
	                if ((rs = libmem.strw(pname,-1,&cp)) >= 0) {
	                    op->apname = cp ;
		            if ((rs = pagesize) >= 0) {
			        op->pagesize = rs ;
			        rs = mailalias_opener(op) ;
		            } /* end if (pagesize) */
	                    if (rs < 0) {
		    		void *vp = voidp(op->apname) ;
	                        libmem.free(vp) ;
	                        op->apname = nullptr ;
	                    } /* end if (error) */
	                } /* end if (ma-a) */
	                if (rs < 0) {
		    	    void *vp = voidp(op->pr) ;
	                    libmem.free(vp) ;
	                    op->pr = nullptr ;
	                } /* end if (error) */
	            } /* end if (m-a) */
	            if (rs < 0) {
	                ids_release(op->idp) ;
	            }
	        } /* end if (ids_load) */
	    } /* end if (vars) */
	    if (rs < 0) {
		mailalias_dtor(op) ;
	    }
	} /* end if (mailalias_ctor) */
	return (rs >= 0) ? f_create : rs ;
}
/* end subroutine (mailalias_open) */

int mailalias_close(MA *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = mailalias_magic(op)) >= 0) {
	    {
	        rs1 = mailalias_dbclose(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->afp) {
	        rs1 = vecstr_finish(op->afp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->dbfname) {
		void *vp = voidp(op->dbfname) ;
	        rs1 = libmem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->dbfname = nullptr ;
	    }
	    if (op->apname) {
		void *vp = voidp(op->apname) ;
	        rs1 = libmem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->apname = nullptr ;
	    }
	    if (op->pr) {
		void *vp = voidp(op->pr) ;
	        rs1 = libmem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->pr = nullptr ;
	    }
	    if (op->idp) {
	        rs1 = ids_release(op->idp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = mailalias_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mailalias_close) */

int mailalias_audit(MA *op) noex {
	int		rs ;
	if ((rs = mailalias_magic(op)) >= 0) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mailalias_audit) */

int mailalias_count(MA *op) noex {
	int		rs ;
	if ((rs = mailalias_magic(op)) >= 0) {
	    rs = (op->rtlen - 1) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mailalias_count) */

int mailalias_indcount(MA *op) noex {
	int		rs ;
	if ((rs = mailalias_magic(op)) >= 0) {
	    rs = op->rilen ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mailalias_indcount) */

int mailalias_curbegin(MA *op,MA_CUR *curp) noex {
	int		rs ;
	if ((rs = mailalias_magic(op,curp)) >= 0) {
	    op->cursors += 1 ;
	    op->fl.cursorlockbroken = false ;
	    op->fl.cursoracc = false ;
	    curp->i = -1 ;
	    curp->magic = MAILALIAS_MAGIC ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mailalias_curbegin) */

int mailalias_curend(MA *op,MA_CUR *curp) noex {
	int		rs ;
	if ((rs = mailalias_magic(op,curp)) >= 0) {
	    rs = SR_NOTOPEN ;
	    if (curp->magic == MAILALIAS_MAGIC) {
		rs = SR_OK ;
	        if (op->fl.cursoracc) {
	            op->ti_access = time(nullptr) ;
	        }
	        if (op->cursors > 0) {
	            op->cursors -= 1 ;
	        }
	        curp->i = -1 ;
	        curp->magic = 0 ;
	    } /* end if (magic) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mailalias_curend) */

int mailalias_enum(MA *op,MA_CUR *curp,char *kbuf,int klen,
		char *vbuf,int vlen) noex {
	int		rs ;
	int		rs1 ;
	int		vl = 0 ;
	if ((rs = mailalias_magic(op,curp,kbuf,vbuf)) >= 0) {
	    rs = SR_NOTOPEN ;
	    if (curp->magic == MAILALIAS_MAGIC) {
		rs = SR_INVALID ;
	        if (op->cursors) {
	            time_t	dt = 0 ;
		    int		ri = (curp->i < 1) ? 1 : (curp->i + 1) ;
		    /* capture a hold on the file */
	            if ((rs = mailalias_enterbegin(op,dt)) >= 0) {
	                if (ri < op->rtlen) {
	                    int		ai = op->rectab[ri][0] ;
	                    int		vi = op->rectab[ri][1] ;
	                    if ((ai < op->sklen) && (vi < op->svlen)) {
	                        int	cl ;
	                        char	*bp ;
	                        if (kbuf != nullptr) {
	                            cl = var.mailaliaslen ;
	                	    if (klen >= 0) {
	                    	        cl = min(klen,var.mailaliaslen) ;
				    }
				    bp = strwcpy(kbuf,(op->skey + ai),cl) ;
	                	    vl = intconv(bp - kbuf) ;
	                         } else {
	                             vl = lenstr(op->skey + ai) ;
	                         }
	                         if (vbuf != nullptr) {
	                             cl = var.mailaddrlen ;
	                             if (vlen >= 0) {
	                                 cl = min(vlen,var.mailaddrlen) ;
	                             }
	                             strwcpy(vbuf,(op->sval + vi),cl) ;
	                         } /* end if (value buffer present) */
		                 /* update the cursor */
	                         curp->i = ri ;
	                    } else {
	                        rs = SR_BADFMT ;
	                    }
	                } else {
	                    rs = SR_NOTFOUND ;
	                }
			rs1 = mailalias_enterend(op,dt) ;
			if (rs >= 0) rs = rs1 ;
		    } /* end if (mailalias-enter) */
	        } /* end if (valid) */
	    } /* end if (cursor-open) */
	} /* end if (magic) */
	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (mailalias_enum) */

int mailalias_fetch(MA *op,int opts,cchar *aname,MA_CUR *curp,
		char *vbuf,int vlen) noex {
	int		rs ;
	int		rs1 ;
	int		vl = 0 ;
	(void) opts ;
	if ((rs = mailalias_magic(op,aname,curp,vbuf)) >= 0) {
	    rs = SR_NOTOPEN ;
	    if (curp->magic == MAILALIAS_MAGIC) {
		rs = SR_INVALID ;
	        if (op->cursors) {
	            time_t	dt = 0 ;
                    if ((rs = mailalias_enterbegin(op,dt)) >= 0) {
			cint	keylen = var.mailaliaslen ;
                        cint        ns = MAILALIAS_NSHIFT ;
	                uint	khash ;
                        int         vi, hi, ri ;
                        int         cl, hl, n ;
                        cchar       *hp ;
                        cchar       *cp ;
                        /* continue with regular fetch activities */
                        op->fl.cursoracc = true ; /* no hurt if no cursor */
                        n = op->rilen ;
                        /* OK, we go from here */
                        if (curp->i < 0) {
                            char    keybuf[keylen + 1] ;
			    if (strcasecmp(aname,pm) == 0) {
                                hp = keybuf ;
                                cp = strwcpylc(keybuf,aname,keylen) ;
                                hl = intconv(cp - keybuf) ;
                            } else {
                                hp = aname ;
                                hl = lenstr(aname) ;
                            } /* end if */
                            khash = hash_elf(hp,hl) ;
                            hi = hashindex(khash,op->rilen) ;
                            /* start searching! */
                            if (op->ropts & MAILALIAS_OSEC) {
                                int         c = 0 ;
                                bool        f{} ;
                                while ((ri = (op->indtab)[hi][0]) != 0) {
                                    f = mailalias_keymatch(op,ri,aname) ;
                                    if (f) break ;
                                    op->collisions += 1 ;
                                    if (op->ropts & MAILALIAS_ORANDLC) {
                                        khash = randlc(khash + c) ;
                                    } else {
                                        khash = rotr(khash,ns) ;
                                    }
                                    hi = hashindex(khash,n) ;
                                    c += 1 ;
                                } /* end while */
                                if (ri == 0) {
                                    rs = SR_NOTFOUND ;
                                }
                            } /* end if (secondary hasing) */
                        } else {
                            /* get the next record index (if there is one) */
                            hi = curp->i ;
                            if (hi != 0) {
                                ri = (op->indtab)[hi][0] ;
                                if (ri != 0) {
                                    hi = (op->indtab)[hi][1] ;
                                    if (hi != 0) {
                                        ri = (op->indtab)[hi][0] ;
                                    } else {
                                        rs = SR_NOTFOUND ;
                                    }
                                } else {
                                    rs = SR_NOTFOUND ;
                                }
                            } else {
                                rs = SR_NOTFOUND ;
                            }
                        } /* end if (preparation) */
                        if (rs >= 0) {
			    bool	f ;
                            while ((ri = (op->indtab)[hi][0]) != 0) {
                                f = mailalias_keymatch(op,ri,aname) ;
                                if (f) break ;
                                hi = (op->indtab)[hi][1] ;
                                if (hi == 0) break ;
                                op->collisions += 1 ;
                            } /* end while */
                            if ((ri == 0) || (hi == 0)) {
                                rs = SR_NOTFOUND ;
                            }
                        } /* end if (following the existing chain) */
                        /* if successful, retrieve value */
                        if (rs >= 0) {
                            vi = op->rectab[ri][1] ;
                            if (vbuf != nullptr) {
                                cl = min(vlen,var.mailaliaslen) ;
                                cp = strwcpy(vbuf,(op->sval + vi),cl) ;
                                vl = intconv(cp - vbuf) ;
                            } else {
                                vl = lenstr(op->sval + vi) ;
                            }
                            /* update cursor */
                            curp->i = hi ;
                        } /* end if (got one) */
                        rs1 = mailalias_enterend(op,dt) ;
                        if (rs >= 0) rs = rs1 ;
                    } /* end if (mailalias-enter) */
		} /* end if (valid) */
	    } /* end if (cursor-magic) */
	} /* end if (magic) */
	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (mailalias_fetch) */

int mailalias_getinfo(MA *op,MA_INFO *rp) noex {
	int		rs ;
	if ((rs = mailalias_magic(op)) >= 0) {
	    if (rp) {
	        rp->collisions = op->collisions ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mailalias_getinfo) */

int mailalias_check(MA *op,time_t dt) noex {
	int		rs ;
	int		f = false ;
	if ((rs = mailalias_magic(op)) >= 0) {
	    if ((! op->fl.held) && op->mapdata) {
	        if (dt == 0) dt = time(nullptr) ;
	        if ((dt - op->ti_check) >= TO_CHECK) {
	            op->ti_check = dt ;
	            if ((rs = mailalias_checkchanged(op,dt)) > 0) {
	                f = true ;
	                rs = mailalias_fileclose(op) ;
	            } else if (rs == 0) {
	                if ((rs = mailalias_checkold(op,dt)) > 0) {
	                    f = true ;
	                    rs = mailalias_fileclose(op) ;
	                }
	            }
	        }
	    }
	} /* end if (magic) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (mailalias_check) */


/* private subroutines */

static int mailalias_opener(MA *op) noex {
	custime		dt = time(nullptr) ;
	int		rs ;
	int		rs1 ;
	int		f_create = false ;
	if ((rs = maxpathlen) >= 0) {
	    cint	maxpath = rs ;
	    cint	sz = ((rs + 1) * 2) ;
	    if (char *a ; (rs = libmem.mall(sz,&a)) >= 0) {
		cchar	*pname = op->apname ;
	        cchar	*fe = MAILALIAS_FSUF ;
	        char	endstr[2] ;
		char	*fname = a ;
	        endstr[0] = (ENDIAN) ? '1' : '0' ;
	        endstr[1] = '\0' ;
	        if ((rs = mkfnamesuf2(fname,pname,fe,endstr)) >= 0) {
		    cchar	*pr = op->pr ;
	            cchar	*db = MAILALIAS_DNAME ;
	            char	*dbfname = (a + (maxpath+1)) ;
	            if ((rs = mkpath3(dbfname,pr,db,fname)) >= 0) {
			cchar	*cp ;
	                if ((rs = libmem.strw(dbfname,-1,&cp)) >= 0) {
	                    vecstr	*alp = op->afp ;
	                    cint	vo = VECSTR_OCOMPACT ;
	                    op->dbfname = cp ;
	                    if ((rs = vecstr_start(alp,5,vo)) >= 0) {
	                        if ((rs = mailalias_dbopen(op,dt)) >= 0) {
	                            f_create = (rs > 0) ;
	                            op->magic = MAILALIAS_MAGIC ;
	                        }
	                        if (rs < 0) {
	                            vecstr_finish(op->afp) ;
				}
	                    } /* end if (vecstr_start) */
	                    if (rs < 0) {
		    		void *vp = voidp(op->dbfname) ;
	                        libmem.free(vp) ;
	                        op->dbfname = nullptr ;
	                    }
	                } /* end if (m-a) */
	            } /* end if (mkpath) */
	        } /* end if (mkfnamesuf) */
		rs1 = libmem.free(a) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (getbufsize) */
	return (rs >= 0) ? f_create : rs ;
}
/* end subroutine (mailalias_opener) */

static int mailalias_checkchanged(MA *op,time_t dt) noex {
	int		rs = SR_OK ;
	int		f = false ;
	if ((dt - op->ti_filecheck) > TO_FILECHECK) {
	    op->ti_filecheck = dt ;
	    if (USTAT sb ; (rs = u_stat(op->dbfname,&sb)) >= 0) {
	        rs = mailalias_filechanged(op,&sb) ;
	        f = rs ;
	    } else if (isNotPresent(rs)) {
	        rs = SR_OK ;
	    }
	}
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (mailalias_checkchanged) */

static int mailalias_checkold(MA *op,time_t dt) noex {
	int		rs = SR_OK ;
	int		f = false ;
	if (op->fl.ocreate && op->fl.owrite) {
	    if ((dt - op->ti_fileold) > TO_FILEOLD) {
	        op->ti_fileold = dt ;
	        rs = mailalias_fileold(op,dt) ;
	        f = rs ;
	    }
	} /* end if (within check interval) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (mailalias_checkold) */

/* read the file header and check it out */
static int mailalias_hdrload(MA *op) noex {
	cint		msize = MAILALIAS_MAGICSIZE ;
	int		rs = SR_OK ;
	cchar		*cp = charp(op->mapdata) ;
	if (hasValidMagic(cp,msize,MAILALIAS_MAGICSTR)) {
	    cp += msize ;
	    if (cp[0] == MAILALIAS_FILEVERSION) {
	        if (cp[1] == ENDIAN) {
	            op->ropts = mkchar(cp[2]) ;
	            rs = mailalias_hdrloader(op) ;
	        } else {
	            rs = SR_NOTSUP ;
	        }
	    } else {
	        rs = SR_NOTSUP ;
	    }
	} else {
	    rs = SR_BADFMT ;
	}
	return rs ;
}
/* end subroutine (mailalias_hdrload) */

static int mailalias_hdrloader(MA *op) noex {
	uint		*table = (uint *) (op->mapdata + MAILALIAS_IDLEN) ;
	int		rs = SR_OK ;
#ifdef	COMMENT
	{
	op->i.wtime = table[mailaliashdr_wtime] ;
	op->i.wcount = table[mailaliashdr_wcount] ;
	}
#endif
	{
	op->keytab = (int *) (op->mapdata + table[mailaliashdr_key]) ;
	op->rectab = (int (*)[2]) (op->mapdata + table[mailaliashdr_rec]) ;
	op->indtab = (int (*)[2]) (op->mapdata + table[mailaliashdr_ind]) ;
	op->skey = charp(op->mapdata + table[mailaliashdr_skey]) ;
	op->sval = charp(op->mapdata + table[mailaliashdr_sval]) ;
	}
	{
	op->ktlen = table[mailaliashdr_keylen] ;
	op->rtlen = table[mailaliashdr_reclen] ;
	op->rilen = table[mailaliashdr_indlen] ;
	op->sklen = table[mailaliashdr_skeysize] ;
	op->svlen = table[mailaliashdr_svalsize] ;
	}
	return rs ;
}
/* end subroutine (mailalias_hdrloader) */

static int mailalias_fileopen(MA *op,time_t dt) noex {
	int		rs = SR_OK ;
	if (op->fd < 0) {
	    int		oflags = op->oflags ;
	    oflags &= (~ O_CREAT) ;
	    oflags &= (~ O_RDWR) ;
	    oflags &= (~ O_WRONLY) ;
	    oflags |= O_RDONLY ;
	    if ((rs = u_open(op->dbfname,oflags,op->operm)) >= 0) {
	        op->fd = rs ;
	        op->ti_open = dt ;
	        rs = uc_closeonexec(op->fd,true) ;
		if (rs < 0) {
		    u_close(op->fd) ;
		    op->fd = -1 ;
		}
	    } /* end if */
	} /* end if (needed open) */
	return (rs >= 0) ? op->fd : rs ;
}
/* end subroutine (mailalias_fileopen) */

static int mailalias_fileclose(MA *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->fd >= 0) {
	    rs1 = u_close(op->fd) ;
	    if (rs >= 0) rs = rs1 ;
	    op->fd = -1 ;
	}
	return rs ;
}
/* end subroutine (mailalias_fileclose) */

static int mailalias_mapbegin(MA *op,time_t dt) noex {
	int		rs = SR_OK ;
	int		f = false ;
	if (op->mapdata == nullptr) {
	    cnullptr	np{} ;
	    size_t	ms = ceil(op->fi.fsize,op->pagesize) ;
	    cint	fd = op->fd ;
	    int		mp = PROT_READ ;
	    int		mf = MAP_SHARED ;
	    void	*md{} ;
	    if (ms == 0) ms = op->pagesize ;
	    if (dt == 0) dt = time(nullptr) ;
	    if ((rs = u_mmapbegin(np,ms,mp,mf,fd,0z,&md)) >= 0) {
	        op->mapdata = charp(md) ;
	        op->mapsize = ms ;
	        op->ti_map = dt ;
	        f = true ;
	    } /* end if (u_mmapbegin) */
	} /* end if (map needed) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (mailalias_mapbegin) */

static int mailalias_mapend(MA *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->mapdata != nullptr) {
	    csize	ms = op->mapsize ;
	    void	*md = op->mapdata ;
	    rs1 = u_mmapend(md,ms) ;
	    if (rs >= 0) rs = rs1 ;
	    op->mapdata = nullptr ;
	    op->mapsize = 0 ;
	}
	return rs ;
}
/* end subroutine (mailalias_mapend) */

static int mailalias_keymatch(MA *op,int ri,cchar *aname) noex {
	cint		ki = op->rectab[ri][0] ;
	cchar		*ks = (op->skey + ki) ;
	bool		f = true ;
	f = f && (ks[0] == aname[0]) ;
	f = f && (strcmp(ks,aname) == 0) ;
	return f ;
}
/* end subroutine (mailalias_keymatch) */

static int mailalias_dbopen(MA *op,time_t dt) noex {
	int		rs ;
	int		f_create = false ;
	if ((rs = mailalias_dbopenfile(op,dt)) >= 0) {
	    f_create = rs ;
	    if ((rs = mailalias_dbopenmake(op,dt)) >= 0) {
	        if ((rs = mailalias_isremote(op)) >= 0) {
	            if ((rs = mailalias_dbopenwait(op)) >= 0) {
	                if ((rs = mailalias_mapbegin(op,dt)) >= 0) {
	                    if ((rs = mailalias_hdrload(op)) >= 0) {
	                        op->ti_access = dt ;
	                    }
	                    if (rs < 0) {
	                        mailalias_mapend(op) ;
			    }
	                } /* end if (mailalias_mapbegin) */
	            } /* end if (mailalias_dbopenwait) */
	        } /* end if (mailalias_isremote) */
	    } /* end if (mailalias_dbopenmake) */
	    if (rs < 0) {
	        mailalias_fileclose(op) ;
	    }
	} /* end if (mailalias_dbopenfile) */
	return (rs >= 0) ? f_create : rs ;
}
/* end subroutine (mailalias_dbopen) */

static int mailalias_dbopenfile(MA *op,time_t dt) noex {
	int		rs ;
	int		of = op->oflags ;
	int		f_create = false ;
	of &= (~ O_CREAT) ;
	of &= (~ O_RDWR) ;
	of &= (~ O_WRONLY) ;
	of |= O_RDONLY ;
	if ((rs = u_open(op->dbfname,of,op->operm)) >= 0) {
	    op->fd = rs ;
	    if (ustat sb ; (rs = u_fstat(op->fd,&sb)) >= 0) {
	        op->fi.mtime = sb.st_mtime ;
	        op->fi.fsize = sb.st_size ;
	        op->fi.ino = sb.st_ino ;
	        op->fi.dev = sb.st_dev ;
	        op->ti_open = dt ;
	        if (op->fl.ocreate && op->fl.owrite) {
	            f_create = (sb.st_size == 0) ;
	            if (! f_create) {
	                if ((rs = mailalias_fileold(op,dt)) > 0) {
	                    f_create = true ;
	                }
	            } /* end if */
	            if (f_create) {
	                mailalias_fileclose(op) ;
	            }
	        } /* end if (we can possibly create the DB) */
	    } /* end if (stat) */
	    if (rs < 0) {
	        u_close(op->fd) ;
	        op->fd = -1 ;
	    }
	} else if (isNotPresent(rs)) {
	    rs = SR_OK ;
	    f_create = true ;
	    op->fl.needcreate = true ;
	} /* end if (successful file open) */
	if ((rs >= 0) && f_create) {
	    op->fl.needcreate = true ;
	}
	return (rs >= 0) ? f_create : rs ;
}
/* end subroutine (mailalias_dbopenfile) */

static int mailalias_dbopenmake(MA *op,time_t dt) noex {
	int		rs = SR_OK ;
	if (op->fl.ocreate && op->fl.owrite && op->fl.needcreate) {
	    if ((rs = mailalias_dbmake(op,dt)) >= 0) {
	        cint	of = O_RDONLY ;
	        if ((rs = u_open(op->dbfname,of,op->operm)) >= 0) {
	            op->fd = rs ;
	            if (ustat sb ; (rs = u_fstat(op->fd,&sb)) >= 0) {
	                op->fi.mtime = sb.st_mtime ;
	                op->fi.fsize = sb.st_size ;
	                op->fi.ino = sb.st_ino ;
	                op->fi.dev = sb.st_dev ;
	                op->ti_open = dt ;
	            } /* end if (u_fstat) */
	            if (rs < 0) {
	                u_close(op->fd) ;
	                op->fd = -1 ;
	            } /* end if (error) */
	        } /* end if (open) */
	    } /* end if (mailalias_dbmake) */
	} /* end if (created or re-created the file) */
	return rs ;
}
/* end subroutine (mailalias_dbopenmake) */

/* wait for the file to come in if it is not yet available */
static int mailalias_dbopenwait(MA *op) noex {
	USTAT		sb ;
	cint		to = TO_FILECOME ;
	int		rs ;
	int		msize = MAILALIAS_TOPLEN ;
	if ((rs = u_fstat(op->fd,&sb)) >= 0) {
	    int		i ; /* used afterwards */
	    op->fi.fsize = sb.st_size ;
	    for (i = 0 ; (i < to) && (sb.st_size < msize) ; i += 1) {
	        if (i > 0) sleep(10) ;
	        rs = u_fstat(op->fd,&sb) ;
	        op->fi.fsize = sb.st_size ;
	        if (rs < 0) break ;
	    } /* end while */
	    if (rs >= 0) {
	        if (i < to) {
	            op->fi.fsize = sb.st_size ;
	            op->fi.mtime = sb.st_mtime ;
	        } else {
	            rs = SR_TIMEDOUT ;
	        }
	    }
	} /* end if (u_fstat) */
	return rs ;
}
/* end subroutine (mailalias_dbopenwait) */

static int mailalias_isremote(MA *op) noex {
	int		rs ;
	if ((rs = isfsremote(op->fd)) > 0) {
	    op->fl.remote = true ;
	}
	return rs ;
}
/* end subroutine (mailalias_isremote) */

static int mailalias_dbclose(MA *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_OK ;
	    {
	        rs1 = mailalias_mapend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = mailalias_fileclose(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mailalias_dbclose) */

/* make the index file */
static int mailalias_dbmake(MA *op,time_t dt) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		cl ;
	cchar		*cp ;
	if (dt == 0) dt = time(nullptr) ;
/* get the directory of the DB file and see if it is writable to us */
	if ((cl = sfdirname(op->dbfname,-1,&cp)) > 0) {
	    if (char *dbuf ; (rs = malloc_mp(&dbuf)) >= 0) {
	        if ((rs = mkpath1w(dbuf,cp,cl)) >= 0) {
	            if (ustat sb ; (rs = u_stat(dbuf,&sb)) >= 0) {
			if (S_ISDIR(sb.st_mode)) {
	                    rs = permid(op->idp,&sb,W_OK) ;
			} else {
			    rs = SR_NOTDIR ;
	                }
	            } else if (isNotPresent(rs)) {
	                cmode	dm = MAILALIAS_DIRMODE ;
	                rs = mkdirs(dbuf,dm) ;
	            }
	            if (rs >= 0) {
	                rs = mailalias_dbmaker(op,dt,dbuf) ;
	            }
	        } /* end if (mkpathxw) */
		rs1 = malloc_free(dbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} else {
	    rs = SR_ISDIR ;
	} /* end if */
	return rs ;
}
/* end subroutine (mailalias_dbmake) */

static int mailalias_dbmaker(MA *op,time_t dt,cchar *dname) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = maxpathlen) >= 0) {
	    cint	maxpath = rs ;
	    cint	sz = ((rs + 1) * 3) ;
	    if (char *a ; (rs = libmem.mall(sz,&a)) >= 0) {
		int	ai = 0 ;
	        cint	clen = rs ;
	        cchar	*pat = "dbmkXXXXXX" ;
	        cchar	*suf = MAILALIAS_FSUF ;
	        cchar	*end = ENDIANSTR ;
	        char	*cbuf = (a + (ai++ * (maxpath + 1))) ;
	        if ((rs = sncpy(cbuf,clen,pat,".",suf,end,"n")) >= 0) {
	            char	*tbuf = (a + (ai++ * (maxpath + 1))) ;
	            if ((rs = mkpath2(tbuf,dname,cbuf)) >= 0) {
	                int	of = (O_WRONLY | O_CREAT | O_EXCL) ;
	                int	fd = -1 ;
		        cmode	om = 0664 ;
	                char	*nfname = (a + (ai++ * (maxpath + 1))) ;
	                if ((rs = opentmpfile(tbuf,of,om,nfname)) >= 0) {
	                    fd = rs ;
	                } else if (rs == SR_EXIST) {
	                    USTAT	sb ;
		            coff	fsz = MAILALIAS_TOPLEN ;
	                    int		i ; /* used-afterwards */
	                    for (i = 0 ; i < TO_FILECOME ; i += 1) {
	                        msleep(1) ;
	                        rs1 = u_stat(op->dbfname,&sb) ;
	                        if ((rs1 >= 0) && (sb.st_size >= fsz)) break ;
	                    } /* end for */
	                    rs = (i < TO_FILECOME) ? SR_OK : SR_TIMEDOUT ;
	                    if (rs == SR_TIMEDOUT) {
	                        of = (O_WRONLY | O_CREAT) ;
	                        rs = u_open(nfname,of,om) ;
	                        fd = rs ;
	                        if (rs == SR_ACCESS) {
	                            u_unlink(nfname) ;
	                            rs = u_open(nfname,of,om) ;
	                            fd = rs ;
	                        }
	                    } /* end if (attempt to break the lock) */
	                } /* end if (waiting for file to come in) */
	                if ((rs >= 0) && (fd >= 0)) {
		            cint	n = NREC_GUESS ;
	                    if ((rs = mailalias_dbmaking(op,fd,dt,n)) >= 0) {
	                        u_close(fd) ;
	                        fd = -1 ;
	                        if ((rs = u_rename(nfname,op->dbfname)) >= 0) {
	                            nfname[0] = '\0' ;
	                        }
	                    }
	                    if (rs < 0) {
	                        if (fd >= 0) {
	                            u_close(fd) ;
	                            fd = -1 ;
	                        }
	                        nfname[0] = '\0' ;
	                    }
	                } else {
	                    nfname[0] = '\0' ;
	                }
	                if (nfname[0] != '\0') {
	                    u_unlink(nfname) ;
	                }
	            } /* end if (mkpath2) */
	        } /* end if (sncpy5) */
		rs1 = libmem.free(a) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (maxpathlen) */
	return rs ;
}
/* end subroutine (mailalias_dbmaker) */

static int mailalias_dbmaking(MA *op,int fd,time_t dt,int n) noex {
	cint		sz = szof(record) ;
	int		rs ;
	int		rs1 ;
	vecobj		recs ;
	if ((rs = vecobj_start(&recs,sz,n,0)) >= 0) {
	    strtab	skeys ;
	    if ((rs = strtab_start(&skeys,n)) >= 0) {
	        strtab	svals ;
	        if ((rs = strtab_start(&svals,(n * 2))) >= 0) {
		    cint	nshift = MAILALIAS_NSHIFT ;
		    cint	mags = MAILALIAS_MAGICSIZE ;
		    cchar	*magp = MAILALIAS_MAGICSTR ;
	            if ((rs = mailalias_aprofile(op,dt)) >= 0) {
	                dbmake	data(&recs,&skeys,&svals,fd) ;
			data.rectab = op->rectab ;
			data.ropts = op->ropts ;
			data.setmagic(magp,mags) ;
			data.setnshift(nshift) ;
			data.setversion(MAILALIAS_FILEVERSION) ;
	                if (char *tbuf ; (rs = malloc_mp(&tbuf)) >= 0) {
	                    for (int i = 0 ; op->aprofile[i] ; i += 1) {
	                        cchar	*cp = charp(op->aprofile[i]) ;
	                        if (*cp != '/') {
	                            cchar	*ap = op->aprofile[i] ;
	                            cp = tbuf ;
	                            rs = mkpath2(tbuf,op->pr,ap) ;
	                        }
	                        if (rs >= 0) {
	                            rs = data.wrfileproc(cp) ;
	                        }
	                        if (rs < 0) break ;
	                    } /* end for */
			    rs1 = malloc_free(tbuf) ;
			    if (rs >= 0) rs = rs1 ;
			} /* end if (m-a-f) */
			/* OK, write out the file */
	                if (rs >= 0) {
	                    rs = data.wrfile(dt) ;
	                } /* end if (ok) */
	            } /* end if (mailalias_aprofile) */
	            rs1 = strtab_finish(&svals) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (strtab_start) */
	        rs1 = strtab_finish(&skeys) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (strtab_start) */
	    rs1 = vecobj_finish(&recs) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (vecobj-recs) */
	return rs ;
}
/* end subroutine (mailalias_dbmaking) */

static int mailalias_filechanged(MA *op,USTAT *sbp) noex {
	csize		fsz = size_t(sbp->st_size) ;
	int		f = false ;
	f = f || (op->fi.fsize != fsz) ;
	f = f || (op->fi.mtime != sbp->st_mtime) ;
	f = f || (op->fi.ino != sbp->st_ino) ;
	f = f || (op->fi.dev != sbp->st_dev) ;
	return f ;
}
/* end subroutine (mailalias_filechanged) */

/* return true if the underlying file(s) are "old" */
static int mailalias_fileold(MA *op,time_t dt) noex {
	int		rs ;
	int		rs1 ;
	int		f = false ;
	if ((rs = mailalias_aprofile(op,dt)) >= 0) {
	    if (char *tbuf ; (rs = malloc_mp(&tbuf)) >= 0) {
	        USTAT	sb ;
	        for (int i = 0 ; op->aprofile[i] ; i += 1) {
	            cchar *cp = charp(op->aprofile[i]) ;
	            if (*cp != '/') {
	                cp = tbuf ;
	                mkpath(tbuf,op->pr,op->aprofile[i]) ;
	            }
	            rs1 = u_stat(cp,&sb) ;
	            f = ((rs1 >= 0) && (sb.st_mtime > op->fi.mtime)) ;
		    if (f) break ;
	        } /* end for */
	        rs1 = malloc_free(tbuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (mailalias_aprofile) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (mailalias_fileold) */

/* get a-profiles */
static int mailalias_aprofile(MA *op,time_t dt) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if ((dt - op->ti_aprofile) >= TO_APROFILE) {
	    if (char *tbuf ; (rs = malloc_mp(&tbuf)) >= 0) {
	        USTAT	sb ;
	        cchar	*profp = nullptr ;
		cint	tlen = rs ;
	        op->ti_aprofile = dt ;
        	/* get the mailalias profile (AP) table */
	        rs1 = SR_NOENT ;
	        for (int i = 0 ; aptabsched[i] ; i += 1) {
	            profp = charp(aptabsched[i]) ;
	            if (profp[0] != '/') {
	                rs = mkpath2(tbuf,op->pr,profp) ;
	                profp = tbuf ;
	            }
	            rs1 = u_stat(profp,&sb) ;
	            if ((rs1 >= 0) && S_ISDIR(sb.st_mode)) {
	                rs1 = SR_ISDIR ;
	            }
	            if (rs1 >= 0) {
	                rs1 = permid(op->idp,&sb,R_OK) ;
	            }
	            if (rs1 >= 0) break ;
	        } /* end for */
	        if ((rs >= 0) && profp) {
	            rs = mailalias_aprofiler(op,tbuf,tlen,profp) ;
	            if_constexpr (f_defprofile) {
	                if (op->aprofile[0] == nullptr) {
	                    op->aprofile = defprofile ;
	                }
	            } /* end if_constexpr (f_defprofile) */
	        } else {
	            op->aprofile = defprofile ;
	        }
		rs1 = malloc_free(tbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (needed due to time-out) */
	return rs ;
}
/* end subroutine (mailalias_aprofile) */

static int mailalias_aprofiler(MA *op,char *tbuf,int tlen,cchar *profp) noex {
	int		rs ;
	int		rs1 ;
	if (kvsfile aptab ; (rs = kvsfile_open(&aptab,0,profp)) >= 0) {
		vecstr		*afp = op->afp ;
		cchar		*fp{} ;
	        for (int i = 0 ; afp->get(i,&fp) >= 0 ; i += 1) {
	            if (fp) {
	                afp->del(i--) ;
	            }
	        } /* end for */
	        kvsfile_cur cur ; 
		if ((rs = kvsfile_curbegin(&aptab,&cur)) >= 0) {
	            while (rs >= 0) {
	                rs1 = kvsfile_fetch(&aptab,op->apname,&cur,tbuf,tlen) ;
	                if (rs1 < 0) break ;
	                rs = afp->add(tbuf,rs1) ;
	            } /* end while */
	            rs1 = kvsfile_curend(&aptab,&cur) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (kvsfile-cur) */
	        if (rs >= 0) {
	            rs = afp->getvec(&op->aprofile) ;
	        }
	    rs1 = kvsfile_close(&aptab) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (opened key-values table) */
	return rs ;
}
/* end subroutine (mailalias_aprofiler) */

/* acquire access to the file (mapped memory) */
static int mailalias_enterbegin(MA *op,time_t dt) noex {
	int		rs ;
	int		f = false ;
	if ((rs = mailalias_mapcheck(op,dt)) > 0) {
	    f = true ;
	}
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (mailalias_enterbegin) */

/* release our hold on the filemap */
static int mailalias_enterend(MA *op,time_t dt) noex {
	int		rs = SR_OK ;
	(void) dt ;
	if (op->fl.held) {
	    op->fl.held = false ;
	}
	return rs ;
}
/* end subroutine (mailalias_enterend) */

static int mailalias_mapcheck(MA *op,time_t dt) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		f = false ;
	if (op->mapdata == nullptr) {
	    if ((rs = mailalias_fileopen(op,dt)) >= 0) {
	        if ((rs = mailalias_mapbegin(op,dt)) > 0) {
	            f = true ;
	            rs = mailalias_hdrload(op) ;
	            if (rs < 0) {
	                mailalias_mapend(op) ;
	            }
	        } /* end if (mailalias_mapbegin) */
		rs1 = mailalias_fileclose(op) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (mailalias_fileopen) */
	}
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (mailalias_mapcheck) */

vars::operator int () noex {
    	int		rs ;
	if ((rs = getbufsize(getbufsize_mailaddr)) >= 0) {
	    mailaddrlen = rs ;
	    mailaliaslen = mailvalue.mailaliaslen ;
	}
	return rs ;
}
/* end method (vars::operator) */


