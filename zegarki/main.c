#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>


long long* liczczas(struct timespec a, struct timespec b, long long* dana);
int main() {

    int clock[]={CLOCK_REALTIME, CLOCK_MONOTONIC, CLOCK_PROCESS_CPUTIME_ID, CLOCK_THREAD_CPUTIME_ID,
                 CLOCK_MONOTONIC_RAW, CLOCK_REALTIME_COARSE, CLOCK_MONOTONIC_COARSE, CLOCK_BOOTTIME,
                 CLOCK_REALTIME_ALARM, CLOCK_BOOTTIME_ALARM, CLOCK_TAI};

/*    for(int i=0; i<11; i++)
    {
        printf("%d.  %d\n", i, clock[i]);
    }*/

    long long srednia[10][2];
    struct timespec start[11]={0};
    struct timespec finish[11]={0};

    for(int r=0; r<10; r++) {
        for (int i = 0; i < 10; i++) {
            int k = clock_gettime(clock[i], &start[i]);
            int j = clock_gettime(clock[i], &finish[i]);

            if (k == -1 && j == -1) {
                int err = errno;
                if (err == EINVAL) {
                    fprintf(stderr, "zegar %d nie istnieje\n", i);
                } else {
                    int err = errno;
                    fprintf(stderr, "%s", strerror(err));
                }
            }
            long long *dana = (long long *) malloc(2 * sizeof(long long));
            dana = liczczas(start[i], finish[i], dana);
            srednia[i][0] += dana[0];
            srednia[i][1] += dana[1];
           // printf("%d. sec: %lld nsec: %lld \n", i, dana[0], dana[1]);
            free(dana);
        }
    }
    for(int i=0; i<10; i++)
    {
        printf("%d. sec: %lld nsec: %lld \n", i, srednia[i][0], srednia[i][1]);
    }
    return 0;
}

long long* liczczas(struct timespec a, struct timespec b, long long* dana)
{
    long long sec, nsec;
    if(a.tv_sec==b.tv_sec)
    {
        sec=0;
        nsec=b.tv_nsec-a.tv_nsec;
    }
    else
    {
        long int h1=a.tv_sec*1000000000+a.tv_nsec;
        long int h2=b.tv_sec*1000000000+b.tv_nsec;
        long int wynik=h2-h1;
         sec=wynik/1000000000;
         nsec=wynik-sec*1000000000;
    }
    dana[0]=sec;
    dana[1]=nsec;
    return dana;
}

