#include "csa.h"
#include "mydefs.h"

#define CSA_STR_MAX 100000

static int popc(mask_t m){
   int n=0;
   while(m){
      n+=(int)(m&1ULL);
      m>>=1;
   }
   return n;
}

static int findblk(csa* c,unsigned int off){
   if(!c||!c->b||c->n==0)return -1;
   int i=0;
   for(;i<c->n;i++){
      if(c->b[i].offset==off)return i;
      if(c->b[i].offset>off)break;
   }
   return -(i+1);
}

static int rank_ex(mask_t m,int p){
   int r=0;
   for(int i=0;i<MSKLEN;i++){
      if(m&(1ULL<<i)){
         if(i==p)return r;
         r++;
      }
   }
   return -1;
}

static int rank_new(mask_t m,int p){
   int r=0;
   for(int i=0;i<p;i++){
      if(m&(1ULL<<i))r++;
   }
   return r;
}

#ifdef EXT
static void addsum(int* p,int* s){
   if(p&&s)*s+=*p;
}
#endif

csa* csa_init(void){
   csa* c=malloc(sizeof*c);
   if(!c)return NULL;
   c->b=NULL;
   c->n=0;
   return c;
}

bool csa_get(csa* c,int idx,int* v){
   if(!c||!v||idx<0)return false;
   unsigned int off=(unsigned int)((idx/MSKLEN)*MSKLEN);
   int p=idx%MSKLEN;
   int bi=findblk(c,off);
   if(bi<0)return false;
   block* b=&c->b[bi];
   if(!(b->msk&(1ULL<<p)))return false;
   int r=rank_ex(b->msk,p);
   if(r<0)return false;
   *v=b->vals[r];
   return true;
}

bool csa_set(csa* c,int idx,int val){
   if(!c||idx<0)return false;
   unsigned int off=(unsigned int)((idx/MSKLEN)*MSKLEN);
   int p=idx%MSKLEN;
   int bi=findblk(c,off);
   block* b=NULL;
   if(bi>=0)b=&c->b[bi];
   else{
      int ins=-(bi+1);
      block* nb=realloc(c->b,(size_t)(c->n+1)*sizeof*nb);
      if(!nb)return false;
      c->b=nb;
      if(c->n-ins>0)memmove(&c->b[ins+1],&c->b[ins],(size_t)(c->n-ins)*sizeof*c->b);
      b=&c->b[ins];
      b->offset=off;
      b->msk=0;
      b->vals=NULL;
      c->n++;
   }
   mask_t bit=(1ULL<<p);
   bool ex=(b->msk&bit)!=0;
   int oc=popc(b->msk);
   if(ex){
      int r=rank_ex(b->msk,p);
      if(r<0)return false;
      b->vals[r]=val;
      return true;
   }
   int* nv=realloc(b->vals,(size_t)(oc+1)*sizeof*nv);
   if(!nv)return false;
   b->vals=nv;
   int r=rank_new(b->msk,p);
   if(oc-r>0)memmove(&b->vals[r+1],&b->vals[r],(size_t)(oc-r)*sizeof*b->vals);
   b->vals[r]=val;
   b->msk|=bit;
   return true;
}

void csa_tostring(csa* c,char* s){
   if(!s)return;
   if(!c){
      s[0]='\0';
      return;
   }
   if(c->n==0){
      strcpy(s,"0 blocks");
      return;
   }
   char* p=s;
   size_t rem=CSA_STR_MAX;
   int w=snprintf(p,rem,"%d block%s",c->n,(c->n==1)?"":"s");
   if(w<0||(size_t)w>=rem){
      s[0]='\0';
      return;
   }
   p+=w;
   rem-=w;
   for(int i=0;i<c->n;i++){
      block* b=&c->b[i];
      int k=popc(b->msk);
      const char* fmt=(i==0)?" {%d|":"{%d|";
      w=snprintf(p,rem,fmt,k);
      if(w<0||(size_t)w>=rem){
         s[CSA_STR_MAX-1]='\0';
         return;
      }
      p+=w;
      rem-=w;
      int j=0;
      bool f=1;
      for(int q=0;q<MSKLEN;q++){
         if(b->msk&(1ULL<<q)){
            if(!f){
               if(rem<=1){
                  s[CSA_STR_MAX-1]='\0';
                  return;
               }
               *p++=':';
               *p='\0';
               rem--;
            }
            int id=(int)b->offset+q;
            w=snprintf(p,rem,"[%d]=%d",id,b->vals[j]);
            if(w<0||(size_t)w>=rem){
               s[CSA_STR_MAX-1]='\0';
               return;
            }
            p+=w;
            rem-=w;
            j++;
            f=0;
         }
      }
      if(rem<=1){
         s[CSA_STR_MAX-1]='\0';
         return;
      }
      *p++='}';
      *p='\0';
      rem--;
   }
   *p='\0';
}

void csa_free(csa** l){
   if(!l||!*l)return;
   csa* c=*l;
   if(c->b){
      for(int i=0;i<c->n;i++){
         free(c->b[i].vals);
         c->b[i].vals=NULL;
      }
      free(c->b);
      c->b=NULL;
   }
   free(c);
   *l=NULL;
}

void test(void){
   mask_t all;
   assert(popc(0ULL)==0);
   assert(popc(1ULL)==1);
   assert(popc((mask_t)1<<3)==1);
   all=(MSKLEN==64)?UINT64_MAX:0ULL;
   if(MSKLEN==64)assert(popc(all)==64);

   mask_t m=0ULL;
   m|=(mask_t)1<<0;
   m|=(mask_t)1<<5;
   m|=(mask_t)1<<10;
   assert(rank_ex(m,0)==0);
   assert(rank_ex(m,5)==1);
   assert(rank_ex(m,10)==2);
   assert(rank_ex(m,2)==-1);
   assert(rank_new(m,0)==0);
   assert(rank_new(m,3)==1);
   assert(rank_new(m,6)==2);
   assert(rank_new(m,11)==3);

   csa* c=csa_init();
   assert(c&&c->n==0);

   char buf[CSA_STR_MAX];
   int v=12345;

   assert(!csa_get(c,0,&v));
   assert(v==12345);
   assert(!csa_set(c,-1,1));
   assert(!csa_get(c,-5,&v));

   assert(csa_set(c,2,20));
   assert(csa_set(c,10,100));
   assert(csa_set(c,40,400));
   assert(c->n==1);
   assert(c->b[0].offset==0U);
   assert(popc(c->b[0].msk)==3);
   assert(csa_get(c,2,&v)&&v==20);
   assert(csa_get(c,10,&v)&&v==100);
   assert(csa_get(c,40,&v)&&v==400);
   assert(!csa_get(c,3,&v));

   int n0=c->n;
   mask_t m0=c->b[0].msk;
   assert(csa_set(c,10,999));
   assert(c->n==n0);
   assert(c->b[0].msk==m0);
   assert(csa_get(c,10,&v)&&v==999);

   assert(csa_set(c,63,6300));
   assert(csa_set(c,64,6400));
   assert(csa_set(c,130,13000));
   assert(c->n>=3);
   assert(c->b[0].offset==0U);
   assert(c->b[1].offset==64U);
   assert(c->b[2].offset==128U);
   assert(popc(c->b[1].msk)==1);
   assert(popc(c->b[2].msk)==1);
   assert(csa_get(c,63,&v)&&v==6300);
   assert(csa_get(c,64,&v)&&v==6400);
   assert(csa_get(c,130,&v)&&v==13000);

   csa_tostring(c,buf);
   assert(strstr(buf,"block")!=NULL);
   assert(strstr(buf,"[2]=")!=NULL);
   assert(strstr(buf,"[64]=")!=NULL);
   assert(strstr(buf,"[130]=")!=NULL);

   v=777;
   assert(!csa_get(NULL,0,&v));
   assert(v==777);
   assert(!csa_set(NULL,0,1));
   assert(!csa_set(c,-100,1));

   csa_tostring(NULL,buf);
   assert(strcmp(buf,"")==0);

#ifdef EXT
   csa_free(&c);
   assert(c==NULL);

   c=csa_init();
   assert(c);
   assert(csa_set(c,1,1));
   assert(csa_set(c,3,3));
   assert(csa_set(c,70,70));
   assert(csa_set(c,71,71));

   int sum=0;
   csa_foreach(addsum,c,&sum);
   assert(sum==145);

   assert(c->n==2);
   assert(csa_delete(c,3));
   assert(popc(c->b[0].msk)==1);
   assert(csa_get(c,1,&v)&&v==1);
   assert(!csa_get(c,3,&v));
   assert(!csa_delete(c,9999));
   assert(!csa_delete(c,-5));

   assert(csa_delete(c,1));
   assert(csa_delete(c,70));
   assert(csa_delete(c,71));
   csa_tostring(c,buf);
   assert(strcmp(buf,"0 blocks")==0);
   assert(c->n==0);
#endif

   csa_free(&c);
   assert(c==NULL);
   csa_free(NULL);
}

#ifdef EXT
void csa_foreach(void (*func)(int* p,int* a),csa* c,int* a){
   if(!c||!func)return;
   for(int i=0;i<c->n;i++){
      block* b=&c->b[i];
      int k=popc(b->msk);
      for(int j=0;j<k;j++)func(&b->vals[j],a);
   }
}

bool csa_delete(csa* c,int idx){
   if(!c||idx<0||c->n==0)return false;
   unsigned int off=(unsigned int)((idx/MSKLEN)*MSKLEN);
   int p=idx%MSKLEN;
   int bi=findblk(c,off);
   if(bi<0)return false;
   block* b=&c->b[bi];
   mask_t bit=(mask_t)1<<p;
   if(!(b->msk&bit))return false;
   int k0=popc(b->msk);
   int r=rank_ex(b->msk,p);
   if(r<0)return false;
   if(k0-r-1>0)memmove(&b->vals[r],&b->vals[r+1],(size_t)(k0-r-1)*sizeof*b->vals);
   b->msk&=~bit;
   int k1=k0-1;
   if(k1==0){
      free(b->vals);
      b->vals=NULL;
      if(c->n==1){
         free(c->b);
         c->b=NULL;
         c->n=0;
      }else{
         if(c->n-bi-1>0)memmove(&c->b[bi],&c->b[bi+1],(size_t)(c->n-bi-1)*sizeof*c->b);
         c->n--;
      }
   }
   return true;
}
#endif


// [hy25163@it106294 csa]$ make run
// ./csa
// ./csa_s
// ./fibmemo
// 1 => 1
// 2 => 1
// 3 => 2
// 4 => 3
// 5 => 5
// 6 => 8
// 7 => 13
// 8 => 21
// 9 => 34
// 10 => 55
// 11 => 89
// 12 => 144
// 13 => 233
// 14 => 377
// 15 => 610
// 16 => 987
// 17 => 1597
// 18 => 2584
// 19 => 4181
// 20 => 6765
// 21 => 10946
// 22 => 17711
// 23 => 28657
// 24 => 46368
// 25 => 75025
// 26 => 121393
// 27 => 196418
// 28 => 317811
// 29 => 514229
// 30 => 832040
// 31 => 1346269
// 32 => 2178309
// 33 => 3524578
// 34 => 5702887
// 35 => 9227465
// 36 => 14930352
// 37 => 24157817
// 38 => 39088169
// 39 => 63245986
// 40 => 102334155
// [hy25163@it106294 csa]$ make runall
// ./csa
// ./csa_s
// ./fibmemo
// 1 => 1
// 2 => 1
// 3 => 2
// 4 => 3
// 5 => 5
// 6 => 8
// 7 => 13
// 8 => 21
// 9 => 34
// 10 => 55
// 11 => 89
// 12 => 144
// 13 => 233
// 14 => 377
// 15 => 610
// 16 => 987
// 17 => 1597
// 18 => 2584
// 19 => 4181
// 20 => 6765
// 21 => 10946
// 22 => 17711
// 23 => 28657
// 24 => 46368
// 25 => 75025
// 26 => 121393
// 27 => 196418
// 28 => 317811
// 29 => 514229
// 30 => 832040
// 31 => 1346269
// 32 => 2178309
// 33 => 3524578
// 34 => 5702887
// 35 => 9227465
// 36 => 14930352
// 37 => 24157817
// 38 => 39088169
// 39 => 63245986
// 40 => 102334155
// ./factorials
// Printing all factorials
// 1
// 2
// 6
// 24
// 120
// 720
// 5040
// 40320
// 362880
// 3628800
// 39916800
// 479001600
// ./primes
// 2
// 3
// 5
// 7
// 11
// 13
// 17
// 19
// 23
// 29
// 31
// 37
// 41
// 43
// 47
// 53
// 59
// 61
// 67
// 71
// 73
// 79
// 83
// 89
// 97
// 101
// 103
// 107
// 109
// 113
// 127
// 131
// 137
// 139
// 149
// 151
// 157
// 163
// 167
// 173
// 179
// 181
// 191
// 193
// 197
// 199
// 211
// 223
// 227
// 229
// 233
// 239
// 241
// 251
// 257
// 263
// 269
// 271
// ./csa_ext
