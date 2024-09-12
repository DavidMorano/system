/* strip SUPPORT */
/* lang=C++20 */

/* strip page control */
/* version %I% last-modified %G% */


/* Copyright © 2017 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */

/*******************************************************************************

	Strip the form feeds out of the file.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdio>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<bfile.h>
#include	<localmisc.h>

#include	"rel.h"


/* local defines */

#define		BUFL	2048


/* exported variables */


/* exported subroutines */

int main(int argc,mainv argv,mainv) {
	bfile		input, *ifp, output, *ofp ;

	int	c, i, n ;

	int		offset, page, line, rs ;



	ifp = &input ;
	ofp = &output ;


	switch (argc) {

case 1:
	bopen(ifp,BFILE_IN,"r",0666) ;

	bopen(ofp,BFILE_OUT,"w",0666) ;

	break ;

case 2:
	rs = bopen(ifp,argv[1],"r",0666) ;
	if (rs < 0) return (rs) ;

	rs = bopen(ofp,BFILE_OUT,"w",0666) ;
	if (rs < 0) return rs ;

	break ;

case 3:
default:
	rs = bopen(ifp,argv[1],"r",0666) ;
	if (rs < 0) return (rs) ;

	rs = bopen(ofp,argv[2],"w",0666) ;
	if (rs < 0) return rs ;

	break ;

	} ; /* end switch */



	while ((c = bgetc(ifp)) != BFILE_EOF) {

		if (c == '\014') {

			bputc(ofp,'\n') ;

		} else bputc(ofp,c) ;

	} ;


	bflush(ofp) ;

	bclose(ofp) ;

}


