/////////////////////////////////////////////////////////////////////////////////
//
// *Notas* 
//
// Classe com os metodos para controlo da camera
// Este arquivo contém todas as definições e funções para tratar a camera.
//
//

#ifndef _CAMERA_H
#define _CAMERA_H


// Esta e classe camera
class CCamera {

public:

	// Construtor
	CCamera();	

	// Funções de acesso a dados para dados privados nossa câmera
	CVectorPontos Position() {	return m_vPosition;		}
	CVectorPontos View()		{	return m_vView;			}
	CVectorPontos UpVector() {	return m_vUpVector;		}
	CVectorPontos Strafe()	{	return m_vStrafe;		}
	
	// Muda a posição, vista, e o vetor da câmera.
	// Inicialização das variaveis da camera
	void PositionCamera(float positionX, float positionY, float positionZ,
			 		    float viewX,     float viewY,     float viewZ,
						float upVectorX, float upVectorY, float upVectorZ);

	// Rodar vista da câmara em torno da posição, dependendo dos valores recebidos
	void RotateView(float angle, float X, float Y, float Z);

	// Mover a visão da câmera, pelos movimentos do rato
	void SetViewByMouse(); 

	// Este gira a câmera em torno de um ponto
	void RotateAroundPoint(CVectorPontos vCenter, float X, float Y, float Z);

	// Este strafes a esquerda ou para a direita da câmara, dependendo da velocidade (+ / -)
	void StrafeCamera(float speed);

	// Para mover a câmara para a frente ou para trás, dependendo da velocidade
	void MoveCamera(float speed);

	// Para verificar o movimento do teclado
	void CheckForMovement();

	// Isto atualiza de visão da câmera e outros dados (Deve ser chamada a cada frame)
	void Update();

	// Usa gluLookAt () para dizer OpenGL para onde olhar
	void Look();

private:

	// A posição da câmera
	CVectorPontos m_vPosition;					

	// A visão da câmera
	CVectorPontos m_vView;						

	// Vector da câmera
	CVectorPontos m_vUpVector;		
	
	// Vector da câmera strafe - geralmente movimento directamente para a esquerda ou direita, em vez de rodar a câmara
	CVectorPontos m_vStrafe;						
};

#endif
