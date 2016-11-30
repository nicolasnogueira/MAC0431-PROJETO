#############################################################################
#
#  Nome:	Nicolas Nogueira Lopes da Silva     9277541
#
#############################################################################

CFLAGS = -g -Wall -pthread -fopenmp -I/usr/local/netpbm/include -L/usr/local/netpbm/lib -L/usr/local/netpbm/include
#Nao consegui configurar direito o netpbm no meu pc (noob alert) tive que colocar esse monte de -I e -L para funcionar

default : alternativa

alternativa : array.h image.c image.h alternativa.c
	$(CC) $(CFLAGS)  array.h image.c image.h alternativa.c -o alternativa -lnetpbm -lm

final:
	$(RM) *.o
	find . -name "*.gch" -exec $(RM) -rf {} \;

clear:
	$(RM) alternativa
	$(RM) *.o
	find . -name "*.gch" -exec $(RM) -rf {} \;
	find . -name "*~" -exec $(RM) -rf {} \;

