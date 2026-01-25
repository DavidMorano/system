/* bufsizes HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* list the various system buffer-size names (as strings) */
/* version %I% last-modified %G% */

/* revision history:

	= 2001-04-11, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	bufsizes

	Description:
	This file contains the string names (enumeration names) for
	the various system buffer-sizes, and the array interface
	to access them.

*******************************************************************************/

#ifndef	BUFSIZES_INCLUDE
#define	BUFSIZES_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


enum bufsizes {
	bufsize_ps,		/* page-size */
	bufsize_mi,		/* max-pid */
	bufsize_ma,		/* max-args */
	bufsize_mn,		/* max-name */
	bufsize_mp,		/* max-path */
	bufsize_ml,		/* max-line */
	bufsize_mm,		/* max-message */
	bufsize_nn,		/* node-name */
	bufsize_hn,		/* host-name */
	bufsize_un,		/* user-name */
	bufsize_gn,		/* group-name */
	bufsize_pn,		/* project-name */
	bufsize_pw,		/* entry passwd */
	bufsize_sp,		/* entry shadow */
	bufsize_ua,		/* entry user-attr */
	bufsize_gr,		/* entry group */
	bufsize_pj,		/* entry project */
	bufsize_pr,		/* entry protocol */
	bufsize_nw,		/* entry network */
	bufsize_ho,		/* entry host */
	bufsize_sv,		/* entry service */
	bufsize_fs,		/* filesystem-type */
	bufsize_sn,		/* symbol-name */
	bufsize_zn,		/* time-zone-name (common abbreviated form) */
	bufsize_zi,		/* zone-info-name (newer concept) */
	bufsize_mailaddr,	/* mail-addr */
	bufsize_overlast
} ; /* end enum (bufsizes) */

extern cchar	*bufsizes[] ;


#endif /* BUFSIZES_INCLUDE */


