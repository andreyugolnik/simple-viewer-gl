/* OS-specific IO functions for xcftools
 *
 * This file was written by Henning Makholm <henning@makholm.net>
 * It is hereby in the public domain.
 *
 * In jurisdictions that do not recognise grants of copyright to the
 * public domain: I, the author and (presumably, in those jurisdictions)
 * copyright holder, hereby permit anyone to distribute and use this code,
 * in source code or binary form, with or without modifications. This
 * permission is world-wide and irrevocable.
 *
 * Of course, I will not be liable for any errors or shortcomings in the
 * code, since I give it away without asking any compenstations.
 *
 * If you use or distribute this code, I would appreciate receiving
 * credit for writing it, in whichever way you find proper and customary.
 */

#include "xcftools.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#if HAVE_MMAP
#include <sys/mman.h>
#endif

static FILE* xcfstream = 0;

void free_or_close_xcf(void)
{
    if (xcf_file)
    {
        if (xcfstream)
        {
            munmap(xcf_file, xcf_length);
            fclose(xcfstream);
            xcf_file = 0;
            xcfstream = 0;
        }
        else
        {
            free(xcf_file);
            xcf_file = 0;
        }
    }
}

void read_or_mmap_xcf(const char* filename, const char* unzipper)
{
    struct stat statbuf;

    free_or_close_xcf();

    if (strcmp(filename, "-") != 0)
    {
        if (access(filename, R_OK) != 0)
        {
            FatalGeneric(21, "!%s", filename);
        }
    }

    if (!unzipper)
    {
        const char* pc;
        pc = filename + strlen(filename);
        if (pc - filename > 2 && strcmp(pc - 2, "gz") == 0)
        {
            unzipper = "zcat";
        }
        else if (pc - filename > 3 && strcmp(pc - 3, "bz2") == 0)
        {
            unzipper = "bzcat";
        }
        else
        {
            unzipper = "";
        }
    }
    else if (strcmp(unzipper, "cat") == 0)
    {
        unzipper = "";
    }

    if (*unzipper)
    {
        int pid, status, outfd;
#if HAVE_MMAP
        xcfstream = tmpfile();
        if (!xcfstream)
        {
            FatalUnexpected(_("!Cannot create temporary unzipped file"));
        }
        outfd = fileno(xcfstream);
#else
        int fh[2];
        if (pipe(fh) < 0)
        {
            FatalUnexpected("!Cannot create pipe for %s", unzipper);
        }
        xcfstream = fdopen(fh[1], "rb");
        if (!xcfstream)
        {
            FatalUnexpected("!Cannot fdopen() unzipper pipe");
        }
        outfd = fh[0];
#endif
        if ((pid = fork()) == 0)
        {
            /* We're the child */
            if (dup2(outfd, 1) < 0)
            {
                perror("Cannot dup2 in unzip process");
                exit(127);
            }
            fclose(xcfstream);
            execlp(unzipper, unzipper, filename, NULL);
            fprintf(stderr, _("Cannot execute "));
            perror(unzipper);
            exit(126);
        }
#if HAVE_MMAP
        while (wait(&status) != pid)
            ;
        if (WIFEXITED(status))
        {
            status = WEXITSTATUS(status);
            if (status > 0)
            {
                fclose(xcfstream);
                xcfstream = 0;
                FatalGeneric(status, NULL);
            }
        }
        else
        {
            fclose(xcfstream);
            xcfstream = 0;
            FatalGeneric(126, _("%s terminated abnormally"), unzipper);
        }
#else
        close(fh[0]);
#endif
    }
    else if (strcmp(filename, "-") == 0)
    {
        xcfstream = fdopen(dup(0), "rb");
        if (!xcfstream)
        {
            FatalUnexpected("!Cannot dup stdin for input");
        }
    }
    else
    {
        xcfstream = fopen(filename, "rb");
        if (!xcfstream)
        {
            FatalGeneric(21, _("!Cannot open %s"), filename);
        }
    }
    /* OK, now we have an open stream ... */
    if (fstat(fileno(xcfstream), &statbuf) == 0 && (statbuf.st_mode & S_IFMT) == S_IFREG)
    {
        xcf_length = statbuf.st_size;
#if HAVE_MMAP
        xcf_file = (uint8_t*)mmap(0, xcf_length, PROT_READ, MAP_SHARED, fileno(xcfstream), 0);
        if (xcf_file != (void*)-1)
        {
            return;
        }
        if (errno != ENODEV)
        {
            int saved = errno;
            fclose(xcfstream);
            xcf_file = 0;
            errno = saved;
            FatalUnexpected("!Could not mmap input");
        }
#endif
        xcf_file = (uint8_t*)malloc(xcf_length);
        if (xcf_file == 0)
        {
            FatalUnexpected(_("Out of memory for xcf data"));
        }
        if (fread(xcf_file, 1, xcf_length, xcfstream) != xcf_length)
        {
            if (feof(xcfstream))
            {
                FatalUnexpected(_("XCF file shrunk while reading it"));
            }
            else
            {
                FatalUnexpected(_("!Could not read xcf data"));
            }
        }
        fclose(xcfstream);
        xcfstream = 0;
    }
    else
    {
        size_t blocksize = 0x80000; /* 512 KB */
        xcf_length = 0;
        xcf_file = 0;
        while (1)
        {
            xcf_file = (uint8_t*)realloc(xcf_file, blocksize);
            if (xcf_file == 0)
            {
                FatalUnexpected(_("Out of memory for xcf data"));
            }
            size_t actual = fread(xcf_file + xcf_length, 1, blocksize - xcf_length,
                                  xcfstream);
            xcf_length += actual;
            if (feof(xcfstream))
            {
                break;
            }
            if (xcf_length < blocksize)
            {
                FatalUnexpected(_("!Could not read xcf data"));
            }
            blocksize += (blocksize >> 1) & ~(size_t)0x3FFF; /* 16 KB granularity */
        }
        fclose(xcfstream);
        xcfstream = 0;
    }
}
