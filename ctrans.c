/*28:*/
#line 251 "ctrans.w"

#include <u.h> 
#include <libc.h> 
#include <ctype.h> 

/*:28*//*29:*/
#line 258 "ctrans.w"

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
#line 259 "ctrans.w"

/*19:*/
#line 213 "ctrans.w"

Rune*candidate;

/*:19*//*22:*/
#line 225 "ctrans.w"

char input[20];
int len= 0;
const Map*laststate= nil;

/*:22*//*27:*/
#line 247 "ctrans.w"

int natural= 0;

/*:27*/
#line 260 "ctrans.w"

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
return(index<0)?nil:&dict[index];
}

/*:10*/
#line 111 "ctrans.w"

default:
if(table!=nil)
/*13:*/
#line 129 "ctrans.w"

{
int cmp;
char*newkey= &key[2];
for/*14:*/
#line 143 "ctrans.w"

(int i= (table[0].key)?0:1;table[i].key;i++)

/*:14*/
#line 133 "ctrans.w"
{
cmp= strncmp(newkey,table[i].key,(len-2));
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

/*:11*//*15:*/
#line 148 "ctrans.w"

void nextstate(char*oldbuf,int n,int outfd){

if(len> 5){
len= 0;
laststate= nil;
}
if(!natural&&islower(oldbuf[1])){
/*24:*/
#line 234 "ctrans.w"

input[len]= oldbuf[1];
len+= 1;
input[len]= '\0';

/*:24*/
#line 156 "ctrans.w"

/*17:*/
#line 189 "ctrans.w"

const Map*result;
Search:
result= match(input,len,laststate);
if(result!=nil){
if(laststate!=nil)
/*21:*/
#line 221 "ctrans.w"

write(outfd,"c\b\0",3);

/*:21*/
#line 195 "ctrans.w"

candidate= result->val;
/*20:*/
#line 216 "ctrans.w"

char buf[128];
int n= snprint(buf,sizeof(buf),"c%C",*candidate)+1;
write(outfd,buf,n);

/*:20*/
#line 197 "ctrans.w"

laststate= result;
}else if(laststate!=nil){
/*25:*/
#line 239 "ctrans.w"

input[0]= input[len-1];
input[1]= '\0';
len= 1;

/*:25*/
#line 200 "ctrans.w"

lastestate= nil;
goto Search;
}

/*:17*/
#line 157 "ctrans.w"

return;
}else/*16:*/
#line 164 "ctrans.w"

if(oldbuf[1]==20){
natural= 1;
/*23:*/
#line 230 "ctrans.w"

len= 0;
laststate= nil;

/*:23*/
#line 167 "ctrans.w"

return;
}
else if(oldbuf[1]==14){
natural= 0;
/*23:*/
#line 230 "ctrans.w"

len= 0;
laststate= nil;

/*:23*/
#line 172 "ctrans.w"

return;
}
else if(!natural&&oldbuf[1]==' '){
/*23:*/
#line 230 "ctrans.w"

len= 0;
laststate= nil;

/*:23*/
#line 176 "ctrans.w"

return;
}
else if(oldbuf[1]==12){
/*18:*/
#line 205 "ctrans.w"

if(laststate!=nil){
/*21:*/
#line 221 "ctrans.w"

write(outfd,"c\b\0",3);

/*:21*/
#line 207 "ctrans.w"

candidate++;
if(!*candidate)candidate= laststate->val;
/*20:*/
#line 216 "ctrans.w"

char buf[128];
int n= snprint(buf,sizeof(buf),"c%C",*candidate)+1;
write(outfd,buf,n);

/*:20*/
#line 210 "ctrans.w"

}

/*:18*/
#line 180 "ctrans.w"

return;
}
else if(oldbuf[1]=='\b'){
/*23:*/
#line 230 "ctrans.w"

len= 0;
laststate= nil;

/*:23*/
#line 184 "ctrans.w"

}

/*:16*/
#line 159 "ctrans.w"

/*26:*/
#line 244 "ctrans.w"

write(outfd,oldbuf,n);

/*:26*/
#line 160 "ctrans.w"

return;
}

/*:15*/
#line 261 "ctrans.w"

void main(int argc,char**argv){
/*3:*/
#line 35 "ctrans.w"

int kbd,newkbd;

/*:3*//*6:*/
#line 63 "ctrans.w"

int n;
char buf[128];

/*:6*/
#line 263 "ctrans.w"
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
#line 268 "ctrans.w"

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
#line 271 "ctrans.w"

}

/*:29*/
