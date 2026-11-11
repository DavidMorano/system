/* stringto HEADER (String-To) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* convert a C++ con std::string of digits to an integer (of various sizes) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */

/*******************************************************************************

  	Description:
	These subroutines convert a C++ con std::string of digits to an
	integer (of various sizes).

*******************************************************************************/

#ifndef	STRINGTO_INCLUDE
#define	STRINGTO_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<string>		/* C++STD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


extern int stringtosi	(con std::string &,int,sint *)			noex ;
extern int stringtosl	(con std::string &,int,slong *)			noex ;
extern int stringtosll	(con std::string &,int,slonglong *)		noex ;

extern int stringtoui	(con std::string &,int,uint *)			noex ;
extern int stringtoul	(con std::string &,int,ulong *)			noex ;
extern int stringtoull	(con std::string &,int,ulonglong *)		noex ;

inline int stringto	(con std::string &s,int b,sint *rp)		noex {
    	return stringtosi(s,b,rp) ;
} /* end */
inline int stringto	(con std::string &s,int b,slong *rp)		noex {
    	return stringtosl(s,b,rp) ;
} /* end */
inline int stringto	(con std::string &s,int b,slonglong *rp)	noex {
    	return stringtosll(s,b,rp) ;
} /* end */

inline int stringto	(con std::string &s,int b,uint *rp)		noex {
    	return stringtoui(s,b,rp) ;
} /* end */
inline int stringto	(con std::string &s,int b,ulong *rp)		noex {
    	return stringtoul(s,b,rp) ;
} /* end */
inline int stringto	(con std::string &s,int b,ulonglong *rp)	noex {
    	return stringtoull(s,b,rp) ;
} /* end */


#endif /* __cplusplus */
#endif /* STRINGTO_INCLUDE */


