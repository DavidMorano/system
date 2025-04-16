/* bufsizedata MODULE (primary module interface) */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* buffer-size-data (default value database) */
/* version %I% last-modified %G% */


/* revision history:

	= 2014-06-17, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2014 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	bufsizedata

	Description:
	This module provides the database from which decisions are
	made for determing buffer sizes.  A decision about how to
	determine any particular buffer size is make at the point
	when a particular buffer size is required.  This data base
	takes the most simplest of forms: an array of structures
	(|bufsuzeitem|).  Oh, I almost forgot.  I am using the new
	|constexpr| features of C++14.  I waited for these C++14
	enhancements to |constexpr| for these many years.  I knew
	that the imbecilic implemention of |constexpr| in C+11
	needed to be enhanced (and eventually so it was).  Merry
	programming!

*******************************************************************************/

module ;

#include	<envstandards.h>	/* ordered first to configure */
#include	<unistd.h>		/* |_SC_xx| */
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<bufsizenames.h>


#ifndef	_SC_ARG_MAX
#define	_SC_ARG_MAX		-1
#endif
#ifndef	_SC_NAME_MAX
#define	_SC_NAME_MAX		-1
#endif
#ifndef	_SC_PATH_MAX
#define	_SC_PATH_MAX		-1
#endif
#ifndef	_SC_LINE_MAX
#define	_SC_LINE_MAX		-1
#endif
#ifndef	_SC_HOST_NAME_MAX		/* really the "nodename" length */
#define	_SC_HOST_NAME_MAX	-1
#endif
#ifndef	_SC_LOGIN_NAME_MAX
#define	_SC_LOGIN_NAME_MAX	-1
#endif
#ifndef	_SC_GETPW_R_SIZE_MAX
#define	_SC_GETPW_R_SIZE_MAX	-1
#endif
#ifndef	_SC_GETGR_R_SIZE_MAX
#define	_SC_GETGR_R_SIZE_MAX	-1
#endif
#ifndef	_SC_FSTYPE
#define	_SC_FSTYPE		-1
#endif
#ifndef	_SC_TZNAME_MAX
#define	_SC_TZNAME_MAX		-1
#endif
#ifndef	_SC_ZONEINFO_MAX
#define	_SC_ZONEINFO_MAX	-1
#endif

export module bufsizedata ;

export {
    struct bufsizedata_item {
	int	name ;
	int	defval ;
    } ;
    struct bufsizedata {
	bufsizedata_item	item[getbufsize_overlast] = {} ;
	constexpr bufsizedata() noex ;
	constexpr bufsizedata_item operator [] (int w) const noex {
	        bufsizedata_item	it = {-1,0} ;
	        if ((w >= 0) && (w < getbufsize_overlast)) {
	            it = item[w] ;
	        }
	        return it ;
	} ; /* end method (operator) */
    } ; /* end class (bufsizedata) */
    constexpr bufsizedata::bufsizedata() noex {
	for (int i = 0 ; i < getbufsize_overlast ; i += 1) {
	    bufsizedata_item	*ip = (item+i) ;
	    ip->name = -1 ;
	    switch (i) {
	    case getbufsize_ps:
	        ip->name = _SC_PAGESIZE ;
		ip->defval = (8 * 1024) ;	/* page-size Solaris® */
	        break ;
	    case getbufsize_ma:
	        ip->name = _SC_ARG_MAX ;
		ip->defval = ARBUFLEN ;
	        break ;
	    case getbufsize_mn:		/* max-name */
	        ip->name = _SC_NAME_MAX ;
		ip->defval = MNBUFLEN ;
	        break ;
	    case getbufsize_mp:		/* max-path */
	        ip->name = _SC_PATH_MAX ;
	        ip->defval = MPBUFLEN ;
	        break ;
	    case getbufsize_ml:		/* max-line */
	        ip->name = _SC_LINE_MAX ;
		ip->defval = MLBUFLEN ;
	        break ;
	    case getbufsize_nn:		/* node-name */
	        ip->name = _SC_HOST_NAME_MAX ; /* <- really is "nodename" */
		ip->defval = NNBUFLEN ;
	        break ;
	    case getbufsize_hn:		/* host-name */
	        ip->defval = HNBUFLEN ;
	        break ;
	    case getbufsize_un:		/* user-name */
		ip->name = _SC_LOGIN_NAME_MAX ;
	        ip->defval = UNBUFLEN ;
	        break ;
	    case getbufsize_gn:		/* group-name */
	        ip->defval = GNBUFLEN ;
	        break ;
	    case getbufsize_pn:		/* project-name */
	        ip->defval = PNBUFLEN ;
	        break ;
	    case getbufsize_pw:
	        ip->name = _SC_GETPW_R_SIZE_MAX ;
		ip->defval = PWBUFLEN ;
	        break ;
	    case getbufsize_sp:
	        ip->defval = SPBUFLEN ;
	        break ;
	    case getbufsize_ua:
	        ip->defval = UABUFLEN ;
	        break ;
	    case getbufsize_gr:
		ip->name = _SC_GETGR_R_SIZE_MAX ;
		ip->defval = GRBUFLEN ;
	        break ;
	    case getbufsize_pj:
		ip->defval = PJBUFLEN ;		/* project-buf */
	        break ;
	    case getbufsize_pr:
		ip->defval = PRBUFLEN ;		/* protocol-buf */
	        break ;
	    case getbufsize_nw:
		ip->defval = NWBUFLEN ;		/* network-buf */
	        break ;
	    case getbufsize_ho:
		ip->defval = HOBUFLEN ;		/* host-buf */
	        break ;
	    case getbufsize_sv:
		ip->defval = SVBUFLEN ;		/* service-buf */
	        break ;
	    case getbufsize_fs:
	        ip->name = _SC_FSTYPE ;
		ip->defval = FSBUFLEN ;		/* filesystem-name-buf */
	        break ;
	    case getbufsize_zn:
	        ip->name = _SC_TZNAME_MAX ;
		ip->defval = ZNBUFLEN ;
	        break ;
	    case getbufsize_zi:
	        ip->name = _SC_ZONEINFO_MAX ;
		ip->defval = ZIBUFLEN ;		/* common value */
	        break ;
	    case getbufsize_mailaddr:
		ip->defval = -1 ;		/* no default size */
	        break ;
	    } /* end switch */
	} /* end for */
    } /* end method (bufsizedata:bufsizedata) */
} /* end export */


