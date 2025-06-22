/* checkdata SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* program to compress 'checkfile' data base */
/* version %I% last-modified %G% */

#define	CF_PRINT		0

/* revision-history:

	= 1988-10-01, David A-D- Morano
	This code was original written.

*/

/* Copyright © 1988 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */

/************************************************************************

  	Name:
	main

	Description:

	Arguments:
	- file name of old database
	- file name of new database

************************************************************************/

#include	<envstandards.h>	/* must be ordered fist to configure */
#include	<sys/stat.h>
#include	<fcntl.h>
#include	<pwd.h>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<bfile.h>
#include	<localmisc.h>


/* local deifines */

#define		MAXNAMELEN	256

#define		NROW		32
#define		NCOL		32
#define		HOLDSIZE	(NROW * NCOL * 4)


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct sumentry {
	char	name[MAXNAMELEN + 1] ;
	int	namelen ;
	long	sumrow, sumcol ;
	long	lasttime ;
} ;


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int main(int argc,mainv argv,mainv) {
	bfile	errfile, *efp = &errfile ;
	bfile	infile, *ifp = &infile ;
	bfile	sumfile, *sfp = &sumfile ;
	bfile	tempfile, *tfp = &tempfile ;
	sumentry	entry ;
	USTAT		ss, *sp = &ss ;
	ucpwent		*pwsp ;
	int	i, j, rs, len ;
	int	namelen ;

	char	*cp, namebuf[MAXNAMELEN + 1], name[MAXNAMELEN + 1] ;
	char	*timebuf, dbuf[100], buf[100] ;


	rs = bopen(efp,BERR,"w",0666) ;

	if (rs < 0) return (rs) ;


	switch (argc) {

	case 1:
	    bprintf(efp,"no file given as argument\n") ;

	    bclose(efp) ;

	    return BAD ;

	case 2:
	    rs = bopen(ifp,argv[1],"r",0666) ;
	    if (rs < 0) goto nobase ;

	    rs = bopen(sfp,BOUT,"w",0666) ;
	    if (rs < 0) goto badout ;

	    break ;

	case 3:
	    rs = bopen(ifp,argv[1],"r",0666) ;
	    if (rs < 0) goto nobase ;

	    rs = bopen(sfp,argv[2],"w",0666) ;
	    if (rs < 0) goto badout ;

	    break ;

	default:
	    bprintf(efp,"can't handle this yet\n") ;

	    bclose(efp) ;

	    return BAD ;

	} /* end switch */


/* read in database entries and valid each one */

	while ((len = bread(ifp,&entry,sizeof(struct sumentry))) > 0) {

	    if (len != sizeof(struct sumentry))
	        goto bad ;

	    if (entry.namelen == 0)
	        continue ;

	    entry.name[entry.namelen] = 0 ;

	    if (stat(entry.name,sp) < 0)
	        continue ;

	    bwrite(sfp,&entry,sizeof(struct sumentry)) ;

	} /* end while */


/* finished */
exit:
	bclose(sfp) ;

exit1:
	bclose(ifp) ;

exit2:
	bclose(efp) ;

	return OK ;

bad:
	bprintf(efp,"file does not have correct length\n") ;

	goto exit2 ;

nobase:
	bprintf(efp,"could not open the database file\n") ;

	goto exit2 ;

badout:
	bprintf(efp,"bad output file\n") ;

	goto	exit1 ;
}
/* end subroutine (main) */



