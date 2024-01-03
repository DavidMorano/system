/* vecstr_loadfile SUPPORT */
/* lang=C++20 */

/* load strings from a file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	vecstr_loadfile

	Description:
	This subroutine will read (process) a file and put all of
	the strings found into the string (supplied) list, consisting
	of a vecstr object.

	Synopsis:
	int vecstr_loadfile(vecstr *vsp,int fu,cchar *fname) noex

	Arguments:
	vsp		pointer to vecstr object
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

	Why are we using FIELD as opposed to 'nextfield()' or
	something similar?  Because our sematics are to process
	quoted strings as a single vecstr entry!

	Note_on_uniqueness:

	The 'fu' argument:
		0=no_unique
		1=unique

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>		/* <- for |UCHAR_MAX| + |CHAR_BIT| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<bufsizevar.hh>
#include	<vecstr.h>
#include	<filebuf.h>
#include	<field.h>
#include	<localmisc.h>


/* local defines */

#ifndef	FD_STDIN
#define	FD_STDIN	0
#endif

#define	DEFBUFLEN	1024

#define	TO_READ		-1		/* read timeout */


/* external subroutines */

extern "C" {
    extern int vecstr_loadfile(vecstr *,int,cchar *) noex ;
}


/* external variables */


/* forward references */

static int	mkterms() noex ;

static int	vecstr_loadfd(vecstr *,int,int) noex ;
static int	vecstr_loadline(vecstr *,int,cchar *,int) noex ;


/* local structures */


/* local variables */

constexpr int		termsize = ((UCHAR_MAX+1)/CHAR_BIT) ;

static bufsizevar	maxlinelen(getbufsize_ml) ;

static char		fterms[termsize] ;


/* exported subroutines */

int vecstr_loadfile(vecstr *vsp,int fu,cchar *fname) noex {
	static cint	srs = mkterms() ;
	int		rs ;
	int		c = 0 ;
	if ((rs = srs) >= 0) {
	    rs = SR_FAULT ;
	    if (vsp && fname) {
	        rs = SR_INVALID ;
	        if (fname[0]) {
		    int		fd = FD_STDIN ;
		    bool	f_opened = false ;
		    rs = SR_OK ;
	            if (strcmp(fname,"-") != 0) {
	                if ((rs = uc_open(fname,O_RDONLY,0666)) >= 0) {
	                    fd = rs ;
	                    f_opened = true ;
	                }
	            }
	            if (rs >= 0) {
	                rs = vecstr_loadfd(vsp,fu,fd) ;
	                c = rs ;
	            }
	            if (f_opened && (fd >= 0)) {
	                u_close(fd) ;
	            }
	        } /* end if (valid) */
	    } /* end if (non-null) */
	} /* end if (mkterms) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecstr_loadfile) */


/* local subroutines */

static int vecstr_loadfd(vecstr *vsp,int fu,int fd) noex {
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
			    rl_f	rl = filebuf_readln ;
	                    while ((rs = rl(lfp,lbuf,llen,to)) > 0) {
	                        int	len = rs ;
	                        if (lbuf[len - 1] == '\n') len -= 1 ;
			        if ((len > 0) && (lbuf[0] != '#')) {
			            rs = vecstr_loadline(vsp,fu,lbuf,len) ;
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
/* end subroutine (vecstr_loadfd) */

static int vecstr_loadline(vecstr *vsp,int fu,cchar *lbuf,int len) noex {
	FIELD		fsb ;
	int		rs ;
	int		c = 0 ;
	if ((rs = field_start(&fsb,lbuf,len)) >= 0) {
	    int		fl ;
	    cchar	*fp ;
	    while ((fl = field_get(&fsb,fterms,&fp)) >= 0) {
		if (fl > 0) {
		    if (fu) {
			rs = vecstr_adduniq(vsp,fp,fl) ;
			if (rs < INT_MAX) c += 1 ;
		    } else {
			c += 1 ;
			rs = vecstr_add(vsp,fp,fl) ;
		    }
		} /* end if (got one) */
		if (fsb.term == '#') break ;
		if (rs < 0) break ;
	    } /* end while (fields) */
	    field_finish(&fsb) ;
	} /* end if (fields) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecstr_loadline) */

static int mkterms() noex {
	return fieldterms(fterms,false,'\n','#') ;
}
/* end subroutine (mkterms) */


