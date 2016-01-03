#include <stdio.h>

#include "image.h"
#include "utils.h"
#include "mpi.h"

void process_data (image *photo)
{
    int i, n;

    n = photo->width * photo->height * 3;
    
    for (i = 0; i < n; i++)
    {
        photo->data [i] = negate (photo->data [i], photo->max_value);
    }
}

image *setup (int argc, char **argv)
{
    image *photo;

    if (argc < 3)
    {
        fprintf (stderr, "Usage: %s <infile> <outfile>\n\n", argv [0]);
        return NULL;
    }

    photo = read_image (argv [1]);
    if (photo == NULL)
    {
        fprintf (stderr, "Unable to read input file %s\n\n", argv [1]);
        return NULL;
    }

    return photo;
}

void cleanup (image *photo, char **argv)
{
    int rc = write_image (argv [2], photo);
    if (!rc)
    {
        fprintf (stderr, "Unable to write output file %s\n\n", argv [2]);
    }

    clear_image (photo);
}

int main (int argc, char **argv)
{
    image *photo;
    double start_time, end_time;
    int id, size, maxSize, count
    MPI_Init (&argc, &argv);
    MPI_COMM_size (MPI_COMM_WORLD, &numprocs);
    MPI_COMM_size (MPI_COMM_WORLD, &myid);

    
    photo = setup (argc, argv);
    start_time = MPI_Wtime ();
    process_data (photo);
    end_time = MPI_Wtime ();
    cleanup (photo, argv);
    printf ("Sequential elapsed time - %.2lf s.\n", end_time - start_time);
    
    MPI_Finalize ();

    return 0;
}
