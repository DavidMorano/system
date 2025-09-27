/* acctab SUPPORT */
/* charset=ISO8859-1 */
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
#include	<climits>		/* |UINT_MAX| */
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strchr(3c)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<regex>			/* C++ REGEX; currently unused */
#include	<usystem.h>
#include	<mallocxx.h>
#include	<absfn.h>
#include	<linebuffer.h>
#include	<bfile.h>
#include	<field.h>
#include	<fieldterminit.hh>
#include	<sfx.h>
#include	<vecobj.h>
#include	<vecstr.h>
#include	<mkpathx.h>
#include	<char.h>
#include	<mkchar.h>
#include	<ischarx.h>
#include	<isnot.h>
#include	<ismatstar.h>
#include	<localmisc.h>

#include	"acctab.h"

import libutil ;

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

#ifndef	CF_REGEX
#define	CF_REGEX	0		/* stupid UNIX® REGEX? (buggy) */
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
	        if ((op->stdalp = new(nothrow) vecobj) != np) {
	            if ((op->rgxalp = new(nothrow) vecobj) != np) {
			rs = SR_OK ;
	            } /* end if (new-vecobj) */
		    if (rs < 0) {
		        delete op->stdalp ;
		        op->stdalp = nullptr ;
		    }
	        } /* end if (new-vecobj) */
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

static int	entry_start(acctab_ent *,int = -1) noex ;
static int	entry_tmpload(acctab_ent *,cc *,cc *,cc *,cc *) noex ;
[[maybe_unused]] static int	entry_addnet(acctab_ent *,cchar *,int) noex ;
[[maybe_unused]] static int	entry_addmac(acctab_ent *,cchar *,int) noex ;
[[maybe_unused]] static int	entry_adduse(acctab_ent *,cchar *,int) noex ;
[[maybe_unused]] static int	entry_addpas(acctab_ent *,cchar *,int) noex ;
static int	entry_addx(acctab_ent *,int,cchar *,int) noex ;
static int	entry_mat2(acctab_ent *,acctab_ent *) noex ;
static int	entry_mat3(acctab_ent *,acctab_ent *) noex ;
static int	entry_release(acctab_ent *) noex ;
static int	entry_finish(acctab_ent *) noex ;

static int	file_start(ACCTAB_FI *,cchar *) noex ;
static int	file_changed(ACCTAB_FI *,custime) noex ;
static int	file_finish(ACCTAB_FI *) noex ;

static int	part_start(acctab_pa *) noex ;
static int	part_load(acctab_pa *,cchar *,int = -1) noex ;
[[maybe_unused]] static int	part_copy(acctab_pa *,acctab_pa *) noex ;
[[maybe_unused]] static int	part_compile(acctab_pa *,cchar *,int) noex ;
static int	part_match(acctab_pa *,cchar *) noex ;
static int	part_release(acctab_pa *) noex ;
static int	part_finish(acctab_pa *) noex ;

static int	parttype(cchar *) noex ;

extern "C" {
    static int	vcmpent(cvoid **,cvoid **) noex ;
}

static int	freeit(cchar **) noex ;

static charp	compile(cchar *,char *,char *) noex ;
static int	advance(cchar *,cchar *) noex ;


/* local varaibles */

static int	regerrno ;		/* TODO - get rid of this stuff! */

constexpr char		gterms[] = { /* BS HT VT FF SP # : */
	0x00, 0x1B, 0x00, 0x00,
	0x09, 0x00, 0x00, 0x04,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ;

constexpr char		aterms[] = { /* BS HT VT FF SP # */
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
	    cint	sz = szof(ACCTAB_FI) ;
	    cint	vn = 10 ;
	    int		vo = VECOBJ_OREUSE ;
	    op->checktime = getustime ;
	    if ((rs = vecobj_start(op->flp,sz,vn,vo)) >= 0) {
		cint	vsz = szof(acctab_ent) ;
	        vo = VECOBJ_OSORTED ;
	        if ((rs = vecobj_start(op->stdalp,vsz,vn,vo)) >= 0) {
	            vo = VECOBJ_OCOMPACT ;
	            if ((rs = vecobj_start(op->rgxalp,vsz,vn,vo)) >= 0) {
	                op->magic = ACCTAB_MAGIC ;
	                if (fname) {
	                    rs = acctab_fileadd(op,fname) ;
	                }
	                if (rs < 0) {
	                    op->magic = 0 ;
	                    vecobj_finish(op->rgxalp) ;
	                }
	            } /* end if (rgx) */
	            if (rs < 0) {
	                vecobj_finish(op->stdalp) ;
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
	        rs1 = vecobj_finish(op->stdalp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->rgxalp) {
	        rs1 = vecobj_finish(op->rgxalp) ;
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
	    cchar	*fn{} ;
	    if (absfn afo ; (rs = afo.start(fname,-1,&fn)) >= 0) {
		if (ACCTAB_FI fe ; (rs = file_start(&fe,fn)) >= 0) {
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
	        /* load up a fake entry for comparison purposes */
	        if (acctab_ent ae ; (rs = entry_start(&ae)) >= 0) {
	            if ((rs = entry_tmpload(&ae,ng,ma,un,pw)) >= 0) {
	        	vecobj_cur	cur ;
	        	acctab_ent	*aep ;
	        	vecobj		*slp ;
	        	void		*vp{} ; /* used in two blocks below */
		        /* search the STD entries first */
	                slp = op->stdalp ;
	                if ((rs = slp->curbegin(&cur)) >= 0) {
		            cauto	vif = vecobj_curfetch ;
			    cauto	vcf = vcmpent ;
	                    while ((rs1 = vif(slp,&ae,&cur,vcf,&vp)) >= 0) {
		                aep = entp(vp) ;
	                        if (vp) {
	                            f = entry_mat2(aep,&ae) ;
	                            if (f) break ;
	                        }
	                    } /* end while */
	                    if ((rs >= 0) && (rs1 != rsn)) rs = rs1 ;
	                    rs1 = slp->curend(&cur) ;
			    if (rs >= 0) rs = rs1 ;
	                } /* end if (vecobj-cur) */
		        /* search the RGX entries (if necessary) */
	                if ((rs == rsn) || (! f)) {
	                    slp = op->rgxalp ;
			    auto vll = [slp] (int i,void **vpp) -> int {
				return slp->get(i,vpp) ;
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
		    } /* end if (entry_tmpload) */
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
	int		rs ;
	int		rs1 ;
	int		rs2 ;
	int		f = false ;
	if ((rs = acctab_magic(op,nlp,mlp,un,pw)) >= 0) {
	    cchar	*ngp ; /* netgroup pointer */
	    cchar	*mp ; /* machine pointer */
	    cauto	vsg = vecstr_get ;
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
	    vecobj	*slp = &op->e ;
	    cchar	*sp ;
	    bool	f = false ;
	    void	*vp{} ;
	    for (i = 0 ; slp->get(i,&vp) >= 0 ; i += 1) {
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
	    vecobj	*slp ;
	    acctab_ent	*aep ;
	    int		j ; /* used-multiple */
	    cauto	vig = vecobj_get ;
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
    	vecobj		*flp = op->flp ;
	int		rs = SR_OK ;
	int		rs1 ;
	void		*vp{} ;
	for (int i = 0 ; flp->get(i,&vp) >= 0 ; i += 1) {
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
    	vecobj		*flp = op->flp ;
	int		rs = SR_OK ;
	int		c_changed = false ;
	void		*vp{} ;
	for (int i = 0 ; flp->get(i,&vp) >= 0 ; i += 1) {
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
	    rs = vecobj_sort(op->stdalp,vcmpent) ;
	} /* end if (something changed) */
	op->checktime = dt ;
	return (rs >= 0) ? c_changed : rs ;
}
/* end subroutine (acctab_filechecks) */

namespace {
    struct parser {
	acctab		*op ;
	ACCTAB_FI	*fep ;
	acctab_ent	se ;
	int		fi ;
	bool		fent = false ;
	parser(acctab *p,int i,ACCTAB_FI *fp) noex : op(p), fi(i) { 
	    fep = fp ;
	} ;
	operator int () noex ;
	int parseln(cchar *,int) noex ;
    } ; /* end struct (parser) */
}

static int acctab_fileparse(acctab *op,int fi) noex {
    	int		rs ;
	int		c = 0 ;
	if (void *vp{} ; (rs = vecobj_get(op->flp,fi,&vp)) >= 0) {
	    ACCTAB_FI	*fep = filep(vp) ;
	    if (vp) {
    	        parser	po(op,fi,fep) ;
		if ((rs = po) >= 0) {
		    c = rs ;
		} else {
		    acctab_filedump(op,fi) ;
		}
	    } else {
		rs = SR_BUGCHECK ;
	    }
	} /* end if (vecobj_get) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (acctab_fileparse) */

parser::operator int () noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
    	if (linebuffer lb ; (rs = lb.start) >= 0) {
	    bfile	afile, *afp = &afile ;
	    if ((rs = bopen(afp,fep->fname,"r",0)) >= 0) {
	        if (USTAT sb ; (rs = bcontrol(afp,BC_STAT,&sb)) >= 0) {
		    if (sb.st_size <= INT_MAX) {
		        if (sb.st_mtime > fep->mtime) {
			    fep->dev = sb.st_dev ;
			    fep->ino = sb.st_ino ;
			    fep->mtime = sb.st_mtime ;
			    fep->len = intsat(sb.st_size) ;
			    while ((rs = breadln(afp,lb.lbuf,lb.llen)) > 0) {
			        cchar	*cp{} ;
			        char	*lp = lb.lbuf ;
			        if (int cl ; (cl = sfcontent(lp,rs,&cp)) > 0) {
				     rs = parseln(cp,cl) ;
				     c += rs ;
			        } /* end if (content) */
			    } /* end while */
			    if ((rs >= 0) && fent) {
				if ((rs = acctab_entadd(op,&se)) >= 0) {
				    rs = entry_release(&se) ;
				    fent = false ;
				}
			    }
			    if ((rs < 0) && fent) {
				fent = false ;
				entry_finish(&se) ;
			    }
		        } /* end if (file older) */
		    } else {
			rs = SR_TOOBIG ;
		    }
	        } /* end if (bcontrol) */
		rs1 = bclose(afp) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (bfile-acctab) */
	    rs1 = lb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (linebuffer) */
	return (rs >= 0) ? c : rs ;
}
/* end method (parser::operator) */

int parser::parseln(cchar *lp,int ll) noex {
    	cint		rsn = SR_NOTFOUND ;
    	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (field fsb ; (rs = fsb.start(lp,ll)) >= 0) {
	    if (cchar *fp{} ; (rs = fsb.get(gterms,&fp)) > 0) {
		int	fl = rs ;
	        if (fsb.term == ':') {
		    if (fent) {
	                if ((rs = acctab_entadd(op,&se)) >= 0) {
			    if ((rs = entry_release(&se)) >= 0) {
				c += 1 ;
				fent = false ;
			    } 
			} 
			if (rs < 0) {
			    fent = false ;
			    rs1 = entry_finish(&se) ;
			    if (rs >= 0) rs = rs1 ;
			} /* end if (error) */
		    } /* end if (had an entry already) */
		    if (rs >= 0) {
	                if ((rs = entry_start(&se,fi)) >= 0) {
			    {
			        fent = true ;
			        rs = entry_addnet(&se,fp,fl) ;
			    }
			    if (rs < 0) {
				fent = false ;
				entry_finish(&se) ;
			    }
		        } /* end if (entry_start) */
		    } /* end if (ok) */
	        } /* end if (new netgroup) */
		if ((rs >= 0) && fent && (fsb.term != '#')) {
		    for (int i = 0 ; (rs >= 0) && (i < 3) ; i += 1) {
	                if ((rs = fsb.get(aterms,&fp)) > 0) {
			    rs = entry_addx(&se,i,fp,rs) ;
			} /* end if (field_get) */
			if (fsb.term == '#') break ;
	            } /* end if */
		    if (rs == rsn) {
			rs = SR_OK ;
		    }
	        } /* end if (got something) */
	    } else if (rs == rsn) {
		rs = SR_OK ;
	    } /* end if (got netgroup) */
	    rs1 = fsb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (field) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (parser::parseln) */

static int acctab_entadd(acctab *op,acctab_ent *sep) noex {
	int		rs ;
	if (parttype(sep->netgroup.patstd) == PARTTYPE_STD) {
	    rs = vecobj_add(op->stdalp,sep) ;
	} else {
	    rs = vecobj_add(op->rgxalp,sep) ;
	}
	return rs ;
}
/* end subroutine (acctab_entadd) */

static int acctab_filedump(acctab *op,int fi) noex {
	vecobj		*slp ;
	cint		rsn = SR_NOTFOUND ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		rs2 ;
	for (int j = 0 ; j < 2 ; j += 1) {
	    slp = (j == 0) ? op->stdalp : op->rgxalp ;
	    void	*vp{} ;
	    for (int i = 0 ; (rs2 = slp->get(i,&vp)) >= 0 ; i += 1) {
	        acctab_ent	*sep = entp(vp) ;
	        if (vp) {
	            if ((sep->fi == fi) || (fi < 0)) {
			{
	                    rs1 = entry_finish(sep) ;
	                    if (rs >= 0) rs = rs1 ;
			}
			{
	                    rs1 = slp->del(i--) ;
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
	    vecobj	*slp ;
	    cauto	vig = vecobj_get ;
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
	if ((rs = uc_stat(fep->fname,&sb)) >= 0) {
	    if (sb.st_mtime > fep->mtime) {
	        f = ((dt - sb.st_mtime) >= intcheck) ;
	    }
	} else if (isNotPresent(rs)) {
	    rs = SR_OK ;
	}
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (file_changed) */

static int entry_start(acctab_ent *sep,int fi) noex {
	int		rs = SR_FAULT ;
	if (sep) {
	    if ((rs = memclear(sep)) >= 0) {
	        sep->fi = fi ;
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

static int entry_release(acctab_ent *ep) noex {
    	int		rs = SR_FAULT ;
	int		rs1 ;
	if (ep) {
	    rs = SR_OK ;
	    if (ep->fi >= 0) {
		{
	            rs1 = part_release(&ep->netgroup) ;
	            if (rs >= 0) rs = rs1 ;
		}
		{
	            rs1 = part_release(&ep->machine) ;
	            if (rs >= 0) rs = rs1 ;
		}
		{
	            rs1 = part_release(&ep->username) ;
	            if (rs >= 0) rs = rs1 ;
		}
		{
	            rs1 = part_release(&ep->password) ;
	            if (rs >= 0) rs = rs1 ;
		}
	        ep->fi = -1 ;
	    } /* end if */
	} /* end if (non-null) */
	return rs ;
}

static int entry_tmpload(acctab_ent *aep,cc *n,cc *m,cc *u,cc *p) noex {
	int		rs = SR_FAULT ;
	if (aep) {
	    rs = SR_OK ;
	    aep->netgroup.patstd = charp(n) ;
	    if ((n == nullptr) || (n[0] == '\0')) {
	        aep->netgroup.patstd = ACCTAB_DEFNETGROUP ;
	    }
	    aep->machine.patstd = charp(m) ;
	    aep->username.patstd = charp(u) ;
	    aep->password.patstd = charp(p) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (entry_tmpload) */

static int entry_addnet(acctab_ent *aep,cc *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (aep) {
	    rs = part_load(&aep->netgroup,sp,sl) ;
	}
	return rs ;
}
/* end subroutine (entry_addnet) */

static int entry_addmac(acctab_ent *aep,cc *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (aep) {
	    rs = part_load(&aep->machine,sp,sl) ;
	}
	return rs ;
}
/* end subroutine (entry_addmac) */

static int entry_adduse(acctab_ent *aep,cc *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (aep) {
	    rs = part_load(&aep->username,sp,sl) ;
	}
	return rs ;
}
/* end subroutine (entry_adduse) */

static int entry_addpas(acctab_ent *aep,cc *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (aep) {
	    rs = part_load(&aep->password,sp,sl) ;
	}
	return rs ;
}
/* end subroutine (entry_addpas) */

static int entry_addx(acctab_ent *aep,int idx,cc *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (aep) {
	    switch (idx) {
	    case 0:
	       rs = part_load(&aep->machine,sp,sl) ;
	       break ;
	    case 1:
	       rs = part_load(&aep->username,sp,sl) ;
	       break ;
	    case 2:
	       rs = part_load(&aep->password,sp,sl) ;
	       break ;
	    default:
	       rs = SR_BUGCHECK ;
	       break ;
	    } /* end switch */
	}
	return rs ;
}
/* end subroutine (entry_addx) */

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
	    pp->patrgxlen = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (part_start) */

static int part_load(PARTTYPE *pp,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (pp && sp) {
	    if (cchar *cp{} ; (rs = uc_mallocstrw(sp,sl,&cp)) >= 0) {
		pp->patstd = cp ;
		pp->type = PARTTYPE_STD	;
	    }
	}
	return rs ;
}
/* end subroutine (part_load) */

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
	                f = ismatstar(pp->patstd,s) ;
	            }
	        } else {
	            if (pp->type == 1) {
			cnullptr	np{} ;
	                if (cchar *tp ; (tp = strchr(pp->patstd,'*')) != np) {
		            int		sl ;
	                    f = false ;
	                    if (strncmp(s,pp->patstd,(tp - pp->patstd)) == 0) {
	                        cint	sl1 = lenstr(s) ;
	                        cint	sl2 = lenstr(pp->patstd) ;
	                        tp += 1 ;
	                        sl = intconv(pp->patstd + sl2 - tp) ;
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
	                    f = ismatstar(pp->patstd,s) ;
	                }
	            } /* end if */
	        } /* end if (STD or RGX) */
	    } /* end if (non-null) */
	} /* end if (non-null) */
	return f ;
}
/* end subroutine (part_match) */

static int part_release(PARTTYPE *pp) noex {
    	int		rs = SR_FAULT ;
	if (pp) {
	    rs = memclear(pp) ;
	}
	return rs ;
}
/* end subroutine (part_release) */

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

/* dummy subroutine; one of the old SysV REGEX functions (not standardized) */
static charp compile(cchar *,char *,char *) noex {
	return nullptr ;
}

/* dummy subroutine; one of the old SysV REGEX functions (not standardized) */
static int advance(cchar *,cchar *) noex {
	return 0 ;
}


