Nome:	Nicolas Nogueira Lopes da Silva     9277541
_________________________________________________________________________________

README - código
_________________________________________________________________________________

O código é composto pelos seguintes arquivos:
	- alternativa.c - arquivo principal
	- image.c - biblioteca para leitura e escrita das imagens ppm
	- image.h - interface para a image.c
	- array.h - interface para a criação de vetores e matrizes

Parte do código foi baseado no conteúdo de um livro que achei interessante que 
trata sobre os arquivos do Netpbm: Rouben Rostamian-Programming Projects in C 
for Students of Engineering, Science, and Mathematics-SIAM (2014).

Os for foram paralelizados de modo que cada thread cuida de uma coluna ou uma
linha da img.

_________________________________________________________________________________

Estratégia
_________________________________________________________________________________

Eu utilizei a mesma estratégia da barra de calor mas fazendo os seguintes 
passos:

- utilizo duas imagens iguais img, e imgb (mesma ideia dos vetores black e red 
da barra de calor), tenho que garantir que img = imgb no início da iteração no
loop original;

- paralelizo as colunas, realizando operações sob imgb somente com os pixels 
atual, de cima e de baixo (bordas ficam intactas);

- paralelizo as linhas, realizando operações sob imgb somente com os pixels
atual, da esquerda a da direita e colocando os pixels "prontos" em img;

- nessa etapa já atualizei os valores e eles podem estar fora do intervalo 
[0, 1], preciso corrigir e faço os mesmos passos para a correção;

- paralelizo as colunas, realizando operações sob img somente com os pixels 
atual, de cima e de baixo (bordas ficam intactas);

- paralelizo as linhas, realizando operações sob img somente com os pixels
atual, da esquerda a da direita;

- atualiza valor de G e sincroniza img e imgb. Obs.: quando o vetor é nulo, 
considera o angulo dtheta sendo 0.

_________________________________________________________________________________

Biblioteca Netpbm
_________________________________________________________________________________

Para a leitura e escrita de arquivos .ppm utilizei a biblioteca libnetpbm, mais
informações em: http://netpbm.sourceforge.net/doc/libnetpbm.html
