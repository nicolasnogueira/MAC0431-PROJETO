/******************************************************************************
 *  Nome:	Nicolas Nogueira Lopes da Silva     9277541
 *
 *	image.h - interface para leitura e escrita em imagens ppm coloridas
 *
 *****************************************************************************/

#ifndef H_IMAGE_H
#define H_IMAGE_H

#include <pam.h>
#include <ppm.h>

struct image {
	struct pam pam;
	//r, g, b in interval [0, 1]
	double **r;
	double **g; //green of PPM format or grey of PGM format
	double **b;	
};

struct image *read_image(char *filename);
void write_image(char *filename, struct image *img);
void free_image(struct image *img);

#endif /* H_IMAGE_IO_H */