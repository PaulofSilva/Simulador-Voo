
#include "main.h"


///////////////////////////////// Criar Texturas \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	
/////   Vamos criar uma textura em opengl que poderemos mapear
/////
///////////////////////////////// Criar Texturas \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool CriarTexture(UINT &texture, LPSTR strFileName)
{
	AUX_RGBImageRec *pImage = NULL;
	FILE *pFile = NULL;

	if(!strFileName) 
		return false;

	// Abre o apontador para o ficheiro BMP e valida se foi encontrado e aberto
	if((pFile = fopen(strFileName, "rb")) == NULL) 
	{
		// Mensagem de erro
		MessageBox(g_hWnd, "Erro a carregar o ficheiro BMP!", "Error", MB_OK);
		return NULL;
	}

	// Carrega o bitmap utilizando a funcao aux (glaux.lib)
	pImage = auxDIBImageLoad(strFileName);				

	// Verifica se a imagem é valida
	if(pImage == NULL)								
		return false;

	// Gerar uma textura com o ID de textura associativa e guardar na matriz
	glGenTextures(1, &texture);

	
	// Isto define os requisitos de alinhamento para o início de cada fila de pixels na memória.
	glPixelStorei (GL_UNPACK_ALIGNMENT, 1);

	
	// Vincular a textura para o índice da matriz de texturas e inicia a textura
	glBindTexture(GL_TEXTURE_2D, texture);
	
	// Construir Mipmaps (cria versões diferentes da imagem para distâncias - Aparencia melhorada)
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, pImage->sizeX, 
					  pImage->sizeY, GL_RGB, GL_UNSIGNED_BYTE, pImage->data);

	//Atribuir os níveis mipmap e informações da textura
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	// Agora precisamos liberar os dados de imagem que nós carregamos do openGL armazenado como uma textura

	if (pImage)										// Sw carregamos a imagem
	{
		if (pImage->data)							// Se há uma textura
		{
			free(pImage->data);						// Limpa a textura, nao sera necessario daqui para diante
		}

		free(pImage);								// Limpa a estrutura da imagem
	}

	
	return true;
}


///////////////////////////////// Altera para Full Screen \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	Altera o ecra para Full Screen
/////
///////////////////////////////// Altera para Full Screen \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void AlterarFullSreen()
{
	DEVMODE dmSettings = {0};						

	// Obter configurações atuais - Esta função preenche nossas as configurações
	// This makes sure NT and Win98 machines change correctly
	if(!EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&dmSettings))
	{
		MessageBox(NULL, "Verifique as definições do ecrã", "Erro", MB_OK);
		return;
	}

	dmSettings.dmPelsWidth	= SCREEN_WIDTH;				// Selecciona a largura do ecrã
	dmSettings.dmPelsHeight	= SCREEN_HEIGHT;			// Selecciona a altura do ecrã
	dmSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

	// Alterar o ecra pra full screen
	// CDS_FULLSCREEN remove a barra de iniciar.
	int result = ChangeDisplaySettings(&dmSettings,CDS_FULLSCREEN);	

	if(result != DISP_CHANGE_SUCCESSFUL)
	{
		
		MessageBox(NULL, "Modo do ecrã não compativel", "Erro", MB_OK);
		PostQuitMessage(0);
	}
}


///////////////////////////////// Criar a janela \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	Esta funcao cria a janela (copia da net)
/////	http://msdn.microsoft.com/en-us/library/windows/desktop/ms632679(v=vs.85).aspx
///////////////////////////////// Criar a janela \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

HWND CreateMyWindow(LPSTR strWindowName, int width, int height, DWORD dwStyle, bool bFullScreen, HINSTANCE hInstance)
{
	HWND hWnd;
	WNDCLASS wndclass;
	
	memset(&wndclass, 0, sizeof(WNDCLASS));				// Inicia o tamanho da class
	wndclass.style = CS_HREDRAW | CS_VREDRAW;			// Recursos normais de desenho
	wndclass.lpfnWndProc = WinProc;						// Passar o apontador da função como o processo da janela
	wndclass.hInstance = hInstance;						
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);	// Icon geral
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);		// Uma seta como cursor
	wndclass.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);	// Janela branca
	wndclass.lpszClassName = "Simvoo";					// Atribuir o nome da classe

	RegisterClass(&wndclass);							// 
	
	if(bFullScreen && !dwStyle) 						// Verifica se vai correr em full screen
	{													// Define como full screen
		dwStyle = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		AlterarFullSreen();							
		ShowCursor(FALSE);								// oculta o cursor
	}
	else if(!dwStyle)									
		dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	
	g_hInstance = hInstance;							 

	RECT rWindow;
	rWindow.left	= 0;								// Valor à esquerda a 0
	rWindow.right	= width;							// Valor à direita = à largura
	rWindow.top	    = 0;								// Valor do topo a 0
	rWindow.bottom	= height;							// Value da base = à altura

	AdjustWindowRect( &rWindow, dwStyle, false);		

														// criar a janela
	hWnd = CreateWindow("Simvoo", strWindowName, dwStyle, 0, 0,
						rWindow.right  - rWindow.left, rWindow.bottom - rWindow.top, 
						NULL, NULL, hInstance, NULL);

	if(!hWnd) return NULL;								

	ShowWindow(hWnd, SW_SHOWNORMAL);					// Mostra a janela
	UpdateWindow(hWnd);									// Desenha a janela

	SetFocus(hWnd);										

	return hWnd;
}

///////////////////////////////// Define o formato dos pixeis \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	Esta função define o formato dos pixeis (copia da net)
/////
///////////////////////////////// Define o formato dos pixeis \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool bSetupPixelFormat(HDC hdc) 
{ 
    PIXELFORMATDESCRIPTOR pfd = {0}; 
    int pixelformat; 
 
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);			// Define o tamanho da estrutura
    pfd.nVersion = 1;									
														
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER; 
    pfd.dwLayerMask = PFD_MAIN_PLANE;					
    pfd.iPixelType = PFD_TYPE_RGBA;						// RGB e tipo Alpha pixel
    pfd.cColorBits = SCREEN_DEPTH;						// aqui utilizamos o nosso #define para os bits de cor
    pfd.cDepthBits = SCREEN_DEPTH;						// Depthbits é ignorado para RGB
    pfd.cAccumBits = 0;									// No special bitplanes needed
    pfd.cStencilBits = 0;								// We desire no stencil bits
 
	// This gets us a pixel format that best matches the one passed in from the device
    if ( (pixelformat = ChoosePixelFormat(hdc, &pfd)) == FALSE ) 
    { 
        MessageBox(NULL, "ChoosePixelFormat failed", "Error", MB_OK); 
        return FALSE; 
    } 
 
	// This sets the pixel format that we extracted from above
    if (SetPixelFormat(hdc, pixelformat, &pfd) == FALSE) 
    { 
        MessageBox(NULL, "SetPixelFormat failed", "Error", MB_OK); 
        return FALSE; 
    } 
 
    return TRUE;										
}

//////////////////////////// Resize da janela do openGL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	Esta funcao faz o resize da viewport para o OpenGL.
/////
//////////////////////////// Redimensiona da janela do openGL\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
void RedimensionaJanela(int largura, int altura)			
{
	GLfloat aspectRatio = (GLfloat)largura / (GLfloat)altura;

	if (altura==0)										// evita uma divisão por 0
	{
		altura=1;										
	}

	glViewport(0,0,largura,altura);						// a viewport passa a ser a janela
														
	glMatrixMode(GL_PROJECTION);						
	glLoadIdentity();									
				  
	gluPerspective(45.0f, aspectRatio, 4 ,4000.0f);

	glMatrixMode(GL_MODELVIEW);							// Selecciona a matriz Modelview
	glLoadIdentity();									// Reset à matriz Modelview
}

///////////////////////////////// Inicializa o opengl \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	Nesta funcao tratamos de toda a incialização do opengl
/////
///////////////////////////////// Inicializa o opengl \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void InicializaOpenGL(int width, int height) 
{  
    g_hDC = GetDC(g_hWnd);								
														
    if (!bSetupPixelFormat(g_hDC))						// Define o nosso formato de pixeis 
        PostQuitMessage (0);							

    g_hRC = wglCreateContext(g_hDC);					
    wglMakeCurrent(g_hDC, g_hRC);

	// Activa a utilização de texturas
	//glEnable(GL_TEXTURE_2D);

	// Define a cor de limpeza do color buffer
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	
	// ILUMINAÇÃO
	// Define caracteristicas da luz
	GLfloat  whiteLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat  sourceLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat	 lightPos[] = { 0.0f, -1.0f, 0.0f, 1.0f };
	

	// Define as caracteristicas e activa o ponto de luz 0
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,whiteLight);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,sourceLight);
	glLightfv(GL_LIGHT0,GL_POSITION,lightPos);

	// Activa a associação do glColor com o material
	glEnable(GL_COLOR_MATERIAL);
	// Associa o glColor com as propriedades ambiente e difusa do material
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// FIM da ILUMINAÇÃO
	
	RedimensionaJanela(width, height);					
}

///////////////////////////////// Limpar todas as variaveis \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	Limpar todas as variaveis
/////
///////////////////////////////// Limpar todas as variaveis \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void DeInitPrograma()
{
	if (g_hRC)											
	{
		wglMakeCurrent(NULL, NULL);						// Limpa a memoria de rendering
		wglDeleteContext(g_hRC);						// Remove o nosso contexto de rendering	
	}
	
	if (g_hDC) 
		ReleaseDC(g_hWnd, g_hDC);						
		
	if(g_bFullScreen)									// Se estiver em full screen 
	{
		ChangeDisplaySettings(NULL,0);					// Volta para o desktop
		ShowCursor(TRUE);								// mostra o cursor do rato
	}

	UnregisterClass("Simvoo", g_hInstance);				// Limpa a classe de janela

	PostQuitMessage (0);								
}


///////////////////////////////// WIN MAIN \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	Regista e cria a janela
/////
///////////////////////////////// WIN MAIN \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hprev, PSTR cmdline, int ishow)
{	
	HWND hWnd;

	//Valida de queremos full screen ou nao
	if(MessageBox(NULL, "Click Yes to go to full screen (Recommended)", "Options", MB_YESNO | MB_ICONQUESTION) == IDNO)
		g_bFullScreen = false;
	
	// Cria a janela com a funcao que criamos, devemos passar:
	// Nome, largura, altura, se queremos full screen ou nao, e o hInstance
	hWnd = CreateMyWindow("Simulador de voo", SCREEN_WIDTH, SCREEN_HEIGHT, 0, g_bFullScreen, hInstance);

	
	if(hWnd == NULL) return TRUE;

	// InitPrograma OpenGL
	InitPrograma(hWnd);													

	
	return MainLoop();						
}
