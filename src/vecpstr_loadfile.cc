/* vecpstr_loadfile */
/* lang=C++20 */

/* load strings from a file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A�D� Morano
	This module was originally written.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	vecpstr_loadfile

	Description:
	This subroutine will read (process) a file and put all of
	the strings found into the string (supplied) list, consisting
	of a vecpstr object.

	Synopsis:
	int vecpstr_loadfile(vecpstr *vsp,int fu,cchar *fname) noex

	Arguments:
	vsp		pointer to vecpstr object
	fu		flag specifying uniqueness
	fname		file to load

	Returns:
	>=0		number of elements loaded
	<0		error

	Notes:
	Why use filebuf over BFILE? Yes, filebuf is a tiny bit more
	lightweight than BFILE -- on a good day. But the real reason
	may be so that we don't need to load BFILE in code that
	resides very deep in a software stack if we don't need it
	-- like deep inside loadable modules. Anyway, just a thought!

        Why are we using FIELD as opposed to 'nextfield()' or something similar?
        Because our sematics are to process quoted strings as a single vecpstr
        entry!

	Note_on_uniqueness:

	The 'fu' argument:
		0=no_unique
		1=unique

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<limits.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<vecpstr.h>
#include	<filebuf.h>
#include	<field.h>
#include	<bufsizevar.hh>
#include	<localmisc.h>


/* local defines */

#ifndef	FD_STDIN
#define	FD_STDIN	0
#endif

#define	DEFBUFLEN	1024

#define	TO_READ		-1		/* read timeout */


/* external subroutines */

extern "C" {
    extern int vecpstr_loadfile(vecpstr *,int,cchar *) noex ;
}


/* external variables */


/* forward references */

static int	vecpstr_loadfd(vecpstr *,int,int) noex ;
static int	vecpstr_loadline(vecpstr *,int,cchar *,int) noex ;


/* local structures */


/* local variables */

static bufsizevar	maxlinelen(getbufsize_ml) ;

static cchar		fterms[32] = {
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

int vecpstr_loadfile(vecpstr *vsp,int fu,cchar *fname) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (vsp && fname) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
		int	fd = FD_STDIN ;
		bool	f_opened = false ;
		rs = SR_OK ;
	        if (strcmp(fname,"-") != 0) {
	            if ((rs = uc_open(fname,O_RDONLY,0666)) >= 0) {
	                fd = rs ;
	                f_opened = true ;
	            }
	        }
	        if (rs >= 0) {
	            rs = vecpstr_loadfd(vsp,fu,fd) ;
	            c = rs ;
	        }
	        if (f_opened && (fd >= 0)) {
	            u_close(fd) ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecpstr_loadfile) */


/* local subroutines */

static int vecpstr_loadfd(vecpstr *vsp,int fu,int fd) noex {
	int		rs ;
	int		rs1 ;
	int		to = -1 ;
	int		c = 0 ;
	USTAT		sb ;
	if ((rs = u_fstat(fd,&sb)) >= 0) {
	    rs = SR_ISDIR ;
	    if (! S_ISDIR(sb.st_mode)) {
	        int	fbsize = DEFBUFLEN ;
	        int	fbo = 0 ;
	        if (S_ISREG(sb.st_mode)) {
	            int	fs = ((sb.st_size == 0) ? 1 : (sb.st_size & INT_MAX)) ;
	            int	cs ;
	            cs = BCEIL(fs,512) ;
	            fbsize = MIN(cs,1024) ;
	        } else {
	            to = TO_READ ;
	            if (S_ISSOCK(sb.st_mode)) fbo |= FILEBUF_ONET ;
	        }
		if ((rs = maxlinelen) >= 0) {
		    cint	llen = rs ;
	            char	*lbuf ;
		    if ((rs = uc_libmalloc((llen+1),&lbuf)) >= 0) {
			typedef int (*rl_f)(filebuf *,char *,int,int) ;
			filebuf		loadfile, *lfp = &loadfile ;
	        	if ((rs = filebuf_start(lfp,fd,0L,fbsize,fbo)) >= 0) {
			    rl_f	rl = filebuf_readline ;
	                    while ((rs = rl(lfp,lbuf,llen,to)) > 0) {
	                        int	len = rs ;
	                        if (lbuf[len - 1] == '\n') len -= 1 ;
			        if ((len > 0) && (lbuf[0] != '#')) {
			            rs = vecpstr_loadline(vsp,fu,lbuf,len) ;
			            c += rs ;
			        }
	                        if (rs < 0) break ;
	                    } /* end while (reading lines) */
	                    rs1 = filebuf_finish(lfp) ;
		            if (rs >= 0) rs = rs1 ;
	                } /* end if (filebuf) */
			rs1 = uc_libfree(lbuf) ;
			if (rs >= 0) rs = rs1 ;
		    } /* end if (m-a-f) */
		} /* end if (maxlinelen) */
	    } /* end if (not a directory) */
	} /* end if (stat) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecpstr_loadfd) */

static int vecpstr_loadline(vecpstr *vsp,int fu,cchar *lbuf,int len) noex {
	FIELD		fsb ;
	int		rs ;
	int		c = 0 ;
	if ((rs = field_start(&fsb,lbuf,len)) >= 0) {
	    int		fl ;
	    cchar	*fp ;
	    while ((fl = field_get(&fsb,fterms,&fp)) >= 0) {
		if (fl > 0) {
		    if (fu) {
			rs = vecpstr_adduniq(vsp,fp,fl) ;
			if (rs < INT_MAX) c += 1 ;
		    } else {
			c += 1 ;
			rs = vecpstr_add(vsp,fp,fl) ;
		    }
		} /* end if (got one) */
		if (fsb.term == '#') break ;
		if (rs < 0) break ;
	    } /* end while (fields) */
	    field_finish(&fsb) ;
	} /* end if (fields) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecpstr_loadline) */


