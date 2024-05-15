/* mailalias SUPPORT */
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

	This module is used to manage a MAILALIAS object.

	We always check if your alias key is the string "Postmater"
	(in a case-insensitive way) and if it is, we convert it to
	LOWER case before doing the 'mailalias_fetch()'.  Also,
	unlike other fetching (with other DBs), there is no concept
	of "prefix" match fetching!

	We use TWO string tables in this DB (unlike some other
	similar DBs). We use one string table for keys and another
	for values. I sort of did this for fun (so far). This is
	actually potentially less space-efficient than using just
	one string table since strings that are both a key and a
	value are not combined in one table but rather appear
	separately in each of the two tables. However, the up side
	is that:

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
#include	<strings.h>		/* |strncasecmp(3c)| */
#include	<usystem.h>
#include	<endian.h>
#include	<vecobj.h>
#include	<vecstr.h>
#include	<bfile.h>
#include	<opentmp.h>
#include	<field.h>
#include	<terminit.hh>
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
#include	<xperm.h>
#include	<nextpowtwo.h>
#include	<randlc.h>
#include	<matxstr.h>
#include	<char.h>
#include	<mkchar.h>
#include	<hasx.h>
#include	<isfiledesc.h>
#include	<isnot.h>
#include	<iserror.h>
#include	<ischarx.h>
#include	<localmisc.h>		/* |MAILADDRLEN| */

#include	"mailalias.h"


/* local defines */

#define	MAILALIAS_DNAME		"var/mailalias"
#define	MAILALIAS_FE		"mac"		/* Mail Alias Cache */
#define	MAILALIAS_DIRMODE	0775

#define	MAILALIAS_IDLEN		(MAILALIAS_FILEMAGICSIZE + 4)
#define	MAILALIAS_HEADLEN	(header_overlast * sizeof(int))
#define	MAILALIAS_TOPLEN	(MAILALIAS_IDLEN + MAILALIAS_HEADLEN)

#define	MAILALIAS_IDOFF		0
#define	MAILALIAS_HEADOFF	MAILALIAS_IDLEN
#define	MAILALIAS_BUFOFF	(MAILALIAS_HEADOFF + MAILALIAS_HEADLEN)

#define	MA			MAILALIAS
#define	MA_CUR			MAILALIAS_CUR
#define	MA_INFO			MAILALIAS_INFO

#define	DBMAKE			class dbmake

#define	MODP2(v,n)	((v) & ((n) - 1))

#define	TO_APROFILE	(1 * 60)
#define	TO_FILECOME	15		/* timeout for file to "come in" */
#define	TO_LOCK		(5 * 60)
#define	TO_OPEN		(60 * 60)
#define	TO_ACCESS	(1 * 60)
#define	TO_CHECK	5		/* minimum check interval */
#define	TO_FILECHECK	5		/* DB file check */
#define	TO_FILEOLD	10		/* backing-store check */

#ifndef	LINEBUFLEN
#define	LINEBUFLEN	MAX((3 * 1024),2048)
#endif

#ifndef	ALIASNAMELEN
#define	ALIASNAMELEN	64
#endif

#define	NREC_GUESS	100		/* guess of number of records */
#define	NSHIFT		6

#ifndef	CF_DEFPROFILE
#define	CF_DEFPROFILE	0		/* always use default MA-profile */
#endif


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */

#ifndef	TYPEDEF_MT
#define	TYPEDEF_MT
typedef mode_t		m_t ;
#endif

typedef int (*rt_t)[2] ;


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    class dbmake {
    public:
	vecobj		*rlp ;
	strtab		*klp ;
	strtab		*vlp ;
	int		(*rectab)[2] ;
	uint		nrecs ;
	uint		nkeys ;
	int		fd ;
	int		ikey{} ;
	int		count{} ;
	int		ktlen ;
	int		ktsize ;
	int		reclen ;
	int		recsize ;
	int		ropts ;
	int		rilen ;
	int		risize ;
	int		sksize ;
	int		svsize ;
	int		f_havekey ;
	dbmake(vecobj *arlp,strtab *aklp,strtab *avlp,int afd) noex {
	    rlp = arlp ;
	    klp = aklp ;
	    vlp = avlp ;
	    fd = afd ;
	    f_havekey = false ;
	} ;
	int procline(cchar *,int) noex ;
	int wrfilekeytab() noex ;
	int wrfilerec() noex ;
	int wrfilekeys() noex ;
	int wrfilevals() noex ;
	int mkind(vecobj *,cc *,rt_t,int) noex ;
    } ; /* end class (dbmake) */
    class record {
    public:
	uint		key ;
	uint		val ;
	record() noex : key(0), val(0) { } ;
	record(uint k,uint v) noex : key(k), val(v) { } ;
    } ; /* end class (record) */
    typedef record *	recordp ;
}


/* forward references */

template<typename ... Args>
static int mailalias_ctor(mailalias *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = SR_OK ;
	    memclear(op) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mailalias_ctor) */

static int mailalias_dtor(mailalias *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
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
static int	mailalias_procfile(MA *,DBMAKE *,cchar *) noex ;
static int	mailalias_wrfile(MA *,DBMAKE *, time_t) noex ;
static int	mailalias_wrfiler(MA *,DBMAKE *, time_t) noex ;
static int	mailalias_fileold(MA *,time_t) noex ;
static int	mailalias_aprofile(MA *,time_t) noex ;

static int	mailalias_checkchanged(MA *,time_t) noex ;
static int	mailalias_checkold(MA *,time_t) noex ;

static int	mailalias_mapcheck(MA *,time_t) noex ;

static int	hashindex(uint,int) noex ;


/* local variables */

enum headers {
	header_wtime,
	header_wcount,
	header_key,
	header_keylen,
	header_rec,
	header_reclen,
	header_ind,
	header_indlen,
	header_skey,
	header_skeysize,
	header_sval,
	header_svalsize,
	header_overlast
} ;

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

/* all white space plus colon (':') */
constexpr terminit	keys("\b\t\n\f\v :") ;

/* all white space plus comma (',') */
constexpr terminit	vals("\b\t\n\f\v ,") ;

constexpr bool		f_defprofile = CF_DEFPROFILE ;


/* exported variables */


/* exported subroutines */

int mailalias_open(MA *op,cc *pr,cc *pname,int of,m_t om,int ot) noex {
	const time_t	dt = time(nullptr) ;
	int		rs ;
	int		f_create = false ;
	if ((rs = mailalias_ctor(op,pr,pname)) >= 0) {
	op->fd = -1 ;
	op->oflags = of ;
	op->operm = om ;
	op->otype = ot ;
	op->f.ocreate = ((of & O_CREAT) == O_CREAT) ;
	op->f.owrite |= ((of & O_WRONLY) == O_WRONLY) ;
	op->f.owrite |= ((of & O_RDWR) == O_RDWR) ;
	op->aprofile = defprofile ;
	op->pagesize = getpagesize() ;
	if ((rs = ids_load(&op->id)) >= 0) {
	    cchar	*cp ;
	    if ((rs = uc_mallocstrw(pr,-1,&cp)) >= 0) {
	        op->pr = cp ;
	        if ((rs = uc_mallocstrw(pname,-1,&cp)) >= 0) {
	            cchar	*fe = MAILALIAS_FE ;
	            char	endian[2] ;
	            char	fname[MAXPATHLEN + 1] ;
	            op->apname = cp ;
	            endian[0] = (ENDIAN) ? '1' : '0' ;
	            endian[1] = '\0' ;
	            if ((rs = mkfnamesuf2(fname,pname,fe,endian)) >= 0) {
	                cchar	*db = MAILALIAS_DNAME ;
	                char		dbfname[MAXPATHLEN + 1] ;
	                if ((rs = mkpath3(dbfname,pr,db,fname)) >= 0) {
	                    if ((rs = uc_mallocstrw(dbfname,-1,&cp)) >= 0) {
	                        vecstr	*alp = &op->apfiles ;
	                        cint	vo = VECSTR_OCOMPACT ;
	                        op->dbfname = cp ;
	                        if ((rs = vecstr_start(alp,5,vo)) >= 0) {
	                            if ((rs = mailalias_dbopen(op,dt)) >= 0) {
	                                f_create = (rs > 0) ;
	                                op->magic = MAILALIAS_MAGIC ;
	                            }
	                            if (rs < 0) {
	                                vecstr_finish(&op->apfiles) ;
				    }
	                        } /* end if (vecstr_start) */
	                        if (rs < 0) {
	                            uc_free(op->dbfname) ;
	                            op->dbfname = nullptr ;
	                        }
	                    } /* end if (m-a) */
	                } /* end if (mkpath) */
	            } /* end if (mkfnamesuf) */
	            if (rs < 0) {
	                uc_free(op->apname) ;
	                op->apname = nullptr ;
	            }
	        } /* end if (ma-a) */
	        if (rs < 0) {
	            uc_free(op->pr) ;
	            op->pr = nullptr ;
	        }
	    } /* end if (m-a) */
	    if (rs < 0) {
	        ids_release(&op->id) ;
	    }
	} /* end if (ids_load) */

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
	    {
	        rs1 = vecstr_finish(&op->apfiles) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->dbfname) {
	        rs1 = uc_free(op->dbfname) ;
	        if (rs >= 0) rs = rs1 ;
	        op->dbfname = nullptr ;
	    }
	    if (op->apname) {
	        rs1 = uc_free(op->apname) ;
	        if (rs >= 0) rs = rs1 ;
	        op->apname = nullptr ;
	    }
	    if (op->pr) {
	        rs1 = uc_free(op->pr) ;
	        if (rs >= 0) rs = rs1 ;
	        op->pr = nullptr ;
	    }
	    {
	        rs1 = ids_release(&op->id) ;
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

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magic != MAILALIAS_MAGIC) return SR_NOTOPEN ;

	op->cursors += 1 ;
	op->f.cursorlockbroken = false ;
	op->f.cursoracc = false ;

	curp->i = -1 ;

	curp->magic = MAILALIAS_MAGIC ;
	return SR_OK ;
}
/* end subroutine (mailalias_curbegin) */

int mailalias_curend(MA *op,MA_CUR *curp) noex {
	const time_t	dt = time(nullptr) ;
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magic != MAILALIAS_MAGIC) return SR_NOTOPEN ;

	if (op->f.cursoracc)
	    op->ti_access = dt ;

	if (op->cursors > 0) op->cursors -= 1 ;
	curp->i = -1 ;

	curp->magic = 0 ;
	return rs ;
}
/* end subroutine (mailalias_curend) */

int mailalias_enum(MA *op,MA_CUR *curp,char *kbuf,int klen,
		char *vbuf,int vlen) noex {
	time_t		dt = 0 ;
	int		rs ;
	int		rs1 ;
	int		ri = 0 ;
	int		vl = 0 ;

	if (op == nullptr) return SR_FAULT ;
	if (op->magic != MAILALIAS_MAGIC) return SR_NOTOPEN ;

	if (curp == nullptr) return SR_FAULT ;

	if (curp->magic != MAILALIAS_MAGIC) return SR_NOTOPEN ;

	if (op->cursors == 0) return SR_INVALID ;

	ri = (curp->i < 1) ? 1 : (curp->i + 1) ;

/* capture a hold on the file */

	if ((rs = mailalias_enterbegin(op,dt)) >= 0) {
	    if (ri < op->rtlen) {
	        int	ai, vi ;
	        int	cl ;

	        ai = op->rectab[ri][0] ;
	        vi = op->rectab[ri][1] ;

	        if ((ai < op->sklen) && (vi < op->svlen)) {
	            char	*bp ;

	            if (kbuf != nullptr) {

	                cl = ALIASNAMELEN ;
	                if (klen >= 0) {
	                    cl = min(klen,ALIASNAMELEN) ;
			}

	                bp = strwcpy(kbuf,(op->skey + ai),cl) ;
	                vl = (bp - kbuf) ;

	            } else {
	                vl = strlen(op->skey + ai) ;
	            }

	            if (vbuf != nullptr) {
	                cl = MAILADDRLEN ;
	                if (vlen >= 0) {
	                    cl = min(vlen,MAILADDRLEN) ;
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
	} /* end if ( mailalias-enter) */

	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (mailalias_enum) */

/* ARGSUSED */
int mailalias_fetch(MA *op,int opts,cchar *aname,MA_CUR *curp,
		char *vbuf,int vlen) noex {
	MAILALIAS_CUR	cur ;
	time_t		dt = 0 ;
	uint		khash ;
	int		rs = SR_OK ;
	int		vl = 0 ;
	int		rs1 ;
	int		f_cur = false ;
	(void) opts ;

	if (op == nullptr) return SR_FAULT ;
	if (op->magic != MAILALIAS_MAGIC) return SR_NOTOPEN ;

	if (aname == nullptr) return SR_INVALID ;

	if (curp == nullptr) {
	    curp = &cur ;
	    curp->i = -1 ;
	} else {
	    f_cur = true ;
	    if (op->cursors == 0) {
	        rs = SR_INVALID ;
	    }
	} /* end if */

	if (rs >= 0) {
	    if ((rs = mailalias_enterbegin(op,dt)) >= 0) {
	        cint	ns = NSHIFT ;
	        int		vi, hi, ri ;
	        int		cl, hl, n ;
	        int		f ;
	        cchar	*hp ;
	        cchar	*cp ;

/* continue with regular fetch activities */

	        op->f.cursoracc = true ;	/* doesn't hurt if no cursor! */
	        n = op->rilen ;

/* OK, we go from here */

	        if (curp->i < 0) {
	            char	keybuf[ALIASNAMELEN + 1] ;

	            f = (strcasecmp(aname,"Postmaster") == 0) ;

	            if (f) {
	                hp = keybuf ;
	                cp = strwcpylc(keybuf,aname,ALIASNAMELEN) ;
	                hl = cp - keybuf ;
	            } else {
	                hp = aname ;
	                hl = strlen(aname) ;
	            } /* end if */

	            khash = hash_elf(hp,hl) ;

	            hi = hashindex(khash,op->rilen) ;

/* start searching! */

	            if (op->ropts & MAILALIAS_OSEC) {
			int	c = 0 ;
	                bool	f{} ;
	                while ((ri = (op->indtab)[hi][0]) != 0) {

	                    f = mailalias_keymatch(op,ri,aname) ;
	                    if (f)
	                        break ;

	                    op->collisions += 1 ;
	                    if (op->ropts & MAILALIAS_ORANDLC) {
	                        khash = randlc(khash + c) ;
	                    } else {
	                        khash = 
	                            ((khash << (32 - ns)) | (khash >> ns)) + c ;
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
	                cl = min(vlen,ALIASNAMELEN) ;
	                cp = strwcpy(vbuf,(op->sval + vi),cl) ;
	                vl = (cp - vbuf) ;
	            } else {
	                vl = strlen(op->sval + vi) ;
	            }
/* update cursor */
	            if (f_cur) {
	                curp->i = hi ;
	            }
	        } /* end if (got one) */
	        rs1 = mailalias_enterend(op,dt) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (mailalias-enter) */
	} /* end if (ok) */

	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (mailalias_fetch) */

#ifdef	COMMENT
int mailalias_getinfo(MA *op,MAILALIAS_INFO *rp) noex {
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;
	if (rp == nullptr) return SR_FAULT ;

	if (op->magic != MAILALIAS_MAGIC) return SR_NOTOPEN ;

	if (rp != nullptr) {
	    rp->collisions = op->collisions ;
	}

	return rs ;
}
/* end subroutine (mailalias_getinfo) */
#endif /* COMMENT */

int mailalias_check(MA *op,time_t dt) noex {
	int		rs = SR_OK ;
	int		f = false ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != MAILALIAS_MAGIC) return SR_NOTOPEN ;

	if ((! op->f.held) && (op->mapdata != nullptr)) {
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

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (mailalias_check) */


/* private subroutines */

static int mailalias_checkchanged(MA *op,time_t dt) noex {
	int		rs = SR_OK ;
	int		f = false ;
	if ((dt - op->ti_filecheck) > TO_FILECHECK) {
	    USTAT	sb ;
	    op->ti_filecheck = dt ;
	    if ((rs = u_stat(op->dbfname,&sb)) >= 0) {
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
	if (op->f.ocreate && op->f.owrite) {
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
	cint		msize = MAILALIAS_FILEMAGICSIZE ;
	int		rs = SR_OK ;
	cchar		*cp = (cchar *) op->mapdata ;
	if (hasValidMagic(cp,msize,MAILALIAS_FILEMAGIC)) {
	    cp += msize ;
	    if (cp[0] == MAILALIAS_FILEVERSION) {
	        if (cp[1] == ENDIAN) {
	            op->ropts = MKCHAR(cp[2]) ;
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
	op->i.wtime = table[header_wtime] ;
	op->i.wcount = table[header_wcount] ;
#endif

	op->keytab = (int *) (op->mapdata + table[header_key]) ;
	op->rectab = (int (*)[2]) (op->mapdata + table[header_rec]) ;
	op->indtab = (int (*)[2]) (op->mapdata + table[header_ind]) ;
	op->skey = charp(op->mapdata + table[header_skey]) ;
	op->sval = charp(op->mapdata + table[header_sval]) ;

	op->ktlen = table[header_keylen] ;
	op->rtlen = table[header_reclen] ;
	op->rilen = table[header_indlen] ;
	op->sklen = table[header_skeysize] ;
	op->svlen = table[header_svalsize] ;

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
	    }
	}
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (mailalias_mapbegin) */

static int mailalias_mapend(MA *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->mapdata != nullptr) {
	    rs1 = u_mmapend(op->mapdata,op->mapsize) ;
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
	    USTAT	sb ;
	    op->fd = rs ;
	    if ((rs = u_fstat(op->fd,&sb)) >= 0) {
	        op->fi.mtime = sb.st_mtime ;
	        op->fi.fsize = sb.st_size ;
	        op->fi.ino = sb.st_ino ;
	        op->fi.dev = sb.st_dev ;
	        op->ti_open = dt ;
	        if (op->f.ocreate && op->f.owrite) {
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
	    op->f.needcreate = true ;
	} /* end if (successful file open) */
	if ((rs >= 0) && f_create) {
	    op->f.needcreate = true ;
	}
	return (rs >= 0) ? f_create : rs ;
}
/* end subroutine (mailalias_dbopenfile) */

static int mailalias_dbopenmake(MA *op,time_t dt) noex {
	int		rs = SR_OK ;
	if (op->f.ocreate && op->f.owrite && op->f.needcreate) {
	    if ((rs = mailalias_dbmake(op,dt)) >= 0) {
	        cint	of = O_RDONLY ;
	        if ((rs = u_open(op->dbfname,of,op->operm)) >= 0) {
	            USTAT	sb ;
	            op->fd = rs ;
	            if ((rs = u_fstat(op->fd,&sb)) >= 0) {
	                op->fi.mtime = sb.st_mtime ;
	                op->fi.fsize = sb.st_size ;
	                op->fi.ino = sb.st_ino ;
	                op->fi.dev = sb.st_dev ;
	                op->ti_open = dt ;
	            }
	            if (rs < 0) {
	                u_close(op->fd) ;
	                op->fd = -1 ;
	            }
	        } /* end if (open) */
	    } /* end if */
	} /* end if (created or re-created the file) */
	return rs ;
}
/* end subroutine (mailalias_dbopenmake) */

static int mailalias_dbopenwait(MA *op) noex {
	USTAT		sb ;
	cint		to = TO_FILECOME ;
	int		rs = SR_OK ;
	int		msize ; /* minimum size */
	int		i ;

	msize = (MAILALIAS_TOPLEN) ;
/* wait for the file to come in if it is not yet available */
	if ((rs = u_fstat(op->fd,&sb)) >= 0) {
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
	    op->f.remote = true ;
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
	int		cl ;
	cchar		*cp ;
	if (dt == 0) dt = time(nullptr) ;
/* get the directory of the DB file and see if it is writable to us */
	if ((cl = sfdirname(op->dbfname,-1,&cp)) > 0) {
	    char	dbuf[MAXPATHLEN + 1] ;
	    if ((rs = mkpath1w(dbuf,cp,cl)) >= 0) {
	        USTAT	sb ;
	        if ((rs = u_stat(dbuf,&sb)) >= 0) {
	            if (! S_ISDIR(sb.st_mode)) rs = SR_NOTDIR ;
	            if (rs >= 0) {
	                rs = sperm(&op->id,&sb,W_OK) ;
	            }
	        } else if (rs == SR_NOENT) {
	            cmode	dm = MAILALIAS_DIRMODE ;
	            rs = mkdirs(dbuf,dm) ;
	        }
	        if (rs >= 0) {
	            rs = mailalias_dbmaker(op,dt,dbuf) ;
	        }
	    }
	} else {
	    rs = SR_ISDIR ;
	} /* end if */
	return rs ;
}
/* end subroutine (mailalias_dbmake) */

static int mailalias_dbmaker(MA *op,time_t dt,cchar *dname) noex {
	cint		sz = MAILALIAS_TOPLEN ;
	cint		clen = MAXNAMELEN ;
	int		n = NREC_GUESS ;
	int		rs ;
	int		rs1 ;
	cchar		*suf = MAILALIAS_FE ;
	cchar		*end = ENDIANSTR ;
	char		cbuf[MAXNAMELEN+1] ;
	if ((rs = sncpy5(cbuf,clen,"dbmkXXXXXX",".",suf,end,"n")) >= 0) {
	    char	tbuf[MAXPATHLEN+1] ;
	    if ((rs = mkpath2(tbuf,dname,cbuf)) >= 0) {
	        int	of = (O_WRONLY | O_CREAT | O_EXCL) ;
	        int	fd = -1 ;
	        char	nfname[MAXPATHLEN+1] ;
	        if ((rs = opentmpfile(tbuf,of,0664,nfname)) >= 0) {
	            fd = rs ;
	        } else if (rs == SR_EXIST) {
	            USTAT	sb ;
	            int		i ; /* used-afterwards */
	            for (i = 0 ; i < TO_FILECOME ; i += 1) {
	                msleep(1) ;
	                rs1 = u_stat(op->dbfname,&sb) ;
	                if ((rs1 >= 0) && (sb.st_size >= sz)) break ;
	            } /* end for */
	            rs = (i < TO_FILECOME) ? SR_OK : SR_TIMEDOUT ;
	            if (rs == SR_TIMEDOUT) {
	                of = (O_WRONLY | O_CREAT) ;
	                rs = u_open(nfname,of,0664) ;
	                if (rs == SR_ACCESS) {
	                    u_unlink(nfname) ;
	                    rs = u_open(nfname,of,0664) ;
	                }
	                fd = rs ;
	            } /* end if (attempt to break the lock) */
	        } /* end if (waiting for file to come in) */
	        if ((rs >= 0) && (fd >= 0)) {
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
	return rs ;
}
/* end subroutine (mailalias_dbmaker) */

static int mailalias_dbmaking(MA *op,int fd,time_t dt,int n) noex {
	cint		sz = sizeof(record) ;
	int		rs ;
	int		rs1 ;
	vecobj		recs ;
	if ((rs = vecobj_start(&recs,sz,n,0)) >= 0) {
	    strtab	skeys ;
	    if ((rs = strtab_start(&skeys,n)) >= 0) {
	        strtab	svals ;
	        if ((rs = strtab_start(&svals,(n * 2))) >= 0) {
	            if ((rs = mailalias_aprofile(op,dt)) >= 0) {
	                dbmake	data(&recs,&skeys,&svals,fd) ;
	                cchar	*cp ;
	                char	tmpfname[MAXPATHLEN+1] ;
			data.rectab = op->rectab ;
			data.ropts = op->ropts ;
	                for (int i = 0 ; op->aprofile[i] != nullptr ; i += 1) {
	                    cp = (cchar *) op->aprofile[i] ;
	                    if (*cp != '/') {
	                        cchar	*ap = op->aprofile[i] ;
	                        cp = tmpfname ;
	                        rs = mkpath2(tmpfname,op->pr,ap) ;
	                    }
	                    if (rs >= 0) {
	                        rs = mailalias_procfile(op,&data,cp) ;
	                    }
	                    if (rs < 0) break ;
	                } /* end for */
/* where are we? */
/* OK, write out the file */
	                if (rs >= 0) {
	                    if ((rs = mailalias_wrfile(op,&data,dt)) >= 0) {
	                        u_close(fd) ;
	                        fd = -1 ;
	                    } /* end if */
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

static int mailalias_procfile(MA *op,DBMAKE *dp,cchar *fname) noex {
	bfile		afile ;
	int		rs ;
	int		rs1 ;
	int		c_rec = 0 ; /* ¥ GCC false complaint */

	if (op == nullptr) return SR_FAULT ;

	if (fname[0] == '\0') return SR_INVALID ;

	if ((rs = bopen(&afile,fname,"r",0666)) >= 0) {
	    cint	llen = LINEBUFLEN ;
	    int		f_bol = true ;
	    int		f_eol ;
	    char	lbuf[LINEBUFLEN + 1] ;

	    dp->f_havekey = false ;
	    c_rec = 0 ;
	    while ((rs = breadln(&afile,lbuf,llen)) > 0) {
	        int	len = rs ;
	        f_eol = (lbuf[len - 1] == '\n') ;
	        if (f_eol) len -= 1 ;
	        if ((len > 0) && f_bol && f_eol) {
	            if (lbuf[0] != '#') {
	                if (! CHAR_ISWHITE(lbuf[0])) {
	                    dp->f_havekey = false ;
	                }
	                rs = dp->procline(lbuf,len) ;
	                c_rec += rs ;
	            }
	        } /* end if (BOL and EOL) */
	        f_bol = f_eol ;
	        if (rs < 0) break ;
	    } /* end while (reading extended lines) */
	    rs1 = bclose(&afile) ;
	    if (rs >= 0) rs = rs1 ;
	} else if (isNotPresent(rs)) {
	    rs = SR_OK ;
	} /* end if (afile) */
	return (rs >= 0) ? c_rec : rs ;
}
/* end subroutine (mailalias_procfile) */

/* write out the cache file */
static int mailalias_wrfile(MA *op,DBMAKE *dp,time_t dt) noex {
	int		rs ;
	if ((rs = strtab_count(dp->klp)) >= 0) {
	    dp->ktlen = (rs+1) ;
	    dp->ktsize = (dp->ktlen + 1) * sizeof(int) ;
	    if ((rs = vecobj_count(dp->rlp)) >= 0) {
		dp->reclen = (rs + 1) ;
		dp->recsize = ((dp->reclen+1) * (2 * sizeof(int))) ;
		dp->rilen = nextpowtwo(dp->reclen) ;
		if (dp->rilen < 4) dp->rilen = 4 ;
		dp->risize = (dp->rilen * 2 * sizeof(int)) ;
		if ((rs = strtab_strsize(dp->klp)) >= 0) {
		    dp->sksize = rs ;
		    if ((rs = strtab_strsize(dp->vlp)) >= 0) {
			dp->svsize = rs ;
		        rs = mailalias_wrfiler(op,dp,dt) ;
		    }
		}
	    }
	}
	return rs ;
}
/* end subroutine (mailalias_wrfile) */

/* OK, we're ready to start writing stuff out! */
static int mailalias_wrfiler(MA *op,DBMAKE *dp,time_t dt) noex {
	int		rs ;
	int		header[header_overlast] ;
	int		fto = 0 ;
	int		ml ;
	char		fidbuf[MAILALIAS_IDLEN + 1] ;
	char		*bp ;
	(void) op ;

/* prepare the file magic */

	bp = fidbuf ;
	ml = mkmagic(bp,MAILALIAS_FILEMAGICSIZE,MAILALIAS_FILEMAGIC) ;
	bp += ml ;

/* prepare the version and encoding (VETU) */

	fidbuf[16] = MAILALIAS_FILEVERSION ;
	fidbuf[17] = ENDIAN ;
	fidbuf[18] = dp->ropts ;
	fidbuf[19] = 0 ;

/* write magic along with version encoding */

	if ((rs = u_write(dp->fd,fidbuf,MAILALIAS_IDLEN)) >= 0) {
	    cint	hsize = (header_overlast * sizeof(int)) ;
	    fto += MAILALIAS_IDLEN ;

/* make the header itself (skip over it for FTO) */

	    fto += (header_overlast * sizeof(int)) ;

/* everything else */

	    header[header_wtime] = (uint) dt ;
	    header[header_wcount] = 1 ;

	    header[header_key] = fto ;
	    header[header_keylen] = dp->ktlen ;
	    fto += dp->ktsize ;

	    header[header_rec] = fto ;
	    header[header_reclen] = dp->reclen ;
	    fto += dp->recsize ;

	    header[header_ind] = fto ;
	    header[header_indlen] = dp->rilen ;
	    fto += dp->risize ;

	    header[header_skey] = fto ;
	    header[header_skeysize] = dp->sksize ;
	    fto += dp->sksize ;

	    header[header_sval] = fto ;
	    header[header_svalsize] = dp->svsize ;
	    fto += dp->svsize ;

/* write out the header */

	    if ((rs = u_write(dp->fd,header,hsize)) >= 0) {
		if ((rs = dp->wrfilekeytab()) >= 0) {
	            if ((rs = dp->wrfilerec()) >= 0) {
	        	if ((rs = dp->wrfilekeys()) >= 0) {
	        	    rs = dp->wrfilevals() ;
			}
		    }
		}
	    } /* end if (ok) */

	} /* end if (write of id-section) */

	return rs ;
}
/* end subroutine (mailalias_wrfiler) */

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
	    USTAT	sb ;
	    int		i ;
	    cchar	*cp ;
	    char	tmpfname[MAXPATHLEN + 1] ;
	    for (i = 0 ; op->aprofile[i] != nullptr ; i += 1) {
	        cp = (cchar *) op->aprofile[i] ;
	        if (*cp != '/') {
	            cp = tmpfname ;
	            mkpath2(tmpfname,op->pr,op->aprofile[i]) ;
	        }
	        rs1 = u_stat(cp,&sb) ;
	        if ((rs1 >= 0) && (sb.st_mtime > op->fi.mtime)) break ;
	    } /* end for */
	    f = (op->aprofile[i] != nullptr) ? 1 : 0 ;
	} /* end if */

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (mailalias_fileold) */

/* get apfiles */
static int mailalias_aprofile(MA *op,time_t dt) noex {
	USTAT		sb ;
	kvsfile		aptab ;
	cint	tlen = MAXPATHLEN ;
	int		rs = SR_OK ;
	int		rs1 ;
	cchar		*profp = nullptr ;
	char		tbuf[MAXPATHLEN + 1] ;

	if ((dt - op->ti_aprofile) <= TO_APROFILE)
	    return SR_OK ;

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

	    if ((rs1 >= 0) && S_ISDIR(sb.st_mode))
	        rs1 = SR_ISDIR ;

	    if (rs1 >= 0)
	        rs1 = sperm(&op->id,&sb,R_OK) ;

	    if (rs1 >= 0)
	        break ;

	} /* end for */

	if (profp) {
	    if ((rs = kvsfile_open(&aptab,0,profp)) >= 0) {
		vecstr		*app = &op->apfiles ;
	        kvsfile_cur	cur ;
		cchar		*fp{} ;
	        for (int i = 0 ; vecstr_get(app,i,&fp) >= 0 ; i += 1) {
	            if (fp) {
	                vecstr_del(&op->apfiles,i--) ;
	            }
	        } /* end for */
	        if ((rs = kvsfile_curbegin(&aptab,&cur)) >= 0) {
	            while (rs >= 0) {
	                rs1 = kvsfile_fetch(&aptab,op->apname,&cur,tbuf,tlen) ;
	                if (rs1 < 0) break ;
	                rs = vecstr_add(&op->apfiles,tbuf,rs1) ;
	            } /* end while */
	            kvsfile_curend(&aptab,&cur) ;
		} /* end if (kvsfile-cur) */
	        if (rs >= 0) {
	            rs = vecstr_getvec(&op->apfiles,&op->aprofile) ;
	        }
	        kvsfile_close(&aptab) ;
	    } /* end if (opened key-values table) */
	    if_constexpr (f_defprofile) {
	        if (op->aprofile[0] == nullptr) {
	            op->aprofile = defprofile ;
	        }
	    } /* end if_constexpr (f_defprofile) */
	} else {
	    op->aprofile = defprofile ;
	}

	return rs ;
}
/* end subroutine (mailalias_aprofile) */

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
/* ARGSUSED */
static int mailalias_enterend(MA *op,time_t dt) noex {
	int		rs = SR_OK ;
	(void) dt ;
	if (op->f.held) {
	    op->f.held = false ;
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

int dbmake::procline(cchar *lbuf,int llen) noex {
	field		fsb ;
	int		rs ;
	int		rs1 ;
	int		c_rec = 0 ;
	if ((rs = field_start(&fsb,lbuf,llen)) >= 0) {
	    cint	rsn = SR_NOTFOUND ;
	    cint	klen = ALIASNAMELEN ;
	    char	kbuf[ALIASNAMELEN+1] = { 0 } ;
	    if (! f_havekey) {
		cchar	*kt = keys.terms ;
	        cchar	*pm = "Postmaster" ;
	        cchar	*kp ;
	        if (int kl ; (kl = field_get(&fsb,kt,&kp)) >= 0) {
	            if (kl > 0) {
	                cbool	f = (kl == 10) && (strncasecmp(pm,kp,kl) == 0) ;
	                f_havekey = true ;
	                if (f) {
	                    strwcpylc(kbuf,kp,min(kl,klen)) ;
	                } else {
	                    strwcpy(kbuf,kp,min(kl,klen)) ;
	                }
	            } /* end if (positive) */
	        } /* end if (field_get) */
	    } /* end if (didn't have a key already) */
	    if (f_havekey && (fsb.term != '#')) {
		int	c = 0 ;
	        int	vl ;
		cchar	*vt = vals.terms ;
	        cchar	*vp ;
	        while ((vl = field_get(&fsb,vt,&vp)) >= 0) {
	            if (vl > 0) {
	                int	ival = 0 ;
	                if (c == 0) { /* enter into key-string table */
	                    if ((rs = strtab_already(klp,kbuf,-1)) == rsn) {
	                        rs = strtab_add(klp,kbuf,-1) ;
	                        ikey = rs ;
	                    } else {
	                        ikey = rs ;
	                    }
	                } /* end if (entering key) */
/* enter value into string table */
	                if (rs >= 0) { /* enter into val-string table */
	                    if ((rs = strtab_already(vlp,vp,vl)) == rsn) {
	                        rs = strtab_add(vlp,vp,vl) ;
	                        ival = rs ;
	                    } else if (rs >= 0) {
	                        ival = rs ;
	                    }
/* enter record */
	                    if ((rs >= 0) && (ival > 0)) {
	                        record	re(ikey,ival) ;
	                        if ((rs = vecobj_find(rlp,&re)) == rsn) {
	                            rs = vecobj_add(rlp,&re) ;
	                        }
	                        if (rs >= 0) {
	                            nrecs += 1 ;
	                            c_rec += 1 ;
				    c += 1 ;
	                        }
	                    } /* end if (new entry) */
	                } /* end if (ok) */
	                if (fsb.term == '#') break ;
	                if (rs < 0) break ;
	            } /* end while (fields) */
	        } /* end if (retrieved key) */
	    } /* end if (have key) */
	    rs1 = field_finish(&fsb) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (field) */
	count = c_rec ;
	return (rs >= 0) ? c_rec : rs ;
}
/* end method (dbmake::procline) */

/* write the key-table */
int dbmake::wrfilekeytab() noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	int		*keytab ;
	if ((rs = uc_malloc(ktsize,&keytab)) >= 0) {
	    if ((rs = strtab_recmk(klp,keytab,ktsize)) >= 0) {
		rs = u_write(fd,keytab,ktsize) ;
		wlen += rs ;
	    }
	    rs1 = uc_free(keytab) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if */
	return (rs >= 0) ? wlen : rs ;
}
/* end method (dbmake::wrfilekeytab) */

/* write the record table */
int dbmake::wrfilerec() noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	int		(*rectab)[2] ;
	if ((rs = uc_malloc(recsize,&rectab)) >= 0) {
	    {
	        void	*vp{} ;
	        int	ri = 0 ;
	        rectab[ri][0] = 0 ;
	        rectab[ri][1] = 0 ;
	        ri += 1 ;
	        for (int i = 0 ; vecobj_get(rlp,i,&vp) >= 0 ; i += 1) {
	    	    record	*rep = recordp(vp) ;
	            rectab[ri][0] = rep->key ;
	            rectab[ri][1] = rep->val ;
	            ri += 1 ;
	        } /* end for */
	        rectab[ri][0] = -1 ;
	        rectab[ri][1] = 0 ;
	        rs = u_write(fd,rectab,recsize) ;
	        wlen = rs ;
	    } /* end block */
	    rs1 = uc_free(rectab) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (record table) */
	return (rs >= 0) ? wlen : rs ;
}
/* end method (dbmake::wrfilerec) */

/* make the index table and the key-string table */
int dbmake::wrfilekeys() noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	int		(*indtab)[2] ;
	if ((rs = uc_malloc(risize,&indtab)) >= 0) {
	    char	*kstab ;
	    if ((rs = uc_malloc(sksize,&kstab)) >= 0) {
	        if ((rs = strtab_strmk(klp,kstab,sksize)) >= 0) {
	            int		(*it)[2] = indtab ;
	            cint	ris = risize ;
	            if ((rs = mkind(rlp,kstab,it,ris)) >= 0) {
	                if ((rs = u_write(fd,indtab,risize)) >= 0) {
			    wlen += rs ;
	                    rs = u_write(fd,kstab,sksize) ;
			    wlen += rs ;
	                }
	            }
	        }
	        rs1 = uc_free(kstab) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (memory allocation) */
	    rs1 = uc_free(indtab) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (memory allocation) */
	return (rs >= 0) ? wlen : rs ;
}
/* end method (dbmake::wrfilekeys) */

/* make the value-string table */
int dbmake::wrfilevals() noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	char		*vstab ;
	if ((rs = uc_malloc(svsize,&vstab)) >= 0) {
	    if ((rs = strtab_strmk(vlp,vstab,svsize)) >= 0) {
	        rs = u_write(fd,vstab,svsize) ;
		wlen += rs ;
	    }
	    rs1 = uc_free(vstab) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (value string-table) */
	return (rs >= 0) ? wlen : rs ;
}
/* end method (dbmake::wrfilevals) */

/* make the (only) index for this file */
int dbmake::mkind(vecobj *rp,cc *skey,rt_t it,int itsz) noex {
	cint		ns = NSHIFT ;
	int		rs = SR_FAULT ;
	int		n = 0 ; /* ¥ GCC false complaint */
	if (it) {
	    int		sz ;
	    if ((rs = vecobj_count(rp)) >= 0) {
	        n = nextpowtwo(rs) ;
	        if (n < 4) n = 4 ;
	        sz = (n * 2 * sizeof(uint)) ;
	        if (sz <= itsz) {
		    void	*vp{} ;
	            uint	khash ;
	            int		ri, hi, ki ;
	            int		v ;
	            int		kl ;
	            int		f ;
	            cchar	*kp ;
	            memset(it,0,sz) ;
	            ri = 1 ;
	            for (int i = 0 ; vecobj_get(rp,i,&vp) >= 0 ; i += 1) {
	                record	*rep = recordp(vp) ;
	                int	c = 0 ;
	                kp = charp(skey + rep->key) ;
	                kl = strlen(kp) ;
	                khash = hash_elf(kp,kl) ;
	                hi = hashindex(khash,n) ;
	                if ((ropts & MAILALIAS_OSEC) && (it[hi][0] != 0)) {
	                    while ((v = it[hi][0]) != 0) {
	                        ki = rectab[v][0] ;
	                        f = (strcmp(kp,(skey + ki)) == 0) ;
	                        if (! f) break ;
	                        if (ropts & MAILALIAS_ORANDLC) {
	                            khash = randlc(khash + c) ;
	                        } else {
	                            khash = ((khash<<(32-ns))|(khash>>ns))+c ;
	                        }
	                        hi = hashindex(khash,n) ;
	                        c += 1 ;
	                    } /* end while */
	                } /* end if (secondary hash on collision) */
	                if (it[hi][0] != 0) {
	                    int	lhi ;
	                    c += 1 ;
	                    while (it[hi][1] != 0) {
	                        c += 1 ;
	                        hi = it[hi][1] ;
	                    }
	                    lhi = hi ;	/* save last hash-index value */
	                    hi = hashindex((hi + 1),n) ;
	                    while (it[hi][0] != 0) {
	                        c += 1 ;
	                        hi = hashindex((hi + 1),n) ;
	                    } /* end while */
	                    it[lhi][1] = hi ; /* update the previous slot */
	                } /* end if (got a hash collision) */
	                it[hi][0] = ri ;
	                it[hi][1] = 0 ;
	                ri += 1 ;
	            } /* end for (looping through records) */
	        } else {
	            rs = SR_OVERFLOW ;
	        }
	    } /* end if (vecobj_count) */
	} /* end if (non-null) */
	return (rs >= 0) ? n : rs ;
}
/* end method (dbmake::mkind) */

/* calculate the next hash table index from a given one */
static int hashindex(uint i,int n) noex {
	int		hi = MODP2(i,n) ;
	if (hi == 0) hi = 1 ;
	return hi ;
}
/* end if (hashindex) */


