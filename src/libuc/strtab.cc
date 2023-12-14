/* strtab */
/* lang=C++20 */

/* string table object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-24, David A�D� Morano
	This object module was morphed from some previous one. I
	do not remember what the previous one was.

	= 2023-11-07, David A-D- Morano
	:-) It is now years later. Yes, I have to laugh at how long
	it has been. Let me calculate ... (2023-1998) ... OK, not
	that bad, only 25 years ago! :-) 1998 seemed like yesterday!
	In fact the 20 years prior to 1998 seemed like yesterday!
	I do not really know why I am laughing right now.  Some of
	this surrounding code dates from the early 1980s. I crack
	up a little bit realizing just how long some code pieces
	can last -- and stay somewhat relevant.  I remember sitting
	at my desk back in late 1995 or so, and remembering about
	code that I had written in the 1980s or early 1990s. I
	remember thinking that any code that I write (at that time)
	needed to possibly last a long time (and therefore should
	be carefully coded for longevity).  I thought this because
	I was reminiscing back then at how long the code that I had
	written many years before (the prior 13 years or so) had
	already lasted and was still being used at that time.  This
	is even more funny now, bcause some of the container objects
	(and other codes pieces, like a lot of string manipulation
	stuff) that I still have and use (now) date from the early
	and mid-1980s.  OK, now to the serious stuff. I came back
	in here looking to see what it looked like (just in general)
	and found a preprocessor symbol |STRTAB_ALLOCOBJ|. That
	symbol controlled which of two allocators this present
	object (STRTAB) would use to allocate whatever (something
	to do with its internal guts, maybe the "chunks"). If that
	symbol was set (to something non-zero) an allocator object
	named |allocobj| would be used. Otherwise the allocator
	object |lookaside| would be used.  It was set to '0' so
	that the |lookaside| object was being used. I do remember
	(vaguely) writing that |allocobj| object (yes, a long time
	ago). But on to the funny (weird) part: I can no longer
	find that old |allocobj| object ANYWHERE! :-)  Again, I am
	not sure why I am laughing.  It does not even appear to be
	in any of the old archives I have checked (albeit I have
	not been totally exhausive in searching through them).
	Somehow through the years, that old object (|allocobj|) has
	been lost. I can only take a wild guess at what must have
	(somehow) happened.  I am going to guess that this present
	object (|strtab|) was the last one to even reference the
	old |allocobj| object. And since this present |strtab| had
	the use of that allocation object compile-time compiled
	OUT, it left open the possibility for the |allocobj| object
	to somehow get deleted from the code base without it ever
	becoming apparent (through any possible compilations of
	various code modules) that a deletion occurred. OK, now to
	what I actually changed here (after that rather long
	introduction).  I removed the preprocessor symbol
	|STRTAB_ALLOCOBJ| along with any references (symbols or
	otherwise) of the old |allocobj| object. So it is no longer
	possible to compile with the choice of the two old allocators.
	Now only the |lookaside| object is used for internal
	allocations. Whew.  That is it. Postscript: Am I sorry for
	the loss of the old |allocobj| object.  Yes, of course I
	am.  I am curious about what was in that old object.  But
	apparently, at some point it was (or I) deemed the |lookaside|
	object to be a better (presumably faster) allocator. So
	maybe I should not mourn the loss of that too excessively.

*/

/* Copyright � 1998,2023 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strtab

	Description:
	This object module creates and manages a string table object.
	This string table can later be written to a file or otherwise
	stored some way so that it can be used in-place later. This
	is often useful for cache files or ELF code object files.

	Arguments:
	stp		pointer to the strtab object
	<others>

	Returns:
	>=0		the total length of the filled up strtab so far!
	<0		error

        Note: 
	The index table (optionally) generated by this OBJECT using
	�hash-linking� to track down matches. It does not use
	�key-linking�.  searching the generated index table must
	also use

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<unistd.h>
#include	<cstdlib>
#include	<cstring>
#include	<new>
#include	<usystem.h>
#include	<usupport.h>
#include	<vecobj.h>
#include	<sysval.hh>
#include	<localmisc.h>
#include	<intceil.h>
#include	<hash.h>
#include	<strwcpy.h>

#include	"strtab.h"


/* local defines */

#define	STRTAB_STARTSIZE	512	/* starting table size */
#define	STRTAB_NSKIP		5

#define	STRTAB_AOSTART(o,es,n)	lookaside_start((o),(es),(n))
#define	STRTAB_AOGET(o,pp)	lookaside_get((o),(pp))
#define	STRTAB_AOREL(o,pp)	lookaside_release((o),(pp))
#define	STRTAB_AOFINISH(o)	lookaside_finish((o))

#define	MODP2(v,n)	((v) & ((n) - 1))

#define	VALBOGUS	(250000 * 10)

#define	STRENTRY	struct strentry


/* local namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */

extern "C" {
    typedef int (*vhg_f)(vechand *,int,void **) noex ;
    typedef int (*vog_f)(vecobj *,int,void **) noex ;
}


/* external subroutines */

extern "C" {
    extern int	nextpowtwo(int) noex ;
}


/* local structures */

struct strentry {
	uint	khash ;
	uint	hi ;
	uint	si ;
} ;


/* forward references */

template<typename ... Args>
static inline int strtab_ctor(strtab *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    nullptr_t	np{} ;
	    rs = SR_NOMEM ;
	    op->magic = 0 ;
	    op->ccp = nullptr ;
	    op->chsize = 0 ;
	    op->stsize = 0 ;
	    op->count = 0 ;
	    if ((op->clp = new(nothrow) vechand) != np) {
		if ((op->hlp = new(nothrow) hdb) != np) {
		    if ((op->lap = new(nothrow) lookaside) != np) {
			rs = SR_OK ;
		    } /* end if (new-lookaside) */
		    if (rs < 0) {
			delete op->hlp ;
			op->hlp = nullptr ;
		    }
	        } /* end if (new-hdb) */
		if (rs < 0) {
		    delete op->clp ;
		    op->clp = nullptr ;
		}
	    } /* end if (new-vechand) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (strtab_ctor) */

static int strtab_dtor(strtab *op) noex {
	int		rs = SR_OK ;
	if (op->lap) {
	    delete op->lap ;
	    op->lap = nullptr ;
	}
	if (op->hlp) {
	    delete op->hlp ;
	    op->hlp = nullptr ;
	}
	if (op->clp) {
	    delete op->clp ;
	    op->clp = nullptr ;
	}
	return rs ;
}
/* end subroutine (strtab_dtor) */

template<typename ... Args>
static inline int strtab_magic(strtab *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == STRTAB_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (strtab_magic) */

static int	strtab_stuff(strtab *,cchar *,int) noex ;
static int	strtab_finishchunks(strtab *) noex ;
static int	strtab_extend(strtab *,int) noex ;
static int	strtab_newchunk(strtab *,int) noex ;

static int	chunk_start(strtab_ch *,int,int) noex ;
static int	chunk_finish(strtab_ch *) noex ;
static int	chunk_check(strtab_ch *,int) noex ;
static int	chunk_add(strtab_ch *,cchar *,int,cchar **) noex ;


/* local variables */

static sysval		pagesize(sysval_ps) ;


/* local subroutines */

static inline int indexlen(int n) noex {
	return nextpowtwo(n) ;
}
/* end subroutine (indexlen) */

static inline int indexsize(int il) noex {
	return ((il + 1) * 3 * sizeof(int)) ;
}
/* end subroutine (indexsize) */

static inline int hashindex(uint hv,int n) noex {
	int		hi = MODP2(hv,n) ;
	if (hi == 0) hi = 1 ;
	return hi ;
}
/* end subroutine (hashindex) */


/* exported subroutines */

int strtab_start(strtab *op,int startsz) noex {
	int		rs ;
	if ((rs = strtab_ctor(op)) >= 0) {
	    if (startsz < STRTAB_STARTSIZE) startsz = STRTAB_STARTSIZE ;
	    if ((rs = pagesize) >= 0) {
		cint	ne = (startsz / 4) ;
		int	opts = 0 ;
	        op->chsize = iceil(startsz,pagesize) ;
	        if ((rs = vechand_start(op->clp,ne,opts)) >= 0) {
		    nullptr_t	n{} ;
	            if ((rs = hdb_start(op->hlp,ne,true,n,n)) >= 0) {
	                cint	size = sizeof(int) ;
	                if ((rs = STRTAB_AOSTART(op->lap,size,ne)) >= 0) {
	                    op->count = 0 ;
	                    if ((rs = strtab_newchunk(op,0)) >= 0) {
	                        op->magic = STRTAB_MAGIC ;
	                    }
	                    if (rs < 0) {
	                        STRTAB_AOFINISH(op->lap) ;
			    }
	                } /* end if (strab-aostart) */
	                if (rs < 0) {
		            hdb_finish(op->hlp) ;
			}
	            } /* end if (hdb-start) */
	            if (rs < 0) {
	                vechand_finish(op->clp) ;
		    }
	        } /* end if (vechand-start) */
	    } /* end if (pagesize) */
	    if (rs < 0) {
		strtab_dtor(op) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (strtab_start) */

int strtab_finish(strtab *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = strtab_magic(op)) >= 0) {
            {
                rs1 = STRTAB_AOFINISH(op->lap) ;
                if (rs >= 0) rs = rs1 ;
            }
            {
                rs1 = hdb_finish(op->hlp) ;
                if (rs >= 0) rs = rs1 ;
            }
            {
                rs1 = strtab_finishchunks(op) ;
                if (rs >= 0) rs = rs1 ;
            }
            {
                rs1 = vechand_finish(op->clp) ;
                if (rs >= 0) rs = rs1 ;
            }
	    {
                rs1 = strtab_dtor(op) ;
                if (rs >= 0) rs = rs1 ;
            }
            op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (strtab_finish) */

int strtab_add(strtab *op,cchar *sp,int sl) noex {
	int		rs ;
	int		vi = 0 ;
	if ((rs = strtab_magic(op,sp)) >= 0) {
	    if (sl < 0) sl = strlen(sp) ;
	    {
		hdb_dat	key{} ;
		hdb_dat	val{} ;
	        key.buf = sp ;
	        key.len = sl ;
	        if ((rs = hdb_fetch(op->hlp,key,nullptr,&val)) >= 0) {
	            int		*ip = (int *) val.buf ;
	            vi = *ip ;
	        } else if (rs == SR_NOTFOUND) {
	            rs = strtab_stuff(op,sp,sl) ;
	            vi = rs ;
	        } /* end if */
	    } /* end block */
	} /* end if (magic) */
	return (rs >= 0) ? vi : rs ;
}
/* end subroutine (strtab_add) */

int strtab_addfast(strtab *op,cchar *sp,int sl) noex {
	int		rs ;
	int		vi = 0 ;
	if ((rs = strtab_magic(op,sp)) >= 0) {
	    if (sl < 0) sl = strlen(sp) ;
		rs = strtab_stuff(op,sp,sl) ;
		vi = rs ;
	} /* end if (magic) */
	return (rs >= 0) ? vi : rs ;
}
/* end subroutine (strtab_addfast) */

int strtab_already(strtab *op,cchar *sp,int sl) noex {
	int		rs ;
	int		vi = 0 ;
	if ((rs = strtab_magic(op,sp)) >= 0) {
	    if (sl < 0) sl = strlen(sp) ;
	    {
	        hdb_dat	key{} ;
	        hdb_dat	val{} ;
	        key.buf = sp ;
	        key.len = sl ;
	        if ((rs = hdb_fetch(op->hlp,key,nullptr,&val)) >= 0) {
	            int		*ip = (int *) val.buf ;
	            vi = *ip ;
	        } /* end if */
	    } /* end block */
	} /* end if (magic) */
	return (rs >= 0) ? vi : rs ;
}
/* end subroutine (strtab_already) */

int strtab_count(strtab *op) noex {
	int		rs ;
	if ((rs = strtab_magic(op)) >= 0) {
		rs = op->count ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (strtab_count) */

int strtab_strsize(strtab *op) noex {
	int		rs ;
	if ((rs = strtab_magic(op)) >= 0) {
		rs = iceil(op->stsize,sizeof(int)) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (strtab_strsize) */

int strtab_strmk(strtab *op,char *tabdata,int tabsize) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = strtab_magic(op,tabdata)) >= 0) {
		rs = SR_INVALID ;
		if (tabsize > 0) {
		    cint	stsize = iceil(op->stsize,sizeof(int)) ;
	   	    rs = SR_OVERFLOW ;
		    if (tabsize >= stsize) {
			vechand		*vlp = op->clp ;
	    		char		*bp = tabdata ;
			vhg_f		vg = vechand_get ;
			void		*vp{} ;
			rs = SR_OK ;
			c = op->count ;
	    		for (int i = 0 ; vg(vlp,i,&vp) >= 0 ; i += 1) {
	    		    strtab_ch	*ccp = (strtab_ch *) vp ;
	        	    if (ccp) {
	            		if (ccp->cdata) {
	                	    memcpy(bp,ccp->cdata,ccp->cl) ;
	                	    bp += ccp->cl ;
	            		}
	        	    }
	    		} /* end for */
	    		while (bp < (tabdata + tabsize)) {
	        	    *bp++ = '\0' ;
	    		}
		    } /* end if (tabsize OK) */
		} /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (strtab_strmk) */

int strtab_recsize(strtab *op) noex {
	int		rs ;
	if ((rs = strtab_magic(op)) >= 0) {
		cint	n = (op->count + 1) ;
		rs = (n + 1) * sizeof(int) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (strtab_recsize) */

int strtab_recmk(strtab *op,int *rec,int recsize) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = strtab_magic(op,rec)) >= 0) {
	    cint	n = (op->count + 1) ;
	    {
	        cint	rsize = (n + 1) * sizeof(int) ;
	        rs = SR_OVERFLOW ;
	        if (recsize >= rsize) {
	            HDB_CUR	cur{} ;
	            int		*ip ;
	            rec[c++] = 0 ;	/* ZERO-entry is NUL-string */
	            if ((rs = hdb_curbegin(op->hlp,&cur)) >= 0) {
	                hdb_dat	key{} ;
	                hdb_dat	val{} ;
	                while (hdb_enum(op->hlp,&cur,&key,&val) >= 0) {
	                    ip = (int *) val.buf ;
	                    rec[c++] = *ip ;
	                } /* end while (looping through strings) */
	                hdb_curend(op->hlp,&cur) ;
	            } /* end if */
	            rec[c] = -1 ;
	        } /* end if (tabsize OK) */
	    } /* end block */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (strtab_recmk) */

int strtab_indlen(strtab *op) noex {
	int		rs ;
	if ((rs = strtab_magic(op)) >= 0) {
		rs = indexlen(op->count + 1) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (strtab_indlen) */

int strtab_indsize(strtab *op) noex {
	int		rs ;
	if ((rs = strtab_magic(op)) >= 0) {
	        cint	il = indexlen(op->count + 1) ;
	        rs = indexsize(il) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (strtab_indsize) */

int strtab_indmk(strtab *op,int (*it)[3],int itsize,int nskip) noex {
	int		rs ;
	int		rs1 ;
	int		sc = 0 ;
	if ((rs = strtab_magic(op,it)) >= 0) {
	    int		isize ;
	    if (nskip < 0) nskip = 0 ;
	    {
		cint	il = indexlen(op->count + 1) ;
		isize = indexsize(il) ;
		rs = SR_OVERFLOW ;
		if (itsize >= isize) {
	            vecobj	ses ;
		    cint	vo = VECOBJ_OCOMPACT ;
		    cint	esize = sizeof(STRENTRY) ;
		    memclear(it,isize) ;
		    if ((rs = vecobj_start(&ses,esize,op->count,vo)) >= 0) {
	    	        STRENTRY	se ;
	    	        HDB_CUR		cur ;
	    	        hdb_dat	key{} ;
	    	        hdb_dat	val{} ;
	    	        uint		khash, chash, nhash ;
	    	        int		lhi, nhi, hi, si ;
	                if ((rs = hdb_curbegin(op->hlp,&cur)) >= 0) {
			    hdb		*hp = op->hlp ;
		            int		*ip ;
		            int		sl ;
		            cchar	*sp ;
	                    while ((sl = hdb_enum(hp,&cur,&key,&val)) >= 0) {
	                        sp = (cchar *) key.buf ;
	                        sl = key.len ;
	                        ip = (int *) val.buf ;
	                        si = *ip ;
	                        khash = hash_elf(sp,sl) ;
	                        hi = hashindex(khash,il) ;
	                        if (it[hi][0] == 0) {
	                            it[hi][0] = si ;
	                            it[hi][1] = (khash & INT_MAX) ;
	                            it[hi][2] = 0 ;
	                            sc += 1 ;
	                        } else {
	                            se.khash = khash ;
	                            se.si = si ;
	                            se.hi = hi ;
	                            rs = vecobj_add(&ses,&se) ;
	                        } /* end if */
	                        if (rs < 0) break ;
	                    } /* end while (looping through strings) */
	                    hdb_curend(op->hlp,&cur) ;
	                } /* end if (cursor) */
	                if (rs >= 0) {
			    vog_f	v = vecobj_get ;
			    int		c ;
			    void	*vp{} ;
	                    for (int i = 0 ; v(&ses,i,&vp) >= 0 ; i += 1) {
	    	        	STRENTRY	*sep = (STRENTRY *) vp ;
	                        khash = sep->khash ;
	                        si = sep->si ;
	                        hi = sep->hi ;
	                        chash = (khash & INT_MAX) ;
	                        nhash = khash ;
	                        c = 0 ;
	                        while (it[hi][0] > 0) {
	                            if ((it[hi][1] & INT_MAX) == chash) break ;
	                            it[hi][1] |= (~ INT_MAX) ;
	                            nhash = hash_again(nhash,c,nskip) ;
	                            hi = hashindex(nhash,il) ;
	                            c += 1 ;
	                        } /* end while */
	                        sc += c ;
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
	                        } /* end while */
	                        it[hi][0] = si ;
	                        it[hi][1] = chash ;
	                        it[hi][2] = 0 ;
	                    } /* end for */
	                    it[il][0] = -1 ;
	                    it[il][1] = 0 ;
	                    it[il][2] = 0 ;
	                    if (sc < 0) sc = 0 ;
	                } /* end if */
	                rs1 = vecobj_finish(&ses) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (vecobj) */
		} /* end if (valid) */
	    } /* end block */
	} /* end if (magic) */
	return (rs >= 0) ? sc : rs ;
}
/* end subroutine (strtab_indmk) */


/* private subroutines */

static int strtab_stuff(strtab *op,cchar *sp,int sl) noex {
	cint		amount = (sl + 1) ;
	int		rs ;
	int		vi = 0 ;
	if ((rs = strtab_extend(op,amount)) >= 0) {
	    cchar	*vp = nullptr ;
	    vi = op->stsize ;
	    if ((rs = chunk_add(op->ccp,sp,sl,&vp)) >= 0) {
		int	*ip ;
	        if ((rs = STRTAB_AOGET(op->lap,&ip)) >= 0) {
		    hdb_dat	key{} ;
		    hdb_dat	val{} ;
	            *ip = vi ;
	            key.buf = vp ;
	            key.len = sl ;
	            val.buf = ip ;
	            val.len = sizeof(int) ;
	            if ((rs = hdb_store(op->hlp,key,val)) >= 0) {
	                op->stsize += amount ;
	                op->count += 1 ;
	            }
	        } /* end if (AOGET) */
	    } /* end if (chunk-add) */
	} /* end if (extend) */
	return (rs >= 0) ? vi : rs ;
}
/* end subroutine (strtab_stuff) */

static int strtab_finishchunks(strtab *op) noex {
	vechand		*clp = op->clp ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ;
	void		*vp{} ;
	for (int i = 0 ; vechand_get(clp,i,&vp) >= 0 ; i += 1) {
	    strtab_ch	*ccp = (strtab_ch *) vp ;
	    if (ccp) {
	        c += 1 ;
		{
	            rs1 = chunk_finish(ccp) ;
	            if (rs >= 0) rs = rs1 ;
		}
		{
	            rs1 = uc_free(ccp) ;
	            if (rs >= 0) rs = rs1 ;
		}
	    }
	} /* end for */
	op->ccp = nullptr ;
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (strtab_finishchunks) */

static int strtab_extend(strtab *op,int amount) noex {
	int		rs = SR_OK ;
	if (op->ccp) {
	    cint	rso = SR_OVERFLOW ;
	    if ((rs = chunk_check(op->ccp,amount)) == rso) {
		rs = SR_OK ;
	        op->ccp = nullptr ;
	    }
	} /* end if (tried to extend a chunk) */
	if ((rs >= 0) && (op->ccp == nullptr)) {
	    rs = strtab_newchunk(op,amount) ;
	}
	return rs ;
}
/* end subroutine (strtab_extend) */

static int strtab_newchunk(strtab *op,int amount) noex {
	cint		size = sizeof(strtab_ch) ;
	int		rs ;
	int		start = 0 ;
	if (amount < op->chsize) amount = op->chsize ;
	op->ccp = nullptr ;
	if ((rs = uc_malloc(size,&op->ccp)) >= 0) {
	    if (op->stsize == 0) {
	        op->stsize = 1 ;
	        start = 1 ;
	    }
	    if ((rs = chunk_start(op->ccp,amount,start)) >= 0) {
		rs = vechand_add(op->clp,op->ccp) ;
		if (rs < 0)
		    chunk_finish(op->ccp) ;
	    }
	    if (rs < 0) {
	        uc_free(op->ccp) ;
	        op->ccp = nullptr ;
	    }
	} /* end if (memory-allocation) */
	return rs ;
}
/* end subroutine (strtab_newchunk) */

static int chunk_start(strtab_ch *ccp,int chsize,int start) noex {
	static constexpr int	minsize = STRTAB_MINCHUNKSIZE ;
	int		rs ;
	if (chsize < minsize) chsize = minsize ;
	memclear(ccp) ;
	if (start > chsize) chsize = start ;
	if ((rs = uc_malloc(chsize,&ccp->cdata)) >= 0) {
	    ccp->csz = chsize ;
	    ccp->cdata[0] = '\0' ;
	    ccp->cl = start ;
	} /* end if (m-a) */
	return rs ;
}
/* end subroutine (chunk_start) */

static int chunk_finish(strtab_ch *ccp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (ccp->cdata) {
	    rs1 = uc_free(ccp->cdata) ;
	    if (rs >= 0) rs = rs1 ;
	    ccp->cdata = nullptr ;
	}
	ccp->csz = 0 ;
	ccp->cl = 0 ;
	return rs ;
}
/* end subroutine (chunk_finish) */

static int chunk_check(strtab_ch *ccp,int amount) noex {
	int		rs = SR_OK ;
	if (amount > (ccp->csz - ccp->cl)) {
	    rs = SR_OVERFLOW ;
	}
	return rs ;
}
/* end subroutine (chunk_check) */

static int chunk_add(strtab_ch *ccp,cchar *sp,int sl,cchar **spp) noex {
	cint		amount = (sl + 1) ;
	int		rs = SR_OK ;
	if (amount <= (ccp->csz - ccp->cl)) {
	    char	*bp = (ccp->cdata + ccp->cl) ;
	    *spp = bp ;
	    strwcpy(bp,sp,sl) ;
	    ccp->cl += amount ;
	    ccp->count += 1 ;
	} else {
	    *spp = nullptr ;
	    rs = SR_NOANODE ;
	}
	return rs ;
}
/* end subroutine (chunk_add) */


