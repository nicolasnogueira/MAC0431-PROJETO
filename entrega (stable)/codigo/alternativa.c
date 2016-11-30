/******************************************************************************
 *  Nome:	Nicolas Nogueira Lopes da Silva     9277541
 *
 *	Projeto MAC0431 - Física Alternativa
 *
 *****************************************************************************/

#include "image.h"
#include <omp.h>
#include <pam.h>
#include <math.h>
#include "array.h"

int main (int argc, char **argv) {
	int num_inter = atoi(argv[3]), inter = 0;
	int i, id = 0, nt, j;

	omp_set_num_threads(atoi(argv[4]));
	pm_init(argv[0], 0);
	struct image *img = read_image(argv[1]);
	struct image *imgb = read_image(argv[1]);

	for (inter = 0; inter < num_inter; inter++) {
		// paralelizando cada linha da img 
		// sentido da operacao img->imgb
		#pragma omp parallel \
		shared(img) \
		private(i, j, id, nt)
		id = omp_get_thread_num();
		nt = omp_get_num_threads();
		for (j = id + 1; j < img->pam.width - 1; j+=nt) {
			for (i = 1; i < img->pam.height - 1; i++) {
				//processa pixels que não estão na borda
				double gvizinho, gcalc = M_PI*img->g[i][j];
				double rx = img->r[i][j]*sin(gcalc); 		//r_x(i,j)
				double bx = img->b[i][j]*sin(gcalc + M_PI); //b_x(i,j)
				double delta;
				if (rx > 0) {
					if (i + 1 < img->pam.height - 1) {
						gvizinho = M_PI*img->g[i+1][j];
						delta = (1 - img->r[i+1][j]*gvizinho)*rx/4;
						imgb->r[i+1][j] += fabs(delta);
						imgb->r[i][j] -= fabs(delta);
					}
				} else {
					if (i - 1 > 0) {
						gvizinho = M_PI*img->g[i-1][j];
						delta = (1 - img->r[i-1][j]*gvizinho)*rx/4;
						imgb->r[i-1][j] += fabs(delta);
						imgb->r[i][j] -= fabs(delta);
					}
				}
				if (bx > 0) {
					if (i + 1 < img->pam.height - 1) {
						gvizinho = M_PI*img->g[i+1][j];
						delta = (1 - img->b[i+1][j]*gvizinho)*bx/4;
						imgb->b[i+1][j] += fabs(delta);
						imgb->b[i][j] -= fabs(delta);
					}
				} else {
					if (i - 1 > 0) {
						gvizinho = M_PI*img->g[i-1][j];
						delta = (1 - img->b[i-1][j]*gvizinho)*bx/4;
						imgb->b[i-1][j] += fabs(delta);
						imgb->b[i][j] -= fabs(delta);
					}
				}
			}
		}

		#pragma omp barrier

		// paralelizando cada coluna da img
		#pragma omp parallel \
		shared(img) \
		private(i, j, id, nt)
		id = omp_get_thread_num();
		nt = omp_get_num_threads();
		for (i = id + 1; i < img->pam.height - 1; i+=nt) {
			for (j = 1; j < img->pam.width - 1; j++) {
				//processa pixels que não estão na borda
				double gvizinho, gcalc = M_PI*img->g[i][j];
				double ry = img->r[i][j]*cos(gcalc); 		//r_y(i,j)
				double by = img->b[i][j]*cos(gcalc + M_PI); //b_y(i,j)
				double delta;
				if (ry > 0) {
					if (j + 1 < img->pam.width - 1) {
						gvizinho = M_PI*img->g[i][j+1];
						delta = (1 - img->r[i][j+1]*gvizinho)*ry/4;
						imgb->r[i][j+1] += fabs(delta);
						imgb->r[i][j] -= fabs(delta);
					}
				} else {
					if (j - 1 > 0) {
						gvizinho = M_PI*img->g[i][j-1];
						delta = (1 - img->r[i][j-1]*gvizinho)*ry/4;
						imgb->r[i][j-1] += fabs(delta);
						imgb->r[i][j] -= fabs(delta);
					}
				}
				if (by > 0) {
					if (j + 1 < img->pam.width - 1) {
						gvizinho = M_PI*img->g[i][j+1];
						delta = (1 - img->b[i][j+1]*gvizinho)*by/4;
						imgb->b[i][j+1] += fabs(delta);
						imgb->b[i][j] -= fabs(delta);
					}
				} else {
					if (j - 1 > 0) {
						gvizinho = M_PI*img->g[i][j-1];
						delta = (1 - img->b[i][j-1]*gvizinho)*by/4;
						imgb->b[i][j-1] += fabs(delta);
						imgb->b[i][j] -= fabs(delta);
					}
				}
				if (j - 1 > 0) {
					img->r[i][j-1] = imgb->r[i][j-1];
					img->b[i][j-1] = imgb->b[i][j-1];
				}
				if (j == img->pam.width - 2) {
					img->r[i][j] = imgb->r[i][j];
					img->b[i][j] = imgb->b[i][j];
				}
			}
		}

		#pragma omp barrier

		//Verificacao de excesso 
		//aqui as imagens deveriam ficar iguais
		//sentido da operacao imgb -> img
		// paralelizando cada linha da img
		#pragma omp parallel \
		shared(img) \
		private(i, j, id, nt)
		id = omp_get_thread_num();
		nt = omp_get_num_threads();
		for (j = id + 1; j < img->pam.width - 1; j+=nt) {
			for (i = 1; i < img->pam.height - 1; i++) {
				double delta;
				if (imgb->r[i][j] > 1) { //tem que pegar valor que vai distribuir do original
					delta = (imgb->r[i][j] - 1)/4;
					if (i + 1 < img->pam.height - 1 && (img->r[i+1][j] + delta) <= 1) {
						img->r[i+1][j] += delta;
					}
					if (i - 1 > 0 && (img->r[i-1][j] + delta) <= 1) {
						img->r[i-1][j] += delta;
					}
					img->r[i][j] -= 2*delta;
				}
				if (imgb->b[i][j] > 1) {
					delta = (imgb->b[i][j] - 1)/4;
					if (i + 1 < img->pam.height - 1 && (img->b[i+1][j] + delta) <= 1) {
						img->b[i+1][j] += delta;
					}
					if (i - 1 > 0 && (img->b[i-1][j] + delta) <= 1) {
						img->b[i-1][j] += delta;
					}
					img->b[i][j] -= 2*delta;
				}
			}
		}

		#pragma omp barrier

		// paralelizando cada coluna da img
		#pragma omp parallel \
		shared(img) \
		private(i, j, id, nt)
		id = omp_get_thread_num();
		nt = omp_get_num_threads();
		for (i = id + 1; i < img->pam.height - 1; i+=nt) {
			for (j = 1; j < img->pam.width - 1; j++) {
				double delta;
				if (imgb->r[i][j] > 1) { //tem que pegar valor que vai distribuir do original
					delta = (imgb->r[i][j] - 1)/4;
					if (j + 1 < img->pam.width - 1 && (img->r[i][j+1] + delta) <= 1) {
						img->r[i][j+1] += delta;
					}
					if (j - 1 > 0 && (img->r[i][j-1] + delta) <= 1) {
						img->r[i][j-1] += delta;
					}
					img->r[i][j] -= 2*delta;
				}
				if (imgb->b[i][j] > 1) {
					delta = (imgb->b[i][j] - 1)/4;
					if (j + 1 < img->pam.width - 1 && (img->b[i][j+1] + delta) <= 1) {
						img->b[i][j+1] += delta;
					}
					if (j - 1 > 0 && (img->b[i][j-1] + delta) <= 1) {
						img->b[i][j-1] += delta;
					}
					img->b[i][j] -= 2*delta;
				}
			}
		}

		#pragma omp barrier
		//calcular os valores de G
		//imgb esta desatualizado
		// paralelizando cada coluna da img
		#pragma omp parallel \
		shared(img) \
		private(i, j, id, nt)
		id = omp_get_thread_num();
		nt = omp_get_num_threads();
		for (i = id + 1; i < img->pam.height - 1; i+=nt) {
			for (j = 1; j < img->pam.width - 1; j++) {
				//calcular angulo entre vetor (R, B) e (0,1)
				//vetor (R, B) não pode ser nulo! COnsiderando 
				double norma = sqrt(img->r[i][j]*img->r[i][j] + 
									img->b[i][j]*img->b[i][j]);
				double cosdtheta = 1;
				if (norma != 0)
					cosdtheta = img->b[i][j]/norma;
					//fprintf(stderr, "VETOR RB NULO! %d %d\n", i, j);
				double dtheta = acos(cosdtheta);
				double temp = img->g[i][j] + dtheta/M_PI;
				int itemp = (int) temp;
				img->g[i][j] = temp - itemp;
				imgb->r[i][j] = img->r[i][j];
				imgb->g[i][j] = img->g[i][j];
				imgb->b[i][j] = img->b[i][j];

			}
		}
	}


	write_image(argv[2], imgb);
	free_image(img);

	return EXIT_SUCCESS;
}