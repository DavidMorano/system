/* ex1div */


    #include <cstring>

typedef unsigned char  uchar;
typedef unsigned short ushort;

/* * Shift left N bits (little-endian) */
local uchar shln(uchar *x,int n) {
    uchar carry = 0;
    for (int i=0;i<n;i++) {
        uchar newcarry = (x[i] >> 7) & 1;
        x[i] = (uchar)((x[i] << 1) | carry);
        carry = newcarry;
    }
    return carry;
} /* end subroutine */

/* * Shift right N bits (little-endian) */
local void shr1(uchar *x,int n) {
    uchar carry = 0;
    for (int i=n-1;i>=0;i--) {
        uchar newcarry = x[i] & 1;
        x[i] = (uchar)((x[i] >> 1) | (carry << 7));
        carry = newcarry;
    }
} /* end subroutine */

/* * a += b over n bytes */
local void add(uchar *a,const uchar *b,int n) {
    int carry = 0;
    for (int i=0;i<n;i++) {
        int t = a[i] + b[i] + carry;
        a[i] = (uchar)(t & 0xFF);
        carry = t >> 8;
    }
} /* end subroutine */

/*
 * Multiply divisor by q and subtract from u
 * u is 5-byte window
 * returns borrow
 */
local int mulsub(uchar *u,const uchar *v,uchar q) {
    int carry = 0;
    int borrow = 0;

    for (int i=0;i<4;i++) {
        int p = v[i] * q + carry;
        carry = p >> 8;
        p &= 0xFF;

        int t = u[i] - p - borrow;

        if (t < 0) {
            t += 256;
            borrow = 1;
        } else {
            borrow = 0;
	}

        u[i] = (uchar)t;
    }

    int t = u[4] - carry - borrow;

    if (t < 0) {
        u[4] = (uchar)(t + 256);
        return 1;
    }

    u[4] = (uchar)t;
    return 0;
} /* end subroutine */

/*
 * 8-byte dividend / 4-byte divisor
 * little-endian
 */
void div(uchar *quotient,
         uchar *remainder,
         const uchar *dividend,
         const uchar *divisor) {
    uchar u[9];
    uchar v[4];

    std::memset(quotient,0,8);

    /* * Copy inputs */
    for (int i=0;i<8;i++) {
        u[i] = dividend[i];
    }
    u[8] = 0;

    for (int i=0;i<4;i++) {
        v[i] = divisor[i];
    }

    /* * Normalize divisor so top bit set */
    int shift = 0;

    while (v[3] < 128) {
        shln(v,4);
        shln(u,9);
        shift++;
    }

    /* * Main division loop - * 5 quotient digits possible */
    for (int j=4;j>=0;j--) {
        /* * Improved 16-bit estimate */
        ushort numerator =
            ((ushort)u[j+4] << 8) | u[j+3];

        ushort qhat16 = numerator / v[3];
        ushort rhat16 = numerator % v[3];

        uchar qhat =
            (qhat16 > 255) ? 255 : (uchar)qhat16;

        /* * Knuth correction test */
        while ( ((ushort)qhat * v[2]) > ((rhat16 << 8) + u[j+2])) {
            qhat--;
            rhat16 += v[3];
            if (rhat16 >= 256) break;
        }

        /* * Multiply-subtract */
        int borrow = mulsub(&u[j],v,qhat);

        /* * Add back if estimate too large */
        if (borrow) {
            qhat--;
            add(&u[j],v,4);
        }

        quotient[j] = qhat;
    }

    /* * Unnormalize remainder */
    for (int i=0;i<shift;i++) {
        shr1(u,4);
    }

    /* * Copy remainder (little-endian) */
    for (int i=0;i<4;i++) {
        remainder[i] = u[i];
    }
} /* end subroutine */


