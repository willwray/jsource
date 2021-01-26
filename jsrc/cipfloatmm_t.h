// cache-blocking code
#ifndef OPHEIGHT
#define OPHEIGHT 2  // height of outer-product block
#define OPWIDTH 4  // width of outer-product block
#define CACHEWIDTH 64  // width of resident cache block
#define CACHEHEIGHT 16  // height of resident cache block
#endif
// Floating-point matrix multiply, hived off to a subroutine to get fresh register allocation
// *zv=*av * *wv, with *cv being a cache-aligned region big enough to hold CACHEWIDTH*CACHEHEIGHT floats
// a is shape mxp, w is shape pxn
static void FUNCNAME (D* av,D* wv,D* zv,I m,I n,I p){D c[(CACHEHEIGHT+1)*CACHEWIDTH + (CACHEHEIGHT+1)*OPHEIGHT*OPWIDTH + 2*CACHELINESIZE/sizeof(D)];
 // m is # 1-cells of a
 // n is # atoms in an item of w (and result)
 // p is number of inner-product muladds (length of a row of a, and # items of w)
 // point to cache-aligned areas we will use for staging the inner-product info
 D *cvw = (D*)(((I)&c+(CACHELINESIZE-1))&-CACHELINESIZE);  // place where cache-blocks of w are staged
 D *cva = (D*)(((I)cvw+(CACHEHEIGHT+1)*CACHEWIDTH*sizeof(D)+(CACHELINESIZE-1))&-CACHELINESIZE);   // place where expanded rows of a are staged
 // zero the result area
 memset(zv,C0,m*n*sizeof(D));
 // process each 64-float vertical stripe of w, producing the corresponding columns of z
 D* w0base = wv; D* z0base = zv; I w0rem = n;
 for(;w0rem>0;w0rem-=CACHEWIDTH,w0base+=CACHEWIDTH,z0base+=CACHEWIDTH){
  // process each 16x64 section of w, adding each result to the columns of z
  D* a1base=av; D* w1base=w0base; D* z1base=z0base; I w1rem=p;
  for(;w1rem>0;w1rem-=CACHEHEIGHT,a1base+=CACHEHEIGHT,w1base+=CACHEHEIGHT*n){D* RESTRICT cvx;D* w1next=w1base;I i;
   // read the 16x64 section of w into the cache area (8KB, 2 ways of cache), with prefetch of rows
   for(i=MIN(CACHEHEIGHT,w1rem),cvx=cvw;i;--i){I j;
    D* RESTRICT w1x=w1next; w1next+=n;  // save start of current input row, point to next row...
    // I don't think it's worth the trouble to move the data with avx instructions - though it was to prefetch it
    for(j=0;j<MIN(CACHEWIDTH,w0rem);++j)*cvx++=*w1x++; for(;j<CACHEWIDTH;++j)*cvx++=0.0;   // move current row during prefetch
   }
   // Because of loop unrolling, we fetch and multiply one extra value in each cache column.  We make sure those values are 0 to avoid NaN errors
   for(i=0;i<MIN(CACHEWIDTH,w0rem);++i)*cvx++=0.0;

   // w1next is left pointing to the next cache block in the column.  We will use that to prefetch

   // the nx16 vertical strip of a will be multiplied by the 16x64 section of w and accumulated into z
   // process each 2x16 section of a against the 16x64 cache block
   D *a2base0=a1base; D* w2base=w1base; I a2rem=m; D* z2base=z1base; D* c2base=cvw;
   for(;a2rem>0;a2rem-=OPHEIGHT,a2base0+=OPHEIGHT*p,z2base+=OPHEIGHT*n){
    static D missingrow[CACHEHEIGHT]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
    // Prepare for the 2x16 block of a
    // If the second row of a is off the end of the data, we mustn't fetch it - switch the pointer to a row of 1s so it won't give NaN error on multiplying by infinity
    D *a2base1 = (a2rem>1)?a2base0+p:missingrow;
#ifdef PREFETCH
    // While we are processing the sections of a, move the next cache block into L2 (not L1, so we don't overrun it)
    // We would like to do all the prefetches for a CACHEWIDTH at once to stay in page mode
    // but that might overrun the prefetch queue, which holds 10 prefetches.  So we just squeeze off 3 at a time
    if(a2rem<=4*OPHEIGHT*CACHEHEIGHT){I offset= (a2rem&(3*OPHEIGHT))*(2*CACHELINESIZE/OPHEIGHT);
     PREFETCH2((C*)w1next+offset); PREFETCH2((C*)w1next+offset+CACHELINESIZE); PREFETCH2((C*)w1next+offset+2*CACHELINESIZE);
     if(offset==CACHELINESIZE*3*OPHEIGHT)w1next += n;  // advance to next row for next time
    }
#endif
    // process each 16x4 section of cache, accumulating into z
    I a3rem=MIN(w0rem,CACHEWIDTH);
    D* RESTRICT z3base=z2base; D* c3base=c2base;
    for(;a3rem>0;a3rem-=OPWIDTH,c3base+=OPWIDTH,z3base+=OPWIDTH){
     // initialize accumulator with the z values accumulated so far.
     D z00,z01,z02,z03,z10,z11,z12,z13;
     z00=z3base[0];
     if(a3rem>3){z01=z3base[1],z02=z3base[2],z03=z3base[3]; if(a2rem>1)z10=z3base[n],z11=z3base[n+1],z12=z3base[n+2],z13=z3base[n+3];
     }else{if(a3rem>1){z01=z3base[1];if(a3rem>2)z02=z3base[2];}; if(a2rem>1){z10=z3base[n];if(a3rem>1)z11=z3base[n+1];if(a3rem>2)z12=z3base[n+2];}}
     // process outer product of each 2x1 section on each 1x4 section of cache

     // Prefetch the next lines of a and z into L2 cache.  We don't prefetch all the way to L1 because that might overfill L1,
     // if all the prefetches hit the same line (we already have 2 lines for our cache area, plus the current z values)
     // The number of prefetches we need per line is (CACHEHEIGHT*sizeof(D)/CACHELINESIZE)+1, and we need that for
     // OPHEIGHT*(OPWIDTH/4) lines for each of a and z.  We squeeze off half the prefetches for a row at once so we can use fast page mode
     // to read the data (only half to avoid overfilling the prefetch buffer), and we space the reads as much as possible through the column-swatches
#ifdef PREFETCH   // if architecture doesn't support prefetch, skip it
     if((3*OPWIDTH)==(a3rem&(3*OPWIDTH))){  // every fourth swatch
      I inc=((a3rem&(8*OPWIDTH))?p:n)*sizeof(D);    // counting down, a then z
      C *base=(C*)((a3rem&(8*OPWIDTH))?a2base0:z2base) + inc + (a3rem&(4*OPWIDTH)?0:inc);
      PREFETCH2(base); PREFETCH2(base+CACHELINESIZE); PREFETCH2(base+2*CACHELINESIZE);
     }
#endif

     I a4rem=MIN(w1rem,CACHEHEIGHT);
     D* RESTRICT c4base=c3base;
     D* RESTRICT a4base0=a2base0; D* RESTRICT a4base1=a2base1; 
     do{
      // read the 2x1 a values and the 1x4 cache values
      // form outer product, add to accumulator
      D t0,t1,a0,a1,c0,c1,c2,c3;
      a0=a4base0[0]; t0=c4base[0]; 
      a1=a4base1[0]; c0=c4base[0];
      t0 *= a0; c0 *= a1;
      t1=c4base[1]; c1=c4base[1]; t1 *= a0; c1 *= a1;
      z00 += t0; t0 = c4base[2]; c2 = c4base[2]; c3 = c4base[3];
      z10 += c0; z01 += t1; z11 += c1;
      t0 *= a0; c2 *= a1; a0 *= c3; c3 *= a1;
      z02 += t0; z12 += c2; z03 += a0; z13 += c3;
      a4base0++,a4base1++;
      c4base+=CACHEWIDTH;
     }while(--a4rem>0);

     // Store accumulator into z.  Don't store outside the array
     z3base[0]=z00;
     if(a3rem>3){z3base[1]=z01,z3base[2]=z02,z3base[3]=z03; if(a2rem>1)z3base[n]=z10,z3base[n+1]=z11,z3base[n+2]=z12,z3base[n+3]=z13;
     }else{if(a3rem>1){z3base[1]=z01;if(a3rem>2)z3base[2]=z02;}; if(a2rem>1){z3base[n]=z10;if(a3rem>1){z3base[n+1]=z11;if(a3rem>2)z3base[n+2]=z12;}}}
    }
   }
  }
 }
}

