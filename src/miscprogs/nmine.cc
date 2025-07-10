/* miscprogs SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* program to change the owner of a file */
/* version %I% last-modified %G% */

/* revision-history:

	= 1988-07-01, David A-D= Morano
	This was originally written.

*/

/* Copyright © 1988 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	main

	Description:
	This program will change the owner of a file to the current
	UID.  The current user needs write permission to the directory
	in order for the effect to take place.

*******************************************************************************/

#include	<envstandards.h>	/* must be ordered first to configure */
#include	<sys/stat.h>
#include	<fcntl.h>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstdio>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<bfile.h>
#include	<localmisc.h>


/* local defines */

#define		LINESIZE	4000


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int main(int argc,mainv argv,mainv) {
	bfile	in, out, err ;

	int	i, rs, len ;

	char	buf[LINESIZE] ;


	rs = bopen(&err,BERR,"w",0666) ;
	if (rs < 0) return (rs) ;


	switch (argc) {

case 1:
	rs = bopen(&in,BIN,"r",0666) ;
	if (rs < 0) return (rs) ;

	rs = bopen(&out,BOUT,"w",0666) ;
	if (rs < 0) return (rs) ;

	break ;

case 2:
	rs = bopen(&in,argv[1],"r",0666) ;
	if (rs < 0) return (rs) ;

	rs = bopen(&out,BOUT,"w",0666) ;
	if (rs < 0) return (rs) ;

	break ;

case 3:
default:
	rs = bopen(&in,argv[1],"r",0666) ;
	if (rs < 0) return (rs) ;

	rs = bopen(&out,argv[2],"wc",0666) ;
	if (rs < 0) return (rs) ;

	break ;

	} ; /* end switch */



/* copy to the temporary file */

	while (1) {

		len = bread(&in,buf,LINESIZE) ;
		if (len <= 0) break ;

		rs = bwrite(&out,buf,len) ;

		if (rs < 0) {

			bprintf(efp,"bad return after write to out file %d\n",
				rs) ; 

			bclose(efp) ;

			return BAD ;
		}
	}


	bclose(&in) ;

	bclose(&out) ;

	bclose(&err) ;

	return OK ;
}


