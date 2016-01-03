#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "image.h"
#include "utils.h"
#include "mpi.h"

void process_data (image *photo)
{
    int i, n;
	printf("entering process_data\n");
	printf("CRAP: %d\n", photo->width);
    /* n = photo->width * photo->height * 3; */
	n = 786432;
	printf("value of n: %d\n", n);    
    for (i = 0; i < n; i++)
    {
        photo->data [i] = negate (photo->data [i], photo->max_value);
    }
}

image *setup (int argc, char **argv)
{
    image *photo;
	printf("entering setup()\n");
    if (argc < 3)
    {
        fprintf (stderr, "Usage: %s <infile> <outfile>\n\n", argv [0]);
        return NULL;
    }
	printf("reading image\n");
    photo = read_image (argv [1]);
    if (photo == NULL)
    {
        fprintf (stderr, "Unable to read input file %s\n\n", argv [1]);
        return NULL;
    }
	
	printf("%d\n", photo);
    return photo;
}

void cleanup (image *photo, char **argv)
{
		printf("almost made it!!!!\n");

    int rc = write_image (argv [2], photo);
    if (!rc)
    {
        fprintf (stderr, "Unable to write output file %s\n\n", argv [2]);
    }

    clear_image (photo);
}

int main (int argc, char **argv)
{

	image *data;
    image *photo;
    double start_time, end_time; 
    int myid, max_Size, size, count;
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);/*send an integer X to muit. processors*/
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
   
      if (myid == 0)
	{
	  printf("photo = setup...\n");
	  photo = setup ( argc,  argv);
	  printf("max size...\n");
	  max_Size = photo->width * photo->height * 3;
	  count = max_Size / size;
	}


/*---------------------------------------------------------------------------------------------------------- */

   
    

    start_time = MPI_Wtime ();
    printf("max_Size: %d, numproc: %d count: %d, Start: %.2lf \n\n", max_Size, size, count, start_time);/* %.2lf = double type*/


/*---------------------------------------------------------------------------------------------------------- */


  
    process_data (photo);
    printf("start: %d\n\n", start_time);
    end_time = MPI_Wtime ();



/*---------------------------------------------------------------------------------------------------------- */


    /*MPI Brocast Function */ 
    MPI_Bcast(&max_Size, 1, MPI_INT, 0, MPI_COMM_WORLD );
    MPI_Bcast(&count, 1, MPI_INT, 0, MPI_COMM_WORLD );
	printf("before data[], photo: %d, image: %d\n", sizeof(photo), sizeof(image));
    data = (image * )malloc (sizeof(image)); 
	printf("after data[]\n");
/*	photo = (image *) malloc (sizeof (image)); */

/*------------------------------------MPI_SCatter Function--------------------------------- */
    /* MPI_Scatter(photo, count, MPI_INT, &photo,count, MPI_INT, 0, MPI_COMM_WORLD );
    MPI_Bcast(&count, 1, MPI_INT, 0, MPI_COMM_WORLD );
    */ 
	process_data (photo);


/*------------------------------------MPI_Gather Function--------------------------------- */
   /* MPI_Gather(photo, count, MPI_INT, &photo,count, MPI_INT, 0, MPI_COMM_WORLD );
   */
   process_data (photo);
   end_time = MPI_Wtime ();



    if (myid == 0  )
	{
	  cleanup (photo, argv);
	  printf ("p = %d, elapsed Time = %.2lf s.\n", &size, end_time - start_time); 
	  /*count = max_Size / size;*/
	}


    MPI_Finalize();
    return 0;
}

/* prototype for nanosleep, this is required as nanosleep is not
   defined in ANSI C, it is a POSIX function instead */
int nanosleep (struct timespec *req, struct timespec *rem);

void delay (int usec)
{
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = usec * 1000 /* ns/us */;
    nanosleep (&ts, NULL);
}

image *read_image (const char *filename)
{
    image *photo = NULL;
    int width, height, max_value, n;
    char p6 [3];
    unsigned char dummy;

    FILE *file = fopen (filename, "r");
    if (file == NULL)
    {
        return NULL;
    }

    fscanf (file, "%s", p6);
    if (strcmp (p6, "P6") != 0)
    {
        fclose (file);
        return NULL;
    }

    fscanf (file, "%ud", &width);
	printf("WIDTH: %d\n", width);
    fscanf (file, "%ud", &height);
	printf("HEIGHT: %d\n", height);
    fscanf (file, "%ud", &max_value);
	printf("MAX VALUE: %d\n", max_value);

    n = width * height * 3; /* 3 bytes per pixel */

    photo = (image *) malloc (sizeof (image));
    if (photo == NULL)
    {
        fclose (file);
        return NULL;
    }

    photo->width = width;
    photo->height = height;
    photo->max_value = max_value;

    photo->data = (unsigned char *) malloc (n);

    if (photo->data == NULL)
    {
        free (photo);
        fclose (file);
        return NULL;
    }

    fread (&dummy, 1, 1, file); /* read the required white space */
    fread (photo->data, n, 1, file);
    return photo;
}

unsigned char negate (unsigned char value, unsigned int max_value)
{
    /* delay (value); */
    return max_value - value;
}

int write_image (const char *filename, image *photo)
{
	int myid;
	printf("write_image() entered\n");
	myid = MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    FILE *file = fopen (filename, "w");
    int length = photo->width * photo->height * 3;
    unsigned char *p = photo->data;
    int i;

		printf("almost made it!!!!\n");
    if (file == NULL)
    {
        return FALSE;
    }

    fprintf (file, "P6\n");
    fprintf (file, "%u %u\n", photo->width, photo->height);
    fprintf (file, "%u\n", photo->max_value);

		printf("made it!!!!\n");
    for (i = 0; i < length; i++)
    {
        putc (*p, file);
        p++;
    }

    if (myid == 0) {
		fclose (file);
	}
    return TRUE;
}

void clear_image (image *photo)
{
    free (photo -> data);
    free (photo);
}
