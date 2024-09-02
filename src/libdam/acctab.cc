/* acctab SUPPORT */
/* lang=C++20 */

/* perform access table file related functions */
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
	This object processes an access table for use by daemon
	multiplexing server programs that want to control access
	to their sub-servers.

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
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


/* forward references */

static int	acctab_filefins(ACCTAB *) noex ;
static int	acctab_freeentries(ACCTAB *) noex ;
static int	acctab_fileparse(ACCTAB *,int) noex ;
static int	acctab_filedump(ACCTAB *,int) noex ;
static int	acctab_filedel(ACCTAB *,int) noex ;
static int	acctab_addentry() noex ;
static int	acctab_addentry(ACCTAB *,ACCTAB_ENT *) noex ;
static int	acctab_checkfiles(ACCTAB *,time_t) noex ;

static int	entry_start(ACCTAB_ENT *) noex ;
static int	entry_load(ACCTAB_ENT *,cchar *,cchar *,cchar *,cchar *) noex ;
static int	entry_mat2(ACCTAB_ENT *,ACCTAB_ENT *) noex ;
static int	entry_mat3(ACCTAB_ENT *,ACCTAB_ENT *) noex ;
static int	entry_finish(ACCTAB_ENT *) noex ;

static int	file_start(ACCTAB_FI *,cchar *) noex ;
static int	file_release(ACCTAB_FI *) noex ;
static int	file_finish(ACCTAB_FI *) noex ;

static int	part_start(ACCTAB_PA *) noex ;
static int	part_copy(ACCTAB_PA *,ACCTAB_PA *) noex ;
static int	part_compile(ACCTAB_PA *,cchar *,int) noex ;
static int	part_match(ACCTAB_PA *,cchar *) noex ;
static int	part_finish(ACCTAB_PA *) noex ;

static int	parttype(cchar *) noex ;
static int	cmpent(ACCTAB_ENT **,ACCTAB_ENT **) noex ;

static int	freeit(cchar **) noex ;


/* local static data */

constexpr cchar		group_terms[32] = {
	0x00, 0x1B, 0x00, 0x00,
	0x09, 0x00, 0x00, 0x04,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ;

constexpr cchar		arg_terms[32] = {
	0x00, 0x1B, 0x00, 0x00,
	0x09, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ;


/* exported variables */


/* exported subroutines */

int acctab_open(ACCTAB *op,cchar *fname) noex {
	int		rs ;
	int		sz ;
	int		vo ;

	if (op == NULL) return SR_FAULT ;

	memclear(op) ;
	op->checktime = time(NULL) ;

/* the vector of files should use a fixed position policy */

	sz = sizeof(struct acctab_file) ;
	vo = VECOBJ_OREUSE ;
	if ((rs = vecobj_start(&op->files,sz,10,vo)) >= 0) {
	    vo = VECITEM_PSORTED ;
	    if ((rs = vecitem_start(&op->aes_std,10,vo)) >= 0) {
	        vo = VECITEM_PNOHOLES ;
	        if ((rs = vecitem_start(&op->aes_rgx,10,vo)) >= 0) {
	            op->magic = ACCTAB_MAGIC ;
	            if (fname != NULL) {
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
	        vecobj_finish(&op->files) ;
	    }
	} /* end if (files) */

	return rs ;
}
/* end subroutine (acctab_open) */

int acctab_close(ACCTAB *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != ACCTAB_MAGIC) return SR_NOTOPEN ;

/* secondary items */

	rs1 = acctab_freeentries(op) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = acctab_filefins(op) ;
	if (rs >= 0) rs = rs1 ;

/* primary items */

	rs1 = vecitem_finish(&op->aes_std) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = vecitem_finish(&op->aes_rgx) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = vecobj_finish(&op->files) ;
	if (rs >= 0) rs = rs1 ;

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (acctab_close) */


/* add a file to the list of files */
int acctab_fileadd(ACCTAB *op,cchar *fname)
{
	ACCTAB_FI	fe ;
	int		rs = SR_OK ;
	int		fi ;
	cchar		*np ;
	char		tmpfname[MAXPATHLEN + 1] ;

	if (op == NULL) return SR_FAULT ;
	if (fname == NULL) return SR_FAULT ;

	if (op->magic != ACCTAB_MAGIC) return SR_NOTOPEN ;

	np = fname ;
	if (fname[0] != '/') {
	    char	pwdbuf[MAXPATHLEN+1] ;
	    if ((rs = getpwd(pwdbuf,MAXPATHLEN)) >= 0) {
	        np = tmpfname ;
	        rs = mkpath2(tmpfname,pwdbuf,fname) ;
	    }
	} /* end if (rooting file) */

	if (rs >= 0) {
	    if ((rs = file_start(&fe,np)) >= 0) {
		if ((rs = vecobj_add(&op->files,&fe)) >= 0) {
		    fi = rs ;
		    file_release(&fe) ;
		    rs = acctab_fileparse(op,fi) ;
		    if (rs < 0)
			acctab_filedel(op,fi) ;
		}
		if (rs < 0)
		    file_finish(&fe) ;
	    } /* end if (vecobj_add) */
	} /* end if (ok) */

	return rs ;
}
/* end subroutine (acctab_fileadd) */


/* search the netgroup table for a netgroup match */
int acctab_allowed(ACCTAB *op,cchar *ng,cchar *ma,cchar *un,cchar *pw)
{
	time_t		dt = time(NULL) ;
	cint	rsn = SR_NOTFOUND ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		f = FALSE ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != ACCTAB_MAGIC) return SR_NOTOPEN ;

	if (ma == NULL) return SR_INVALID ;

/* should we check for changes to underlying file? */

	if ((dt - op->checktime) > ACCTAB_MINCHECKTIME) {
	    rs = acctab_checkfiles(op,dt) ;
	} /* end if (we needed to check) */

	if (rs >= 0) {
	    ACCTAB_ENT	ae, *aep ;
	    VECITEM	*slp ;
	    VECITEM_CUR	cur ;
	    int		i ;

/* load up a fake entry for comparison purposes */

	    if ((rs = entry_start(&ae)) >= 0) {
	        if ((rs = entry_load(&ae,ng,ma,un,pw)) >= 0) {

/* search the STD entries first */

	    slp = &op->aes_std ;
	    if ((rs = vecitem_curbegin(slp,&cur)) >= 0) {

	        while ((rs1 = vecitem_fetch(slp,&ae,&cur,cmpent,&aep)) >= 0) {
	            if (aep != NULL) {
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
	        for (i = 0 ; (rs1 = vecitem_get(slp,i,&aep)) >= 0 ; i += 1) {
	            if (aep != NULL) {
	                f = entry_mat3(aep,&ae) ;
	                if (f) break ;
	            }
	        } /* end for (looping through entries) */
	        if ((rs >= 0) && (rs1 != rsn)) rs = rs1 ;
	    } /* end if (comparing RGX entries) */

	        } /* end if (entry_load) */
		entry_finish(&ae) ;
	    } /* end if () */

	} /* end if (ok) */

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (acctab_allowed) */


/* search the netgroup table for any netgroup match */
int acctab_anyallowed(ACCTAB *op,vecstr *ngp,vecstr *mnp,cchar *un,cchar *pw)
{
	cint	rsn = SR_NOTFOUND ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		rs2 ;
	int		i, j ;
	int		f = FALSE ;
	cchar		*np ; /* netgroup pointer */
	cchar		*mp ; /* machine pointer */

	if (op == NULL) return SR_FAULT ;

	if (op->magic != ACCTAB_MAGIC) return SR_NOTOPEN ;

	for (i = 0 ; (rs1 = vecstr_get(ngp,i,&np)) >= 0 ; i += 1) {
	    if (np != NULL) {
	        for (j = 0 ; (rs2 = vecstr_get(mnp,j,&mp)) >= 0 ; j += 1) {
	            if (mp != NULL) {
	                if ((rs = acctab_allowed(op,np,mp,un,pw)) >= 0) {
	                    f = TRUE ;
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

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (acctab_anyallowed) */


#ifdef	COMMENT

/* search the netgroup table for a netgroup match */
int acctab_find(op,netgroup,sepp)
ACCTAB		*op ;
cchar	netgroup[] ;
ACCTAB_ENT	**sepp ;
{
	VECITEM		*slp ;
	int		i ;
	cchar	*sp ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != ACCTAB_MAGIC) return SR_NOTOPEN ;

	slp = &op->e ;
	for (i = 0 ; vecitem_get(slp,i,sepp) >= 0 ; i += 1) {
	    if (*sepp == NULL) continue ;

	    sp = (*sepp)->netgroup ;

	    if (strcmp(netgroup,sp) == 0)
	        return i ;

	} /* end for (looping through entries) */

	return -1 ;
}
/* end subroutine (acctab_find) */

#endif /* COMMENT */


/* cursor manipulations */
int acctab_curbegin(ACCTAB *op,ACCTAB_CUR *cp)
{

	if (op == NULL) return SR_FAULT ;

	if (op->magic != ACCTAB_MAGIC) return SR_NOTOPEN ;

	cp->i = cp->j = -1 ;
	return SR_OK ;
}
/* end subroutine (acctab_curbegin) */


int acctab_curend(ACCTAB *op,ACCTAB_CUR *cp)
{

	if (op == NULL) return SR_FAULT ;

	if (op->magic != ACCTAB_MAGIC) return SR_NOTOPEN ;

	cp->i = cp->j = -1 ;
	return SR_OK ;
}
/* end subroutine (acctab_curend) */


/* enumerate the netgroup entries */
int acctab_enum(ACCTAB *op,ACCTAB_CUR *cp,ACCTAB_ENT **sepp)
{
	VECITEM		*slp ;
	ACCTAB_ENT	*aep ;
	int		rs = SR_OK ;
	int		j ;

	if (op == NULL) return SR_FAULT ;
	if (cp == NULL) return SR_FAULT ;

	if (op->magic != ACCTAB_MAGIC) return SR_NOTOPEN ;

	if (sepp == NULL)
	    sepp = &aep ;

	rs = SR_NOTFOUND ;
	if (cp->i <= 0) {

	    slp = &op->aes_std ;
	    cp->j = (cp->j < 0) ? 0 : (cp->j + 1) ;
	    for (j = cp->j ; (rs = vecitem_get(slp,j,sepp)) >= 0 ; j += 1) {
	        if (*sepp != NULL) break ;
	    } /* end for */

	    if (rs < 0) {
	        cp->j = -1 ;
	        cp->i = 1 ;
	    } else {
	        cp->i = 0 ;
	        cp->j = j ;
	    } /* end if */

	} /* end if (cursor needed initialization) */

	if (cp->i == 1) {

	    slp = &op->aes_rgx ;
	    cp->j = (cp->j < 0) ? 0 : cp->j + 1 ;
	    for (j = cp->j ; (rs = vecitem_get(slp,j,sepp)) >= 0 ; j += 1) {
	        if (*sepp != NULL) break ;
	    } /* end for */

	    if (rs < 0) {
	        cp->j = -1 ;
	        cp->i += 1 ;
	    } else {
	        cp->j = j ;
	    } /* end if */

	} /* end if (RGX entries) */

	return rs ;
}
/* end subroutine (acctab_enum) */

/* check if the access tables files have changed */
int acctab_check(ACCTAB *op) noex {
	custime		dt = time(NULL) ;
	int		rs = SR_OK ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != ACCTAB_MAGIC) return SR_NOTOPEN ;

/* should we even check? */

	if ((dt - op->checktime) > ACCTAB_CHECKTIME) {
	    rs = acctab_checkfiles(op,dt) ;
	}

	return rs ;
}
/* end subroutine (acctab_check) */


/* private subroutines */


/* free up all of the files in this ACCTAB list */
static int acctab_filefins(ACCTAB *op)
{
	ACCTAB_FI	*fep ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		i ;

	for (i = 0 ; vecobj_get(&op->files,i,&fep) >= 0 ; i += 1) {
	    if (fep != NULL) {
	        rs1 = file_finish(fep) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end for */

	return rs ;
}
/* end subroutine (acctab_filefins) */


/* check if the access table files have changed */
static int acctab_checkfiles(ACCTAB *op,time_t dt)
{
	struct ustat	sb ;
	ACCTAB_FI	*fep ;
	int		rs = SR_OK ;
	int		i ;
	int		c_changed = FALSE ;

	for (i = 0 ; vecobj_get(&op->files,i,&fep) >= 0 ; i += 1) {
	    if (fep != NULL) {
	    if ((u_stat(fep->fname,&sb) >= 0) &&
	        (sb.st_mtime > fep->mtime) &&
	        ((dt - sb.st_mtime) >= ACCTAB_CHANGETIME)) {

	        acctab_filedump(op,i) ;

	        rs = acctab_fileparse(op,i) ;

	        if (rs >= 0)
	            c_changed += 1 ;

	    } /* end if */
	    } /end if (non-NULL) */
	    if (rs < 0) break ;
	} /* end for */

	if ((rs >= 0) && c_changed) {
	    rs = vecitem_sort(&op->aes_std,cmpent) ;
	} /* end if (something changed) */
	op->checktime = dt ;
	return (rs >= 0) ? c_changed : rs ;
}
/* end subroutine (acctab_checkfiles) */

/* parse an access server file */
static int acctab_fileparse(ACCTAB *op,int fi) noex {
	PARTTYPE	netgroup ;
	bfile		file, *fp = &file ;
	ACCTAB_FI	*fep ;
	ACCTAB_ENT	se ;
	struct ustat	sb ;
	FIELD		fsb ;
	int		rs ;
	int		c ;
	int		cl, len ;
	int		line = 0 ;
	int		c_added = 0 ;
	cchar	*cp ;
	char		lbuf[LINEBUFLEN + 1] ;

	rs = vecobj_get(&op->files,fi,&fep) ;

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

	        if ((fl = field_get(&fsb,group_terms,&fp)) > 0) {

/* we have something! */

	            entry_start(&se) ;

	            se.fi = fi ;
	            if (fsb.term == ':') {
	                part_finish(&netgroup) ;
	                rs = part_compile(&netgroup,fp,fl) ;
	                fl = field_get(&fsb,arg_terms,&fp) ;
	            } /* end if */

/* see if there is a machine on this same line */

	            if (fl > 0) {

	                part_copy(&se.netgroup,&netgroup) ;

/* there is something else on this line */

	                rs = part_compile(&se.machine,fp,fl) ;

	                if (rs == SR_NOMEM)
	                    goto badpart ;

	                if ((fl = field_get(&fsb,arg_terms,&fp)) > 0) {

	                    rs = part_compile(&se.username,fp,fl) ;

	                    if (rs == SR_NOMEM)
	                        goto badpart ;

	                }

	                if ((fl = field_get(&fsb,arg_terms,&fp)) > 0) {

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


/* add an entry to the access entry list */
static int acctab_addentry(ACCTAB *op,ACCTAB_ENT *sep)
{
	int		rs ;

	if (parttype(sep->netgroup.std) == PARTTYPE_STD) {
	    rs = vecitem_add(&op->aes_std,sep,sizeof(ACCTAB_ENT)) ;
	} else {
	    rs = vecitem_add(&op->aes_rgx,sep,sizeof(ACCTAB_ENT)) ;
	}

	return rs ;
}
/* end subroutine (acctab_addentry) */


/* free up all of the entries in this ACCTAB list associated w/ a file */
static int acctab_filedump(ACCTAB *op,int fi)
{
	ACCTAB_ENT	*sep ;
	VECITEM		*slp ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		i, j ;

	for (j = 0 ; j < 2 ; j += 1) {

	    slp = (j == 0) ? &op->aes_std : &op->aes_rgx ;
	    for (i = 0 ; (rs = vecitem_get(slp,i,&sep)) >= 0 ; i += 1) {
	        if (sep != NULL) {

	            if ((sep->fi == fi) || (fi < 0)) {

	                rs1 = entry_finish(sep) ;
	                if (rs >= 0) rs = rs1 ;

	                rs1 = vecitem_del(slp,i--) ;
	                if (rs >= 0) rs = rs1 ;

	            } /* end if (found matching entry) */

	        }
	    } /* end for (looping through entries) */

	} /* end for (looping on the different types of entries) */

	return rs ;
}
/* end subroutine (acctab_filedump) */


static int acctab_filedel(ACCTAB *op,int fi)
{
	ACCTAB_FI	*fep ;
	int		rs ;

	if ((rs = vecobj_get(&op->files,fi,&fep)) >= 0) {
	    if (fep != NULL) {
	        file_finish(fep) ;
	        rs = vecobj_del(&op->files,fi) ;
	    }
	}

	return rs ;
}
/* end subroutine (acctab_filedel) */

static int acctab_freeentries(ACCTAB *op) noex {
	ACCTAB_ENT	*sep ;
	VECITEM		*slp ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		i, j ;

	for (j = 0 ; j < 2 ; j += 1) {
	    slp = (j == 0) ? &op->aes_std : &op->aes_rgx ;
	    for (i = 0 ; vecitem_get(slp,i,&sep) >= 0 ; i += 1) {
	        if (sep != NULL) {
	            rs1 = entry_finish(sep) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	    } /* end for */
	} /* end for */

	return rs ;
}
/* end subroutine (acctab_freeentries) */

static int file_start(ACCTAB_FI *fep,cchar *fname) noex {
	int		rs = SR_OK ;
	cchar		*cp ;

	if (fname == NULL) return SR_FAULT ;

	memclear(fep) ;

	if ((rs = uc_mallocstrw(fname,-1,&cp)) >= 0) {
	    fep->fname = cp ;
	}

	return rs ;
}
/* end subroutine (file_start) */

static int file_release(ACCTAB_FI *fep) noex {
	fep->fname = NULL ;
	return SR_OK ;
}
/* end subroutine (file_release) */

static int file_finish(ACCTAB_FI *fep) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (fep == NULL) return SR_FAULT ;

	if (fep->fname != NULL) {
	    rs1 = uc_free(fep->fname) ;
	    if (rs >= 0) rs = rs1 ;
	    fep->fname = NULL ;
	}

	return rs ;
}
/* end subroutine (file_finish) */

static int entry_start(ACCTAB_ENT *sep) noex {
	int		rs = SR_OK ;

	memclear(sep) ;
	sep->fi = -1 ;

	part_start(&sep->netgroup) ;

	part_start(&sep->machine) ;

	part_start(&sep->username) ;

	part_start(&sep->password) ;

	return rs ;
}
/* end subroutine (entry_start) */

static int entry_finish(ACCTAB_ENT *sep) noex {
	int		rs = SR_OK ;
	int		rs1 ;

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

	return rs ;
}
/* end subroutine (entry_finish) */


static int entry_load(aep,netgroup,machine,username,password)
ACCTAB_ENT	*aep ;
cchar	netgroup[] ;
cchar	machine[] ;
cchar	username[] ;
cchar	password[] ;
{

	aep->netgroup.std = (char *) netgroup ;
	if ((netgroup == NULL) || (netgroup[0] == '\0')) {
	    aep->netgroup.std = ACCTAB_DEFNETGROUP ;
	}

	aep->machine.std = (char *) machine ;
	aep->username.std = (char *) username ;
	aep->password.std = (char *) password ;

	return SR_OK ;
}
/* end subroutine (entry_load) */


/* compare if all but the netgroup are equal (or matched) */
static int entry_mat2(ACCTAB_ENT *e1p,ACCTAB_ENT *e2p) noex {

#ifdef	OPTIONAL
	if (! part_match(&e1p->netgroup,e2p->netgroup.std))
	    return FALSE ;
#endif

	if (! part_match(&e1p->machine,e2p->machine.std))
	    return FALSE ;

	if (! part_match(&e1p->username,e2p->username.std))
	    return FALSE ;

	if (! part_match(&e1p->password,e2p->password.std))
	    return FALSE ;

	return TRUE ;
}
/* end subroutine (entry_mat2) */


/* compare if all of the entry is equal (or matched) */
static int entry_mat3(ACCTAB_ENT *e1p,ACCTAB_ENT *e2p) noex {

	if (! part_match(&e1p->netgroup,e2p->netgroup.std))
	    return FALSE ;

	if (! part_match(&e1p->machine,e2p->machine.std))
	    return FALSE ;

	if (! part_match(&e1p->username,e2p->username.std))
	    return FALSE ;

	if (! part_match(&e1p->password,e2p->password.std))
	    return FALSE ;

	return TRUE ;
}
/* end subroutine (entry_mat3) */

static int part_start(PARTTYPE *pp) noex {
	pp->type = 0 ;
	pp->std = NULL ;
	pp->rgx = NULL ;
	return SR_OK ;
}
/* end subroutine (part_start) */

/* p1 gets loaded up (from p2) */
static int part_copy(PARTTYPE *p1p,PARTTYPE *p2p) noex {
	int		rs = SR_OK ;
	cchar		*cp ;

	memclear(p1p) ;

	p1p->type = p2p->type ;
	if (p2p->std != NULL) {
	    if ((rs = uc_mallocstrw(p2p->std,-1,&cp)) >= 0) {
	        p1p->std = cp ;
	    }
	} /* end if (copied STD) */

#if	ACCTAB_REGEX

	if (rs >= 0) {
	    if (p2p->rgx != NULL) {
	        if ((rs = uc_mallocstrw(p2p->rgx,p2p->rgxlen,&cp)) >= 0) {
	            p1p->rgx = cp ;
	            p1p->rgxlen = p2p->rgxlen ;
	        }
	    } /* end if (copied RGX) */
	} /* end if (ok) */

	if (rs < 0) {
	    if (p1p->std != NULL) {
	        uc_free(p1p->std) ;
	        p1p->std = NULL ;
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
	pp->std = NULL ;
	pp->rgx = NULL ;
	pp->rgxlen = 0 ;

	if ((rs = uc_mallocstrw(sp,sl,&cp)) >= 0) {
	    pp->std = cp ;

#if	ACCTAB_REGEX
	    if ((rs >= 0) && ((type = parttype(pp->std)) > PARTTYPE_STD)) {
	        char	*rgxbuf ;
	        char	*rp ;

	        if ((rs = uc_malloc(ACCTAB_RGXLEN,&rgxbuf)) >= 0) {

	            rp = compile(pp->std,rgxbuf,(rgxbuf + ACCTAB_RGXLEN)) ;

	            if (rp != NULL) {
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
	int		f = FALSE ;
	cchar	*cp ;

	if (pp->std == NULL) return TRUE ;

	if (s == NULL)
	    return FALSE ;

	if (pp->rgx == NULL) {

	    f = (strcmp(pp->std,s) == 0) ;
	    if (! f) {
	        f = starmat(pp->std,s) ;
	    }

	} else {

	    if (pp->type == 1) {

	        if ((cp = strchr(pp->std,'*')) != NULL) {

	            f = FALSE ;
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
	        f = FALSE ;
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
	if (*pp != NULL) {
	    rs = uc_free(*pp) ;
	    *pp = NULL ;
	}
	return rs ;
}
/* end subroutine (freeit) */

/* compare just the 'netgroup' part of entries (used for sorting) */
static int cmpent(ACCTAB_ENT **e1pp,ACCTAB_ENT **e2pp) noex {
	int		rc = 0 ;
	if ((*e1pp != NULL) || (*e2pp != NULL)) {
	    if (*e1pp != NULL) {
	        if (*e2pp != NULL) {
	    	    rc = strcmp((*e1pp)->netgroup.std,(*e2pp)->netgroup.std) ;
		} else
	    	    rc = -1 ;
	    } else
	        rc = 1 ;
	}
	return rc ;
}
/* end subroutine (cmpent) */

/* determine the type of this string part */
static int parttype(cchar *s) noex {
	int		type = PARTTYPE_STD ;
	bool		f = FALSE ;
	for ( ; *s ; s += 1) {
	    cint	ch = MKCHAR(*s) ;
	    f = f || isalnumlatin(ch) ;
	    f = f || (*s == '-') ;
	    f = f || (*s == '_') ;
	    if ((! f) && (*s == '*')) {
	        type = PARTTYPE_RGX ;
	        f = TRUE ;
	    }
	    if (! f) break ;
	} /* end for */
	return (*s == '\0') ? type : PARTTYPE_UNKNOWN ;
}
/* end subroutine (parttype) */


