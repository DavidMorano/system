/* systems SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* get machine dialing information from UUCP "Systems" DB */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was adopted for use from the DWD program.

	- 2003-11-04, David A­D­ Morano
	I don't know where all this has been (apparently "around")
	but I grabbed it from the SYSTEMS object!

*/

/* Copyright © 1998,2003 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	systems

	Description:
	This object is a container for "systems" listed in "systems"
	files.  These are files that are keyed by the name of a
	"system" (whatever that is) and has a composite value that
	is one or more strings.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
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
#include	<bfile.h>
#include	<field.h>
#include	<vecobj.h>
#include	<localmisc.h>

#include	"systems.h"


/* local defines */

#define	SYS_FILE	systems_file
#define	CUR		systems_cur
#define	ENT		systems_ent

#define	TI_SYS_FILECHECK	3
#define	SYSLINELEN	((4 * 1024) + 1)
#define	ARGSBUFLEN	((6 * MAXPATHLEN) + 35)


/* external subroutines */


/* external variables */


/* local structures */

struct systems_file {
	cchar		*fname ;
	dev_t		dev ;
	ino_t		ino ;
	time_t		mtime ;
	size_t		fsize ;
} ;

typedef systems_file *	filep ;


/* forward references */

static int systems_fileparse(systems *,int,SYS_FILE *) noex ;
static int systems_filealready(systems *,dev_t,ino_t) noex ;
static int systems_procline(systems *,int,field *) noex ;
static int systems_delfes(systems *,int) noex ;

static int file_start(SYS_FILE *,cchar *) noex ;
static int file_finish(SYS_FILE *) noex ;

static int entry_start(ENT *,int,cchar *,int) noex ;
static int entry_dialer(ENT *,cchar *,int) noex ;
static int entry_args(ENT *,cchar *,int) noex ;
static int entry_finish(ENT *) noex ;

static int bdumpline(bfile *,char *,int) noex ;


/* local variables */

/* entry field terminators */
constexpr cchar		fterms[32] = {
	0x00, 0x00, 0x00, 0x00,
	0x08, 0x10, 0x00, 0x24,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ;

/* entry argument terminators (just '#' to provide "remainder" function) */
constexpr cchar		remterms[32] = {
	0x00, 0x00, 0x00, 0x00,
	0x08, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ;


/* exported variables */


/* exported subroutines */

int systems_open(systems *op,cchar *sysfname) noex {
	int		rs ;
	int		sz ;
	int		opts ;

	if (op == nullptr) return SR_FAULT ;

	sz = sizeof(SYS_FILE) ;
	opts = VECOBJ_OREUSE ;
	if ((rs = vecobj_start(&op->files,sz,10,opts)) >= 0) {
	    sz = sizeof(ENT) ;
	    opts = 0 ;
	    if ((rs = vecobj_start(&op->entries,sz,20,opts)) >= 0) {
	        op->magic = SYSTEMS_MAGIC ;
	        if (sysfname != nullptr) {
	            rs = systems_fileadd(op,sysfname) ;
	        }
	        if (rs < 0) {
		    op->magic = 0 ;
		    vecobj_finish(&op->entries) ;
	        }
	    }
	    if (rs < 0) {
	       vecobj_finish(&op->files) ;
	    }
	} /* end if (vecobj_start) */

	return rs ;
}
/* end subroutine (systems_open) */

int systems_close(systems *op) noex {
	ENT		*dep ;
	SYS_FILE	*fep ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		i ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != SYSTEMS_MAGIC) return SR_NOTOPEN ;

/* free up all enties */

	for (i = 0 ; vecobj_get(&op->entries,i,&dep) >= 0 ; i += 1) {
	    if (dep != nullptr) {
	        rs1 = entry_finish(dep) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end for */

	rs1 = vecobj_finish(&op->entries) ;
	if (rs >= 0) rs = rs1 ;

/* free up the files */

	for (i = 0 ; vecobj_get(&op->files,i,&fep) >= 0 ; i += 1) {
	    if (fep != nullptr) {
	        if (fep->fname != nullptr) {
	            rs1 = uc_free(fep->fname) ;
		    if (rs >= 0) rs = rs1 ;
	        }
	    }
	} /* end for */

	rs1 = vecobj_finish(&op->files) ;
	if (rs >= 0) rs = rs1 ;

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (systems_close) */

int systems_fileadd(systems *op,cchar *sysfname) noex {
	int		rs = SR_OK ;
	cchar		*sp ;
	char		tmpfname[MAXPATHLEN + 1] ;

	if ((op == nullptr) || (sysfname == nullptr)) return SR_FAULT ;

	if (op->magic != SYSTEMS_MAGIC) return SR_NOTOPEN ;

/* make a file pathname if necessary */

	sp = sysfname ;
	if (sysfname[0] != '/') {
	    char	pwdbuf[MAXPATHLEN+1] ;
	    if ((rs = getpwd(pwdbuf,MAXPATHLEN)) >= 0) {
	        sp = tmpfname ;
	        rs = mkpath2(tmpfname,pwdbuf,sysfname) ;
	    }
	} /* end if */

	if (rs >= 0) {
	    SYS_FILE	fe, *fep ;
	    if ((rs = file_start(&fe,sp)) >= 0) {
	        if ((rs = vecobj_add(&op->files,&fe)) >= 0) {
	            int		fi = rs ;
	            if ((rs = vecobj_get(&op->files,fi,&fep)) >= 0) {
	                rs = systems_fileparse(op,fi,fep) ;
	                if ((rs < 0) && (rs != SR_EXIST)) {
		            vecobj_del(&op->files,fi) ;
	                }
	            }
	            if (rs < 0) {
		        vecobj_del(&op->files,fi) ;
		    }
	        } /* end if (vecobj_add) */
	        if (rs < 0) {
		    file_finish(&fe) ;
		}
	    } /* end if (file_start) */
	} /* end if (ok) */

	return rs ;
}
/* end subroutine (systems_fileadd) */

int systems_curbegin(systems *op,CUR *curp) noex {
	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magic != SYSTEMS_MAGIC) return SR_NOTOPEN ;

	curp->i = -1 ;
	return SR_OK ;
}
/* end subroutine (systems_curbegin) */

int systems_curend(systems *op,CUR *curp) noex {
	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magic != SYSTEMS_MAGIC) return SR_NOTOPEN ;

	curp->i = -1 ;
	return SR_OK ;
}
/* end subroutine (systems_curend) */

int systems_enum(systems *op,CUR *curp,ENT **depp) noex {
	int		rs ;
	int		ei ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;
	if (depp == nullptr) return SR_FAULT ;

	if (op->magic != SYSTEMS_MAGIC) return SR_NOTOPEN ;

	ei = (curp->i < 0) ? 0 : (curp->i + 1) ;

	if ((rs = vecobj_get(&op->entries,ei,depp)) >= 0)
	    curp->i = ei ;

	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (systems_enum) */

int systems_fetch(systems *op,cchar *name,CUR *curp,ENT **depp) noex {
	int		rs ;
	int		ei ;

	if (op == nullptr) return SR_FAULT ;
	if (name == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;
	if (depp == nullptr) return SR_FAULT ;

	if (op->magic != SYSTEMS_MAGIC) return SR_NOTOPEN ;

	ei = (curp->i < 0) ? 0 : (curp->i + 1) ;

	while ((rs = vecobj_get(&op->entries,ei,depp)) >= 0) {
	    if (*depp == nullptr) continue ;

	    if (starmat((*depp)->sysname,name))
	        break ;

	    ei += 1 ;

	} /* end while */

	if (rs >= 0)
	    curp->i = ei ;

	return (rs >= 0) ? ei : rs ;
}
/* end subroutine (systems_fetch) */

int systems_check(systems *op,time_t dt) noex {
	USTAT		sb ;
	SYS_FILE	*fep ;
	int		rs = SR_OK ;
	int		i ;
	int		c = 0 ;
	int		f ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != SYSTEMS_MAGIC) return SR_NOTOPEN ;

	if (dt <= 0)
	    dt = time(nullptr) ;

/* should we even check? */

	if ((dt - op->checktime) <= TI_SYS_FILECHECK)
	    return SR_OK ;

	op->checktime = dt ;

/* loop through files looking for a change */

	for (i = 0 ; vecobj_get(&op->files,i,&fep) >= 0 ; i += 1) {
	    if (fep == nullptr) continue ;

	    rs = u_stat(fep->fname,&sb) ;

	    f = ((rs < 0) || (sb.st_mtime > fep->mtime) || 
	        (sb.st_size != fep->fsize)) ;

	    if (f) {

	        c += 1 ;
	        systems_delfes(op,i) ;

	        if (rs >= 0)
	            systems_fileparse(op,i,fep) ;

	    } /* end if (file changed) */

	} /* end for */

	return c ;
}
/* end subroutine (systems_check) */


/* private subroutines */

static int systems_fileparse(systems *op,int fi,SYS_FILE *fep) noex {
	bfile		dialfile, *sfp = &dialfile ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = bopen(sfp,fep->fname,"r",0664)) >= 0) {
	    if (USTAT sb ; (rs = bcontrol(sfp,BC_STAT,&sb)) >= 0) {
		const dev_t	dev = sb.st_dev ;
		const ino_t	ino = sb.st_ino ;
		if ((rs = systems_filealready(op,dev,ino)) == 0) {
		    cint	llen = SYSLINELEN ;
		    char	*lbuf ;
		    if ((rs = uc_malloc((llen+1),&lbuf)) >= 0) {
		        field	fsb ;
		        int	len ;
		        fep->mtime = sb.st_mtime ;
		        fep->fsize = size_t(sb.st_size & UINT_MAX) ;
		        fep->dev = sb.st_dev ;
		        fep->ino = sb.st_ino ;
		        while ((rs = breadlns(sfp,lbuf,llen,nullptr)) > 0) {
	    	            len = rs ;

	    	        if (len == 1) continue ;	/* blank line */

	    	        if (lbuf[len - 1] != '\n') {
			    bdumpline(sfp,lbuf,llen) ;
	        	    continue ;
	    	        }

	    		len -= 1 ;
	    		if ((len == 0) || (lbuf[0] == '#')) continue ;

	    	        if ((rs = field_start(&fsb,lbuf,len)) >= 0) {
			    int		fl ;
			    cchar	*fp ;

	        	    fl = field_get(&fsb,fterms,&fp) ;

	        	    if ((fl > 0) && (fsb.term != '#')) {
	            	        rs = systems_procline(op,fi,&fsb) ;
	            	        if (rs > 0) c += 1 ;
	        	    } /* end if */

	        	    field_finish(&fsb) ;
	    	        } /* end if (field) */

	                if (rs < 0) break ;
	            } /* end while (reading lines) */
			rs1 = uc_free(lbuf) ;
			if (rs >= 0) rs = rs1 ;
		    } /* end if (m-a) */
	        } /* end if (not-already) */
	    } /* end if (bcontrol) */
	    rs1 = bclose(sfp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (file) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (systems_fileparse) */

static int systems_filealready(systems *op,dev_t dev,ino_t ino) noex {
	SYS_FILE	*fep ;
	vecobj		*flp = &op->files ;
	int		rs ;
	int		f = false ;
	for (int i = 0 ; (rs = vecobj_get(flp,i,&fep)) >= 0 ; i += 1) {
	    if (fep) {
		f = ((fep->dev == dev) && (fep->ino == ino)) ;
		if (f) break ;
	    }
	} /* end for */
	if (rs == SR_NOTFOUND) rs = SR_OK ;
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (systems_filealready) */

static int systems_procline(systems *op,int fi,field *fsp) noex {
	int		rs = SR_OK ;
	int		f = false ;
	if ((fsp->fl > 0) && (fsp->term != '#')) {
	    int		fl = fsp->fl ;
	    cchar	*fp = fsp->fp ;
	    if (ENT e ; (rs = entry_start(&e,fi,fp,fl)) >= 0) {
		bool	f_fin = true ;
	        if ((fl = field_get(fsp,fterms,&fp)) > 0) {
	            if ((rs = entry_dialer(&e,fp,fl)) >= 0) {
	                if (fsp->term != '#') {
	                    cint	alen = ARGSBUFLEN ;
			    cchar	*ft = remterms ;
	                    char	abuf[ARGSBUFLEN + 1] ;
	                    if ((rs = field_srvarg(fsp,ft,abuf,alen)) >= 0) {
	                	cint	al = rs ;
	                	if ((rs = entry_args(&e,abuf,al)) >= 0) {
				    vecobj	*elp = &op->entries ;
	                    	    if ((rs = vecobj_add(elp,&e)) >= 0) {
					f = true ;
					f_fin = false ;
				    }
			        }
	             	    } /* end if (field_srvarg) */
	                } /* end if (not a comment) */
	            } /* end if (entry_dialer) */
	        } /* end if (field_get) */
	        if ((rs < 0) || f_fin)
		    entry_finish(&e) ;
	    } /* end if (entry_start) */
	} /* end if (possible) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (systems_procline) */

static int systems_delfes(systems *op,int fi) noex {
	ENT		*ep ;
	vecobj		*elp = &op->entries ;
	int		rs = SR_OK ;
	int		rs1 ;
	for (int i = 0 ; vecobj_get(elp,i,&ep) >= 0 ; i += 1) {
	    if (ep) {
	        if (ep->fi == fi) {
		    {
	                rs1 = entry_finish(ep) ;
		        if (rs >= 0) rs = rs1 ;
		    }
		    {
	                rs1 = vecobj_del(&op->entries,i--) ;
		        if (rs >= 0) rs = rs1 ;
		    }
	        }
	    }
	} /* end for */
	return rs ;
}
/* end subroutine (systems_delfes) */

static int file_start(SYS_FILE *fep,cchar *fname) noex {
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

static int file_finish(SYS_FILE *fep) noex {
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

static int entry_start(ENT *ep,int fi,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (ep) {
	    memclear(ep) ;
	    ep->fi = fi ;
	    if (cchar *cp ; (rs = uc_mallocstrw(sp,sl,&cp)) >= 0) {
	        ep->sysnamelen = sl ;
	        ep->sysname = cp ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (entry_start) */

static int entry_dialer(ENT *ep,cchar *dp,int dl) noex {
	int		rs ;
	if (cchar *cp ; (rs = uc_mallocstrw(dp,dl,&cp)) >= 0) {
	    ep->dialernamelen = dl ;
	    ep->dialername = cp ;
	}
	return rs ;
}
/* end subroutine (entry_dialer) */

static int entry_args(ENT *ep,cchar *args,int argslen) noex {
	int		rs = SR_OK ;
	if (argslen > 0) {
	    ep->dialerargslen = argslen ;
	    if (cchar *cp ; (rs = uc_mallocstrw(args,argslen,&cp)) >= 0) {
		ep->dialerargs = cp ;
	    }
	} /* end if */

	return (rs >= 0) ? argslen : rs ;
}
/* end subroutine (entry_args) */

static int entry_finish(ENT *ep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (ep) {
	    rs = SR_OK ;
	    if (ep->sysnamelen > 0) {
	        if (ep->dialerargs) {
	            rs1 = uc_free(ep->dialerargs) ;
	            if (rs >= 0) rs = rs1 ;
		    ep->dialerargs = nullptr ;
	        }
	        if (ep->dialername) {
	            rs1 = uc_free(ep->dialername) ;
	            if (rs >= 0) rs = rs1 ;
	            ep->dialername = nullptr ;
	        }
	        if (ep->sysname) {
	            rs1 = uc_free(ep->sysname) ;
	            if (rs >= 0) rs = rs1 ;
	            ep->sysname = nullptr ;
	        }
	        ep->sysnamelen = 0 ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (entry_finish) */

static int bdumpline(bfile *fp,char *lbuf,int llen) noex {
	int		rs ;
	while ((rs = breadlns(fp,lbuf,llen,nullptr)) > 0) {
	    if (lbuf[rs - 1] == '\n') break ;
	}
	return rs ;
}
/* end subroutine (bdumpline) */


