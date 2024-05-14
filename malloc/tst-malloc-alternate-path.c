/* Test that malloc uses mmap when sbrk or brk fails.
   This code returns success when there is an obstruction setup
   and sbrk() fails to grow the heap size forcing malloc to use mmap().
   Copyright (C) 2024 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define LARGE_SIZE (10 * (1 << 20)) // 10 MB

static int
do_test (void)
{
  /* Get current program break */
  void *current_brk = sbrk (0);

  /* Try to extend the heap beyond the obstruction using sbrk */
  int *ptr = (int *) sbrk (sizeof (int));
  if (ptr != (void *) -1)
    {
      fprintf (stderr, "memory allocation can be done using sbrk.\n");
      free (ptr);
      return 1;
    }

  /* Attempt to allocate memory using malloc */
  void *memptr = malloc (LARGE_SIZE);
  if (memptr == NULL)
    {
      perror ("malloc");
      return 1;
    }

  printf ("malloc used alternate path to allocate memory\n");

  /* Get program break after malloc */
  void *new_brk = sbrk (0);

  /* Check if malloc changed program break */
  if (current_brk != new_brk)
    {
      fprintf (stderr, "malloc changed program break\n");
      free (memptr);
      return 1;
    }

  free (memptr);
  return 0;
}

#include <support/test-driver.c>
