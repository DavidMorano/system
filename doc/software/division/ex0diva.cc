/* ex0div */

#include <cstring>

typedef unsigned char  uchar;
typedef unsigned short ushort;

/*
 * Compare n-byte unsigned values
 * arrays are little-endian externally,
 * but comparison here is done high-byte first.
 */
local int cmp(const uchar *a,const uchar *b,int n) {
    for (int i=n-1;i>=0;i--) {
        if(a[i] < b[i]) return -1;
        if(a[i] > b[i]) return 1;
    }
    return 0;
} /* end subroutine */

/*
 * a := a - b   over n bytes
 * little-endian
 */
local int sub(uchar *a,const uchar *b,int n) {
    int borrow = 0;
    for (int i=0;i<n;i++) {
        int t = (int)a[i] - (int)b[i] - borrow;

        if (t < 0) {
            t += 256;
            borrow = 1;
        } else {
            borrow = 0;
	}
        a[i] = (uchar)t;
    }
    return borrow ;
} /* end subroutine */

/*
 * Multiply 4-byte divisor by one byte q
 * result = 5-byte product
 * little-endian
 */
local void mul_byte(uchar *temp,const uchar *divisor,uchar q) {
    int carry = 0;
    for (int i=0;i<4;i++) {
        int t = divisor[i] * q + carry;
        temp[i] = (uchar)(t & 0xFF);
        carry = t >> 8;
    } /* end for */
    temp[4] = (uchar)carry;
} /* end subroutine */

/*
 * Shift left by one byte:
 * multiply by 256
 * little-endian means move upward
 */
local void shift_left(uchar *x,int n) {
    for (int i=n-1;i>0;i--) {
        x[i] = x[i-1];
    }
    x[0] = 0;
} /* end subroutine */

/*
 * Divide 8-byte dividend by 4-byte divisor
 * all arrays little-endian
 */
void div(uchar *quotient,uchar *remainder,
         const uchar *dividend,const uchar *divisor) {
    uchar rem[5];
    uchar prod[5];
    std::memset(quotient,0,8);
    std::memset(rem,0,5);
    /* * Process from most-significant dividend byte downward */
    for (int i=7;i>=0;i--) {
        shift_left(rem,5);
        rem[0] = dividend[i];
        /* * Estimate quotient digit from top two bytes */
        ushort top = ((ushort)rem[4] << 8) | rem[3];
        uchar q = (uchar)(top / divisor[3]);
        /* * Correct estimate downward if needed */
        forever {
            mul_byte(prod,divisor,q);
            if(cmp(rem,prod,5) >= 0) break;
            q--;
        } /* end forever */
        quotient[i] = q;
        mul_byte(prod,divisor,q);
        sub(rem,prod,5);
    } /* end for */
    /* * final remainder: low 4 bytes */
    for (int i=0;i<4;i++) {
        remainder[i] = rem[i];
    }
} /* end subroutine (div) */


