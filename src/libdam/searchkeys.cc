/* searchkeys SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* provides management for search-keys */
/* version %I% last-modified %G% */

#define	CF_REGPROC	0		/* compile regular processing */
#define	CF_BUILDREDUCE	1		/* try |{xx}_buildreduce()| */
#define	CF_SHORTCUT	1		/* use short-cut */

/* revision history:

	= 2009-04-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2009 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	searchkeys

	Description:
	This object manages groups of words (called 'phrases') that
	form search keys for searching a text database.  The text
	database is not our concern as we only get a call to us
	(with a word) to confirm whether all keys have been matched
	or not.  We maintain whether all of the search keys have
	been matched or not and return this indication to the caller.

*******************************************************************************/

#include	<envstandards.h>	/* must be before others */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<vecobj.h>
#include	<strpack.h>
#include	<nleadstr.h>
#include	<strwcpy.h>
#include	<hasx.h>
#include	<ischarx.h>
#include	<localmisc.h>		/* |NATURALWORDLEN| */

#include	"searchkeys.h"
#include	"naturalwords.h"
#include	"xwords.h"


/* local defines */

#define	SK		searchkeys
#define	SK_CUR		searchkeys_cur
#define	SK_POP		searchkeys_pop
#define	SK_PH		searchkeys_ph
#define	SK_KW		searchkeys_kw

#undef	BUILD
#define	BUILD		struct build

#undef	BUILD_PH
#define	BUILD_PH	struct build_phrase

#ifndef	KEYBUFLEN
#define	KEYBUFLEN	NATURALWORDLEN
#endif

#ifdef	CF_BUILDREDUCE
#define	CF_BUILDREDUCE	1		/* try '_buildreduce()' */
#endif

#ifndef	CF_SHORTCUT
#define	CF_SHORTCUT	1		/* use short-cut */
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

struct build {
	vecobj	phrases ;
} ;

struct build_phrase {
	vecobj	words ;
} ;


/* forward references */

template<typename ... Args>
static int searchkeys_ctor(SK *op,Args ... args) noex {
    	SEARCHKEYS	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->spp = new(nothrow) strpack) != np) {
		rs = SR_OK ;
	    } /* end if (new-strpack) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (searchkeys_ctor) */

static int searchkeys_dtor(SK *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->spp) {
		delete op->spp ;
		op->spp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (searchkeys_dtor) */

template<typename ... Args>
static inline int searchkeys_magic(SK *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == SEARCHKEYS_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (searchkeys_magic) */

static int searchkeys_build(SK *,cchar **) noex ;
static int searchkeys_buildadd(SK *,BUILD *,cchar *) noex ;
static int searchkeys_buildaddword(SK *,BUILD_PH *,cchar *,int) noex ;
static int searchkeys_buildphrasemat(SK *,BUILD *,BUILD_PH *) noex ;
static int searchkeys_buildmatone(SK *,BUILD *,int,BUILD_PH *) noex ;
static int searchkeys_buildreduce(SK *,BUILD *) noex ;
static int searchkeys_buildload(SK *,BUILD *) noex ;
static int searchkeys_buildfins(SK *,BUILD *) noex ;
static int searchkeys_curinc(SK *,SK_CUR *,int *,int *) noex ;

static int buildphrase_start(BUILD_PH *) noex ;
static int buildphrase_add(BUILD_PH *,cchar *,int) noex ;
static int buildphrase_count(BUILD_PH *) noex ;
static int buildphrase_getkey(BUILD_PH *,int,cchar **) noex ;
static int buildphrase_havekey(BUILD_PH *,cchar *,int) noex ;
static int buildphrase_finish(BUILD_PH *) noex ;

#if	CF_REGPROC
static int kphrase_proc(SK_PH *,int,int,cchar *,int) noex ;
#endif

static int kphrase_procxw(SK_PH *,int,int,xwords *) noex ;


/* local variables */

constexpr bool		f_buildreduce = CF_BUILDREDUCE ;
constexpr bool		f_shortcut = CF_SHORTCUT ;


/* exported variables */


/* exported subroutines */

int searchkeys_start(SK *op,cchar**qsp) noex {
	int		rs ;
	int		nphrases = 0 ;
	if ((rs = searchkeys_ctor(op,qsp)) >= 0) {
	    if ((rs = strpack_start(op->spp,0)) >= 0) {
	        if ((rs = searchkeys_build(op,qsp)) >= 0) {
	            nphrases = op->nphrases ;
	            op->magic = SEARCHKEYS_MAGIC ;
	        }
	        if (rs < 0) {
	            strpack_finish(op->spp) ;
	        }
	    } /* end if (strpack) */
	    if (rs < 0) {
		searchkeys_dtor(op) ;
	    }
	} /* end if (searchkeys_ctor) */
	return (rs >= 0) ? nphrases : rs ;
}
/* end subroutine (searchkeys_start) */

int searchkeys_finish(SK *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = searchkeys_magic(op)) >= 0) {
	    if (op->kphrases) {
	        for (int i = 0 ; i < op->nphrases ; i += 1) {
	            if (op->kphrases[i].kwords != nullptr) {
	                rs1 = uc_free(op->kphrases[i].kwords) ;
	                if (rs >= 0) rs = rs1 ;
	                op->kphrases[i].kwords = nullptr ;
	            }
	        } /* end for */
	        if (op->kphrases) {
	            rs1 = uc_free(op->kphrases) ;
	            if (rs >= 0) rs = rs1 ;
	            op->kphrases = nullptr ;
	            op->nphrases = 0 ;
	        }
	    } /* end if */
	    {
	        rs1 = strpack_finish(op->spp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = searchkeys_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (searchkeys_finish) */

int searchkeys_popbegin(SK *op,SK_POP *pop,int f_prefix) noex {
	int		rs ;
	int		n = 0 ;
	if ((rs = searchkeys_magic(op,pop)) >= 0) {
	    rs = SR_FAULT ;
	    if (pop) {
		memclear(pop) ;
		rs = SR_INVALID ;
		if ((n = op->nphrases) > 0) {
	            cint	sz = (n + 1) * szof(int) ;
	            pop->f_prefix = f_prefix ;
	            if (void *vp{} ; (rs = uc_malloc(sz,&vp)) >= 0) {
	                pop->nmatch = intp(vp) ;
	                memset(pop->nmatch,0,sz) ;
	                pop->cphrases = n ;
	                pop->magic = SEARCHKEYS_MAGIC ;
	            } /* end if (memory-allocation) */
	        } /* end if (valid) */
	    } /* end if (non-null) */
	} /* end if (magic) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (searchkeys_popbegin) */

int searchkeys_popend(SK *op,SK_POP *pop) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = searchkeys_magic(op,pop)) >= 0) {
	    rs = SR_FAULT ;
	    if (pop) {
		rs = SR_NOTOPEN ;
	        if (pop->magic == SEARCHKEYS_MAGIC) {
		    rs = SR_OK ;
	            pop->cphrases = 0 ;
	            if (pop->nmatch) {
	                rs1 = uc_free(pop->nmatch) ;
	                if (rs >= 0) rs = rs1 ;
	                pop->nmatch = nullptr ;
	            }
	            pop->magic = 0 ;
	        } /* end if (was open) */
	    } /* end if (non-null) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (searchkeys_popend) */

#if	CF_REGPROC
int searchkeys_process(SK *op,SK_POP *pop,cchar *sp,int sl) noex {
	int		rs ;
	int		f_match = false ;
	if ((rs = searchkeys_magic(op,pop,sp)) >= 0) {
	    rs = SR_NOTOPEN ;
	    if (pop->magic == SEARCHKEYS_MAGIC) {
		rs = SR_BADFMT ;
		if (pop->nmatch) {
		    rs = SR_OK ;
	            if (pop->cphrases > 0) {
	                bool	fpr = pop->f_prefix ;
	                for (int i = 0 ; i < op->nphrases ; i += 1) {
	                    SK_PH	*pep = (op->kphrases + i) ;
	                    cint	ki = pop->nmatch[i] ;
	                    if (ki < pep->nwords) {
	                        if ((rs = kphrase_proc(pep,fpr,ki,sp,sl)) > 0) {
	                            pop->nmatch[i] += 1 ;
	                            if (pop->nmatch[i] == pep->nwords) {
	                                pop->cphrases -= 1 ;
		                    }
	                        } else if (rs == 0) {
	                            pop->nmatch[i] = 0 ;
		                }
	                    }
	                } /* end for */
	                if (pop->cphrases == 0) f_match = true ;
	            } else {
	                f_match = true ;
	            }
		} /* end if (good-format) */
	    } /* end if (POP-magic) */
	} /* end if (magic) */
	return (rs >= 0) ? f_match : rs ;
}
/* end subroutine (searchkeys_process) */
#endif /* CF_REGPROC */

int searchkeys_processxw(SK *op,SK_POP *pop,xwords *xwp) noex {
	int		rs ;
	int		f_match = false ;
	if ((rs = searchkeys_magic(op,pop,xwp)) >= 0) {
	    rs = SR_BUGCHECK ;
	    if (pop->nmatch) {
	        if (pop->cphrases > 0) {
	            bool	fpr = pop->f_prefix ;
	            for (int i = 0 ; (rs >= 0) && (i < op->nphrases) ; i += 1) {
	                SK_PH	*pep = (op->kphrases + i) ;
	                cint		ki = pop->nmatch[i] ;
	                if (ki < pep->nwords) {
	                    if ((rs = kphrase_procxw(pep,fpr,ki,xwp)) > 0) {
	                        pop->nmatch[i] += 1 ;
	                        if (pop->nmatch[i] == pep->nwords) {
	                            pop->cphrases -= 1 ;
		                }
	                    } else {
	                        pop->nmatch[i] = 0 ;
		            }
	                } /* end if */
	            } /* end for */
	            if (pop->cphrases == 0) {
		        f_match = true ;
	            }
	        } else {
	            f_match = true ;
	        }
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? f_match : rs ;
}
/* end subroutine (searchkeys_processxw) */

int searchkeys_curbegin(SK *op,SK_CUR *curp) noex {
    	int		rs ;
	if ((rs = searchkeys_magic(op,curp)) >= 0) {
	    curp->i = -1 ;
	    curp->j = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (searchkeys_curbegin) */

int searchkeys_curend(SK *op,SK_CUR *curp) noex {
    	int		rs ;
	if ((rs = searchkeys_magic(op,curp)) >= 0) {
	    curp->i = -1 ;
	    curp->j = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (searchkeys_curend) */

int searchkeys_curenum(SK *op,SK_CUR *curp,cchar **rpp) noex {
	int		rs ;
	int		kl = 0 ;
	if ((rs = searchkeys_magic(op,curp)) >= 0) {
	    int		i{} ;
	    int		j{} ;
	    if ((rs = searchkeys_curinc(op,curp,&i,&j)) >= 0) {
	        if (i < op->nphrases) {
	            SK_KW	*wep = op->kphrases[i].kwords ;
	            cint	wel = op->kphrases[i].nwords ;
	            if (j < wel) {
	                kl = wep[j].kl ;
	                if (rpp) {
			    *rpp = wep[j].kp ;
		        }
	                curp->i = i ;
	                curp->j = j ;
	            } else {
	                rs = SR_NOTFOUND ;
		    }
	        } else {
	            rs = SR_NOTFOUND ;
	        }
	    } /* end if (searchkeys_curinc) */
	} /* end if (magic) */
	return (rs >= 0) ? kl : rs ;
}
/* end subroutine (searchkeys_enum) */


/* private subroutines */

static int searchkeys_build(SK *op,cchar **qsp) noex {
	BUILD		bi, *bip = &bi ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	cint		vsz = szof(BUILD_PH) ;
	cint		vn = 4 ;
	cint		vo = VECOBJ_OSWAP ;
	if ((rs = vecobj_start(&bip->phrases,vsz,vn,vo)) >= 0) {
	    for (int i = 0 ; qsp[i] ; i += 1) {
	        rs = searchkeys_buildadd(op,bip,qsp[i]) ;
	        if (rs < 0) break ;
	    } /* end for */
	    /* finish up */
	    if (rs >= 0) {
		if_constexpr (f_buildreduce) {
	            rs = searchkeys_buildreduce(op,bip) ;
		}
	        if (rs >= 0) {
	            rs = searchkeys_buildload(op,bip) ;
	            c = 0 ;
	        }
	    } /* end if */
	    {
	        rs1 = searchkeys_buildfins(op,bip) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    rs1 = vecobj_finish(&bip->phrases) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (phrases) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (searchkeys_build) */

static int searchkeys_buildadd(SK *op,BUILD *bip,cchar *phrase) noex {
	int		rs ;
	int		rs1 ;
	int		wc = 0 ;
	if (BUILD_PH bpe{} ; (rs = buildphrase_start(&bpe)) >= 0) {
	    bool	f_match = false ;
	    bool	f_buildphrase = true ;
	    cchar	*tp ;
	    cchar	*sp = phrase ;
	    while ((tp = strpbrk(sp," \t,")) != nullptr) {
	        if ((tp - sp) > 0) {
	            rs = searchkeys_buildaddword(op,&bpe,sp,(tp - sp)) ;
		}
	        sp = (tp + 1) ;
	        if (rs < 0) break ;
	    } /* end while */
	    if ((rs >= 0) && (sp[0] != '\0')) {
	        rs = searchkeys_buildaddword(op,&bpe,sp,-1) ;
	    }
	    /* conditionally add this phrase to the list */
	    if (rs >= 0) {
	        f_match = false ;
	        rs = buildphrase_count(&bpe) ;
	        wc = rs ;
	        if ((rs >= 0) && (wc == 1)) {
	            rs = searchkeys_buildphrasemat(op,bip,&bpe) ;
	            f_match = (rs > 0) ;
	        }
	        if (rs >= 0) {
	            if ((wc > 0) && (! f_match)) {
	                rs = vecobj_add(&bip->phrases,&bpe) ;
	                f_buildphrase = (rs >= 0) ;
	            } else {
	                f_buildphrase = false ;
	                rs1 = buildphrase_finish(&bpe) ;
	                if (rs >= 0) rs = rs1 ;
	            }
	        } /* end if */
	    } /* end if */
	    if (rs < 0) {
	        if (f_buildphrase) {
	            buildphrase_finish(&bpe) ;
	        }
	    }
	    if (wc > 0) {
	        op->nphrases += 1 ;
	    }
	} /* end if (buildphrase_start) */
	return (rs >= 0) ? wc : rs ;
}
/* end subroutine (searchkeys_buildadd) */

static int searchkeys_buildaddword(SK *op,BUILD_PH *bpp,cc *wp,int wl) noex {
	int		rs = SR_OK ;
	int		kl ;
	cchar		*kp = wp ;
	char		keybuf[KEYBUFLEN + 1] ;
	if (wl < 0) wl = strlen(wp) ;
	kl = wl ;
	if (kl > KEYBUFLEN) kl = KEYBUFLEN ;
	if (hasuc(kp,kl)) {
	    strwcpylc(keybuf,kp,kl) ;	/* can't overflow */
	    rs = strpack_store(op->spp,keybuf,kl,&kp) ;
	} /* end if */
	if (rs >= 0) {
	    rs = buildphrase_add(bpp,kp,kl) ;
	}
	return rs ;
}
/* end subroutine (searchkeys_buildaddword) */

static int searchkeys_buildphrasemat(SK *op,BUILD *bip,BUILD_PH *bpp) noex {
	int		rs  = SR_FAULT ;
	int		rs1 ;
	int		f_match = false ;
	if (op) {
	    if (cchar *kp{} ; (rs = buildphrase_getkey(bpp,0,&kp)) > 0) {
		vecobj	*plp = &bip->phrases ;
	        int	kl = rs ;
	        void	*vp{} ;
	        for (int i = 0 ; plp->get(i,&vp) >= 0 ; i += 1) {
	            BUILD_PH	*ptp = (BUILD_PH *) vp ;
	            if (vp) {
	                if ((rs1 = buildphrase_havekey(ptp,kp,kl)) > 0) {
	                    f_match = true ;
	                    break ;
	                }
		    }
	        } /* end for */
	    } /* end if */
	} /* end if (non-null) */
	return (rs >= 0) ? f_match : rs ;
}
/* end subroutine (searchkeys_buildphrasemat) */

static int searchkeys_buildreduce(SK *op,BUILD *bip) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	void		*vp{} ;
	for (int i = 0 ; vecobj_get(&bip->phrases,i,&vp) >= 0 ; i += 1) {
	    BUILD_PH	*bpp = (BUILD_PH *) vp ;
	    if (vp) {
	        if ((rs = buildphrase_count(bpp)) >= 0) {
	            cint	wc = rs ;
	            if (wc == 1) {
			cint	n = (i+1) ;
	                if ((rs = searchkeys_buildmatone(op,bip,n,bpp)) > 0) {
	                    buildphrase_finish(bpp) ;
	                    vecobj_del(&bip->phrases,i--) ;
	                    if (op->nphrases > 0) op->nphrases -=1 ;
		        }
	            }
	        } /* end if */
	    }
	    if (rs < 0) break ;
	} /* end for */
	c = op->nphrases ;
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (searchkeys_buildreduce) */

static int searchkeys_buildfins(SK *op,BUILD *bip) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    void	*vp{} ;
	    for (int i = 0 ; vecobj_get(&bip->phrases,i,&vp) >= 0 ; i += 1) {
	        BUILD_PH	*bpp = (BUILD_PH *) vp ;
	        if (vp) {
		    {
	                rs1 = buildphrase_finish(bpp) ;
	                if (rs >= 0) rs = rs1 ;
		    }
		    {
	                rs1 = vecobj_del(&bip->phrases,i--) ;
	                if (rs >= 0) rs = rs1 ;
		    }
	        }
	    } /* end for */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (searchkeys_buildfins) */

static int searchkeys_buildmatone(SK *op,BUILD *bip,int si,
		BUILD_PH *bpp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		f_match = false ;
	if (op) {
	    rs = SR_INVALID ;
	    if (si >= 0) {
		vecobj	*plp = &bip->phrases ;
	        if (cchar *kp{} ; (rs = buildphrase_getkey(bpp,0,&kp)) > 0) {
	            int		kl = rs ;
	            void	*vp{} ;
	            for (int i = si ; plp->get(i,&vp) >= 0 ; i += 1) {
	                BUILD_PH	*ptp = (BUILD_PH *) vp ;
	                if (vp) {
	                    if ((rs1 = buildphrase_havekey(ptp,kp,kl)) > 0) {
	                        f_match = (rs1 > 0) ;
	                        break ;
		            }
	                }
	            } /* end for */
	        } /* end if */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? f_match : rs ;
}
/* end subroutine (searchkeys_buildmatone) */

static int searchkeys_buildload(SK *op,BUILD *bip) noex {
	vecobj		*plp = &bip->phrases ;
	int		rs ;
	int		rs1 ;
	int		nphrases = 0 ;
	if ((rs = vecobj_count(plp)) >= 0) {
	    int		sz = (rs + 1) * szof(SK_PH) ;
	    nphrases = rs ;
	    if (void *vp{} ; (rs = uc_malloc(sz,&vp)) >= 0) {
	        int	nwords ;
	        int	pi, pj ;
	        int	wi, wj ;
	        int	kl ;
	        memset(vp,0,sz) ;
	        op->kphrases = (SK_PH *) vp ;
	        op->nphrases = nphrases ;
	        pj = 0 ;
	        for (pi = 0 ; vecobj_get(plp,pi,&vp) >= 0 ; pi += 1) {
	    	    BUILD_PH	*bpp = (BUILD_PH *) vp ;
	            if (vp) {
	    		SK_KW	*wep{} ;
			{
	                    rs = buildphrase_count(bpp) ;
	                    if (rs < 0) break ;
	                    nwords = rs ;
		    	}
			{
	                    sz = (nwords + 1) * szof(SK_KW) ;
	                    rs = uc_malloc(sz,&wep) ;
	                    if (rs < 0) break ;
		    	}
			{
	                    op->kphrases[pj].kwords = wep ;
	                    op->kphrases[pj].nwords = nwords ;
	                    pj += 1 ;
		    	}
			{
			    cchar	*kp{} ;
	                    wj = 0 ;
	                    wi = 0 ;
	                    while ((kl = buildphrase_getkey(bpp,wi,&kp)) >= 0) {
	                        wep[wj].kp = kp ;
	                        wep[wj].kl = kl ;
	                        wj += 1 ;
	                        wi += 1 ;
	                    } /* end while */
	                    if (kl != SR_NOTFOUND) rs = kl ;
			}
			{
	                    wep[wj].kp = nullptr ;
	                    wep[wj].kl = 0 ;
			}
		    } /* end if (non-null) */
	            if (rs < 0) break ;
	        } /* end for */
		{
	            op->kphrases[pj].kwords = nullptr ;
	            op->kphrases[pj].nwords = 0 ;
		}
	        if (rs < 0) {
	            for (pi = 0 ; op->kphrases[pi].kwords ; pi += 1) {
	                rs1 = uc_free(op->kphrases[pi].kwords) ;
			if (rs >= 0) rs = rs1 ;
	                op->kphrases[pi].kwords = nullptr ;
	            } /* end for */
	            {
	                rs1 = uc_free(op->kphrases) ;
			if (rs >= 0) rs = rs1 ;
	                op->kphrases = nullptr ;
	            }
	        } /* end if (error) */
	    } /* end if (memory-allocation) */
	} /* end if (vecobj_count) */
	return (rs >= 0) ? nphrases : rs ;
}
/* end subroutine (searchkeys_buildload) */

static int searchkeys_curinc(SK *op,SK_CUR *curp,int *ip,int *jp) noex {
	int		rs = SR_OK ;
	if (curp->i < 0) {
	    *ip = 0 ;
	    *jp = 0 ;
	} else if (curp->i >= op->nphrases) {
	    rs = SR_NOTFOUND ;
	} else {
	    int		wel ;
	    *ip = curp->i ;
	    *jp = (curp->j < 0) ? 1 : (curp->j + 1) ;
	    wel = op->kphrases[*ip].nwords ;
	    if (*jp >= wel) {
	        *ip = (curp->i + 1) ;
	        *jp = 0 ;
	        if (*ip >= op->nphrases) {
	            rs = SR_NOTFOUND ;
		}
	    }
	} /* end if */
	return rs ;
}
/* end subroutine (searchkeys_curinc) */

static int buildphrase_start(BUILD_PH *bpp) noex {
	int		rs = SR_FAULT ;
	if (bpp) {
	    cint	sz = szof(SK_KW) ;
	    cint	vn = 1 ;
	    cint	vo = (VECOBJ_OCOMPACT | VECOBJ_OSTATIONARY) ;
	    rs = vecobj_start(&bpp->words,sz,vn,vo) ;
	}
	return rs ;
}
/* end subroutine (buildphrase_start) */

static int buildphrase_finish(BUILD_PH *bpp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (bpp) {
	    rs1 = vecobj_finish(&bpp->words) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (buildphrase_finish) */

static int buildphrase_add(BUILD_PH *bpp,cchar *kp,int kl) noex {
	int		rs = SR_FAULT ;
	if (bpp) {
	    SK_KW	ke ;
	    if (kl < 0) kl = strlen(kp) ;
	    ke.kp = kp ;
	    ke.kl = kl ;
	    rs = vecobj_add(&bpp->words,&ke) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (buildphrase_add) */

static int buildphrase_count(BUILD_PH *bpp) noex {
	int		rs = SR_FAULT ;
	if (bpp) {
	    rs = vecobj_count(&bpp->words) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (buildphrase_count) */

static int buildphrase_getkey(BUILD_PH *bpp,int i,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	int		kl = 0 ;
	if (bpp) {
	    rs = SR_INVALID ;
	    if (i >= 0) {
		vecobj	*wlp = &bpp->words ;
	        if (void *vp{} ; (rs = wlp->get(i,&vp)) >= 0) {
	            SK_KW	*kep = (SK_KW *) vp ;
	            if (vp) {
	                kl = kep->kl ;
	                if (rpp) {
	                    *rpp = (cchar *) kep->kp ;
	                }
	            }
	        } /* end if (vecobj_get) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? kl : rs ;
}
/* end subroutine (buildphrase_getkey) */

static int buildphrase_havekey(BUILD_PH *bpp,cchar *kp,int kl) noex {
    	int		rs = SR_FAULT ;
	int		f = false ;
	if (bpp && kp) {
	    void	*vp{} ;
	    if (kl < 0) kl = strlen(kp) ;
	    for (int i = 0 ; vecobj_get(&bpp->words,i,&vp) >= 0 ; i += 1) {
	        SK_KW	*kep = (SK_KW *) vp ;
	        if (vp) {
	            f = (kl == kep->kl) ;
	            f = f && (kep->kp[0] == kp[0]) ;
	            f = f && (strncmp(kep->kp,kp,kl) == 0) ;
	            if (f) break ;
	        }
	    } /* end for */
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (buildphrase_havekey) */

#if	CF_REGPROC
static int kphrase_proc(SKPHRSE *pep,int f_prefix,int ki,cc *wp,int wl) noex {
	int		rs = SR_FAULT ;
	int		f = false ;
	if (pep && wp) {
	    rs = SR_OK ;
	    if (ki <= pep->nwords) {
	        if (ki < pep->nwords) {
	            SK_KW	*kep = (pep->kwords + ki) ;
	            if (kep != nullptr) {
	                f = (kep->kp[0] == wp[0]) ;
	                if (f) {
	                    if (f_prefix) {
	                        cint	m = nleadstr(kep->kp,wp,wl) ;
	                        f = ((m > 0) && (kep->kl == m)) ;
	                    } else {
	                        f = (wl == kep->kl) ;
	                        f = f && (strncmp(kep->kp,wp,wl) == 0) ;
	                    }
	                }
	            } else {
	                rs = SR_FAULT ;
	            }
	        } /* end if (less-than) */
	    } else {
	        rs = SR_INVALID ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (kphrase_proc) */
#endif /* CF_REGPROC */

static int kphrase_procxw(SK_PH *pep,int f_prefix,int ki,xwords *xwp) noex {
	int		rs = SR_OK ;
	int		f = false ;
	if (ki <= pep->nwords) {
	    if (ki < pep->nwords) {
	        SK_KW	*kep = (pep->kwords + ki) ;
	        if (kep != nullptr) {
		    int		wl ;
		    auto	xw_get = xwords_get ;
		    cchar	*wp{} ;
	            for (int i = 0 ; (wl = xw_get(xwp,i,&wp)) > 0 ; i += 1) {
	                f = (kep->kp[0] == wp[0]) ;
			if_constexpr (f_shortcut) {
	                    if (! f) break ;
			}
	                if (f) {
	                    if (f_prefix) {
	                        cint	m = nleadstr(kep->kp,wp,wl) ;
	                        f = ((m > 0) && (kep->kl == m)) ;
	                    } else {
	                        f = (wl == kep->kl) ;
	                        f = f && (strncmp(kep->kp,wp,wl) == 0) ;
	                    }
	                }
	                if (f) break ;
	            } /* end for (checking each "eXtra" word) */
	        } else {
	            rs = SR_FAULT ;
	        }
	    } /* end if (less than) */
	} else {
	    rs = SR_INVALID ;
	}
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (kphrase_procxw) */


