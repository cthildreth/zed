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
    int myid, max_Size, size, count;
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);/*send an integer X to muit. processors*/
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
   
      if (myid == 0  )
	{
	  photo = setup ( argc,  argv);
	  max_Size = photo->width * photo->height * 3;
	  count = max_Size / size;
	}


    start_time = MPI_Wtime ();
    printf("max_Size: %d,numproc: %d count: %d, Start: %.2lf \n\n", max_Size, size, count, start_time);


    /*process_data (photo);*/
    /*printf("start: %d\n\n", start_time);*/
    /*end_time = MPI_Wtime ();*/


    /*MPI Brocast Function */ 
    MPI_Bcast(&max_Size, 1, MPI_INT, 0, MPI_COMM_WORLD );
    MPI_Bcast(&count, 1, MPI_INT, 0, MPI_COMM_WORLD );
    /*local_data = (unsingned char * ) malloc (&photo) *   ???????  Problem */

/*----------------------------MPI_SCatter Function--------------------------------- */
    MPI_Scatter(photo, count, MPI_INT, &photo,count, MPI_INT, 0, MPI_COMM_WORLD );
    MPI_Bcast(&count, 1, MPI_INT, 0, MPI_COMM_WORLD );
    process_data (photo);


/*-----------------------------MPI_Gather Function--------------------------------- */
   MPI_Gather(photo, count, MPI_INT, &photo,count, MPI_INT, 0, MPI_COMM_WORLD );
   process_data (photo);
   end_time = MPI_Wtime ();



    if (myid == 0  )
	{
	  cleanup (photo, argv);
	  printf ("p = %d, elapsed Time = %.2lf s.\n", size, end_time - start_time); 
	  /*count = max_Size / size;*/
	}


    MPI_Finalize();
    return 0;
}
