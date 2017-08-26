/////////////////////////////////////////////////////////////////////////////////
//
// *Notas* 
//
// Classe com os metodos para controlo da camera
// Este arquivo cont�m todas as defini��es e fun��es para tratar a camera.
//
//

#ifndef _CAMERA_H
#define _CAMERA_H


// Esta e classe camera
class CCamera {

public:

	// Construtor
	CCamera();	

	// Fun��es de acesso a dados para dados privados nossa c�mera
	CVectorPontos Position() {	return m_vPosition;		}
	CVectorPontos View()		{	return m_vView;			}
	CVectorPontos UpVector() {	return m_vUpVector;		}
	CVectorPontos Strafe()	{	return m_vStrafe;		}
	
	// Muda a posi��o, vista, e o vetor da c�mera.
	// Inicializa��o das variaveis da camera
	void PositionCamera(float positionX, float positionY, float positionZ,
			 		    float viewX,     float viewY,     float viewZ,
						float upVectorX, float upVectorY, float upVectorZ);

	// Rodar vista da c�mara em torno da posi��o, dependendo dos valores recebidos
	void RotateView(float angle, float X, float Y, float Z);

	// Mover a vis�o da c�mera, pelos movimentos do rato
	void SetViewByMouse(); 

	// Este gira a c�mera em torno de um ponto
	void RotateAroundPoint(CVectorPontos vCenter, float X, float Y, float Z);

	// Este strafes a esquerda ou para a direita da c�mara, dependendo da velocidade (+ / -)
	void StrafeCamera(float speed);

	// Para mover a c�mara para a frente ou para tr�s, dependendo da velocidade
	void MoveCamera(float speed);

	// Para verificar o movimento do teclado
	void CheckForMovement();

	// Isto atualiza de vis�o da c�mera e outros dados (Deve ser chamada a cada frame)
	void Update();

	// Usa gluLookAt () para dizer OpenGL para onde olhar
	void Look();

private:

	// A posi��o da c�mera
	CVectorPontos m_vPosition;					

	// A vis�o da c�mera
	CVectorPontos m_vView;						

	// Vector da c�mera
	CVectorPontos m_vUpVector;		
	
	// Vector da c�mera strafe - geralmente movimento directamente para a esquerda ou direita, em vez de rodar a c�mara
	CVectorPontos m_vStrafe;						
};

#endif
