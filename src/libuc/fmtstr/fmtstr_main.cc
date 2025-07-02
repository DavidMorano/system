/* fmtstr_main SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* subroutine to format string output */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	Of course, this subroutine was inspired by the UNIX®
	equivalent, but this is my own version for a) when I do not
	have the UNIX® libraries around, and b) to customize it to
	what I want!

	= 2025-05-16, David A-D- Morano
	I am updating this to be more like what I coded up for
	Aruba Networks.

*/

/* Copyright © 1998,2025 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	fmtstr

	Description:
	This subroutine is used by |printf(3c)| type routines to
	format an output string from a format specification.  Floating
	point support is optional at compile time by using the
	compile time switch "CF_FLOAT".

	Synopsis:
	int fmtstr(char *ubuf,int ulen,int mode,cchar *fmt,va_list ap) noex

	Arguments:
	ubuf		call buffer to receive result
	ulen		length of caller buffer
	mode		formatting mode (options)
	fmt		the format string
	ap		argument-pointer (see STDARG)

	Returns:
	>=0		resulting length of filled call result buffer

	Note on formatting run-time options -> 'modes':

	bits	description
	-----------------------------------
	1<<0	clean up bad strings (with bad characters) by substitution
	1<<1	return error on overflow

	Options:
	There are several compile time options available through the
	switches located at the top of this file.  

	The following nonstandard additions are supported:
	%r		counted byte string output like |u_write(2)|

	Note that all of the following format-code additions have been
	removed since the C-89 standard (adding the 'C' format-code):
	%x.yH		terminal cursor positioning, 2 integer arguments (r.c)
	%xA		the usual cursor movements
	%xB
	%xC
	%xD
	%xK		x=0,1,2
	%xJ		x=0,1,2

	Not implemented yet:
	%{string}
	%[string]	%[3A]
			%[*ERASE]

	Notes:
	1. 2025-05-16  Yes, I kind of laugh a little bit whenever
	I go and visit some very old code (like this code).  This
	code below, and in this whole code directory, dates from
	the early 1980s.  This code, in some flavor, was written
	for my mebedded protocol days at AT&T Bell Laboratories
	starting around 1983 or so.  Ya, that is why I am cracking
	up a little bit.  This code formed the main core of the
	|printf(3dam)| subroutine on the embedded computers back
	then.  I think that the first embedded computer where this
	code appeared was (called) the Emulator Peripheral Protocol
	Processor (EPPI). That was part of the control complex of
	the follow-on System-75® switching system (later called and
	marketed as DEFINITY®).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdint>
#include	<cstdarg>		/* |va_list(3c)| */
#include	<new>			/* |nothrow(3c++)| */
#include	<stdintx.h>		/* extended integer types */
#include	<usystem.h>
#include	<localmisc.h>

#include	"fmtstr.h"

import fmtobj ;

/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward refernces */


/* local variables */


/* exported variables */


/* exported subroutines */

int fmtstr(char *ubuf,int ulen,int fm,cchar *fmt,va_list ap) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (ubuf && fmt && ap) {
	    rs = SR_INVALID ;
	    if (fmt[0]) {
		fmtobj	fo(ubuf,ulen,fm,fmt) ;
		rs = fo(ap) ;
		len = rs ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (fmtstr) */


