/* getpgopts HEADER (Get_postfacts-Options) */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* get (retreive) Postfacts options */
/* version %I% last-modified %G% */


/* revision history:

	= 1995-05-01, David A­D­ Morano
	This code module was completely rewritten to replace any
	original garbage that was here before.

*/

/* Copyright © 1995 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */

#ifndef	GETPFOPTS_INCLUDE
#define	GETPFOPTS_INCLUDE


enum getpfopts {
	getpfopt_mailername,
	getpfopt_progrbbpost,
	getpfopt_progmsgs,
	getpfopt_newsgroup,
	getpfopt_spooldir,
	getpfopt_overlast
} ; /* end enum */


#define	PROGOPT_MAILERNAME	getpfopt_mailername
#define	PROGOPT_PROGRBBPOST	getpfopt_progrbbpost
#define	PROGOPT_PROGMSGS	getpfopt_progmsgs
#define	PROGOPT_NEWSGROUP	getpfopt_newsgroup
#define	PROGOPT_SPOOLDIR	getpfopt_spooldir
#define	PROGOPT_OVERLAST	getpfopt_overlast


#endif /* GETPFOPTS_INCLUDE */


