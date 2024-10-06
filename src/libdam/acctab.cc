/* acctab SUPPORT */
/* lang=C++20 */

/* perform access-table file related functions */
/* version %I% last-modified %G% */

#define	CF_REGEX	0		/* stupid old UNIX® REGEX? (garbage) */

/* revision history:

	= 1998-06-01, David A­D­ Morano
	This code was originally written (and largely forgotten).

	= 2024-10-05, David A­D­ Morano
	Comment only.  I am reviewing this code (for the fun of
	it).  This code is quite old.  I think it (at least some
	of it) dates from about 1996 or even before.  From the date
	above, it looks like it all came together in 1998.  OK, but
	this code contains some very old coding styles and elements.
	How do I know?  I know a little bit about my own garbage
	code from the past.  I am not sure that the attempt at
	incuding some REGEX functionality ever really worked
	(properly).  But the old UNIX® library code for the old
	REGEX routines used below seems to be long gone, and has
	been so for some decades now already.  I thought about
	refactoring this code to use the C++11 REGEX library code,
	but in the end it does not seem to require or substantially
	benefit from such.  So I leave this code as is: with the
	old (garbage) REGEX compiled out.  Note also, that the old
	garbage REGEX code (of which there appears to be no trace
	left in the world) was also never thread-safe.  That alone
	should have disqualified it from ever being included here
	at all.  So that REGEX garbage has not been missed for these
	last 26+ years, so I do not think that it is going to mised
	going forward.  Regards.

*/

/* Copyright © 1998,2024 David A­D­ Morano.  All rights reserved. */

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
#include	<cstring>		/* |strlen(3c)| + |strchr(3c)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<regex>			/* C++ REGEX; currently unused */
#include	<usystem.h>
#include	<mallocxx.h>
#include	<absfn.h>
#include	<bfile.h>
#include	<field.h>
#include	<fieldterminit.hh>
#include	<vecobj.h>
#include	<vecitem.h>
#include	<vecstr.h>
#include	<mkpathx.h>
#include	<starmat.h>
#include	<char.h>
#include	<mkchar.h>
#include	<ischarx.h>
#include	<isnot.h>
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

#ifndef	CF_REGEX
#define	CF_REGEX	0		/* stupid UNIX REGEX? (buggy) */
#endif


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
	        if ((op->stdalp = new(nothrow) vecitem) != np) {
	            if ((op->rgxalp = new(nothrow) vecitem) != np) {
			rs = SR_OK ;
	            } /* end if (new-vecitem) */
		    if (rs < 0) {
		        delete op->stdalp ;
		        op->stdalp = nullptr ;
		    }
	        } /* end if (new-vecitem) */
		if (rs < 0) {
		    delete op->flp ;
		    op->flp = nullptr ;
		}
	    } /* end if (new-vecobj) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (acctab_ctor) */

static int acctab_dtor(acctab *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->rgxalp) {
		delete op->rgxalp ;
		op->rgxalp = nullptr ;
	    }
	    if (op->stdalp) {
		delete op->stdalp ;
		op->rgxalp = nullptr ;
	    }
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
static int	acctab_fileparse(acctab *,int) noex ;
static int	acctab_filedump(acctab *,int) noex ;
static int	acctab_filedel(acctab *,int) noex ;
static int	acctab_filechecks(acctab *,time_t) noex ;
static int	acctab_entadd(acctab *,acctab_ent *) noex ;
static int	acctab_entfins(acctab *) noex ;

static int	entry_start(acctab_ent *) noex ;
static int	entry_load(acctab_ent *,cchar *,cchar *,cchar *,cchar *) noex ;
static int	entry_mat2(acctab_ent *,acctab_ent *) noex ;
static int	entry_mat3(acctab_ent *,acctab_ent *) noex ;
static int	entry_finish(acctab_ent *) noex ;

static int	file_start(ACCTAB_FI *,cchar *) noex ;
static int	file_changed(ACCTAB_FI *,custime) noex ;
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

static charp	compile(cchar *,char *,char *) noex ;
static int	advance(cchar *,cchar *) noex ;


/* local varaibles */

static int	regerrno ;

constexpr cchar		gterms[] = { /* BS HT VT FF SP # : */
	0x00, 0x1B, 0x00, 0x00,
	0x09, 0x00, 0x00, 0x04,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ;

constexpr cchar		aterms[] = { /* BS HT VT FF SP # */
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

constexpr bool		f_regex = CF_REGEX ;


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
	        if ((rs = vecitem_start(op->stdalp,vn,vo)) >= 0) {
	            vo = VECITEM_OCOMPACT ;
	            if ((rs = vecitem_start(op->rgxalp,vn,vo)) >= 0) {
	                op->magic = ACCTAB_MAGIC ;
	                if (fname) {
	                    rs = acctab_fileadd(op,fname) ;
	                }
	                if (rs < 0) {
	                    op->magic = 0 ;
	                    vecitem_finish(op->rgxalp) ;
	                }
	            } /* end if (rgx) */
	            if (rs < 0) {
	                vecitem_finish(op->stdalp) ;
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
	        rs1 = acctab_entfins(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = acctab_filefins(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->stdalp) {
	        rs1 = vecitem_finish(op->stdalp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->rgxalp) {
	        rs1 = vecitem_finish(op->rgxalp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->flp) {
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
	int		rs1 ;
	int		fi = 0 ;
	if ((rs = acctab_magic(op,fname)) >= 0) {
	    absfn	afo ;
	    if (cchar *fn{} ; (rs = afo.start(fname,-1,&fn)) >= 0) {
		ACCTAB_FI	fe ;
	        if ((rs = file_start(&fe,fn)) >= 0) {
		    if ((rs = vecobj_add(op->flp,&fe)) >= 0) {
		        fi = rs ;
		        rs = acctab_fileparse(op,fi) ;
		        if (rs < 0) {
			    acctab_filedel(op,fi) ;
		        }
		    } /* end if (vecobj_add) */
		    if (rs < 0) {
		        file_finish(&fe) ;
		    }
	        } /* end if (file_start) */
		rs1 = afo.finish ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (absfn) */
	} /* end if (magic) */
	return (rs >= 0) ? fi : rs ;
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
	        rs = acctab_filechecks(op,dt) ;
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
	                slp = op->stdalp ;
	                if ((rs = vecitem_curbegin(slp,&cur)) >= 0) {
		            auto	vif = vecitem_fetch ;
			    auto	vcf = vcmpent ;
	                    while ((rs1 = vif(slp,&ae,&cur,vcf,&vp)) >= 0) {
		                aep = entp(vp) ;
	                        if (vp) {
	                            f = entry_mat2(aep,&ae) ;
	                            if (f) break ;
	                        }
	                    } /* end while */
	                    if ((rs >= 0) && (rs1 != rsn)) rs = rs1 ;
	                    rs1 = vecitem_curend(slp,&cur) ;
			    if (rs >= 0) rs = rs1 ;
	                } /* end if (vecitem-cur) */
		        /* search the RGX entries (if necessary) */
	                if ((rs == rsn) || (! f)) {
	                    slp = op->rgxalp ;
			    auto	vll = [slp] (int i,void **vpp) -> int {
				return vecitem_get(slp,i,vpp) ;
			    } ;
	                    for (int i = 0 ; (rs1 = vll(i,&vp)) >= 0 ; i += 1) {
		    		aep = entp(vp) ;
	            		if (vp) {
				    f = entry_mat3(aep,&ae) ;
	                            if (f) break ;
	                        }
	                    } /* end for (looping through entries) */
	                    if ((rs >= 0) && (rs1 != rsn)) rs = rs1 ;
	                } /* end if (comparing RGX entries) */
		    } /* end if (entry_load) */
		    rs1 = entry_finish(&ae) ;
		    if (rs >= 0) rs = rs1 ;
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
	        slp = op->stdalp ;
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
	        slp = op->rgxalp ;
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
	custime		dt = getustime ;
	int		rs ;
	if ((rs = acctab_magic(op)) >= 0) {
	    if ((dt - op->checktime) > intcheck) {
	        rs = acctab_filechecks(op,dt) ;
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
	    if (vp) {
	        rs1 = file_finish(fep) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end for */
	return rs ;
}
/* end subroutine (acctab_filefins) */

static int acctab_filechecks(acctab *op,time_t dt) noex {
	int		rs = SR_OK ;
	int		c_changed = false ;
	void		*vp{} ;
	for (int i = 0 ; vecobj_get(op->flp,i,&vp) >= 0 ; i += 1) {
	    ACCTAB_FI	*fep = (ACCTAB_FI *) vp ;
	    if (vp) {
		if ((rs = file_changed(fep,dt)) > 0) {
	            acctab_filedump(op,i) ;
	            if ((rs = acctab_fileparse(op,i)) >= 0) {
	                c_changed += 1 ;
		    }
		} /* end if (file_changed) */
	    } /* end if (non-null) */
	    if (rs < 0) break ;
	} /* end for */
	if ((rs >= 0) && c_changed) {
	    rs = vecitem_sort(op->stdalp,vcmpent) ;
	} /* end if (something changed) */
	op->checktime = dt ;
	return (rs >= 0) ? c_changed : rs ;
}
/* end subroutine (acctab_filechecks) */

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

	                rs = acctab_entadd(op,&se) ;

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

static int acctab_entadd(acctab *op,acctab_ent *sep) noex {
	int		rs ;
	if (parttype(sep->netgroup.patstd) == PARTTYPE_STD) {
	    rs = vecitem_add(op->stdalp,sep,sizeof(acctab_ent)) ;
	} else {
	    rs = vecitem_add(op->rgxalp,sep,sizeof(acctab_ent)) ;
	}
	return rs ;
}
/* end subroutine (acctab_entadd) */

static int acctab_filedump(acctab *op,int fi) noex {
	vecitem		*slp ;
	cint		rsn = SR_NOTFOUND ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		rs2 ;
	for (int j = 0 ; j < 2 ; j += 1) {
	    slp = (j == 0) ? op->stdalp : op->rgxalp ;
	    void	*vp{} ;
	    for (int i = 0 ; (rs2 = vecitem_get(slp,i,&vp)) >= 0 ; i += 1) {
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
	    if ((rs >= 0) && (rs2 != rsn)) rs = rs2 ;
	} /* end for (looping on the different types of entries) */
	return rs ;
}
/* end subroutine (acctab_filedump) */

static int acctab_filedel(acctab *op,int fi) noex {
	int		rs ;
	int		rs1 ;
	if (void *vp{} ; (rs = vecobj_get(op->flp,fi,&vp)) >= 0) {
	    ACCTAB_FI	*fep = (ACCTAB_FI *) vp ;
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
/* end subroutine (acctab_filedel) */

static int acctab_entfins(acctab *op) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rs2 ;
	if (op) {
	    vecitem	*slp ;
	    auto	vig = vecitem_get ;
	    rs = SR_OK ;
	    for (int j = 0 ; j < 2 ; j += 1) {
	        slp = (j == 0) ? op->stdalp : op->rgxalp ;
		void	*vp{} ;
	        for (int i = 0 ; (rs2 = vig(slp,i,&vp)) >= 0 ; i += 1) {
	    	    acctab_ent	*sep = entp(vp) ;
	            if (vp) {
	                rs1 = entry_finish(sep) ;
	                if (rs >= 0) rs = rs1 ;
	            }
	        } /* end for */
		if ((rs >= 0) && (rs2 != rsn)) rs = rs2 ;
	    } /* end for */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (acctab_entfins) */

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

static int file_changed(ACCTAB_FI *fep,custime dt) noex {
	USTAT		sb ;
	int		rs ;
	int		f = false ;
	if ((rs = u_stat(fep->fname,&sb)) >= 0) {
	    if (sb.st_mtime > fep->mtime) {
	        f = ((dt - sb.st_mtime) >= intcheck) ;
	    }
	} else if (isNotPresent(rs)) {
	    rs = SR_OK ;
	}
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (file_changed) */

static int entry_start(acctab_ent *sep) noex {
	int		rs = SR_FAULT ;
	if (sep) {
	    if ((rs = memclear(sep)) >= 0) {
	        sep->fi = -1 ;
	        if ((rs = part_start(&sep->netgroup)) >= 0) {
	    	    if ((rs = part_start(&sep->machine)) >= 0) {
	    	        if ((rs = part_start(&sep->username)) >= 0) {
			    {
	    		        rs = part_start(&sep->password) ;
			    }
			    if (rs < 0) {
			        part_finish(&sep->username) ;
			    }
			}
			if (rs < 0) {
			    part_finish(&sep->machine) ;
			}
		    }
		    if (rs < 0) {
			part_finish(&sep->netgroup) ;
		    }
		}
	    } /* end if (memclear) */
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
		{
	            rs1 = part_finish(&sep->netgroup) ;
	            if (rs >= 0) rs = rs1 ;
		}
		{
	            rs1 = part_finish(&sep->machine) ;
	            if (rs >= 0) rs = rs1 ;
		}
		{
	            rs1 = part_finish(&sep->username) ;
	            if (rs >= 0) rs = rs1 ;
		}
		{
	            rs1 = part_finish(&sep->password) ;
	            if (rs >= 0) rs = rs1 ;
		}
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
	    aep->netgroup.patstd = charp(netgroup) ;
	    if ((netgroup == nullptr) || (netgroup[0] == '\0')) {
	        aep->netgroup.patstd = ACCTAB_DEFNETGROUP ;
	    }
	    aep->machine.patstd = charp(machine) ;
	    aep->username.patstd = charp(username) ;
	    aep->password.patstd = charp(password) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (entry_load) */

static int entry_mat2(acctab_ent *e1p,acctab_ent *e2p) noex {
	int		rs ;
	int		f = false ;
	cchar		*cp ;
	cp = e2p->machine.patstd ;
	if ((rs = part_match(&e1p->machine,cp)) > 0) {
	    cp = e2p->username.patstd ;
	    if ((rs = part_match(&e1p->username,cp)) > 0) {
	        cp = e2p->password.patstd ;
	        if ((rs = part_match(&e1p->password,cp)) > 0) {
		    f = true ;
		}
	    }
	}
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (entry_mat2) */

static int entry_mat3(acctab_ent *e1p,acctab_ent *e2p) noex {
	int		rs ;
	int		f = false ;
	cchar		*cp ;
	cp = e2p->netgroup.patstd ;
	if ((rs = part_match(&e1p->netgroup,cp)) > 0) {
	    cp = e2p->machine.patstd ;
	    if ((rs = part_match(&e1p->machine,cp)) > 0) {
	        cp = e2p->username.patstd ;
		if ((rs = part_match(&e1p->username,cp)) > 0) {
	            cp = e2p->password.patstd ;
		    if ((rs = part_match(&e1p->password,cp)) > 0) {
			f = true ;
		    }
		}
	    }
	}
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (entry_mat3) */

static int part_start(PARTTYPE *pp) noex {
	int		rs = SR_FAULT ;
	if (pp) {
	    rs = SR_OK ;
	    pp->type = 0 ;
	    pp->patstd = nullptr ;
	    pp->patrgx = nullptr ;
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
	if (p2p->patstd != nullptr) {
	    if ((rs = uc_mallocstrw(p2p->patstd,-1,&cp)) >= 0) {
	        p1p->patstd = cp ;
	    }
	} /* end if (copied STD) */
	if_constexpr (f_regex) {
	    if (rs >= 0) {
	        if (p2p->patrgx != nullptr) {
		    cint	sl = p2p->patrgxlen ;
		    cchar	*sp = p2p->patrgx ;
	            if ((rs = uc_mallocstrw(sp,sl,&cp)) >= 0) {
	                p1p->patrgx = cp ;
	                p1p->patrgxlen = p2p->patrgxlen ;
	            }
	        } /* end if (copied RGX) */
	    } /* end if (ok) */
	    if (rs < 0) {
	        if (p1p->patstd != nullptr) {
	            uc_free(p1p->patstd) ;
	            p1p->patstd = nullptr ;
	        }
	    }
	} /* end if_constexpr (f_regex) */
	return rs ;
}
/* end subroutine (part_copy) */

static int part_compile(PARTTYPE *pp,cchar *sp,int sl) noex {
	int		rs ;
	pp->type = 0 ;
	pp->patstd = nullptr ;
	pp->patrgx = nullptr ;
	pp->patrgxlen = 0 ;
	if (cchar *cp{} ; (rs = uc_mallocstrw(sp,sl,&cp)) >= 0) {
	    pp->patstd = cp ;
	    if_constexpr (f_regex) {
		cint	pt = PARTTYPE_STD ;
		int	type ;
	        if ((rs >= 0) && ((type = parttype(pp->patstd)) > pt)) {
	            char	*rgxbuf ;
	            char	*rp ;
	            if ((rs = uc_malloc(ACCTAB_RGXLEN,&rgxbuf)) >= 0) {
			cint	rlen = ACCTAB_RGXLEN ;
	                rp = compile(pp->patstd,rgxbuf,(rgxbuf + rlen)) ;
	                if (rp != nullptr) {
	                    pp->patrgx = rp ;
	                    pp->patrgxlen = ACCTAB_RGXLEN ;
	                    pp->type = type ;
	                } else {
	                    rs = (regerrno == 0) ? SR_NOMEM : SR_INVALID ;
	                    uc_free(rgxbuf) ;
	                }
	            } /* end if (got memory) */
	        } /* end if (RGX indicated) */
	    } /* end if_constexpr (f_regex) */
	} /* end if (m-a) */
	return rs ;
}
/* end subroutine (part_compile) */

static int part_match(PARTTYPE *pp,cchar *s) noex {
	int		f = true ;
	if (pp->patstd) {
	    f = false ;
	    if (s) {
	        if (pp->patrgx == nullptr) {
	            f = (strcmp(pp->patstd,s) == 0) ;
	            if (! f) {
	                f = starmat(pp->patstd,s) ;
	            }
	        } else {
	            if (pp->type == 1) {
			cnullptr	np{} ;
	                if (cchar *tp ; (tp = strchr(pp->patstd,'*')) != np) {
		            int		sl ;
	                    f = false ;
	                    if (strncmp(s,pp->patstd,(tp - pp->patstd)) == 0) {
	                        cint	sl1 = strlen(s) ;
	                        cint	sl2 = strlen(pp->patstd) ;
	                        tp += 1 ;
	                        sl = pp->patstd + sl2 - tp ;
	                        f = (strncmp(s + sl1 - sl,tp,sl) == 0) ;
	                    } /* end if */
	                } else {
	                    f = (strcmp(pp->patstd,s) == 0) ;
	                }
	            } else {
		        if_constexpr (f_regex) {
	                    f = advance(pp->patrgx,s) ;
		        } else {
	                    f = false ;
		        } /* end if_constexpr (f_regex) */
	                if (! f) {
	                    f = starmat(pp->patstd,s) ;
	                }
	            } /* end if */
	        } /* end if (STD or RGX) */
	    } /* end if (non-null) */
	} /* end if (non-null) */
	return f ;
}
/* end subroutine (part_match) */

static int part_finish(PARTTYPE *pp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (pp) {
	    rs = SR_OK ;
	    if (pp->patstd) {
	        rs1 = freeit(&pp->patstd) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (pp->patrgx) {
	        rs1 = freeit(&pp->patrgx) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    pp->patrgxlen = 0 ;
	} /* end if (non-null) */
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
		    cchar	*s1 = e1p->netgroup.patstd ;
		    cchar	*s2 = e2p->netgroup.patstd ;
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
	    cint	ch = mkchar(*s) ;
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

/* dummy subroutine; one of the old SysV REGEX functions */
static charp compile(cchar *,char *,char *) noex {
	return nullptr ;
}

/* dummy subroutine; one of the old SysV REGEX functions */
static int advance(cchar *,cchar *) noex {
	return 0 ;
}


