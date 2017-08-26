/////////////////////////////////////////////////////////////////////////////////
//
// *Notas* 
//
// Este arquivo contém todas as definições e funções para tratar o terreno.
//
// 

#ifndef _TERRENO_H
#define _TERRENO_H

#define TAM_TERRENO		1024				// Tamanho do nosso .raw do terreno
#define TAM_TRING		16					// Largura e a altura de cada grelha de triângulo

// Retorna a altura (0 a 255) a partir de uma dada altura do terreno um X e Y
int Height(BYTE *pAlturaTerreno, int X, int Y);

// Carrega um ficheiro em bruto de um determinado tamanho a partir do ficheiro
void CarregarRawFile(LPSTR strName, int nSize, BYTE *pAlturaTerreno);

// Transforma os dados de alturas em primitivos e chama-os para o ecrá
void RenderAlturaTerreno(BYTE *pAlturaTerreno);

void colisao (int vPosx, int vPosy, int vPosz);

void CriarListaObj(BYTE pAlturaTerreno[]);

void PosXZBolas();

#endif
