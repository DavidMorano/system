/* clanguage_keywords HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* C-language defines */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-15, David A­D­ Morano
	This module was originally written.

	= 2017-08-01, David A­D­ Morano
	Updated for lack of interfaces in MacOS Darwin

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

#ifndef	CLANGUAGEKEYWORDS_INCLUDE
#define	CLANGUAGEKEYWORDS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */


#ifdef	__cplusplus
#ifndef	restrict
#define	restrict		__restrict__
#endif
#endif /* __cplusplus */

#ifndef	noex
#ifdef	__cplusplus
#define	noex			noexcept
#else
#define	noex			/* noexcept */
#endif
#endif /* noex */

#ifndef	conex
#ifdef	__cplusplus
#define	conex			constexpr
#else
#define	conex			/* constexpr */
#endif
#endif /* conex */

#ifdef	__cplusplus
#ifndef	typeof
#define	typeof			decltype
#endif
#endif /* __cplusplus */

#ifdef	__cplusplus
#ifndef	typealias
#define	typealias		typedef
#endif
#endif /* __cplusplus */

#ifdef	__cplusplus
#ifndef	destruct		
#define	destruct		compl
#endif
#endif /* __cplusplus */

#ifndef	repeat
#define repeat			do
#define	until(cond)		while(!(cond))
#endif

#ifndef	forever
#define	forever			for (;;)
#endif

#ifndef	local
#define	local			static
#endif

#ifndef	typealias
#define	typealias		typedef
#endif

#ifndef	falldown
#define	falldown		[[fallthrough]]
#endif

#ifdef	__cplusplus
#ifndef	if_constexpr
#define	if_constexpr		if constexpr
#endif
#endif /* __cplusplus */

#ifdef	__cplusplus
#ifndef	if_consteval
#define	if_consteval		if consteval
#endif
#endif /* __cplusplus */

#ifdef	__cplusplus
#ifndef	cauto
#define	cauto			const auto
#endif
#endif /* __cplusplus */

#ifdef	__cplusplus
#ifndef	asconst
#define	asconst			as_const
#endif
#endif /* __cplusplus */

#ifndef	mut
#define	mut			/* mutable */
#endif /* __cplusplus */

#ifndef	con
#define	con			const
#endif /* __cplusplus */

#ifndef	vol
#define	vol			volatile
#endif /* __cplusplus */

#ifndef	conv
#define	conv			static_cast
#endif


#endif /* CLANGUAGEKEYWORDS_INCLUDE */


