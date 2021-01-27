/* Copyright 1990-2008, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Adverbs: Oblique and Key                                                */

#include "j.h"

// This is the derived verb for f/. y
static DF1(jtoblique){A x,y,z;I m,n,r;D rkblk[16];
 F1PREFIP;ARGCHK1(w);
 r=AR(w);  // r = rank of w
 // create y= ,/ w - the _2-cells of w arranged in a list (virtual block)
 RZ(y=redcat(w,self)); if(1>=r){m=AN(w); n=1;}else{m=AS(w)[0]; n=AS(w)[1];}
 // Create x=+"0 1&i./ 2 {. $y
 A xm; RZ(xm=IX(m)); A xn; RZ(xn=IX(n));
 RZ(x=ATOMIC2(jt,xm,xn,rkblk,0L,1L,CPLUS)); AR(x)=1; AS(x)[0]=AN(x);
 // perform x f/. y, which does the requested operation, collecting the identical keys
 RZ(df2ip(z,x,y,sldot(VAV(self)->fgh[0])));
 // Final tweak: the result should have (0 >. <: +/ 2 {. $y) cells.  It will, as long as
 // m and n are both non0: when one is 0, result has 0 cells (but that cell is the correct result
 // of execution on a fill-cell).  Correct the length of the 0 case, when the result length should be nonzero
// if((m==0 || n==0) && (m+n>0)){return reitem(sc(m+n-1),x);}  This change withdrawn pending further deliberation
 return z;
}


#define OBQCASE(t,id)    ((t)+(11*(id)))

#define OBQLOOP(Tw,Tz,zt,init,expr)  \
 {Tw* RESTRICT u,*v,*ww=(Tw*)wv;Tz x,* RESTRICT zz;                  \
  b=1; k=n1;                                     \
  GA(z,zt,d*c,r-1,1+s); AS(z)[0]=d; zz=(Tz*)AV(z); \
  DO(n,  v=ww+i;      u=v+n1*MIN(i,m1);     init; while(v<=(u-=n1))expr; *zz++=x;);  \
  DO(m1, v=ww+(k+=n); u=v+n1*MIN(m-i-2,n1); init; while(v<=(u-=n1))expr; *zz++=x;);  \
 }

// Derived verb for f//. y for atomic f
static DF1(jtobqfslash){A y,z;B b=0,p;C er,id,*wv;I c,d,k,m,m1,mn,n,n1,r,*s,wt;
 ARGCHK1(w);
 r=AR(w); s=AS(w); wt=AT(w); wv=CAV(w);
 if((-AN(w)&(1-r)&-(DENSE&wt))>=0)return oblique(w,self);  // revert to default if rank<2, empty, or sparse.  This implies m/n below are non0
 y=FAV(self)->fgh[0]; y=FAV(y)->fgh[0]; id=FAV(y)->id;
 m=s[0]; m1=m-1;
 n=s[1]; n1=n-1; mn=m*n; d=m+n-1; PROD(c,r-2,2+s);
 if(((1-m)&(1-n))>=0){GA(z,wt,AN(w),r-1,1+s); AS(z)[0]=d; MC(AV(z),wv,AN(w)<<bplg(wt)); return z;}  // m=1 or n=1, return item of input
 if(wt&FL+CMPX)NAN0;
 if(1==c)switch(OBQCASE(CTTZ(wt),id)){
  case OBQCASE(B01X, CNE     ): OBQLOOP(B,B,wt,x=*u, x^=*u        ); break;
  case OBQCASE(B01X, CEQ     ): OBQLOOP(B,B,wt,x=*u, x=x==*u      ); break;
  case OBQCASE(B01X, CMAX    ):
  case OBQCASE(B01X, CPLUSDOT): OBQLOOP(B,B,wt,x=*u, x|=*u        ); break;
  case OBQCASE(B01X, CMIN    ):
  case OBQCASE(B01X, CSTAR   ):
  case OBQCASE(B01X, CSTARDOT): OBQLOOP(B,B,wt,x=*u, x&=*u        ); break;
  case OBQCASE(B01X, CLT     ): OBQLOOP(B,B,wt,x=*u, x=*u< x      ); break;
  case OBQCASE(B01X, CLE     ): OBQLOOP(B,B,wt,x=*u, x=*u<=x      ); break;
  case OBQCASE(B01X, CGT     ): OBQLOOP(B,B,wt,x=*u, x=*u> x      ); break;
  case OBQCASE(B01X, CGE     ): OBQLOOP(B,B,wt,x=*u, x=*u>=x      ); break;
  case OBQCASE(B01X, CPLUS   ): OBQLOOP(B,I,INT,x=*u, x+=*u       ); break;
  case OBQCASE(SBTX, CMAX    ): OBQLOOP(SB,SB,wt,x=*u, x=SBGT(x,*u)?x:*u ); break;
  case OBQCASE(SBTX, CMIN    ): OBQLOOP(SB,SB,wt,x=*u, x=SBLT(x,*u)?x:*u ); break;
  case OBQCASE(FLX,  CMAX    ): OBQLOOP(D,D,wt,x=*u, x=MAX(x,*u)  ); break;
  case OBQCASE(FLX,  CMIN    ): OBQLOOP(D,D,wt,x=*u, x=MIN(x,*u)  ); break;
  case OBQCASE(FLX,  CPLUS   ): OBQLOOP(D,D,wt,x=*u, x+=*u        ); break;
  case OBQCASE(CMPXX,CPLUS   ): OBQLOOP(Z,Z,wt,x=*u, x=zplus(x,*u)); break;
  case OBQCASE(XNUMX,CMAX    ): OBQLOOP(X,X,wt,x=*u, x=1==xcompare(x,*u)? x:*u); break;
  case OBQCASE(XNUMX,CMIN    ): OBQLOOP(X,X,wt,x=*u, x=1==xcompare(x,*u)?*u: x); break;
  case OBQCASE(XNUMX,CPLUS   ): OBQLOOP(X,X,wt,x=*u, x=xplus(x,*u)); break;
  case OBQCASE(RATX, CMAX    ): OBQLOOP(Q,Q,wt,x=*u, x=1==QCOMP(x,*u)? x:*u); break;
  case OBQCASE(RATX, CMIN    ): OBQLOOP(Q,Q,wt,x=*u, x=1==QCOMP(x,*u)?*u: x); break;
  case OBQCASE(RATX, CPLUS   ): OBQLOOP(Q,Q,wt,x=*u, x=qplus(x,*u)); break;
  case OBQCASE(INTX, CBW0001 ): OBQLOOP(I,I,wt,x=*u, x&=*u        ); break;
  case OBQCASE(INTX, CBW0110 ): OBQLOOP(I,I,wt,x=*u, x^=*u        ); break;
  case OBQCASE(INTX, CBW0111 ): OBQLOOP(I,I,wt,x=*u, x|=*u        ); break;
  case OBQCASE(INTX, CMAX    ): OBQLOOP(I,I,wt,x=*u, x=MAX(x,*u)  ); break;
  case OBQCASE(INTX, CMIN    ): OBQLOOP(I,I,wt,x=*u, x=MIN(x,*u)  ); break;
  case OBQCASE(INTX, CPLUS   ): 
   er=0; OBQLOOP(I,I,wt,x=*u, {p=0>x; x+=*u; BOV(p==0>*u&&p!=0>x);}); 
   if(er>=EWOV)OBQLOOP(I,D,FL,x=(D)*u, x+=*u);
 }
 if(wt&FL+CMPX)NAN1; RE(0);
 return b?z:oblique(w,self);
}    /* f//.y for atomic f */


#define TYMESF(x)       {LD t=*u--*(LD)*v++; x=(I)t; BOV(t<IMIN||IMAX<t);}
#define ACCUMF          {B p;I y; TYMESF(y); p=0>x; x+=y; BOV(p==0>y&&p!=0>x);}

#define PMCASE(t,c,d)   (65536*(c)+256*(d)+(t))

#define PMLOOP(Tw,Tz,zt,expr0,expr)  \
 {Tw*aa=(Tw*)av,* RESTRICT u,* RESTRICT v,*ww=(Tw*)wv;Tz x,* RESTRICT zv;  \
  b=1; GATVS(z,zt,zn,1,0,zt##SIZE,GACOPYSHAPE0,return 0); zv=(Tz*)AV(z);       \
  for(i=0;i<zn;++i){                         \
   j=MIN(i,m1); u=aa+j; v=ww+i-j;            \
   p=MIN(1+i,zn-i); p=MIN(p,k);              \
   expr0; DQ(p-1, expr;); *zv++=x;           \
 }}

DF2(jtpolymult){A f,g,z;B b=0;C*av,c,d,*wv;I at,i,j,k,m,m1,n,p,t,wt,zn;V*v;
 ARGCHK3(a,w,self);
 ASSERT(!((AT(a)|AT(w))&SPARSE),EVNONCE);
 m=AN(a); n=AN(w); m1=m-1; zn=m+n-1; k=MIN(m,n);
 at=AT(a); wt=AT(w); t=maxtyped(at,wt);
 if(TYPESNE(t,at))RZ(a=cvt(t,a)); at=AT(a); av=CAV(a);
 if(TYPESNE(t,wt))RZ(w=cvt(t,w)); wt=AT(w); wv=CAV(w);
 v=FAV(self);  // f//. @ (g/)
 f=v->fgh[0]; g=v->fgh[1];
 c=FAV(FAV(FAV(f)->fgh[0])->fgh[0])->id;   // id of f     f//. f/ f
 d=FAV(FAV(g)->fgh[0])->id;   // id of g     g/ g
 if((-m&(AR(a)-2)&-n&(AR(w)-2))>=0)return obqfslash(df2(z,a,w,g),f);  // if empty, or not atoms/lists, do general code.  Never happens.
 // from here on polymult on nonempty lists
 if(t&FL+CMPX)NAN0;
 switch(PMCASE(CTTZ(t),c,d)){
  case PMCASE(B01X, CNE,    CMAX    ): 
  case PMCASE(B01X, CNE,    CPLUSDOT): PMLOOP(B,B,B01,  x=*u--|*v++, x^=*u--|*v++); break;
  case PMCASE(B01X, CNE,    CSTAR   ): 
  case PMCASE(B01X, CNE,    CMIN    ): 
  case PMCASE(B01X, CNE,    CSTARDOT): PMLOOP(B,B,B01,  x=*u--&*v++, x^=*u--&*v++); break;
  case PMCASE(B01X, CPLUS,  CMAX    ): 
  case PMCASE(B01X, CPLUS,  CPLUSDOT): PMLOOP(B,I,INT,  x=*u--|*v++, x+=*u--|*v++); break;
  case PMCASE(B01X, CPLUS,  CSTAR   ): 
  case PMCASE(B01X, CPLUS,  CMIN    ): 
  case PMCASE(B01X, CPLUS,  CSTARDOT): PMLOOP(B,I,INT,  x=*u--&*v++, x+=*u--&*v++); break;
  case PMCASE(FLX,  CPLUS,  CSTAR   ): PMLOOP(D,D,FL,   x=*u--**v++, x+=*u--**v++); break;
  case PMCASE(CMPXX,CPLUS,  CSTAR   ): PMLOOP(Z,Z,CMPX, x=ztymes(*u--,*v++), x=zplus(x,ztymes(*u--,*v++))); break;
  case PMCASE(XNUMX,CPLUS,  CSTAR   ): PMLOOP(X,X,XNUM, x=xtymes(*u--,*v++), x=xplus(x,xtymes(*u--,*v++))); break;
  case PMCASE(RATX, CPLUS,  CSTAR   ): PMLOOP(Q,Q,RAT,  x=qtymes(*u--,*v++), x=qplus(x,qtymes(*u--,*v++))); break;
  case PMCASE(INTX, CBW0110,CBW0001 ): PMLOOP(I,I,INT,  x=*u--&*v++, x^=*u--&*v++); break;
  case PMCASE(INTX, CPLUS,  CSTAR   ): 
/*
   er=0; PMLOOP(I,I,INT, TYMESF(x), ACCUMF);
*/
  // here for +//.@(*/)
  {A a1,y;I*aa,i,*u,*ww=(I*)wv,*v,*yv,*zv;VA2 adocv; VARPS adocvsum;
   b=1;
   adocv=var(ds(CSTAR),at,wt); varps(adocvsum,FAV(f)->fgh[0],wt,0);  // get sum routine from f/, which is +/
   GATV0(a1,INT,m,1); aa=AV(a1); u=m+(I*)av; DO(m, aa[i]=*--u;);
   GATV0(y,INT,MIN(m,n),1); yv=AV(y);
   GATV0(z,INT,zn,1); zv=AV(z);
   for(i=0;i<zn;++i){
    j=MIN(i,m1); u=aa+m1-j; v=ww+i-j;
    p=MIN(1+i,zn-i); p=MIN(p,k);
    I rc=((AHDR2FN*)adocv.f)((I)1,p,u,v,yv,jt); if(rc&255)jsignal(rc); if(255&(rc=((AHDRRFN*)adocvsum.f)((I)1,p,(I)1,yv,zv,jt)))jsignal(rc);
    ++zv;
   }
   if(EWOV<=jt->jerr){RESETERR; PMLOOP(I,D,FL, x=*u--*(D)*v++, x+=*u--*(D)*v++);}  // erroneous fa(z) removed; any error >= EWOV will be an overflow
  break;
  }
 }
 if(t&FL+CMPX)NAN1; RE(0);
 if(!b)return obqfslash(df2(z,a,w,g),f);
 return z;
}    /* f//.@(g/) for atomic f, g */

// start of x f/. y (Key)
static DF2(jtkey);

static DF2(jtkeysp){PROLOG(0008);A b,by,e,q,x,y,z;I j,k,n,*u,*v;P*p;
 ARGCHK2(a,w);
 {I t2; ASSERT(SETIC(a,n)==SETIC(w,t2),EVLENGTH);}  // verify agreement.  n is # items of a
 RZ(q=indexof(a,a)); p=PAV(q); 
 x=SPA(p,x); u=AV(x);
 y=SPA(p,i); v=AV(y);
 e=SPA(p,e); k=i0(e); 
 j=0; DO(AN(x), if(k<=u[i])break; if(u[i]==v[i])++j;);
 RZ(b=ne(e,x));
 RZ(by=repeat(b,y));
 RZ(x=key(repeat(b,x),from(ravel(by),w),self));
 GASPARSE(q,SB01,1,1,(I*)0); *AS(q)=n;  /* q=: 0 by}1$.n;0;1 */
 p=PAV(q); SPB(p,a,iv0); SPB(p,e,num(1)); SPB(p,i,by); SPB(p,x,reshape(tally(by),num(0)));
 RZ(z=over(df1(b,repeat(q,w),VAV(self)->fgh[0]),x));
 z=j?cdot2(box(IX(1+j)),z):z;
 EPILOG(z);
}

// a u/. w.  Self-classify a, then rearrange w and call cut.  Includes special cases for f//.
static DF2(jtkey){F2PREFIP;PROLOG(0009);A ai,z=0;I nitems;
 ARGCHK2(a,w);
 if((SPARSE&AT(a))!=0)return keysp(a,w,self);  // if sparse, go handle it
 {I t2; ASSERT(SETIC(a,nitems)==SETIC(w,t2),EVLENGTH);}  // verify agreement.  nitems is # items of a
 RZ(ai=indexofsub(IFORKEY,a,a));   // self-classify the input using ct set before this verb
 // indexofsub has 2 returns: most of the time, it returns a normal i.-family result, but with each slot holding the index PLUS the number of values
 // mapped to that index.  If processing determines that small-range lookup would be best, indexofsub doesn't do it, but instead returns a block giving the size, min value, and range.
 // We then allocate and run the small-range table and use it to rearrange the input.  The small-range variant is signaled by the LSB of the result
 // of indexofsub being set.
 PUSHCCT(jt->cctdefault);  // now that partitioning is over, reset ct for the executions of u
 I cellatoms; PROD(cellatoms,AR(w)-1,AS(w)+1);   // length of a cell of w, in atoms
 // if this is a supported f//., handle it without calling cut.  We can take it if the flag says f//. and the rank is >1 or the type is one we can do cheaply: B01/INT/FL 
 if(SZI==SZD&&FAV(self)->flag&VFKEYSLASHT){  // f//. where f is + >. <. mean   Implementation requires SZI==SZD
  // start of f//. on special cases
  if((((AR(w)-2)|((AT(w)&CMPX+XNUM+RAT)-1))&((AT(w)&FAV(self)->flag&VFKEYSLASHT)-1))>=0){  // rank>=2 and extended, or type we can handle locally  flag is B01+INT+FL for <. >., B01+FL for +  (we don't handle int ovfl)
   // We are going to handle the //. locally
   A freq;  // for mean, save frequencies here
   I keyslashfn=(FAV(self)->flag>>VFKEYSLASHFX)&3; // 0-3 for <. >. + mean
   // Figure out the type of result, and which routine will handle the operation
   I ztoride=(0x0e010000>>((4*keyslashfn+((AT(w)>>INTX)&3))<<1))&3;  // z type override. 00=none, 01=B to I 10=B to FL 11=I to FL  bits are opFI, values are (0) 0 3 2 (0) 0 x 1 (0) 0 0 0 (0) 0 0 0
   ztoride=AT(w)&FL+INT+B01?ztoride:0;  // no override for other types
   I zt=AT(w)^(((((INT+FL)<<12)+((B01+FL)<<8)+((B01+INT)<<4))>>(ztoride<<2))&0xf);  // switch result precision as needed
   I celllen = cellatoms<<bplg(zt);  // length of a cell of z, in bytes
#define RTNCASE(r,t) ((r)*4+(((t)>>INTX)&3))
   I routineid; VA2 adocv;   // case index to use 
   if(AR(w)<=1){
    // partitions have rank 1, so operations are on atoms.  set the index
    routineid=RTNCASE(keyslashfn,zt) ^ ztoride; // min, max are normal.  + is B01 (BtoI) or FL.  mean is B01 (BtoF), INT (ItoF), or FL
   }else{
    // partitions are arrays.  Operate on them in the output area
    routineid=RTNCASE(ztoride,INT+FL);  // special case index for action routine: 3 for normal (for <. >.), 7 for BtoI (for +), 11 for BtoF (for mean), 15 for ItoF (for mean) 17 = FL mean 16 for other mean e. g. INTX
    I meanrtn=16+((zt>>FLX)&1);  // 17 if FL result, 16 if not
    routineid=keyslashfn==routineid?meanrtn:routineid;  // 'other mean': only way they can be = is if both are 3.  Switch to 16/17, which differ in type of freq result
    A accfn=FAV(FAV(self)->fgh[0])->fgh[0]; accfn=keyslashfn==3?ds(CPLUS):accfn;
    adocv=var(accfn,AT(w),zt);  // get dyadic action routine for f out of f//. or (+/%#)/. for the given arguments
   }
   
   if(!((I)ai&1)){
    // not smallrange processing.  ai has combined fret/frequency information
    I nfrets=AM(ai);  // before we possibly clone it, extract # frets found
    makewritable(ai);  // we modify the size+index info to be running endptrs into the reorder area
    // allocate the result area(s)
    GA(z,zt,nfrets*cellatoms,AR(w),AS(w)); AS(z)[0]=nfrets; if(AN(z)==0)return z;  // avoid calls with empty args
    if(keyslashfn==3){
     GA(freq,zt&FL?FL:INT,nfrets,1,0);  // allocate place for divisor - INT if result may be XNUM/RAT
    }
    // loop through the index, copying the first value and operating on the others
    I indexx, indexn;  // index into the ai data, length of ai data
    I *ai0=IAV(ai);  // base of index table
    void *wv;  // current argument item

#define KSLGLP(wincr,zincr,pwvinit,pwvupd) \
 {void *nextpartition=voidAV(z);  /* place where next partition will be started */ \
 for(indexx=0, indexn=AN(ai), wv=voidAV(w); indexx!=indexn;++indexx, wv=(C*)wv+(wincr)){ \
     I nextx=ai0[indexx];  /* result of classifier for next item */ \
     if(nextx>=indexx){ \
      /* new partition.  Allocate it sequentially, move the data to be the initial result value, and replace the index with the pointer to the partition */ \
      ai0[indexx]=(I)nextpartition;  /* save start point for partition */ \
      void *partition=nextpartition; \
      pwvinit  /* move the data in */ \
      nextpartition=(C*)nextpartition+(zincr);  /* step up to next output position */ \
     }else{ \
      /* continuing a partition.  Point to the data and operate on it */ \
      void *partition=(void *)ai0[nextx];  /* partition we are extending */ \
      pwvupd \
     } \
    } \
 }
#define KSLGLPMEAN(wincr,zincr,pwvinit,pwvupd,freqt) \
 {I nextpartition=0;  /* index where next partition will be started */ \
 freqt *fv0=(freqt *)voidAV(freq); \
 void *zv0=voidAV(z); \
 for(indexx=0, indexn=AN(ai), wv=voidAV(w); indexx!=indexn;++indexx, wv=(C*)wv+(wincr)){ \
     I nextx=ai0[indexx];  /* result of classifier for next item */ \
     if(nextx>=indexx){ \
      /* new partition.  Allocate it sequentially, move the data to be the initial result value, and replace the index with the pointer to the partition */ \
      ai0[indexx]=nextpartition;  /* index of partition */ \
      void *partition=(void *)((C*)zv0+nextpartition*(zincr));  /* partition we are extending */ \
      pwvinit  /* move the data in */ \
      fv0[nextpartition]=1; /* init freq to 1 */ \
      nextpartition++;  /* step up to next output position */ \
     }else{ \
      /* continuing a partition.  Point to the data and operate on it */ \
      fv0[ai0[nextx]]++; \
      void *partition=(void *)((C*)zv0+ai0[nextx]*(zincr));  /* partition we are extending */ \
      pwvupd \
     } \
    } \
 }
    switch(routineid){
    case 3: {I rc; KSLGLP(celllen,celllen,MC(partition,wv,celllen);,ASSERT(EVOK==(rc=((AHDR2FN*)adocv.f)(1,cellatoms,wv,partition,partition,jt)),rc)) break;}  // <. >.
    case 7: {I rc; KSLGLP(cellatoms,celllen,DO(cellatoms, ((I*)partition)[i]=((C*)wv)[i];),ASSERT(EVOK==(rc=((AHDR2FN*)adocv.f)(1,cellatoms,wv,partition,partition,jt)),rc)) break;}  // + B01
    case 11: {I rc; KSLGLPMEAN(cellatoms,celllen,DO(cellatoms, ((D*)partition)[i]=((B*)wv)[i];),ASSERT(EVOK==(rc=((AHDR2FN*)adocv.f)(1,cellatoms,wv,partition,partition,jt)),rc),D) break;}  // mean B01
    case 15: {I rc; KSLGLPMEAN(celllen,celllen,DO(cellatoms, ((D*)partition)[i]=((I*)wv)[i];),ASSERT(EVOK==(rc=((AHDR2FN*)adocv.f)(1,cellatoms,wv,partition,partition,jt)),rc),D) break;}  // mean INT
    case 16: {I rc; KSLGLPMEAN(celllen,celllen,MC(partition,wv,celllen);,ASSERT(EVOK==(rc=((AHDR2FN*)adocv.f)(1,cellatoms,wv,partition,partition,jt)),rc),I) break;}  // mean for XNUM/RAT
    case 17: {I rc; KSLGLPMEAN(celllen,celllen,MC(partition,wv,celllen);,ASSERT(EVOK==(rc=((AHDR2FN*)adocv.f)(1,cellatoms,wv,partition,partition,jt)),rc),D) break;}  // mean for FL
    case RTNCASE(0,B01): KSLGLP(1,1,*(C*)partition=*(C*)wv;,*(C*)partition&=*(B*)wv;) break;  // <.
    case RTNCASE(0,INT): KSLGLP(SZI,SZI,*(I*)partition=*(I*)wv;,I p0=*(I*)partition; I w0=*(I*)wv; p0=p0<w0?p0:w0;  *(I*)partition=p0;) break;  // <.
    case RTNCASE(0,FL): KSLGLP(SZD,SZD,*(D*)partition=*(D*)wv;,D *p0=partition; p0=*(D*)partition<*(D*)wv?p0:wv;*(D*)partition=*p0;) break;  // <.
    case RTNCASE(1,B01): KSLGLP(1,1,*(C*)partition=*(C*)wv;,*(C*)partition|=*(B*)wv;) break;  // <.
    case RTNCASE(1,INT): KSLGLP(SZI,SZI,*(I*)partition=*(I*)wv;,I p0=*(I*)partition; I w0=*(I*)wv; p0=p0>w0?p0:w0;  *(I*)partition=p0;) break;  // >.
    case RTNCASE(1,FL): KSLGLP(SZD,SZD,*(D*)partition=*(D*)wv;,D *p0=partition; p0=*(D*)partition>*(D*)wv?p0:wv;*(D*)partition=*p0;) break;  // >.
    case RTNCASE(2,B01): KSLGLP(1,SZI,*(I*)partition=*(B*)wv;,*(I*)partition+=*(B*)wv;) break;  // +  the input is B01 but the result is INT
    case RTNCASE(2,FL): NAN0; KSLGLP(SZD,SZD,*(D*)partition=*(D*)wv;,*(D*)partition+=*(D*)wv;) NAN1 break;
    case RTNCASE(3,B01): NAN0; KSLGLPMEAN(1,SZD,*(D*)partition=*(B*)wv;,*(D*)partition+=*(B*)wv;,D) NAN1 break;
    case RTNCASE(3,INT): NAN0; KSLGLPMEAN(SZI,SZD,*(D*)partition=*(I*)wv;,*(D*)partition+=*(I*)wv;,D) NAN1 break;
    case RTNCASE(3,FL): NAN0; KSLGLPMEAN(SZD,SZD,*(D*)partition=*(D*)wv;,*(D*)partition+=*(D*)wv;,D) NAN1 break;
    }
   }else{
    // small-range processing of f//.
    // indexofsub detected that small-range processing is in order.  Information about the range is secreted in fields of a
    ai=(A)((I)ai-1); I k=AN(ai); I datamin=AK(ai); I p=AM(ai);  // get size of an item in bytes, smallest item, range+1
    // allocate a tally area and clear it to ~0
    A ftbl; GATV0(ftbl,INT,p,1); void **ftblv=voidAV(ftbl); memset(ftblv,~0,p<<LGSZI);
    // pass through the inputs, setting ftblv to 0 if there is a partition, and counting the number of partitions
    I valmsk=(UI)~0LL>>(((-k)&(SZI-1))<<LGBB);  // mask to leave the k lowest bytes valid
    ftblv-=datamin;  // bias starting addr so that values hit the table
    I nparts=0;  // number of partitions in the result
    I *av=IAV(a); DQ(nitems, void * of=ftblv[*av&valmsk]; ftblv[*av&valmsk]=0; nparts+=(I)of&1; av=(I*)((I)av+k);)   // count partitions and set pointers there to 0
    // allocate the result area(s)
    GA(z,zt,nparts*cellatoms,AR(w),AS(w)); AS(z)[0]=nparts; if(AN(z)==0)return z;  // avoid calls with empty args
    if(keyslashfn==3){
     GA(freq,zt&FL?FL:INT,nparts,1,0);  // allocate place for divisor - INT if result may be XNUM/RAT
    }
    // pass through the inputs again, accumulating the result.  First time we encounter a partition, initialize its pointer and the result cell
    // loop through the index, copying the first value and operating on the others
    void *nextpartition=voidAV(z);  // place where next partition will be started
    void *wv;  // current argument item

#define KSLSRLP(wincr,zincr,pwvinit,pwvupd) \
 {void *nextpartition=voidAV(z);  /* place where next partition will be started */ \
 for(av=IAV(a), wv=voidAV(w); nitems; av=(I*)((I)av+k), wv=(C*)wv+(wincr),--nitems){ \
     void *partition=ftblv[*av&valmsk];  /* partition address, or 0 if not allocated */ \
     if(!partition){ \
      /* new partition.  Allocate it sequentially, move the data to be the initial result value, and replace the index with the pointer to the partition */ \
      ftblv[*av&valmsk]=nextpartition;  /* save start point for partition */ \
      partition = nextpartition; \
      pwvinit  /* move the data in */ \
      nextpartition=(C*)nextpartition+(zincr);  /* step up to next output position */ \
     }else{ \
      /* continuing a partition.  Point to the data and operate on it */ \
      pwvupd \
     } \
    } \
 }
#define KSLSRLPMEAN(wincr,zincr,pwvinit,pwvupd,freqt) \
 {I nextpartition=1;  /* index where next partition will be started - can't be 0 */ \
 freqt *fv0=(freqt *)(CAV(freq)-SZI);  /* bias to match nextpartition */ \
 void *zv0=(void *)(CAV(z)-(zincr)); \
 for(av=IAV(a), wv=voidAV(w); nitems; av=(I*)((I)av+k), wv=(C*)wv+(wincr),--nitems){ \
     I partitionx=(I)ftblv[*av&valmsk];  /* partition index, or 0 if not allocated */ \
     if(!partitionx){ \
      /* new partition.  Allocate it sequentially, move the data to be the initial result value, and replace the index with the pointer to the partition */ \
      partitionx=nextpartition; \
      ftblv[*av&valmsk]=(void *)partitionx;  /* save start point for partition */ \
      fv0[partitionx]=1; \
      void *partition=(void *)((C*)zv0+partitionx*(zincr));  /* partition we are extending */ \
      pwvinit  /* move the data in */ \
      ++nextpartition;  /* step up to next output position */ \
     }else{ \
      /* continuing a partition.  Point to the data and operate on it */ \
      fv0[partitionx]++; \
      void *partition=(void *)((C*)zv0+partitionx*(zincr));  /* partition we are extending */ \
      pwvupd \
     } \
    } \
 }

    switch(routineid){
    case 3: {I rc; KSLSRLP(celllen,celllen,MC(partition,wv,celllen);,ASSERT(EVOK==(rc=((AHDR2FN*)adocv.f)(1,cellatoms,wv,partition,partition,jt)),rc)) break;}  // <. >.
    case 7: {I rc; KSLSRLP(cellatoms,celllen,DO(cellatoms, ((I*)partition)[i]=((C*)wv)[i];),ASSERT(EVOK==(rc=((AHDR2FN*)adocv.f)(1,cellatoms,wv,partition,partition,jt)),rc)) break;}  // + B01
    case 11: {I rc; KSLSRLPMEAN(cellatoms,celllen,DO(cellatoms, ((D*)partition)[i]=((B*)wv)[i];),ASSERT(EVOK==(rc=((AHDR2FN*)adocv.f)(1,cellatoms,wv,partition,partition,jt)),rc),D) break;}  // mean B01
    case 15: {I rc; KSLSRLPMEAN(celllen,celllen,DO(cellatoms, ((D*)partition)[i]=((I*)wv)[i];),ASSERT(EVOK==(rc=((AHDR2FN*)adocv.f)(1,cellatoms,wv,partition,partition,jt)),rc),D) break;}  // mean INT
    case 16: {I rc; KSLSRLPMEAN(celllen,celllen,MC(partition,wv,celllen);,ASSERT(EVOK==(rc=((AHDR2FN*)adocv.f)(1,cellatoms,wv,partition,partition,jt)),rc),I) break;}  // mean for XNUM/RAT
    case 17: {I rc; KSLSRLPMEAN(celllen,celllen,MC(partition,wv,celllen);,ASSERT(EVOK==(rc=((AHDR2FN*)adocv.f)(1,cellatoms,wv,partition,partition,jt)),rc),D) break;}  // mean for FL
    case RTNCASE(0,B01): KSLSRLP(1,1,*(C*)partition=*(C*)wv;,*(C*)partition&=*(B*)wv;) break;  // <.
    case RTNCASE(0,INT): KSLSRLP(SZI,SZI,*(I*)partition=*(I*)wv;,I p0=*(I*)partition; I w0=*(I*)wv; p0=p0<w0?p0:w0;  *(I*)partition=p0;) break;  // <.
    case RTNCASE(0,FL): KSLSRLP(SZD,SZD,*(D*)partition=*(D*)wv;,D *p0=partition; p0=*(D*)partition<*(D*)wv?p0:wv;*(D*)partition=*p0;) break;  // <.
    case RTNCASE(1,B01): KSLSRLP(1,1,*(C*)partition=*(C*)wv;,*(C*)partition|=*(B*)wv;) break;  // <.
    case RTNCASE(1,INT): KSLSRLP(SZI,SZI,*(I*)partition=*(I*)wv;,I p0=*(I*)partition; I w0=*(I*)wv; p0=p0>w0?p0:w0;  *(I*)partition=p0;) break;  // >.
    case RTNCASE(1,FL): KSLSRLP(SZD,SZD,*(D*)partition=*(D*)wv;,D *p0=partition; p0=*(D*)partition>*(D*)wv?p0:wv;*(D*)partition=*p0;) break;  // >.
    case RTNCASE(2,B01): KSLSRLP(1,SZD,*(I*)partition=*(B*)wv;,*(I*)partition+=*(B*)wv;) break;  // +  the input is B01 but the result is INT
    case RTNCASE(2,FL): NAN0; KSLSRLP(SZD,SZD,*(D*)partition=*(D*)wv;,*(D*)partition+=*(D*)wv;) NAN1 break;
    case RTNCASE(3,B01): NAN0; KSLSRLPMEAN(1,SZD,*(D*)partition=*(B*)wv;,*(D*)partition+=*(B*)wv;,D) NAN1 break;
    case RTNCASE(3,INT): NAN0; KSLSRLPMEAN(SZI,SZD,*(D*)partition=*(I*)wv;,*(D*)partition+=*(I*)wv;,D) NAN1 break;
    case RTNCASE(3,FL): NAN0; KSLSRLPMEAN(SZD,SZD,*(D*)partition=*(D*)wv;,*(D*)partition+=*(D*)wv;,D) NAN1 break;
    }
   }
   // Finally, if this was mean, divide the total by the frequency
   if(((FAV(self)->flag&VFKEYSLASHF)>>VFKEYSLASHFX)==3){z=divideAW(z,freq);}  // always inplaceable
   EPILOG(z);
  }  // end 'locally handled arg types'
 }

 // ** Here it wasn't fspecial//. - reorder the input and create frets
 // Allocate the area for the reordered copy of the input.  Do these calls early to free up registers for the main loop
 A frets,wperm;  // fret block, and place to reorder w
 UC *fretp;  // where the frets will be stored
 GA(wperm,AT(w),AN(w),AR(w),AS(w)); // Note we could avoid initialization of indirect types, since we are filling it all
 I celllen = cellatoms<<bplg(AT(w));  // length of a cell of w, in bytes
 JMCDECL(endmask) JMCSETMASK(endmask,celllen,1)   // set mask for JMCR
 // Once we know the number of frets, we can allocate the fret area.  If the number is small, we can use the canned area on the C stack.
 // The max # bytes needed is 4*((max # partitions of size>=256) clamped at # frets) + #frets
 // If y is inplaceable we can probably use it to store the frets, since it is copied sequentially.  The requirements are:
 // The fret area must start at least 5 bytes before the data in y, to leave space for the first fret before the first cell of y is moved (i. e. rank>0 for 64-bit, >1 for 32-bit)
 // The cells must be at least 5 bytes long so that the frets don't overrun them
 // The cells must be in the actual fret area (following the shape), i. e not virtual or NJA 
 // The cells must be DIRECT type so that it is OK to write garbage over them
 // We are taking advantage here of the fact that the shape comes before the cells in a standard GA noun
 // We don't have to worry about a==w because a has already been modified

 I nfrets;  // we get the # frets early and use that for allocation
 I localfrets[32];  // if we don't have too many frets, we can put them on the C stack
 if(!((I)ai&1)){
  // NOT small-range processing: go through the index+size table to create the frets and reordered data for passing to cut
  nfrets=AM(ai);  //fetch # frets before we possibly clone ai
  I maxfretsize=(nitems>>8); maxfretsize=maxfretsize<nfrets?nfrets:maxfretsize; maxfretsize=4*maxfretsize+nfrets+1;  // max # bytes needed for frets, if some are long
  if((UI)maxfretsize<sizeof(localfrets)-NORMAH*SZI){frets=(A)localfrets; AT(frets)=0; } // Cut tests the type field - only.  If debug, the flag also
  else if((I)jtinplace&(I)((AFLAG(w)&(AFVIRTUAL|AFNJA))==0)&((UI)((-(I )(AT(w)&DIRECT))&AC(w)&(4-celllen)&((I )(SZI==4)-AR(w)))>>(BW-1-JTINPLACEWX)))frets=w;
  else GATV0(frets,LIT,maxfretsize,0);   // 1 byte per fret is adequate, since we have padding
  fretp=CUTFRETFRETS(frets);  // Place where we will store the fret-lengths.  They are 1 byte normally, or 5 bytes for groups longer than 254

  makewritable(ai);  // we modify the size+index info to be running endptrs into the reorder area
  // pass through the input, incrementing each reference
  I *av=IAV(ai);  // av->a data
  // Now each item av[i] is either (1) smaller than i, which means that it is extending a previous key; or (2) greater than i, which
  // means it starts a new partition whose length is a[i]-i.  Process the values in order, creating partitions as they come up, and
  // moving the data for each input value in turn, reading in order and scatter-writing.
  I i; I nextpartitionx;  // loop index, address of place to store next partition
  I * RESTRICT wv=IAV(w);   // source & target pointers
  for(i=0, nextpartitionx=(I)IAV(wperm);i<nitems;++i){
   I * RESTRICT partitionptr;  // pointer to where output will go
   I avvalue=av[i];  // fetch partition length/index
   if(avvalue<i){  // this value extends its partition
    partitionptr=(I*)av[avvalue];  // addr of end of selected partition
   }else{
    // start of new partition.  Figure out the length; out new partition; replace length with starting pointer; Use length to advance partition pointer
    avvalue-=i;  // length of partition
    if(avvalue<255)*fretp++ = (UC)avvalue; else{*fretp++ = 255; *(UI4*)fretp=(UI4)avvalue; fretp+=SZUI4;}
    partitionptr=(I*)nextpartitionx;  // copy this item's data to the start of the partition
    nextpartitionx+=avvalue*celllen;  // reserve output space for the partition
    avvalue=i;   // shift meaning of avvalue from length to index, where the partition pointer will be stored
   }

   av[avvalue]=(I)partitionptr+celllen;  // store updated end-of-partition after move
   JMCR(partitionptr,wv,celllen,loop1,1,endmask); wv = (I*)((C*)wv+celllen);  // Don't overwrite, since we are scatter-writing

  }
 }else{I *av;  // running pointer through the inputs
  // indexofsub detected that small-range processing is in order.  Information about the range is secreted in fields of a
  ai=(A)((I)ai-1); I k=AN(ai); I datamin=AK(ai); I p=AM(ai);  // get size of an item, smallest item, range+1
  // allocate a tally area and clear it.  Could use narrower table perhaps
  A ftbl; GATV0(ftbl,INT,p,1); I *ftblv=IAV(ftbl); memset(ftblv,0,p<<LGSZI);
  // pass through the inputs, counting the negative of the number of slots mapped to each index
  I valmsk=(UI)~0LL>>(((-k)&(SZI-1))<<LGBB);  // mask to leave the k lowest bytes valid
  ftblv-=datamin;  // bias starting addr so that values hit the table
  nfrets=nitems;  // initialize fret counter, decremented for each non-fret
  av=IAV(a); DQ(nitems, I tval=ftblv[*av&valmsk]; ftblv[*av&valmsk]=tval-1; nfrets-=SGNTO0(tval); av=(I*)((I)av+k);)   // build (negative) frequency table; sub 1 for each non-fret
  // now that we have the number of freats, we can allocate the fret block
  I maxfretsize=(nitems>>8); maxfretsize=maxfretsize<nfrets?nfrets:maxfretsize; maxfretsize=4*maxfretsize+nfrets+1;  // max # bytes needed for frets
  if((UI)maxfretsize<sizeof(localfrets)-NORMAH*SZI){frets=(A)localfrets; AT(frets)=0; } // Cut tests the type field - only; for memaudit we need flag too
  else if((I)jtinplace&(I)((AFLAG(w)&(AFVIRTUAL|AFNJA))==0)&((UI)((-(I)(AT(w)&DIRECT))&AC(w)&(4-celllen)&((I)(SZI==4)-AR(w)))>>(BW-1-JTINPLACEWX)))frets=w;
  else GATV0(frets,LIT,maxfretsize,0);   // 1 byte per fret is adequate, since we have padding
  fretp=CUTFRETFRETS(frets);  // Place where we will store the fret-lengths.  They are 1 byte normally, or 5 bytes for groups longer than 254

  // pass through the inputs again.  If we encounter a negative value, allocate the next fret.  Copy the next item and advance
  // that partition's fret pointer
  // Now each item ftblv[av[i]] is either (1) nonnegative, which means that it is extending a previous key; or (2) negative, which
  // means it starts a new partition whose length is -ftblv[av[i]].  Process the values in order, creating partitions as they come up, and
  // moving the data for each input value in turn, reading in order and scatter-writing.

  I i; I nextpartitionx;  // loop index, index of place to store next partition
  I * RESTRICT wv=IAV(w);   // source pointer
  C * RESTRICT wpermv=CAV(wperm);  // addr of output area
  for(av=IAV(a), i=nitems, nextpartitionx=0;i;--i){
   I partitionndx;  // index of where this output will go
   I *slotaddr=&ftblv[*av&valmsk];  // the slot in the table we are processing
   I avvalue=*slotaddr;  // fetch input value, mask to valid portion, fetch partition length/index
   if(avvalue>=0){  // this value extends its partition
    partitionndx=avvalue;  // index of current value in selected partition
   }else{
    // start of new partition.  Figure out the length; out new partition; replace length with starting pointer; Use length to advance partition pointer
    avvalue= -avvalue;  // length of partition
    if(avvalue<255)*fretp++ = (UC)avvalue; else{*fretp++ = 255; *(UI4*)fretp=(UI4)avvalue; fretp+=SZUI4;}
    partitionndx=nextpartitionx;  // copy this item's data to the start of the partition
    nextpartitionx+=avvalue;  // reserve output space for the partition
   }

   I *partitionptr=(I*)(wpermv+partitionndx*celllen);  // place to copy next input to
   *slotaddr=partitionndx+1;  // store updated next-in-partition after move
   // copy the data to the end of its partition and advance the input pointer
   JMCR(partitionptr,wv,celllen,loop2,1,endmask); wv = (I*)((C*)wv+celllen);  // Don't overwrite, since we are scatter-writing

   av=(I*)((I)av+k);  // advance to next input value
  }
 }

 // Frets are calculated and w is reordered.  Call cut to finish the job.  We have to store the count and length of the frets
 CUTFRETCOUNT(frets)=nfrets;  // # frets is #bytes stored, minus the length of the extended encodings
 CUTFRETEND(frets)=(I)fretp;   // pointer to end+1 of data
 // wperm is always inplaceable.  If u is inplaceable, make the call to cut inplaceable
 // Transfer pristinity of w to wperm so we can see if it went away, but only if w is pristine inplaceable.  We want to leave
 // wperm pristine so it can be used, but it's pristine only is w is zombie.  We sacrifice pristinity to inplaceability
 AFLAG(wperm)|=AFLAG(w)&AFPRISTINE&REPSGN(AC(w)&SGNIF(jtinplace,JTINPLACEWX));
 // We pass the self pointer for /. into cut, as it uses the id therein to interpret a
 z=jtcut2((J)(intptr_t)((I)jt+((FAV(self)->flag&VGERL)?0:(FAV(FAV(self)->fgh[0])->flag>>(VJTFLGOK1X-JTINPLACEWX))&JTINPLACEW)),frets,wperm,self);
 // If the operation turned off pristinity of wperm, do the same for w.  Remember that pristinity only matters if the block is inplaceable
 AFLAG(w)&=AFLAG(wperm)|~AFPRISTINE;
 POPCCT
 EPILOG(z);
}    /* a f/. w for dense x & w */

// bivalent entry point: a </. w   or  (</. i.@#) w
DF2(jtkeybox){F2PREFIP;PROLOG(0009);A ai,z=0;I nitems;
 ARGCHK2(a,w);  // we don't neep ip, but all jtkey dyads must support it
 if((SPARSE&AT(a))!=0)return (AT(w)&NOUN?(AF)jtkeysp:(AF)jthook1cell)(jt,a,w,self);  // if sparse, go handle it
 SETIC(a,nitems);   // nitems is # items in a and w
 I cellatoms, celllen;  // number of atoms in an item of w, and the number of bytes therein.  celllen is negative for the monad
 struct AD fauxw;  // needed only for (</. i.@#) but must stay in scope
 if((AT(w)&NOUN)!=0){
  // dyad: </.
  I t2; ASSERT(nitems==SETIC(w,t2),EVLENGTH);  // verify agreement
  PROD(cellatoms,AR(w)-1,AS(w)+1);   // length of a cell of w, in atoms
  celllen=cellatoms<<bplg(AT(w));  // length of a cell of w, in bytes
 }else{
  // monad: (<./ i.@#) .  Create a suitable w and switch to it
  cellatoms=1; celllen=-1;  // len and flag of cells from the synthetic index vector
  // Fill in enough of fauxw to stand in for the vector.  We need type=INT and rank=1, and the shape needs to be a fetchable address but the value doesn't matter.
  // The data address returned by IAV needs to be 0
  w=&fauxw;  // switch to synthetic w
  AK(w)=-(I)w; AT(w)=INT; AR(w)=1;
 }
 JMCDECL(endmask) JMCSETMASK(endmask,celllen,1)   // set mask for JMCR.  Harmless if celllen=-1

 // Note: self is invalid from here on
 RZ(ai=indexofsub(IFORKEY,a,a));   // self-classify the input using ct set before this verb
 // indexofsub has 2 returns: most of the time, it returns a normal i.-family result, but with each slot holding the index PLUS the number of values
 // mapped to that index.  If processing determines that small-range lookup would be best, indexofsub doesn't do it, but instead returns a block giving the size, min value, and range.
 // We then allocate and run the small-range table and use it to rearrange the input.  The small-range variant is signaled by the LSB of the result
 // of indexofsub being set.
 PUSHCCT(jt->cctdefault);  // now that partitioning is over, reset ct for the executions of u

 A *pushxsave;  // place to save the tpop stack when we hijack it
  A y;  // name under which boxes are allocated
 if(!((I)ai&1)){
  // NOT small-range processing: go through the index+size table to create the frets and reordered data for passing to cut
  I nboxes=AM(ai);  //fetch # frets before we possibly clone ai
  makewritable(ai);  // we modify the size+index info to be running endptrs into the reorder area
  // allocate the result, which will be recursive, and set up to fill it with blocks off the tstack
  GATV0(z,BOX,nboxes,1); if(nboxes==0){return z;} // allocate result, and exit if empty for comp ease below
  // boxes will be in AAV(z), in order.  Details of hijacking tnextpushp are discussed in jtbox().
  // We have to do it a little differently here, because when we allocate a block the contents come in piecemeal and we have no
  // convenient time to INCORPRA the new contents.  So instead we leave the outer box non-recursive, and rely on the EPILOG to make
  // it recursive, including recurring on the contents of the boxes.  To make this work we install an AC of 0 in the allocated
  // boxes, because they have no tpop outstanding.  The EPILOG will raise that to 1.
  pushxsave = jt->tnextpushp; jt->tnextpushp=AAV(z);  // save tstack info before allocation
  // **** MUST NOT FAIL FROM HERE UNTIL THE END, WHERE THE ALLOCATION SYSTEM CAN BE RESTORED ****

  // pass through the input, incrementing each reference
  I *av=IAV(ai);  // av->a data
  // Now each item av[i] is either (1) smaller than i, which means that it is extending a previous key; or (2) greater than i, which
  // means it starts a new partition whose length is a[i]-i.  Process the values in order, creating partitions as they come up, and
  // moving the data for each input value in turn, reading in order and scatter-writing.
  I i;   // loop index
  I * RESTRICT wv=IAV(w);   // source pointer, advanced item by item
  I wt=AT(w); I yr=MAX(AR(w),1);   // type of cells, and rank
  for(i=0;i<nitems;++i){
   I * RESTRICT partitionptr;  // pointer to where output will go
   I avvalue=av[i];  // fetch partition length/index
   if(avvalue<i){  // this value extends its partition
    partitionptr=(I*)av[avvalue];  // addr of end of selected partition
   }else{
    // start of new partition.  Figure out the length; out new partition; replace length with starting pointer; Use length to advance partition pointer
    avvalue-=i;  // length of partition
    GAE(y,wt,cellatoms*avvalue,yr,AS(w),break); AC(y)=0; AS(y)[0]=avvalue; // allocate a region for the boxed data and set usecount to 0 since it is not on the tstack.  EPILOG will raise it to 1
    partitionptr=IAV(y);  // start of partition: in the data area of the block
    avvalue=i;   // shift meaning of avvalue from length to index, where the partition pointer will be stored
   }

   if(celllen>=0) {
     av[avvalue]=(I)partitionptr+celllen;  // store updated end-of-partition after move
     // copy the data to the end of its partition and advance the partition pointer
    JMCR(partitionptr,wv,celllen,loop3,1,endmask); wv = (I*)((C*)wv+celllen);  // Don't overwrite, since we are scatter-writing

   }else{  // flag for (<./ i.@#)
     *partitionptr=(I)wv;   // store wv itself, which is the index vector
     wv=(I*)((I)wv+1);  // advance index vector
     av[avvalue]=(I)partitionptr+SZI;  // store updated end-of-partition after move
   }
  }
 }else{I *av;  // running pointer through the inputs
  // indexofsub detected that small-range processing is in order.  Information about the range is secreted in fields of ai
  ai=(A)((I)ai-1); I k=AN(ai); I datamin=AK(ai); I p=AM(ai);  // get size of an item, smallest item, range+1
  // allocate a tally area and clear it.  Could use narrower table perhaps
  A ftbl; GATV0(ftbl,INT,p,1); I *ftblv=IAV(ftbl);
  // pass through the inputs, counting the number of slots mapped to each index
  // This is tricky, because we want the slot to hold either the number of slots (i. e. size of box) or the next address to store into for the slot.
  // But the address can be anything - ah, but not quite.  The address can't be inside an allocated block, for example it can't be inside a.
  // So we initialize the table to the address of the start of the data of a, and any number in the range (a,AN(a)+a] will be known to hold a count, not a pointer
  // NOTE that we know the items of a are not empty, so the prohibited address space is big enough
  I freqminval=(I)voidAV(a)+1;  // values from here to here+nitems-1 are frequencies
  {I mm=freqminval-1; mvc(p<<LGSZI,ftblv,SZI,&mm);}  // init table to '0' values
  I valmsk=(UI)~0LL>>(((-k)&(SZI-1))<<LGBB);  // mask to leave the k lowest bytes valid
  ftblv-=datamin;  // bias starting addr so that values hit the table
  I nboxes=0;  // initialize fret counter, incremented for each fret
  av=IAV(a); DQ(nitems, I tval=ftblv[*av&valmsk]; ftblv[*av&valmsk]=++tval; nboxes+=tval==freqminval; av=(I*)((I)av+k);)   // build (negative) frequency table; sub 1 for each non-fret
  // allocate the result, which will be recursive, and set up to fill it with blocks off the tstack
  GATV0(z,BOX,nboxes,1); if(nboxes==0){return z;} // allocate result, and exit if empty for comp ease below
  // boxes will be in AAV(z), in order.  Details discussed in jtbox().
  // We will later make the block recursive usecount, and we will set all the initial usecounts to 1 since they are not on the tpop stack
  pushxsave = jt->tnextpushp; jt->tnextpushp=AAV(z);  // save tstack info before allocation
  // **** MUST NOT FAIL FROM HERE UNTIL THE END, WHERE THE ALLOCATION SYSTEM CAN BE RESTORED ****
  // pass through the inputs again.  If we encounter a frequency value, allocate the next box.  Copy the next item and advance
  // that partition's pointer

  I i; // loop index
  I * RESTRICT wv=IAV(w);   // source pointer
  I wt=AT(w); I yr=MAX(AR(w),1);   // type of cells, and rank
  for(av=IAV(a), i=nitems;i;--i){
   I *partitionptr;  // recipient of next cell
   I *slotaddr=&ftblv[*av&valmsk];  // the slot in the table we are processing
   I avvalue=*slotaddr;  // fetch partition length/pointer
   if((UI)(avvalue-freqminval)>=(UI)nitems){  // this value extends its partition
    partitionptr=(I*)avvalue;  // load into the address in the slot
   }else{
    // start of new partition.  Figure out the length; out new partition; replace length with starting pointer; Use length to advance partition pointer
    avvalue=avvalue-freqminval+1;  // length of partition
    GAE(y,wt,cellatoms*avvalue,yr,AS(w),break); AC(y)=0; AS(y)[0]=avvalue; // allocate a region for the boxed data and set usecount to 0 since it is not on the tstack.  EPILOG will raise it to 1
    partitionptr=(I*)IAV(y);  // start the data for the partition at the beginning of the allocated box's data
   }

   // copy the data to the end of its partition and advance the partition pointer
   if(celllen>=0) {
     *slotaddr=(I)partitionptr+celllen;  // store updated end-of-partition after move
     // copy the data to the end of its partition and advance the partition pointer
     JMCR(partitionptr,wv,celllen,loop4,1,endmask); wv = (I*)((C*)wv+celllen);  // Don't overwrite, since we are scatter-writing
   }else{  // flag for (<./ i.@#)
     *slotaddr=(I)partitionptr+SZI;  // store updated end-of-partition after move
     *partitionptr=(I)wv;   // store wv itself, which is the index vector
     wv=(I*)((I)wv+1);  // advance index vector
   }

   av=(I*)((I)av+k);  // advance to next input value
  }
 }
 // restore the allocation system
 jt->tnextpushp=pushxsave;   // restore tstack pointer
 // Set PRISTINE if w now has DIRECT type (note that w has been switched to INT for (<./ i.@#))
 AFLAG(z)=(-(AT(w)&DIRECT) & AFPRISTINE);  // maybe pristine
 ASSERT(y!=0,EVWSFULL);  // if we broke out on allocation failure, fail.
 POPCCT
 EPILOG(z);
}    // a <./ w

// indexed by [chartype][k]
const UI4 shortrange[3][4] = {{0,65536,65536,0}, {0,2,258,0}, {0,256,65536,0}};  // C2T, B01, LIT



static DF2(jtkeytally);

static F1(jtkeytallysp){PROLOG(0015);A b,e,q,x,y,z;I c,d,j,k,*u,*v;P*p;
 ARGCHK1(w);
 RZ(q=indexof(w,w));
 p=PAV(q); 
 x=SPA(p,x); u=AV(x); c=AN(x);
 y=SPA(p,i); v=AV(y);
 e=SPA(p,e); k=i0(e); 
 j=0; DO(c, if(k<=u[i])break; if(u[i]==v[i])++j;);
 RZ(b=ne(e,x));
 RZ(x=repeat(b,x)); RZ(x=keytally(x,x,mark)); u=AV(x); d=AN(x);
 GATV0(z,INT,1+d,1); v=AV(z);
 DQ(j, *v++=*u++;); *v++=SETIC(w,k)-bsum(c,BAV(b)); DQ(d-j, *v++=*u++;);
 EPILOG(z);
}    /* x #/.y , sparse x */

static DF2(jtkeytally){F2PREFIP;PROLOG(0016);A z,q;I at,j,k,n,r,s,*qv,*u,*v;
 ARGCHK2(a,w);  // we don't neep ip, but all jtkey dyads must support it
 SETIC(a,n); at=AT(a);
 ASSERT(n==SETIC(w,k),EVLENGTH);
 if(!AN(a))return vec(INT,!!n,&AS(a)[0]);  // handle case of empties - a must have rank, so use AS[0] as  proxy for n
 if((at&SPARSE)!=0)return keytallysp(a);
 if((-n&SGNIF(at,B01X)&(AR(a)-2))<0){B*b=BAV(a); k=bsum(n,b); return BETWEENO(k,1,n)?v2(*b?k:n-k,*b?n-k:k):vci(n);}  // nonempty rank<2 boolean a, just add the 1s
 A ai;  // result from classifying a
 RZ(ai=indexofsub(IFORKEY,a,a));   // self-classify the input using ct set before this verb
 // indexofsub has 2 returns: most of the time, it returns a normal i.-family result, but with each slot holding the index PLUS the number of values
 // mapped to that index.  If processing determines that small-range lookup would be best, indexofsub doesn't do it, but instead returns a block giving the size, min value, and range.
 // We then allocate and run the small-range table and use it to rearrange the input.  The small-range variant is signaled by the LSB of the result
 // of indexofsub being set.
 if((I)ai&1){  // if small-range
  // we should do small-range processing.  Extract the info
  ai=(A)((I)ai-1); I k=AN(ai); I datamin=AK(ai); I p=AM(ai);  // get size of an item in bytes, smallest item, range+1
  // allocate a tally area and clear it to 0
  A ftbl; GATV0(ftbl,INT,p,1); I *ftblv=voidAV(ftbl); memset(ftblv,~0,p<<LGSZI);
  // pass through the inputs, setting ftbl to tally-1, and counting the number of partitions
  I valmsk=(UI)~0LL>>(((-k)&(SZI-1))<<LGBB);  // mask to leave the k lowest bytes valid
  ftblv-=datamin;  // bias starting addr so that values hit the table
  I nparts=0;  // number of partitions in the result
  // count tally and partition
  I *av=IAV(a); DQ(n, I of=ftblv[*av&valmsk]; ftblv[*av&valmsk]=of+1; nparts+=SGNTO0(of); av=(I*)((I)av+k);)   // count partitions and set pointers there to 0
  // allocate result area
  GATV0(z,INT,nparts,1);  // output area: one per partition
  // pass the input again, copying out the values.  Use branches to avoid the long carried dependency
  av=IAV(a); I *zv=AV(z); while(1){I of=ftblv[*av&valmsk]; ftblv[*av&valmsk]=-1; if(of>=0){*zv++=of+1; if(--nparts==0)break;} av=(I*)((I)av+k);}
  EPILOG(z);
 }

 // not smallrange processing.  ai has combined fret/frequency information
 I nparts=AM(ai);  // before we possibly clone it, extract # frets found
 makewritable(ai);  // we modify the size+index info
 // allocate the result area
 GATV0(z,INT,nparts,1);   // avoid calls with empty args
 // pass through the table, writing out every value that starts a new partition.  The partition size is encoded in the value
 I i=0; I *av=IAV(ai);I *zv=IAV(z); while(1){I a0=*av++; if(a0-i>=0){*zv++=a0-i; if(--nparts==0)break;} ++i;}
 EPILOG(z);

}    /* x #/.y main control & dense x */


//  bivalent entry point for x ({.,#)/.y or x (#,{.)/. y (dyad), or (({.,#)/. i.@#) y or ((#,{.)/. i.@#) y  (monad)
DF2(jtkeyheadtally){F2PREFIP;PROLOG(0017);A f,q,x,y,z;I b;I at,*av,k,n,r,*qv,*u,*v,wt,*zv;
 ARGCHK2(a,w);  // we don't neep ip, but all jtkey dyads must support it
 SETIC(a,n); wt=AT(w);
 if((AT(w)&NOUN)!=0){
  // dyad: </.
  ASSERT(n==SETIC(w,k),EVLENGTH);  // verify agreement
  f=FAV(self)->fgh[0];  // f->({.,#)
 }else{
  // monad: (<./ i.@#)   In case of failover, set self right, and leave w pointing to self
  f=FAV(FAV(w)->fgh[0])->fgh[0];  // f->({.,#), found inside (({.,#)/. i.@#)  left of hook, then go past /.
 }
 ASSERT((-n&((wt&NUMERIC+VERB)-1))>=0,EVDOMAIN); // OK if n=0 or numeric/i.@# w
 if((((SPARSE&AT(a))-1)&((I)AR(w)-2)&(-n)&(-AN(a)))>=0)return wt&VERB?jthook1cell(jt,a,w):key(a,w,self);  // if sparse or w has rank>1 or a has no cells or no atoms, revert, to monad/dyad.  w=self for monad
 av=AV(a);
 f=FAV(f)->fgh[0]; b=CHEAD==ID(f);  // f-> left tine of (#,{.)  b is 1 for {.,#  0 for #,{.  i. e. index of tally
 if((AT(a)&B01)!=0)if(1>=AR(a)&&!(wt&VERB)){B*p=(B*)av;I i,j,m;  // first special case: boolean list/atom, for which we can handle all types (except VERB)
  B *c=memchr(p,1^*p,n); i=c-p; i=c?i:0; j=i; i=*p?0:i; j=*p?j:0;  // i=index of first 1, j=index of first 0 (0 if not found)
  k=bsum(n,p); m=i+j?1:0;  // k=# 1s  m is 1 if there are 0s and 1s
  GATV0(x,INT,m+1,1); v=AV(x); v[m]=i+j; v[0]=0;  // 0=index of first item (always 0); 1 if it exists is the other
  GATV0(y,INT,m+1,1); v=AV(y); j=n-k; k=i?j:k; k&=-m; v[0]=k; v[m]=n-k;  // if 1st value is 0, complement k; if only 1 value, clear k
  if(!(wt&VERB))RZ(x=from(x,w)); w=x; x=b?x:y; y=b?y:w; return stitch(x,y);  // select using index (unless i.@#, then keep index); set order & ,.
 }
 // for other types of a, we handle it quickly only if w is B01/INT/FL or i.@# which has type of VERB
 if(wt&B01+INT+FL+VERB){
  A ai;  // result from classifying a
  RZ(ai=indexofsub(IFORKEY,a,a));   // self-classify the input using ct set before this verb
  // indexofsub has 2 returns: most of the time, it returns a normal i.-family result, but with each slot holding the index PLUS the number of values
  // mapped to that index.  If processing determines that small-range lookup would be best, indexofsub doesn't do it, but instead returns a block giving the size, min value, and range.
  // We then allocate and run the small-range table and use it to rearrange the input.  The small-range variant is signaled by the LSB of the result
  // of indexofsub being set.
  if((I)ai&1){  // if small-range
   // we should do small-range processing.  Extract the info
   ai=(A)((I)ai-1); I k=AN(ai); I datamin=AK(ai); I p=AM(ai);  // get size of an item in bytes, smallest item, range+1
   // allocate a tally area and clear it to 0
   A ftbl; GATV0(ftbl,INT,p,1); I *ftblv=voidAV(ftbl); memset(ftblv,~0,p<<LGSZI);
   // pass through the inputs, setting ftbl to tally-1, and counting the number of partitions
   I valmsk=(UI)~0LL>>(((-k)&(SZI-1))<<LGBB);  // mask to leave the k lowest bytes valid
   ftblv-=datamin;  // bias starting addr so that values hit the table
   I nparts=0;  // number of partitions in the result
   // count tally and partition
   I *av=IAV(a); DQ(n, I of=ftblv[*av&valmsk]; ftblv[*av&valmsk]=of+1; nparts+=SGNTO0(of); av=(I*)((I)av+k);)   // count partitions and set pointers there to 0
   // allocate result area
   GA(z,MAX(wt&~VERB,INT),nparts*2,2,0); AS(z)[0]=nparts; AS(z)[1]=2;  // output area: one per partition
   // pass the input again, copying out the values.  Use branches to avoid the long carried dependency
   I i=0; av=IAV(a);   // item index and input scan pointer
   if(wt&INT){
    I *wv=IAV(w); I *zv=IAV(z); while(1){I of=ftblv[*av&valmsk]; ftblv[*av&valmsk]=-1; if(of>=0){zv[b]=of+1; zv[1-b]=wv[i]; if(--nparts==0)break; zv+=2;} av=(I*)((I)av+k); ++i;}
   }else if(wt&B01){
    B *wv=BAV(w); I *zv=IAV(z); while(1){I of=ftblv[*av&valmsk]; ftblv[*av&valmsk]=-1; if(of>=0){zv[b]=of+1; zv[1-b]=wv[i]; if(--nparts==0)break; zv+=2;} av=(I*)((I)av+k); ++i;}
   }else if(wt&FL){  // FL
    D *wv=DAV(w); D *zv=DAV(z); while(1){I of=ftblv[*av&valmsk]; ftblv[*av&valmsk]=-1; if(of>=0){zv[b]=(D)(of+1); zv[1-b]=wv[i]; if(--nparts==0)break; zv+=2;} av=(I*)((I)av+k); ++i;}
   }else{  // i.@#
    I *zv=IAV(z); while(1){I of=ftblv[*av&valmsk]; ftblv[*av&valmsk]=-1; if(of>=0){zv[b]=of+1; zv[1-b]=i; if(--nparts==0)break; zv+=2;} av=(I*)((I)av+k); ++i;}
   }
  }else{

   // not smallrange processing.  ai has combined fret/frequency information
   I nparts=AM(ai);  // before we possibly clone it, extract # frets found
   makewritable(ai);  // we modify the size+index info
   // allocate the result area
   GA(z,MAX(wt&~VERB,INT),nparts*2,2,0); AS(z)[0]=nparts; AS(z)[1]=2;  // output area: one per partition
   // pass through the table, writing out every value that starts a new partition.  The partition size is encoded in the value
   I i=0; I *av=IAV(ai);   // item index and input scan pointer
   if(wt&INT){
    I *wv=IAV(w); I *zv=IAV(z); while(1){I a0=*av++; if(a0-i>=0){zv[b]=a0-i; zv[1-b]=wv[i]; if(--nparts==0)break; zv+=2;} ++i;}
   }else if(wt&B01){
    B *wv=BAV(w); I *zv=IAV(z); while(1){I a0=*av++; if(a0-i>=0){zv[b]=a0-i; zv[1-b]=wv[i]; if(--nparts==0)break; zv+=2;} ++i;}
   }else if(wt&FL){  // FL
    D *wv=DAV(w); D *zv=DAV(z); while(1){I a0=*av++; if(a0-i>=0){zv[b]=(D)(a0-i); zv[1-b]=wv[i]; if(--nparts==0)break; zv+=2;} ++i;}
   }else{  // i.@#
    I *zv=IAV(z); while(1){I a0=*av++; if(a0-i>=0){zv[b]=a0-i; zv[1-b]=i; if(--nparts==0)break; zv+=2;} ++i;}
   }
  }
 }else{  // no special processing
  RZ(q=indexof(a,a)); x=repeat(eq(q,IX(n)),w); y=keytally(q,q,0L); z=stitch(b?x:y,b?y:x);  // (((i.~a) = i. # a) # w) ,. (#/.~ i.~ a)   for ({. , #)
 }
 EPILOG(z);
}    /* x ({.,#)/.y or x (#,{.)/. y */


F1(jtsldot){A h=0;AF f1=jtoblique,f2;C c,d,e;I flag=VJTFLGOK1|VJTFLGOK2;V*v;
// NOTE: u/. is processed using the code for u;.1 and passing the self for /. into the cut verb.  So, the self produced
// by /. and ;.1 must be the same as far as flags etc.
 ARGCHK1(w);
 if(NOUN&AT(w)){flag|=VGERL; RZ(h=fxeachv(1L,w));}
 v=VAV(w);
 switch(ID(w)){  // no default for f2: every path must set it
  case CPOUND: f2=jtkeytally; break;
  case CSLASH: f2=jtkey; if(AT(v->fgh[0])&VERB&&FAV(v->fgh[0])->flag&VISATOMIC2){ // f//.  if f is atomic2
   f1=jtobqfslash; flag&=~VJTFLGOK1;
   // dyad f//. is special for f=+ >. <.   we set flags to indicate the operation and the allowed types
#define keyslashvalues(w)CCM(w,CPLUS)+CCM(w,CMIN)+CCM(w,CMAX)
    CCMWDS(keyslash) CCMCAND(keyslash,cand,FAV(v->fgh[0])->id) if(CCMTST(cand,FAV(v->fgh[0])->id)){
     I op=FAV(v->fgh[0])->id&1; op=FAV(v->fgh[0])->id==CPLUS?2:op;  // 0=<. 1=>. 2=+
     flag += (((((2<<VFKEYSLASHFX)+((FL+B01)<<VFKEYSLASHTX))<<16) + (((1<<VFKEYSLASHFX)+((FL+INT+B01)<<VFKEYSLASHTX))<<8) + ((0<<VFKEYSLASHFX)+((FL+INT+B01)<<VFKEYSLASHTX))) >> (op<<3)) & (VFKEYSLASHT+VFKEYSLASHF);   // get flag bits
    }

   } break;
  case CBOX: f2=jtkeybox; break;  // </.
  case CFORK:  if(v->valencefns[0]==(AF)jtmean){flag+=(3<<VFKEYSLASHFX)+((FL+INT+B01)<<VFKEYSLASHTX);  // (+/%#)/., treated as f//.
               }else{c=ID(v->fgh[0]); d=ID(v->fgh[1]); e=ID(v->fgh[2]); 
                if(((c^e)==(CHEAD^CPOUND))&&d==CCOMMA&&(c==CHEAD||c==CPOUND)){f2=jtkeyheadtally; break;}
               }
               // otherwise (including keymean) fall through to...
  default: f2=jtkey; flag |= (FAV(w)->flag&VASGSAFE);  // pass through ASGSAFE.
 }
 return fdef(0,CSLDOT,VERB, f1,f2, w,0L,h, flag, RMAX,RMAX,RMAX);
}
