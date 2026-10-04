/* main SUPPORT (IPADDR) */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* create an IP address suitable for use by the TLI API */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time */
#define	CF_DEBUG	0		/* run-time */

/* revision history:

	= 1998-03-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Description:

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<pwd.h>
#include	<grp.h>
#include	<ctime>
#include	<csignal>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<uclibmem.h>
#include	<bfile.h>
#include	<baops.h>
#include	<cfdec.h>
#include	<sfx.h>			/* |sfbasename(3uc)| */
#include	<localmisc.h>

#include	"config.h"


/* local defines */

#define	NPARG		1

#define	LINELEN		200

#define	CLASS_A		0
#define	CLASS_B		1
#define	CLASS_C		2


/* external subroutines */

extern void	int_alarm() ;


/* external variables */


/* local structures */

struct parts {
	int	v ;
	uint	valid : 1 ;
} ;


/* local variables */


/* exported variables */


/* exported subroutines */

int main(int argc,mainv argv,mainv envv) {
	bfile		errfile, *efp = &errfile ;
	bfile		outfile, *ofp = &outfile ;
	bfile		infile, *ifp = &infile ;
	struct parts	p[4] ;
	long	a ;
	int	argl, aol ;
	int	pan, i, l ;
	int	rs ;
	int	ex = EXIT_SUCCESS ;
	int	f_usage = false ;
	int	f_ddash = false ;
	int	f_version = false ;
	int	len ;
	int	a_net, a_host ;
	int	nparts ;
	int	classnum ;

	char	*argp, *aop ;
	char	*progname ;
	char	linebuf[LINELEN + 1], *cp2 ;
	cchar	*cp ;

	if (int cl ; (cl = sfbasename(argv[0],-1,&cp)) >= 0) {
	    rs = lm_strw(cp,cl,&progname) ;
	}

	if (bopen(efp,BFILE_STDERR,"dwca",0666) >= 0)
		bcontrol(efp,BC_LINEBUF,0) ;


	if (bopen(ofp,BFILE_STDOUT,"wct",0666) < 0) 
		return BAD ;

	if (bopen(ifp,BFILE_STDIN,"r",0666) < 0) 
		return BAD ;


	if ((len = breadln(ifp,linebuf,LINELEN)) < 1) 
		goto badinopen ;

	if (linebuf[len - 1] == '\n') {
		len -= 1 ;
	}
	linebuf[len] = '\0' ;

/* parse into pieces */

	cp = linebuf ;
	while (ISWHITE(*cp)) {
		cp += 1 ;
	}

	for (i = 0 ; i < 4 ; i += 1) {
		p[i].valid = false ;
	}

	nparts = 0 ;
	for (i = 0 ; i < 3 ; i += 1) {
	    if ((cp2 = strchr(cp,'.')) == NULL) break ;
	    *cp2 = '\0' ;

#if	CF_DEBUG
	debugprintf("main: top inside %d=%s\n",i,cp) ;
#endif

	    if ((rs = cfdeci(cp,strlen(cp),&p[i].v)) < 0)
	        goto badin_a ;

#if	CF_DEBUG
	debugprintf("main: bottom inside %d=%d\n",i,p[i].v) ;
#endif

	    nparts += 1 ;
	    p[i].valid = true ;
	    cp = cp2 + 1 ;

	} /* end for */

#if	CF_DEBUG
	debugprintf("main: nparts=%d cp=%s\n",nparts,cp) ;
#endif

	if ((rs = cfdeci(cp,strlen(cp),&p[nparts].v)) < 0)
	    goto badin_b ;

	p[nparts].valid = true ;
	nparts += 1 ;

	switch (nparts) {

	case 1:
	    a_net = 0 ;
	    a_host = p[0].v ;
	    a = a_host ;
	    break ;

	case 2:
	    a_net = (p[0].v & 255) ;
	    a_host = (p[1].v & 0xFFFFFF) ;
	    a = (a_net << 24) + a_host ;
	    break ;

	case 3:
	    a_net = ((p[0].v & 255) << 8) + (p[1].v & 255) ;
	    a_host = (p[2].v & 0xFFFF) ;
	    a = (a_net << 16) + a_host ;
	    break ;

	case 4:
	    a_net = (p[0].v << 24) + (p[1].v << 16) + 
	        (p[2].v << 8) ;
	    a_host = p[3].v ;
	    a = (p[0].v << 24) + (p[1].v << 16) + 
	        (p[2].v << 8) + p[3].v ;
	    break ;

	default:
	    a = 0 ;
	    a_net = a_host = 0 ;

	} /* end switch */

	if (a >= (192 << 24)) {
	    classnum = CLASS_C ;
	} else if (a >= (135 << 24)) {
	    classnum = CLASS_B ;
	} else {
	    classnum = CLASS_A ;
	}

	p[0].v = (a >> 24) ;
	p[1].v = (a >> 16) & 255 ;
	p[2].v = (a >> 8) & 255 ;
	p[3].v = a & 255 ;

	switch (classnum) {
	case CLASS_A:
	    a_net = a >> 24 ;
	    a_host = a & 0xFFFFFF ;
	    break ;
	case CLASS_B:
	    a_net = a >> 16 ;
	    a_host = a & 0xFFFF ;
	    break ;
	case CLASS_C:
	    a_net = a >> 8 ;
	    a_host = a & 255 ;
	    break ;
	} /* end switch */

	bprintf(ofp,"dot=%d.%d.%d.%d hex=\\x%08X\n",
	    p[0].v,p[1].v,p[2].v,p[3].v,a) ;

	bprintf(ofp,"net=%d host=%d\n",
	    a_net,a_host) ;

ret0:
	if (ofp) {
	    bclose(ofp) ;
	    ofp = nullptr ;
	}
	if (efp) {
	    bclose(efp) ;
	    efp = nullptr ;
	}
	return ex ;

badinopen:
	bprintf(efp,"%s: bad input\n",progname) ;
	goto badret ;

badin_a:
	bprintf(efp,"%s: bad input A\n",progname) ;
	goto badret ;

badin_b:
	bprintf(efp,"%s: bad input B\n",progname) ;

badret:
	bclose(efp) ;
	ex = EXIT_FAILURE ;
	goto ret0 ;
}
/* end subroutine (main) */



