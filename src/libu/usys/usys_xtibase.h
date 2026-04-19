/* usys_xtibase HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* XTI main defines */
/* version %I% last-modified %G% */

/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License, Version 1.0 only
 * (the "License").  You may not use this file except in compliance
 * with the License.
 *
 * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */
/*
 * Copyright 2014 Garrett D'Amore <garrett@damore.org>
 */
/*	Copyright (c) 1996 Sun Microsystems, Inc.	*/
/*	  All Rights Reserved	*/


#ifndef USYSXTIBASE_INCLUDE
#define	USYSXTIBASE_INCLUDE

#include <sys/types.h>
#include <stdint.h>

/*
 * The following include file has declarations needed by both the kernel
 * level transport providers and the user level library. This file includes
 * it to expose its namespaces to XTI user level interface.
 */
#include <sys/tpicommon.h>

/*
 * Include XTI interface level options management declarations
 */
#include <sys/xti_xtiopt.h>

#if !defined(_XPG5)

/*
 * Include declarations related to OSI transport and management data
 * structures, and the Internet Protocol Suite.
 * Note: The older Unix95/XNS4 XTI spec required these to be
 * exposed through the generic interface header.
 */
#include <sys/xti_osi.h>
#include <sys/xti_inet.h>

#endif /* !defined(_XPG5) */

/*
 * The following t_errno error codes are included in the namespace by
 * inclusion of <sys/tpicommon.h> above. The english language error strings
 * associated with the error values are reproduced here for easy reference.
 *
 * Error		Value	Error message string
 * ----			-----	--------------------
 * TBADADDR		1	Incorrect address format
 * TBADOPT		2	Incorrect options format
 * TACCES		3	Illegal permissions
 * TBADF		4	Illegal file descriptor
 * TNOADDR		5	Couldn't allocate address
 * TOUTSTATE		6	Routine will place interface out of state
 * TBADSEQ		7	Illegal called/calling sequence number
 * TSYSERR		8	System error
 * TLOOK		9	An event requires attention
 * TBADDATA		10	Illegal amount of data
 * TBUFOVFLW		11	Buffer not large enough
 * TFLOW		12	Can't send message - (blocked)
 * TNODATA		13	No message currently available
 * TNODIS		14	Disconnect message not found
 * TNOUDERR		15	Unitdata error message not found
 * TBADFLAG		16	Incorrect flags specified
 * TNOREL		17	Orderly release message not found
 * TNOTSUPPORT		18	Primitive not supported by provider
 * TSTATECHNG		19	State is in process of changing
 * TNOSTRUCTYPE		20	Unsupported structure type requested
 * TBADNAME		21	Invalid transport provider name
 * TBADQLEN		22	Listener queue length limit is zero
 * TADDRBUSY		23	Transport address is in use
 * TINDOUT		24	Outstanding connection indications
 * TPROVMISMATCH	25	Listener-acceptor transport provider mismatch
 * TRESQLEN		26	Connection acceptor has listen queue length
 *				limit greater than zero
 * TRESADDR		27	Connection acceptor-listener addresses not
 *				same but required by transport
 * TQFULL		28	Incoming connection queue is full
 * TPROTO		29	Protocol error on transport primitive
 *
 */

/*
 * The following are the events returned by t_look
 */
#define	T_LISTEN	0x0001	/* connection indication received	*/
#define	T_CONNECT	0x0002	/* connect confirmation received	*/
#define	T_DATA		0x0004	/* normal data received			*/
#define	T_EXDATA	0x0008	/* expedited data received		*/
#define	T_DISCONNECT	0x0010	/* disconnect received			*/
#define	T_UDERR		0x0040	/* data gram error indication		*/
#define	T_ORDREL	0x0080	/* orderly release indication		*/
#define	T_GODATA	0x0100	/* sending normal data is again possible */
#define	T_GOEXDATA	0x0200	/* sending expedited data is again possible */

/*
 * Flags for data primitives
 */
#define	T_MORE		0x001	/* more data		*/
#define	T_EXPEDITED	0x002	/* expedited data	*/
#define	T_PUSH		0x004	/* send data immediately */

/*
 * The following are for t_sysconf()
 */
#ifndef T_IOV_MAX
#define	T_IOV_MAX	16	/* Maximum number of scatter/gather buffers */
#endif				/* Should be <= IOV_MAX */

#ifndef _SC_T_IOV_MAX
#define	_SC_T_IOV_MAX	79	/* Should be same in <unistd.h> for use by */
#endif				/* sysconf() */

/*
 * Flags definitions for the t_info structure
 */
#define	T_SENDZERO	0x001	/* supports 0-length TSDUs */
#define	T_ORDRELDATA	0x002	/* supports orderly release data */

/*
 * The following are structure types used when dynamically
 * allocating the above structures via t_structalloc().
 */
#define	T_BIND		1		/* struct t_bind	*/
#define	T_OPTMGMT	2		/* struct t_optmgmt	*/
#define	T_CALL		3		/* struct t_call	*/
#define	T_DIS		4		/* struct t_discon	*/
#define	T_UNITDATA	5		/* struct t_unitdata	*/
#define	T_UDERROR	6		/* struct t_uderr	*/
#define	T_INFO		7		/* struct t_info	*/

/*
 * The following bits specify which fields of the above
 * structures should be allocated by t_alloc().
 */
#define	T_ADDR	0x01			/* address		*/
#define	T_OPT	0x02			/* options		*/
#define	T_UDATA	0x04			/* user data		*/
#define	T_ALL	0xffff			/* all the above fields */

/*
 * the following are the states for the user
 */

#define	T_UNINIT	0		/* uninitialized		*/
#define	T_UNBND		1		/* unbound			*/
#define	T_IDLE		2		/* idle				*/
#define	T_OUTCON	3		/* outgoing connection pending	*/
#define	T_INCON		4		/* incoming connection pending	*/
#define	T_DATAXFER	5		/* data transfer		*/
#define	T_OUTREL	6		/* outgoing release pending	*/
#define	T_INREL		7		/* incoming release pending	*/

#define	T_UNUSED		-1
#define	T_NULL			0

/*
 * Allegedly general purpose constant. Used with (and needs to be bitwise
 * distinct from) T_NOPROTECT, T_PASSIVEPROTECT and T_ACTIVEPROTECT
 * which are OSI specific constants but part of this header (defined
 * in <xti_osi.h> which is included in this header for historical
 * XTI specification reasons)
 */
#define	T_ABSREQ		0x8000

/*
 * General definitions for option management
 *
 * Multiple variable length options may be packed into a single option buffer.
 * Each option consists of a fixed length header followed by variable length
 * data. The header and data will have to be aligned at appropriate
 * boundaries. The following macros are used to manipulate the options.
 *
 * Helper Macros: Macros beginning with a "_T" prefix are helper macros.
 *		  They are private, not meant for public use and may
 *		  change without notice. Users  should use the standard
 *		  XTI macros beginning with "T_" prefix
 */

#define	_T_OPT_HALIGN_SZ	(sizeof (xti_int)) /* Hdr Alignment size  */
#define	_T_OPT_DALIGN_SZ	(sizeof (int32_t))    /* Data Alignment size */
#define	_T_OPTHDR_SZ		(sizeof (struct t_opthdr))

/* Align 'x' to the next 'asize' alignment  boundary */
#define	_T_OPT_ALIGN(x, asize) \
	(((uintptr_t)(x) + ((asize) - 1L)) & ~((asize) - 1L))

/* Align 'x' to the next header alignment  boundary */
#define	_T_OPTHDR_ALIGN(x) \
	(_T_OPT_ALIGN((x), _T_OPT_HALIGN_SZ))

/* Align 'x' to the next data alignment  boundary */
#define	_T_OPTDATA_ALIGN(x) \
	(_T_OPT_ALIGN((x), _T_OPT_DALIGN_SZ))

/*
 * struct t_opthdr *T_OPT_FIRSTHDR(struct netbuf *nbp):
 *     Get aligned start of first option header
 *
 * unsigned char *T_OPT_DATA(struct t_opthdr *tohp):
 *     Get aligned start of data part after option header
 *
 * struct t_opthdr *T_OPT_NEXTHDR(struct netbuf *nbp, struct t_opthdr *tohp):
 *	Skip to next option header
 */

#define	T_OPT_FIRSTHDR(nbp)    \
	((nbp)->len >= _T_OPTHDR_SZ ? (struct t_opthdr *)(nbp)->buf : \
	    (struct t_opthdr *)0)

#define	T_OPT_DATA(tohp)	\
	((unsigned char *)_T_OPTDATA_ALIGN((char *)(tohp) + _T_OPTHDR_SZ))

#define	_T_NEXTHDR(pbuf, buflen, popt) \
	(((char *)_T_OPTHDR_ALIGN((char *)(popt) + (popt)->len) + \
	    _T_OPTHDR_SZ <= ((char *)(pbuf) + (buflen))) ? \
	(struct t_opthdr *)((char *)_T_OPTHDR_ALIGN((char *)(popt) + \
	    (popt)->len)) : (struct t_opthdr *)0)

#define	T_OPT_NEXTHDR(nbp, tohp)   (_T_NEXTHDR((nbp)->buf, (nbp)->len, (tohp)))

#if !defined(_XPG5)
/*
 * The macros below are meant for older applications for compatibility.
 * New applications should use the T_OPT_* macros, obviating the need
 * to explicitly use the T_ALIGN macro
 *
 * struct t_opthdr *OPT_NEXTHDR(char *pbuf, unsigned int buflen,
 *                               struct t_opthdr *popt):
 *         Skip to next option header
 */
#define	T_ALIGN(p)	(((uintptr_t)(p) + (sizeof (xti_int) - 1))\
					& ~(sizeof (xti_int) - 1))
#define	OPT_NEXTHDR(pbuf, buflen, popt)	(_T_NEXTHDR((pbuf), (buflen), (popt)))

#endif /* #if !defined(_XPG5) */


#endif	/* USYSXTIBASE_INCLUDE */


