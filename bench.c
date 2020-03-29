/* EXAMPLE: Benchmark algorithm, Al Grant */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static unsigned char const PWTAB1[64] = {
        0x48,0x93,0x46,0x67,0x98,0x3D,0xE6,0x8D,
        0xB7,0x10,0x7A,0x26,0x5A,0xB9,0xB1,0x35,
        0x6B,0x0F,0xD5,0x70,0xAE,0xFB,0xAD,0x11,
        0xF4,0x47,0xDC,0xA7,0xEC,0xCF,0x50,0xC0,
        0x48,0x93,0x46,0x67,0x98,0x3D,0xE6,0x8D,
        0xB7,0x10,0x7A,0x26,0x5A,0xB9,0xB1,0x35,
        0x6B,0x0F,0xD5,0x70,0xAE,0xFB,0xAD,0x11,
        0xF4,0x47,0xDC,0xA7,0xEC,0xCF,0x50,0xC0
};

unsigned char X32[7] = {
  0x48, 0x58, 0x78, 0xB8, 0xC8, 0xD8, 0xF8
};

static unsigned long nCalcs;

unsigned int Reverse2 (unsigned char x[96])
{
  unsigned int nFound = 0;
  unsigned char a[64];
  unsigned int na32reject = 0;

  a[32] = 0;
  do {

      unsigned int sp = 0;
      unsigned int stack[64];
      unsigned int i, j;

      for (i = 32; i < 63; ++i)
          a[i+1] = a[i] + x[i+32];

      for (j = 0; j < 7; ++j) {
          unsigned char cand = X32[j];
          unsigned int xp = 32;
          for (;;) {
              unsigned int xpp = xp + (a[xp]&31);
              if (a[xp]&31) {
                  cand = (x[xp+32] ^ (cand+a[xp])) + PWTAB1[xp];
                  xp = xpp;
                  if (xp < 64)
                      continue;
              } else {
                  cand = (cand+a[xp]) ^ (cand-PWTAB1[xp]);
                  xp = xp + 32;

              }
              break;
          }
          if (x[xp] == cand)
              goto a32ok;
      }
      ++na32reject;
      goto nexta;

a32ok:
      {
          unsigned int t = 64;

nextt:
          --t;
          if (a[t]&31) {
              ++nCalcs;
              x[t] = (x[t+32] ^ (x[t+(a[t]&31)]-PWTAB1[t])) - a[t];
gotit:
              if (t >= 32) {
                  a[t-32] = a[t-31] - x[t];
                  if (t == 32 && a[0] != 0)
                      goto nextfail;
                  if (!(a[t-32]&31) && ((x[t]-a[t-32]-PWTAB1[t]) & ~(x[t]<<1) &
0xFF))
                      goto nextfail;
              }
              if (t != 0)
                  goto nextt;
              ++nFound;
              goto nextfail;
          } else {
              x[t] = 0;
              do {
                  if (x[t+32] == (unsigned char)((x[t]+a[t])^(x[t]-PWTAB1[t])))
{
                      stack[sp++] = t;
                      goto gotit;
                  }
incrit:
                  ++x[t];
              } while (x[t]);
          }

nextfail:
          if (sp) {
              t = stack[--sp];
              goto incrit;
          }
      }

nexta:
      ++a[32];
  } while (a[32]);

  return nFound;
}

int main (int argc, char **argv)
{
    typedef unsigned int u32;
    u32 i;
    u32 nIters = 200;   /* 200 is a good number for simulated */
    unsigned long total = 0;
    unsigned char x[96];
    clock_t start = clock();
    clock_t end;

#if 1
    if (argv[1]) {
        nIters = atol(argv[1]);
    }
#endif

    for (i = 0; i < nIters; ++i) {
        unsigned int j;
        for (j = 0; j < 32; ++j) {
            x[64+j] = (i ^ j);
        }
        total += Reverse2(x);
    }
    end = clock();
    /* For 1000 iterations we expect 928 */
    printf("** " __TIME__ " total: %lu\n",total);
    printf("** Took %u ticks\n", (unsigned int)(end - start));
    return 0;
}

