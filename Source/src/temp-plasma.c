/* 
** This code is a pure Watcom C ( not difficult to port ), no Assembly 
** based implementation of plasma special effect.
**
** The plasma implemented here works by coloring a pixel with four cos 
** values, the angles of which are incremented every time in two loops. 
** Two cos values increment in the outer loop and two in the inner. But 
** before we enter both the loops, we initialize the inner four variables 
** from four other variables which are themselves incremented every once 
** in a frame. These outer four variable set ensures a movement in the 
** plasma.
**
** Note that the incrementing of all the inner & outer vars need not be in 
** terms of 1, but recommended to keep below 3 for bigger plasmas.
** A custom gradient pallete is also setup in the code.
**
** Prog. by Ranjith K Thamizhan ( ranjithk@crosswinds.net )
** Date: 3/4/2000
**
** Please send me a mail, if you find this code useful.
*/

#include <i86.h>
#include <math.h>

/* Set RGB values for a color */

void setrgb( unsigned char col, 
	     unsigned char R, 
	     unsigned char G, 
	     unsigned char B )
{
    outp(0x3C8,col);   
    outp(0x3C9,R);   
    outp(0x3C9,G);   
    outp(0x3C9,B);   
}

int main()
{
    unsigned char  *scr = (unsigned char*) 0xA0000; /* Screen */
    unsigned char  *vscr, *tscr;    /* Video buffer for speed */
    union REGS     s;
    int            costbl[256];
    int            t,y,x;
    unsigned char  a1,a2,a3,a4,
                   b1,b2,b3,b4;


    vscr = (unsigned char*)malloc(320*200);
    
    if( !vscr )
    {
        printf("Not enough memory");
        return(0);
    }

    /* Start with a clean screen */

    memset(vscr,0,320*200);

    /* Initialize cos table along with some pre-calculation */

    for(t=0;t<256;t++)
        costbl[t] = (int)( 30 * cos(t * (3.14159/128) ) );
    
    /* Start 320*200 graphics mode */

    s.w.ax = 0x13;
    int386(0x10,&s,&s);


    /* Set up a gradient like palette */

    for (x=1; x<=32; x++) {
        setrgb(x, 0, 0, x*2-1);       /* Blue part */
        setrgb(x+32, x*2-1, 0, 63);
        setrgb(x+64, 63, x*2-1, 63);
        setrgb(x+96, 63, 63, 63);
 
        setrgb(x+128, 63, 63, 63);
        setrgb(x+160, 63, 63, 63-(x*2-1));
        setrgb(x+192, 63, 63-(x*2-1), 0);
        setrgb(x+224, 63-(x*2-1), 0, 0); /* Red part */

    }


    /* Loop until key is pressed */

    while( !kbhit() ){

	/* Initialize with outer variables */

        a1 = b1;
        a2 = b2;

        /* 
	** tscr for *tscr++, rather than *(vscr+(y*320)+x),
	** this temporary variable will speed up calculation. 
	*/

        tscr = vscr;

        for(y=0;y<200;y++)
        {
	    /* Initialize with outer variables */

            a3 = b3;
            a4 = b4;

            for(x=0;x<320;x++)
            {
                *tscr++ = costbl[a1] +
                          costbl[a2] +
                          costbl[a3] +
                          costbl[a4] ;

                /* Higher values result in many slower plasmas */

                a3 += 1;
                a4 += 2;
            }

            /* Same as the previous comment*/

            a1 += 2;
            a2 += 1;

        }

        /* 
	** Increment the outer variables. The higher these vars are 
	** incremented, the faster is the plasma. 
	*/

        b1 += 1;
        b2 -= 2;
        b3 -= 1;
        b4 += 3;
        
        /* Blit plasmas unto screen */

        memcpy(scr,vscr,320*200);
    
    }

    getch();

    /* Get back to text mode */

    s.w.ax = 0x3;
    int386(0x10,&s,&s);

    return(0);
}
