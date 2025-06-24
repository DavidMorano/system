/* onc SUPPORT */
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
#include	<sys/types.h>
#include	<usystem.h>
#include	<localmisc.h>

#include	"stubrpc.h"
#include	"onc.h"


/* local defines */

#define	PASSPHRASELEN	8		/* maximum ONC password length */

#define	HEXKEYBYTES	STUBRPC_HEXKEYBYTES


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int onckeyalready(cchar *netname) noex {
	int		rs = SR_FAULT ;
	if (netname) {
	    if ((rs = key_secretkey_is_set()) < 0) {
	        rs = SR_NOPKG ;
	    }
	}
	return rs ;
}
/* end subroutine (onckeyalready) */

int onckeygetset(cchar *netname,cchar *passbuf) noex {
	int		rs = SR_FAULT ;
	if (netname && passbuf) {
	    rs = SR_INVALID ;
	    if (netname[0]) {
	        key_netstarg	sna{} ;
	        cchar		*nnp = netname ;
	        char		passphrase[PASSPHRASELEN + 1] ;
		rs = SR_OK ;
	        strncpy(passphrase,passbuf,PASSPHRASELEN) ;
	        passphrase[PASSPHRASELEN] = '\0' ;	/* truncate */
	        memset(sna.st_priv_key,0,HEXKEYBYTES) ;
	        sna.st_pub_key[0] = '\0' ;
	        sna.st_netname = (char *) netname ;
/* decrypt and retrieve the private key */
	            if (getsecretkey(nnp,sna.st_priv_key,passphrase) > 0) {
	                if (sna.st_priv_key[0] != '\0') {
/* we have successfully decrypted our private ONC key, give it to KEYSERV */
	                    if ((rs = key_setnet(&sna)) > 0) {
	                        memset(sna.st_priv_key,0,HEXKEYBYTES) ;
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
	        memset(passphrase,0,PASSPHRASELEN) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (oncskeygetset) */

int onckeylogin(cchar *passbuf) noex {
	int		rs = SR_FAULT ;
	if (passbuf) {
	    key_netstarg	sna{} ;
	    char		netname[MAXNETNAMELEN + 1] ;
	    rs = SR_OK ;
	    sna.st_netname = netname ;
	    if (int f_netname ; (f_netname = getnetname(netname)) == 0) {
	        if (int rc ; (rc = key_secretkey_is_set()) == 0) {
	            char	netname3[MAXNETNAMELEN + 1] ;
	            char	passphrase[PASSPHRASELEN + 1] ;
	            char	*nnp = netname ;
	            strncpy(passphrase,passbuf,PASSPHRASELEN) ;
	            passphrase[PASSPHRASELEN] = '\0' ; /* truncate at maximum */
	            strcpy(netname3,nnp) ;
	            memset(sna.st_priv_key,0,HEXKEYBYTES) ;
	            sna.st_pub_key[0] = '\0' ;
	            sna.st_netname = netname3 ;
	            if (getsecretkey(netname,sna.st_priv_key,passphrase) &&
	                (sna.st_priv_key[0] != '\0')) {
/* decrypted private ONC key, give it to KEYSERV */
	                if ((rs = key_setnet(&sna)) < 0) {
	                    rs = SR_ACCESS ;
		        }
/* destroy the private key since we do not really need or want it! */
	                memset(sna.st_priv_key,0,HEXKEYBYTES) ;
	            } /* end if (decrypted and retrieved private key) */
	            memset(passphrase,0,PASSPHRASELEN) ;
	        } /* end if (key-is-already-set) */
	    } else {
	        rs = SR_NOTSUP ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (onckeylogin) */


