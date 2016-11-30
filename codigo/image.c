/******************************************************************************
 *  Nome:	Nicolas Nogueira Lopes da Silva     9277541
 *
 *	image.c - leitura e escrita em imagens ppm coloridas
 *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "array.h"
#include "image.h"

static void read_ppm_pixel_data(struct image *img)
{
	struct pam *pam = &img->pam;
	tuple *row = pnm_allocpamrow(pam);
	make_matrix(img->r, pam->height, pam->width);
	make_matrix(img->g, pam->height, pam->width);
	make_matrix(img->b, pam->height, pam->width);
	int i, id = 0, nt = 1, j;

	// paralelizando cada coluna da img
	#pragma omp parallel \
	shared(img, pam, row) \
	private(i, j, id, nt)
	id = omp_get_thread_num();
	nt = omp_get_num_threads();
	for (i = id; i < pam->height; i+=nt) {
		pnm_readpamrow(pam, row);
		for (j = 0; j < pam->width; j++){
			//cores no intervalo [0, maxval] -> cores no intervalo [0, 1]
			img->r[i][j] = (double)((double) row[j][0]/((double) pam->maxval));
			img->g[i][j] = (double)((double) row[j][1]/((double) pam->maxval));
			img->b[i][j] = (double)((double) row[j][2]/((double) pam->maxval));
		}
	}
	pnm_freepamrow(row);
}

static void write_ppm_pixel_data(struct image *img)
{
	struct pam *pam = &img->pam;
	tuple *row = pnm_allocpamrow(pam);
	int i, id = 0, nt, j;
	pam->plainformat = 1;

	// paralelizando cada coluna da img
	#pragma omp parallel \
	shared(img, pam, row) \
	private(i, j, id, nt)
	id = omp_get_thread_num();
	nt = omp_get_num_threads();
	pixel *rowp = malloc (pam->width * sizeof (pixel));
	for (i = id; i < pam->height; i+=nt) {

		for (j = 0; j < pam->width; j++){
			//cores no intervalo [0, 1] -> cores no intervalo [0, maxval]
			rowp[j].r = row[j][0] = img->r[i][j]*pam->maxval;
			rowp[j].g = row[j][1] = img->g[i][j]*pam->maxval;
			rowp[j].b = row[j][2] = img->b[i][j]*pam->maxval;

		}
		ppm_writeppmrow(pam->file, rowp, pam->width, pam->maxval, 1);
	}
	free(rowp);
	pnm_freepamrow(row);
}

struct image *read_image(char *filename)
{
	struct image *img = malloc(sizeof *img);
	struct pam *pam = &img->pam;
	FILE *fp = pm_openr(filename);
	pnm_readpaminit(fp, pam, PAM_STRUCT_SIZE(tuple_type));
	if (pam->format == PPM_FORMAT || pam->format == RPPM_FORMAT){
		read_ppm_pixel_data(img);
	}
	else {
		fprintf(stderr, "Erro: arquivo ‘%s’ não está no formato "
			"PPM\n", filename);
		exit(EXIT_FAILURE);
	}
	pm_close(fp);
	return img;
}


void write_image(char *filename, struct image *img)
{
	struct pam *pam = &img->pam;
	pam->file = pm_openw(filename);
	memcpy(pam->tuple_type, PAM_PPM_TUPLETYPE, strlen(PAM_PPM_TUPLETYPE)+1);
	pam->format = PAM_FORMAT_TYPE(PPM_TYPE); 
	pam->plainformat = 1; //isso se torna inutil no netpbm, desenvolvedores malucos
	pam->bytes_per_sample = 1;
	ppm_writeppminit(pam->file, pam->width, pam->height, 
                         (pixval) pam->maxval, 1);
	if (pam->format == PPM_FORMAT){
		write_ppm_pixel_data(img);
	} else {
		fprintf(stderr, "Erro na saída!");
		exit(EXIT_FAILURE);
	}
	pm_close(pam->file);
}

void free_image(struct image *img)
{
	if (img != NULL) {
		free_matrix(img->r);
		free_matrix(img->g);
		free_matrix(img->b);
		free(img);
	}
}