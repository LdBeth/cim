/*30:*/
#line 268 "ctrans.w"

#include <u.h> 
#include <libc.h> 
#include <ctype.h> 

/*:30*//*31:*/
#line 275 "ctrans.w"

/*8:*/
#line 83 "ctrans.w"

typedef struct Map Map;
struct Map{
int key;
Rune*val;
};
extern const Map dict[];

/*:8*//*9:*/
#line 92 "ctrans.w"

extern const int lvl1[];
extern const int lvl2[];

/*:9*/
#line 276 "ctrans.w"

/*19:*/
#line 204 "ctrans.w"

int natural= 0;

/*:19*//*22:*/
#line 233 "ctrans.w"

Rune*candidate;

/*:22*//*25:*/
#line 245 "ctrans.w"

char input[20];
int len= 0;
const Map*laststate= nil;

/*:25*/
#line 277 "ctrans.w"

/*12:*/
#line 113 "ctrans.w"

const Map*match(const char*key,int len,const Map*table)
{
switch(len){
case 1:
/*10:*/
#line 97 "ctrans.w"

return&dict[lvl1[key[0]-'a']];

/*:10*/
#line 118 "ctrans.w"

case 2:
/*11:*/
#line 103 "ctrans.w"

{
int index= lvl2[(key[0]-'a')*26+(key[1]-'a')];
return(index<0)?nil:&dict[index];
}

/*:11*/
#line 120 "ctrans.w"

default:
if(table!=nil)
/*14:*/
#line 136 "ctrans.w"

{
int cmp;
int keycode;
/*15:*/
#line 150 "ctrans.w"

keycode= 0;
for(int i= 2;i<len;i++){
keycode<<= 5;
keycode+= key[i]-'a'+1;
}

/*:15*/
#line 140 "ctrans.w"

for/*16:*/
#line 159 "ctrans.w"

(int i= (table[0].key)?0:1;table[i].key;i++)

/*:16*/
#line 141 "ctrans.w"
{
cmp= (table[i].key>>(5*(3-(len-2))))-keycode;
if(cmp==0)return&table[i];
else if(cmp> 0)return nil;
}
return nil;
}

/*:14*/
#line 123 "ctrans.w"

else
/*13:*/
#line 129 "ctrans.w"

return match(key,len,match(key,2,nil));

/*:13*/
#line 125 "ctrans.w"

}
}

/*:12*//*17:*/
#line 164 "ctrans.w"

void nextstate(char*oldbuf,int n,int outfd){

if(len> 5){
len= 0;
laststate= nil;
}
if(!natural&&islower(oldbuf[1])){
/*27:*/
#line 254 "ctrans.w"

input[len]= oldbuf[1];
len+= 1;
input[len]= '\0';

/*:27*/
#line 172 "ctrans.w"

/*20:*/
#line 209 "ctrans.w"

const Map*result;
Search:
result= match(input,len,laststate);
if(result!=nil){
if(laststate!=nil)
/*24:*/
#line 241 "ctrans.w"

write(outfd,"c\b\0",3);

/*:24*/
#line 215 "ctrans.w"

candidate= result->val;
/*23:*/
#line 236 "ctrans.w"

char buf[128];
int n= snprint(buf,sizeof(buf),"c%C",*candidate)+1;
write(outfd,buf,n);

/*:23*/
#line 217 "ctrans.w"

laststate= result;
}else if(laststate!=nil){
/*28:*/
#line 259 "ctrans.w"

input[0]= input[len-1];
input[1]= '\0';
len= 1;

/*:28*/
#line 220 "ctrans.w"

laststate= nil;
goto Search;
}

/*:20*/
#line 173 "ctrans.w"

return;
}else/*18:*/
#line 180 "ctrans.w"

if(oldbuf[1]==20){
natural= 1;
/*26:*/
#line 250 "ctrans.w"

len= 0;
laststate= nil;

/*:26*/
#line 183 "ctrans.w"

return;
}
else if(oldbuf[1]==14){
natural= 0;
/*26:*/
#line 250 "ctrans.w"

len= 0;
laststate= nil;

/*:26*/
#line 188 "ctrans.w"

return;
}
else if(!natural&&oldbuf[1]==' '){
/*26:*/
#line 250 "ctrans.w"

len= 0;
laststate= nil;

/*:26*/
#line 192 "ctrans.w"

return;
}
else if(oldbuf[1]==12){
/*21:*/
#line 225 "ctrans.w"

if(laststate!=nil){
/*24:*/
#line 241 "ctrans.w"

write(outfd,"c\b\0",3);

/*:24*/
#line 227 "ctrans.w"

candidate++;
if(!*candidate)candidate= laststate->val;
/*23:*/
#line 236 "ctrans.w"

char buf[128];
int n= snprint(buf,sizeof(buf),"c%C",*candidate)+1;
write(outfd,buf,n);

/*:23*/
#line 230 "ctrans.w"

}

/*:21*/
#line 196 "ctrans.w"

return;
}
else if(oldbuf[1]=='\b'){
/*26:*/
#line 250 "ctrans.w"

len= 0;
laststate= nil;

/*:26*/
#line 200 "ctrans.w"

}

/*:18*/
#line 175 "ctrans.w"

/*29:*/
#line 264 "ctrans.w"

write(outfd,oldbuf,n);

/*:29*/
#line 176 "ctrans.w"

return;
}

/*:17*/
#line 278 "ctrans.w"

void main(int argc,char**argv){
/*3:*/
#line 35 "ctrans.w"

int kbd,newkbd;

/*:3*//*7:*/
#line 71 "ctrans.w"

int n;
char buf[128];

/*:7*/
#line 280 "ctrans.w"
;

if(argc> 1){
/*32:*/
#line 293 "ctrans.w"

const Map*result= match(argv[1],strlen(argv[1]),nil);
if(result==nil)print("not found!\n");
else print("%S\n",result->val);

/*:32*/
#line 283 "ctrans.w"

exits(nil);
}

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
#line 287 "ctrans.w"

if(fork())exits(nil);

/*5:*/
#line 55 "ctrans.w"

while((n= read(kbd,buf,sizeof(buf)))> 0){
buf[n-1]= 0;
int bg,len;
for(bg= 0;bg<n;bg+= len){
/*6:*/
#line 67 "ctrans.w"

len= 1;
while(!buf[bg+len])len++;

/*:6*/
#line 60 "ctrans.w"

if(len<2||buf[bg]!='c')
write(newkbd,&buf[bg],len);
else nextstate(&buf[bg],len,newkbd);
}
}

/*:5*/
#line 290 "ctrans.w"

}

/*:31*/
