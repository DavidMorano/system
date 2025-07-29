/* clanguage HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* version %I% last-modified %G% */
/* C-language defines */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

	= 2017-08-01, David A­D­ Morano
	Updated for lack of interfaces in MacOS Darwin

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

#ifndef	CLANGUAGE_INCLUDE
#define	CLANGUAGE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */


#ifndef	noex
#ifdef	__cplusplus
#define	noex			noexcept
#else
#define	noex			/* noexcept */
#endif
#endif /* noex */

#ifndef	scharconv
#define	scharconv		schar
#endif

#ifndef	charconv
#define	charconv		char
#endif
#ifndef	shortconv
#define	shortconv		short
#endif
#ifndef	intconv
#define	intconv			int
#endif
#ifndef	longconv
#define	longconv		long
#endif

#ifndef	ucharconv
#define	ucharconv		uchar
#endif
#ifndef	ushortconv
#define	ushortconv		ushort
#endif
#ifndef	uintconv
#define	uintconv		uint
#endif
#ifndef	uintlongconv
#define	ulongconv		ulong
#endif

#ifndef	floatconv
#define	floatconv		float
#endif

#ifndef	ylikely
#define	ylikely			[[likely]]
#endif /* ylikely */

#ifndef	nlikely
#define	nlikely			[[unlikely]]
#endif /* nlikely */

#ifdef	__cplusplus
#ifndef	restrict
#define	restrict		__restrict__
#endif
#endif /* __cplusplus */

#ifdef	__cplusplus
#ifndef	attrweak
#define	attrweak		__attribute__((weak))
#endif
#endif /* __cplusplus */

#ifdef	__cplusplus
#ifndef	attrweakref
#define	attrweakref(target)	__attribute__((weakref(target)))
#endif
#endif /* __cplusplus */

#ifdef	__cplusplus
#ifndef	attrconst
#define	attrconst		__attribute__((const))
#endif
#endif /* __cplusplus */

#ifdef	__cplusplus
#ifndef	attrpure
#define	attrpure		__attribute__((pure))
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

#ifndef	va_begin
#define	va_begin(ap,arg)	va_start((ap),(arg))
#endif
#ifndef	va_get
#define	va_get(ap,atype)	va_arg(ap,atype)
#endif

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

#ifndef	fallthrough
#define	fallthrough		/* FALLTHROUGH */
#endif

#ifndef	nelements
#define	nelements(n)		((int) (sizeof(n) / sizeof((n)[0])))
#endif

#ifndef	nelem
#define	nelem(n)		((int) (sizeof(n) / sizeof((n)[0])))
#endif

#ifdef	__cplusplus
#ifndef	npack
#define	npack(args)		int(sizeof...(args))
#endif
#endif /* __cplusplus */

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
#ifndef	cast_static
#define	cast_static		static_cast
#define	cast_dynamic		dynamic_cast
#define	cast_const		const_cast
#define	cast_reinterpret	reinterpret_cast
#define	cast_bit		bit_cast
#define	cast_saturate		saturate_cast
#define	cast_any		any_cast
#endif
#endif /* __cplusplus */

#ifdef	__cplusplus
#ifndef	assert_contract
#define	assert_contract		contract_assert
#define	assert_static		static_assert
#endif
#endif /* __cplusplus */

#ifdef	__cplusplus
#ifndef	cauto
#define	cauto			const auto
#endif
#endif /* __cplusplus */

#ifdef	__cplusplus
#ifndef	szof
#define	szof(T)		int(sizeof(T))
#endif
#endif /* __cplusplus */

#ifndef	EXTERNC_begin
#ifdef	__cplusplus
#define	EXTERNC_begin	extern "C" {
#define	EXTERNC_end	}
#else
#define	EXTERNC_begin	/* externc_begin */
#define	EXTERNC_end	/* externc_end */
#endif /* __cplusplus */
#endif /* EXTERNC_begin */

#ifndef	CAST_R
#define	CAST_R		cast_reinterpret
#endif


#endif /* CLANGUAGE_INCLUDE */


