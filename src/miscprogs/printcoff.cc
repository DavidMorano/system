/* printcoff SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* print out standard and option headers on COFFs */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2017 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */

/*******************************************************************************

	Name:
	printcoff

	Description:
	Print out some information in a COFF header.  Print out the
	file header and the optional UNIX header.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstdio>
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */


/* local defines */

#define		EOL	'\n'		/* end of line mark */
#define		BUFL	2048


/* external subroutines */


/* external variables */


/* local structures */

	struct fh {
		unsigned short	f_magic ;
		unsigned short	f_nscns ;
		long int	f_timdat ;
		long int	f_symptr ;
		long int	f_nsyms ;
		unsigned short	f_opthdr ;
		unsigned short	f_flags ;
	} ;


	struct uh {
		short		magic ;
		short		vstamp ;
		long		tsize ;
		long		dsize ;
		long		bsize ;
		long		entry ;
		long		text_start ;
		long		data_start ;
	} ;


/* exported variables */


/* exported subroutines */

int main(int,con mainv,con mainv) {
	struct fh	st_fh ;
	struct uh	st_uh ;
	int		ex = EX_SUCCESS ;
	int		rs = SR_OK ;
	int		c, i, n ;
	int		ifd = FD_STDIN ;
	int		len ;
	int		count ;
	short		buf[BUFL] ;
	char		*bp, obuf[82] ;
	/* start by reading in the file header */

	len = read(ifd,&st_fh,sizeof(struct fh)) ;
	if (len <= 0) goto reteof ;

	printf("f_magic		%04X\n",st_fh.f_magic) ;
	printf("f_nscns		%04X\n",st_fh.f_nscns) ;
	printf("f_timdat	%08X\n",st_fh.f_timdat) ;
	printf("f_symptr	%08X\n",st_fh.f_symptr) ;
	printf("f_nsyms		%08X\n",st_fh.f_nsyms) ;
	printf("f_opthdr	%04X\n",st_fh.f_opthdr) ;
	printf("f_flags		%04X\n",st_fh.f_flags) ;



/* now read in the optional standard UNIX header if present */

	if (st_fh.f_opthdr) {

		if (st_fh.f_opthdr == sizeof(struct uh)) {

			len = read(ifd,&st_uh,sizeof(struct uh)) ;
			if (len <= 0) goto reteof ;

	printf("magic	%04X\n",st_uh.magic) ;
	printf("vstamp	%04X\n",st_uh.vstamp) ;
	printf("tsize	%08X\n",st_uh.tsize) ;
	printf("dsize	%08X\n",st_uh.dsize) ;
	printf("bsize	%08X\n",st_uh.bsize) ;
	printf("entry	%08X\n",st_uh.entry) ;
	printf("tstart	%08X\n",st_uh.text_start) ;
	printf("dstart	%08X\n",st_uh.data_start) ;


		} else {

			debugprint("optioonal header is not standard UNIX\n") ;

			lseek(0,(int) st_fh.f_opthdr,1) ;

		}


	} /* end if */


reteof:
	if ((ex == EXIT_SUCCESS) && (rs < 0)) {
	    ex = EXIT_FAILURE ;
	}
	return ex ;
} /* end subroutine (main) */


