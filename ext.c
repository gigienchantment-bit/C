#include "ext_nm.h"
#include <string.h>

static bool inb(int r,int c){
    return r>=0&&r<H&&c>=0&&c<W;
}

static bool samev(int a,int b){
    if(a==0||b==0)return false;
    return a==b||(a+b==10);
}

static bool lineok(const board*b,int r1,int c1,int r2,int c2){
    int dr=r2-r1,dc=c2-c1;
    if(!dr&&!dc)return false;
    int adr=dr>0?dr:-dr,adc=dc>0?dc:-dc;
    if(!(dr==0||dc==0||adr==adc))return false;
    int sr=(dr>0)-(dr<0),sc=(dc>0)-(dc<0);
    int step=adr>adc?adr:adc;
    int r=r1+sr,c=c1+sc;
    for(int k=1;k<step;k++,r+=sr,c+=sc){
        if(b->a[r][c]!=0)return false;
    }
    return true;
}

static bool allzero(const board*b){
    for(int r=0;r<H;r++)
        for(int c=0;c<W;c++)
            if(b->a[r][c]!=0)return false;
    return true;
}

static bool sameb(const board*a,const board*b){
    return memcmp(a,b,sizeof(board))==0;
}

board randfill(int n){
    board b;
    srand((unsigned)n);
    for(int r=0;r<H;r++)
        for(int c=0;c<W;c++)
            b.a[r][c]=(unsigned char)(rand()%9+1);
    return b;
}

bool take(board*p,pair z){
    int x1=z.x1,y1=z.y1,x2=z.x2,y2=z.y2;
    if(!inb(y1,x1)||!inb(y2,x2))return false;
    if(x1==x2&&y1==y2)return false;
    unsigned char v1=p->a[y1][x1],v2=p->a[y2][x2];
    if(!samev(v1,v2))return false;
    if(!lineok(p,y1,x1,y2,x2))return false;
    p->a[y1][x1]=0;
    p->a[y2][x2]=0;
    return true;
}

bool solve(int seed){
    board q[MAXLIST];
    int f=0,bk=0;
    q[bk++]=randfill(seed);
    while(f<bk){
        board cur=q[f++];
        if(allzero(&cur))return true;
        for(int i=0;i<H*W;i++){
            int x1=i%W,y1=i/W;
            unsigned char v1=cur.a[y1][x1];
            if(!v1)continue;
            for(int j=i+1;j<H*W;j++){
                int x2=j%W,y2=j/W;
                unsigned char v2=cur.a[y2][x2];
                if(!v2)continue;
                if(!samev(v1,v2))continue;
                pair z={x1,y1,x2,y2};
                board nxt=cur;
                if(!take(&nxt,z))continue;
                bool seen=false;
                for(int k=0;k<bk;k++){
                    if(sameb(&nxt,&q[k])){seen=true;break;}
                }
                if(seen)continue;
                if(bk>=MAXLIST)return false;
                q[bk++]=nxt;
            }
        }
    }
    return false;
}

void test(void){
    board b=randfill(6);
    assert(take(&b,(pair){1,0,2,0}));
    assert(take(&b,(pair){1,1,1,2}));
    assert(take(&b,(pair){0,2,2,2}));
    assert(!take(&b,(pair){3,0,2,1}));

    b=randfill(6);
    assert(take(&b,(pair){2,0,1,0}));
    assert(take(&b,(pair){1,2,1,1}));
    assert(take(&b,(pair){2,2,0,2}));
    assert(!take(&b,(pair){2,1,3,0}));

    b=randfill(6);
    assert(take(&b,(pair){1,0,2,0}));
    assert(!take(&b,(pair){0,0,3,2}));

    b=randfill(15);
    assert(take(&b,(pair){2,1,3,2}));
    assert(!take(&b,(pair){1,2,4,2}));

    assert(solve(3648));
    assert(solve(1762));
    assert(solve(924));
    assert(solve(363));
    assert(!solve(6));
    assert(!solve(2000));
    assert(!solve(666));
}
