/* cci_main SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* program to correct indentations */
/* version %I% last-modified %G% */


/* revision history:

	= 1985-02-12, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1985 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

  	Description:
	This subroutine is a little program that is used to follow
	the processing of a C language source file by the CB program.

******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<fcntl.h>
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<prognamevar.hh>	/* LIBUC */
#include	<sfx.h>			/* LIBUC */
#include	<bfile.h>		/* LIBUC */
#include	<ascii.h>		/* LIBU */
#include	<exitcodes.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<libb.h>		/* LIBB */

#include	"cci_config.h"
#include	"defs.h"


/* local defines */

#define	PI		proginfo


/* external subroutines */


/* forward references */

local int	procfile(PI *,bfile *,cchar *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int main(int argc,con mainv argv,con mainv envv) {
	PI		pi, *pip = &pi ;
	prognamevar	pn(argc,argv,envv) ;
	bfile		errfile ;
	bfile		outfile, *ofp = &outfile ;
	int		rs = SR_OK ;
	int		ex = EX_INFO ;

	memclear(pip) ;
	pip->progname = ccp(pn) ;
	if ((rs = bopen(&errfile,BFILE_STDERR,"dwca",0666)) >= 0) {
	    pip->efp = &errfile ;
	    bcontrol(&errfile,BC_LINEBUF,0) ;
	}

	/* open standard-output */
	if ((rs = bopen(ofp,BFILE_STDOUT,"dwct",0666)) >= 0) {
	    /* loop processing files */
	    for (int i = 1 ; argv[i] ; i += 1) {
	        cchar *cp = argv[i] ;
	        if ((cp[0] != '\0') && (strcmp(cp,"--") != 0)) {
	            rs = procfile(pip,ofp,cp) ;
	        }
	        if (rs < 0) break ;
	    } /* end for */
	    rs1 = bclose(ofp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (bfile-output) */

	bclose(pip->efp) ;
	ex = (rs >= 0) ? EX_OK : EX_DATAERR ;
	return ex ;
} /* end subroutine (main) */


/* local subroutines */

local int procfile(PI *pip,bfile *ofp,cchar *fname) noex {
	bfile	infile, *ifp = &infile ;

#ifdef	INSIDE
	int	inside = 0 ;
#endif

	int	rs ;
	int	i, j ;
	int	nblank = 0 ;
	int	llen, outlen ;
	int	c ;
	int	c_white = 0 ;
	int	f_comment = false ;
	int	f_comstart = false ;
	int	f_comend = false ;
	int	f_braceend = false ;
	int	f_leading = false ;
	char	linebuf[LINEBUFLEN + 1] ;
	char	outbuf[LINEBUFLEN * 4] ;

	if (fname == nullptr)
	    return SR_FAULT ;

	if ((fname[0] != '-') && (fname[0] != '\0')) {
	    rs = bopen(ifp,fname,"r",0666) ;
	} else {
	    rs = bopen(ifp,BFILE_STDIN,"dr",0666) ;
	}
	if (rs < 0)
	    goto badinopen ;

/* go through the loops */

	while ((rs = breadln(ifp,linebuf,LINEBUFLEN)) > 0) {

	    llen = rs ;
	    outlen = 0 ;
	    for (i = 0 ; i < llen ; i += 1) {

	        c = linebuf[i] ;
	        switch (c) {

/* handle a possible beginning of a comment */
	        case '/':
	            outbuf[outlen++] = c ;
	            f_braceend = false ;

	            if (f_comment) {

	                if (f_comend) {

	                    f_comend = false ;
	                    f_comment = false ;

	                }

	                f_leading = false ;

	            } else {

	                f_comstart = true ;

	            }

	            break ;

	        case '*':
	            f_comend = false ;
	            f_braceend = false ;

	            outbuf[outlen++] = c ;

	            if (! f_comment) {

	                if (f_leading && f_comstart) {

	                    outbuf[0] = '/' ;
	                    outbuf[1] = c ;
	                    outlen = 2 ;

	                }

	            } else
	                f_comend = true ;

	            if (f_comstart)
	                f_comment = true ;

	            f_leading = false ;
	            break ;

	        case '\n':
	            if (i == 0)
	                nblank += 1 ;

	            else if (nblank > 0)
	                nblank -= 1 ;

	            f_leading = false ;
	            f_braceend = false ;
	            f_comstart = false ;
	            f_comend = false ;
	            outbuf[outlen++] = c ;
	            break ;

	        case '\t':
	            f_comstart = false ;
	            f_comend = false ;
	            f_braceend = false ;

	            if (f_comment) {

	                outbuf[outlen++] = c ;

	            } else {

	                if (i == 0) {

	                    outbuf[outlen++] = c ;
	                    f_leading = true ;

	                } else if (f_leading) {

	                    for (j = 0 ; j < 4 ; j += 1)
	                        outbuf[outlen++] = ' ' ;

	                } else 
	                    outbuf[outlen++] = c ;

	            }

	            break ;

	        case CH_LPAREN:
	            f_leading = false ;
	            f_braceend = false ;
	            f_comend = false ;

	            outbuf[outlen++] = c ;

	            if (f_comment)
	                break ;

	            f_comstart = false ;
#ifdef	INSIDE
	            inside += 1 ;
#endif
	            break ;

	        case CH_RPAREN:
	            f_leading = false ;

	            outbuf[outlen++] = c ;

	            if (f_comment)
	                break ;

	            f_comend = false ;
	            f_comstart = false ;
	            f_braceend = true ;
#ifdef	INSIDE
	            inside -= 1 ;
#endif

	            break ;

	        case ';':
	            f_leading = false ;
	            f_comend = false ;
	            f_comstart = false ;

	            if (f_comment) {

	                outbuf[outlen++] = c ;

	            } else {

	                if (f_braceend) {

	                    if (((llen - i) > 0) && (linebuf[i + 1] == ' ')) {

	                        i += 1 ;
	                    }

	                    outbuf[outlen++] = ' ' ;
	                    outbuf[outlen++] = c ;
	                    f_braceend = false ;

	                } else if ((! c_white) && (linebuf[i + 1] == '\n')) {

	                    outbuf[outlen++] = ' ' ;
	                    outbuf[outlen++] = c ;

	                } else
	                    outbuf[outlen++] = c ;

	            }

	            break ;

	        case ' ':
	            c_white = 2 ;

/* fall through to case below */

	        default:
	            outbuf[outlen++] = c ;
	            f_braceend = false ;
	            f_leading = false ;
	            f_comend = false ;
	            f_comstart = false ;
	            break ;

	        } /* end switch */

	        if (c_white)
	            c_white -= 1 ;

	    } /* end for */

	    if (rs >= 0)
	    rs = bwrite(ofp,outbuf,outlen) ;

	    if (rs < 0) break ;
	} /* end while (reading lines) */

	if ((rs >= 0) && (nblank < 2)) {
	    for (i = 0 ; i < (2 - nblank) ; i += 1)
	        bputc(ofp,'\n') ;
	}

	bclose(ofp) ;

badinopen:
	return rs ;
}
/* end subroutine (procfile) */


