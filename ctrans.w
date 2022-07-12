% -*- mode:cweb -*-
\def\title{CIM - The Cangjie Input Method for 9front}

@*Introduction. This program translates input from {\tt /dev/kbd} to
Chinese runes. The idea is based on {\tt ktrans} but this is a rewrite
to have much cleaner code and the efficiency for handling a much
bigger dictionary.

The usage is the almost the same as {\tt ktrans}: just invoke {\tt
cim} before {\tt rio}.

{\tt \^{}T} gives you English mode, {\tt \^{}N} enters Cangjie mode. 
{\tt \^{}L} rotates selection.

I know you might want Pinyin but Cangjie is a little bit easier to
implement and way easier to use when there is no prompt.

@.ktrans@>
@.rio@>
@.cim@>

@*Setup keyboard I/O. See {\tt kbdfs(8)}. Reading {\tt /dev/kbd} gives
keycodes and the {\tt c} messages are what we want to process. Besides
handling the event produced by the operating system, we need also to
ensure other programs that read {\tt /dev/kbd} can acquire the
translated result by faking a new keyboard device to shadow the
original one.

@.kbdfs 8@>
@<Setup keyboard@>=
if((kbd = open("/dev/kbd", OREAD)) < 0)@|
  sysfatal("open kbd: %r");
@<Shadowing {\tt/dev/kbd}@>@;

@ @<Variables@>=
int kbd, newkbd;

@ The call |bind("#|", dir, MREPL)| would create a pair of pipes named
{\tt data} and {\tt data1} in |dir|.

@<Shadowing...@>=
if(bind("#|", "/n/temp", MREPL) < 0)@|
  sysfatal("bind /n/temp: %r");
if((newkbd = open("/n/temp/data1", OWRITE)) < 0)@|
  sysfatal("open kbd pipe: %r");
if(bind("/n/temp/data", "/dev/kbd", MREPL) < 0)@|
  sysfatal("bind kbd pipe: %r");
unmount(nil, "/n/temp");

@ This loop would be used to intercept input and output translated
result. When the start of the message is |'c'| the following stting
contains one character.  The function |nextstate()| would output runes
to |newkbd| when possible.

@<Key translation loop@>=
while ((n = read(kbd, buf, sizeof(buf))) > 0) {
  buf[n-1] = 0;
  if(n < 2 || buf[0] != 'c')@|
    write(newkbd,buf,n);
  else nextstate(buf, n, newkbd);
}

@ @<Variables@>=
int n;
char buf[128];

@*Dictionary. The dictionary is defined as several C arrays in a
separated file named {\tt dict.c}. It has to be generated by a
external program and that detail would not be discussed here.  The
requirement for dictionary is that all entries need to be sorted with
lexicographical order. An |int| is used as a key to encode the string
of at most three alphabets.

@f Rune char /* |Rune| is the type for Unicode characters */
@<Declarations@>=
typedef	struct	Map Map;
struct	Map {
 int    key;
 Rune   *val;
};
extern const Map dict[];

@ There are two precomputed indices to speed up lookup.
@<Declarations@>=
extern const int lvl1[];
extern const int lvl2[];

@ |lvl1| contains indices from |"a"| to |"z"|.
@<Lookup in |lvl1|@>=
return &dict[lvl1[key[0]-'a']];

@ |lvl2| contains indices from |"aa"|, |"ab"| to |"zz"|. Since it is
likely the indices are sparse, a negative value indicates not found.

@<Lookup in |lvl2|@>=
{
int index = lvl2[(key[0]-'a')*26+(key[1]-'a')];
return (index < 0) ? nil : &dict[index];
}

@ Find in the dictionary. Since the indices are very sparse after
level 2, only linear search would be needed. The optional argument
|table| can be used to resume a search.

@<Subroutines@>=
const Map *match(const char* key, int len, const Map* table)
{
  switch(len){
    case 1:
      @<Lookup in |lvl1|@>@;
    case 2:
      @<Lookup in |lvl2|@>@;
    default:
      if (table != nil)
        @<Linear search@>@;
      else
        @<Speedup with |lvl2|@>@;
  }
}

@ @<Speedup with |lvl2|@>=
return match(key, len, match(key, 2, nil));

@ Since the keys are ordered, if the compare result is negative,
return |nil|. Only codes after the first two characters are stored in
the key. If the key's length is less than 3 it is only stored as |0|.

@<Linear search@>=
{
int cmp;
int keycode;
@<Compute keycode@>@;
for @<First none |0| key until next |0|@> {
  cmp = (table[i].key >> (5 * (3 - (len - 2)))) - keycode;
  if (cmp == 0) return &table[i];
  else if (cmp < 0) return nil;
}
return nil;
}

@ Each alphabet occupies 5 bits. There are 3 alphabets in the keycode.
@<Compute keycode@>=
keycode = 0;
for (int i = 2; i < len; i++) {
  keycode <<= 5;
  keycode += key[i] - 'a' + 1;
}

@ When a two character key doesn't exist, the index for next key with
same prefix is stored.
@<First none...@>=
(int i = (table[0].key) ? 0 : 1; table[i].key; i++)

@*State machine. Right now this is a bit of messy.

@<Subroutines@>=
void nextstate(char *oldbuf, int n, int outfd) {

   if (len > 5) {
      len = 0;
      laststate = nil;
   }
   if (!natural && islower(oldbuf[1])) {
     @<Copy input character to input buffer@>@;
     @<Search and output@>@;
     return;
   } else @<Handle mode change and selection@>@;
   @<Directly write to output@>@;
   return;
}

@ @<Handle mode change...@>=
if (oldbuf[1] == 20) { // {\tt \^{}T}
   natural = 1;
   @<Reset state@>@;
   return;
}
else if (oldbuf[1] == 14) { // {\tt \^{}N}
   natural = 0;
   @<Reset state@>@;
   return;
}
else if (!natural && oldbuf[1] == ' ') {
   @<Reset state@>@;
   return;
}
else if (oldbuf[1] == 12) {// {\tt \^{}L}
   @<Rotate selection@>@;
   return;
}
else if (oldbuf[1] == '\b') {
   @<Reset state@>@;
}

@ This indicates the English/Chinese state.
@<Global states@>=
int natural = 0;

@ The only |goto| statement in this program. It enables auto start
of next input if the last character is already complete.
@<Search and...@>=
const Map *result;
Search:
result = match(input, len, laststate);
if (result != nil) {
   if (laststate != nil)
     @<Backspace@>@;
   candidate = result->val;
   @<Write current candidate@>@;
   laststate = result;
} else if (laststate != nil) {
   @<Flush input buffer...@>@;
   laststate = nil;
   goto Search;
}

@ @<Rotate selection@>=
if (laststate != nil) {
   @<Backspace@>@;
   candidate++;
   if (!*candidate) candidate = laststate->val;
   @<Write current candidate@>@;
}

@ @<Global states@>=
Rune *candidate;

@ @<Write current candidate@>=
char buf[128];
int n = snprint(buf, sizeof(buf), "c%C", *candidate) + 1;
write(outfd, buf, n);

@ @<Backspace@>=
write(outfd, "c\b\0", 3);

@ These states keeps a record of current input.
@<Global states@>=
char input[20];
int len = 0;
const Map *laststate = nil;

@ @<Reset state@>=
len = 0;
laststate = nil;

@ @<Copy input character...@>=
input[len] = oldbuf[1];
len += 1;
input[len]='\0';

@ @<Flush input buffer until the but last character@>=
input[0] = input[len-1];
input[1]= '\0';
len = 1;

@ @<Directly...@>=
write(outfd,oldbuf,n);

@*Program entry. A standard Plan9 C program would use these headers.
@c
#include <u.h>
#include <libc.h>
#include <ctype.h>

@ The |main()| function starts the daemon process and call |exits(nil)|.

@c
@<Declarations@>@;
@<Global states@>@;
@<Subroutines@>@;
void main(int argc, char **argv) {
  @<Variables@>;

  USED(argc);
  USED(argv);

  @<Setup keyboard@>@;
  if(fork()) exits(nil);

  @<Key translation loop@>@;
}

@*Bugs. The translation could fail if typing is too fast.
