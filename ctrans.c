/*26:*/
#line 227 "ctrans.w"

#include <u.h> 
#include <libc.h> 
#include <ctype.h> 

/*:26*//*27:*/
#line 234 "ctrans.w"

/*7:*/
#line 71 "ctrans.w"

typedef struct Map Map;
struct Map{
char*key;
Rune*val;
};
extern const Map dict[];

/*:7*//*8:*/
#line 80 "ctrans.w"

extern const int lvl1[];
extern const int lvl2[];

/*:8*/
#line 235 "ctrans.w"

/*18:*/
#line 194 "ctrans.w"

Rune*candidate;

/*:18*//*21:*/
#line 206 "ctrans.w"

char input[20];
int len= 0;
Map*laststate= nil;

/*:21*//*25:*/
#line 223 "ctrans.w"

int natural= 0;

/*:25*/
#line 236 "ctrans.w"

/*11:*/
#line 102 "ctrans.w"

const Map*match(const char*key,int len,const Map*table)
{
switch(len){
case 1:
/*9:*/
#line 85 "ctrans.w"

return&dict[lvl1[key[0]-'a']];

/*:9*/
#line 107 "ctrans.w"

case 2:
/*10:*/
#line 91 "ctrans.w"

{
int index= lvl2[(key[0]-'a')*26+(key[1]-'a')];
if(index<0)return nil;
else return&dict[index];
}

/*:10*/
#line 109 "ctrans.w"

default:
if(table!=nil)
/*13:*/
#line 124 "ctrans.w"

{
int cmp;
for(int i= 0;table[i].key;i++){
cmp= strncmp(key,table[i].key,len);
if(cmp==0)return&table[i];
else if(cmp<0)return nil;
}
return nil;
}

/*:13*/
#line 112 "ctrans.w"

else
/*12:*/
#line 118 "ctrans.w"

return match(key,len,match(key,2,nil));

/*:12*/
#line 114 "ctrans.w"

}
}

/*:11*//*14:*/
#line 137 "ctrans.w"

void nextstate(char*oldbuf,int n,int outfd){

if(len> 5){
len= 0;
laststate= nil;
}
if(!natural&&islower(oldbuf[1])){
/*23:*/
#line 215 "ctrans.w"

input[len]= oldbuf[1];
len+= 1;
input[len]= 0;

/*:23*/
#line 145 "ctrans.w"

/*16:*/
#line 176 "ctrans.w"

Map*result= match(input,len,laststate);
if(result!=nil){
if(laststate!=nil)
/*20:*/
#line 202 "ctrans.w"

write(outfd,"c\b\0",3);

/*:20*/
#line 180 "ctrans.w"

candidate= result->val;
/*19:*/
#line 197 "ctrans.w"

char buf[128];
int n= snprint(buf,sizeof(buf),"c%C",*candidate)+1;
write(outfd,buf,n);

/*:19*/
#line 182 "ctrans.w"

laststate= result;
}

/*:16*/
#line 146 "ctrans.w"

return;
}else/*15:*/
#line 153 "ctrans.w"

if(oldbuf[1]==5){
natural= 1;
/*22:*/
#line 211 "ctrans.w"

len= 0;
laststate= nil;

/*:22*/
#line 156 "ctrans.w"

return;
}
else if(oldbuf[1]==14){
natural= 0;
/*22:*/
#line 211 "ctrans.w"

len= 0;
laststate= nil;

/*:22*/
#line 161 "ctrans.w"

return;
}
else if(!natural&&oldbuf[1]==' '){
/*22:*/
#line 211 "ctrans.w"

len= 0;
laststate= nil;

/*:22*/
#line 165 "ctrans.w"

return;
}
else if(oldbuf[1]==12){
/*17:*/
#line 186 "ctrans.w"

if(laststate!=nil){
/*20:*/
#line 202 "ctrans.w"

write(outfd,"c\b\0",3);

/*:20*/
#line 188 "ctrans.w"

candidate++;
if(!*candidate)candidate= laststate->val;
/*19:*/
#line 197 "ctrans.w"

char buf[128];
int n= snprint(buf,sizeof(buf),"c%C",*candidate)+1;
write(outfd,buf,n);

/*:19*/
#line 191 "ctrans.w"

}

/*:17*/
#line 169 "ctrans.w"

return;
}
else if(oldbuf[1]=='\b'){
/*22:*/
#line 211 "ctrans.w"

len= 0;
laststate= nil;

/*:22*/
#line 173 "ctrans.w"

}

/*:15*/
#line 148 "ctrans.w"

/*24:*/
#line 220 "ctrans.w"

write(outfd,oldbuf,n);

/*:24*/
#line 149 "ctrans.w"

return;
}

/*:14*/
#line 237 "ctrans.w"

void main(int argc,char**argv){
/*3:*/
#line 35 "ctrans.w"

int kbd,newkbd;

/*:3*//*6:*/
#line 60 "ctrans.w"

int n;
char buf[128];

/*:6*/
#line 239 "ctrans.w"
;

USED(argc);
USED(argv);

/*2:*/
#line 30 "ctrans.w"

if((kbd= open("/dev/kbd",OREAD))<0)
sysfatal("open kbd: %r");
/*4:*/
#line 38 "ctrans.w"

if(bind("#|","/n/temp",MREPL)<0)
sysfatal("bind /n/temp: %r");
if((newkbd= open("/n/temp/data1",OWRITE))<0)
sysfatal("open kbd pipe: %r");
if(bind("/n/temp/data","/dev/kbd",MREPL)<0)
sysfatal("bind kbd pipe: %r");
unmount(nil,"/n/temp");

/*:4*/
#line 33 "ctrans.w"


/*:2*/
#line 244 "ctrans.w"

if(fork())exits(0);

/*5:*/
#line 52 "ctrans.w"

while((n= read(kbd,buf,sizeof(buf)))> 0){
buf[n-1]= 0;
if(n<2||buf[0]!='c')
write(newkbd,buf,n);
else nextstate(buf,n,newkbd);
}

/*:5*/
#line 247 "ctrans.w"

}

/*:27*/
