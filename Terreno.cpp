/////////////////////////////////////////////////////////////////////////////////
//
// *Notas* 
//
// Este arquivo contém todas as definições e funções para tratar o terreno.
//
// 

#include "main.h"

// Isto diz-nos se queremos detalhe texturização ligado ou desligado
extern bool g_bDetail;

// Valor para detailScale de textura
extern int g_DetailScale;

float pBx[10] ;
float pBy[10] ;
float pBz[10] ;

int pRandBolas[10];

GLfloat d;

int raio = 3;

int ligacor = 0;
int posicaoluz = 0;
GLuint lista1;

//	Returna a altura para o terreno

int Height(BYTE *pAlturaTerreno, int X, int Y)
{
	// Validar se não ultrapassa o tamanho da matriz
	int x = X % TAM_TERRENO;					// verificar o nosso valor x
	int y = Y % TAM_TERRENO;					// verificar o nosso valor y

	if(!pAlturaTerreno) return 0;				// Validar se os valores estão correctos

	// índice = (x + (y * arrayWidth)) para encontrar a altura atual
	return pAlturaTerreno[x + (y * TAM_TERRENO)];	// Índice de nossa matriz em altura e retornar a altura
}

void PosXZBolas()
{
for(int i=0; i<10; i++) { pRandBolas[i] = (50+(rand()%300));}
}

//	Isto define a coordenada de textura corrente do terreno, com base no X e Z

void SetTextureCoord(float x, float z)
{
	// Encontrar a coordenada (u, v) para o vértice corrente
	float u =  (float)x / (float)TAM_TERRENO;
	float v = -(float)z / (float)TAM_TERRENO;
	
	// Dá ao OpenGL a coordenada de textura atual do terreno em relação á altura
	glMultiTexCoord2fARB(GL_TEXTURE0_ARB, u, v);

	
	glMultiTexCoord2fARB(GL_TEXTURE1_ARB, u, v);
}

void colisao (int vPosx, int vPosy, int vPosz) 
{
	for(int i=0; i<10; i++)
	{
		d = sqrt(((vPosx - pBx[i]) * (vPosx - pBx[i])) + ((vPosy - pBy[i]) * (vPosy - pBy[i])) 
			+ ((vPosz - pBz[i]) * (vPosz - pBz[i])));
		if(d <= raio)
			pRandBolas[i] = 0;
	}
}


void CriarListaObj(BYTE pAlturaTerreno[])
{
	/* Variáveis para definição da capacidade de brilho do material */
	GLfloat semespecular[4]={0.0,0.0,0.0,1.0};
	GLfloat especular[] = { 1.0, 1.0, 1.0, 1.0 };
	/* Posição da luz */
	GLfloat posicao[] = { 0.0, 3.0, 2.0, 0.0 };
	/*
	Limpa o buffer de pixels e
	determina a cor padrão dos objetos.
	*/
	lista1 = glGenLists (1);
	glNewList (lista1, GL_COMPILE);
	/* Armazena o estado anterior para
	rotação da posição da luz */
	glColor3f (1.0, 1.0, 1.0);
	glRotated ((GLdouble) posicaoluz, 1.0, 0.0, 0.0);
	glLightfv (GL_LIGHT0, GL_POSITION, posicao);

	glPushMatrix();
	glRotatef (0, 1.0, 0.0, 0.0);
	//glTranslatef (0.0, 2.0, 0.0);
	glTranslatef(100, Height(pAlturaTerreno, 0, 0), 50); 
	glRotatef (0, 0.0, 1.0, 0.0);
	glColor3f (0.0, 0.0, 1.0);
	/* Define a propriedade do material */
	//refletância do material
	glMaterialfv(GL_FRONT,GL_SPECULAR, especular);
	// Define a concentração do brilho
	glMateriali(GL_FRONT,GL_SHININESS,20);
	glutSolidSphere(raio,100,100);
	//glutSolidTorus(0.1,0.2,10,20);
	glPopMatrix();
	glEndList ();
}
//	Render da altura do terreno (QUADS)
void RenderAlturaTerreno(BYTE pAlturaTerreno[])
{
	int X = 0, Y = 0;						// variáveis ​​para deslocar com a matriz.
	int x, y, z;							// variáveis ​​para facilitar a leitura
	bool bSwitchSides = false;

	// Verificar se a altura do terreno é valida
	if(!pAlturaTerreno) return;		
	
	// Ativar o ID primeira textura a vincular
	//glActiveTextureARB(GL_TEXTURE0_ARB);
	//glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, g_Texture[0]);

	// Se queremos mais detalhe texturização, vamos activar a segunda textura
	if(g_bDetail)
	{
		// Ativar o ID segunda textura e vincular 
		glActiveTextureARB(GL_TEXTURE1_ARB);
		//glEnable(GL_TEXTURE_2D);
		
		// Ativar as propriedades juntar e aumentar o RGB
		// gama para o detalhe da textura.
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
		glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE_ARB, 2);
		
		// Vincular os detalhes de textura
		glBindTexture(GL_TEXTURE_2D, g_Texture[1]);
	
		// Agora, queremos entrar na matriz de textura. Isso vai nos permitir
		// alterar a selecção do detalhe da textura.
		glMatrixMode(GL_TEXTURE);

			// Redefinir a matriz atual e aplicar o nosso valor escala escolhido
			glLoadIdentity();
			glScalef((float)g_DetailScale, (float)g_DetailScale, 1);

		// Definir a ModelView
		glMatrixMode(GL_MODELVIEW);
	}

	//glNormal3f(0, 1, 0);
	//glColor3f(1, 1, 1);

	glBegin( GL_TRIANGLE_STRIP );	


	// Percorrer todas as linhas em altura no terreno
	for ( X = 0; X <= TAM_TERRENO; X += TAM_TRING )
	{
		// Verifica se precisamos fazer o render do caminho oposto para esta coluna 
		if(bSwitchSides)
		{	
			// Render da coluna do terreno, para valor actual de X
			// Render do TAM_TERRENO até 0.
			for ( Y = TAM_TERRENO; Y >= 0; Y -= TAM_TRING )
			{
				// Obter o valor (X, Y, Z) para a parte inferior esquerda vértice		
				x = X;							
				y = Height(pAlturaTerreno, X, Y );	
				z = Y;							

				//--

				// Definir a coordenada da textura actual e fazer o render do vertice
				SetTextureCoord( (float)x, (float)z );
				//glNormal3f(0, 1, 0);
				//glColor3f(1, 1, 1);
				glVertex3i(x, y, z);		

				// Obter o valor (X, Y, Z) para a parte inferior direita vértice	
				x = X + TAM_TRING; 
				y = Height(pAlturaTerreno, X + TAM_TRING, Y ); 
				z = Y;

				//--

				// Definir a textura actual e fazer o render do vertice
				SetTextureCoord( (float)x, (float)z );
				//glNormal3f(0, 1, 0);
				//glColor3f(1, 1, 1);
				glVertex3i(x, y, z);			
			}
		}
		else
		{	
			// Render da coluna do terreno, para valor actual de X
			// Render começa em 0 até TAM_TERRENO.
			for ( Y = 0; Y <= TAM_TERRENO; Y += TAM_TRING )
			{
				// Obter o valor (X, Y, Z) para a parte inferior direita vértice	
				x = X + TAM_TRING; 
				y = Height(pAlturaTerreno, X + TAM_TRING, Y ); 
				z = Y;


				// Definir a textura actual e fazer o render do vertice
				SetTextureCoord( (float)x, (float)z );
				//glNormal3f(0, 1, 0);
				//glColor3f(1, 1, 1);
				glVertex3i(x, y, z);

				//Obter o valor (X, Y, Z) para a parte inferior esquerda vértice		
				x = X;							
				y = Height(pAlturaTerreno, X, Y );	
				z = Y;

				//--

				// Definir a textura actual e fazer o render do vertice
				SetTextureCoord( (float)x, (float)z );

				glNormal3f(0, 1, 0);
				glColor3f(1, 1, 1);

				glVertex3i(x, y, z);		
			}
		}


		// Alterar a direcção do render 
		bSwitchSides = !bSwitchSides;
	}

	// Parar o render triangle strips
	glEnd();

	int ran;
	//Desenha as Esfera
	for(int i=1; i<10; i++)
	{
		if (pRandBolas[i]>0)
		{
			ran = pRandBolas[i];
			//Esfera
			glPushMatrix();
			glTranslatef(100*i+ran, Height(pAlturaTerreno, X, Y ), 50*i+ran); 

			pBx[i] = (100*i)+ran;
			pBy[i] = Height(pAlturaTerreno, X, Y );
			pBz[i] = (50*i)+ran;

			//glColor3f(1,1,0);
			// Desenha uma esfera com o raio definido pela variavel raio
			glutSolidSphere(raio,100,100);
			glPopMatrix();
		}
	}

	// Desactivar a 2 textura
	glActiveTextureARB(GL_TEXTURE1_ARB);
    glDisable(GL_TEXTURE_2D);

	// Desactivar a 1 textura
	glActiveTextureARB(GL_TEXTURE0_ARB);		
    glDisable(GL_TEXTURE_2D);
}

//	Load do .raw file no array de bytes.  Cada valor é um valor de altura.
void CarregarRawFile(LPSTR strName, int nSize, BYTE *pAlturaTerreno)
{
	FILE *pFile = NULL;

	// abrir o arquivo em leitura / modo binário.
	pFile = fopen( strName, "rb" );

	// Valida se existe o ficheiro
	if ( pFile == NULL )	
	{
		// Mostra a mesagem de erro e para a função
		MessageBox(NULL, "Não foi possivel encontrar o ficheiro!", "Erro", MB_OK);
		return;
	}

	// Carregar o ficheiro .raw do nosso terreno
	// Estamos apenas a ler em (1), e o tamanho é (largura*altura)
	fread( pAlturaTerreno, 1, nSize, pFile );

	// Validar se esta tudo bem com pFile
	int result = ferror( pFile );

	// Verificar se ocorreu erro
	if (result)
	{
		MessageBox(NULL, "Não foi possivel obter os dados!", "Erro", MB_OK);
	}

	// Fechar o ficheiro.
	fclose(pFile);
}


/////////////////////////////////////////////////////////////////////////////////
//
// * Notas * 
//
        
