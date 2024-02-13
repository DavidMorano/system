/* mkdirlist SUPPORT */
/* lang=C++20 */

/* create a list of the newsgroup directories */
/* version %I% last-modified %G% */


/* revision history:

	= 1994-04-13, David A­D­ Morano
	This is new so that we can track directory visits for
	duplicates.

	= 2014-11-25, David A­D­ Morano
	This object was enhanced to include much of the functionality
	from the old 'get_bds' function.

*/

/* Copyright © 2014 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Create a list of pathnames for each newsgroup in the spool
	area.  This routine is called with the path to the spool
	area directory.  The routine returns two open file pointers
	(Basic I/O).  These file pointers must be eventually closed
	by the calling routine or somebody!

	One returned file pointer is to a file of the path names.
	The other file pointer is a file of an array of 'stat(2)'
	structures corresponding to the directory specified by the
	path in the other file.

	Synopsis:
	int mkdirlist(char *basedir,bfile *sfp,bfile *nfp,int (*uf)()) noex

	Arguments:
	basedir		directory at top of tree
	nfp		Name File Pointer
	sfp		Stat File Pointer

	Returns:
	>=0		count
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<limits.h>
#include	<unistd.h>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<usupport.h>
#include	<bfile.h>
#include	<filebuf.h>
#include	<fsdirtree.h>
#include	<mkpathx.h>
#include	<pathadd.h>
#include	<isnot.h>
#include	<strwcmp.h>
#include	<mktmpfile.h>
#include	<localmisc.h>

#include	"mkdirlist.h"


/* local defines */

#define	MKDIRLIST_DIRCACHE	".dircache"
#define	MKDIRLIST_DCMAGIC	"DIRCACHE"

#define	DS_SIZE		sizeof(MKDIRLIST)

#ifndef	TMPDNAME
#define	TMPDNAME	"/tmp"
#endif


/* external subroutines */

extern int	openpcsdircache(cchar *,cchar *,int,mode_t,int) ;

extern int	bbcmp(cchar *,cchar *) ;


/* external variables */


/* local structures */


/* forward references */

static int mkdirlist_procdircache(MKDIRLIST *,cchar *,int) noex ;
static int mkdirlist_procnewsdir(MKDIRLIST *,cchar *) noex ;
static int mkdirlist_newentry(MKDIRLIST *,USTAT *,cchar *,int) noex ;
static int mkdirlist_entfins(MKDIRLIST *) noex ;

static int entry_start(MKDIRLIST_ENT *,USTAT *,cchar *,int) noex ;
static int entry_finish(MKDIRLIST_ENT *) noex ;
static int entry_showdef(MKDIRLIST_ENT *) noex ;
static int entry_show(MKDIRLIST_ENT *,cchar *,int) noex ;
static int entry_matung(MKDIRLIST_ENT *,cchar *,time_t,int,int) noex ;

static int ordercmp(MKDIRLIST_ENT **,MKDIRLIST_ENT **) noex ;


/* local variables */


/* exported subroutines */

int mkdirlist_start(MKDIRLIST *op,cchar *pr,cchar *newsdname) noex {
	int	rs ;
	int	c = 0 ;

	if (op == NULL) return SR_FAULT ;
	if (pr == NULL) return SR_FAULT ;
	if (newsdname == NULL) return SR_FAULT ;

	if (pr[0] == '\0') return SR_INVALID ;
	if (newsdname[0] == '\0') return SR_INVALID ;

	memclear(op) ;

	if ((rs = vechand_start(&op->dirs,20,0)) >= 0) {
	    const mode_t	om = 0666 ;
	    cint		of = O_RDONLY ;
	    if ((rs = openpcsdircache(pr,newsdname,of,om,-1)) >= 0) {
	        int	fd = rs ;
	        rs = mkdirlist_procdircache(op,newsdname,fd) ;
	        c = rs ;
	        u_close(fd) ;
	    } else if (isNotPresent(rs)) {
	        rs = mkdirlist_procnewsdir(op,newsdname) ;
	        c = rs ;
	    }
	    if (rs >= 0) op->magic = MKDIRLIST_MAGIC ;
	    if (rs < 0)
	        mkdirlist_entfins(op) ;
	    if (rs < 0)
	        vechand_finish(&op->dirs) ;
	} /* end if (vechand) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mkdirlist_start) */


int mkdirlist_finish(MKDIRLIST *op)
{
	int	rs = SR_OK ;
	int	rs1 ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != MKDIRLIST_MAGIC) return SR_NOTOPEN ;

	rs1 = mkdirlist_entfins(op) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = vechand_finish(&op->dirs) ;
	if (rs >= 0) rs = rs1 ;

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (mkdirlist_finish) */


int mkdirlist_get(MKDIRLIST *op,int i,MKDIRLIST_ENT **epp)
{
	int		rs ;

	if (op == NULL) return SR_FAULT ;
	if (epp == NULL) return SR_FAULT ;

	if (op->magic != MKDIRLIST_MAGIC) return SR_NOTOPEN ;

	rs = vechand_get(&op->dirs,i,epp) ;

	return rs ;
}
/* end subroutine (mkdirlist_get) */


int mkdirlist_link(MKDIRLIST *op)
{
	MKDIRLIST_ENT	*pep, *ep ;
	vechand		*dlp ;
	int		rs = SR_OK ;
	int		i, j ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != MKDIRLIST_MAGIC) return SR_NOTOPEN ;

	dlp = &op->dirs ;
	for (i = 0 ; vechand_get(dlp,i,&ep) >= 0 ; i += 1) {
	    if (ep == NULL) continue ;

	    if (! ep->f.link) {
	        pep = ep ;

	        for (j = (i+1) ; vechand_get(dlp,j,&ep) >= 0 ; j += 1) {
	            if (ep == NULL) continue ;

	            if ((! ep->f.link) && (bbcmp(pep->name,ep->name) == 0)) {

	                pep->link = ep ;
	                ep->f.link = true ;

	                pep = ep ;
	            } /* end if (board match) */

	        } /* end for (inner) */

	    } /* end if (entry not linked) */

	} /* end for (linking like entries) */

	return rs ;
}
/* end subroutine (mkdirlist_link) */


int mkdirlist_showdef(MKDIRLIST *op)
{
	MKDIRLIST_ENT	*ep ;
	int		rs = SR_OK ;
	int		i ;
	int		c = 0 ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != MKDIRLIST_MAGIC) return SR_NOTOPEN ;

	for (i = 0 ; vechand_get(&op->dirs,i,&ep) >= 0 ; i += 1) {
	    if (ep != NULL) {
	        rs = entry_showdef(ep) ;
	        c += rs ;
	    }
	    if (rs < 0) break ;
	} /* end for */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mkdirlist_showdef) */


int mkdirlist_show(MKDIRLIST *op,cchar *ng,int order)
{
	MKDIRLIST_ENT	*ep ;
	int		rs = SR_OK ;
	int		i ;
	int		c = 0 ;

	if (op == NULL) return SR_FAULT ;
	if (ng == NULL) return SR_FAULT ;

	if (op->magic != MKDIRLIST_MAGIC) return SR_NOTOPEN ;
	if (ng[0] == '\0') return SR_INVALID ;

	for (i = 0 ; vechand_get(&op->dirs,i,&ep) >= 0 ; i += 1) {
	    if (ep == NULL) continue ;
	    rs = entry_show(ep,ng,order) ;
	    c += rs ;
	    if (rs < 0) break ;
	} /* end for */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mkdirlist_show) */


int mkdirlist_ung(op,ung,utime,f_sub,order)
MKDIRLIST	*op ;
cchar	ung[] ;
time_t		utime ;
int		f_sub ;
int		order ;
{
	MKDIRLIST_ENT	*ep ;
	VECHAND		*dlp ;
	int		rs = SR_OK ;
	int		i ;
	int		c = 0 ;

	if (op == NULL) return SR_FAULT ;
	if (ung == NULL) return SR_FAULT ;

	if (op->magic != MKDIRLIST_MAGIC) return SR_NOTOPEN ;
	if (ung[0] == '\0') return SR_INVALID ;

	dlp = &op->dirs ;
	for (i = 0 ; vechand_get(dlp,i,&ep) >= 0 ; i += 1) {
	    if (ep == NULL) continue ;
	    rs = entry_matung(ep,ung,utime,f_sub,order) ;
	    c += rs ;

	    if (rs < 0) break ;
	} /* end for (looping through entrires) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mkdirlist_ung) */


int mkdirlist_sort(MKDIRLIST *op)
{
	int	rs ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != MKDIRLIST_MAGIC) return SR_NOTOPEN ;

	rs = vechand_sort(&op->dirs,ordercmp) ;

	return rs ;
}
/* end subroutine (mkdirlist_sort) */


int mkdirlist_audit(op)
MKDIRLIST	*op ;
{
	VECHAND		*dlp ;
	int		rs = SR_OK ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != MKDIRLIST_MAGIC) return SR_NOTOPEN ;

	dlp = &op->dirs ;
	rs = vechand_audit(dlp) ;

	return rs ;
}
/* end subroutine (mkdirlist_audit) */


/* private subroutines */

static int mkdirlist_procdircache(MKDIRLIST *op,cchar *newsdname,int fd) noex {
	int	rs ;
	int	c = 0 ;
	cchar	*dcm = MKDIRLIST_DCMAGIC ;
	char	dbuf[MAXPATHLEN+1] ;

	if ((rs = mkpath1(dbuf,newsdname)) >= 0) {
	    FILEBUF	b ;
	    int	dlen = rs ;

	    if ((rs = filebuf_start(&b,fd,0L,0,0)) >= 0) {
	        USTAT	sb ;
	        const int	nlen = MAXPATHLEN ;
	        int	line = 0 ;
	        int	f_bol = true ;
	        int	f_eol ;
	        char	nbuf[MAXPATHLEN+1] ;
	        while ((rs = filebuf_readline(&b,nbuf,nlen,-1)) > 0) {
	            int	len = rs ;
	            f_eol = (len && (nbuf[len-1] == '\n')) ;
	            if (f_eol) nbuf[--len] = '\0' ;
	            if (f_bol) {

	                if (line++ == 0) {
	                    if (strwcmp(dcm,nbuf,len) != 0) {
	                        rs = SR_BADFMT ;
	                    }
	                } else {
	                    if ((rs = pathadd(dbuf,dlen,nbuf)) >= 0) {
	                        if (u_stat(dbuf,&sb) >= 0) {
	                            rs = mkdirlist_newentry(op,&sb,nbuf,len) ;
	                            c += rs ;
	                        }
	                    }
	                }
	            } /* end if (BOL) */
	            f_bol = f_eol ;
	            if (rs < 0) break ;
	        } /* end while */
	        filebuf_finish(&b) ;
	    } /* end if */

	} /* end if (mkpath) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mkdirlist_procdircache) */


static int mkdirlist_procnewsdir(op,newsdname)
MKDIRLIST	*op ;
cchar	newsdname[] ;
{
	FSDIRTREE	dir ;

	int	rs ;
	int	opts = 0 ;
	int	c = 0 ;

	opts = (FSDIRTREE_MFOLLOW | FSDIRTREE_MDIR) ;
	if ((rs = fsdirtree_open(&dir,newsdname,opts)) >= 0) {
	    USTAT	sb ;
	    const int	nglen = MAXPATHLEN ;
	    char	ngdname[MAXPATHLEN+1] ;

	    while ((rs = fsdirtree_read(&dir,&sb,ngdname,nglen)) > 0) {
	        int	ngl = rs ;

	        if (ngdname[0] != '.') {
	            rs = mkdirlist_newentry(op,&sb,ngdname,ngl) ;
	            c += rs ;
	        }

	        if (rs < 0) break ;
	    } /* end while */

	    fsdirtree_close(&dir) ;
	} /* end if (fsdirtree) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mkdirlist_procnewsdir) */


static int mkdirlist_newentry(op,sbp,nbuf,nlen)
MKDIRLIST	*op ;
USTAT	*sbp ;
cchar	nbuf[] ;
int		nlen ;
{
	const int	esize = sizeof(MKDIRLIST_ENT) ;
	int	rs ;
	int	c = 0 ;
	void	*p ;

	if ((rs = uc_malloc(esize,&p)) >= 0) {
	    MKDIRLIST_ENT	*ep = (MKDIRLIST_ENT *) p ;
	    if ((rs = entry_start(ep,sbp,nbuf,nlen)) > 0) { /* rs>0 */
	        c = rs ;
	        rs = vechand_add(&op->dirs,ep) ;
	        if (rs < 0)
	            entry_finish(ep) ;
	    } /* end if (entry) */
	    if ((rs < 0) || (c == 0))
	        uc_free(p) ;
	} /* end if (memory-allocation) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mkdirlist_newentry) */

static int mkdirlist_entfins(MKDIRLIST *op) noex {
	VECHAND		*dlp = &op->dirs ;
	MKDIRLIST_ENT	*ep ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		i ;

	for (i = 0 ; vechand_get(dlp,i,&ep) >= 0 ; i += 1) {
	    if (ep == NULL) continue ;
	    rs1 = entry_finish(ep) ;
	    if (rs >= 0) rs = rs1 ;
	    rs1 = uc_free(ep) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end for */

	return rs ;
}
/* end subroutine (mkdirlist_entfins) */


static int entry_start(ep,sbp,dbuf,dlen)
MKDIRLIST_ENT	*ep ;
USTAT	*sbp ;
cchar	*dbuf ;
int		dlen ;
{
	int		rs ;
	int		c = 0 ;
	cchar		*cp ;
	memclear(ep) ;
	if ((rs = uc_mallocstrw(dbuf,dlen,&cp)) >= 0) {
	    cint	nlen = rs ;
	    ep->name = cp ;
	    c += 1 ;
	    ep->nlen = nlen ;
	    ep->mode = sbp->st_mode ;
	    ep->mtime = sbp->st_mtime ;
	    ep->ino = sbp->st_ino ;
	    ep->dev = sbp->st_dev ;
	} /* end if (memory-allocation) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (entry_start) */

static int entry_finish(MKDIRLIST_ENT *ep) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (ep->name) {
	    rs1 = uc_free(ep->name) ;
	    if (rs >= 0) rs = rs1 ;
	    ep->name = NULL ;
	}
	return rs ;
}
/* end subroutine (entry_finish) */


static int entry_matung(ep,ung,utime,f_sub,order)
MKDIRLIST_ENT	*ep ;
cchar	ung[] ;
time_t		utime ;
int		f_sub ;
int		order ;
{
	int		rs = SR_OK ;
	if (! ep->f.link) {
	    if (bbcmp(ung,ep->name) == 0) {
	        rs = 1 ;
	        ep->f.seen = true ;
	        ep->f.subscribe = f_sub ;
	        ep->utime = utime ;
		ep->order = order ;
	    } /* end if (name match) */
	} /* end if (not a linked entry) */
	return rs ;
}
/* end subroutine (entry_matung) */

static int entry_showdef(MKDIRLIST_ENT *ep) noex {
	int		rs = SR_OK ;
	if (!ep->f.link) {
	    ep->f.show = ep->f.subscribe ;
	    if (ep->f.show) rs = 1 ;
	}
	return rs ;
}
/* end subroutine (entry_showdef) */

static int entry_show(MKDIRLIST_ENT *ep,cchar *ng,int order) noex {
	int		rs = SR_OK ;
	if (! ep->f.link) {
	    if (bbcmp(ng,ep->name) == 0) {
		ep->order = order ;
	        ep->f.show = true ;
	        rs = 1 ;
	    } /* end if (name match) */
	} /* end if (not a linked entry) */
	return rs ;
}
/* end subroutine (entry_show) */

static int ordercmp(MKDIRLIST_ENT **e1pp,MKDIRLIST_ENT **e2pp) noex {
	MKDIRLIST_ENT	*e1p = *e1pp ;
	MKDIRLIST_ENT	*e2p = *e2pp ;
	int		rc = 0 ;
	if ((e1p != NULL) || (e2p != NULL)) {
	    if (e1p != NULL) {
	        if (e2p != NULL) {
		    rc = (e1p->order - e2p->order) ;
		} else
		    rc = -1 ;
	    } else
		rc = 1 ;
	}
	return rc ;
}
/* end subroutine (ordercmp) */


