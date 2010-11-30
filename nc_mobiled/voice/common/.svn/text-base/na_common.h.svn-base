#ifndef NA_COMMON_H
#define NA_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include <time.h>
#include <pthread.h>

#include "na_sync.h"
#include "na_time.h"
#include "na_misc.h"


#define NA_MAGIC                12345

#define MAX_GIANT_BUFFER_SIZE   20480
#define MAX_HUGE_BUFFER_SIZE    10240
#define MAX_LARGE_BUFFER_SIZE   2048
#define MAX_BIG_BUFFER_SIZE     1024
#define MAX_MODEST_BUFFER_SIZE  512
#define MAX_NORMAL_BUFFER_SIZE  256
#define MAX_SMALL_BUFFER_SIZE   128
#define MAX_LITTLE_BUFFER_SIZE  64
#define MAX_TINY_BUFFER_SIZE    32

#if !defined (LEAK)

#define MEM_ALLOC(p,q,r) \
  { \
    p = r malloc (q); \
    if (p) { \
      memset (p, 0, q); \
    } \
  }

#define MEM_CLEAN(p) \
  if (p) { \
    free (p); \
    p = 0; \
  }

#define STR_ALLOC(p,q) \
  if (q) { \
    MEM_ALLOC(p,strlen(q)+1,(char *)) \
    if (p) { \
      strcpy (p, q); \
    } \
  }

#define STR_CLEAN(p) \
  MEM_CLEAN(p)

#define OPEN(p,q,r) \
  p = open (q, r)

#define CLOSE(p) \
  if (0 < p) { \
    close (p); \
    p = 0; \
  }

#define CLOSE_EX(p,q) \
  if (0 < q) { \
    p = close (q); \
    q = 0; \
  } \
  else { \
    p = 0; \
  }

#define FOPEN(p,q,r) \
  p = fopen (q, r)

#define FCLOSE(p) \
    if (0 < p) { \
      fclose (p); \
      p = 0; \
    } \

#define FCLOSE_EX(p,q) \
  if (0 < q) { \
    p = fclose (q); \
    q = 0; \
  } \
  else { \
    p = 0; \
  }

#define FREAD(a,b,c,d) \
  fread (a, b, c, d)

#define FWRITE(a,b,c,d) \
  fwrite (a, b, c, d)

#define FEOF(a) \
  feof (a)

#define FERROR(a) \
  ferror (a)

#define SOCK_ALLOC(p,q,r,s) \
  p = socket (q, r, s)

#define SOCK_ACCEPT(p,q,r,s) \
  p = accept (q, r, (socklen_t *) s)

#define SOCK_CLOSE(p) \
  if (0 < p) { \
    close (p); \
    p = -1; \
  }

#define SOCK_CLOSE_EX(p,q) \
  if (0 < q) { \
    p = close (q); \
    q = -1; \
  } \
  else { \
    p = 0; \
  }
/*
#else

#define MEM_ALLOC(p,q,r) \
  p = r malloc (q); \
  if (p) { \
    memset (p, 0, q); \
    printf ("ALLOC,0x%p,%u,%s\n", p, __LINE__, __FILE__); \
  }

#define MEM_CLEAN(p) \
  if (p) { \
    free (p); \
    printf ("CLEAN,0x%p,%u,%s\n", p, __LINE__, __FILE__); \
    p = 0; \
  }

#define STR_ALLOC(p,q) \
  if (q) { \
    MEM_ALLOC(p,strlen(q)+1,(char *)); \
    if (p) { \
      strcpy (p, q); \
    } \
  }

#define STR_CLEAN(p) \
  MEM_CLEAN(p)

#define OPEN(p,q,r) \
  p = open (q, r); \
  printf ("OPEN,0x%08X,%u,%s\n", p, __LINE__, __FILE__)

#define CLOSE(p) \
  if (0 < p) { \
    close (p); \
    printf ("CLOSE,0x%08X,%u,%s\n", p, __LINE__, __FILE__); \
    p = 0; \
  }

#define CLOSE_EX(p,q) \
  if (0 < q) { \
    p = close (q); \
    printf ("CLOSE,0x%08X,%u,%s\n", q, __LINE__, __FILE__); \
    q = 0; \
  } \
  else { \
    p = 0; \
  }

#define FOPEN(p,q,r) \
  p = fopen (q, r); \
  printf ("FOPEN,0x%p,%u,%s\n", p, __LINE__, __FILE__)

#define FCLOSE(p) \
  if (0 < p) { \
    fclose (p); \
    printf ("FCLOSE,0x%p,%u,%s\n", p, __LINE__, __FILE__); \
    p = 0; \
  }

#define FCLOSE_EX(p,q) \
  if (0 < q) { \
    p = fclose (q); \
    printf ("FCLOSE,0x%p,%u,%s\n", q, __LINE__, __FILE__); \
    q = 0; \
  } \
  else { \
    p = 0; \
  }

#define FREAD(a,b,c,d) \
  fread (a, b, c, d)

#define FWRITE(a,b,c,d) \
  fwrite (a, b, c, d)

#define FEOF(a) \
  feof (a)

#define FERROR(a) \
  ferror (a)

#define SOCK_ALLOC(p,q,r,s) \
  p = socket (q, r, s); \
  printf ("SOCK_ALLOC,0x%08X,%u,%s\n", p, __LINE__, __FILE__)

#define SOCK_ACCEPT(p,q,r,s) \
  p = accept (q, r, (socklen_t *) s); \
  printf ("SOCK_ACCEPT,0x%08X,%u,%s\n", p, __LINE__, __FILE__)

#define SOCK_CLOSE(p) \
  if (0 < p) { \
    close (p); \
    printf ("SOCK_CLOSE,0x%08X,%u,%s\n", p, __LINE__, __FILE__); \
    p = -1; \
  }

#define SOCK_CLOSE_EX(p,q) \
  if (0 < q) { \
    p = close (q); \
    printf ("SOCK_CLOSE,0x%08X,%u,%s\n", q, __LINE__, __FILE__); \
    q = -1; \
  } \
  else { \
    p = 0; \
  }
*/
#endif

#ifdef __cplusplus
}
#endif

#endif
