/* paramopt SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* paramater option manipulations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-01, David A­D­ Morano
	This code module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	paramopt

	Description:
	This code module contains subroutines used to add paramters
	to parameter lists and such for later access.  Although meant
	to be relatively straight forward, there are several aspects
	of this object that are tailored towards special uses that
	are not at all obvious.

	All parameter names and values are stored in freshly allocated
	memory.  The original storage for parameter names and values
	can be freed after they are stored using these routines.

	One of the more strange uses of this object (and actually
	the reason that it was written) is to gather parameters
	with values but to also organize them in a way that all
	combinations of parameter values can be exponentially
	enumerated.  This is useful for programs that want to
	enumerate all possible combinations of user supplied values
	of some sort.

	This object stores a funny sort of key-value(s) pairs.  For
	each key, multiple values can be stored along with the same
	key.  Multiple apparent entries with the same key are stored
	in the SAME record.  This is unlike regular database objects
	that will store multiple entries with the same key in
	different records.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<estrings.h>
#include	<strn.h>
#include	<strwcpy.h>
#include	<strwcmp.h>
#include	<char.h>
#include	<localmisc.h>

#include	"paramopt.h"


/* local defines */

#define	PO		paramopt
#define	PO_CUR		paramopt_cur
#define	PO_NAME		paramopt_name
#define	PO_VAL		paramopt_val


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	paramopt_findkey(PO *,cchar *,PO_NAME **) noex ;

static int	name_incri(PO_NAME *) noex ;
static int	name_vfind(PO_NAME *,cchar *,int,PO_VAL **) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int paramopt_start(PO *op) noex {
    	PARAMOPT	*hop = op ;
	int		rs = SR_FAULT ;
	if (op) {
	    if ((rs = memclear(hop)) >= 0) {
	        op->head = nullptr ;
	        op->tail = nullptr ;
	        op->magic = PARAMOPT_MAGIC ;
	        op->f_inited = true ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (paramopt_start) */

int paramopt_finish(PO *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = paramopt_magic(op)) >= 0) {
	    PO_NAME	*ap, *nnp ;
	    PO_VAL	*vp, *nvp ;
	    for (ap = op->head ; ap != nullptr ; ap = nnp) {
	        for (vp = ap->head ; vp != nullptr ; vp = nvp) {
	            if (vp->value != nullptr) {
	                rs1 = uc_free(vp->value) ;
		        if (rs >= 0) rs = rs1 ;
		    }
	            nvp = vp->next ;
	            rs1 = uc_free(vp) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end for */
	        if (ap->name != nullptr) {
	            rs1 = uc_free(ap->name) ;
		    if (rs >= 0) rs = rs1 ;
	        }
	        nnp = ap->next ;
	        rs1 = uc_free(ap) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end for */
	    op->head = nullptr ;
	    op->tail = nullptr ;
	    op->f_inited = false ;
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (paramopt_finish) */

/* load a parameter with an unknown "name" and values all in string 's' */

/****

This loads someting that looks like:
	key=v1,v2,v3,
Notice that the keyname is extrcted from the supplied string.

****/

int paramopt_loadu(PO *op,cchar *sp,int sl) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = paramopt_magic(op,sp)) >= 0) {
	    if (sl < 0) sl = strlen(sp) ;
	    if (int si ; (si = sibrk(sp,sl,"=\t")) >= 0) {
	        cchar	*cp{} ;
	        if (int cl ; (cl = sfshrink((sp + si),(sl- si),&cp)) > 0) {
		    if (char *name{} ; (rs = uc_malloc((cl+1),&name)) >= 0) {
			{
		            strwcpy(name,cp,cl) ;
	                    si += 1 ;
	                    rs = paramopt_loads(op,name,(sp + si),(sl- si)) ;
	                    c += rs ;
			}
		        rs1 = uc_free(name) ;
			if (rs >= 0) rs = rs1 ;
		    } /* end if (memory-allocation) */
	        } /* end if */
	    } else {
	        rs = SR_INVALID ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (paramopt_loadu) */

/* load parameter with a known "name" and string of values (in 's') */

/****

This loads someting that looks like:
	v1,v2,v3,
given that a keyname is specified explicitly.

****/

int paramopt_loads(PO *op,cchar *name,cchar *sp,int sl) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = paramopt_magic(op,name,sp)) >= 0) {
	    if (sl < 0) sl = strlen(sp) ;
	    for (int si ; (si = sibrk(sp,sl," :,\t\r\n\v\f")) >= 0 ; ) {
	        cchar	*cp = (sp+si) ;
	        if (si > 0) {
	            rs = paramopt_load(op,name,sp,si) ;
	            c += rs ;
	        }
	        sp = (cp + 1) ;
	        sl -= (si + 1) ;
	        if (rs < 0) break ;
	    }  /* end for */
	    if ((rs >= 0) && (sl > 0)) {
	        rs = paramopt_load(op,name,sp,sl) ;
	        c += rs ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (paramopt_loads) */

/* load a single key=value pair */
int paramopt_load(PO *op,cchar *name,cchar *vbuf,int vlen) noex {
	int		rs ;
	int		f = false ;
	if ((rs = paramopt_magic(op,name,vbuf)) >= 0) {
	    rs = SR_INVALID ;
	    if (name[0]) {
	        cchar	*vp{} ;
		rs = SR_OK ;
	        if (vlen < 0) vlen = strlen(vbuf) ;
	        if (int vl ; (vl = sfshrink(vbuf,vlen,&vp)) > 0) {
	            PO_VAL	*ovp ;
	            PO_VAL	*nvp ;
	            PO_NAME	*pp ;
	            cint	rsn = SR_NOTFOUND ;
	            int		nsz ;
	            cchar	*cp ;
	            while ((vl > 0) && 
	                (CHAR_ISWHITE(vp[vl - 1]) || (vp[vl - 1] == ','))) {
	                vl -= 1 ;
	            }
		    /* do we have one of these named keys already? */
	            if ((rs = paramopt_findkey(op,name,&pp)) == rsn) {
			/* make a new parameter header block, insert at head */
	                nsz = szof(PO_NAME) ;
	                if ((rs = uc_malloc(nsz,&pp)) >= 0) {
	                    pp->c = 0 ;
	                    pp->next = op->head ;	/* insert at head */
	                    pp->head = nullptr ;
	                    pp->tail = nullptr ;
	                    pp->current = nullptr ;
	                    if ((rs = uc_mallocstrw(name,-1,&cp)) >= 0) {
	                        pp->name = cp ;
	                        op->head = pp ;	/* insert at head */
	                    }
	                    if (rs < 0) {
	                        uc_free(pp) ;
		            }
	                } /* end if */
	            } /* end if (adding a new parameter block on the list) */
		    /* OK, now we have the parameter block in 'pp' */
	            if (rs >= 0) {
	                if ((rs = name_vfind(pp,vp,vl,&ovp)) == rsn) {
	                    cint	vsz = szof(PO_VAL) ;
	                    f = true ;
	                    if ((rs = uc_malloc(vsz,&nvp)) >= 0) {
	                        nvp->next = nullptr ;
	                        if ((rs = uc_mallocstrw(vp,vl,&cp)) >= 0) {
	                            nvp->value = cp ;
	                            ovp = pp->tail ;
	                            if (pp->head == nullptr) {
	                                pp->head = nvp ;
			            }
	                            pp->c += 1 ;
	                            pp->tail = nvp ;
	                            pp->current = nvp ;
	                            if (ovp) {
	                                ovp->next = nvp ;
			            }
	                        }
	                        if (rs < 0) {
	                            uc_free(nvp) ;
			        }
	                    } /* end if (new value) */
		        } /* end if (name_vfind) */
	            } /* end if (ok) */
	        } /* end if (non-zero) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (paramopt_load) */

int paramopt_loaduniq(PO *op,cchar *name,cchar *vp,int vl) noex {
	int		rs ;
	if ((rs = paramopt_haveval(op,name,vp,vl)) == 0) {
	    rs = paramopt_load(op,name,vp,vl) ;
	}
	return rs ;
}
/* end subroutine (paramopt_loaduniq) */

int paramopt_havekey(PO *op,cchar *name) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = paramopt_magic(op,name)) >= 0) {
	    rs = SR_INVALID ;
	    if (name[0]) {
	        if (PO_NAME *pp ; (rs = paramopt_findkey(op,name,&pp)) >= 0) {
	            c = pp->c ;
	        } else if (rs == SR_NOTFOUND) {
	            rs = SR_OK ;
	        }
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (paramopt_havekey) */

int paramopt_curenumkeys(PO *op,PO_CUR *curp,cchar **rpp) noex {
	int		rs ;
	int		kl = 0 ;
	if ((rs = paramopt_magic(op,curp)) >= 0) {
	    PO_NAME	*kp ;
	    if (rpp) {
	        *rpp = nullptr ;
	    }
	    if (curp->keyp == nullptr) {
	        kp = op->head ;	/* get the next key-pointer */
	        curp->valp = nullptr ;
	    } else {
	        kp = curp->keyp ;	/* get the current key-pointer */
	        kp = kp->next ;		/* get the next key-pointer */
	    }
	    if (kp != nullptr) {
	        curp->keyp = kp ;	/* update to the current key-pointer */
	        if (rpp != nullptr) *rpp = kp->name ;
	        kl = strlen(kp->name) ;
	    } else {
	        rs = SR_NOTFOUND ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? kl : rs ;
}
/* end subroutine (paramopt_curenumkeys) */

int paramopt_fetch(PO *op,cchar *key,PO_CUR *curp,cchar **rpp) noex {
	int		rs ;
	int		vl = 0 ;
	if ((rs = paramopt_magic(op,key)) >= 0) {
	    rs = SR_INVALID ;
	    if (key[0]) {
	        PO_NAME	*kp = nullptr ;
	        PO_VAL	*vp = nullptr ;
	        PO_CUR	ncur ;
	        if (curp == nullptr) {
	            curp = &ncur ;
	            curp->keyp = nullptr ;
	            curp->valp = nullptr ;
	        }
	        if (rpp) *rpp = nullptr ;
		/* do we have this key? */
	        if (curp->keyp == nullptr) {
	            if ((rs = paramopt_findkey(op,key,&kp)) >= 0) {
	                curp->keyp = kp ;
	                vp = kp->head ;
	            }
	        } else {
	            kp = curp->keyp ;
	            vp = (curp->valp)->next ;
	        } /* end if */
	        if ((rs >= 0) && (kp && vp)) {
	            if (rpp) *rpp = vp->value ;
	            if (vp->value) vl = strlen(vp->value) ;
	            curp->valp = vp ;
	        } else {
	            rs = SR_NOTFOUND ;
	        }
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (paramopt_fetch) */

int paramopt_enumvalues(PO *op,cchar *key,PO_CUR *curp,cchar **rpp) noex {
	return paramopt_fetch(op,key,curp,rpp) ;
}
/* end subroutine (paramopt_enumvalues) */

int paramopt_haveval(PO *op,cchar *key,cchar *vp,int vl) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = paramopt_magic(op,key,vp)) >= 0) {
	    rs = SR_INVALID ;
	    if (key[0]) {
	        PO_CUR	cur ;
	        if ((rs = paramopt_curbegin(op,&cur)) >= 0) {
		    auto	pe = paramopt_enumvalues ;
	            int		f = 0 ;
	            cchar	*pvp{} ;
	            while ((rs1 = pe(op,key,&cur,&pvp)) >= 0) {
	                if (pvp) {
	                    f = (strwcmp(pvp,vp,vl) == 0) ;
	                    c += f ;
		        }
	            } /* end while */
	            if ((rs >= 0) && (rs1 != SR_NOTFOUND)) rs = rs1 ;
	            rs1 = paramopt_curend(op,&cur) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (paramopt-cur) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (paramopt_haveval) */

/* increment the parameters */
int paramopt_incr(PO *op) noex {
	int		rs ;
	if ((rs = paramopt_magic(op)) >= 0) {
	    PO_NAME	*pp = op->head ;
	    PO_VAL	*vp ;
	    rs = SR_NOTFOUND ;
	    if (pp->next) {
	        rs = name_incri(pp->next) ;
	    }
	    /* increment ourselves as may be appropriate */
	    if (rs < 0) {
	        vp = pp->current ;
	        if (vp->next == nullptr) {
	            pp->current = pp->head ;
	            rs = -1 ;
	        } else {
	            pp->current = vp->next ;
	            rs = 0 ;
	        }
	    } /* end if (error) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (paramopt_incr) */

/* initialize a cursor */
int paramopt_curbegin(PO *op,PO_CUR *curp) noex {
	int		rs ;
	if ((rs = paramopt_magic(op,curp)) >= 0) {
	    curp->keyp = nullptr ;
	    curp->valp = nullptr ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (paramopt_curbegin) */

/* free up a cursor */
int paramopt_curend(PO *op,PO_CUR *curp) noex {
	int		rs ;
	if ((rs = paramopt_magic(op,curp)) >= 0) {
	    curp->keyp = nullptr ;
	    curp->valp = nullptr ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (paramopt_curend) */

/* find the number of values for a given key */
int paramopt_countvals(PO *op,cchar *key) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = paramopt_magic(op,key)) >= 0) {
	    rs = SR_INVALID ;
	    if (key[0]) {
	        PO_NAME	*kp{} ;
	        if ((rs = paramopt_findkey(op,key,&kp)) >= 0) {
	            c = kp->c ;
	        } else if (rs == SR_NOTFOUND) {
	            rs = SR_OK ;
	        }
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (paramopt_countvals) */


/* private subroutines */

/* find a parameter by key-name */
static int paramopt_findkey(PO *op,cc *name,PO_NAME **rpp) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = paramopt_magic(op,name)) >= 0) {
	    PO_NAME	*pp ;
	    for (pp = op->head ; pp != nullptr ; pp = pp->next) {
	        if (strcmp(pp->name,name) == 0) break ;
	    } /* end for */
	    if (pp) c = pp->c ;
	    if (rpp) *rpp = pp ;
	    if (pp == nullptr) rs = SR_NOTFOUND ;
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (paramopt_findkey) */

#ifdef	COMMENT

/* find a paramter by key-value pair */
static int paramopt_findvalue(PO *op,cc *key,cc *val,int vlen,
		PO_VAL **rpp) noex {
	int		rs ;
	if ((rs = paramopt_magic(op)) >= 0) {
	    PO_NAME	*kp{} ;
	    if (vlen < 0) vlen = strlen(val) ;
	    if ((rs = paramopt_findkey(op,key,&kp)) >= 0) {
	        rs = name_vfind(kp,val,vlen,rpp) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (paramopt_findvalue) */

#endif /* COMMENT */

static int name_incri(PO_NAME *pp) noex {
	PO_VAL		*vp ;
	int		rs = SR_NOSYS ;
	if (pp->next != nullptr) {
	    rs = name_incri(pp->next) ;
	}
	/* increment ourselves as may be appropriate */
	if (rs < 0) {
	    vp = pp->current ;
	    if (vp->next == nullptr) {
	        pp->current = pp->head ;
	        rs = -1 ;
	    } else {
	        pp->current = vp->next ;
	        rs = 0 ;
	    }
	}
	return rs ;
}
/* end subroutine (name_incri) */

/* find a paramter by value? */
static int name_vfind(PO_NAME *pp,cchar *vp,int vl,PO_VAL **rpp) noex {
	PO_VAL		*vep = nullptr ;
	int		c = 0 ;
	bool		f = false ;
	if (vl < 0) vl = strlen(vp) ;
	for (vep = pp->head ; vep ; vep = vep->next) {
	    f = (strncmp(vep->value,vp,vl) == 0) ;
	    f = f && (vep->value[vl] == '\0') ;
	    if (f) break ;
	    c += 1 ;
	} /* end for */
	if (rpp) *rpp = vep ;
	return (f) ? c : SR_NOTFOUND ;
}
/* end subroutine (name_vfind) */


