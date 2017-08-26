/////////////////////////////////////////////////////////////////////////////////
//
// *Notas* 
//
// Este arquivo contém todas as definições e funções para tratar o terreno.
//
//

#ifndef _MAIN_H
#define _MAIN_H

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <gl\freeglut.h>											
#include <gl\glaux.h>									
#include <math.h>

// Incluir arquivo de cabeçalho para o nosso Terreno.cpp
#include "Terreno.h"									

// Definições das multiplas texturas
#define GL_TEXTURE0_ARB                     0x84C0
#define GL_TEXTURE1_ARB                     0x84C1

#define GL_COMBINE_ARB						0x8570
#define GL_RGB_SCALE_ARB					0x8573

// Prototipos de funções multitexturas
typedef void (APIENTRY * PFNGLMULTITEXCOORD2FARBPROC) (GLenum target, GLfloat s, GLfloat t);
typedef void (APIENTRY * PFNGLACTIVETEXTUREARBPROC) (GLenum target);

// Apontadores de função para os recursos multitexturas
extern PFNGLMULTITEXCOORD2FARBPROC			glMultiTexCoord2fARB;
extern PFNGLACTIVETEXTUREARBPROC			glActiveTextureARB;


#define MAX_TEXTURES 1000								// O Valor máximo de texturas a carregar

extern UINT g_Texture[MAX_TEXTURES];					// Armazena os IDs de textura

// Precisamos definir isto para glTexParameteri ()
#define GL_CLAMP_TO_EDGE	0x812F						// Texturas da caixa da área

// Dimensões da caixa da área													
#define SCREEN_WIDTH 800								// Largura 800 pixels
#define SCREEN_HEIGHT 600								// Altura 600 pixels
#define SCREEN_DEPTH 16									// Definimos 16 bits por pixel

extern bool  g_bFullScreen;								// Definir ecrá full screen 
extern HWND  g_hWnd;									// Identificador para a janela
extern RECT  g_rRect;									// Mantém as dimensões da janela
extern HDC   g_hDC;										// Geral HDC - (lidar com o contexto de dispositivo)
extern HGLRC g_hRC;										// Geral OpenGL_DC - Nosso contexto Rendering para OpenGL
extern HINSTANCE g_hInstance;							// Fixar a nossa janela hInstance



// Classe de pontos
class CVectorPontos
{
public:
	
	// Cnstructor
	CVectorPontos() {}

	// Permite inicializar os dados na criação de uma instância
	CVectorPontos(float X, float Y, float Z) 
	{ 
		x = X; y = Y; z = Z;
	}

	// Server sobrecarregamos o operador + para que possamos adicionar vetores juntos 
	CVectorPontos operator+(CVectorPontos vVector)
	{
		// Retornar o resultado vetores agregado.
		return CVectorPontos(vVector.x + x, vVector.y + y, vVector.z + z);
	}

	// Sobrecarregamos o operador - para que possamos subtrair vetores
	CVectorPontos operator-(CVectorPontos vVector)
	{
		// Retorna o resultado subtraído vetores
		return CVectorPontos(x - vVector.x, y - vVector.y, z - vVector.z);
	}
	
	// Sobrecarregamos o operador * para que possamos multiplicar por escalares
	CVectorPontos operator*(float num)
	{
		// Retorna o vetor 
		return CVectorPontos(x * num, y * num, z * num);
	}

	// Sobrecarregamos o operador / para que possamos dividir por um escalar
	CVectorPontos operator/(float num)
	{
		// Retorne o vetor 
		return CVectorPontos(x / num, y / num, z / num);
	}

	float x, y, z;						
};


// (MAIN) para Janela
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hprev, PSTR cmdline, int ishow);

// Processa todas as mensagens.
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

// Controlar o loop principal do programa
WPARAM MainLoop();

// Isto cria um mapa de textura a partir de um nome de arquivo e uma referência a um UINT para armazená-lo
bool CriarTexture(UINT &texture, LPSTR strFileName);

// Altera o ecrã para FullScreen
void AlterarFullSreen();

// Esta é a nossa própria função que torna a criação de uma janela modular e fácil
HWND CreateMyWindow(LPSTR strWindowName, int width, int height, DWORD dwStyle, bool bFullScreen, HINSTANCE hInstance);

// Permite configurar a nossa janela para OpenGL e backbuffered
bool bSetupPixelFormat(HDC hdc);

// InitProgramas translações e projeções
void RedimensionaJanela(int width, int height);

// Configura OpenGL
void InicializaOpenGL(int width, int height);

// Inicializa o programa
void InitPrograma(HWND hWnd);

// Desenha toda a cena
void DesenhaCena();

// Libertar toda a memória definida no InitPrograma.cpp
void DeInitPrograma();


#endif 
