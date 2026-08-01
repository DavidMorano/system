/* bufsizes SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++23 (MUST-BE C-language; conformance reviewed to C23) */

/* enumerate the various system buffer-size names */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	bufsizs

	Description:
	This file contains the enumerations for the various system
	buffer-size names.  This code module MUST-BE written in the
	C-language.  The reason is because C++ does NOT (officially)
	support the language feature (according to its ISO standard)
	used below.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */

#include	"bufsizes.h"


/* local structures */

namespace {
    struct bufsizer {
	cchar	*names[bufsize_overlast + 1] ;
	consteval void mknames() noex ;
	consteval bufsizer() noex {
	    mknames() ;
	} ;
    } ; /* end struct (bufsizer) */
    struct initer {
	initer() noex ;
    } ; /* end struct (initer) */
} /* end namespace */

consteval void bufsizer::mknames() noex {
    	cchar **n = names ;
	n[bufsize_ps]		= "PS" ;	/* page-size */
	n[bufsize_pl]		= "PL" ;	/* pid-length (digit string) */
	n[bufsize_ma]		= "MA" ;	/* max-args */
	n[bufsize_ml]		= "ML" ;	/* max-line */
	n[bufsize_li]		= "LI" ;	/* max-login */
	n[bufsize_mm]		= "MM" ;	/* max-message */
	n[bufsize_mn]		= "MN" ;	/* max-name */
	n[bufsize_mp]		= "MP" ;	/* max-path */
	n[bufsize_nn]		= "NN" ;	/* name node-name */
	n[bufsize_un]		= "UN" ;	/* name user-name */
	n[bufsize_gn]		= "GN" ;	/* name group-name */
	n[bufsize_pn]		= "PN" ;	/* name project-name */
	n[bufsize_protname]	= "PROTNAME" ;	/* name prot-name (protocol) */
	n[bufsize_netwname]	= "NETWNAME" ;	/* name prot-name (network) */
	n[bufsize_hostname]	= "HOSTNAME" ;	/* name host-name (host) */
	n[bufsize_servname]	= "SERVNAME" ;	/* name serv-name (service) */
	n[bufsize_ut]		= "UT" ;	/* entry system UTMP */
	n[bufsize_pw]		= "PW" ;	/* entry system passwd */
	n[bufsize_sp]		= "SP" ;	/* entry system shadow */
	n[bufsize_ua]		= "UA" ;	/* entry system userattr */
	n[bufsize_gr]		= "GR" ;	/* entry system group */
	n[bufsize_pj]		= "PJ" ;	/* entry system project */
	n[bufsize_pr]		= "PR" ;	/* entry network protocol */
	n[bufsize_nw]		= "NW" ;	/* entry network network */
	n[bufsize_ho]		= "HO" ;	/* entry network host */
	n[bufsize_sv]		= "SV" ;	/* entry network service */
	n[bufsize_fs]		= "FS" ;	/* filesystem-type */
	n[bufsize_sn]		= "SN" ;	/* symbol-name */
	n[bufsize_zn]		= "ZN" ;	/* time-zone-name abbr */
	n[bufsize_zi]		= "ZI" ;	/* zone-info-name */
	n[bufsize_mailaddr]	= "MAILADDR" ;	/* mail-address */
	n[bufsize_overlast]	= nullptr ;
} /* end method (bufsizer::operator) */


/* exported variables */

cpcchar			*bufsizenames ;
constexpr bufsizer	bufsize_data ;
static initer		initer_data ;


/* exported subroutines */


/* local subroutines */

initer::initer() noex {
	bufsizenames = bufsize_data.names ;
} ; /* end ctor */


