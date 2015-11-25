//==========================================================================
//K CODEC PACKING ALGORITHM (C) 2015 K RING TECHNOLOGIES, SIMON P. JACKSON
//==========================================================================

/* This file can be used in LMMS (for audio sampl compression only)
 * under the following licence.

 * This file is part of LMMS - http://lmms.io
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program (see COPYING); if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.

*/

//The K codec is an interesting algorithm for data compression. A reversable
//LCG PRBS goes through a sequence, and has branch points in the sequence as
//data is modulating the stream. The nature of any branch point, and unique
//reverse detection, requires that a choice of which branch to follow is 
//decided mostly without user data choice. Ocassionally, it however does
//allow the opertunity for a bit decision to be placed (via what I term
//modulation) in the stream of branches, at no storage cost.

//Quite a lot of branches have to be sorted though to find what is refered
//to as a "strictly long" branch (where the decode is certain). This branch
//has the amazing property of unique decode, as the alternate paths of
//modulation will NEVER be confused in decode.

//"To make a rare event rarer still increases its self information. The rare
//event being detectable with a know statistical probability, can store more
//information by virtue of a deviation from this known probability. The
//deviation can be measured and the non modulated statistic restored, for
//further detection."

//The code is not that efficient, and has a bias for backup, not restore speed.
//It need a block at a time, with the final block truncated, and then the
//truncation length, and then the number of blocks storing.
//=============================================================================

//PRBS

/* (a^b)%m */
static int powmod(int a, int b, int m) {
  int x = 1, y = a;
  while(b > 0) {
    if(b%2 == 1) {
      x = (x*y);
      if(x > m) x %= m;
    }
    y = (y*y);
    if(y > m) y %= m;
    b /= 2;
  }
  return x;
}

static int phi(int m) {
  int p = 1;
  int f = 2;
  int lf = 1;//last factor
  while(f < 65536) {
    if(m%f == 0) {
      m /= f;
      if(lf == f) p *= f; else p *= (f-1);
      lf = f;
    } else {
      f += 1;//slow, but correct
    }
    if(m == 1) break; 
  }
  if(m != 1) p *= (m - 1);//remaining prime
  return p;
}
 
static int inverse(int a, int m) {
  return powmod(a, phi(m) - 1, m);
}

//sequence travel functions

//the SEED contains the compressed information statistic
static int seed = 7;

//these four are the sequencing control numbers, constants (sort of)
static int b[] = { 411, 713 };
static int i[2];
static int mod = 234567;
static bool done = false;

static void RevTravel(bool val) {
  if(!done) {
    i[0] = inverse(b[0], mod);
    i[1] = inverse(b[1], mod);
    done = true;//don't do more than once!
  }
  int a = i[0];
  int c = b[1];
  if(val) {
    a = i[1];
    c = b[0];
  }
  seed = powmod(a * (seed - c), 1, mod);
}

static void ForTravel(bool val) {
  int a = b[0];
  int c = b[1];
  if(val) {
    int t = a;
    a = c;
    c = t;
  }
  seed = powmod(a * seed + c, 1, mod);
}

static bool stateLong() {
  return (seed & 4) == 0;//any bit except LSB?
}

//branch coding functions

static void RevTravel0() {
  RevTravel(false);
}

static void ForTravel0() {
  ForTravel(false);
}

static void RevTravel1() {
  RevTravel(true);
}

static void ForTravel1() {
  ForTravel(true);
}

//strictly long test functions

static bool For0StrictShort() {
  //test??
  bool x = !stateLong();
  ForTravel0();
  RevTravel1();
  x &= stateLong();//strict short
  ForTravel1();
  RevTravel0();//back at start
  return x;
}

static bool Rev0StrictShort() {
  RevTravel0();
  //test??
  bool x = For0StrictShort();
  ForTravel0();
  return x;
}

static bool For1StrictLong() {
  //test??
  bool x = stateLong();
  ForTravel1();
  RevTravel0();
  x &= !stateLong();//strict long
  ForTravel0();
  RevTravel1();//back at start
  return x;
}

static bool Rev1StrictLong() {
  RevTravel1();
  //test??
  bool x = For1StrictLong();
  ForTravel1();
  return x;
}

//user data provider functions

static int buffer;
static int cnt;

static bool absorb() {
  cnt--;
  bool t = (buffer & 1) == 1;
  buffer >>= 1;
  return t;
}

static void emit(bool val) {
  cnt--;
  buffer <<= 1;
  buffer |= val?1:0;
}

//state machine progression functions

static void decompressOne() {
    if(Rev0StrictShort()) { 
        RevTravel0();
        if(For1StrictLong()) { emit(false); ForTravel0(); return; }
        ForTravel0();
        if(Rev1StrictLong()) { 
            RevTravel1();
            if(!For0StrictShort()) { emit(true); return; }
            else { ForTravel1(); RevTravel0(); return; }
        }
    } else { RevTravel0(); return; }
}

static void compressOne() {
    if(For1StrictLong()) { 
        if(For0StrictShort()) {
            if(absorb()) { ForTravel1(); return;}
            else { ForTravel0(); return; }
        } else {
            ForTravel1(); return;
        }
    } else { ForTravel0(); return; }
}   

//blat - do an int!

static void compress(int x) {
  buffer = x;
  cnt = 32;
  while(cnt > 0) {
    compressOne();
  }
}

static int decompress() {
  cnt = 32;
  while(cnt > 0) {
    decompressOne();
  }
  return buffer;
}

//===================================================
//END of packing algorithm
//===================================================
