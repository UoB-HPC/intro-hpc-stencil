#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

// Define output file name
#define OUTPUT_FILE "stencil.pgm"

void stencil(const int nx, const int ny, const int width, const int height,
             double* image, double* tmp_image);
void init_image(const int nx, const int ny, const int width, const int height,
                double* image, double* tmp_image);
void output_image(const char* file_name, const int nx, const int ny,
                  const int width, const int height, double* image);
double wtime(void);

int main(int argc, char* argv[])
{
  // Check usage
  if (argc != 4) {
    fprintf(stderr, "Usage: %s nx ny niters\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  // Initiliase problem dimensions from command line arguments
  int nx = atoi(argv[1]);
  int ny = atoi(argv[2]);
  int niters = atoi(argv[3]);

  // we pad the outer edge of the image to avoid out of range address issues in
  // stencil
  int width = nx + 2;
  int height = ny + 2;

  // Allocate the image
  double* image = malloc(sizeof(double) * width * height);
  double* tmp_image = malloc(sizeof(double) * width * height);

  // Set the input image
  init_image(nx, ny, width, height, image, tmp_image);

  // Call the stencil kernel
  double tic = wtime();
  for (int t = 0; t < niters; ++t) {
    stencil(nx, ny, width, height, image, tmp_image);
    stencil(nx, ny, width, height, tmp_image, image);
  }
  double toc = wtime();

  // Output
  printf("------------------------------------\n");
  printf(" runtime: %lf s\n", toc - tic);
  printf("------------------------------------\n");

  output_image(OUTPUT_FILE, nx, ny, width, height, image);
  free(image);
  free(tmp_image);
}

void stencil(const int nx, const int ny, const int width, const int height,
             double* image, double* tmp_image)
{
  for (int j = 1; j < ny + 1; ++j) {
    for (int i = 1; i < nx + 1; ++i) {
      tmp_image[j + i * height] =  image[j     + i       * height] * 3.0 / 5.0;
      tmp_image[j + i * height] += image[j     + (i - 1) * height] * 0.5 / 5.0;
      tmp_image[j + i * height] += image[j     + (i + 1) * height] * 0.5 / 5.0;
      tmp_image[j + i * height] += image[j - 1 + i       * height] * 0.5 / 5.0;
      tmp_image[j + i * height] += image[j + 1 + i       * height] * 0.5 / 5.0;
    }
  }
}

// Create the input image
void init_image(const int nx, const int ny, const int width, const int height,
                double* image, double* tmp_image)
{
  // Zero everything
  for (int j = 0; j < ny + 2; ++j) {
    for (int i = 0; i < nx + 2; ++i) {
      image[j + i * height] = 0.0;
      tmp_image[j + i * height] = 0.0;
    }
  }

  const int tile_size = 64;
  // checkerboard pattern
  for (int jb = 0; jb < ny; jb += tile_size) {
    for (int ib = 0; ib < nx; ib += tile_size) {
      if ((ib + jb) % (tile_size * 2)) {
        const int jlim = (jb + tile_size > ny) ? ny : jb + tile_size;
        const int ilim = (ib + tile_size > nx) ? nx : ib + tile_size;
        for (int j = jb + 1; j < jlim + 1; ++j) {
          for (int i = ib + 1; i < ilim + 1; ++i) {
            image[j + i * height] = 100.0;
          }
        }
      }
    }
  }
}

// Routine to output the image in Netpbm grayscale binary image format
void output_image(const char* file_name, const int nx, const int ny,
                  const int width, const int height, double* image)
{
  // Open output file
  FILE* fp = fopen(file_name, "w");
  if (!fp) {
    fprintf(stderr, "Error: Could not open %s\n", OUTPUT_FILE);
    exit(EXIT_FAILURE);
  }

  // Ouptut image header
  fprintf(fp, "P5 %d %d 255\n", nx, ny);

  // Calculate maximum value of image
  // This is used to rescale the values
  // to a range of 0-255 for output
  double maximum = 0.0;
  for (int j = 1; j < ny + 1; ++j) {
    for (int i = 1; i < nx + 1; ++i) {
      if (image[j + i * height] > maximum) maximum = image[j + i * height];
    }
  }

  // Output image, converting to numbers 0-255
  for (int j = 1; j < ny + 1; ++j) {
    for (int i = 1; i < nx + 1; ++i) {
      fputc((char)(255.0 * image[j + i * height] / maximum), fp);
    }
  }

  // Close the file
  fclose(fp);
}

// Get the current time in seconds since the Epoch
double wtime(void)
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec + tv.tv_usec * 1e-6;
}
