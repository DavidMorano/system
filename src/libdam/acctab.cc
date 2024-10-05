/* acctab SUPPORT */
/* lang=C++20 */

/* perform access-table file related functions */
/* version %I% last-modified %G% */

#define	CF_REGEX	0		/* stupid UNIX REGEX? (buggy) */

/* revision history:

	= 1998-06-01, David A­D­ Morano
	This code was originally written (and largely forgotten).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	acctab

	Description:
	This object processes an access-table (file) for use by daemon
	multiplexing server programs that want to control access
	to their sub-servers.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<getpwd.h>
#include	<bfile.h>
#include	<field.h>
#include	<vecobj.h>
#include	<vecitem.h>
#include	<vecstr.h>
#include	<mkpathx.h>
#include	<starmat.h>
#include	<char.h>
#include	<mkchar.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"acctab.h"


/* local defines */

#define	ACCTAB_MINCHECKTIME	5	/* file check interval (seconds) */
#define	ACCTAB_CHECKTIME	60	/* file check interval (seconds) */
#define	ACCTAB_CHANGETIME	3	/* wait change interval (seconds) */
#define	ACCTAB_DEFNETGROUP	"DEFAULT"
#define	ACCTAB_RGXLEN		256
#define	ACCTAB_FI		struct acctab_file

#define	PARTTYPE		struct acctab_part
#define	PARTTYPE_STD		0
#define	PARTTYPE_RGX		1
#define	PARTTYPE_UNKNOWN	2

#ifndef	LINEBUFLEN
#ifdef	LINE_MAX
#define	LINEBUFLEN	MAX(LINE_MAX,2048)
#else
#define	LINEBUFLEN	2048
#endif
#endif

#undef	ACCTAB_REGEX
#define	ACCTAB_REGEX	CF_REGEX


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef acctab_ent	ent ;
typedef acctab_ent *	entp ;


/* external subroutines */


/* external variables */


/* local structures */

struct acctab_file {
	cchar		*fname ;
	time_t		mtime ;
	dev_t		dev ;
	ino_t		ino ;
	int		len ;
} ;

typedef acctab_file *	filep ;


/* forward references */

template<typename ... Args>
static int acctab_ctor(acctab *op,Args ... args) noex {
	ACCTAB		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    memclear(hop) ;
	    if ((op->flp = new(nothrow) vecobj) != np) {
		rs = SR_OK ;
	    } /* end if (new-vecobj) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (acctab_ctor) */

static int acctab_dtor(acctab *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->flp) {
		delete op->flp ;
		op->flp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (acctab_dtor) */

template<typename ... Args>
static inline int acctab_magic(acctab *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == ACCTAB_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (acctab_magic) */

static int	acctab_filefins(acctab *) noex ;
static int	acctab_freeentries(acctab *) noex ;
static int	acctab_fileparse(acctab *,int) noex ;
static int	acctab_filedump(acctab *,int) noex ;
static int	acctab_filedel(acctab *,int) noex ;
static int	acctab_addentry(acctab *,acctab_ent *) noex ;
static int	acctab_checkfiles(acctab *,time_t) noex ;

static int	entry_start(acctab_ent *) noex ;
static int	entry_load(acctab_ent *,cchar *,cchar *,cchar *,cchar *) noex ;
static int	entry_mat2(acctab_ent *,acctab_ent *) noex ;
static int	entry_mat3(acctab_ent *,acctab_ent *) noex ;
static int	entry_finish(acctab_ent *) noex ;

static int	file_start(ACCTAB_FI *,cchar *) noex ;
static int	file_release(ACCTAB_FI *) noex ;
static int	file_finish(ACCTAB_FI *) noex ;

static int	part_start(acctab_pa *) noex ;
static int	part_copy(acctab_pa *,acctab_pa *) noex ;
static int	part_compile(acctab_pa *,cchar *,int) noex ;
static int	part_match(acctab_pa *,cchar *) noex ;
static int	part_finish(acctab_pa *) noex ;

static int	parttype(cchar *) noex ;

extern "C" {
    static int	vcmpent(cvoid **,cvoid **) noex ;
}

static int	freeit(cchar **) noex ;


/* local varaibles */

constexpr cchar		gterms[] = {
	0x00, 0x1B, 0x00, 0x00,
	0x09, 0x00, 0x00, 0x04,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ;

constexpr cchar		aterms[] = {
	0x00, 0x1B, 0x00, 0x00,
	0x09, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ;

constexpr int		intcheck = ACCTAB_CHECKTIME ;


/* exported variables */


/* exported subroutines */

int acctab_open(acctab *op,cchar *fname) noex {
	int		rs ;
	if ((rs = acctab_ctor(op)) >= 0) {
	    cint	sz = sizeof(struct acctab_file) ;
	    cint	vn = 10 ;
	    int		vo = VECOBJ_OREUSE ;
	    op->checktime = getustime ;
	    if ((rs = vecobj_start(op->flp,sz,vn,vo)) >= 0) {
	        vo = VECITEM_OSORTED ;
	        if ((rs = vecitem_start(&op->aes_std,vn,vo)) >= 0) {
	            vo = VECITEM_OCOMPACT ;
	            if ((rs = vecitem_start(&op->aes_rgx,vn,vo)) >= 0) {
	                op->magic = ACCTAB_MAGIC ;
	                if (fname) {
	                    rs = acctab_fileadd(op,fname) ;
	                }
	                if (rs < 0) {
	                    op->magic = 0 ;
	                    vecitem_finish(&op->aes_rgx) ;
	                }
	            } /* end if (rgx) */
	            if (rs < 0) {
	                vecitem_finish(&op->aes_std) ;
		    }
	        } /* end if (std) */
	        if (rs < 0) {
	            vecobj_finish(op->flp) ;
	        }
	    } /* end if (vecobj-files) */
	    if (rs < 0) {
		acctab_dtor(op) ;
	    }
	} /* end if (acctab_ctor) */
	return rs ;
}
/* end subroutine (acctab_open) */

int acctab_close(acctab *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = acctab_magic(op)) >= 0) {
	    {
	        rs1 = acctab_freeentries(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = acctab_filefins(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = vecitem_finish(&op->aes_std) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = vecitem_finish(&op->aes_rgx) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = vecobj_finish(op->flp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = acctab_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (acctab_close) */

int acctab_fileadd(acctab *op,cchar *fname) noex {
	int		rs ;
	if ((rs = acctab_magic(op,fname)) >= 0) {
	ACCTAB_FI	fe ;
	int		fi ;
	cchar		*sp ;
	char		tmpfname[MAXPATHLEN + 1] ;
	sp = fname ;
	if (fname[0] != '/') {
	    char	pwdbuf[MAXPATHLEN+1] ;
	    if ((rs = getpwd(pwdbuf,MAXPATHLEN)) >= 0) {
	        sp = tmpfname ;
	        rs = mkpath2(tmpfname,pwdbuf,fname) ;
	    }
	} /* end if (rooting file) */

	if (rs >= 0) {
	    if ((rs = file_start(&fe,sp)) >= 0) {
		if ((rs = vecobj_add(op->flp,&fe)) >= 0) {
		    fi = rs ;
		    file_release(&fe) ;
		    rs = acctab_fileparse(op,fi) ;
		    if (rs < 0) {
			acctab_filedel(op,fi) ;
		    }
		} /* end if (vecobj_add) */
		if (rs < 0) {
		    file_finish(&fe) ;
		}
	    } /* end if (file_start) */
	} /* end if (ok) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (acctab_fileadd) */

int acctab_allowed(acctab *op,cchar *ng,cchar *ma,cchar *un,cchar *pw) noex {
	custime		dt = getustime ;
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	int		rs1 ;
	int		f = false ;
	if ((rs = acctab_magic(op,ng,ma,un,pw)) >= 0) {
	    /* should we check for changes to underlying file? */
	    if ((dt - op->checktime) > ACCTAB_MINCHECKTIME) {
	        rs = acctab_checkfiles(op,dt) ;
	    } /* end if (we needed to check) */
	    if (rs >= 0) {
	        acctab_ent	ae ;
	        acctab_ent	*aep ;
	        vecitem		*slp ;
	        vecitem_cur	cur ;
	        void		*vp{} ;
	        /* load up a fake entry for comparison purposes */
	        if ((rs = entry_start(&ae)) >= 0) {
	            if ((rs = entry_load(&ae,ng,ma,un,pw)) >= 0) {
		    /* search the STD entries first */
	                slp = &op->aes_std ;
	                if ((rs = vecitem_curbegin(slp,&cur)) >= 0) {
		            auto	vif = vecitem_fetch ;
	                    while ((rs1 = vif(slp,&ae,&cur,vcmpent,&vp)) >= 0) {
		                aep = entp(vp) ;
	                        if (vp) {
	                            f = entry_mat2(aep,&ae) ;
	                            if (f) break ;
	                        }
	                    } /* end while */
	                    if ((rs >= 0) && (rs1 != rsn)) rs = rs1 ;
	                    vecitem_curend(slp,&cur) ;
	                } /* end if (vecitem-cur) */
		        /* search the RGX entries (if necessary) */
	                if ((rs == rsn) || (! f)) {
	                    slp = &op->aes_rgx ;
		            auto	vig = vecitem_get ;
	                for (int i = 0 ; (rs1 = vig(slp,i,&vp)) >= 0 ; i += 1) {
		    		aep = entp(vp) ;
	            		if (vp) {
				    f = entry_mat3(aep,&ae) ;
	                            if (f) break ;
	                        }
	                    } /* end for (looping through entries) */
	                    if ((rs >= 0) && (rs1 != rsn)) rs = rs1 ;
	                } /* end if (comparing RGX entries) */
		    } /* end if (entry_load) */
		    entry_finish(&ae) ;
	        } /* end if (entry) */
	    } /* end if (ok) */
	} /* end if (magic) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (acctab_allowed) */

int acctab_anyallowed(acctab *op,vecstr *nlp,vecstr *mlp,cc *un,cc *pw) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		rs2 ;
	int		f = false ;
	if ((rs = acctab_magic(op,nlp,mlp,un,pw)) >= 0) {
	    cchar	*ngp ; /* netgroup pointer */
	    cchar	*mp ; /* machine pointer */
	    auto	vsg = vecstr_get ;
	    for (int i = 0 ; (rs1 = vsg(nlp,i,&ngp)) >= 0 ; i += 1) {
	        if (ngp) {
	            for (int j = 0 ; (rs2 = vsg(mlp,j,&mp)) >= 0 ; j += 1) {
	                if (mp) {
	                    if ((rs = acctab_allowed(op,ngp,mp,un,pw)) >= 0) {
	                        f = true ;
	                    } else if (rs == rsn) {
	                        rs = SR_OK ;
	                    }
	                }
	                if (f) break ;
	                if (rs < 0) break ;
	            } /* end for */
	            if ((rs >= 0) && (rs2 != rsn)) rs = rs2 ;
	        }
	        if (f) break ;
	        if (rs < 0) break ;
	    } /* end for (looping over netgroups) */
	    if ((rs >= 0) && (rs1 != rsn)) rs = rs1 ;
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (acctab_anyallowed) */

#ifdef	COMMENT

/* search the netgroup table for a netgroup match */
int acctab_find(acctab *op,cc *netgroup,acctab_ent **sepp) noex {
	int		rs ;
	int		idx = 0 ;
	if ((rs = acctab_magic(op,netgroup,sepp)) >= 0) {
	    int		i ; /* used-afterwards */
	    vecitem	*slp = &op->e ;
	    cchar	*sp ;
	    bool	f = false ;
	    void	*vp{} ;
	    for (i = 0 ; vecitem_get(slp,i,&vp) >= 0 ; i += 1) {
		*sepp = entp(vp) ;
	        if (*sepp == nullptr) continue ;
	        sp = (*sepp)->netgroup ;
	        f = (strcmp(netgroup,sp) == 0) ;
	        if (f) break ;
	    } /* end for (looping through entries) */
	    if (f) idx = i ;
	} /* end if (magic) */
	return (rs >= 0) ? idx : rs ;
}
/* end subroutine (acctab_find) */

#endif /* COMMENT */

int acctab_curbegin(acctab *op,acctab_cur *curp) noex {
	int		rs ;
	if ((rs = acctab_magic(op,curp)) >= 0) {
	    curp->i = -1 ;
	    curp->j = -1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (acctab_curbegin) */

int acctab_curend(acctab *op,acctab_cur *curp) noex {
	int		rs ;
	if ((rs = acctab_magic(op,curp)) >= 0) {
	    curp->i = -1 ;
	    curp->j = -1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (acctab_curend) */

int acctab_curenum(acctab *op,acctab_cur *curp,acctab_ent **sepp) noex {
	int		rs ;
	if ((rs = acctab_magic(op,curp)) >= 0) {
	    vecitem	*slp ;
	    acctab_ent	*aep ;
	    int		j ;
	    auto	vig = vecitem_get ;
	    void	*vp{} ;
	    if (sepp == nullptr) sepp = &aep ;
	    rs = SR_NOTFOUND ;
	    if (curp->i <= 0) {
	        slp = &op->aes_std ;
	        curp->j = (curp->j < 0) ? 0 : (curp->j + 1) ;
	        for (j = curp->j ; (rs = vig(slp,j,&vp)) >= 0 ; j += 1) {
		    *sepp = entp(vp) ;
	            if (*sepp != nullptr) break ;
	        } /* end for */
	        if (rs < 0) {
	            curp->j = -1 ;
	            curp->i = 1 ;
	        } else {
	            curp->i = 0 ;
	            curp->j = j ;
	        } /* end if */
	    } /* end if (cursor needed initialization) */
	    if (curp->i == 1) {
	        slp = &op->aes_rgx ;
	        curp->j = (curp->j < 0) ? 0 : curp->j + 1 ;
	        for (j = curp->j ; (rs = vig(slp,j,&vp)) >= 0 ; j += 1) {
		    *sepp = entp(vp) ;
	            if (*sepp != nullptr) break ;
	        } /* end for */
	        if (rs < 0) {
	            curp->j = -1 ;
	            curp->i += 1 ;
	        } else {
	            curp->j = j ;
	        } /* end if */
	    } /* end if (RGX entries) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (acctab_curenum) */

/* check if the access tables files have changed */
int acctab_check(acctab *op) noex {
	custime		dt = time(nullptr) ;
	int		rs ;
	if ((rs = acctab_magic(op)) >= 0) {
	    if ((dt - op->checktime) > intcheck) {
	        rs = acctab_checkfiles(op,dt) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (acctab_check) */


/* private subroutines */

static int acctab_filefins(acctab *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	void		*vp{} ;
	for (int i = 0 ; vecobj_get(op->flp,i,&vp) >= 0 ; i += 1) {
	    ACCTAB_FI	*fep = (ACCTAB_FI *) vp ;
	    if (fep != nullptr) {
	        rs1 = file_finish(fep) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end for */
	return rs ;
}
/* end subroutine (acctab_filefins) */

static int acctab_checkfiles(acctab *op,time_t dt) noex {
	int		rs = SR_OK ;
	int		c_changed = false ;
	void		*vp{} ;
	for (int i = 0 ; vecobj_get(op->flp,i,&vp) >= 0 ; i += 1) {
	    ACCTAB_FI	*fep = (ACCTAB_FI *) vp ;
	    if (vp) {
		USTAT	sb ;
	    if ((u_stat(fep->fname,&sb) >= 0) &&
	        (sb.st_mtime > fep->mtime) &&
	        ((dt - sb.st_mtime) >= ACCTAB_CHANGETIME)) {

	        acctab_filedump(op,i) ;
	        if ((rs = acctab_fileparse(op,i)) >= 0) {
	            c_changed += 1 ;
		}
	    } /* end if */
	    } /* end if (non-nullptr) */
	    if (rs < 0) break ;
	} /* end for */
	if ((rs >= 0) && c_changed) {
	    rs = vecitem_sort(&op->aes_std,vcmpent) ;
	} /* end if (something changed) */
	op->checktime = dt ;
	return (rs >= 0) ? c_changed : rs ;
}
/* end subroutine (acctab_checkfiles) */

/* parse an access server file */
static int acctab_fileparse(acctab *op,int fi) noex {
	PARTTYPE	netgroup ;
	bfile		file, *fp = &file ;
	ACCTAB_FI	*fep ;
	acctab_ent	se ;
	USTAT		sb ;
	field		fsb ;
	int		rs ;
	int		c ;
	int		cl, len ;
	int		line = 0 ;
	int		c_added = 0 ;
	cchar	*cp ;
	char		lbuf[LINEBUFLEN + 1] ;

	void		*vp{} ;
	rs = vecobj_get(op->flp,fi,&vp) ;
	fep = filep(vp) ;

	if (rs < 0)
	    goto ret0 ;

	rs = bopen(fp,fep->fname,"r",0664) ;

	if (rs < 0)
	    goto ret0 ;

	rs = bcontrol(fp,BC_STAT,&sb) ;
	if (rs < 0)
	    goto done ;

/* have we already parsed this one? */

	rs = SR_OK ;
	if (fep->mtime >= sb.st_mtime)
	    goto done ;

	fep->dev = sb.st_dev ;
	fep->ino = sb.st_ino ;
	fep->mtime = sb.st_mtime ;
	fep->len = sb.st_size ;

/* start processing the configuration file */

	part_compile(&netgroup,ACCTAB_DEFNETGROUP,-1) ;

	c_added = 0 ;
	while ((rs = breadln(fp,lbuf,LINEBUFLEN)) > 0) {
	    len = rs ;

	    line += 1 ;
	    if (len == 1) continue ;	/* blank line */

	    if (lbuf[len - 1] != '\n') {
	        while ((c = bgetc(fp)) >= 0) {
	            if (c == '\n') break ;
	        }
	        continue ;
	    }

	    cp = lbuf ;
	    cl = len ;
	    lbuf[--cl] = '\0' ;
	    while (CHAR_ISWHITE(*cp)) {
	        cp += 1 ;
	        cl -= 1 ;
	    }

	    if ((*cp == '\0') || (*cp == '#')) continue ;

	    if ((rs = field_start(&fsb,cp,cl)) >= 0) {
	        int	fl ;
	        cchar	*fp ;
	        if ((fl = field_get(&fsb,gterms,&fp)) > 0) {

/* we have something! */

	            entry_start(&se) ;

	            se.fi = fi ;
	            if (fsb.term == ':') {
	                part_finish(&netgroup) ;
	                rs = part_compile(&netgroup,fp,fl) ;
	                fl = field_get(&fsb,aterms,&fp) ;
	            } /* end if */

/* see if there is a machine on this same line */

	            if (fl > 0) {

	                part_copy(&se.netgroup,&netgroup) ;

/* there is something else on this line */

	                rs = part_compile(&se.machine,fp,fl) ;

	                if (rs == SR_NOMEM)
	                    goto badpart ;

	                if ((fl = field_get(&fsb,aterms,&fp)) > 0) {

	                    rs = part_compile(&se.username,fp,fl) ;

	                    if (rs == SR_NOMEM)
	                        goto badpart ;

	                }

	                if ((fl = field_get(&fsb,aterms,&fp)) > 0) {

	                    rs = part_compile(&se.password,fp,fl) ;

	                    if (rs == SR_NOMEM)
	                        break ;

	                }

	                rs = acctab_addentry(op,&se) ;

	                if (rs >= 0)
	                    c_added += 1 ;

	            } /* end if */

	        } /* end if (got something) */

	        field_finish(&fsb) ;
	    } /* end if (field) */

	    if (rs < 0) break ;
	} /* end while (reading lines) */

	part_finish(&netgroup) ;

	if (rs < 0)
	    goto bad4 ;

/* done with configuration file processing */
done:
	bclose(fp) ;

ret0:
	return (rs >= 0) ? c_added : rs ;

/* handle bad things */
bad4:
	acctab_filedump(op,fi) ;

badpart:
	part_finish(&netgroup) ;

	entry_finish(&se) ;

	goto done ;
}
/* end subroutine (acctab_fileparse) */

static int acctab_addentry(acctab *op,acctab_ent *sep) noex {
	int		rs ;
	if (parttype(sep->netgroup.std) == PARTTYPE_STD) {
	    rs = vecitem_add(&op->aes_std,sep,sizeof(acctab_ent)) ;
	} else {
	    rs = vecitem_add(&op->aes_rgx,sep,sizeof(acctab_ent)) ;
	}
	return rs ;
}
/* end subroutine (acctab_addentry) */

static int acctab_filedump(acctab *op,int fi) noex {
	vecitem		*slp ;
	int		rs = SR_OK ;
	int		rs1 ;
	for (int j = 0 ; j < 2 ; j += 1) {
	    slp = (j == 0) ? &op->aes_std : &op->aes_rgx ;
	    void	*vp{} ;
	    for (int i = 0 ; (rs = vecitem_get(slp,i,&vp)) >= 0 ; i += 1) {
	        acctab_ent	*sep = entp(vp) ;
	        if (vp) {
	            if ((sep->fi == fi) || (fi < 0)) {
			{
	                rs1 = entry_finish(sep) ;
	                if (rs >= 0) rs = rs1 ;
			}
			{
	                rs1 = vecitem_del(slp,i--) ;
	                if (rs >= 0) rs = rs1 ;
			}
	            } /* end if (found matching entry) */
	        }
	    } /* end for (looping through entries) */
	} /* end for (looping on the different types of entries) */
	return rs ;
}
/* end subroutine (acctab_filedump) */

static int acctab_filedel(acctab *op,int fi) noex {
	int		rs = SR_OK ;
	if (void *vp{} ; (rs = vecobj_get(op->flp,fi,&vp)) >= 0) {
	    ACCTAB_FI	*fep = (ACCTAB_FI *) vp ;
	    if (vp) {
	        file_finish(fep) ;
	        rs = vecobj_del(op->flp,fi) ;
	    }
	}
	return rs ;
}
/* end subroutine (acctab_filedel) */

static int acctab_freeentries(acctab *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    vecitem	*slp ;
	    rs = SR_OK ;
	    for (int j = 0 ; j < 2 ; j += 1) {
	        slp = (j == 0) ? &op->aes_std : &op->aes_rgx ;
		void	*vp{} ;
	        for (int i = 0 ; vecitem_get(slp,i,&vp) >= 0 ; i += 1) {
	    	    acctab_ent	*sep = entp(vp) ;
	            if (vp) {
	                rs1 = entry_finish(sep) ;
	                if (rs >= 0) rs = rs1 ;
	            }
	        } /* end for */
	    } /* end for */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (acctab_freeentries) */

static int file_start(ACCTAB_FI *fep,cchar *fname) noex {
	int		rs = SR_FAULT ;
	if (fep && fname) {
	    memclear(fep) ;
	    if (cchar *cp{} ; (rs = uc_mallocstrw(fname,-1,&cp)) >= 0) {
	        fep->fname = cp ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (file_start) */

static int file_release(ACCTAB_FI *fep) noex {
	int		rs = SR_FAULT ;
	if (fep) {
	    rs = SR_OK ;
	    fep->fname = nullptr ;
	}
	return rs ;
}
/* end subroutine (file_release) */

static int file_finish(ACCTAB_FI *fep) noex {
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

static int entry_start(acctab_ent *sep) noex {
	int		rs = SR_FAULT ;
	if (sep) {
	    rs = memclear(sep) ;
	    sep->fi = -1 ;
	    part_start(&sep->netgroup) ;
	    part_start(&sep->machine) ;
	    part_start(&sep->username) ;
	    part_start(&sep->password) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (entry_start) */

static int entry_finish(acctab_ent *sep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (sep) {
	    rs = SR_OK ;
	    if (sep->fi >= 0) {
	        rs1 = part_finish(&sep->netgroup) ;
	        if (rs >= 0) rs = rs1 ;
	        rs1 = part_finish(&sep->machine) ;
	        if (rs >= 0) rs = rs1 ;
	        rs1 = part_finish(&sep->username) ;
	        if (rs >= 0) rs = rs1 ;
	        rs1 = part_finish(&sep->password) ;
	        if (rs >= 0) rs = rs1 ;
	        sep->fi = -1 ;
	    } /* end if */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (entry_finish) */

static int entry_load(acctab_ent *aep,cc *netgroup,cc *machine,
		cc *username,cc *password) noex {
	int		rs = SR_FAULT ;
	if (aep) {
	    rs = SR_OK ;
	    aep->netgroup.std = charp(netgroup) ;
	    if ((netgroup == nullptr) || (netgroup[0] == '\0')) {
	        aep->netgroup.std = ACCTAB_DEFNETGROUP ;
	    }
	    aep->machine.std = (char *) machine ;
	    aep->username.std = (char *) username ;
	    aep->password.std = (char *) password ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (entry_load) */

static int entry_mat2(acctab_ent *e1p,acctab_ent *e2p) noex {

#ifdef	OPTIONAL
	if (! part_match(&e1p->netgroup,e2p->netgroup.std))
	    return false ;
#endif

	if (! part_match(&e1p->machine,e2p->machine.std))
	    return false ;

	if (! part_match(&e1p->username,e2p->username.std))
	    return false ;

	if (! part_match(&e1p->password,e2p->password.std))
	    return false ;

	return true ;
}
/* end subroutine (entry_mat2) */

static int entry_mat3(acctab_ent *e1p,acctab_ent *e2p) noex {

	if (! part_match(&e1p->netgroup,e2p->netgroup.std))
	    return false ;

	if (! part_match(&e1p->machine,e2p->machine.std))
	    return false ;

	if (! part_match(&e1p->username,e2p->username.std))
	    return false ;

	if (! part_match(&e1p->password,e2p->password.std))
	    return false ;

	return true ;
}
/* end subroutine (entry_mat3) */

static int part_start(PARTTYPE *pp) noex {
	int		rs = SR_FAULT ;
	if (pp) {
	    rs = SR_OK ;
	    pp->type = 0 ;
	    pp->std = nullptr ;
	    pp->rgx = nullptr ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (part_start) */

/* p1 gets loaded up (from p2) */
static int part_copy(PARTTYPE *p1p,PARTTYPE *p2p) noex {
	int		rs = SR_OK ;
	cchar		*cp ;

	memclear(p1p) ;

	p1p->type = p2p->type ;
	if (p2p->std != nullptr) {
	    if ((rs = uc_mallocstrw(p2p->std,-1,&cp)) >= 0) {
	        p1p->std = cp ;
	    }
	} /* end if (copied STD) */

#if	ACCTAB_REGEX

	if (rs >= 0) {
	    if (p2p->rgx != nullptr) {
	        if ((rs = uc_mallocstrw(p2p->rgx,p2p->rgxlen,&cp)) >= 0) {
	            p1p->rgx = cp ;
	            p1p->rgxlen = p2p->rgxlen ;
	        }
	    } /* end if (copied RGX) */
	} /* end if (ok) */

	if (rs < 0) {
	    if (p1p->std != nullptr) {
	        uc_free(p1p->std) ;
	        p1p->std = nullptr ;
	    }
	}

#endif	/* ACCTAB_REGEX */

	return rs ;
}
/* end subroutine (part_copy) */

static int part_compile(PARTTYPE *pp,cchar *sp,int sl) noex {
	int		rs ;
	cchar		*cp ;

	pp->type = 0 ;
	pp->std = nullptr ;
	pp->rgx = nullptr ;
	pp->rgxlen = 0 ;

	if ((rs = uc_mallocstrw(sp,sl,&cp)) >= 0) {
	    pp->std = cp ;

#if	ACCTAB_REGEX
	    if ((rs >= 0) && ((type = parttype(pp->std)) > PARTTYPE_STD)) {
	        char	*rgxbuf ;
	        char	*rp ;

	        if ((rs = uc_malloc(ACCTAB_RGXLEN,&rgxbuf)) >= 0) {

	            rp = compile(pp->std,rgxbuf,(rgxbuf + ACCTAB_RGXLEN)) ;

	            if (rp != nullptr) {
	                pp->rgx = rp ;
	                pp->rgxlen = ACCTAB_RGXLEN ;
	                pp->type = type ;
	            } else {
	                rs = (regerrno == 0) ? SR_NOMEM : SR_INVALID ;
	                uc_free(rgxbuf) ;
	            }

	        } /* end if (got memory) */

	    } /* end if (RGX indicated) */
#endif /* ACCTAB_REGEX */

	} /* end if (m-a) */

	return rs ;
}
/* end subroutine (part_compile) */

static int part_match(PARTTYPE *pp,cchar *s) noex {
	int		sl, sl1, sl2 ;
	int		f = false ;
	cchar	*cp ;

	if (pp->std == nullptr) return true ;

	if (s == nullptr)
	    return false ;

	if (pp->rgx == nullptr) {

	    f = (strcmp(pp->std,s) == 0) ;
	    if (! f) {
	        f = starmat(pp->std,s) ;
	    }

	} else {

	    if (pp->type == 1) {

	        if ((cp = strchr(pp->std,'*')) != nullptr) {

	            f = false ;
	            if (strncmp(s,pp->std,(cp - pp->std)) == 0) {

	                cp += 1 ;
	                sl1 = strlen(s) ;

	                sl2 = strlen(pp->std) ;

	                sl = pp->std + sl2 - cp ;

	                f = (strncmp(s + sl1 - sl,cp,sl) == 0) ;

	            } /* end if */

	        } else {
	            f = (strcmp(pp->std,s) == 0) ;
	        }

	    } else {

#if	ACCTAB_REGEX
	        f = advance(pp->rgx,s) ;
#else
	        f = false ;
#endif

	        if (! f) {
	            f = starmat(pp->std,s) ;
	        }

	    } /* end if */

	} /* end if (STD or RGX) */

	return f ;
}
/* end subroutine (part_match) */

static int part_finish(PARTTYPE *pp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    rs1 = freeit(&pp->std) ;
	    if (rs >= 0) rs = rs1 ;
	}
	{
	    rs1 = freeit(&pp->rgx) ;
	    if (rs >= 0) rs = rs1 ;
	}
	pp->rgxlen = 0 ;
	return rs ;
}
/* end subroutine (part_finish) */

static int freeit(cchar **pp) noex {
	int		rs = SR_OK ;
	if (*pp != nullptr) {
	    rs = uc_free(*pp) ;
	    *pp = nullptr ;
	}
	return rs ;
}
/* end subroutine (freeit) */

/* compare just the 'netgroup' part of entries (used for sorting) */
static int vcmpent(cvoid **v1pp,cvoid **v2pp) noex {
	acctab_ent	*e1p = (acctab_ent *) *v1pp ;
	acctab_ent	*e2p = (acctab_ent *) *v2pp ;
	int		rc = 0 ;
	if (e1p || e2p) {
	    rc = +1 ;
	    if (e1p) {
		rc = -1 ;
	        if (e2p) {
		    cchar	*s1 = e1p->netgroup.std ;
		    cchar	*s2 = e2p->netgroup.std ;
	    	    rc = strcmp(s1,s2) ;
		}
	    }
	}
	return rc ;
}
/* end subroutine (vcmpent) */

/* determine the type of this string part */
static int parttype(cchar *s) noex {
	int		type = PARTTYPE_STD ;
	bool		f = false ;
	for ( ; *s ; s += 1) {
	    cint	ch = MKCHAR(*s) ;
	    f = f || isalnumlatin(ch) ;
	    f = f || (*s == '-') ;
	    f = f || (*s == '_') ;
	    if ((! f) && (*s == '*')) {
	        type = PARTTYPE_RGX ;
	        f = true ;
	    }
	    if (! f) break ;
	} /* end for */
	return (*s == '\0') ? type : PARTTYPE_UNKNOWN ;
}
/* end subroutine (parttype) */


