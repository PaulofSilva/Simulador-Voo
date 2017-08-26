#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glaux.lib")

#include "main.h"								
#include "camera.h"								
#include "terreno.h"	
bool  g_bFullScreen = true;						// Variavel para ecrã FullScreen a true
HWND  g_hWnd;									// Este é o identificador para a janela
RECT  g_rRect;									// Isto mantém as dimensões da janela
HDC   g_hDC;									// Geral HDC - (lidar com o contexto de dispositivo)
HGLRC g_hRC;									// Geral OpenGL_DC - Contexto de Rendering Context for OpenGL
HINSTANCE g_hInstance;							// Isto mantem a global hInstance para UnregisterClass() em DeInitPrograma()

UINT g_FontListID = 0;
HFONT hOldFont;

#define MAX_CHARS	256
#define FONT_EXTRUDE	0.4f

GLYPHMETRICSFLOAT g_GlyphInfo[MAX_CHARS];

UINT CreateOpenGL3DFont(LPSTR strFontName, float extrude)	
{
	UINT	fontListID = 0;								
	HFONT	hFont;										

	fontListID = glGenLists(MAX_CHARS);					// Cria Lista de fontes


	hFont = CreateFont(	0,								
						0,								
						0,								
						0,								
						FW_BOLD,						
						FALSE,							
						FALSE,							
						FALSE,							
						ANSI_CHARSET,					
						OUT_TT_PRECIS,					
						CLIP_DEFAULT_PRECIS,			
						ANTIALIASED_QUALITY,			
						FF_DONTCARE|DEFAULT_PITCH,		
						strFontName);					

	hOldFont = (HFONT)SelectObject(g_hDC, hFont);

	wglUseFontOutlines(	g_hDC,							
						0,								
						MAX_CHARS - 1,					
						fontListID,						
						0,								
						extrude,						
						WGL_FONT_POLYGONS,				
						g_GlyphInfo);					

	return fontListID;									
}


void glDraw3DText(const char *strString, ...)
{
	char		strText[256];							
	va_list		argumentPtr;							//  ponteiro para a lista de argumentos
	float		unitLength=0.0f;						//  armazena o comprimento da fonte 3D em unidade de comprimento

	if (strString == NULL)								// Verifique se a string é null
		return;											

	// First we need to parse the string for arguments given
	// To do this we pass in a va_list variable that is a pointer to the list of arguments.
	// Then we pass in the string that holds all of those arguments.
	va_start(argumentPtr, strString);					// Parse the arguments out of the string

	// Then we use a special version of sprintf() that takes a pointer to the argument list.
	// This then does the normal sprintf() functionality.
	vsprintf(strText, strString, argumentPtr);			// Now add the arguments into the full string

	va_end(argumentPtr);								// This resets and frees the pointer to the argument list.

	// Below we find out the total length of the characters in 3D units, then center them.

	for (int i=0; i < (int)strlen(strText); i++)		// Go through all of the characters
	{
		// Here we go through each character and add up it's length.
		// It doesn't matter where the text if facing in 3D, it uses a 2D system
		// for the width and height of the characters.  So X is the width, Y is the height.
		unitLength += g_GlyphInfo[strText[i]].gmfCellIncX;	
	}

	// Here we translate the text centered around the XYZ according to it's width.
	// Since we have the length, we can just divide it by 2, then subtract that from the X.
	// This will then center the text at that position, in the way of width.
	// Can you see how to center the height too if you wanted? (hint: .gmfCellIncY)
	glTranslatef(0.0f - (unitLength / 2), 0.0f, 0.0f);

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


	// Now, before we set the list base, we need to save off the current one.
	glPushAttrib(GL_LIST_BIT);							// This saves the list base information

	// Then we want to set the list base to the font's list base, which should be 1 in our case.
	// That way when we call our display list it will start from the font's lists'.
	glListBase(g_FontListID);							// This sets the lists base

	// Now comes the actually rendering.  We pass in the length of the string,
	// then the data types (which are characters so its a UINT), then the actually char array.
	// This will then take the ASCII value of each character and associate it with a bitmap.
	glCallLists((int)strlen(strText), GL_UNSIGNED_BYTE, strText);

	glPopAttrib();										// Return the display list back to it's previous state
}



// Ponteiros de função para as funções ARB multitexturing
PFNGLMULTITEXCOORD2FARBPROC		glMultiTexCoord2fARB	 = NULL;
PFNGLACTIVETEXTUREARBPROC		glActiveTextureARB		 = NULL;

// Isto controla se temos detalhes texturização ligado ou desligado
bool g_bDetail = true;

// Lida com a escala atual para a matriz de textura para os detalhes de textura
int g_DetailScale = 16;

// Objecto da camera global
CCamera g_Camera;								

// Contem a altura do terreno
BYTE g_AlturaTerreno[TAM_TERRENO*TAM_TERRENO];			

// Isto diz-nos se queremos linhas ou modo de preenchimento
bool  g_bRenderMode = true;	



// Isto é a informação do ID da textura
UINT g_Texture[MAX_TEXTURES] = {0};				

#define BACK_ID		11		// O ID de textura para o lado de trás do cubo
#define FRONT_ID	12		// O ID de textura para o lado da frente do cubo
#define BOTTOM_ID	13		// O ID de textura para o lado inferior do cubo
#define TOP_ID		14		// O ID de textura para o lado superior do cubo
#define LEFT_ID		15		// O ID de textura para o lado esquerdo do cubo
#define RIGHT_ID	16		// O ID de textura para o lado direito do cubo


//	Isto cria uma caixa de céu centrada em torno de XYZ com uma altura, largura e comprimento


void SkyBox(float x, float y, float z, float width, float height, float length)
{
	// Activar o mapeamento da textura
	glEnable(GL_TEXTURE_2D);

	// Vincular a textura ao lado de trás do cubo
	glBindTexture(GL_TEXTURE_2D, g_Texture[BACK_ID]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Centra a caixa em torno (x, y, z)
	x = x - width  / 2;
	y = y - height / 2;
	z = z - length / 2;

	// Desenha o lado de trás com o G_QUADS
	glBegin(GL_QUADS);		

	// Atribuir  as coordenadas da textura e os vertices para o lado de trás
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x + width, y,			z);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x + width, y + height, z); 
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x,			y + height, z);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x,			y,			z);

	glEnd();

	// Vincular a textura ao lado da frente do cubo
	glBindTexture(GL_TEXTURE_2D, g_Texture[FRONT_ID]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	// Desenha o lado da frente com o G_QUADS
	glBegin(GL_QUADS);	

	// Atribuir  as coordenadas da textura e os vertices para o lado da frente
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x,			y,			z + length);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x,			y + height, z + length);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y + height, z + length); 
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y,			z + length);
	glEnd();

	// Vincular a textura ao lado inferior do cubo
	glBindTexture(GL_TEXTURE_2D, g_Texture[BOTTOM_ID]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Desenha o lado inferior com o G_QUADS
	glBegin(GL_QUADS);		

	// Atribuir  as coordenadas da textura e os vertices para o lado inferior
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x,			y,			z);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x,			y,			z + length);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y,			z + length); 
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y,			z);
	glEnd();

	// Vincular a textura ao lado superior do cubo
	glBindTexture(GL_TEXTURE_2D, g_Texture[TOP_ID]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Desenha o lado superior com o G_QUADS
	glBegin(GL_QUADS);		

	// Atribuir  as coordenadas da textura e os vertices para o lado superior
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y + height, z);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y + height, z + length); 
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x,			y + height,	z + length);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x,			y + height,	z);

	glEnd();

	// Vincular a textura ao lado esquerdo do cubo
	glBindTexture(GL_TEXTURE_2D, g_Texture[LEFT_ID]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Desenha o lado com o G_QUADS
	glBegin(GL_QUADS);		

	// Atribuir  as coordenadas da textura e os vertices para o lado esquerdo
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x,			y + height,	z);	
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x,			y + height,	z + length); 
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x,			y,			z + length);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x,			y,			z);		

	glEnd();

	// Vincular a textura ao lado direiro do cubo
	glBindTexture(GL_TEXTURE_2D, g_Texture[RIGHT_ID]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Desenha o lado com o G_QUADS
	glBegin(GL_QUADS);		

	// Atribuir  as coordenadas da textura e os vertices para o lado direito
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y,			z);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x + width, y,			z + length);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x + width, y + height,	z + length); 
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y + height,	z);
	glEnd();
}


//	Esta função inicializa a aplicação


void InitPrograma(HWND hWnd)
{
	g_hWnd = hWnd;										// Atribuir o identificador de janela para um identificador global de janela
	GetClientRect(g_hWnd, &g_rRect);					// Atribuir o retângulo janelas a um global RECT 
	InicializaOpenGL(g_rRect.right, g_rRect.bottom);	// Iniciar OpenGL com global rect

	// Inicializa o tipo de letra
	g_FontListID = CreateOpenGL3DFont("Arial", FONT_EXTRUDE);

	// Aqui nós inicializar nossas funções multitexturing
	glActiveTextureARB		= (PFNGLACTIVETEXTUREARBPROC)		wglGetProcAddress("glActiveTextureARB");
	glMultiTexCoord2fARB	= (PFNGLMULTITEXCOORD2FARBPROC)		wglGetProcAddress("glMultiTexCoord2fARB");

	// Vamos validar se as funções multitexturing estão activas
	// com versão actual do OpenGL. Caso contrario os apontadores estão a NULL  
	if(!glActiveTextureARB || !glMultiTexCoord2fARB)
	{
		// Dá mensaguem de erro e sai fora da aplicação
		MessageBox(g_hWnd, "A sua configuração actual não suporta multitexturing!", "Erro", MB_OK);
		PostQuitMessage(0);
	}

	// Aqui lemos a altura do terreno a partir do arquivo em bruto e vamos colocalo no nosso array g_AlturaTerreno
	// Ficheiro .raw  tamanho (1024).
	CarregarRawFile("terreno.raw", TAM_TERRENO * TAM_TERRENO, g_AlturaTerreno);	

	glEnable(GL_DEPTH_TEST);							// Activar o teste de profundidade
	glEnable(GL_TEXTURE_2D);							// Activar mapeamento de textura
	glEnable(GL_CULL_FACE);								// Activar back face culling
	//glEnable(GL_LIGHTING);								// Activa Luz
	//glEnable(GL_LIGHT0);								// Activa luz 0



	CriarTexture(g_Texture[0],			"terreno.bmp"); // Carregar a textura do terreno
	//CriarTexture(g_Texture[1],			"Detail.bmp");
	CriarTexture(g_Texture[BACK_ID],	"Back.bmp");	// Carregar a textura da caixa Back
	CriarTexture(g_Texture[FRONT_ID],	"Front.bmp");	// Carregar a textura da caixa Front
	CriarTexture(g_Texture[BOTTOM_ID],	"Bottom.bmp");	// Carregar a textura da caixa Botton
	CriarTexture(g_Texture[TOP_ID],		"Top.bmp");		// Carregar a textura da caixa Top
	CriarTexture(g_Texture[LEFT_ID],	"Left.bmp");	// Carregar a textura da caixa Left
	CriarTexture(g_Texture[RIGHT_ID],	"Right.bmp");	// Carregar a textura da caixa Right
	
	// Criar lista de objectos
	CriarListaObj(g_AlturaTerreno);
	PosXZBolas(); // gera posição no x e z das bolas

	// Definir um ponto de posição para a camara na caixa
	g_Camera.PositionCamera( 280, 35, 225,  281, 35, 225,  0, 1, 0);
}


///////////////////////////////// LOOP PRINCIPAL DO JOGO \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	Essa função manipula o loop principal do jogo
/////
///////////////////////////////// LOOP PRINCIPAL DO JOGO \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

WPARAM MainLoop()
{
	MSG msg;

	while(1)											// Fazer o nosso loop infInitProgramao
	{													// Verifique se havia uma mensagem
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
		{ 
			if(msg.message == WM_QUIT)					// Se a mensagem não era para sair
				break;
			TranslateMessage(&msg);						// Descubrir o que a mensagem faz
			DispatchMessage(&msg);						// Executar a mensagem
		}
		else											// se não houver uma mensagem
		{ 
			g_Camera.Update();							// Atualizar os dados da câmera
			DesenhaCena();								// Render da cena para todas as frames
		} 
	}

	DeInitPrograma();											// Liberar toda a memória alocada definida no InitPrograma.cpp

	return(msg.wParam);									// Retorno do programa
}


void DesenhaAviaoTri() 
{
	glLoadIdentity();									// Reset 

	glTranslatef(0, 0, -10);															
	glBegin (GL_TRIANGLES);								
	glVertex3f(0, 1, 0);				
	glVertex3f(-1, 0, 0);	
	glVertex3f(1, 0, 0);	
	glEnd();											
}

///////////////////////////////// RENDER DA CENA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	Esta função faz renders da cena inteira.
/////
///////////////////////////////// RENDER DA CENA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void DesenhaCena() 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Limpar o ecrã e o buffer de profundidade
	glLoadIdentity();									// Reset da matrix

	// Obter a posição actual da câmera
	CVectorPontos vPos		= g_Camera.Position();
	CVectorPontos vNewPos    = vPos;

	// Verificar se a câmara está abaixo da altura do terreno em X e Z,
	// adicionamos 10 garantir que a câmera não está no chão.
	if(vPos.y < Height(g_AlturaTerreno, (int)vPos.x, (int)vPos.z ) + 20)
	{
		// Definir a nova posição da câmera para que ela fique +10 acima do terreno
		vNewPos.y = (float)Height(g_AlturaTerreno, (int)vPos.x, (int)vPos.z ) + 20;

		// Obter a diferença do y (câmera) puxada para trás 
		float temp = vNewPos.y - vPos.y;

		//  Obter a visão atual e incrementar para outra a posição quando é movida
		CVectorPontos vView = g_Camera.View();
		vView.y += temp;

		// Definir a nova posição da câmera.
		g_Camera.PositionCamera(vNewPos.x,  vNewPos.y,  vNewPos.z,
			vView.x,	vView.y,	vView.z,	0, 1, 0);								
	}

	// Dar ao OpenGL a posição da câmera
	g_Camera.Look();

	// Valida colisao com as bolas
	colisao((int)vPos.x, (int)vPos.y, (int)vPos.z);

	// Render da altura do terreno
	RenderAlturaTerreno(g_AlturaTerreno);						

	// Criar a caixa e centralizá-lo em todo o terreno
	SkyBox(500, 0, 500, 2000, 2000, 2000);

	//desenhar Aviao
	glPushMatrix();
	DesenhaAviaoTri(); 
	glPopMatrix();

	// --Texto								

	glPushMatrix();
	glColor3f(1, 1, 0);	

		glTranslatef(-6, 15, -1);						
		//glDraw3DText("GameOver");			// Draw do texto

	glPopMatrix();						



	// Trocar as backbuffers para o plano principal
	SwapBuffers(g_hDC);									
}


//	Essa função manipula as mensagens de janela.


LRESULT CALLBACK WinProc(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT    ps;

	switch (uMsg)
	{ 
	case WM_SIZE:										// Se a janela for redimensionada
		if(!g_bFullScreen)								// Fazer isto apenas se não estivermos em full screen
		{
			RedimensionaJanela(LOWORD(lParam),HIWORD(lParam));// LoWord=Width, HiWord=Height
			GetClientRect(hWnd, &g_rRect);				// Obter o retângulo da janela
		}
		break; 

	case WM_PAINT:										// Se precisamos pintar a cena
		BeginPaint(hWnd, &ps);							// InitPrograma da paint struct		
		EndPaint(hWnd, &ps);							// EndPaint e limpar
		break;


	case WM_LBUTTONDOWN:

		g_bDetail = !g_bDetail;
		break;

	case WM_RBUTTONDOWN:								// Se o botão esquerdo do mouse for clicado

		g_bRenderMode = !g_bRenderMode;

		// Alterar o modo de renderização para linhas e triângulos
		if(g_bRenderMode) 				
		{
			// Tornar os triângulos no modo de preenchimento		
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	
		}
		else 
		{
			// Tornar os triângulos no modo de wire frame
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	
		}
		break;

	case WM_KEYDOWN:
		switch(wParam) 
		{
		case VK_ESCAPE:								// Verifique se pressionar a tecla ESCAPE.
			PostQuitMessage(0);						// Diz ao windows que queremos SAIR
			break;

		case VK_SPACE:								// Verifique se pressionar a barra de ESPAÇO

			// Para obter ideias diferentes como a textura pode ficar
			// alteramos o valor de escala.

			// Actual valor de escala *2 e fazer loop até ao valor 128
			g_DetailScale = (g_DetailScale * 2) % 128;

			// Se o valor da escala é de 0, configurá-lo para 1 novamente
			if(g_DetailScale == 0)
				g_DetailScale = 1;

			break;

		}
		break;

	case WM_CLOSE:										// Se fecharmos o programa
		PostQuitMessage(0);								// Diz ao windows que queremos SAIR
		break; 
	} 

	return DefWindowProc (hWnd, uMsg, wParam, lParam); 	// Retorna valores padrão
}   
