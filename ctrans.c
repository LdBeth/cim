/*26:*/
#line 229 "ctrans.w"

#include <u.h> 
#include <libc.h> 
#include <ctype.h> 

/*:26*//*27:*/
#line 236 "ctrans.w"

/*7:*/
#line 74 "ctrans.w"

typedef struct Map Map;
struct Map{
char*key;
Rune*val;
};
extern const Map dict[];

/*:7*//*8:*/
#line 83 "ctrans.w"

extern const int lvl1[];
extern const int lvl2[];

/*:8*/
#line 237 "ctrans.w"

/*18:*/
#line 196 "ctrans.w"

Rune*candidate;

/*:18*//*21:*/
#line 208 "ctrans.w"

char input[20];
int len= 0;
Map*laststate= nil;

/*:21*//*25:*/
#line 225 "ctrans.w"

int natural= 0;

/*:25*/
#line 238 "ctrans.w"

/*11:*/
#line 104 "ctrans.w"

const Map*match(const char*key,int len,const Map*table)
{
switch(len){
case 1:
/*9:*/
#line 88 "ctrans.w"

return&dict[lvl1[key[0]-'a']];

/*:9*/
#line 109 "ctrans.w"

case 2:
/*10:*/
#line 94 "ctrans.w"

{
int index= lvl2[(key[0]-'a')*26+(key[1]-'a')];
return?(index<0)nil:&dict[index];
}

/*:10*/
#line 111 "ctrans.w"

default:
if(table!=nil)
/*13:*/
#line 126 "ctrans.w"

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
#line 114 "ctrans.w"

else
/*12:*/
#line 120 "ctrans.w"

return match(key,len,match(key,2,nil));

/*:12*/
#line 116 "ctrans.w"

}
}

/*:11*//*14:*/
#line 139 "ctrans.w"

void nextstate(char*oldbuf,int n,int outfd){

if(len> 5){
len= 0;
laststate= nil;
}
if(!natural&&islower(oldbuf[1])){
/*23:*/
#line 217 "ctrans.w"

input[len]= oldbuf[1];
len+= 1;
input[len]= 0;

/*:23*/
#line 147 "ctrans.w"

/*16:*/
#line 178 "ctrans.w"

Map*result= match(input,len,laststate);
if(result!=nil){
if(laststate!=nil)
/*20:*/
#line 204 "ctrans.w"

write(outfd,"c\b\0",3);

/*:20*/
#line 182 "ctrans.w"

candidate= result->val;
/*19:*/
#line 199 "ctrans.w"

char buf[128];
int n= snprint(buf,sizeof(buf),"c%C",*candidate)+1;
write(outfd,buf,n);

/*:19*/
#line 184 "ctrans.w"

laststate= result;
}

/*:16*/
#line 148 "ctrans.w"

return;
}else/*15:*/
#line 155 "ctrans.w"

if(oldbuf[1]==5){
natural= 1;
/*22:*/
#line 213 "ctrans.w"

len= 0;
laststate= nil;

/*:22*/
#line 158 "ctrans.w"

return;
}
else if(oldbuf[1]==14){
natural= 0;
/*22:*/
#line 213 "ctrans.w"

len= 0;
laststate= nil;

/*:22*/
#line 163 "ctrans.w"

return;
}
else if(!natural&&oldbuf[1]==' '){
/*22:*/
#line 213 "ctrans.w"

len= 0;
laststate= nil;

/*:22*/
#line 167 "ctrans.w"

return;
}
else if(oldbuf[1]==12){
/*17:*/
#line 188 "ctrans.w"

if(laststate!=nil){
/*20:*/
#line 204 "ctrans.w"

write(outfd,"c\b\0",3);

/*:20*/
#line 190 "ctrans.w"

candidate++;
if(!*candidate)candidate= laststate->val;
/*19:*/
#line 199 "ctrans.w"

char buf[128];
int n= snprint(buf,sizeof(buf),"c%C",*candidate)+1;
write(outfd,buf,n);

/*:19*/
#line 193 "ctrans.w"

}

/*:17*/
#line 171 "ctrans.w"

return;
}
else if(oldbuf[1]=='\b'){
/*22:*/
#line 213 "ctrans.w"

len= 0;
laststate= nil;

/*:22*/
#line 175 "ctrans.w"

}

/*:15*/
#line 150 "ctrans.w"

/*24:*/
#line 222 "ctrans.w"

write(outfd,oldbuf,n);

/*:24*/
#line 151 "ctrans.w"

return;
}

/*:14*/
#line 239 "ctrans.w"

void main(int argc,char**argv){
/*3:*/
#line 35 "ctrans.w"

int kbd,newkbd;

/*:3*//*6:*/
#line 63 "ctrans.w"

int n;
char buf[128];

/*:6*/
#line 241 "ctrans.w"
;

USED(argc);
USED(argv);

/*2:*/
#line 30 "ctrans.w"

if((kbd= open("/dev/kbd",OREAD))<0)
sysfatal("open kbd: %r");
/*4:*/
#line 41 "ctrans.w"

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
#line 246 "ctrans.w"

if(fork())exits(nil);

/*5:*/
#line 55 "ctrans.w"

while((n= read(kbd,buf,sizeof(buf)))> 0){
buf[n-1]= 0;
if(n<2||buf[0]!='c')
write(newkbd,buf,n);
else nextstate(buf,n,newkbd);
}

/*:5*/
#line 249 "ctrans.w"

}

/*:27*/
