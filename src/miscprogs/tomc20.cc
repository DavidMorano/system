/* to_mc20 SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* program to convert magic number to that of the MC20 */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright 1998 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */

/*******************************************************************************


	This program converts the input COFF to an output
	COFF with the MC20 magic number.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<fcntl.h>
#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<localmisc.h>

#include	"rel.h"


/* local defines */

#define		MAGIC_MC20	0x0150

#define		EOL	'\n'		/* end of line mark */

#define		BUFL	2048


/* local namespaces */


/* local typedefs */


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
		short		v_stamp ;
		long		tsize ;
		long		d_size ;
		long		b_size ;
		long		entry ;
		long		text_start ;
		long		data_start ;
	} ;


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int main(int argc,mainv argv,mainv envv) {
	struct fh	st_fh ;
	struct uh	st_uh ;
	int	c, i, n ;

	int		ifd, ofd ;
	int		len, count ;


	short		buf[BUFL] ;

	char		*bp, obuf[82] ;



	switch (argc) {

case 1:
	ifd = 0 ; ofd = 1 ;

	break ;

case 2:
	ifd = open(argv[1],O_RDONLY,0x0FFF) ;
	if (ifd < 0) return(ifd) ;

	ofd = 1 ;

	break ;

case 3:
default:
	ifd = open(argv[1],O_RDONLY,0x0FFF) ;
	if (ifd < 0) return(ifd) ;

	ofd = open(argv[2],O_WRONLY | O_CREAT | O_TRUNC,0x0FFF) ;
	if (ofd < 0) return(ofd) ;

	break ;

	} ; /* end switch */


/* start by reading in the file header */

	len = read(ifd,&st_fh,sizeof(struct fh)) ;
	if (len <= 0) goto eof ;


	st_fh.f_magic = MAGIC_MC20 ;

	write(ofd,&st_fh,sizeof(struct fh)) ;


loop:
	len = read(ifd,buf,BUFL) ;
	if (len <= 0) goto eof ;

	write(ofd,buf,len) ;

	goto loop ;

eof:
	return (0) ;
}
/* end subroutine (main) */


