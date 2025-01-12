/* hostsfile SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* perform access table file related functions */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-01, David A�D� Morano
	This code was originally written (and largely forgotten).

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	hostsfile

	Description:
	This object processes INET 'hosts' files for use by daemon
	multiplexing server programs that want to control access
	to their sub-servers.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<bfile.h>
#include	<field.h>
#include	<fieldterminit.hh>
#include	<vecitem.h>
#include	<vecstr.h>
#include	<cthex.h>
#include	<char.h>
#include	<mallocstuff.h>
#include	<strtab.h>
#include	<rectab.h>
#include	<hash.h>
#include	<localmisc.h>

#include	"hostsfile.h"


/* local defines */

#define	HOSTSFILE_MINCHECKTIME	5	/* file check interval (seconds) */
#define	HOSTSFILE_CHECKTIME	360	/* file check interval (seconds) */
#define	HOSTSFILE_CHANGETIME	2	/* wait change interval (seconds) */

#define	HF		hostsfile
#define	HF_FI		hostsfile_fi
#define	HF_ENT		hostsfile_ent
#define	HF_ERR		hostsfile_err
#define	VS		vecstr

#define	LINELEN		200
#define	RECTAB		vecitem


/* imported namespaces */


/* local typedefs */

typedef hostsfile_ent	ent ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

int		hostsfile_fileadd(HF *,char *,vecitem *) noex ;

static int	hostsfile_finishfiles() noex ;
static int	hostsfile_finents() noex ;
static int	hostsfile_parsefile(HF *,int,vecitem *) noex ;
static int	hostsfile_addentry() noex ;
static int	hostsfile_finishfes() noex ;
static int	hostsfile_checkfiles(HF *,time_t,vecitem *) noex ;

static int	entry_start(HF_ENT *) noex ;
static int	entry_mat2(HF_ENT *,HF_ENT *) noex ;
static int	entry_mat3(HF_ENT *,HF_ENT *) noex ;
static int	entry_finish(HF_ENT *) noex ;

static int	cmpstdfunc() noex ;

#ifdef	COMMENT
static int	cmpfunc2(), cmpfunc3() ;
#endif

static void	errline(vecitem *,char *,int) noex ;
static void	freeit(cchar **) noex ;


/* local variables */

constexpr char		arg_terms[fieldterminit_termsize] = {
	0x00, 0x1B, 0x00, 0x00,
	0x09, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
} ;


/* exported variables */


/* exported subroutines */

int hostsfile_start(HF *atp,char *cdir,char *atfname,vecitem *eep) noex {
	USTAT		sb ;
	time_t		daytime = time(nullptr) ;
	int		rs = SR_OK ;
	int		f_defcache = false ;

	if (atp == nullptr)
	    return SR_FAULT ;

	atp->magic = 0 ;

/* initialize */

	if ((rs = vecitem_start(&atp->files,10,vecitem_PNOHOLES)) < 0)
	    goto bad1 ;

	if ((rs = vecitem_start(&atp->aes_std,10,vecitem_PSORTED)) < 0)
	    goto bad2 ;

	if ((rs = vecitem_start(&atp->aes_rgx,10,vecitem_PNOHOLES)) < 0)
	    goto bad3 ;

/* store the cache directory */

	if (cdir == nullptr) {
		f_defcache = true ;
		cdir = HOSTSFILE_CACHEDIR ;
	}

/* try to make the last component of the cache directory */

	if ((rs = u_stat(cdir,&sb)) < 0) {

	    rs = u_mkdir(cdir,0777) ;

	    if ((rs < 0) && (! f_defcache)) {
		f_defcache = true ;
		cdir = HOSTSFILE_CACHEDIR ;
	        if ((rs = u_stat(cdir,&sb)) < 0)
		    rs = u_mkdir(cdir,0777) ;
	    }

	} /* end if (tried to make cache directory) */

	u_chmod(cdir,0777) ;

	if (rs < 0)
		goto bad4 ;

	atp->cdir = mallocstr(cdir) ;

	if (atp->cdir == nullptr) {
		rs = SR_NOMEM ;
		goto bad5 ;
	}

	atp->checktime = time(nullptr) ;

	if (atfname != nullptr) {

	    rs = hostsfile_fileadd(atp,atfname,eep) ;

		if (rs < 0)
			goto badparse ;

	}

	atp->magic = HOSTSFILE_MAGIC ;
	return rs ;

/* handle bad things */
badparse:
	uc_free(atp->cdir) ;

bad5:

bad4:
	vecitem_finish(&atp->aes_rgx) ;

bad3:
	vecitem_finish(&atp->aes_std) ;

bad2:
	vecitem_finish(&atp->files) ;

bad1:
	return rs ;
}
/* end subroutine (hostsfile_start) */


/* add a file to the list of files */
int hostsfile_fileadd(HF *atp,char *atfname,vecitem *eep) noex {
	HF_FI	fe ;
	cint	clen = CACHENAMELEN ;
	int		rs ;
	int		fi ;
	cchar	*cp ;
	char		hostsfname[MAXPATHLEN + 1], *hnp ;
	char		cachefname[MAXPATHLEN + 1] ;
	char		cachename[CACHENAMELEN + 1] ;

	if (atp == nullptr) return SR_FAULT ;
	if (atfname == nullptr) return SR_FAULT ;

	hnp = atfname ;
	if (atfname[0] != '/') {

		rs = getpwd(hostsfname,MAXPATHLEN) ;

		if (rs < 0)
			return rs ;

		if ((rs + 1) < MAXPATHLEN) {
			hostfname[rs++] = '/' ;
			hostfname[rs] = '\0' ;
		}

		if ((rs + 1) < MAXPATHLEN)
		strwcpy((hostsfname + rs),atfname,(MAXPATHLEN - rs)) ;

		hnp = hostsfname ;

	} /* end if (constructing full path) */

/* store the filename away for later use (polling) */

	if ((fe.hostsfname = mallocstr(hnp)) == nullptr) {
	    rs = SR_NOMEM ;
	    goto badmal0 ;
	}

#ifdef	MALLOCLOG
	malloclog_alloc(fe.hostsfname,-1,"hostsfile_fileadd:filename") ;
#endif

/* calculate the cache name */

	rs = hash_elf(hnp,-1) ;

	strcpy(cachename,"hc") ;

	cthexi((cachename+2),clen,CACHENAMELEN) ;

	cachename[10] = '\0' ;

/* calculate the cache filepath name */

	cp = strwcpy(cachefname,atp->cachedir,MAXPATHLEN) ;

	if ((cp - cachefname + 1) < MAXPATHLEN) {
		*cp++ = '/' ;
		*cp = '\0' ;
	}

	if ((cp - cachefname + 1) < MAXPATHLEN)
		strwcpy(cp,cachename,(MAXPATHLEN - (cp - cachefname))) ;

	if ((fe.cachefname = mallocstr(cachefname)) == nullptr) {
	    rs = SR_NOMEM ;
	    goto badmal1 ;
	}

#ifdef	MALLOCLOG
	malloclog_alloc(fe.cachefname,-1,"hostsfile_fileadd:filename") ;
#endif

/* store the entry */

	fe.mtime = 0 ;
	rs = vecitem_add(&atp->files,&fe,szof(HF_FI)) ;

	if (rs < 0)
	    goto badaddfile ;

	fi = rs ;

/* do we need to create the file? */

	rs = hostsfile_mapit(atp,fi,eep) ;

/* parse the file for the first time */

	rs = hostsfile_parsefile(atp,fi,eep) ;

	return rs ;

/* handle bad things */
badparse:

badaddfile:
	uc_free(fe.cachefname) ;

#ifdef	MALLOCLOG
	malloclog_free(fe.cachefname,"hostsfile_fileadd:cachefname") ;
#endif

badmal1:
	uc_free(fe.hostsfname) ;

#ifdef	MALLOCLOG
	malloclog_free(fe.cachefname,"hostsfile_fileadd:hostsfname") ;
#endif

badmal0:
	return rs ;
}
/* end subroutine (hostsfile_fileadd) */


/* free up the resources occupied by an HF list object */
int hostsfile_finish(HF *atp) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (atp == nullptr) return SR_FAULT ;

	if (atp->magic != HOSTSFILE_MAGIC) return SR_NOTOPEN ;
	{
	rs1 = hostsfile_finents(atp) ;
	if (rs >= 0) rs = rs1 ;
	}
	{
	rs1 = hostsfile_finishfiles(atp) ;
	if (rs >= 0) rs = rs1 ;
	}
/* free up the rest of the main object data */
	{
	rs1 = vecitem_finish(&atp->aes_std) ;
	if (rs >= 0) rs = rs1 ;
	}
	{
	rs1 = vecitem_finish(&atp->aes_rgx) ;
	if (rs >= 0) rs = rs1 ;
	}
	{
	rs1 = vecitem_finish(&atp->files) ;
	if (rs >= 0) rs = rs1 ;
	}
	atp->magic = 0 ;
	return rs ;
}
/* end subroutine (hostsfile_finish) */


/* search the netgroup table for a netgroup match */
int hostsfile_allowed(HF *atp,char *netgroup,char *machine,
		char *username,char *password) noex {
	HF_ENT	ae, *aep ;
	vecitem		*slp ;
	vecitem_CUR	cur ;
	time_t		daytime = time(nullptr) ;
	int		rs ;
	int		i ;

	if (atp == nullptr) return SR_FAULT ;

	if (atp->magic != HOSTSFILE_MAGIC) return SR_NOTOPEN ;

	if (machine == nullptr) return SR_INVALID ;

/* should we even check? */

	if ((daytime - atp->checktime) > HOSTSFILE_MINCHECKTIME) {

		rs = hostsfile_checkfiles(atp,daytime,nullptr) ;
		if (rs < 0)
			goto ret0 ;

	} /* end if (we needed to check) */

/* load up a fake entry for comparison purposes */

	entry_start(&ae) ;

	ae.netgroup.std = netgroup ;
	if ((netgroup == nullptr) || (netgroup[0] == '\0'))
	    ae.netgroup.std = HOSTSFILE_DEFNETGROUP ;

	ae.machine.std = machine ;
	ae.username.std = username ;
	ae.password.std = password ;

/* search the STD entries first */

	slp = &atp->aes_std ;

	vecitem_curbegin(slp,&cur) ;

	while ((rs = vecitem_fetch(slp,&ae,&cur,cmpstdfunc,&aep)) >= 0) {
		if (aep == nullptr) continue ;

	    if (entry_mat2(aep,&ae)) 
		break ;


	} /* end while */

	vecitem_curend(slp,&cur) ;

/* search the RGX entries (if necessary) */

	if (rs < 0) {
	    slp = &atp->aes_rgx ;
	    for (int i = 0 ; vecitem_get(slp,i,&aep) >= 0 ; i += 1) {
	        if (aep == nullptr) continue ;

	        if (aep == nullptr) continue ;

	        if (entry_mat3(aep,&ae)) 
			break ;

	    } /* end for (looping through entries) */
	} /* end if (comparing RGX entries) */

ret0:
	return rs ;
}
/* end subroutine (hostsfile_allowed) */

/* search the netgroup table for any netgroup match */
int hostsfile_anyallowed(HF *atp,vecstr *ngp,vecstr *mnp,
		char *username,char *password) noex {
	int		rs ;
	char		*netgroup ;
	char		*machine ;

	if (atp == nullptr) return SR_FAULT ;

	if (atp->magic != HOSTSFILE_MAGIC) return SR_NOTOPEN ;

	for (int i = 0 ; (rs = vecstr_get(ngp,i,&netgroup)) >= 0 ; i += 1) {

	    for (int j = 0 ; (rs = vecstr_get(mnp,j,&machine)) >= 0 ; j += 1) {

	        rs = hostsfile_allowed(atp,netgroup,machine,username,password) ;
	        if (rs >= 0) 
			goto done ;

	    } /* end for */

	} /* end for (looping over netgroups) */

done:
	return rs ;
}
/* end subroutine (hostsfile_anyallowed) */

#ifdef	COMMENT

/* search the netgroup table for a netgroup match */
int hostsfile_find(HF *atp,char *netgroup,HF_ENT **sepp) noex {
	vecitem		*slp ;
	int		i ;
	cchar	*sp ;

	if (atp == nullptr) return SR_FAULT ;

	if (atp->magic != HOSTSFILE_MAGIC) return SR_NOTOPEN ;

	slp = &atp->e ;
	for (i = 0 ; vecitem_get(slp,i,sepp) >= 0 ; i += 1) {
	    if (*sepp == nullptr) continue ;

	    sp = (*sepp)->netgroup ;

	    if (strcmp(netgroup,sp) == 0)
	        return i ;

	} /* end for (looping through entries) */

	return -1 ;
}
/* end subroutine (hostsfile_find) */

#endif /* COMMENT */

int hostsfile_curbegin(HF *atp,HF_CUR *cp) noex {
	if (atp == nullptr) return SR_FAULT ;

	if (atp->magic != HOSTSFILE_MAGIC) return SR_NOTOPEN ;

	cp->i = cp->j = -1 ;
	return SR_OK ;
}
/* end subroutine (hostsfile_curbegin) */

int hostsfile_curend(HF *atp,HF_CUR *cp) noex {
	if (atp == nullptr) return SR_FAULT ;

	if (atp->magic != HOSTSFILE_MAGIC) return SR_NOTOPEN ;

	cp->i = cp->j = -1 ;
	return SR_OK ;
}
/* end subroutine (hostsfile_curend) */

int hostsfile_enum(HF *atp,HF_CUR *cp,HF_ENT **sepp) noex {
	vecitem		*slp ;
	HF_ENT		*aep ;
	int		rs ;
	int		i, j ;

	if (atp == nullptr) return SR_FAULT ;
	if (cp == nullptr) return SR_FAULT ;

	if (atp->magic != HOSTSFILE_MAGIC) return SR_NOTOPEN ;

	if (sepp == nullptr)
		sepp = &aep ;

	rs = SR_NOTFOUND ;
	if (cp->i <= 0) {

	    slp = &atp->aes_std ;
	    cp->j = (cp->j < 0) ? 0 : cp->j + 1 ;
	    for (j = cp->j ; (rs = vecitem_get(slp,j,sepp)) >= 0 ; j += 1) {
	        if (*sepp != nullptr) break ;
	    }

	    if (rs < 0) {
	        cp->j = -1 ;
	        cp->i = 1 ;
	    } else {
	        cp->i = 0 ;
	        cp->j = j ;
	    }

	} /* end if */

	if (cp->i == 1) {

	    slp = &atp->aes_rgx ;
	    cp->j = (cp->j < 0) ? 0 : cp->j + 1 ;
	    for (j = cp->j ; (rs = vecitem_get(slp,j,sepp)) >= 0 ; j += 1) {
	        if (*sepp != nullptr) break ;
	    }

	    if (rs < 0) {
	        cp->j = -1 ;
	        cp->i += 1 ;
	    } else {
	        cp->j = j ;
		}

	} /* end if (RGX entries) */

	return rs ;
}
/* end subroutine (hostsfile_enum) */

/* check if the access tables files have changed */
int hostsfile_check(HF *atp,vecitem *eep) noex {
	USTAT		sb ;
	HF_FI	*fep ;
	time_t		daytime = time(nullptr) ;
	int		rs, i ;
	int		c_changed = 0 ;

	if (atp == nullptr) return SR_FAULT ;

	if (atp->magic != HOSTSFILE_MAGIC) return SR_NOTOPEN ;

/* should we even check? */

	if ((daytime - atp->checktime) <= HOSTSFILE_CHECKTIME)
	    return SR_OK ;

	rs = hostsfile_checkfiles(atp,daytime,eep) ;

	return rs ;
}
/* end subroutine (hostsfile_check) */


/* private subroutines */

static int hostsfile_checkfiles(HF *atp,time_t daytime,vecitem *eep) noex {
	USTAT		sb ;
	HF_FI	*fep ;
	int		rs = SR_OK ;
	int		i ;
	int		c_changed = 0 ;

	for (i = 0 ; vecitem_get(&atp->files,i,&fep) >= 0 ; i += 1) {
	    if (fep == nullptr) continue ;

	    if ((u_stat(fep->filename,&sb) >= 0) &&
	        (sb.st_mtime > fep->mtime) &&
	        ((daytime - sb.st_mtime) >= HOSTSFILE_CHANGETIME)) {

	        hostsfile_finishfes(atp,i) ;

	        if ((rs = hostsfile_parsefile(atp,i,eep)) >= 0) {
	            c_changed += 1 ;
		}

	    } /* end if */

	} /* end for */

	if ((rs >= 0) && c_changed) {
	        rs = vecitem_sort(&atp->aes_std,cmpstdfunc) ;
	}

	atp->checktime = daytime ;
	return (rs >= 0) ? c_changed : rs ;
}
/* end subroutine (hostsfile_check) */

static int hostsfile_parsefile(HF *atp,int fi,vecitem *eep) noex {
	USTAT		sb ;
	HF_FI	*fep ;
	HF_ENT	se ;
	STRTAB		nst ;		/* name string table */
	FIELD		fsb ;
	bfile		file, *fp = &file ;
	int		rs = SR_OK ;
	int		i ;
	int		len, line ;
	int		fl ;
	int		c_added = 0 ;
	cchar	*fp ;
	cchar	*filename ;
	cchar	*cp ;
	char		lbuf[LINELEN + 1] ;

	rs = vecitem_get(&atp->files,fi,&fep) ;
	if (rs < 0)
	    goto ret0 ;

	if (fep == nullptr) {
	    rs = SR_NOTFOUND ;
	    goto ret0 ;
	}

	rs = bopen(fp,fep->filename,"r",0664) ;
	if (rs < 0)
	    goto ret0 ;

	rs = bcontrol(fp,BC_STAT,&sb) ;
	if (rs < 0)
	    goto done ;

/* have we already parsed this one? */

	if (fep->mtime >= sb.st_mtime)
	    goto done ;

	fep->mtime = sb.st_mtime ;

/* loop through the lines of the file */

	rs = strtab_start(&nst,(int) (sb.st_size / 2)) ;
	if (rs < 0)
		goto bad2 ;

	rs = rectab_init(&rt,(int) (sb.st_size / 20)) ;
	if (rs < 0)
		goto bad3 ;

	c_added = 0 ;
	line = 0 ;
	while ((len = breadln(fp,lbuf,LINELEN)) > 0) {
		INETADDR	ia ;
		int		cnamelen, csi, si ;
		char		*cname ;


	    line += 1 ;
	    if (len == 1) 
		continue ;	/* blank line */

	    if (lbuf[len - 1] != '\n') {

	        while ((c = bgetc(fp)) >= 0)
	            if (c == '\n') break ;

	        continue ;
	    }

	    lbuf[--len] = '\0' ;
	    cp = lbuf ;
	    while (CHAR_ISWHITE(*cp)) {
	        cp += 1 ;
	        len -= 1 ;
	    }

	    if ((*cp == '\0') || (*cp == '#')) continue ;

		if ((rs = field_start(&fsb,cp,len)) >= 0) {

	    if ((fl = field_get(&fsb,arg_terms,&fp)) > 0) {

		rs = inetaddr_startstr(&ia,fp,fl) ;

		if (rs < 0) {
			errline(eep,fep->filename,line) ;
			continue ;
		}

/* pick off names until the end */

		i = 0 ;
		while ((fl = field_get(&fsb,arg_terms,&fp)) > 0) {

/* add this entry */

		si = strtab_add(&nst,fp,fl) ;

			if (i == 0) {

				cname = fp ;
				cnamelen = fl ;
				csi = si ;

			} /* end if (canonical name) */

/* add this record to the record table */

		rs = rectab_add(&rt,ia,csi,si) ;

		if (rs < 0)
			break ;

/* continue */

			i += 1 ;
			if (fsb.term == '#')
				break ;

		} /* end while (extracting names) */

		if (i < 1)
			errline(eep,fep->filename,line) ;

		inetaddr_finish(&ia) ;

	        if (rs >= 0)
	            c_added += i ;

		} /* end if */

		field_finish(&fsb) ;
	    } /* end if */

		if (rs < 0)
			break ;

	} /* end while (reading lines) */

/* OK, if no errors, make the indexes and write them out to the file */

	if (rs >= 0) {



	} /* end if (writing out cache file) */

bad4:
	rectab_free(&rt) ;

bad3:
	strtab_finish(&nst) ;

/* done with configuration file processing */
bad2:
done:
ret1:
	bclose(fp) ;
ret0:
	return (rs >= 0) ? c_added : rs ;
}
/* end subroutine (hostsfile_parsefile) */

static int hostsfile_addentry(HF *atp,HF_ENT *sep) noex {
	int		rs = SR_OK ;
	if (parttype(sep->netgroup.std) == 0) {
	    rs = vecitem_add(&atp->aes_std,sep,szof(HF_ENT)) ;
	} else {
	    rs = vecitem_add(&atp->aes_rgx,sep,szof(HF_ENT)) ;
	}
	return rs ;
}
/* end subroutine (hostsfile_addentry) */

static int hostsfile_finents(HF *atp) noex {
	vecitem		*slp ;
	int		rs = SR_OK ;
	int		rs1 ;
	for (int j = 0 ; j < 2 ; j += 1) {
	    HF_ENT	*sep ;
	    slp = (j == 0) ? &atp->aes_std : &atp->aes_rgx ;
	    for (int i = 0 ; vecitem_get(slp,i,&sep) >= 0 ; i += 1) {
	        if (sep) {
	            rs1 = entry_finish(sep) ;
		    if (rs >= 0) rs = rs1 ;
		}
	    } /* end for */
	} /* end for */
	return rs ;
}
/* end subroutine (hostsfile_finents) */

/* free up all of the entries in this HF list associated w/ a file */
static int hostsfile_finishfes(HF *atp,int fi) noex {
	HF_ENT		*sep ;
	vecitem		*slp ;
	int		rs = SR_OK ;
	int		rs1 ;
	for (int j = 0 ; j < 2 ; j += 1) {
	    slp = (j == 0) ? &atp->aes_std : &atp->aes_rgx ;
	    for (int i = 0 ; (rs = vecitem_get(slp,i,&sep)) >= 0 ; i += 1) {
	        if (sep) {
	            if ((sep->fi == fi) || (fi < 0)) {
		        {
	                    rs1 = entry_finish(sep) ;
		            if (rs >= 0) rs = rs1 ;
		        }
		        {
	                    rs1 = vecitem_del(slp,i) ;
		            if (rs >= 0) rs = rs1 ;
		        }
	                i -= 1 ;
	            } /* end if */
		}
	    } /* end for */
	} /* end for */
	return rs ;
}
/* end subroutine (hostsfile_finishfes) */

/* free up all of the files in this HF list */
static int hostsfile_finishfiles(HF *atp) noex {
	HF_FI		*afp ;
	vecitem		*slp = &atp->files ;
	cint		rsn = SR_NOTFOUND ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		rs2 ;
	for (int i = 0 ; (rs2 = vecitem_get(slp,i,&afp)) >= 0 ; i += 1) {
	    if (afp) {
	        if (afp->filename != nullptr) {
	            rs1 = uc_free(afp->filename) ;
		    if (rs >= 0) rs = rs1 ;
	        }
	        vecitem_del(slp,i--) ;
	    }
	} /* end for */
	if ((rs >= 0) && (rs2 != rsn)) rs = rs2 ;

	return rs ;
}
/* end subroutine (hostsfile_finishfiles) */

static int entry_start(HF_ENT *sep) noex {
    	int		rs = SR_FAULT ;
	if (sep) {
	    rs = SR_OK ;
	    sep->fi = -1 ;
	    part_start(&sep->netgroup) ;
	    part_start(&sep->machine) ;
	    part_start(&sep->username) ;
	    part_start(&sep->password) ;
	}
	return rs ;
}
/* end subroutine (entry_start) */

static int entry_finish(HF_ENT *sep) noex {
    	int		rs = SR_FAULT ;
	if (sep) {
	    rs = SR_OK ;
	    part_finish(&sep->netgroup) ;
	    part_finish(&sep->machine) ;
	    part_finish(&sep->username) ;
	    part_finish(&sep->password) ;
	}
	return rs ;
}
/* end subroutine (entry_finish) */

static int entry_mat2(HF_ENT *e1p,HF_ENT *e2p) noex {

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

static int entry_mat3(HF_ENT *e1p,HF_ENT *e2p) noex {
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

static void freeit(cchar *pp) noex {
	if (*pp) {
	    uc_free(*pp) ;
	    *pp = nullptr ;
	}
}
/* end subroutine (freeit) */

/* compare just the 'netgroup' part of entries (used for sorting) */
static int cmpstdfunc(HF_ENT **e1pp,HF_ENT **e2pp) noex {
	if ((*e1pp == nullptr) && (*e2pp == nullptr))
	    return 0 ;

	if (*e1pp == nullptr)
	    return 1 ;

	if (*e2pp == nullptr)
	    return -1 ;

	return strcmp((*e1pp)->netgroup.std,(*e2pp)->netgroup.std) ;
}
/* end subroutine (cmpstdfunc) */

/* record table stuff */
static void errline(vecitem *eep,cc *filename,inr line) noex {
	HF_ERR	el ;
	if (eep == nullptr) return ;
	el.filename = filename ;
	el.line = line ;
	vecitem_add(eep,&el,szof(HF_ERR)) ;
}
/* end subroutine (errline) */


