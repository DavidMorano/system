/* ucnprocs */
/* lang=C++20 */

/* get the number of processes on the system */


/* revision history:

	= 1997-11-18, David A-D­ Morano
        This little subroutine was put together to get the current number of
        processes executing on the system.

	= 2019-01-14, David A-D- Morano
	Enhanced (after all this time) to ignore special entries (now common on
	newer OSes).

*/

/* Copyright © 1998,2019 David D-A- Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	uc_nprocs

	Description:
	We basically cheat and count the number of "files" in '/proc'
	that start with a leading digit character. The filename may
	have to return a good |stat(2)| also. Of course, the whole
	file-system might be unmounted also, in which case we return
	with failure.

	Synopsis:
	int uc_nprocs(int w) noex

	Arguments:
	w		which processes to count:
				0=all
				1=system-only
				2=current-user
				3=current-session

	Returns:
	<0		error
	>=0		number of processes on the machine

	Notes:
	Amazingly, the '/proc' file-system with file entries in it
	that look like process IDs is probably the most portable
	way invented so far to enumerate process IDs on the system.
	I think we can all thank AT&T for this, for inventing the
	PROC FS back in the mid-1980s or so. It is amazing how much
	stuff that AT&T invented in the 1980s for UNIX® that has
	passed the test of time. The only real problem with the
	PROC FS is that it is possible that it can be unmounted
	(not mounted), in which case everyone has nothing!

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<usystem.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<getbufsize.h>
#include	<mkchar.h>
#include	<fsdir.h>
#include	<localmisc.h>


/* local defines */

#ifndef	PROCDNAME
#define	PROCDNAME	"/proc"
#endif

#ifndef	UIDSYS
#define	UIDSYS		100		/* common but unofficial value */
#endif


/* external subroutines */

extern "C" {
    extern int	mkpath1(char *,cchar *) noex ;
    extern int	pathadd(char *,int,cchar *) noex ;
    extern int	cfdeci(cchar *,int,int *) noex ;
    extern int	isdigitlatin(int) noex ;
}


/* external variables */


/* local structures */


/* forward references */

static int	ucnprocs_fsdir(char *,int,int) noex ;


/* local variables */


/* exported subroutines */

int uc_nprocs(int w) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	char		*nbuf ;
	if ((rs = getbufsize(getbufsize_mn)) >= 0) {
	    cint	nlen = rs ;
	    if ((rs = uc_libmalloc(nlen,&nbuf)) >= 0) {
		{
	            rs = ucnprocs_fsdir(nbuf,nlen,w) ;
		    c = rs ;
		}
		rs1 = uc_libfree(nbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (lib-allocation-free) */
	} /* end if (getbufsize) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (uc_nprocs) */


/* local subroutines */

static int ucnprocs_fsdir(char *nbuf,int nlen,int w) noex {
	FSDIR		d ;
	FSDIR_ENT	de ;
	int		rs ;
	int		rs1 ;
	int		n = 0 ;
	cchar		*procdname = PROCDNAME ;
        if ((rs = fsdir_open(&d,procdname)) >= 0) {
            if ((rs = getbufsize(getbufsize_mp)) >= 0) {
                cint        plen = rs ;
                char        *pbuf ;
                switch (w) {
                case 0: /* all processes */
                    {
                        while ((rs = fsdir_read(&d,&de,nbuf,nlen)) > 0) {
                            cint    ch = mkchar(de.name[0]) ;
                            if (isdigitlatin(ch)) {
                                n += 1 ;
                            }
                        } /* end while */
                    } /* end block */
                    break ;
                case 1: /* system processes */
                    if ((rs = uc_libmalloc((plen+1),&pbuf)) >= 0) {
                        if ((rs = mkpath1(pbuf,procdname)) >= 0) {
                            cint    pl = rs ;
                            while ((rs = fsdir_read(&d,&de,nbuf,nlen)) > 0) {
                                cint        ch = mkchar(de.name[0]) ;
                                if (isdigitlatin(ch)) {
                                    cchar   *np = de.name ;
                                    if ((rs = pathadd(pbuf,pl,np)) >= 0) {
                                        USTAT       sb ;
                                        if (u_stat(pbuf,&sb) >= 0) {
                                            if (sb.st_uid < UIDSYS) n += 1 ;
                                        }
                                    }
                                }
                                if (rs < 0) break ;
                            } /* end while */
                        } /* end if (mkpath) */
                        uc_libfree(pbuf) ;
                    } /* end if (m-a) */
                    break ;
                case 2: /* user processes */
                    if ((rs = uc_libmalloc((plen+1),&pbuf)) >= 0) {
                        if ((rs = mkpath1(pbuf,procdname)) >= 0) {
                            const uid_t		uid = getuid() ;
                            cint            pl = rs ;
                            while ((rs = fsdir_read(&d,&de,nbuf,nlen)) > 0) {
                                cint        ch = mkchar(de.name[0]) ;
                                if (isdigitlatin(ch)) {
                                    cchar   *np = de.name ;
                                    if ((rs = pathadd(pbuf,pl,np)) >= 0) {
                                        USTAT       sb ;
                                        if (u_stat(pbuf,&sb) >= 0) {
                                            if (sb.st_uid == uid) n += 1 ;
                                        }
                                    }
                                }
                                if (rs < 0) break ;
                            } /* end while */
                        } /* end if (mkpath) */
                        uc_libfree(pbuf) ;
                    } /* end if (m-a) */
                    break ;
                case 3: /* session processes */
                    {
                        const pid_t	sid = getsid(0) ;
                        while ((rs = fsdir_read(&d,&de,nbuf,nlen)) > 0) {
                            cint    ch = mkchar(de.name[0]) ;
                            if (isdigitlatin(ch)) {
                                int v ;
                                if (cfdeci(de.name,rs,&v) >= 0) {
                                    const pid_t             cid = v ;
                                    if (cid > 0) {
                                        pid_t               csid ;
                                        if ((csid = getsid(cid)) >= 0) {
                                            if (csid == sid) n += 1 ;
                                        }
                                    }
                                }
                            }
                        } /* end while */
                    } /* end block */
                    break ;
                default:
                    rs = SR_NOSYS ;
                    break ;
                } /* end switch */
            } /* end if (getbufsize) */
            rs1 = fsdir_close(&d) ;
            if (rs >= 0) rs = rs1 ;
        } /* end if (fsdir) */
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (uc_nprocs) */


