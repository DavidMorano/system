/* ucopenusvc SUPPORT (open-facility-service) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* open a facility-service */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	uc_openusvc

	Description:
	This subroutine opens what is referred to as a "facility-service."
	Facility services are services provided by "facilities."
	Facilities are normally or usually really a software
	distribution that has its own source-root or program-root.
	Examples of software distributions within AT&T are:

	NCMP
	PCS (Personal Communication Services)
	EXPTOOLS (Experimental Tools)
	LOCAL
	REXEC (Remote Execution)
	POST
	EXTRA
	AST (Advanced Software Technologies)
	GNU

	Facilities are stored in the filesystem hierarchy rooted
	at a certain place.  Facilities are usually rooted at one
	of the following locations in the system filesystem hierarchy:

	/usr
	/usr/add-on
	/opt

	Facility services are represented in the filesystem as files
	with names of the form:

	<user>~<svc>[Â­<arg(s)>

	These sorts of file names are often actually stored in the
	filesystem as symbolic links.

	Synopsis:
	int uc_openusvc(pr,prn,svc,of,om,argv,envv,to)
	cchar	pr[] ;
	cchar	prn[] ;
	cchar	svc[] ;
	int		of ;
	mode_t		om ;
	cchar	**argv[] ;
	cchar	**envv[] ;
	int		to ;

	Arguments:
	pr		program-root
	prn		facility name
	svc		service name
	of		open-flags
	om		open-mode
	argv		array of arguments
	envv		attay of environment
	to		time-out

	Returns:
	>=0		file-descriptor
	<0		error (system-return)

	Notes:
	Facility services are implemented with loadable shared-object
	files.  Each service has a file of the same name as the
	service name itself.  The file is a shared-object with a
	global symbol of a callable subroutine with the name
	'opensvc_<svc>' where <svc> is the service name.  The
	subroutine looks like:

	int opensvc_<svc>(pr,prn,of,om,argv,envv,to)
	cchar	*pr ;
	cchar	*prn ;
	int		of ;
	mode_t		om ;
	cchar	*argv[] ;
	cchar	*envv[] ;

	Multiple services can be actually implemented in the same
	shared-object.  But the actual file of that object should
	be linked to other files, each with the filename of a service
	to be implemented.  These links are required because this
	code only searches for services by searching for files with
	the names of the services.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<localmisc.h>

import openxsvc ;

/* local defines */


/* external subroutines */

extern int uc_openxsvc(openxsvc *) noex ;


/* exported variables */


/* exported subroutines */

int uc_openusvc(cc *pr,cc *prn,cc *svc,int of,mode_t om,
		mainv argv,mainv envv,int to) noex {
    	openxsvc	os(pr,prn,svc,of,om,argv,envv,to) ;
	os.prefix = "opensvc" ;
	return uc_openxsvc(&os) ;
}
/* end subroutine (uc_openusvc) */


