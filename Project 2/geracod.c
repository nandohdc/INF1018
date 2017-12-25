/* Fernando Homem da Costa 1211971 33WB */
/* Tatiana de Oliveira Magdalena 1321440 33WB */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "geracod.h"

typedef int (*funcp) ();


funcp geracod(FILE *f)
{
	/***************************************************************************/
	/* var := varc1 op varc2 */
	/* var pode ser uma variável local ou um parâmetro da função e varc1 e varc1 podem ser variáveis locais, parâmetros ou constantes inteiras. op é um dos operadores: + - * */
	/***************************************************************************/
	/* 'ifeq' varc1 varc2 num */
	/* o programa desvie para a instrução na linha de número num se os valores de varc1 e varc2 forem iguais. */
	/***************************************************************************/
	/* 'ret' varc */
	/***************************************************************************/
	/* constantes: $i */
	/* variáveis locais: vi */
	/* parâmetros: pi */
	/* máximo 5 variáveis locais e 5 parâmetros */
	/***************************************************************************/

	int line = 1;
	int  c, i = 0, j;
	int linha[50];
	unsigned char *codigo;

	codigo = (unsigned char*)malloc(1024 * sizeof(unsigned char)); /* Alocou-se um tamanho de memoria suficiente para armazenar o codigo da funcao minima passada. */

	/* Prologo - sempre igual */
	codigo[i++] = 0x55;						/* push	%ebp */
	codigo[i++] = 0x89;						/* mov	%esp, %ebp */
	codigo[i++] = 0xe5;						/* mov	%esp, %ebp */

	/* Regiao para as variaveis locais */
	codigo[i++] = 0x83;						/*sub $0x20, %esp*/
	codigo[i++] = 0xec;						/*sub $0x20, %esp*/
	codigo[i++] = 0x14;						/*sub $0x20, %esp*/
	

	/* Aqui vai o codigo da funcao de fato - fazer os testes de leitura e etc */
	while ((c = fgetc(f)) != EOF)
	{
		switch (c)
		{

 /*** ret ***/	case 'r':
		{
			int idx;
			char var;
			unsigned char idxx;
			
			if(fscanf(f, "et %c%d", &var, &idx)!=2)
			{
				printf("Error Ret!\n");
				exit(1);
			}
			
			linha[line - 1] = i; /* O "endereco" desta linha dentro de codigo[] eh o indice em que ela comeca a ser escrita */
					  
			/* Caso em que retorna constante */
			if (var == '$') 
			{
				codigo[i++] = 0xb8; /* movl cte, %eax */	
							
				for(j = 0 ; j < 4; j++)
				{
					codigo[i++] = (unsigned char)((idx >> (8 * j)) & 0xff);
				}

				/* finalizacao - sempre igual */
				codigo[i++] = 0x89;	/* mov    %ebp, %esp */
				codigo[i++] = 0xec; 	/* mov    %ebp, %esp */
				codigo[i++] = 0x5d;	/* pop    %ebp*/
				codigo[i++] = 0xc3;	/* ret */
			}
			
			/* Caso em que retorna variavel local */
			if (var == 'v') 
			{
				/*Caso de retornar primeira variavel*/
				codigo[i++] = 0x8b; /*mov -xx(%ebp), %eax*/
				codigo[i++] = 0x45; /*mov -xx(%ebp), %eax*/
				idxx = (unsigned char)(0xfc - (idx * 4)); /*Conforme o indice do Vx  */
				codigo[i++] = idxx;

				/* finalizacao - sempre igual */
				codigo[i++] = 0x89;	/* mov    %ebp, %esp */
				codigo[i++] = 0xec; 	/* mov    %ebp, %esp */
				codigo[i++] = 0x5d;	/* pop    %ebp*/
				codigo[i++] = 0xc3;	/* ret */
						 
			}
			
			/* Caso em que retorna parametro */
			if (var == 'p') 
			{
				/*Caso de retornar somente  parametro*/
				codigo[i++] = 0x8b;  /*mov xx(%ebp), %eax*/
				codigo[i++] = 0x45;  /*mov xx(%ebp), %eax*/
				idxx = (unsigned char)(0x08 + (idx * 4));
				codigo[i++] = idxx;
							
				/* finalizacao - sempre igual */
				codigo[i++] = 0x89;	/* mov    %ebp, %esp */
				codigo[i++] = 0xec; /* mov    %ebp, %esp */
				codigo[i++] = 0x5d;	/* pop    %ebp*/
				codigo[i++] = 0xc3;	/* ret */
			}
			
			break;
		} /* end ret */


/*** ifeq ***/	case 'i': /* 'ifeq' varc1 varc2 num */
		{
			int idx0, idx1, num, temp, a = 0, b= 0;
			int matriz[50][2];
			char var0, var1, idxx1, idxx2;
			
			if (fscanf(f, "feq %c%d %c%d %d", &var0, &idx0, &var1, &idx1, &num) != 5)
			{
				printf("Error ifeq!\n");
				exit(1);
			}
			
			linha[line - 1] = i ; /* O "endereco" desta linha dentro de codigo[] eh o indice em que ela comeca a ser escrita */ 

			/*------------------------------CONSTANTES com CONSTANTES----------------------------------------*/

			if ((var0 == '$') && (var1 == '$'))
			{
				codigo[i++] = 0xb9; /*mov $xxxx, %ecx*/
				for (j = 0; j < 4; j++) /* Escreve a primeira constante lida em ordem LittleEndian */
				{
					codigo[i++] = (unsigned char)((idx0 >> (8 * j)) & 0xff);
				}

				codigo[i++] = 0xba; /*mov $xxx, %edx*/
				for (j = 0; j < 4; j++) /* Escreve a segunda constante lida em ordem LittleEndian */
				{
					codigo[i++] = (unsigned char)((idx1 >> (8 * j)) & 0xff);
				}

				codigo[i++] = 0x39; /*cmp %ecx, %edx*/
				codigo[i++] = 0xca; /*cmp %ecx, %edx*/

				if (num < line)
				{
					codigo[i++] = 0x0f; /*je(jump para longe) xxx*/
					codigo[i++] = 0x84; /*je(jump para longe) xxx*/

					temp = linha[num-1] - i; /*número de bytes (o indice em codigo[] da linha para onde quero ir menos o indice em codigo[] da proxima instrucao)*/
					
					for (j = 0; j < 4; j++)
					{ /* Escreve em ordem LittleEndian */
						codigo[i++] = (unsigned char)((temp >> (8 * j)) & 0xff);
					}

				}

				if (num > line)
				{
					codigo[i++] = 0x0f; /*je(jump para longe) xxx*/
					codigo[i++] = 0x84; /*je(jump para longe) xxx*/
					matriz[a][0] = i; /*indice que foi deixado em branco pelo je*/
					matriz[a][1] = num; /*linha que é preciso encontrar o endereço*/
							  
					while (b < a)
					{
						temp = linha[(matriz[b][1]) - 1] - matriz[b][0]; /* Preenchendo os buracos */
						
						for (j = 0; j < 4; j++)
						{ /* Escreve a segunda constante lida em ordem LittleEndian */
							codigo[i++] = (unsigned char)((temp >> (8 * j)) & 0xff);
						}
						b++;
					}
				}
			}

			/*---------------------------------------VARIAVEIS com VARIAVEIS------------------------------------------------------------*/
			if ((var0 == 'v') && (var1 == 'v'))
			{
				codigo[i++] = 0x8b; /*mov xx(%ebp), %ecx*/
				codigo[i++] = 0x4d; /*mov xx(%ebp), %ecx*/
				idxx1 = (unsigned char)(0xfc - (idx0 * 4)); /*Conforme o indice do Vx = V1 + V2 && mov -4(%ebp), %ecx */
				codigo[i++] = idxx1;

				codigo[i++] = 0x8b; /*mov xx(%ebp), %edx*/
				codigo[i++] = 0x55; /*mov xx(%ebp), %edx*/
				idxx2 = (unsigned char)(0xfc - (idx1 * 4)); /*Conforme o indice do Vx = V1 + V2 && mov -8(%ebp), %edx*/
				codigo[i++] = idxx2;

				codigo[i++] = 0x39; /*cmp %ecx, %edx*/
				codigo[i++] = 0xca; /*cmp %ecx, %edx*/

				if (num < line)
				{
					codigo[i++] = 0x0f; /*je(jump para longe) xxx*/
					codigo[i++] = 0x84; /*je(jump para longe) xxx*/
					temp = linha[num-1] - i; /*número de bytes (o indice em codigo[] da linha para onde quero ir menos o indice em codigo[] da proxima instrucao)*/
					for (j = 0; j < 4; j++)
					{ /* Escreve a segunda constante lida em ordem LittleEndian */
						codigo[i++] = (unsigned char)((temp >> (8 * j)) & 0xff);
					}

				}

				if (num > line)
				{
					codigo[i++] = 0x0f; /*je(jump para longe) xxx*/
					codigo[i++] = 0x84; /*je(jump para longe) xxx*/
					matriz[a][0] = i; /*indice que foi deixado em branco pelo je*/
					matriz[a][1] = num; /*linha que é preciso encontrar o endereço*/
							
					while (b < a)
					{
						temp = linha[(matriz[b][1]) - 1] - matriz[b][0]; // preenchendo os buracos
						for (j = 0; j < 4; j++)
						{ /* Escreve  em ordem LittleEndian */
							codigo[i++] = (unsigned char)((temp >> (8 * j)) & 0xff);
						}
						b++;
					}
				}
			}

			/*----------------------------------------------------------Parametros com Parametros--------------------------------------------------------------------------------*/
			
			if ((var0 == 'p') && (var1 == 'p'))
			{
				codigo[i++] = 0x8b; /*mov xx(%ebp), %ecx*/
				codigo[i++] = 0x4d; /*mov xx(%ebp), %ecx*/
				idxx1 = (unsigned char)(0x08 + (idx0 * 4)); /*Conforme o indice do Vx = V1 + V2 && mov -4(%ebp), %ecx */
				codigo[i++] = idxx1;

				codigo[i++] = 0x8b; /*mov xx(%ebp), %edx*/
				codigo[i++] = 0x55; /*mov xx(%ebp), %edx*/
				idxx2 = (unsigned char)(0x08 + (idx1 * 4)); /*Conforme o indice do Vx = V1 + V2 && mov -8(%ebp), %edx*/
				codigo[i++] = idxx2;

				codigo[i++] = 0x39; /*cmp %ecx, %edx*/
				codigo[i++] = 0xca; /*cmp %ecx, %edx*/

				if (num < line)
				{
					codigo[i++] = 0x0f; /*je(jump para longe) xxx*/
					codigo[i++] = 0x84; /*je(jump para longe) xxx*/
					temp = linha[num-1] - i; /*número de bytes (o indice em codigo[] da linha para onde quero ir menos o indice em codigo[] da proxima instrucao)*/
					for (j = 0; j < 4; j++)
					{ /* Escreve em ordem LittleEndian */
						codigo[i++] = (unsigned char)((temp >> (8 * j)) & 0xff);
					}
				}

				if (num > line)
				{
					codigo[i++] = 0x0f; /*je(jump para longe) xxx*/
					codigo[i++] = 0x84; /*je(jump para longe) xxx*/
					matriz[a][0] = i; /*indice que foi deixado em branco pelo je*/
					matriz[a][1] = num; /*linha que é preciso encontrar o endereço*/

					while (b < a)
					{
						temp = linha[(matriz[b][1]) - 1] - matriz[b][0]; //preenchendo os buracos
						for (j = 0; j < 4; j++)
						{ /*Escreve em ordem LittleEndian */
							codigo[i++] = (unsigned char)((temp >> (8 * j)) & 0xff);
						}
						b++;
					}
				}
			}

			/*-----------------------------------VARIAVEIS com CONSTANTES----------------------------------------------*/
	
			if ((var0 == 'v') && (var1 == '$'))
			{
				codigo[i++] = 0xba; /*mov $xxx, %edx*/
				for (j = 0; j < 4; j++) /* Escreve a segunda constante lida em ordem LittleEndian */
				{
					codigo[i++] = (unsigned char)((idx1 >> (8 * j)) & 0xff);
				}

				codigo[i++] = 0x8b; /*mov xx(%ebp), %edx*/
				codigo[i++] = 0x55; /*mov xx(%ebp), %edx*/
				idxx2 = (unsigned char)(0x08 + (idx0 * 4)); /*Conforme o indice do Vx// mov -xx(%ebp), %edx*/
				codigo[i++] = idxx1;

				codigo[i++] = 0x39; /*cmp %ecx, %edx*/
				codigo[i++] = 0xca; /*cmp %ecx, %edx*/

				if (num < line)
				{
					codigo[i++] = 0x0f; /*je(jump para longe) xxx*/
					codigo[i++] = 0x84; /*je(jump para longe) xxx*/
					temp = linha[num-1] - i; /*número de bytes (o indice em codigo[] da linha para onde quero ir menos o indice em codigo[] da proxima instrucao)*/
					for (j = 0; j < 4; j++)
					{ /* Escreve em ordem LittleEndian */
						codigo[i++] = (unsigned char)((temp >> (8 * j)) & 0xff);
					}
				}

				if (num > line)
				{
					codigo[i++] = 0x0f; /*je(jump para longe) xxx*/
					codigo[i++] = 0x84; /*je(jump para longe) xxx*/
					matriz[a][0] = i; /*indice que foi deixado em branco pelo je*/
					matriz[a][1] = num; /*linha que é preciso encontrar o endereço*/
					  
					while (b < a)
					{
						temp = linha[(matriz[b][1]) - 1] - matriz[b][0]; /*preenchendo os buracos*/
						for (j = 0; j < 4; j++)
						{ /* Escreve em ordem LittleEndian */
							codigo[i++] = (unsigned char)((temp >> (8 * j)) & 0xff);
						}
						b++;
					}
				}
			}

			/*-----------------------------------CONSTANTES com VARIAVEIS-------------------------------------------*/
			if ((var0 == '$') && (var1 == 'v'))
			{
				codigo[i++] = 0xba; /*mov $xxx, %edx*/
				for (j = 0; j < 4; j++) /* Escreve a constante lida em ordem LittleEndian */
				{
					codigo[i++] = (unsigned char)((idx0 >> (8 * j)) & 0xff);
				}

				codigo[i++] = 0x8b; /*mov xx(%ebp), %edx*/
				codigo[i++] = 0x55; /*mov xx(%ebp), %edx*/
				idxx2 = (unsigned char)(0x08 + (idx1 * 4)); /*Conforme o indice do Vx = V1 + V2 && mov -8(%ebp), %edx*/
				codigo[i++] = idxx2;

				codigo[i++] = 0x39; /*cmp %ecx, %edx*/
				codigo[i++] = 0xca; /*cmp %ecx, %edx*/

				if (num < line)
				{
					codigo[i++] = 0x0f; /*je(jump para longe) xxx*/
					codigo[i++] = 0x84; /*je(jump para longe) xxx*/
					temp = linha[num-1] - i; /*número de bytes (o indice em codigo[] da linha para onde quero ir menos o indice em codigo[] da proxima instrucao)*/
					for (j = 0; j < 4; j++)
					{ /* Escreve em ordem LittleEndian */
						codigo[i++] = (unsigned char)((temp >> (8 * j)) & 0xff);
					}
				}

				if (num > line)
				{
					codigo[i++] = 0x0f; /*je(jump para longe) xxx*/
					codigo[i++] = 0x84; /*je(jump para longe) xxx*/
					matriz[a][0] = i; /*indice que foi deixado em branco pelo je*/
					matriz[a][1] = num; /*linha que é preciso encontrar o endereço*/
						  
					while (b < a)
					{
						temp = linha[(matriz[b][1]) - 1] - matriz[b][0]; /*preenchendo os buracos*/
						for (j = 0; j < 4; j++)
						{ /* Escreve em ordem LittleEndian */
							codigo[i++] = (unsigned char)((temp >> (8 * j)) & 0xff);
						}
						b++;
					}
				}
			}

			/*---------------------------------------PARAMETROS com CONSTANTES-------------------------------------------------------*/

			if ((var0 == 'p') && (var1 == '$'))
			{
				codigo[i++] = 0xba; /*mov $xxx, %edx*/
				for (j = 0; j < 4; j++) /* Escreve a constante lida em ordem LittleEndian */
				{
					codigo[i++] = (unsigned char)((idx1 >> (8 * j)) & 0xff);
				}

				codigo[i++] = 0x8b; /*mov xx(%ebp), %edx*/
				codigo[i++] = 0x55; /*mov xx(%ebp), %edx*/
				idxx2 = (unsigned char)(0x08 + (idx0 * 4)); /*Conforme o indice do Vx = V1 + V2 && mov -8(%ebp), %edx*/
				codigo[i++] = idxx2;

				codigo[i++] = 0x39; /*cmp %ecx, %edx*/
				codigo[i++] = 0xca; /*cmp %ecx, %edx*/

				if (num < line)
				{
					codigo[i++] = 0x0f; /*je(jump para longe) xxx*/
					codigo[i++] = 0x84; /*je(jump para longe) xxx*/
					temp = linha[num-1] - i; /*número de bytes (o indice em codigo[] da linha para onde quero ir menos o indice em codigo[] da proxima instrucao)*/
					for (j = 0; j < 4; j++)
					{ /* Escreve em ordem LittleEndian */
						codigo[i++] = (unsigned char)((temp >> (8 * j)) & 0xff);
					}
				}

				if (num > line)
				{
					codigo[i++] = 0x0f; /*je(jump para longe) xxx*/
					codigo[i++] = 0x84; /*je(jump para longe) xxx*/
					matriz[a][0] = i; /*indice que foi deixado em branco pelo je*/
					matriz[a][1] = num; /*linha que é preciso encontrar o endereço*/

					while (b < a)
					{
						temp = linha[(matriz[b][1]) - 1] - matriz[b][0]; /*preenchendo os buracos*/
						for (j = 0; j < 4; j++)
						{ /* Escreve em ordem LittleEndian */
							codigo[i++] = (unsigned char)((temp >> (8 * j)) & 0xff);
						}
						b++;
					}
				}
			}

			/*---------------------------------------CONSTANTES com PARAMETROS-------------------------------------------------------*/

			if ((var0 == '$') && (var1 == 'p'))
			{
				codigo[i++] = 0xba; /*mov $xxx, %edx*/
				for (j = 0; j < 4; j++) 
				{/* Escreve em ordem LittleEndian */
					codigo[i++] = (unsigned char)((idx0 >> (8 * j)) & 0xff);
				}

				codigo[i++] = 0x8b; /*mov xx(%ebp), %edx*/
				codigo[i++] = 0x55; /*mov xx(%ebp), %edx*/
				idxx2 = (unsigned char)(0x08 + (idx1 * 4)); /*Conforme o indice do Vx = V1 + V2 && mov -8(%ebp), %edx*/
				codigo[i++] = idxx2;

				codigo[i++] = 0x39; /*cmp %ecx, %edx*/
				codigo[i++] = 0xca; /*cmp %ecx, %edx*/

				if (num < line)
				{
					codigo[i++] = 0x0f; /*je(jump para longe) xxx*/
					codigo[i++] = 0x84; /*je(jump para longe) xxx*/
					temp = linha[num-1] - i; /*número de bytes (o indice em codigo[] da linha para onde quero ir menos o indice em codigo[] da proxima instrucao)*/
					for (j = 0; j < 4; j++)
					{ /* Escreve em ordem LittleEndian */
						codigo[i++] = (unsigned char)((temp >> (8 * j)) & 0xff);
					}
				}

				if (num > line)
				{
					codigo[i++] = 0x0f; /*je(jump para longe) xxx*/
					codigo[i++] = 0x84; /*je(jump para longe) xxx*/
					matriz[a][0] = i; /*indice que foi deixado em branco pelo je*/
					matriz[a][1] = num; /*linha que é preciso encontrar o endereço*/
							  
					while (b < a)
					{
						temp = linha[(matriz[b][1]) - 1] - matriz[b][0]; /*preenchendo os buracos*/
						for (j = 0; j < 4; j++)
						{ /* Escreve em ordem LittleEndian */
							codigo[i++] = (unsigned char)((temp >> (8 * j)) & 0xff);
						}
						b++;
					}
				}
			}

			/*----------------------------------PARAMETROS com VARIAVEIS------------------------------------------------------------*/

			if ((var0 == 'p') && (var1 == 'v'))
			{
				codigo[i++] = 0x8b; /*mov xx(%ebp), %ecx*/
				codigo[i++] = 0x4d; /*mov xx(%ebp), %ecx*/
				idxx1 = (unsigned char)(0xfc - (idx1 * 4)); /*Conforme o indice do Vx = V1 + V2 && mov -4(%ebp), %ecx */
				codigo[i++] = idxx1;

				codigo[i++] = 0x8b; /*mov xx(%ebp), %edx*/
				codigo[i++] = 0x55; /*mov xx(%ebp), %edx*/
				idxx2 = (unsigned char)(0x08 + (idx0 * 4)); /*Conforme o indice do Vx = V1 + V2 && mov -8(%ebp), %edx*/
				codigo[i++] = idxx2;

				codigo[i++] = 0x39; /*cmp %ecx, %edx*/
				codigo[i++] = 0xca; /*cmp %ecx, %edx*/

				if (num < line)
				{
					codigo[i++] = 0x0f; /*je(jump para longe) xxx*/
					codigo[i++] = 0x84; /*je(jump para longe) xxx*/
					temp = linha[num-1] - i; /*número de bytes (o indice em codigo[] da linha para onde quero ir menos o indice em codigo[] da proxima instrucao)*/
					for (j = 0; j < 4; j++)
					{ /* Escreve em ordem LittleEndian */
						codigo[i++] = (unsigned char)((temp >> (8 * j)) & 0xff);
					}
				}

				if (num > line)
				{
					codigo[i++] = 0x0f; /*je(jump para longe) xxx*/
					codigo[i++] = 0x84; /*je(jump para longe) xxx*/
					matriz[a][0] = i; /*indice que foi deixado em branco pelo je*/
					matriz[a][1] = num; /*linha que é preciso encontrar o endereço*/
							  
					while (b < a)
					{
						temp = linha[(matriz[b][1]) - 1] - matriz[b][0]; /*preenchendo os buracos*/
						for (j = 0; j < 4; j++)
						{ /* Escreve em ordem LittleEndian */
							codigo[i++] = (unsigned char)((temp >> (8 * j)) & 0xff);
						}
						b++;
					}
				}
			}
			
			/*----------------------------------------------------------VARIAVEIS com PARAMETROS--------------------------------------------------------------------------------*/
		
			if ((var0 == 'v') && (var1 == 'p'))
			{
				codigo[i++] = 0x8b; /*mov xx(%ebp), %ecx*/
				codigo[i++] = 0x4d; /*mov xx(%ebp), %ecx*/
				idxx1 = (unsigned char)(0xfc - (idx0 * 4)); /*Conforme o indice do Vx = V1 + V2 && mov -4(%ebp), %ecx */
				codigo[i++] = idxx1;

				codigo[i++] = 0x8b; /*mov xx(%ebp), %edx*/
				codigo[i++] = 0x55; /*mov xx(%ebp), %edx*/
				idxx2 = (unsigned char)(0x08 + (idx1 * 4)); /*Conforme o indice do Vx = V1 + V2 && mov -8(%ebp), %edx*/
				codigo[i++] = idxx2;

				codigo[i++] = 0x39; /*cmp %ecx, %edx*/
				codigo[i++] = 0xca; /*cmp %ecx, %edx*/

				if (num < line)
				{
					codigo[i++] = 0x0f; /*je(jump para longe) xxx*/
					codigo[i++] = 0x84; /*je(jump para longe) xxx*/
					temp = linha[num-1] - i; /*número de bytes (o indice em codigo[] da linha para onde quero ir menos o indice em codigo[] da proxima instrucao)*/
					for (j = 0; j < 4; j++)
					{ /* Escreve em ordem LittleEndian */
						codigo[i++] = (unsigned char)((temp >> (8 * j)) & 0xff);
					}
				}

				if (num > line)
				{
					codigo[i++] = 0x0f; /*je(jump para longe) xxx*/
					codigo[i++] = 0x84; /*je(jump para longe) xxx*/
					matriz[a][0] = i; /*indice que foi deixado em branco pelo je*/
					matriz[a][1] = num; /*linha que é preciso encontrar o endereço*/
							  
					while (b < a)
					{
						temp = linha[(matriz[b][1]) - 1] - matriz[b][0]; /*preenchendo os buracos*/
						for (j = 0; j < 4; j++)
						{ /* Escreve em ordem LittleEndian */
							codigo[i++] = (unsigned char)((temp >> (8 * j)) & 0xff);
						}
						b++;
					}
				}
			}

			break;
		} /* end ifeq */
		
/* atribuicao */ case 'v': case 'p': case '$':
		{  
			int idx0, idx1, idx2;
			char var0 = c, var1, var2;
			unsigned char idxx, idxx1, idxx2;
			char op;
			
			if(fscanf(f, "%d := %c%d %c %c%d", &idx0, &var1, &idx1, &op, &var2, &idx2) != 6)
			{
				printf("Error Atribuicao!\n");
				exit(1);
			}
			
			linha[line-1] = i; 

			/*---------------------------------------VARIAVEIS com VARIAVEIS------------------------------------------------------*/

			if ((var1 == 'v') && (var2 == 'v'))
			{
				codigo[i++] = 0x8b; /*mov -xx(%ebp), %ecx*/
				codigo[i++] = 0x4d; /*mov -xx(%ebp), %ecx*/
				idxx1 = (unsigned char)(0xfc - (idx2 * 4)); /*Conforme o indice do Vx = V1 + V2 && mov -4(%ebp), %ecx */
				codigo[i++] = idxx1;

				codigo[i++] = 0x8b; /*mov -xx(%ebp), %edx*/
				codigo[i++] = 0x55; /*mov -xx(%ebp), %edx*/
				idxx2 = (unsigned char)(0xfc - (idx1 * 4)); /*Conforme o indice do Vx = V1 + V2 && mov -8(%ebp), %edx*/
				codigo[i++] = idxx2;

				if (op == '+')
				{
					codigo[i++] = 0x01; /*add %ecx, %edx*/
					codigo[i++] = 0xca; /*add %ecx, %edx*/
				}

				if (op == '-')
				{
					codigo[i++] = 0x29; /*subl %ecx, %edx*/
					codigo[i++] = 0xca; /*subl %ecx, %edx*/
				}

				if (op == '*')
				{
					codigo[i++] = 0x0f; /*imul %ecx, %edx*/
					codigo[i++] = 0xaf; /*imul %ecx, %edx*/
					codigo[i++] = 0xd1; /*imul %ecx, %edx*/
				}
						  						  
				if (var0 == 'v')
				{
					codigo[i++] = 0x89; /*mov %edx,-xx(%ebp)*/
					codigo[i++] = 0x55; /*mov %edx,-xx(%ebp)*/
					idxx = (unsigned char)(0xfc - (idx0 * 4)); /*Conforme o indice do Vx*/
					codigo[i++] = idxx;
				}
				
				if (var0 == 'p')
				{
					codigo[i++] = 0x89; /*mov %edx, xx(%ebp)*/
					codigo[i++] = 0x55; /*mov %edx, xx(%ebp)*/
					idxx = (unsigned char)(0x08 + (idx0 * 4)); /*Conforme o indice do px*/
					codigo[i++] = idxx;
				}

			}
			
			/*-----------------------------------------------------VARIAVEIS com CONSTANTES---------------------------------------------------*/

			if ((var1 == 'v') && (var2 == '$'))
			{
				codigo[i++] = 0xb9; /*mov $xxx, %ecx*/
				for (j = 0; j < 4; j++) 
				{ /* Escreve a constante lida em ordem LittleEndian */
					codigo[i++] = (unsigned char)((idx2 >> (8 * j)) & 0xff);
				}

				codigo[i++] = 0x8b; /*mov xx(%ebp), %edx*/
				codigo[i++] = 0x55; /*mov xx(%ebp), %edx*/
				idxx1 = (unsigned char)(0xfc - (idx1 * 4)); /*Conforme o indice do Vx = V1 + V2 && mov -4(%ebp), %ecx */
				codigo[i++] = idxx1;

				if (op == '+')
				{
					codigo[i++] = 0x01; /*add %ecx, %edx*/
					codigo[i++] = 0xca; /*add %ecx, %edx*/
				}

				if (op == '-')
				{
					codigo[i++] = 0x29; /*subl %ecx, %edx*/
					codigo[i++] = 0xca; /*subl %ecx, %edx*/
				}

				if (op == '*')
				{
					codigo[i++] = 0x0f; /*imul %ecx, %edx*/
					codigo[i++] = 0xaf; /*imul %ecx, %edx*/
					codigo[i++] = 0xd1; /*imul %ecx, %edx*/
				}

				if (var0 == 'v')
				{
					codigo[i++] = 0x89; /*mov %edx,-xx(%ebp)*/
					codigo[i++] = 0x55; /*mov %edx,-xx(%ebp)*/
					idxx = (unsigned char)(0xfc - (idx0 * 4)); /*Conforme o indice do Vx = V1 + V2*/
					codigo[i++] = idxx;
				}
				
				if (var0 == 'p')
				{
					codigo[i++] = 0x89; /*mov %edx, xx(%ebp)*/
					codigo[i++] = 0x55; /*mov %edx, xx(%ebp)*/
					idxx = (unsigned char)(0x08 + (idx0 * 4)); /*Conforme o indice do Px*/
					codigo[i++] = idxx;
				}
			}

			/*---------------------------------------------CONSTANTES com VARIAVEIS-----------------------------------------------------*/
			if ((var1 == '$') && (var2 == 'v'))
			{
				codigo[i++] = 0xba; /*mov $xxx, %edx*/
				for (j = 0; j < 4; j++) /* Escreve a constante lida em ordem LittleEndian */
				{
					codigo[i++] = (unsigned char)((idx1 >> (8 * j)) & 0xff);
				}

				codigo[i++] = 0x8b; /*mov xx(%ebp), %ecx*/
				codigo[i++] = 0x4d; /*mov xx(%ebp), %ecx*/
				idxx2 = (unsigned char)(0xfc - (idx2 * 4)); /*Conforme o indice do Vx = V1 + V2 && mov -4(%ebp), %ecx */
				codigo[i++] = idxx2;

				if (op == '+')
				{
					codigo[i++] = 0x01; /*add %ecx, %edx*/
					codigo[i++] = 0xca; /*add %ecx, %edx*/
				}

				if (op == '-')
				{
					codigo[i++] = 0x29; /*subl %ecx, %edx*/
					codigo[i++] = 0xca; /*subl %ecx, %edx*/
				}

				if (op == '*')
				{
					codigo[i++] = 0x0f; /*imul %ecx, %edx*/
					codigo[i++] = 0xaf; /*imul %ecx, %edx*/
					codigo[i++] = 0xd1; /*imul %ecx, %edx*/
				}
						  						  
				if (var0 == 'v')
				{
					codigo[i++] = 0x89; /*mov %edx,-xx(%ebp)*/
					codigo[i++] = 0x55; /*mov %edx,-xx(%ebp)*/
					idxx = (unsigned char)(0xfc - (idx0 * 4)); /*Conforme o indice do Vx = V1 + V2*/
					codigo[i++] = idxx;
				}
				
				if (var0 == 'p')
				{
					codigo[i++] = 0x89; /*mov %edx, xx(%ebp)*/
					codigo[i++] = 0x55; /*mov %edx, xx(%ebp)*/
					idxx = (unsigned char)(0x08 + (idx0 * 4)); /*Conforme o indice do Vx = V1 + V2*/
					codigo[i++] = idxx;
				}
			}

			/*---------------------------------------VARIAVEIS com PARAMETROS-----------------------------------------------------*/

			if ((var1 == 'v') && (var2 == 'p'))
			{
				codigo[i++] = 0x8b; /*mov xx(%ebp), %ecx*/
				codigo[i++] = 0x4d; /*mov xx(%ebp), %ecx*/
				idxx1 = (unsigned char)(0xfc - (idx1 * 4)); /*Conforme o indice do Vx = V1 + V2 && mov -4(%ebp), %ecx */
				codigo[i++] = idxx1;

				codigo[i++] = 0x8b; /*mov xx(%ebp), %edx*/
				codigo[i++] = 0x55; /*mov xx(%ebp), %edx*/
				idxx2 = (unsigned char)(0x08 + (idx2 * 4)); /*Conforme o indice do Vx = V1 + V2 && mov -8(%ebp), %edx*/
				codigo[i++] = idxx2;

				if (op == '+')
				{
					codigo[i++] = 0x01; /*add %ecx, %edx*/
					codigo[i++] = 0xca; /*add %ecx, %edx*/
				}

				if (op == '-')
				{
					codigo[i++] = 0x29; /*subl %ecx, %edx*/
					codigo[i++] = 0xca; /*subl %ecx, %edx*/
				}

				if (op == '*')
				{
					codigo[i++] = 0x0f; /*imul %ecx, %edx*/
					codigo[i++] = 0xaf; /*imul %ecx, %edx*/
					codigo[i++] = 0xd1; /*imul %ecx, %edx*/
				}

				if (var0 == 'v')
				{
					codigo[i++] = 0x89; /*mov %edx,-xx(%ebp)*/
					codigo[i++] = 0x55; /*mov %edx,-xx(%ebp)*/
					idxx = (unsigned char)(0xfc - (idx0 * 4)); /*Conforme o indice do Vx = V1 + V2*/
					codigo[i++] = idxx;
				}
				
				if (var0 == 'p')
				{
					codigo[i++] = 0x89; /*mov %edx, xx(%ebp)*/
					codigo[i++] = 0x55; /*mov %edx, xx(%ebp)*/
					idxx = (unsigned char)(0x08 + (idx0 * 4)); /*Conforme o indice do Vx = V1 + V2*/
					codigo[i++] = idxx;
				}

			}

			/*---------------------------------------PARAMETROS com VARIAVEIS-----------------------------------------------------*/

			if ((var1 == 'p') && (var2 == 'v'))
			{
				codigo[i++] = 0x8b; /*mov xx(%ebp), %ecx*/
				codigo[i++] = 0x4d; /*mov xx(%ebp), %ecx*/
				idxx1 = (unsigned char)(0x08 + (idx1 * 4)); /*Conforme o indice do Vx = V1 + V2 && mov -4(%ebp), %ecx */
				codigo[i++] = idxx1;

				codigo[i++] = 0x8b; /*mov xx(%ebp), %edx*/
				codigo[i++] = 0x55; /*mov xx(%ebp), %edx*/
				idxx2 = (unsigned char)(0xfc - (idx2 * 4)); /*Conforme o indice do Vx = V1 + V2 && mov -8(%ebp), %edx*/
				codigo[i++] = idxx2;

				if (op == '+')
				{
					codigo[i++] = 0x01; /*add %ecx, %edx*/
					codigo[i++] = 0xca; /*add %ecx, %edx*/
				}

				if (op == '-')
				{
					codigo[i++] = 0x29; /*subl %ecx, %edx*/
					codigo[i++] = 0xca; /*subl %ecx, %edx*/
				}

				if (op == '*')
				{
					codigo[i++] = 0x0f; /*imul %ecx, %edx*/
					codigo[i++] = 0xaf; /*imul %ecx, %edx*/
					codigo[i++] = 0xd1; /*imul %ecx, %edx*/
				}

				codigo[i++] = 0x89; /*mov %edx, %eax*/
				codigo[i++] = 0xd0; /*mov %edx, %eax*/

				if (var0 == 'v')
				{
					codigo[i++] = 0x89; /*mov %edx,-xx(%ebp)*/
					codigo[i++] = 0x55; /*mov %edx,-xx(%ebp)*/
					idxx = (unsigned char)(0xfc - (idx0 * 4)); /*Conforme o indice do Vx = V1 + V2*/
					codigo[i++] = idxx;
				}

				if (var0 == 'p')
				{
					codigo[i++] = 0x89; /*mov %edx, xx(%ebp)*/
					codigo[i++] = 0x55; /*mov %edx, xx(%ebp)*/
					idxx = (unsigned char)(0x08 + (idx0 * 4)); /*Conforme o indice do Vx = V1 + V2*/
					codigo[i++] = idxx;
				}
			}

			/*----------------------------------------------PARAMETROS com CONSTANTES---------------------------------------------*/

			if ((var1 == 'p') && (var2 == '$'))
			{
				codigo[i++] = 0xb9; /*mov $xxx, %ecx*/
				for (j = 0; j < 4; j++) 
				{ /* Escreve a constante lida em ordem LittleEndian */
					codigo[i++] = (unsigned char)((idx2 >> (8 * j)) & 0xff);
				}

				codigo[i++] = 0x8b; /*mov xx(%ebp), %ecx*/
				codigo[i++] = 0x55; /*mov xx(%ebp), %ecx*/
				idxx1 = (unsigned char)(0x08 + (idx1 * 4)); /*Conforme o indice do Vx = V1 + V2 && mov -8(%ebp), %edx*/
				codigo[i++] = idxx1;

				if (op == '+')
				{
					codigo[i++] = 0x01; /*add %ecx, %edx*/
					codigo[i++] = 0xca; /*add %ecx, %edx*/
				}

				if (op == '-')
				{
					codigo[i++] = 0x29; /*subl %ecx, %edx*/
					codigo[i++] = 0xca; /*subl %ecx, %edx*/
				}

				if (op == '*')
				{
					codigo[i++] = 0x0f; /*imul %ecx, %edx*/
					codigo[i++] = 0xaf; /*imul %ecx, %edx*/
					codigo[i++] = 0xd1; /*imul %ecx, %edx*/
				}

				if (var0 == 'v')
				{
					codigo[i++] = 0x89; /*mov %edx,-xx(%ebp)*/
					codigo[i++] = 0x55; /*mov %edx,-xx(%ebp)*/
					idxx = (unsigned char)(0xfc - (idx0 * 4)); /*Conforme o indice do Vx = V1 + V2*/
					codigo[i++] = idxx;
				}
				
				if (var0 == 'p')
				{
					codigo[i++] = 0x89; /*mov %edx, xx(%ebp)*/
					codigo[i++] = 0x55; /*mov %edx, xx(%ebp)*/
					idxx = (unsigned char)(0x08 + (idx0 * 4)); /*Conforme o indice do Px*/
					codigo[i++] = idxx;
				}
			}

			/*-----------------------------------------CONSTANTES com PARAMETROS----------------------------------------------------*/

			if ((var1 == '$') && (var2 == 'p'))
			{
				codigo[i++] = 0xba; /*mov $xxx, %edx*/
				for (j = 0; j < 4; j++) 
				{ /* Escreve a constante lida em ordem LittleEndian */
					codigo[i++] = (unsigned char)((idx1 >> (8 * j)) & 0xff);
				}

				codigo[i++] = 0x8b; /*mov xx(%ebp), %ecx*/
				codigo[i++] = 0x4d; /*mov xx(%ebp), %ecx*/
				idxx2 = (unsigned char)(0x08 + (idx2 * 4)); /*Conforme o indice do Vx = V1 + V2 && mov -8(%ebp), %edx*/
				codigo[i++] = idxx2;

				if (op == '+')
				{
					codigo[i++] = 0x01; /*add %ecx, %edx*/
					codigo[i++] = 0xca; /*add %ecx, %edx*/
				}

				if (op == '-')
				{
					codigo[i++] = 0x29; /*subl %ecx, %edx*/
					codigo[i++] = 0xca; /*subl %ecx, %edx*/
				}

				if (op == '*')
				{
					codigo[i++] = 0x0f; /*imul %ecx, %edx*/
					codigo[i++] = 0xaf; /*imul %ecx, %edx*/
					codigo[i++] = 0xd1; /*imul %ecx, %edx*/
				}

				if (var0 == 'v')
				{
					codigo[i++] = 0x89; /*mov %edx,-xx(%ebp)*/
					codigo[i++] = 0x55; /*mov %edx,-xx(%ebp)*/
					idxx = (unsigned char)(0xfc - (idx0 * 4)); /*Conforme o indice do Vx = V1 + V2*/
					codigo[i++] = idxx;
				}

				if (var0 == 'p')
				{
					codigo[i++] = 0x89; /*mov %edx, xx(%ebp)*/
					codigo[i++] = 0x55; /*mov %edx, xx(%ebp)*/
					idxx = (unsigned char)(0x08 + (idx0 * 4)); /*Conforme o indice do Vx = V1 + V2*/
					codigo[i++] = idxx;
				}
			}

			/*---------------------------------------------PARAMETROS com PARAMETROS-----------------------------------------------------*/

			if ((var1 == 'p') && (var2 == 'p'))
			{
				codigo[i++] = 0x8b; /*mov xx(%ebp), %ecx*/
				codigo[i++] = 0x4d; /*mov xx(%ebp), %ecx*/
				idxx2 = (unsigned char)(0x08 + (idx2 * 4)); /*Conforme o indice do Vx = V1 + V2 && mov -4(%ebp), %ecx */
				codigo[i++] = idxx2;

				codigo[i++] = 0x8b; /*mov xx(%ebp), %edx*/
				codigo[i++] = 0x55; /*mov xx(%ebp), %edx*/
				idxx1 = (unsigned char)(0x08 + (idx1 * 4)); /*Conforme o indice do Vx = V1 + V2 && mov -8(%ebp), %edx*/
				codigo[i++] = idxx1;

				if (op == '+')
				{
					codigo[i++] = 0x01; /*add %ecx, %edx*/
					codigo[i++] = 0xca; /*add %ecx, %edx*/
				}

				if (op == '-')
				{
					codigo[i++] = 0x29; /*subl %ecx, %edx*/
					codigo[i++] = 0xca; /*subl %ecx, %edx*/
				}

				if (op == '*')
				{
					codigo[i++] = 0x0f; /*imul %ecx, %edx*/
					codigo[i++] = 0xaf; /*imul %ecx, %edx*/
					codigo[i++] = 0xd1; /*imul %ecx, %edx*/
				}

				codigo[i++] = 0x89; /*mov %edx, %eax*/
				codigo[i++] = 0xd0; /*mov %edx, %eax*/

				if (var0 == 'v')
				{
					codigo[i++] = 0x89; /*mov %edx, -xx(%ebp)*/
					codigo[i++] = 0x55; /*mov %edx, -xx(%ebp)*/
					idxx = (unsigned char)(0xfc - (idx0 * 4)); /*Conforme o indice do Vx*/
					codigo[i++] = idxx;
				}
				
				if ((var0 == 'p'))
				{
					codigo[i++] = 0x89; /*mov %edx, xx(%ebp)*/
					codigo[i++] = 0x55; /*mov %edx, xx(%ebp)*/
					idxx = (unsigned char)(0x08 + (idx0 * 4)); /*Conforme o indice do Px*/
					codigo[i++] = idxx;
				}
			}

			/*----------------------------------------------------------CONSTANTES com CONSTANTES--------------------------------------------------------------------------------*/

			if ((var1 == '$') && (var2 == '$'))
			{
				codigo[i++] = 0xb9; /*mov $xxxx, %ecx*/
				for (j = 0; j < 4; j++) 
				{ /* Escreve a primeira constante lida em ordem LittleEndian */
					codigo[i++] = (unsigned char)((idx2 >> (8 * j)) & 0xff);
				}

				codigo[i++] = 0xba; /*mov $xxx, %edx*/
				for (j = 0; j < 4; j++) 
				{ /* Escreve a segunda constante lida em ordem LittleEndian */
					codigo[i++] = (unsigned char)((idx1 >> (8 * j)) & 0xff);
				}

				if ((op == '+'))
				{
					codigo[i++] = 0x01; /*add %ecx, %edx*/
					codigo[i++] = 0xca; /*add %ecx, %edx*/
				}

				if ((op == '-'))
				{
					codigo[i++] = 0x29; /*subl %ecx, %edx*/
					codigo[i++] = 0xca; /*subl %ecx, %edx*/
				}

				if ((op == '*'))
				{
					codigo[i++] = 0x0f; /*imul %ecx, %edx*/
					codigo[i++] = 0xaf; /*imul %ecx, %edx*/
					codigo[i++] = 0xd1; /*imul %ecx, %edx*/
				}

				if ((var0 == 'v'))
				{
					codigo[i++] = 0x89; /*mov %edx,-xx(%ebp)*/
					codigo[i++] = 0x55; /*mov %edx,-xx(%ebp)*/
					idxx = (unsigned char)(0xfc - (idx0 * 4)); /*Conforme o indice do Vx = V1 + V2*/
					codigo[i++] = idxx;
				}

				if ((var0 == 'p'))
				{
					codigo[i++] = 0x89; /*mov %edx,-xx(%ebp)*/
					codigo[i++] = 0x55; /*mov %edx,-xx(%ebp)*/
					idxx = (unsigned char)(0x08 + (idx0 * 4)); /*Conforme o indice do Vx = V1 + V2*/
					codigo[i++] = idxx;
				}
			}	
			break;
		} /* end atribuicao */

		} /* end switch */

		line++;
		fscanf(f, " ");

	} /* end while */
	
	return (funcp)codigo;

} /* end geracod */

