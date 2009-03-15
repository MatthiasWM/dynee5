
#include <stdio.h>

typedef unsigned int uint32_t;
typedef unsigned char uchar;


void dump_unknown(const char *src, const char *dst, uint32_t start, uint32_t end)
{
  FILE *in = fopen(src, "rb");
  if (!in) {
    fprintf(stderr, "Can't open input file %s\n", src);
    return;
  }
  FILE *out = fopen(dst, "wb");
  if (!out) {
    fclose(in);
    fprintf(stderr, "Can't open output file %s\n", dst);
    return;
  }
  fprintf(out, 
    "\t\t.arm\n"
    "\t\t.code\t32\n\n"
    "\t\t.macro unknown v\n\t\t.long\t\\v\n\t\t.endm\n\n"
    "\t\t.text\n"
    "\t\t.align\t4\n\n"
    "\t\t.org\t0x%08x\n\n", start);

  uint32_t i;
  for (i=start; i<end; i+=4) {
    uint32_t v = 0;
    v  = fgetc(in)<<24;
    v |= fgetc(in)<<16;
    v |= fgetc(in)<<8;
    v |= fgetc(in);
    fprintf(out, "\t\tunknown\t0x%08x\n", v);
  }

  fclose(in);
  fclose(out);
}


int main(int argc, char **argv) 
{
  const char *src = argv[1];
  const char *dst = argv[2];
  uint32_t start = 0;
  sscanf(argv[3], "%x", &start);
  uint32_t end = 0;
  sscanf(argv[4], "%x", &end);

  dump_unknown(src, dst, start, end);

  return 0;
}

