/* textlook SUPPORT */
/* charset=ISO8859-1 */
/* lang=C20 */

/* text look-up manager (we use the index and verify speculative results) */
/* version %I% last-modified %G% */

#define	CF_SINGLEWORD	1		/* treat extra words as single */

/* revision history:

	= 2008-10-01, David A-D- Morano
	This object module was originally written.

	= 2018-07-15, David A-D- Morano
	Small error check enhancement.

*/

/* Copyright © 2008,2018 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	textlook

	Description:
	This little object provides access to the TEXTLOOK database
	and index (if any).

	Note that any eigen-word list can be used because the list
	is stored in the index of the DB so that the same list is
	always used on queries as was used in the original creation
	of the index itself.  The DB proper only stores the real
	data, no eigen-words; so eigen-word lists can be changed
	on every recreation of the index.

	Notes:

	= Number of ourstanding lookups: Only one query can be
	outstanding at a time.  But any number of queries can be
	done successively.

	= On memory usage and the work queue: We limit the number
	of items that the producer thread puts onto the work queue
	(to some reasonable number like about (15 * n-cpus) because
	the work queue forms a real FIFO.  The items that are being
	FIFOed are text-reference tags.  These tags each can be
	something over MAXPATHLEN in length.  And the main producer
	thread can easily produce thousands of such tags since they
	represent files or pieces of files to be verified for
	queue-key content.  So if we multiply up what is possible,
	we can easily get something like (2048 * MAXPATHLEN) amount
	of data just in the work queue FIFO!  This could get a
	little bit big and maybe problematic on some systems with
	only a 4-GB memory address space to start with.  So by
	limiting the number of items in the work queue to some
	reasonable maximum we avoid this problem while still
	maintaining maximum concurrency.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/mman.h>
#include	<climits>
#include	<ctime>			/* |time_t| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<upt.h>
#include	<mallocxx.h>
#include	<baops.h>
#include	<vecstr.h>
#include	<ids.h>
#include	<ascii.h>
#include	<field.h>
#include	<sbuf.h>
#include	<bfile.h>
#include	<psem.h>
#include	<ptm.h>
#include	<ptc.h>
#include	<ciq.h>
#include	<intfloor.h>
#include	<intceil.h>
#include	<naturalwords.h>
#include	<txtindexmk.h>
#include	<txtindex.h>
#include	<xwords.h>
#include	<searchkeys.h>
#include	<strn.h>
#include	<sfx.h>
#include	<mkpathx.h>
#include	<sncpyx.h>
#include	<strwcpy.h>
#include	<strdcpy.h>
#include	<rtags.h>
#include	<char.h>
#include	<hasx.h>
#include	<isnot.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"textlook.h"


/* local defines */

#define	TEXTLOOK_NVERSES	33000
#define	TEXTLOOK_MINWLEN	2		/* minimum word-length */
#define	TEXTLOOK_MAXWLEN	6		/* less collisions? */
#define	TEXTLOOK_NEIGEN		2000		/* number of keys in chunk */
#define	TEXTLOOK_DIRPERM	0775		/* parent directory */
#define	TEXTLOOK_INDPERM	0664		/* the index files */
#define	TEXTLOOK_DBDNAME	"share/bibledbs"
#define	TEXTLOOK_DBNAME		"av"
#define	TEXTLOOK_QLEN		16
#define	TEXTLOOK_MAXRECLEN	(10*1024*1024)

#define	TL		textlook
#define	TL_CUR		textlook_cur
#define	TL_TAG		textlook_tag
#define	TL_IN		textlook_info

#define	SK		searchkeys
#define	SK_CUR		searchkeys_cur
#define	SK_POP		searchkeys_pop

#define	SI		subinfo
#define	SI_FL		subinfo_flags

#define	DISP		disp_head
#define	DISP_ARGS	disp_args
#define	DISP_THR	disp_thr

#define	TI		txtindex
#define	TI_CUR		txtindex_cur
#define	TI_TAG		txtindex_tag
#define	TI_INFO		txtindex_info

#define	TO_FILEMOD	(60 * 24 * 3600)
#define	TO_MKWAIT	(5 * 50)

#ifndef	KEYBUFLEN
#define	KEYBUFLEN	NATURALWORDLEN
#endif

#ifndef	CF_SINGLEWORD
#define	CF_SINGLEWORD	1		/* treat extra words as single */
#endif


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* external subroutines */


/* external variables */


/* local structures */

struct subinfo_flags {
	uint		id:1 ;
} ;

struct subinfo {
	ids		id ;
	SI_FL		f ;
	time_t		daytime ;
} ;

struct tagq {
	psem		wsem ;
	ciq		q ;
} ;

struct tagq_thing {
	uint		recoff ;
	uint		reclen ;
	char		fname[1] ;
} ;

struct disp_args {
	textlook	*op ;
	rtags		*rtp ;
	SK		*skp ;
	int		qo ;		/* query options */
	int		npar ;		/* n-parallelism */
} ;

struct disp_thr {
	pthread_t	tid ;
	volatile int	f_busy ;
	volatile int	f_exiting ;
	volatile int	f_active ;
	volatile int	rs ;
} ;

struct disp_head {
	DISP_THR	*threads ;	/* thread-privte data */
	DISP_ARGS	a ;		/* arguments */
	tagq		wq ;		/* work-queue */
	psem		sem_wq ;	/* work-queue semaphore */
	psem		sem_done ;	/* done-semaphore */
	ptm		mx ;		/* nbusy-mutex */
	ptc		cv ;		/* condition variable */
	volatile int	f_exit ;	/* assumed atomic */
	volatile int	f_done ;	/* assumed atomic */
	volatile int	f_ready ;	/* ready for workers to access */
	int		qlen ;		/* max work-queue length */
	int		nthr ;		/* bumber of threads configured */
} ;


/* forward references */

template<typename ... Args>
static int textlook_ctor(textlook *op,Args ... args) noex {
    	TEXTLOOK	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_OK ;
	    memclear(hop) ;
	    if ((op->edp = new(nothrow) eigendb) != np) {
		if ((op->idp = new(nothrow) txtindex) != np) {
		    rs = SR_OK ;
		} /* end if (new-txtindex) */
		if (rs < 0) {
		    delete op->edp ;
		    op->edp = nullptr ;
		}
	    } /* end if (new-eigendb) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (textlook_ctor) */

static int textlook_dtor(textlook *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->idp) {
		delete op->idp ;
		op->idp = nullptr ;
	    }
	    if (op->edp) {
		delete op->edp ;
		op->edp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (textlook_dtor) */

template<typename ... Args>
static inline int textlook_magic(textlook *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == TEXTLOOK_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (textlook_magic) */

static int	textlook_inbegin(TL *,cchar *,cchar *) noex ;
static int	textlook_inend(TL *) noex ;
static int	textlook_indopen(TL *,SI *) noex ;

static int	textlook_snbegin(TL *) noex ;
static int	textlook_snend(TL *) noex ;

static int	textlook_dispstart(TL *,int,SK *,rtags *) noex ;
static int	textlook_dispfinish(TL *) noex ;

static int	textlook_indclose(TL *) noex ;
static int	textlook_havekeys(TL *,TI_TAG *,cc *,int,int,SK *) noex ;
static int	textlook_havekeyer(TL *,TI_TAG *,int,
			SK *,SK_POP *,cchar *) noex ;
static int	textlook_havekeyers(TL *,TI_TAG *,int,
			SK *,int,SK_POP *) noex ;
static int	textlook_havekeysln(TL *,
			SK *,SK_POP *,cchar *,int) noex ;
static int	textlook_matchkeys(TL *,
			SK *,SK_POP *,cchar *,int) noex ;
static int	textlook_mkhkeys(TL *,vecstr *,SK *) noex ;

static int	textlook_lookuper(TL *,TL_CUR *,int,SK *,mainv) noex ;
static int	textlook_checkdisp(TL *,int,SK *,rtags *) noex ;

static int	subinfo_start(SI *) noex ;
static int	subinfo_finish(SI *) noex ;

static int	disp_start(DISP *,DISP_ARGS *) noex ;
static int	disp_starter(DISP *) noex ;
static int	disp_finish(DISP *,int) noex ;
static int	disp_addwork(DISP *,TI_TAG *,cc *,int) noex ;
static int	disp_setstate(DISP *,DISP_THR *,int) noex ;
static int	disp_nbusy(DISP *) noex ;
static int	disp_nexiting(DISP *) noex ;
static int	disp_waitdone(DISP *) noex ;
static int	disp_worker(DISP *) noex ;
static int	disp_workone(DISP *,TI_TAG *,cc *,int) noex ;
static int	disp_getourthr(DISP *,DISP_THR **) noex ;
static int	disp_readyset(DISP *) noex ;
static int	disp_readywait(DISP *) noex ;

static int	tagq_start(tagq *,int) noex ;
static int	tagq_finish(tagq *) noex ;
static int	tagq_count(tagq *) noex ;
static int	tagq_ins(tagq *,TI_TAG *,cc *,int) noex ;
static int	tagq_rem(tagq *,TI_TAG *,char *,int) noex ;

static int	mkfieldterms(char *) noex ;

#ifdef	COMMENT
static int	vcmpthreads(DISP_THR **,DISP_THR **) noex ;
#endif /* COMMENT */


/* local variables */

static char		wterms[32] ;

constexpr bool		f_singleword = CF_SINGLEWORD ;


/* exported variables */

extern const textlook_obj	textlook_modinfo = {
	"textlook",
	szof(textlook),
	szof(textlook_cur)
} ;


/* exported subroutines */

int textlook_open(TL *op,cchar *pr,cchar *dbname,cchar *bdname) noex {
	int		rs ;
	int		rs1 ;
	int		rv = 0 ;
	if ((rs = textlook_ctor(op,pr,dbname,bdname)) >= 0) {
	    rs = SR_INVALID ;
	    if (pr[0] && dbname[0]) {
		op->pr = pr ;
		static cint	rsm = mkfieldterms(wterms) ;
		if ((rs = rsm) >= 0) {
	            if ((rs = ucpagesize) >= 0) {
			op->pagesize = rs ;
	                if ((rs = textlook_inbegin(op,dbname,bdname)) >= 0) {
			    if (subinfo si ; (rs = subinfo_start(&si)) >= 0) {
	                        if ((rs = textlook_indopen(op,&si)) >= 0) {
				    rv = rs ;
	                   	    op->magic = TEXTLOOK_MAGIC ;
				}
	                        rs1 = subinfo_finish(&si) ;
	                        if (rs >= 0) rs = rs1 ;
	                    } /* end if */
	                    if (rs < 0) {
	                        textlook_inend(op) ;
	                    }
	                } /* end if (textlook_inbegin) */
	            } /* end if (ucpagesize) */
		} /* end if (mkfieldterms) */
	    } /* end if (valid) */
	    if (rs < 0) {
		textlook_dtor(op) ;
	    }
	} /* end if (textlook_ctor) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (textlook_open) */

int textlook_close(TL *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = textlook_magic(op)) >= 0) {
	    {
	        rs1 = textlook_dispfinish(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = textlook_snend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = textlook_indclose(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = textlook_inend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = textlook_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (textlook_close) */

int textlook_audit(TL *op) noex {
	int		rs ;
	if ((rs = textlook_magic(op)) >= 0) {
	    rs = txtindex_audit(op->idp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (textlook_audit) */

int textlook_getinfo(TL *op,TL_IN *tlip) noex {
	int		rs ;
	if ((rs = textlook_magic(op,tlip)) >= 0) {
	    memclear(tlip) ;
	    if (TI_INFO ti ; (rs = txtindex_getinfo(op->idp,&ti)) >= 0) {
	        tlip->ticreat = ti.ticreat ;	/* index creation-time */
	        tlip->timod = ti.timod ;	/* index modification-time */
	        tlip->count = ti.count ;	/* number of tags */
	        tlip->neigen = ti.neigen ;
	        tlip->minwlen = ti.minwlen ;	/* minimum word length */
	        tlip->maxwlen = ti.maxwlen ;	/* maximum word length */
	    } /* end if (textindex_getinfo) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (textlook_getinfo) */

int textlook_getsdn(TL *op,char *rb,int rl) noex {
	int		rs ;
	if ((rs = textlook_magic(op,rb)) >= 0) {
	    rs = txtindex_getsdn(op->idp,rb,rl) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (textlook_getsdn) */

int textlook_getsfn(TL *op,char *rb,int rl) noex {
	int		rs ;
	if ((rs = textlook_magic(op,rb)) >= 0) {
	    rs = txtindex_getsfn(op->idp,rb,rl) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (textlook_getsfn) */

int textlook_curbegin(TL *op,TL_CUR *curp) noex {
	int		rs ;
	if ((rs = textlook_magic(op,curp)) >= 0) {
	    memclear(curp) ;
	    if (char *tb{} ; (rs = malloc_mp(&tb)) >= 0) {
		curp->tbuf = tb ;
	        if ((rs = rtags_start(&curp->tags,0)) >= 0) {
		    rtags_cur	*rcp = &curp->tcur ;
	            if ((rs = rtags_curbegin(&curp->tags,rcp)) >= 0) {
	                op->ncursors += 1 ;
	                curp->magic = TEXTLOOK_MAGIC ;
	            }
	            if (rs < 0) {
	                rtags_finish(&curp->tags) ;
	            }
	        } /* end if (rtags_start) */
		if (rs < 0) {
		    uc_free(tb) ;
		    curp->tbuf = nullptr ;
		} /* end if (error-handling) */
	    } /* end if (memory-allocation) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (textlook_curbegin) */

int textlook_curend(TL *op,TL_CUR *curp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = textlook_magic(op,curp)) >= 0) {
	    rs = SR_NOTOPEN ;
	    if (curp->magic == TEXTLOOK_MAGIC) {
		{
		    rs1 = rtags_curend(&curp->tags,&curp->tcur) ;
		    if (rs >= 0) rs = rs1 ;
		}
		{
		    rs1 = rtags_finish(&curp->tags) ;
		    if (rs >= 0) rs = rs1 ;
		}
		if (curp->tbuf) {
		    rs1 = uc_free(curp->tbuf) ;
		    if (rs >= 0) rs = rs1 ;
		    curp->tbuf = nullptr ;
		}
	        curp->ntags = 0 ;
	        if (op->ncursors > 0) {
	            op->ncursors -= 1 ;
	        }
		curp->magic = 0 ;
	    } /* end if (valid-subcursor) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (textlook_curend) */

int textlook_curlook(TL *op,TL_CUR *curp,int qo,cchar **qsp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = textlook_magic(op,curp,qsp)) >= 0) {
	    rs = SR_NOTOPEN ;
	    if (curp->magic == TEXTLOOK_MAGIC) {
		cnullptr	np{} ;
	        /* as a curtesy, dump any prior results */
	        if (curp->ntags > 0) {
	            curp->ntags = 0 ;
	            rs = rtags_curdump(&curp->tags,&curp->tcur) ;
	        }
	        /* go with this lookup */
	        if (rs >= 0) {
	            if (SK sk ; (rs = searchkeys_start(&sk,qsp)) >= 0) {
	                vecstr	hkeys, *hlp = &hkeys ; /* hash-keys */
		        cint	vn = 10 ;
	                cint	vo = (VECSTR_OCOMPACT) ;
	                if ((rs = hlp->start(vn,vo)) >= 0) {
	                    if ((rs = textlook_mkhkeys(op,&hkeys,&sk)) >= 0) {
	            		mainv	hkeya{} ;
	                        if ((rs = hlp->getvec(&hkeya)) >= 0) {
			            auto	lu = textlook_lookuper ;
	                            if ((rs = lu(op,curp,qo,&sk,hkeya)) >= 0) {
	                                c = rs ;
	                                curp->ntags = c ;
	                                if (c > 1) {
	                                    rs = rtags_sort(&curp->tags,np) ;
	                                }
	                            } /* end if (textlook_lookuper) */
	                        } /* end if (vecstr_getvec) */
	                    } /* end if (textlook_mkhkeys) */
	                    rs1 = hlp->finish ;
	                    if (rs >= 0) rs = rs1 ;
	                } /* end if (hkeys) */
	                rs1 = searchkeys_finish(&sk) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (searchkeys) */
	        } /* end if (ok) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (textlook_curlook) */

int textlook_curread(TL *op,TL_CUR *curp,TL_TAG *tagp,char *bp,int bl) noex {
	int		rs ;
	int		fl = 0 ;
	if ((rs = textlook_magic(op,curp,tagp,bp)) >= 0) {
	    rs = SR_NOTOPEN ;
	    bp[0] = '\0' ;
	    if (curp->magic == TEXTLOOK_MAGIC) {
	        if (curp->ntags > 0) {
		    rtags	*rtp = &curp->tags ;
		    rtags_cur	*rcp = &curp->tcur ;
		    rtags_tag	rt{} ;
		    auto rt_en = rtags_curenum ;
	            if ((rs = rt_en(rtp,rcp,&rt,bp,bl)) >= 0) {
			fl = rs ;
	                tagp->recoff = rt.recoff ;
	                tagp->reclen = rt.reclen ;
	            }
	        } else {
	            rs = SR_NOTFOUND ;
	        }
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? fl : rs ;
}
/* end subroutine (textlook_curread) */

int textlook_count(TL *op) noex {
	int		rs ;
	if ((rs = textlook_magic(op)) >= 0) {
	    rs = txtindex_count(op->idp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (textlook_count) */


/* private subroutines */

static int textlook_inbegin(TL *op,cchar *dbname,cchar *bdname) noex {
	int		rs ;
	if (cc *cp{} ; (rs = uc_mallocstr(dbname,&cp)) >= 0) {
	    op->dbname = cp ;
	    if (bdname) {
	        if ((rs = uc_mallocstr(bdname,&cp)) >= 0) {
	            op->bdname = cp ;
		}
	    }
	    if (rs < 0) {
	        uc_free(op->dbname) ;
		op->dbname = nullptr ;
	    }
	} /* end if (m-a) */
	return rs ;
}
/* end subroutine (textlook_inbegin) */

static int textlook_inend(TL *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->bdname) {
	    rs1 = uc_free(op->bdname) ;
	    if (rs >= 0) rs = rs1 ;
	    op->bdname = nullptr ;
	}
	if (op->dbname) {
	    rs1 = uc_free(op->dbname) ;
	    if (rs >= 0) rs = rs1 ;
	    op->dbname = nullptr ;
	}
	return rs ;
}
/* end subroutine (textlook_inend) */

static int textlook_indopen(TL *op,SI *sip) noex {
    	int		rs = SR_FAULT ;
	if (op && sip) {
	    if ((rs = txtindex_open(op->idp,op->pr,op->dbname)) >= 0) {
	        op->fl.ind = true ;
	        rs = textlook_snbegin(op) ;
	        if (rs < 0) {
	            txtindex_close(op->idp) ;
	        }
	    } /* end if (txtindex_open) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (textlook_indopen) */

static int textlook_snbegin(TL *op) noex {
	int		rs ;
	if ((rs = textlook_snend(op)) >= 0) {
	    if (char *tbuf{} ; (rs = malloc_mp(&tbuf)) >= 0) {
		cint	tlen = rs ;
		if ((rs = txtindex_getsdn(op->idp,tbuf,tlen)) >= 0) {
		    cchar	*cp{} ; /* used multiple blocks */
		    int		tl = rs ;
	            if (tbuf[0] != '\0') {
	                if ((rs = uc_mallocstrw(tbuf,tl,&cp)) >= 0) {
	                    op->sdn = cp ;
		        }
	            } /* end if (SDN) */
	            if (rs >= 0) {
			if ((rs = txtindex_getsfn(op->idp,tbuf,tlen)) >= 0) {
			    tl = rs ;
	            	    if (tbuf[0] != '\0') {
	            	        if ((rs = uc_mallocstrw(tbuf,tl,&cp)) >= 0) {
	                	    op->sfn = cp ;
	            		}
			    }
			}
	                if (rs < 0) {
	                    if (op->sdn) {
	                        uc_free(op->sdn) ;
	                        op->sdn = nullptr ;
	                    }
	                } /* end if (error-handling) */
		    } /* end if (ok) */
	        } /* end if (txtindex_getsdn) */
		rs = rsfree(rs,tbuf) ;
	    } /* end if (m-a-f) */
	} /* end if (textlook_snend) */
	return rs ;
}
/* end subroutines (textlook_snbegin) */

static int textlook_snend(TL *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->sfn) {
	    rs1 = uc_free(op->sfn) ;
	    if (rs >= 0) rs = rs1 ;
	    op->sfn = nullptr ;
	}
	if (op->sdn) {
	    rs1 = uc_free(op->sdn) ;
	    if (rs >= 0) rs = rs1 ;
	    op->sdn = nullptr ;
	}
	return rs ;
}
/* end subroutines (textlook_snend) */

static int textlook_indclose(TL *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    rs1 = textlook_snend(op) ;
	    if (rs >= 0) rs = rs1 ;
	}
	if (op->fl.ind) {
	    op->fl.ind = false ;
	    rs1 = txtindex_close(op->idp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (textlook_inclose) */

static int textlook_havekeys(TL *op,TI_TAG *tagp,cc *fp,int fl,
		int qo,SK *skp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		f = false ;
	if (tagp->reclen > 0) {
	    SK_POP	pkeys ;
	    cnullptr	np{} ;
	    cint	f_prefix = (qo & TEXTLOOK_OPREFIX) ;
	    if ((rs = searchkeys_popbegin(skp,&pkeys,f_prefix)) >= 0) {
	        if (rs > 0) {
	            cchar	*fn = fp ;
	            if ((fn[0] == '\0') && (op->sfn != np)) {
	                fn = op->sfn ;
	            }
	            if (fn[0] != '\0') {
			if (char *fbuf{} ; (rs = malloc_mp(&fbuf)) >= 0) {
	                    if (fn[0] != '/') {
	                        cchar	*dn = op->sdn ;
	                        if ((dn == np) || (dn[0] == '\0')) {
	                            dn = op->bdname ;
	                        }
	                        if ((dn != np) && (dn[0] != '\0')) {
	                            rs = mkpath(fbuf,dn,fn,fl) ;
	                            fn = fbuf ;
	                        }
	                    }
	                    if (rs >= 0) {
			        auto tl_hk = textlook_havekeyer ;
	                        rs = tl_hk(op,tagp,qo,skp,&pkeys,fn) ;
	                        f = rs ;
	                    }
			    rs = rsfree(rs,fbuf) ;
			} /* end if (m-a-f) */
	            } /* end if (non-empty) */
	        } /* end if (positive) */
	        rs1 = searchkeys_popend(skp,&pkeys) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (searchkeys-pop) */
	} /* end if (positive) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (textlook_havekeys) */

static int textlook_havekeyer(TL *op,TI_TAG *tagp,int qo,
		SK *skp,SK_POP *pkp,cchar *fn) noex {
	cint		of = O_RDONLY ;
	int		rs ;
	int		rs1 ;
	int		f = false ;
	if ((rs = u_open(fn,of,0666)) >= 0) {
	    cint	fd = rs ;
	    if (USTAT sb ; (rs = u_fstat(fd,&sb)) >= 0) {
	        if (S_ISREG(sb.st_mode)) {
	            csize	fsize = (sb.st_size & INT_MAX) ;
	            if (tagp->recoff < fsize) {
	                rs = textlook_havekeyers(op,tagp,qo,skp,fd,pkp) ;
	                f = (rs > 0) ;
	            }
	        }
	    }
	    rs1 = u_close(fd) ;
	    if (rs >= 0) rs = rs1 ;
	} else if (isNotAccess(rs)) {
	    rs = SR_OK ;
	} /* end if (file) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (textlook_havekeyer) */

static int textlook_havekeyers(TL *op,TI_TAG *tagp,int qo,
		SK *skp,int fd,SK_POP *pkp) noex {
    	cnullptr	np{} ;
	off_t		recoff = tagp->recoff ;
	off_t		recext ;
	off_t		mo ;
	size_t		ms ;
	uint		reclen = tagp->reclen ;
	cint		maxreclen = TEXTLOOK_MAXRECLEN ;
	cint		mt = PROT_READ ;
	cint		mf = MAP_SHARED ;
	int		rs ;
	int		rs1 ;
	int		f = false ;
	(void) qo ; /* UNUSED */
	if (reclen > maxreclen) reclen = maxreclen ;
	mo = ufloor(recoff,op->pagesize) ;
	recext = (recoff + reclen) ;
	ms = uceil(recext,op->pagesize) - mo ;
	if (char *md{} ; (rs = u_mmapbegin(np,ms,mt,mf,fd,mo,&md)) >= 0) {
	    int		ml = reclen ;
	    int		ll ;
	    cchar	*lp ;
	    cchar	*tp ;
	    cchar	*mp = (md + (tagp->recoff - mo)) ;
	    while ((tp = strnchr(mp,ml,'\n')) != nullptr) {
	        cint	len = ((tp + 1) - mp) ;
	        lp = mp ;
	        ll = (len - 1) ;
	        if (ll > 0) {
	            rs = textlook_havekeysln(op,skp,pkp,lp,ll) ;
	            f = (rs > 0) ;
	            if (f) break ;
	        } /* end if */
	        ml -= len ;
	        mp += len ;
	        if (rs < 0) break ;
	    } /* end while (readling lines) */
	    rs1 = u_mmapend(md,ms) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (memory-map) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (textlook_havekeyers) */

static int textlook_havekeysln(TL *op,SK *skp,SK_POP *pkp,
		cc *lp,int ll) noex {
	int		rs ;
	int		rs1 ;
	int		f = false ;
	if (field fsb ; (rs = fsb.start(lp,ll)) >= 0) {
	    int		fl, sl, kl ;
	    cchar	*fp, *sp, *kp ;
	    char	keybuf[KEYBUFLEN + 1] ;
	    while ((fl = fsb.word(wterms,&fp)) >= 0) {
	        if (fl && (fp[0] == CH_SQUOTE)) {
	            fp += 1 ;
	            fl -= 1 ;
	        }
	        if (fl == 0) continue ;
	        sl = sfword(fp,fl,&sp) ;
	        if ((sl <= 0) || (sp == nullptr)) continue ;
	        kp = sp ;
	        kl = sl ;
	        if (kl > KEYBUFLEN) {			/* prevents overflow */
	            kl = KEYBUFLEN ;
		}
	        if (hasuc(kp,kl)) {
	            strwcpylc(keybuf,kp,kl) ;	/* cannot (does not) overflow */
	            kp = keybuf ;
	        }
		{
	            rs = textlook_matchkeys(op,skp,pkp,kp,kl) ;
	            f = (rs > 0) ;
		}
	        if (f) break ;
	        if (rs < 0) break ;
	    } /* end while (fielding words) */
	    rs1 = fsb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (field) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (textlook_havekeysln) */

/* do the keys match? */
static int textlook_matchkeys(TL *op,SK *skp,SK_POP *pkp,cc *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		f = false ;
	if (op) {
	    /* deal with extra (ex: possessive) words */
	    if_constexpr (f_singleword) {
	        if (xwords xw ; (rs = xwords_start(&xw,sp,sl)) >= 0) {
	            {
	                rs = searchkeys_processxw(skp,pkp,&xw) ;
	                f = (rs > 0) ;
	            }
	            rs1 = xwords_finish(&xw) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if */
	    } else {
	        if (xwords xw ; (rs = xwords_start(&xw,sp,sl)) >= 0) {
	            int		wi = 0 ;
	            int		cl ;
	            cchar	*cp ;
	            while ((cl = xwords_get(&xw,wi,&cp)) > 0) {
		        {
	                    rs = searchkeys_process(skp,pkp,cp,cl) ;
	                    f = (rs > 0) ;
		        }
	                if (rs < 0) break ;
	                if (f) break ;
	                wi += 1 ;
	            } /* end while (matching words) */
	            rs1 = xwords_finish(&xw) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if */
	    } /* end if_constexpr (f_singleword) */
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (textlook_matchkeys) */

static int textlook_mkhkeys(TL *op,vecstr *hkp,SK *skp) noex {
	int		rs ;
	int		rs1 ;
	int		nkeys = 0 ;
	if (SK_CUR cur ; (rs = searchkeys_curbegin(skp,&cur)) >= 0) {
	    int		kl ;
	    cchar	*kp ;
	    while ((rs1 = searchkeys_curenum(skp,&cur,&kp)) >= 0) {
	        kl = rs1 ;
	        if ((kp != nullptr) && (kl >= op->minwlen)) {
	            rs = vecstr_adduniq(hkp,kp,kl) ;
	            if (rs < INT_MAX) nkeys += 1 ;
	        }
	        if (rs < 0) break ;
	    } /* end while (enumerating search-keys) */
	    if ((rs >= 0) && (rs1 != SR_NOTFOUND)) rs = rs1 ;
	    rs1 = searchkeys_curend(skp,&cur) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (cursor) */
	return (rs >= 0) ? nkeys : rs ;
}
/* end subroutine (textlook_mkhkeys) */

static int textlook_lookuper(TL *op,TL_CUR *curp,int qo,SK *skp,
		mainv hkeya) noex {
	rtags		*rtp = &curp->tags ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = textlook_checkdisp(op,qo,skp,rtp)) >= 0) {
	    TI_CUR	tcur ;
	    TI_TAG	ttag ;
	    DISP	*dop = (DISP *) op->disp ;
	    if ((rs = txtindex_curbegin(op->idp,&tcur)) >= 0) {
	        if ((rs = txtindex_curlook(op->idp,&tcur,hkeya)) >= 0) {
	            int		ntags = rs ;
		    int		fl ;
	            while ((rs >= 0) && (ntags-- > 0)) {
			cint	tl = curp->tlen ;
			char	*tb = curp->tbuf ;
	                rs1 = txtindex_curenum(op->idp,&tcur,&ttag,tb,tl) ;
	                if (rs1 == SR_NOTFOUND) break ;
	                rs = rs1 ;
			fl = rs ;
	                if ((rs >= 0) && (ttag.reclen > 0)) {
	                    rs = disp_addwork(dop,&ttag,tb,fl) ;
	                }
	            } /* end while */
	        } /* end if (txtindex_curlook) */
	        rs1 = txtindex_curend(op->idp,&tcur) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (txtindex-cur) */
	    rs1 = disp_waitdone(dop) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (disp-checkstart) */
	if (rs >= 0) {
	    rs = rtags_count(rtp) ;
	    c = rs ;
	}
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (textlook_lookuper) */

static int textlook_checkdisp(TL *op,int qo,SK *skp,rtags *rtp) noex {
	int		rs = SR_OK ;
	if (op->disp == nullptr) {
	    rs = textlook_dispstart(op,qo,skp,rtp) ;
	}
	return rs ;
}
/* end subroutine (textlook_checkdisp) */

static int textlook_dispstart(TL *op,int qo,SK *skp,rtags *rtp) noex {
	int		rs = SR_OK ;
	if (op->disp == nullptr) {
	    if ((rs = uptgetconcurrency()) >= 0) {
	        cint	npar = (rs+1) ;
	        cint	osz = szof(DISP) ;
	        if (void *p{} ; (rs = uc_malloc(osz,&p)) >= 0) {
	            DISP	*dop = (DISP *) p ;
	            {
	                DISP_ARGS	a{} ;
	                a.op = op ;
	                a.qo = qo ;
	                a.skp = skp ;
	                a.rtp = rtp ;
	                a.npar = npar ;
	                if ((rs = disp_start(dop,&a)) >= 0) {
	                    op->disp = dop ;
	                }
	            } /* end block */
	            if (rs < 0) uc_free(p) ;
	        } /* end if (memory-allocation) */
	    } /* end if (uptgetconcurrency) */
	} /* end if (needed start-up) */
	return rs ;
}
/* end subroutine (textlook_dispstart) */

static int textlook_dispfinish(TL *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		f_abort = true ;
	if (op->disp) {
	    DISP	*dop = (DISP *) op->disp ;
	    {
	        rs1 = disp_finish(dop,f_abort) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = uc_free(dop) ;
	        if (rs >= 0) rs = rs1 ;
	        op->disp = nullptr ;
	    }
	}
	return rs ;
}
/* end subroutine (textlook_dispfinish) */

static int subinfo_start(SI *sip) noex {
	int		rs = SR_FAULT ;
	if (sip) {
	    rs = memclear(sip) ;
	    sip->daytime = getustime ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (subinfo_start) */

static int subinfo_finish(SI *sip) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (sip->fl.id) {
	    sip->fl.id = false ;
	    rs1 = ids_release(&sip->id) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (subinfo_finish) */

static int disp_start(DISP *dop,DISP_ARGS *dap) noex {
	cint		qlen = TEXTLOOK_QLEN ;
	int		rs = SR_FAULT ;
	if (dop) {
	    memclear(dop) ;
	    dop->a = *dap ;
	    dop->nthr = dap->npar ;
	    if ((rs = ptm_create(&dop->mx,nullptr)) >= 0) {
	        if ((rs = ptc_create(&dop->cv,nullptr)) >= 0) {
	            cint	f_sh = false ;
	            if ((rs = psem_create(&dop->sem_wq,f_sh,0)) >= 0) {
	                psem	*ws = &dop->sem_done ;
	                if ((rs = psem_create(ws,f_sh,0)) >= 0) {
	                    dop->qlen = (dop->a.npar + qlen) ;
	                    if ((rs = tagq_start(&dop->wq,dop->qlen)) >= 0) {
	                        {
	                            rs = disp_starter(dop) ;
	                        }
	                        if (rs < 0) {
	                            tagq_finish(&dop->wq) ;
			        }
	                    } /* end if (tagq_start) */
	                    if (rs < 0) {
	                        psem_destroy(&dop->sem_done) ;
			    }
	                }
	                if (rs < 0) {
	                    psem_destroy(&dop->sem_wq) ;
		        }
	            }
	            if (rs < 0) {
	                ptc_destroy(&dop->cv) ;
		    }
	        } /* end if (ptc-create) */
	        if (rs < 0) {
	            ptm_destroy(&dop->mx) ;
	        }
	    } /* end if (ptm-create) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (disp_start) */

static int disp_starter(DISP *dop) noex {
	cint		sz = (dop->nthr * szof(DISP_THR)) ;
	int		rs ;
	if (void *p{} ; (rs = uc_malloc(sz,&p)) >= 0) {
	    DISP_THR	*dtp ;
	    pthread_t	tid ;
	    uptsub_f	sub = uptsub_f(disp_worker) ;
	    int		i ; /* used-afterwards */
	    dop->threads = (DISP_THR *) p ;
	    memset(p,0,sz) ;
	    for (i = 0 ; i < dop->nthr ; i += 1) {
	        dtp = (dop->threads+i) ;
	        if ((rs = uptcreate(&tid,nullptr,sub,dop)) >= 0) {
	            dtp->tid = tid ;
	            dtp->f_active = true ;
	        }
	        if (rs < 0) break ;
	    } /* end for */
	    if (rs >= 0) {
	        rs = disp_readyset(dop) ;
	    }
	    if (rs < 0) {
	        cint	n = i ; /* <- 'i' variable was set from above */
	        dop->f_exit = true ;
	        for (int j = 0 ; j < n ; j += 1) {
	            psem_post(&dop->sem_wq) ;
	        }
	        for (int j = 0 ; j < n ; j += 1) {
	            dtp = (dop->threads + j) ;
	            uptjoin(dtp->tid,nullptr) ;
	        } /* end for */
	        uc_free(p) ;
	        dop->threads = nullptr ;
	    } /* end if (error) */
	} /* end if (m-a) */
	return rs ;
}
/* end subroutine (disp_starter) */

static int disp_finish(DISP *dop,int f_abort) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (dop) {
	    rs = SR_OK ;
	    dop->f_done = true ;	/* assumed to be atomic! */
	    if (f_abort) {
	        dop->f_exit = true ;	/* assumed to be atomic! */
	    }
	    for (int i = 0 ; i < dop->nthr ; i += 1) {
	        rs1 = psem_post(&dop->sem_wq) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (dop->threads != nullptr) {
	        for (int i = 0 ; i < dop->nthr ; i += 1) {
	            DISP_THR	*dtp = (dop->threads + i) ;
	            if (dtp->f_active) {
	                pthread_t	tid = dtp->tid ;
	                dtp->f_active = false ;
	                rs1 = uptjoin(tid,nullptr) ;
	                if (rs >= 0) rs = rs1 ;
	                if (rs >= 0) rs = dtp->rs ;
	            } /* end if (active) */
	        } /* end for */
	        rs1 = uc_free(dop->threads) ;
	        if (rs >= 0) rs = rs1 ;
	        dop->threads = nullptr ;
	    } /* end if (threads) */
	    {
	        rs1 = tagq_finish(&dop->wq) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = psem_destroy(&dop->sem_done) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = psem_destroy(&dop->sem_wq) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = ptm_destroy(&dop->mx) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (disp_finish) */

static int disp_addwork(DISP *dop,TI_TAG *tagp,cc *fp,int fl) noex {
	int		rs ;
	if ((rs = tagq_ins(&dop->wq,tagp,fp,fl)) >= 0) {
	    rs = psem_post(&dop->sem_wq) ; /* post always (more parallelism) */
	}
	return rs ;
}
/* end subroutine (disp_addwork) */

/* worker threads call this to set their "busy" status */
static int disp_setstate(DISP *dop,DISP_THR *tip,int f) noex {
	ptm		*mxp = &dop->mx ;
	int		rs ;
	int		rs1 ;
	int		f_prev = false ;
	if ((rs = mxp->lockbegin) >= 0) {
	    f_prev = tip->f_busy ;
	    tip->f_busy = f ;
	    if ((! f) && f_prev) {
	        rs = psem_post(&dop->sem_done) ;
	    }
	    rs1 = mxp->lockend ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if */
	return (rs >= 0) ? f_prev : rs ;
}
/* end subroutine (disp_setstate) */

/* main or manager thread calls this to find out how many workers are busy */
static int disp_nbusy(DISP *dop) noex {
	int		rs = SR_OK ;
	int		n = 0 ;
	if (dop->threads != nullptr) {
	    DISP_THR	*dtp ;
	    for (int i = 0 ; i < dop->nthr ; i += 1) {
	        dtp = (dop->threads + i) ;
	        if (dtp->f_busy) n += 1 ;
	    }
	}
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (disp_nbusy) */

/* manager thread calls this get get number of exited workers */
static int disp_nexiting(DISP *dop) noex {
	DISP_THR	*dtp = dop->threads ;
	int		rs = SR_OK ;
	int		n = 0 ;
	for (int i = 0 ; i < dop->nthr ; i += 1) {
	    if (dtp[i].f_exiting) n += 1 ;
	}
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (disp_nexiting) */

/* main-thread calls this to wait for a parallel query to complete */
static int disp_waitdone(DISP *dop) noex {
	int		rs = SR_OK ;
	int		nbusy = -1 ;
	int		nexited = -1 ;
	int		nwq = -1 ;
	bool		f_cond = false ;
	bool		f_notbusy = false ;
	bool		f_empty = false ;
	bool		f_allexited = false ;
	for (int i = 0 ; (rs >= 0) && (! f_cond) ; i += 1) {
	    if ((rs >= 0) && (i > 0)) {
	        rs = psem_wait(&dop->sem_done) ;
	    }
	    if ((rs >= 0) && (! f_empty)) {
	        rs = tagq_count(&dop->wq) ;
	        nwq = rs ;
	        f_empty = (nwq == 0) ;
	    }
	    if ((rs >= 0) && f_empty && (! f_notbusy)) {
	        rs = disp_nbusy(dop) ;
	        nbusy = rs ;
	        f_notbusy = (nbusy == 0) ;
	    }
	    {
	        f_cond = (f_empty && f_notbusy) ;
	    }
	    if ((rs >= 0) && (! f_cond)) {
	        rs = disp_nexiting(dop) ;
	        nexited = rs ;
	        f_allexited = (nexited == dop->nthr) ;
	        if (f_allexited) f_cond = true ;
	    }
	} /* end for (waiting for done-ness) */
	return rs ;
}
/* end subroutine (disp_waitdone) */

static int disp_worker(DISP *dop) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (DISP_THR *dtp{} ; (rs = disp_getourthr(dop,&dtp)) >= 0) {
	    if (char *tb{} ; (rs = malloc_mp(&tb)) >= 0) {
		cint	tl = rs ;
	        while ((rs >= 0) && (! dop->f_exit)) {
	            if ((rs = psem_wait(&dop->sem_wq)) >= 0) {
	                if (dop->f_exit) break ;
	                if ((rs = disp_setstate(dop,dtp,true)) >= 0) {
	    		    TI_TAG	qv ;
	                    while ((rs = tagq_rem(&dop->wq,&qv,tb,tl)) > 0) {
				cint	fl = rs ;
			        rs = disp_workone(dop,&qv,tb,fl) ;
	            	        if (dop->f_exit) break ;
			        if (rs < 0) break ;
	                    } /* end while (work) */
	                    rs1 = disp_setstate(dop,dtp,false) ;
	                    if (rs >= 0) rs = rs1 ;
	                } /* end if (worker was busy) */
	            } /* end if (psem_wait) */
	            if (dop->f_done) break ;
	        } /* end while (waiting) */
		rs = rsfree(rs,tb) ;
	    } /* end if (m-a-f) */
	    dtp->rs = rs ;
	    dtp->f_exiting = true ;
	} /* end if (disp_getourthr) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (disp_worker) */

static int disp_workone(DISP *dop,TI_TAG *qvp,cc *fp,int fl) noex {
	TL	*op = dop->a.op ;
	SK	*skp = dop->a.skp ; /* renew */
	rtags	*rtp = dop->a.rtp ; /* renew */
	int	qo = dop->a.qo ; /* renew */
	int	rs ;
	int	c = 0 ;
	if ((rs = textlook_havekeys(op,qvp,fp,fl,qo,skp)) > 0) {
	    rtags_tag	rt{} ;
	    c += 1 ;
	    rt.recoff = qvp->recoff ;
	    rt.reclen = qvp->reclen ;
	    rs = rtags_add(rtp,&rt,fp,fl) ;
	} /* end if (found a key) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (disp_workone) */

/* child thread calls this to get its own local-data pointer */
static int disp_getourthr(DISP *dop,DISP_THR **rpp) noex {
	int		rs ;
	int		i = 0 ; /* used-afterwards (returned) */
	int		f = false ;
	if ((rs = disp_readywait(dop)) >= 0) {
	    DISP_THR	*dtp ;
	    pthread_t	tid = pthread_self() ;
	    for (i = 0 ; i < dop->nthr ; i += 1) {
	        dtp = (dop->threads+i) ;
	        f = uptequal(dtp->tid,tid) ;
	        if (f) break ;
	    } /* end for */
	    if (f) {
	        if (rpp) *rpp = dtp ;
	    } else {
	        rs = SR_BUGCHECK ;
	    }
	} /* end if (disp_readywait) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (disp_getourthr) */

/* main-thread calls this to indicate sub-threads can read completed object */
static int disp_readyset(DISP *dop) noex {
	ptm		*mxp = &dop->mx ;
	int		rs ;
	int		rs1 ;
	if ((rs = mxp->lockbegin) >= 0) {
	    {
	        dop->f_ready = true ;
	        rs = ptc_broadcast(&dop->cv) ; /* 0-bit semaphore */
	    }
	    rs1 = mxp->lockend ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ptm) */
	return rs ;
}
/* end subroutine (disp_readyset) */

/* sub-threads call this to wait until object is ready */
static int disp_readywait(DISP *dop) noex {
	ptm		*mxp = &dop->mx ;
	int		rs ;
	int		rs1 ;
	if ((rs = mxp->lockbegin) >= 0) {
	    while ((! dop->f_ready) && (! dop->f_exit)) {
	        rs = ptc_wait(&dop->cv,mxp) ;
	        if (rs < 0) break ;
	    } /* end while */
	    rs1 = mxp->lockend ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ptm) */
	return rs ;
}
/* end subroutine (disp_readywait) */

/* this object ('tagq') forms the Q of work going into the worker threads */
static int tagq_start(tagq *tqp,int n) noex {
	cint		f_shared = false ;
	int		rs ;
	if (n < 1) n = 1 ;
	if ((rs = psem_create(&tqp->wsem,f_shared,n)) >= 0) {
	    rs = ciq_start(&tqp->q) ;
	    if (rs < 0) {
	        psem_destroy(&tqp->wsem) ;
	    }
	}
	return rs ;
}
/* end subroutine (tagq_start) */

static int tagq_finish(tagq *tqp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rs2 ;
	if (tqp) {
	    ciq		*cqp = &tqp->q ;
	    char	*p ;
	    rs = SR_OK ;
	    while ((rs2 = ciq_rem(cqp,&p)) >= 0) {
	        rs1 = uc_free(p) ; /* these things are opaque */
	        if (rs >= 0) rs = rs1 ;
	    }
	    if ((rs >= 0) && (rs2 != SR_NOTFOUND)) rs = rs2 ;
	    {
	        rs1 = ciq_finish(cqp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = psem_destroy(&tqp->wsem) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (tagq_finish) */

static int tagq_count(tagq *tqp) noex {
	return ciq_count(&tqp->q) ;
}
/* end subroutine (tagq_finish) */

static int tagq_ins(tagq *tqp,TI_TAG *tagp,cc *fp,int fl) noex {
	cint		sz = szof(tagq_thing) + fl + 1 ;
	int		rs ;
	int		rc = 0 ;
	if (void *p{} ; (rs = uc_malloc(sz,&p)) >= 0) {
	    tagq_thing	*ttp = (tagq_thing *) p ;
	    ttp->recoff = tagp->recoff ;
	    ttp->reclen = tagp->reclen ;
	    strwcpy(ttp->fname,fp,fl) ;
	    if ((rs = psem_wait(&tqp->wsem)) >= 0) {
	        rs = ciq_ins(&tqp->q,ttp) ;
	        rc = rs ;
	    }
	    if (rs < 0) uc_free(p) ;
	} /* end if */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (tagq_ins) */

static int tagq_rem(tagq *tqp,TI_TAG *tagp,char *fbuf,int flen) noex {
	int		rs ;
	int		len = 0 ;
	if (tagq_thing *ttp{} ; (rs = ciq_rem(&tqp->q,&ttp)) >= 0) {
	    tagp->recoff = ttp->recoff ;
	    tagp->reclen = ttp->reclen ;
	    len = strdcpy(fbuf,flen,ttp->fname) - fbuf ;
	    uc_free(ttp) ;
	    rs = psem_post(&tqp->wsem) ;
	} else if (rs == SR_NOTFOUND) {
	    rs = SR_OK ;
	} /* end if (ciq_rem) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (tagq_rem) */

static int mkfieldterms(char *terms) noex {
    	int		rs = SR_FAULT ;
	if (terms) {
	    rs = SR_OK ;
	    for (int i = 0 ; i < 32 ; i += 1) {
	        terms[i] = 0xFF ;
	    }
	    for (int i = 0 ; i < 256 ; i += 1) {
	        if (isalnumlatin(i)) {
	            baclr(terms,i) ;
	        }
	    } /* end for */
	    baclr(terms,CH_SQUOTE) ;		/* allow apostrophe */
	    baclr(terms,'_') ;			/* allow under-score */
	    baclr(terms,'-') ;			/* allow minus-sign */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mkfieldterms) */


