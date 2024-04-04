/* pwfile SUPPORT */
/* lang=C++20 */

/* open a 'passwd' formatted file for access */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-01, David A­D­ Morano
	This subroutine (object) module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This module manipulates passsword records in a 'passwd'
	type file.  No, it is not particularly pretty but such is
	life often times!

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<pwd.h>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<pwentry.h>
#include	<gecos.h>
#include	<bfile.h>
#include	<hdb.h>
#include	<storeitem.h>
#include	<mallocstuff.h>
#include	<strn.h>
#include	<snwcpy.h>
#include	<cfdec.h>
#include	<lockfile.h>
#include	<localmisc.h>

#include	"pwfile.h"


/* local defines */

#ifndef	LINEBUFLEN
#ifdef	LINE_MAX
#define	LINEBUFLEN	MAX(LINE_MAX,2048)
#else
#define	LINEBUFLEN	2048
#endif
#endif

#define	DEFENTRIES	10
#define	TO_LOCK		60

#ifndef	F_LOCK
#define	F_ULOCK	0
#define	F_LOCK	1
#define	F_TLOCK	2
#define	F_TEST	3
#endif


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static int pwfile_ctor(pwfile *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    memclear(op) ;
	    rs = SR_NOMEM ;
	    if ((op->alp = new(nothrow) vecitem) != nullptr) {
	        if ((op->ulp = new(nothrow) hdb) != nullptr) {
		    rs = SR_OK ;
		} /* end if (new-hdb) */
		if (rs < 0) {
		    delete op->alp ;
		    op->alp = nullptr ;
		}
	    } /* end if (new-vecitem) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (pwfile_ctor) */

static int pwfile_dtor(pwfile *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->ulp) {
		delete op->ulp ;
		op->ulp = nullptr ;
	    }
	    if (op->alp) {
		delete op->alp ;
		op->alp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (pwfile_dtor) */

template<typename ... Args>
static int pwfile_magic(pwfile *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == PWFILE_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (pwfile_magic) */

static int	pwfile_loadbegin(pwfile *) noex ;
static int	pwfile_loadend(pwfile *) noex ;
static int	pwfile_filefront(pwfile *) noex ;
static int	pwfile_filefronter(pwfile *) noex ;
static int	pwfile_fileback(pwfile *) noex ;

static int	pwentry_start(pwentry *) noex ;
static int	pwentry_finish(pwentry *) noex ;
static int	pwentry_fieldpw(pwentry *,int,cchar *,int) noex ;
static int	pwentry_mkextras(pwentry *) noex ;
static int	pwentry_mkcopy(pwentry *,pwentry *,char *,int) noex ;

static int	loaditem(cchar **,cchar *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int pwfile_open(pwfile *op,cchar *pwfname) noex {
	int		rs ;
	if ((rs = pwfile_ctor(op,pwfname)) >= 0) {
	    rs = SR_INVALID ;
	    if (pwfname[0]) {
	        cchar	*cp{} ;
	        if ((rs = uc_mallocstrw(pwfname,-1,&cp)) >= 0) {
	            op->fname = cp ;
	            op->lfd = -1 ;
	            if ((rs = pwfile_loadbegin(op)) >= 0) {
	                op->f = {} ;
	                op->magic = PWFILE_MAGIC ;
	            }
	            if (rs < 0) {
	                uc_free(op->fname) ;
	                op->fname = nullptr ;
	            }
	        } /* end if (m-a) */
	    } /* end if (valid) */
	    if (rs < 0) {
		pwfile_dtor(op) ;
	    }
	} /* end if (pwfile_ctor) */
	return rs ;
}
/* end subroutine (pwfile_open) */

int pwfile_close(pwfile *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = pwfile_magic(op)) >= 0) {
	    {
	        rs1 = pwfile_loadend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->lfd >= 0) {
	        rs1 = u_close(op->lfd) ;
	        if (rs >= 0) rs = rs1 ;
	        op->lfd = -1 ;
	    }
	    if (op->fname) {
	        rs1 = uc_free(op->fname) ;
	        if (rs >= 0) rs = rs1 ;
	        op->fname = nullptr ;
	    }
	    {
		rs1 = pwfile_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (pwfile_close) */

int pwfile_enum(pwfile *op,pwfile_cur *curp,pwentry *uep,
		char *rbuf,int rlen) noex {
	int		rs ;
	if ((rs = pwfile_magic(op,curp,uep,rbuf)) >= 0) {
	    pwentry	*ep{} ;
	    while ((rs = vecitem_get(op->alp,curp->i,&ep)) >= 0) {
	        curp->i += 1 ;
	        if (ep != nullptr) break ;
	    } /* end while */
/* load up the user's structure with the information from our internal one */
	    if (rs >= 0) {
	        rs = pwentry_mkcopy(ep,uep,rbuf,rlen) ;
	    } else if (rs == SR_NOTFOUND) {
	        rs = SR_OK ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (pwfile_enum) */

/* fetch the next entry that matches the specified username */
int pwfile_fetchuser(pwfile *op,cc *username,pwfile_cur *curp,
		pwentry *uep,char *rbuf,int rlen) noex {
	int		rs ;
	if ((rs = pwfile_magic(op,username,curp,uep,rbuf)) >= 0) {
	    rs = SR_INVALID ;
	    if (username[0]) {
	        USTAT	sb ;
	        if (op->lfd < 0) {
	            rs = u_stat(op->fname,&sb) ;
	        } else {
	            rs = u_fstat(op->lfd,&sb) ;
	        }
	        if (rs >= 0) {
	            hdb_dat	key ;
	            hdb_dat	val{} ;
	            hdb_cur	*hcurp = ((curp) ? curp->hcp : nullptr) ;
	            if (sb.st_mtime > op->readtime) {
	                pwfile_loadend(op) ;
	                pwfile_loadbegin(op) ;
	            } /* end if */
/* continue with the query */
	            key.buf = username ;
	            key.len = strlen(username) ;
	            if ((rs = hdb_fetch(op->ulp,key,hcurp,&val)) >= 0) {
	                if (uep) {
	                    pwentry	*ep = (pwentry *) val.buf ;
	                    rs = pwentry_mkcopy(ep,uep,rbuf,rlen) ;
	                }
	            } /* end if (hdb_fetch) */
	        } /* end if (ok) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (pwfile_fetchuser) */

int pwfile_curbegin(pwfile *op,pwfile_cur *curp) noex {
	int		rs ;
	if ((rs = pwfile_magic(op,curp)) >= 0) {
	    bool	f_locked = false ;
	    if (op->lfd < 0) {
		cint	of = O_RDONLY ;
	        rs = u_open(op->fname,of,0666) ;
	        op->lfd = rs ;
	    } /* end if (it wasn't open) */
	    if (rs >= 0) {
		int	cmd ;
	        if (! op->f.locked) {
		    cint	to = TO_LOCK ;
		    cmd = F_RLOCK ;
	            if ((rs = lockfile(op->lfd,cmd,0L,0L,to)) < 0) {
	                u_close(op->lfd) ;
	                op->lfd = -1 ;
	                return rs ;
	            }
	            op->f.locked = op->f.locked_cur = true ;
	            f_locked = true ;
	        } /* end if (not locked) */
	        curp->i = 0 ;
	        rs = hdb_curbegin(op->ulp,curp->hcp) ;
	        if ((rs < 0) && f_locked) {
	            op->f.locked = op->f.locked_cur = false ;
#ifdef	COMMENT
		    cmd = F_ULOCK ;
	            lockfile(op->lfd,cmd,0L,0L,to) ;
#else
	            u_close(op->lfd) ;
	            op->lfd = -1 ;
#endif
	        } /* end if (cleanup on error) */
	    } /* end if (ok) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (pwfile_curbegin) */

int pwfile_curend(pwfile *op,pwfile_cur *curp) noex {
	int		rs ;
	if ((rs = pwfile_magic(op,curp)) >= 0) {
	    if (op->f.locked_cur && (! op->f.locked_explicit)) {
		cint	to = TO_LOCK ;
	        op->f.locked = false ;
	        lockfile(op->lfd,F_ULOCK,0L,0L,to) ;
	    }
	    op->f.locked_cur = false ;
	    curp->i = 0 ;
	    rs = hdb_curend(op->ulp,curp->hcp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (pwfile_curend) */

int pwfile_lock(pwfile *op,int type,int to_lock) noex {
	int		rs ;
	int		f_opened = false ;
	if ((rs = pwfile_magic(op)) >= 0) {
	    if (op->lfd < 0) {
		cint	of = O_RDONLY ;
	        rs = u_open(op->fname,of,0666) ;
	        op->lfd = rs ;
	        f_opened = (rs >= 0) ;
	    }
	    if (rs >= 0) {
	        switch (type) {
	        case F_ULOCK:
	            if (op->f.locked_explicit) {
	                op->f.locked = op->f.locked_explicit = false ;
	                rs = lockfile(op->lfd,type,0L,0L,to_lock) ;
	                if (rs < 0) {
	                    u_close(op->lfd) ;
	                    op->lfd = -1 ;
	                }
	            } else {
	                rs = SR_INVALID ;
	            }
	            break ;
	        default:
	        case F_RTEST:
	        case F_WTEST:
	            rs = SR_LOCKED ;
	            if (! op->f.locked) {
	                rs = lockfile(op->lfd,type,0L,0L,to_lock) ;
#ifdef	COMMENT
	                if (f_opened) {
	                    u_close(op->lfd) ;
	                    op->lfd = -1 ;
	                }
#endif /* COMMENT */
	            } /* end if (not locked already) */
	            break ;
	        case F_RLOCK:
	        case F_WLOCK:
	        case F_TRLOCK:
	        case F_TWLOCK:
	            if (! op->f.locked) {
			cint	to = to_lock ;
	                if ((rs = lockfile(op->lfd,type,0L,0L,to)) >= 0) {
	                    op->f.locked = op->f.locked_explicit = true ;
	                }
	            } else {
	                rs = SR_INVALID ;
	            }
	            break ;
	        } /* end switch */
	    } /* end if (ok) */
	} /* end if (magic) */
	return (rs >= 0) ? f_opened : rs ;
}
/* end subroutine (pwfile_lock) */


/* private subroutines */

static int pwfile_loadbegin(pwfile *op) noex {
	int		rs ;
	int		n = 0 ;
	if ((rs = pwfile_filefront(op)) >= 0) {
	    cnullptr	np{} ;
	    hdb_dat	key ;
	    hdb_dat	val ;
	    n = rs ;
	    if ((rs = hdb_start(op->ulp,n,0,np,np)) >= 0) {
	        vecitem	*alp = op->alp ;
	        pwentry	*ep ;
	        for (int i = 0 ; vecitem_get(alp,i,&ep) >= 0 ; i += 1) {
	            if (ep) {
	                key.buf = ep->username ;
	                key.len = strlen(ep->username) ;
	                val.buf = ep ;
	                val.len = sizeof(pwentry) ;
	                rs = hdb_store(op->ulp,key,val) ;
	            }
	            if (rs < 0) break ;
	        } /* end for */
	        if (rs < 0) {
	            hdb_finish(op->ulp) ;
	        }
	    }
	    if (rs < 0) {
	        pwfile_fileback(op) ;
	    }
	} /* end if (pwfile_filefront) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (pwfile_loadbegin) */

static int pwfile_loadend(pwfile *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->ulp) {
	    rs1 = hdb_finish(op->ulp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	{
	    rs1 = pwfile_fileback(op) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (pwfile_loadend) */

static int pwfile_filefront(pwfile *op) noex {
	int		rs = SR_NOENTRY ;
	if (op->fname[0]) {
	    USTAT	sb ;
	    if ((rs = uc_stat(op->fname,&sb)) >= 0) {
	        vecitem	*alp = op->alp ;
	        cint	vo = VECITEM_OCOMPACT ;
	        int	ne = ((sb.st_size / 60) + 5) ;
	        if (ne < DEFENTRIES) ne = DEFENTRIES ;
	        if ((rs = vecitem_start(alp,ne,vo)) >= 0) {
	            op->readtime = sb.st_mtime ;
	            rs = pwfile_filefronter(op) ;
	            if (rs < 0) {
	                vecitem_finish(alp) ;
		    }
	        }
	    } /* end if (uc_stat) */
	} /* end if (valid) */
	return rs ;
}
/* end subroutine (pwfile_filefront) */

static int pwfile_filefronter(pwfile *op) noex {
	bfile		pwfile, *fp = &pwfile ;
	int		rs ;
	int		rs1 ;
	int		n = 0 ;
	if ((rs = bopen(fp,op->fname,"rc",0644)) >= 0) {
	    if (! op->f.locked) {
	        rs = bcontrol(fp,BC_LOCKREAD,TO_LOCK) ;
	    }
	    if (rs >= 0) {
	        pwentry		entry ;
	        cint		llen = LINEBUFLEN ;
	        int		len ;
	        char		lbuf[LINEBUFLEN+1] ;
	        while ((rs = breadln(fp,lbuf,llen)) > 0) {
	            len = rs ;
	            if (lbuf[len - 1] == '\n') len -= 1 ;
	            lbuf[len] = '\0' ;
	            if ((rs = pwentry_start(&entry)) >= 0) {
	                int		fn = 0 ;
	                cchar		*tp ;
	                cchar		*cp = lbuf ;
	                while ((tp = strchr(cp,':')) != nullptr) {
	                    rs = pwentry_fieldpw(&entry,fn,cp,(tp - cp)) ;
	                    cp = (tp + 1) ;
	                    fn += 1 ;
	                    if (rs < 0) break ;
	                } /* end while */
	                if ((rs >= 0) && (cp[0] != '\0')) {
	                    rs = pwentry_fieldpw(&entry,fn,cp,-1) ;
	                }
/* make any extras fields that we want */
	                if (rs >= 0) {
	                    rs = pwentry_mkextras(&entry) ;
	                }
/* add the entry to our list */
	                if (rs >= 0) {
	                    cint	esize = sizeof(pwentry) ;
	                    n += 1 ;
	                    rs = vecitem_add(op->alp,&entry,esize) ;
	                }
	                if (rs < 0) {
	                    pwentry_finish(&entry) ;
			}
	            } /* end if (initialized new entry) */
	            if (rs < 0) break ;
	        } /* end while (reading file entries) */
	        if (! op->f.locked) {
	            bcontrol(fp,BC_UNLOCK,0) ;
	        }
	    } /* end if (ok) */
	    rs1 = bclose(fp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (bfile) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (pwfile_filefronter) */

static int pwfile_fileback(pwfile *op) noex {
	vecitem		*alp = op->alp ;
	pwentry		*ep ;
	int		rs = SR_OK ;
	int		rs1 ;
	for (int i = 0 ; vecitem_get(alp,i,&ep) >= 0 ; i += 1) {
	    if (ep) {
	        rs1 = pwentry_finish(ep) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end for */
	{
	    rs1 = vecitem_finish(alp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (pwfile_fileback) */


#ifdef	COMMENT

An actual 'passwd' entry in the file has the fields (in order):

name
passwd
uid
gid
gecos
dir
shell
lstchg		/* last change (in days) */
min		/* minimum number of days between password changes */
max		/* number of days password is valid */
warn		/* number of days to warn user to change passwd */
inact		/* number of days the login may be inactive */
expire		/* date when the login is no longer valid */
flag		/* currently not being used */

These fields are loaded into the following standard structures:

struct passwd {
	char	*pw_name ;
	char	*pw_passwd ;
	uid_t	pw_uid ;
	gid_t	pw_gid ;
	char	*pw_age ;
	char	*pw_comment ;
	char	*pw_gecos ;
	char	*pw_dir ;
	char	*pw_shell ;
} ;

struct spwd {
	char *sp_namp;	/* user name */
	char *sp_pwdp;	/* user password */
	long sp_lstchg;	/* password lastchanged date */
	long sp_min;	/* minimum number of days between password changes */
	long sp_max;	/* number of days password is valid */
	long sp_warn;	/* number of days to warn user to change passwd */
	long sp_inact;	/* number of days the login may be inactive */
	long sp_expire;	/* date when the login is no longer valid */
	unsigned long sp_flag;	/* currently not being used */
} ;

A typical 'passwd' file entry looks like:

dam:
gWI5lTXfCPT6g:
201:
309:
d.a.morano:
/home/dam:
/bin/ksh:
10990:
-1:
-1:
-1:
-1:
:
0

#endif /* COMMENT */

/* initialize a 'passwd' entry */
static int pwentry_start(pwentry *ep) noex {
	int		rs = SR_FAULT ;
	if (ep) {
	    rs = memclear(ep) ;
	    ep->lstchg = -1 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (pwentry_start) */

static int pwentry_fieldpw(pwentry *ep,int fn,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (ep && sp) {
	    int		v ;
	    cchar	*mp = nullptr ;
	    rs = SR_OK ;
	    if (sl < 0) sl = strlen(sp) ;
	    switch (fn) {
	    case 0:
	        ep->username = mp = mallocstrw(sp,sl) ;
	        break ;
	    case 1:
	        ep->password = mp = mallocstrw(sp,sl) ;
	        break ;
	    case 2:
	        mp = sp ;
	        ep->uid = -1 ;
	        if ((sl > 0) && (cfdeci(sp,sl,&v) >= 0)) ep->uid = v ;
	        break ;
	    case 3:
	        mp = sp ;
	        ep->gid = -1 ;
	        if ((sl > 0) && (cfdeci(sp,sl,&v) >= 0)) ep->gid = v ;
	        break ;
	    case 4:
	        ep->gecos = mp = mallocstrw(sp,sl) ;
	        break ;
	    case 5:
	        ep->dir = mp = mallocstrw(sp,sl) ;
	        break ;
	    case 6:
	        ep->shell = mp = mallocstrw(sp,sl) ;
	        break ;
	    case 7:
	        mp = sp ;
	        ep->lstchg = -1 ;
	        if (sl > 0) cfdecl(sp,sl,&ep->lstchg) ;
	        break ;
	    case 8:
	        mp = sp ;
	        ep->daymin = -1 ;
	        if (sl > 0) cfdecl(sp,sl,&ep->daymin) ;
	        break ;
	    case 9:
	        mp = sp ;
	        ep->daymax = -1 ;
	        if (sl > 0) cfdecl(sp,sl,&ep->daymax) ;
	        break ;
	    case 10:
	        mp = sp ;
	        ep->warn = -1 ;
	        if (sl > 0) cfdecl(sp,sl,&ep->warn) ;
	        break ;
	    case 11:
	        mp = sp ;
	        ep->inact = -1 ;
	        if (sl > 0) cfdecl(sp,sl,&ep->inact) ;
	        break ;
	    case 12:
	        mp = sp ;
	        ep->expire = -1 ;
	        if (sl > 0) cfdecl(sp,sl,&ep->expire) ;
	        break ;
	    case 13:
	        mp = sp ;
	        if (sl > 0) {
		    if (cfdec(sp,sl,&v)) {
	                ep->flag = v ;
		    }
		}
	        break ;
	    default:
	        mp = sp ;
	        break ;
	    } /* end switch */
	    rs = ((mp == nullptr) ? SR_NOMEM : SR_OK) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (pwentry_fieldpw) */

static int pwentry_mkextras(pwentry *ep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (ep) {
	    rs = SR_OK ;
	    if (ep->gecos != nullptr) {
	        gecos	g ;
	        if ((rs = gecos_start(&g,ep->gecos,-1)) >= 0) {
	            for (int i = 0 ; i < gecosval_overlast ; i += 1) {
	                cchar	*vp{} ;
	                if (int vl ; (vl = gecos_getval(&g,i,&vp)) >= 0) {
	                    if (vp != nullptr) {
	                        void	*p{} ;
	                        switch (i) {
	                        case gecosval_organization:
	                            rs = loaditem(&ep->organization,vp,vl) ;
	                            break ;
	                        case gecosval_realname:
	                            if ((rs = uc_malloc((vl+1),&p)) >= 0) {
	                                char	*nbuf = charp(p) ;
	                                if (strnchr(vp,vl,'_') != nullptr) {
	                                    rs = snwcpyhyphen(nbuf,-1,vp,vl) ;
	                                    vp = nbuf ;
	                                }
	                                if (rs >= 0) {
	                                    rs = loaditem(&ep->realname,vp,vl) ;
	                                }
	                                rs1 = uc_free(p) ;
					if (rs >= 0) rs = rs1 ;
	                            } /* end if (m-a-f) */
	                            break ;
	                        case gecosval_account:
	                            rs = loaditem(&ep->account,vp,vl) ;
	                            break ;
	                        case gecosval_bin:
	                            rs = loaditem(&ep->bin,vp,vl) ;
	                            break ;
	                        case gecosval_office:
	                            rs = loaditem(&ep->office,vp,vl) ;
	                            break ;
	                        case gecosval_wphone:
	                            rs = loaditem(&ep->wphone,vp,vl) ;
	                            break ;
	                        case gecosval_hphone:
	                            rs = loaditem(&ep->hphone,vp,vl) ;
	                            break ;
	                        case gecosval_printer:
	                            rs = loaditem(&ep->printer,vp,vl) ;
	                            break ;
	                        } /* end switch */
	                    } /* end if (non-nullptr) */
	                } /* end if (gecos-getval) */
	                if (rs < 0) break ;
	            } /* end for */
	            rs1 = gecos_finish(&g) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (gecos) */
	    } /* end if (non-null) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (pwentry_mkextras) */

static int pwentry_mkcopy(pwentry *ep,pwentry *uep,char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (ep && uep && rbuf) {
	    storeitem	ubuf ;
	    memclear(uep) ;
	    if ((rs = storeitem_start(&ubuf,rbuf,rlen)) >= 0) {
	        if (ep->username != nullptr) {
	            storeitem_strw(&ubuf,ep->username,-1,&uep->username) ;
	        }
	        if (ep->password != nullptr) {
	            storeitem_strw(&ubuf,ep->password,-1,&uep->password) ;
	        }
#ifdef	OPTIONAL
	        uep->uid = ep->uid ;
	        uep->gid = ep->gid ;
#endif
	        if (ep->gecos != nullptr) {
	            storeitem_strw(&ubuf,ep->gecos,-1,&uep->gecos) ;
	        }
/* the standard AT&T stuff */
	        if (ep->organization != nullptr) {
		    cchar	**cpp = &uep->organization ;
	            storeitem_strw(&ubuf,ep->organization,-1,cpp) ;
	        }
	        if (ep->realname != nullptr) {
	            storeitem_strw(&ubuf,ep->realname,-1,&uep->realname) ;
	        }
	        if (ep->account != nullptr) {
	            storeitem_strw(&ubuf,ep->account,-1,&uep->account) ;
	        }
	        if (ep->bin != nullptr) {
	            storeitem_strw(&ubuf,ep->bin,-1,&uep->bin) ;
	        }
/* the finger information stuff */
	        if (ep->office != nullptr) {
	            storeitem_strw(&ubuf,ep->office,-1,&uep->office) ;
	        }
	        if (ep->wphone != nullptr) {
	            storeitem_strw(&ubuf,ep->wphone,-1,&uep->wphone) ;
	        }
	        if (ep->hphone != nullptr) {
	            storeitem_strw(&ubuf,ep->hphone,-1,&uep->hphone) ;
	        }
	        if (ep->printer != nullptr) {
	            storeitem_strw(&ubuf,ep->printer,-1,&uep->printer) ;
	        }
/* the remaining standard password stuff */
	        if (ep->dir != nullptr) {
	            storeitem_strw(&ubuf,ep->dir,-1,&uep->dir) ;
		}
	        if (ep->shell != nullptr) {
	            storeitem_strw(&ubuf,ep->shell,-1,&uep->shell) ;
	        }
	        len = storeitem_finish(&ubuf) ;
	        if (rs >= 0) rs = len ;
	    } /* end if (storeitem) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (pwentry_mkcopy) */

static int pwentry_finish(pwentry *ep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (ep) {
	    rs = SR_OK ;
	    if (ep->username != nullptr) {
	        rs1 = uc_free(ep->username) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (ep->password != nullptr) {
	        rs1 = uc_free(ep->password) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (ep->gecos != nullptr) {
	        rs1 = uc_free(ep->gecos) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (ep->dir != nullptr) {
	        rs1 = uc_free(ep->dir) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (ep->shell != nullptr) {
	        rs1 = uc_free(ep->shell) ;
	        if (rs >= 0) rs = rs1 ;
	    }
/* the AT&T standard extras */
	    if (ep->organization != nullptr) {
	        rs1 = uc_free(ep->organization) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (ep->realname != nullptr) {
	        rs1 = uc_free(ep->realname) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (ep->account != nullptr) {
	        rs1 = uc_free(ep->account) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (ep->bin != nullptr) {
	        rs1 = uc_free(ep->bin) ;
	        if (rs >= 0) rs = rs1 ;
	    }
/* the sometimes pseudo-standard finger information fields */
	    if (ep->office != nullptr) {
	        rs1 = uc_free(ep->office) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (ep->wphone != nullptr) {
	        rs1 = uc_free(ep->wphone) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (ep->hphone != nullptr) {
	        rs1 = uc_free(ep->hphone) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (ep->printer != nullptr) {
	        rs1 = uc_free(ep->printer) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    memclear(ep) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (pwentry_finish) */

static int loaditem(cchar **rpp,cchar *vp,int vl) noex {
	int		rs = SR_FAULT ;
	if (vp) {
	    rs = uc_mallocstrw(vp,vl,rpp) ;
	}
	return rs ;
}
/* end subroutine (loaditem) */


