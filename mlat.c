#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

/*  Copyright 2013 Chris Studholme.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


static void basic_test(const unsigned short* ptr, 
                       const unsigned short* buf_end, 
                       size_t nbuf, size_t nsteps) {
    size_t i;
    fprintf(stdout,"starting basic test...\n");
    clock_t start = clock();
    for (i = nsteps; i > 0; --i) {
        ptr += *ptr;
        ptr += 65536;
        if (ptr >= buf_end)
            ptr -= nbuf;
    }
    int latency = (int)round((clock()-start)/(double)CLOCKS_PER_SEC*1e9/nsteps);
    fprintf(stdout,"latency: %dns [%d]\n",latency,*ptr);
}

static void prefetch_test(const unsigned short* ptr, 
                          const unsigned short* buf_end, 
                          size_t nbuf, size_t nsteps,
                          unsigned nprefetch) {
    size_t i,pi;
    const unsigned short** ptr_buf = malloc(nprefetch*sizeof(unsigned short*));
    for (i = 0; i < nprefetch; ++i)
        ptr_buf[i] = ptr;
    pi = 0;
    fprintf(stdout,"starting prefetch[%d] test...\n",nprefetch);
    clock_t start = clock();
    for (i = nsteps; i > 0; --i) {
        ptr += *(ptr_buf[pi]);
        ptr += 65536;
        if (ptr >= buf_end)
            ptr -= nbuf;
        __builtin_prefetch(ptr,0,0);
        ptr_buf[pi] = ptr;
        if (++pi >= nprefetch)
            pi -= nprefetch;
    }
    int latency = (int)round((clock()-start)/(double)CLOCKS_PER_SEC*1e9/nsteps);
    fprintf(stdout,"latency: %dns [%d]\n",latency,*ptr);
    free(ptr_buf);
}

int main(int argc, const char* argv[]) {

    assert(sizeof(short) == 2);

    size_t buf_bytes = 64*1024*1024;
    if (argc >= 2) {
        buf_bytes = atoi(argv[1])*1024*1024;
        assert(buf_bytes > 0);
    }
    assert((buf_bytes&(buf_bytes-1)) == 0);
    fprintf(stdout,"buffer size: %d MiB\n",(int)(buf_bytes/1024/1024));
    
    const size_t buf_short = buf_bytes / 2;

    unsigned short* buf = malloc(buf_bytes);
    unsigned short* buf_end = buf + buf_short;

    srandom(1234);

    size_t i;
    for (i = 0; i < buf_short; ++i)
        buf[i] = random();

    basic_test(buf,buf_end,buf_short,buf_short);

    prefetch_test(buf,buf_end,buf_short,buf_short,2);
    prefetch_test(buf,buf_end,buf_short,buf_short,3);
    prefetch_test(buf,buf_end,buf_short,buf_short,4);
    prefetch_test(buf,buf_end,buf_short,buf_short,6);
    prefetch_test(buf,buf_end,buf_short,buf_short,8);
    prefetch_test(buf,buf_end,buf_short,buf_short,12);
    prefetch_test(buf,buf_end,buf_short,buf_short,16);

    free(buf);
    return 0;
}

