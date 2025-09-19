#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <getopt.h>

#define PAGE_SIZE 4096

// Function prototype for shuffle
void shuffle(uint64_t *array, size_t n);
void writeFirstByteOfEachPage(char **pagesAddresses, int numPages, char value);

static inline double elapsed_sec(struct timespec a, struct timespec b)
{
    return (b.tv_sec - a.tv_sec) + (b.tv_nsec - a.tv_nsec) / 1e9;
}

static void usage(const char *prog)
{
    fprintf(stderr,
            "Usage: %s --backing {file|anon} --share {shared|private} --order {random|sequential}\n",
            prog);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    enum
    {
        BACK_FILE,
        BACK_ANON
    } backing = BACK_FILE;
    int share_flag = MAP_SHARED; // or MAP_PRIVATE
    bool do_random = true;

    static struct option opts[] = {
        {"backing", required_argument, 0, 'b'}, // file|anon
        {"share", required_argument, 0, 's'},   // shared|private
        {"order", required_argument, 0, 'o'},   // random|sequential
        {0, 0, 0, 0}};

    int c;
    while ((c = getopt_long(argc, argv, "b:s:o:", opts, NULL)) != -1)
    {
        switch (c)
        {
        case 'b':
            if (strcmp(optarg, "file") == 0)
                backing = BACK_FILE;
            else if (strcmp(optarg, "anon") == 0)
                backing = BACK_ANON;
            else
                usage(argv[0]);
            break;
        case 's':
            if (strcmp(optarg, "shared") == 0)
                share_flag = MAP_SHARED;
            else if (strcmp(optarg, "private") == 0)
                share_flag = MAP_PRIVATE;
            else
                usage(argv[0]);
            break;
        case 'o':
            if (strcmp(optarg, "random") == 0)
                do_random = true;
            else if (strcmp(optarg, "sequential") == 0)
                do_random = false;
            else
                usage(argv[0]);
            break;
        default:
            usage(argv[0]);
        }
    }

    int fd;
    int flags = share_flag;

    if (backing == BACK_FILE)
    {
        fd = open("file-1g", O_RDWR);
        if (fd < 0)
        {
            perror("open");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        fd = -1;
        flags |= MAP_ANONYMOUS;
    }
    
    int len = 1 << 30;
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    void *p = mmap(NULL, len, PROT_READ | PROT_WRITE, flags, fd, 0);
    if (p == MAP_FAILED)
    {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    int numPages = len / PAGE_SIZE;
    char **pagesAddresses = malloc(numPages * sizeof(char *));
    for (int i = 0; i < numPages; i++) // load the starting addresses of all pages into the array
    {
        pagesAddresses[i] = (char *)p + i * PAGE_SIZE;
    }

    if (do_random)
    {
        shuffle((uint64_t *)pagesAddresses, numPages); // shuffle the array to access pages in random order
    }
    else
    {
    }

    writeFirstByteOfEachPage(pagesAddresses, numPages, 'B');
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("%f seconds\n", elapsed_sec(start, end));

    free(pagesAddresses);
    msync(p, len, MS_SYNC);
    return 0;
}

void writeFirstByteOfEachPage(char **pagesAddresses, int numPages, char value)
{
    for (int i = 0; i < numPages; i++)
    {
        pagesAddresses[i][0] = value; // write to the first byte of each page
    }
}

void shuffle(uint64_t *array, size_t n)
{
    if (n > 1)
    {
        size_t i;
        for (i = 0; i < n - 1; i++)
        {
            size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
            uint64_t t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}