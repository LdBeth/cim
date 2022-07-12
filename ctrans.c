/*29:*/
#line 261 "ctrans.w"

#include <u.h> 
#include <libc.h> 
#include <ctype.h> 

/*:29*//*30:*/
#line 268 "ctrans.w"

/*7:*/
#line 75 "ctrans.w"

typedef struct Map Map;
struct Map{
int key;
Rune*val;
};
extern const Map dict[];

/*:7*//*8:*/
#line 84 "ctrans.w"

extern const int lvl1[];
extern const int lvl2[];

/*:8*/
#line 269 "ctrans.w"

/*18:*/
#line 197 "ctrans.w"

int natural= 0;

/*:18*//*21:*/
#line 226 "ctrans.w"

Rune*candidate;

/*:21*//*24:*/
#line 238 "ctrans.w"

char input[20];
int len= 0;
const Map*laststate= nil;

/*:24*/
#line 270 "ctrans.w"

/*11:*/
#line 105 "ctrans.w"

const Map*match(const char*key,int len,const Map*table)
{
switch(len){
case 1:
/*9:*/
#line 89 "ctrans.w"

return&dict[lvl1[key[0]-'a']];

/*:9*/
#line 110 "ctrans.w"

case 2:
/*10:*/
#line 95 "ctrans.w"

{
int index= lvl2[(key[0]-'a')*26+(key[1]-'a')];
return(index<0)?nil:&dict[index];
}

/*:10*/
#line 112 "ctrans.w"

default:
if(table!=nil)
/*13:*/
#line 128 "ctrans.w"

{
int cmp;
int keycode,keylen;
/*14:*/
#line 142 "ctrans.w"

keycode= 0;
for(keylen= 2;keylen<len;keylen++){
keycode<<= 5;
keycode+= key[keylen]-'a'+1;
}
keylen-= 2;

/*:14*/
#line 132 "ctrans.w"

for/*15:*/
#line 152 "ctrans.w"

(int i= (table[0].key)?0:1;table[i].key;i++)

/*:15*/
#line 133 "ctrans.w"
{
cmp= (table[i].key>>(3-keylen))-keycode;
if(cmp==0)return&table[i];
else if(cmp<0)return nil;
}
return nil;
}

/*:13*/
#line 115 "ctrans.w"

else
/*12:*/
#line 121 "ctrans.w"

return match(key,len,match(key,2,nil));

/*:12*/
#line 117 "ctrans.w"

}
}

/*:11*//*16:*/
#line 157 "ctrans.w"

void nextstate(char*oldbuf,int n,int outfd){

if(len> 5){
len= 0;
laststate= nil;
}
if(!natural&&islower(oldbuf[1])){
/*26:*/
#line 247 "ctrans.w"

input[len]= oldbuf[1];
len+= 1;
input[len]= '\0';

/*:26*/
#line 165 "ctrans.w"

/*19:*/
#line 202 "ctrans.w"

const Map*result;
Search:
result= match(input,len,laststate);
if(result!=nil){
if(laststate!=nil)
/*23:*/
#line 234 "ctrans.w"

write(outfd,"c\b\0",3);

/*:23*/
#line 208 "ctrans.w"

candidate= result->val;
/*22:*/
#line 229 "ctrans.w"

char buf[128];
int n= snprint(buf,sizeof(buf),"c%C",*candidate)+1;
write(outfd,buf,n);

/*:22*/
#line 210 "ctrans.w"

laststate= result;
}else if(laststate!=nil){
/*27:*/
#line 252 "ctrans.w"

input[0]= input[len-1];
input[1]= '\0';
len= 1;

/*:27*/
#line 213 "ctrans.w"

laststate= nil;
goto Search;
}

/*:19*/
#line 166 "ctrans.w"

return;
}else/*17:*/
#line 173 "ctrans.w"

if(oldbuf[1]==20){
natural= 1;
/*25:*/
#line 243 "ctrans.w"

len= 0;
laststate= nil;

/*:25*/
#line 176 "ctrans.w"

return;
}
else if(oldbuf[1]==14){
natural= 0;
/*25:*/
#line 243 "ctrans.w"

len= 0;
laststate= nil;

/*:25*/
#line 181 "ctrans.w"

return;
}
else if(!natural&&oldbuf[1]==' '){
/*25:*/
#line 243 "ctrans.w"

len= 0;
laststate= nil;

/*:25*/
#line 185 "ctrans.w"

return;
}
else if(oldbuf[1]==12){
/*20:*/
#line 218 "ctrans.w"

if(laststate!=nil){
/*23:*/
#line 234 "ctrans.w"

write(outfd,"c\b\0",3);

/*:23*/
#line 220 "ctrans.w"

candidate++;
if(!*candidate)candidate= laststate->val;
/*22:*/
#line 229 "ctrans.w"

char buf[128];
int n= snprint(buf,sizeof(buf),"c%C",*candidate)+1;
write(outfd,buf,n);

/*:22*/
#line 223 "ctrans.w"

}

/*:20*/
#line 189 "ctrans.w"

return;
}
else if(oldbuf[1]=='\b'){
/*25:*/
#line 243 "ctrans.w"

len= 0;
laststate= nil;

/*:25*/
#line 193 "ctrans.w"

}

/*:17*/
#line 168 "ctrans.w"

/*28:*/
#line 257 "ctrans.w"

write(outfd,oldbuf,n);

/*:28*/
#line 169 "ctrans.w"

return;
}

/*:16*/
#line 271 "ctrans.w"

void main(int argc,char**argv){
/*3:*/
#line 35 "ctrans.w"

int kbd,newkbd;

/*:3*//*6:*/
#line 63 "ctrans.w"

int n;
char buf[128];

/*:6*/
#line 273 "ctrans.w"
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
#line 278 "ctrans.w"

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
#line 281 "ctrans.w"

}

/*:30*/
