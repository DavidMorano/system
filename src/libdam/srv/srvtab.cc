/* srvtab SUPPORT */
/* lang=C++20 */

/* perform service table file related functions */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-07-01, David A­D­ Morano
	This subroutine was adopted for use in the 'rexecd' daemon
	program.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

	Name:
	srvtab

	Description:
	This object reads in a service table file and stores the
	information parsed from that file.

******************************************************************************/

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
#include	<vecitem.h>
#include	<ascii.h>
#include	<mallocstuff.h>
#include	<getpwd.h>
#include	<mkpathx.h>
#include	<sfx.h>
#include	<strwcpy.h>
#include	<matstr.h>
#include	<char.h>
#include	<localmisc.h>

#include	"srvtab.h"


/* local defines */

#define	SRVTAB_RGXLEN	256		/* regexp buffer length */

#ifndef	LINEBUFLEN
#define	LINEBUFLEN	2048
#endif

#ifndef	BUFLEN
#define	BUFLEN		(2 * MAXPATHLEN)
#endif

#define	MAXOPENTIME	300		/* maximum FD cache time */
#define	TI_FILECHECK	9		/* file check interval (seconds) */
#define	TI_FILECHANGE	3		/* wait change interval (seconds) */

#define	SRVTAB_REGEX	CF_REGEX


/* external subroutines */


/* external variables */


/* forward references */

static int	srvtab_fileparse(SRVTAB *,time_t,vecitem *) noex ;
static int	srvtab_filedump(SRVTAB *) noex ;

static int	entry_start(SRVTAB_ENT *) noex ;
static int	entry_groupsload(SRVTAB_ENT *,cchar *,int) noex ;
static int	entry_groupadd(SRVTAB_ENT *,cchar *) noex ;
static int	entry_enough(SRVTAB_ENT *) noex ;
static int	entry_finish(SRVTAB_ENT *) noex ;

static int	stradd(cchar **,cchar *,int) noex ;

static void	freeit(cchar **) noex ;


/* local variables */

/* key field terminators ('#', ',', ':', '=') */
constexpr cchar		key_terms[32] = {
	0x00, 0x00, 0x00, 0x00,
	0x08, 0x10, 0x00, 0x24,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
} ;

/* argument field terminators (pound '#' and comma ',') */
constexpr cchar		saterms[32] = {
	0x00, 0x00, 0x00, 0x00,
	0x08, 0x10, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ;

enum srvkeys {
	srvkey_program,
	srvkey_arguments,
	srvkey_args,
	srvkey_username,
	srvkey_groupname,
	srvkey_options,
	srvkey_opts,
	srvkey_groups,
	srvkey_access,
	srvkey_interval,
	srvkey_addr,
	srvkey_passfile,
	srvkey_project,
	srvkey_overlast
} ;

constexpr cpcchar	srvkeys[] = {
	"program",
	"arguments",
	"args",
	"username",
	"groupname",
	"options",
	"opts",
	"groups",
	"access",
	"interval",
	"addr",
	"passfile",
	"project",
	NULL
} ;


/* exported variables */


/* exported subroutines */

int srvtab_open(srvtab *op,cchar *fname,vecitem *eep) noex {
	time_t		daytime = time(NULL) ;
	int		rs = SR_OK ;
	int		fnl = -1 ;
	cchar		*fnp ;
	cchar		*cp ;
	char		tmpfname[MAXPATHLEN + 1] ;

	if (op == NULL)
	    return SR_FAULT ;

	op->magic = 0 ;
	op->fd = -1 ;

/* initialize */

	op->opentime = daytime ;
	op->checktime = daytime ;

	fnp = fname ;
	if (fname[0] != '/') {
	    char	pwdbuf[MAXPATHLEN + 1] ;

	    fnp = tmpfname ;
	    rs = getpwd(pwdbuf,MAXPATHLEN) ;
	    if (rs >= 0) {
	        rs = mkpath2(tmpfname,pwdbuf,fname) ;
		fnl = rs ;
	    }

	} /* end if (rooting file) */

	if (rs < 0)
	    goto bad0 ;

	rs = uc_mallocstrw(fnp,fnl,&cp) ;
	if (rs < 0) goto bad1 ;
	op->fname = cp ;

	rs = vecitem_start(&op->e,10,VECITEM_OSWAP) ;
	if (rs < 0)
	    goto bad2 ;

	rs = srvtab_fileparse(op,daytime,eep) ;
	if (rs < 0)
	    goto bad3 ;

	op->magic = SRVTAB_MAGIC ;

ret0:
	return rs ;

/* handle bad things */
bad3:
bad2:
	vecitem_finish(&op->e) ;

bad1:
	uc_free(op->fname) ;
	op->fname = NULL ;

bad0:
	goto ret0 ;
}
/* end subroutine (srvtab_open) */

int srvtab_close(srvtab *op) noex {
	if (op == NULL) return SR_FAULT ;

	if (op->magic != SRVTAB_MAGIC)
	    return SR_NOTOPEN ;

	if (op->fd >= 0) {
	    u_close(op->fd) ;
	    op->fd = -1 ;
	}

	srvtab_filedump(op) ;

	vecitem_finish(&op->e) ;

	if (op->fname != NULL) {
	    uc_free(op->fname) ;
	    op->fname = NULL ;
	}

	op->magic = 0 ;
	return SR_OK ;
}
/* end subroutine (srvtab_close) */

int srvtab_match(srvtab *op,cchar *service,srvtab_ent **sepp) noex {
	vecitem	*slp ;
	int	rs = SR_NOTFOUND ;
	int	sl, l1, l2 ;
	int	i ;
	cchar	*sp, *cp ;


	if (op == NULL)
	    return SR_FAULT ;

	if (op->magic != SRVTAB_MAGIC)
	    return SR_NOTOPEN ;

	if (service == NULL)
	    return SR_FAULT ;

	slp = &op->e ;
	for (i = 0 ; vecitem_get(slp,i,sepp) >= 0 ; i += 1) {
	    if (*sepp == NULL) continue ;

	    sp = (*sepp)->service ;

	    if (((cp = strchr(sp,'*')) != NULL) &&
	        (strchr(sp,'\\') == NULL)) {

	        if (strncmp(service,sp,cp - sp) == 0) {

	            cp += 1 ;
	            l1 = strlen(service) ;

	            l2 = strlen(sp) ;

	            sl = sp + l2 - cp ;

	            if (strncmp(service + l1 - sl,cp,sl) == 0)
	                rs = SR_OK ;

	        } /* end if */

	    } else if (strcmp(service,sp) == 0) {
	        rs = SR_OK ;
	    }
	    if (rs >= 0) break ;
	} /* end for (looping through entries) */

	return (rs >= 0) ? i : rs ;
}
/* end subroutine (srvtab_match) */

int srvtab_find(srvtab *op,cchar *service,srvtab_ent **sepp) noex {
	vecitem		*slp ;
	SRVTAB_ENT	*ep ;
	int	i ;
	cchar	*sp ;


	if (op == NULL)
	    return SR_FAULT ;

	if (op->magic != SRVTAB_MAGIC)
	    return SR_NOTOPEN ;

	if (service == NULL)
	    return SR_FAULT ;

	if (sepp == NULL)
	    sepp = &ep ;

	slp = &op->e ;
	for (i = 0 ; vecitem_get(slp,i,sepp) >= 0 ; i += 1) {
	    if (*sepp == NULL) continue ;

	    sp = (*sepp)->service ;

	    if (strcmp(service,sp) == 0)
	        return i ;

	} /* end for (looping through entries) */

	return -1 ;
}
/* end subroutine (srvtab_find) */


/* enumerate the service entries */
int srvtab_get(srvtab *op,int i,srvtab_ent **sepp) noex {
	vecitem		*slp ;
	int		rs ;

	if (op == NULL)
	    return SR_FAULT ;

	if (op->magic != SRVTAB_MAGIC)
	    return SR_NOTOPEN ;

	slp = &op->e ;
	rs = vecitem_get(slp,i,sepp) ;

	return rs ;
}
/* end subroutine (srvtab_get) */

int srvtab_check(srvtab *op,time_t daytime,vecitem *eep) noex {
	USTAT		sb ;
	int		rs = SR_OK ;

	if (op == NULL)
	    return SR_FAULT ;

	if (op->magic != SRVTAB_MAGIC)
	    return SR_NOTOPEN ;

	if (daytime <= 0)
	    daytime = time(NULL) ;

/* should we even check? */

	if ((daytime - op->checktime) <= TI_FILECHECK)
	    goto ret0 ;

	op->checktime = daytime ;

/* is the file open already? */

	if (op->fd < 0) {

	    rs = uc_open(op->fname,O_RDONLY,0666) ;
	    op->fd = rs ;
	    if (rs >= 0) {
	        op->opentime = daytime ;
	        uc_closeonexec(op->fd,TRUE) ;
	    }

	} /* end if (opening file to cache the FD) */
	if (rs < 0) goto ret0 ;

/* check the modification time on the file */

	rs = u_fstat(op->fd,&sb) ;
	if (rs < 0) goto ret0 ;

	if ((sb.st_mtime > op->mtime) &&
	    ((daytime - sb.st_mtime) >= TI_FILECHANGE)) {

	    rs = srvtab_filedump(op) ;

	    if (rs >= 0) {
	        op->mtime = sb.st_mtime ;
	        rs = srvtab_fileparse(op,daytime,eep) ;
	    }

	} else if ((daytime - op->opentime) > MAXOPENTIME) {
	    u_close(op->fd) ;
	    op->fd = -1 ;
	}

ret0:
	return rs ;
}
/* end subroutine (srvtab_check) */


/* private subroutines */

static int srvtab_fileparse(srvtab *op,time_t daytime,vecitem *eep) noex {
	USTAT		sb ;
	vecitem		*slp ;
	SRVTAB_ENT	se ;
	field		fsb ;
	bfile	sfile, *sfp = &sfile ;

	int	rs = SR_OK ;
	int	n, c, len ;
	int	line = 0 ;
	int	cl ;
	int	f_closed = FALSE ;
	int	f_ent = FALSE ;

	cchar	*cp ;

	char	linebuf[LINEBUFLEN + 1] ;
	char	linebuf2[LINEBUFLEN + 1] ;

	slp = &op->e ;
	if (op->fd >= 0) {

	    rs = bopen(sfp,((char *) op->fd),"dr",0664) ;
	    if (rs >= 0)
	        bseek(sfp,0L,SEEK_SET) ;

	} else {
	    rs = bopen(sfp,op->fname,"r",0664) ;
	}

	if (rs < 0)
	    goto badopen ;

/* what about caching the file descriptor? */

	if (op->fd >= 0) {
	    if (daytime <= 0) daytime = time(NULL) ;
	    if ((daytime - op->opentime) > MAXOPENTIME) {
	        u_close(op->fd) ;
	        op->fd = -1 ;
	        f_closed = TRUE ;
	    }
	}

/* store the file modification time */

	bcontrol(sfp,BC_STAT,&sb) ;
	op->mtime = sb.st_mtime ;

/* start processing the file */

	while ((rs = breadln(sfp,linebuf,LINEBUFLEN)) > 0) {

	    len = rs ;
	    line += 1 ;

	    if (linebuf[len - 1] != '\n') {
	        while ((c = bgetc(sfp)) >= 0) {
	            if (c == '\n') break ;
		}
	        continue ;
	    }

	    linebuf[--len] = '\0' ;
	    cp = linebuf ;
	    cl = len ;
	    while (CHAR_ISWHITE(*cp)) {
	        cp += 1 ;
	        cl -= 1 ;
	    }

	    if ((*cp == '\0') || (*cp == '#'))
	        continue ;

	    if ((rs = field_start(&fsb,cp,cl)) >= 0) {
		int		fl ;
		cchar	*fp ;

	        if ((fl = field_get(&fsb,key_terms,&fp)) > 0) {

	        if (fsb.term == ':') {

	            if (f_ent) {

			if (entry_enough(&se) > 0) {

	                rs = vecitem_add(slp, &se,sizeof(SRVTAB_ENT)) ;
	                if (rs < 0) {
			    f_ent = FALSE ;
	                    entry_finish(&se) ;
	                    break ;
			}

	            } else {
			f_ent = FALSE ;
	                entry_finish(&se) ;
		    }

		    } /* end if (had previous entry) */

	            rs = entry_start(&se) ;
		    f_ent = (rs >= 0) ;

		    if (rs >= 0) {

	            se.service = mallocstrw(fp,fl) ;

	            if (se.service == NULL)
	                rs = SR_NOMEM ;

#ifdef	MALLOCLOG
	            malloclog_alloc(se.service,-1,"srvtab_fileparse:service") ;
#endif

/* compile the expression also */

#if	SRVTAB_REGEX
	            se.matchlen = 0 ;
	            rs = uc_malloc(SRVTAB_RGXLEN,&se.matchcode) ;

	            if (rs > 0) {

#ifdef	MALLOCLOG
	                malloclog_alloc(se.matchcode,-1,
			"srvtab_fileparse:matchcode") ;
#endif

	                cp = compile(se.service,se.matchcode,
	                    (se.matchcode + SRVTAB_RGXLEN)) ;

	                if (cp != NULL) {
	                    se.matchcode = cp ;
	                    se.matchlen = SRVTAB_RGXLEN ;
	                } else {
	                    se.matchlen = 0 ;
	                    freeit(&se.matchcode) ;
	                } /* end if (compiling RE) */

	            } else {
	                se.matchcode = NULL ;
		    }
#endif /* SRVTAB_REGEX */

		    } /* end if (new entry) */

/* see if there is a service key on this same line */

		    if (rs >= 0)
	            fl = field_get(&fsb,key_terms,&fp) ;

	        } /* end if (a new service) */

/* loop while we have additional fields on this line */

	        while ((rs >= 0) && (fl >= 0)) {
		    int	ki = matstr(srvkeys,fp,fl) ;

	            if (fsb.term != ',') {

			fp = linebuf2 ;
	                fl = field_srvarg(&fsb,saterms,linebuf2,LINEBUFLEN) ;

	            } else
	                fp = NULL ;

	            switch (ki) {

	            case srvkey_program:
	                if (fp != NULL) {
			    cl = sfshrink(fp,fl,&cp) ;
			    if (cl > 0) {
	                        freeit(&se.program) ;
	                        se.program = mallocstrw(cp,cl) ;
			    }
	                }
	                break ;

	            case srvkey_arguments:
	            case srvkey_args:
	                if (fp != NULL) {
	                    freeit(&se.args) ;
	                    se.args = mallocstrw(fp,fl) ;
	                }
	                break ;

	            case srvkey_username:
	                if ((fp != NULL) && (fl > 0)) {
	                    freeit(&se.username) ;
	                    se.username = mallocstrw(fp,fl) ;
	                }
	                break ;

	            case srvkey_groupname:
	                if ((fp != NULL) && (fl > 0)) {
	                    freeit(&se.groupname) ;
	                    se.groupname = mallocstrw(fp,fl) ;
	                }
	                break ;

	            case srvkey_options:
	            case srvkey_opts:
	                if (fp != NULL) {
	                    freeit(&se.options) ;
	                    se.options = mallocstrw(fp,fl) ;
	                }
	                break ;

/* groups were specified */
	            case srvkey_groups:
	                if (se.ngroups < 0)
	                    se.ngroups = 0 ;

	                if ((fl >= 0) && (fp != NULL))
	                    entry_groupsload(&se,fp,fl) ;

	                while ((fsb.ll > 0) && (fsb.term != ',')) {

			    fp = linebuf2 ;
	                    fl = field_srvarg(&fsb,saterms,
				linebuf2,LINEBUFLEN) ;

	                    if (fl > 0)
	                        entry_groupsload(&se,fp,fl) ;

	                } /* end while */

	                break ;

	            case srvkey_access:
	                if (fp != NULL) {
	                    freeit(&se.access) ;
	                    se.access = mallocstrw(fp,fl) ;
	                }
	                break ;

	            case srvkey_interval:
	                if (fp != NULL) {
	                    freeit(&se.interval) ;
	                    se.interval = mallocstrw(fp,fl) ;
	                }
	                break ;

	            case srvkey_addr:
	                if (fp != NULL) {
	                    stradd(&se.addr,fp,fl) ;
	                } /* end if (getting address) */
	                break ;

	            case srvkey_passfile:
	                if (fp != NULL) {
	                    freeit(&se.pass) ;
	                    se.pass = mallocstrw(fp,fl) ;
	                }
	                break ;

	            case srvkey_project:
	                if (fp != NULL) {
	                    freeit(&se.project) ;
	                    se.project = mallocstrw(fp,fl) ;
	                }
	                break ;

	            } /* end switch */

		    if (rs < 0) break ;

	            if (fsb.term == '#')
	                break ;

/* is there another service key on this line */

	            fl = field_get(&fsb,key_terms,&fp) ;

	        } /* end while (looping on fields) */

	    } /* end if (non-empty field) */

	    field_finish(&fsb) ;
	    } /* end if (field) */

	    if (rs < 0)
	        break ;

	} /* end while (reading lines) */

	if ((rs >= 0) && f_ent) {
	    if (entry_enough(&se) > 0) {
	        rs = vecitem_add(slp, &se,sizeof(SRVTAB_ENT)) ;
	        if (rs >= 0) f_ent = FALSE ;
	    }
	    if (f_ent) {
		f_ent = FALSE ;
	        entry_finish(&se) ;
	    }

	} /* end if (entry) */

	if (rs < 0)
	    goto badadd ;

	rs = vecitem_count(slp) ;
	n = rs ;

	if ((rs >= 0) && (! f_closed) && (op->fd < 0)) {
	    int	fd ;

	    if (bcontrol(sfp,BC_FD,&fd) >= 0) {

	        if (daytime <= 0)
	            daytime = time(NULL) ;

	        op->opentime = daytime ;
	        op->fd = u_dup(fd) ;

	        uc_closeonexec(op->fd,TRUE) ;

	    } /* end if (caching FD and setting CLOEXEC) */

	} /* end if (caching FD) */

	bclose(sfp) ;

ret0:
	return (rs >= 0) ? n : rs ;

/* done with configuration file processing */
badadd:
	srvtab_filedump(op) ;

	if (f_ent) {
	    f_ent = FALSE ;
	    entry_finish(&se) ;
	}

	bclose(sfp) ;

badopen:
	if (op->fd >= 0) {
	    u_close(op->fd) ;
	    op->fd = -1 ;
	}

	goto ret0 ;
}
/* end subroutine (srvtab_fileparse) */

static int srvtab_filedump(SRVTAB *op) noex {
	SRVTAB_ENT	*ep ;
	int		rs = SR_OK ;
	int		rs1 ;
	for (int i = 0 ; vecitem_get(&op->e,i,&ep) >= 0 ; i += 1) {
	    if (ep == NULL) continue ;
	    {
	        rs1 = entry_finish(ep) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = vecitem_del(&op->e,i--) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end for */

	return rs ;
}
/* end subroutine (srvtab_filedump) */

static int entry_start(SRVTAB_ENT *sep) noex {
	int		rs = SR_FAULT ;
	if (sep) {
	    rs = memclear(sep) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (entry_start) */

static int entry_finish(SRVTAB_ENT *sep) noex {
	int	i ;

	if (sep->service == NULL)
	    return SR_OK ;

	freeit(&sep->service) ;

	freeit(&sep->matchcode) ;

	freeit(&sep->program) ;

	freeit(&sep->args) ;

	freeit(&sep->options) ;

	freeit(&sep->username) ;

	freeit(&sep->groupname) ;

	for (i = 0 ; sep->groupnames[i] != NULL ; i += 1) {
	    freeit(sep->groupnames + i) ;
	}
	freeit(&sep->access) ;

	freeit(&sep->interval) ;

	freeit(&sep->addr) ;

	freeit(&sep->pass) ;

	sep->matchlen = 0 ;
	return 0 ;
}
/* end subroutine (entry_finish) */

static int entry_enough(srvtab_ent *sep) noex {
	if ((sep->service == NULL) || (sep->service[0] == '\0'))
	    return FALSE ;

	if ((sep->program != NULL) && (sep->program[0] != '\0'))
	    return TRUE ;

	if ((sep->args != NULL) && (sep->args[0] != '\0'))
	    return TRUE ;

	return FALSE ;
}
/* end subroutine (entry_enough) */


/* load up some groups into the current entry */
static int entry_groupsload(sep,buf,buflen)
SRVTAB_ENT	*sep ;
cchar	buf[] ;
int		buflen ;
{
	field		fsb ;
	int	rs ;
	int	fl ;
	cchar	*fp ;

	if ((rs = field_start(&fsb,buf,buflen)) >= 0) {

	while ((fl = field_get(&fsb,key_terms,&fp)) >= 0) {
	    if (fl == 0) continue ;

	    rs = entry_groupadd(sep,fp) ;
	    if (rs < 0) break ;

	} /* end while */

	field_finish(&fsb) ;
	} /* end if (field) */

	return rs ;
}
/* end subroutine (entry_groupsload) */

/* add another (single) group to the current entry */
static int entry_groupadd(SEVTAB_ENT *sep,cchar *name) noex {
	int		rs = SR_OK ;
	int		i = 0 ;
	/* enter the raw group name into a group slot */
	while ((i < NGROUPS_MAX) && (sep->groupnames[i] != NULL)) {
	    i += 1 ;
	}
	rs = i ;
	if (i < NGROUPS_MAX) {
	    cchar	*sp ;
	    rs = uc_mallocstrw(name,-1,&sp) ;

	    if (rs >= 0) {
	        sep->groupnames[i++] = sp ;
	        sep->groupnames[i] = NULL ;
	    }

	} else
	    rs = SR_TOOBIG ;

	if (rs >= 0)
	    sep->ngroups = i ;

	return rs ;
}
/* end subroutine (entry_groupadd) */

/* add something to an existing string */
static int stradd(cchar **spp,cchar *s,int slen) noex {
	int	rs = SR_OK ;
	int	sl ;
	int	len = 0 ;
	cchar	*sp = *spp ;

	len = (slen + 1) ;
	if (sp != NULL) {
	    char	*cp ;
	    char	*osp = (char *) sp ;

	    sl = strlen(sp) ;

	    len += (sl + 1) ;
	    if ((rs = uc_realloc(osp,len,&cp)) >= 0) {
		*spp = cp ;
		cp += sl ;
		*cp++ = CH_US ;
	        strwcpy(cp,s,slen) ;
	    } else {
	        *spp = NULL ;
	    }

	} else {
	    rs = uc_mallocstrw(s,slen,&sp) ;
	    len = rs ;
	    if (rs >= 0) *spp = sp ;
	}

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (stradd) */

static void freeit(cchar **pp) noex {
	if (*pp) {
	    uc_free(*pp) ;
	    *pp = NULL ;
	}
}
/* end subroutine (freeit) */


