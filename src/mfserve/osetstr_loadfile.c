/* osetstr_loadfile */

/* load strings from a file */
/* version %I% last-modified %G% */


#define	CF_DEBUGS	0		/* compile-time debug print-outs */


/* revision history:

	= 1998-09-10, David A�D� Morano
	This module was originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine will read (process) a file and put all of the strings
	found into the string (supplied) list, consisting of a OSETSTR object.

	Synopsis:

	int osetstr_loadfile(vsp,fo,fname)
	osetstr		*vsp ;
	int		fo ;
	const char	fname[] ;

	Arguments:

	vsp		pointer to OSETSTR object
	fo		0=ignore, 1=replace
	fname		file to load

	Returns:

	>=0		number of elements loaded
	<0		error

	Notes:

        Why use FILER over BFILE? Yes, FILER is a tiny bit more lightweight
        than BFILE -- on a good day. But the real reason may be so that we don't
        need to load BFILE in code that resides very deep in a software stack if
        we don't need it -- like deep inside loadable modules. Anyway, just a
        thought!

        Why are we using FIELD as opposed to 'nextfield()' or something similar?
        Because our sematics are to process quoted strings as a single OSETSTR
        entry!

	Note_on_uniqueness:

	The 'fo' argument:
		0=ignore
		1=replace


*******************************************************************************/


#include	<envstandards.h>	/* MUST be first to configure */

#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<limits.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<stdlib.h>
#include	<string.h>

#include	<usystem.h>
#include	<osetstr.h>
#include	<filer.h>
#include	<field.h>
#include	<localmisc.h>


/* local defines */

#ifndef	LINEBUFLEN
#ifdef	LINE_MAX
#define	LINEBUFLEN	MAX(LINE_MAX,2048)
#else
#define	LINEBUFLEN	2048
#endif
#endif

#ifndef	FD_STDIN
#define	FD_STDIN	0
#endif

#define	TO_READ		-1		/* read timeout */


/* external subroutines */

extern int	osetstr_adduniq(osetstr *,const char *,int) ;

#if	CF_DEBUGS
extern int	debugprintf(const char *,...) ;
extern int	strlinelen(const char *,int,int) ;
#endif


/* external variables */


/* forward references */

static int	osetstr_loadfd(OSETSTR *,int,int) ;
static int	osetstr_loadline(OSETSTR *,int,const char *,int) ;


/* local structures */


/* local variables */

static const uchar	fterms[32] = {
	0x00, 0x04, 0x00, 0x00,
	0x08, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ;


/* exported subroutines */


int osetstr_loadfile(osetstr *vsp,int fu,cchar *fname)
{
	int		rs = SR_OK ;
	int		fd = FD_STDIN ;
	int		c = 0 ;
	int		f_opened = FALSE ;

	if (vsp == NULL) return SR_FAULT ;
	if (fname == NULL) return SR_FAULT ;

	if (fname[0] == '\0') return SR_INVALID ;

	if (strcmp(fname,"-") != 0) {
	    if ((rs = uc_open(fname,O_RDONLY,0666)) >= 0) {
	        fd = rs ;
	        f_opened = TRUE ;
	    }
	}

	if (rs >= 0) {
	    rs = osetstr_loadfd(vsp,fu,fd) ;
	    c = rs ;
	}

	if (f_opened && (fd >= 0)) {
	    u_close(fd) ;
	}

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (osetstr_loadfile) */


/* local subroutines */


static int osetstr_loadfd(osetstr *vsp,int fu,int fd)
{
	struct ustat	sb ;
	int		rs ;
	int		rs1 ;
	int		to = -1 ;
	int		c = 0 ;

	if ((rs = u_fstat(fd,&sb)) >= 0) {
	    if (! S_ISDIR(sb.st_mode)) {
		FILER	loadfile, *lfp = &loadfile ;
		int	fbsize = 1024 ;
		int	fbo = 0 ;

	        if (S_ISREG(sb.st_mode)) {
	            int	fs = ((sb.st_size == 0) ? 1 : (sb.st_size & INT_MAX)) ;
	            int	cs ;
	            cs = BCEIL(fs,512) ;
	            fbsize = MIN(cs,1024) ;
	        } else {
	            to = TO_READ ;
	            if (S_ISSOCK(sb.st_mode)) fbo |= FILER_ONET ;
	        }

	        if ((rs = filer_start(lfp,fd,0L,fbsize,fbo)) >= 0) {
	            const int	llen = LINEBUFLEN ;
	            int		len ;
	            char	lbuf[LINEBUFLEN + 1] ;

	            while ((rs = filer_readln(lfp,lbuf,llen,to)) > 0) {
	                len = rs ;

	                if (lbuf[len - 1] == '\n') len -= 1 ;

			if ((len > 0) && (lbuf[0] != '#')) {
			    rs = osetstr_loadline(vsp,fu,lbuf,len) ;
			    c += rs ;
			}

	                if (rs < 0) break ;
	            } /* end while (reading lines) */

	            rs1 = filer_finish(lfp) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (filer) */

	    } else {
	        rs = SR_ISDIR ;
	    }
	} /* end if (stat) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (osetstr_loadfd) */


static int osetstr_loadline(OSETSTR *vsp,int fu,const char *lbuf,int len)
{
	FIELD		fsb ;
	int		rs ;
	int		c = 0 ;
	if ((rs = field_start(&fsb,lbuf,len)) >= 0) {
	    int		fl ;
	    cchar	*fp ;
	    while ((fl = field_get(&fsb,fterms,&fp)) >= 0) {
		if (fl > 0) {
		    if (fu) {
			rs = osetstr_del(vsp,fp,fl) ;
		    }
		    if (rs >= 0) {
			rs = osetstr_add(vsp,fp,fl) ;
		    }
		    if (rs != INT_MAX) c += 1 ;
		} /* end if (got one) */
		if (fsb.term == '#') break ;
		if (rs < 0) break ;
	    } /* end while (fields) */
	    field_finish(&fsb) ;
	} /* end if (fields) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (osetstr_loadline) */


