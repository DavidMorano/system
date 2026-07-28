/* onc SUPPORT (Open-Network-Computing) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* ONC operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-10, David A­D­ Morano
	This subroutine was written to deal with NIS key authorization
	issues when using Solaris 2.x.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	onckeyalready

	Description:
	This subroutine checks if the KEYSERV daemon already has
	our secret ONC private key.  We use the (formerly) secret
	undocumented subroutine |key_secretkey_is_set()| to do the
	job for us.

	Synopsis:
	int onckeyalready(cchar *netname) noex

	Arguments:
	netname		a user supplied buffer with an ONC "netname"

	Returns:
	>=0	OK, 0=NO, 1=YES
	<0	error (system-return)


	Name:
	onckeygetset

	Description:
	This subroutine will decrypt the private key by getting the
	encrypted version from the key database and then decrypting
	it.  This is "getting the key."  We then give the decrypted
	private key to the key-server for subsequent (whatever the
	user wants) ONC secure operations.

	Synopsis:
	int onckeygetset(cchar *netname,cchar *passwd) noex

	Arguments:
	netname		user supplied netname to use
	passwd		user supplied password used to decrypt the private key

	Returns:
	>=0		OK
	<0		error (system-return)

	Program notes:
	The |key_setnet()| subroutine is part of the KEYSERV version 2 API.
	There does not appear to be a manual page on it so we guess at its use
	where necessary!  It must be used by |login(1)|, |keylogin(1)|, and
	other such programs for setting the ONC netname on login.

	Returns:
	1	succeeded in setting ONC private key to KEYSERV
	-1	failed to set ONC private key with KEYSERV


	Name:
	getsecretkey

	Synopsis:
	int getsecretkey(cchar *netname,char *result) noex

	Arguments:
	netname		given net-name
	result		buffer to retrieve secret key (in HEX)

	Returns:
	1		succeeded
	0		failed


	Name:
	onckeylogin

	Description:
	This subroutine is a sort of "all in one" key-login subroutine.
	We will get the user's ONC netname, get the encrypted private
	key from the key database, decrypt it with the supplied
	password (the missing piece), and give the resulting private
	key to the KEYSERV server.

	Synopsis:
	int onckeylogin(cchar *passwd) noex

	Returns:
	>=0	OK
	<0	error (system-return)

	Program notes:
	The |key_setnet()| subroutine is part of the KEYSERV version
	2 API.  There does not appear to be a manual page on it so
	we guess at its use where necessary!  It must be used by
	|login(1)|, |keylogin(1)|, and other such programs for
	setting the ONC netname on login.

	Rerturns:
	1	succeeded in setting ONC private key to KEYSERV
	-1	failed to set ONC private key with KEYSERV

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"stubrpc.h"
#include	"onc.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	PASSPHRASELEN	8		/* maximum ONC password length */

#define	HEXKEYBYTES	STUBRPC_HEXKEYBYTES


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct vars {
	int	netnamelen ;
	operator int () noex ;
    } ; /* end struct (vars) */
} /* end namespace */


/* forward references */

local int oncinit() noex ;


/* local variables */

static vars	var ;

cint		passlen = PASSPHRASELEN ;


/* exported variables */


/* exported subroutines */

int onckeyalready(cchar *netname) noex {
	int		rs = SR_FAULT ;
	if (netname) ylikely {
	    if ((rs = key_secretkey_is_set()) < 0) {
	        rs = SR_NOPKG ;
	    }
	}
	return rs ;
} /* end subroutine (onckeyalready) */

int onckeygetset(cchar *netname,cchar *pass) noex {
	int		rs = SR_FAULT ;
	if (netname && pass) ylikely {
	    rs = SR_INVALID ;
	    if (netname[0]) ylikely {
		if ((rs = oncinit()) >= 0) ylikely {
	            key_netstarg	sna{} ;
	            cchar		*nnp = netname ;
	            char		passbuf[passlen + 1] ;
		    rs = SR_OK ;
	            strncpy(passbuf,pass,passlen) ;
	            passbuf[passlen] = '\0' ;	/* truncate */
	            memclear(sna.st_priv_key,HEXKEYBYTES) ;
	            sna.st_pub_key[0] = '\0' ;
	            sna.st_netname = charp(netname) ;
		    /* decrypt and retrieve the private key */
	            if (getsecretkey(nnp,sna.st_priv_key,passbuf) > 0) {
	                if (sna.st_priv_key[0] != '\0') {
			/* have decrypted our private ONC key */
			/* give it to KEYSERV */
	                    if ((rs = key_setnet(&sna)) > 0) {
	                        memclear(sna.st_priv_key,HEXKEYBYTES) ;
		            } else if (rs == 0) {
		                rs = SR_PROTO ;
		            } else {
	                        rs = SR_SRCH ;
		            }
		        } else {
		             rs = SR_PROTO ;
		        }
	            } else {
	                rs = SR_NOENT ;
	            }
	            memclear(passbuf,passlen) ;
		} /* end if (oncinit) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (oncskeygetset) */

int onckeylogin(cchar *pass) noex {
	int		rs = SR_FAULT ;
	if (pass) {
	    if ((rs = oncinit()) >= 0) {
		cint netlen = var.netnamelen ;
	        key_netstarg	sna{} ;
	        char		netname[netlen + 1] ;
	        sna.st_netname = netname ;
	        if (int f_nn ; (f_nn = getnetname(netname)) == 0) {
	            if (int rc ; (rc = key_secretkey_is_set()) == 0) {
	                char	netname3[netlen + 1] ;
	                char	passbuf[passlen + 1] ;
	                char	*nnp = netname ;
	                strncpy(passbuf,pass,passlen) ;
		        /* truncate at maximum */
	                passbuf[passlen] = '\0' ; 
	                strcpy(netname3,nnp) ;
	                memclear(sna.st_priv_key,HEXKEYBYTES) ;
	                sna.st_pub_key[0] = '\0' ;
	                sna.st_netname = netname3 ;
		        bool f = true ;
		        cauto gsk = getsecretkey ;
	                f = f && gsk(netname,sna.st_priv_key,passbuf) ;
		        f = f && (sna.st_priv_key[0] != '\0') ;
		        if (f) {
			    /* decrypted private ONC key */
			    /* give it to KEYSERV */
	                    if ((rs = key_setnet(&sna)) < 0) {
	                        rs = SR_ACCESS ;
		            }
			    /* destroy the private key */
	                    memclear(sna.st_priv_key,HEXKEYBYTES) ;
	                } /* end if (decrypted and retrieved private key) */
	                memclear(passbuf,passlen) ;
	            } /* end if (key-is-already-set) */
	        } else {
	            rs = SR_NOTSUP ;
	        }
	    } /* end if (oncinit) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (onckeylogin) */


/* local subroutines */

vars::operator int () noex {
    	int		rs ;
	if ((rs = usys::getnetnamelen()) >= 0) {
	    netnamelen = rs ;
	}
	return rs ;
} /* end methof (vars::operator) */

local int oncinit() noex {
    static cint rsv = var ;
    return rsv ;
} /* end subroutine (oncinit) */


