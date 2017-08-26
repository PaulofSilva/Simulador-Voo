
#pragma comment(lib, "winmm.lib")

#include "main.h"
#include "Camera.h"



// Velocidade do movimento da camera / aviao
#define kSpeed	100.0f									


//APAGAR variavel global que guarda o tempo entre anterior e a actual
float g_FrameInterval = 0.0f;


///////////////////////////////// CALCULAR AS FRAMES POR SEGUNDO \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	Esta funcao calcula o numero de frames e o intervalo entre elas
/////   
/////
///////////////////////////////// Calcular o numero de frames \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*



void CalculateFrameRate()
{
	static float framesPerSecond    = 0.0f;		// Guarda o numero de Frames por segundo
    static float lastTime			= 0.0f;		// Guarda a diferen�a entre a frame actual e a anterior
//	static char strFrameRate[50] = {0};			// We will store the string here for the window title

	static float frameTime = 0.0f;				// O ultimo tempo da frame

	// O tempo actual em segundos
    float currentTime = timeGetTime() * 0.001f;				

	// Guarda a diferen�a entre a frame actual e a anterior.
 	g_FrameInterval = currentTime - frameTime;
	frameTime = currentTime;

	// Aumenta o contador das frames
    ++framesPerSecond;
}


/////////////////////////////////////// Produto Vectorial \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////   Retorna o vetor perpendicular entre dois vectores, para tal deveremos realizar o produto vectorial
/////	
/////   O produto vetorial pode tamb�m ser utilizado para calcular a normal de um tri�ngulo 
/////   ou outro pol�gono, o que � importante no ramo da computa��o gr�fica e do desenvolvimento de jogos eletr�nicos
/////   para permitir efeitos que simulam ilumina��o, dentre outros.
/////////////////////////////////////// Produto Vectorial \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
												
CVectorPontos Cross(CVectorPontos vVector1, CVectorPontos vVector2)
{
	CVectorPontos vNormal;	

	// Calcula o produto vectorial com a equa�ao nao comutativa
	vNormal.x = ((vVector1.y * vVector2.z) - (vVector1.z * vVector2.y));
	vNormal.y = ((vVector1.z * vVector2.x) - (vVector1.x * vVector2.z));
	vNormal.z = ((vVector1.x * vVector2.y) - (vVector1.y * vVector2.x));

	// Retorna o produto vectorial
	return vNormal;										 
}


/////////////////////////////////////// Tamanho \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns the magnitude of a vector
/////
/////////////////////////////////////// Tamanho \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

float Magnitude(CVectorPontos vNormal)
{
	// Equa��o para calcular o tamanho do vector = raiz (V.x^2 + V.y^2 + V.z^2) : V � o vector
	return (float)sqrt( (vNormal.x * vNormal.x) + 
						(vNormal.y * vNormal.y) + 
						(vNormal.z * vNormal.z) );
}


/////////////////////////////////////// Vetor unit�rio \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	retornamos um vector unit�rio com exactamente uma unidade
/////
/////////////////////////////////////// Vetor unit�rio \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CVectorPontos Normalize(CVectorPontos vVector)
{
	// Tamanho do vector
	float magnitude = Magnitude(vVector);				


    //Dividimos o nosso vector pelo seu tamanho e assim conseguimos obter um vector de comprimento 1 
	vVector = vVector / magnitude;		
	
	// Retornamos o nosso vector normalizado
	return vVector;										
}


///////////////////////////////// CCAMERA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	Construtor da classe camara
/////
///////////////////////////////// CCAMERA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CCamera::CCamera()
{
	CVectorPontos vZero = CVectorPontos(0.0, 0.0, 0.0);		// Inicia um vector com 0 0 0 para a nossa posi��o inicial
	CVectorPontos vView = CVectorPontos(0.0, 1.0, 0.5);		// Cria o vector para a camera (para olhar para cima e para fora do ecra)
	CVectorPontos vUp   = CVectorPontos(0.0, 0.0, 1.0);		// Inicia um vector para cima (nunca se altera o valor)

	m_vPosition	= vZero;					// Inciar a posi��o inicial = 0
	m_vView		= vView;					// Inciar o Campo de vis�o inicial
	m_vUpVector	= vUp;						// Iniciar o vector
}


///////////////////////////////// Posi��o da Camara\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////	
/////   Esta funcao define a posi��o da camara, a vista e o vector up
/////
///////////////////////////////// Posi��o da Camara \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CCamera::PositionCamera(float positionX, float positionY, float positionZ,
				  		     float viewX,     float viewY,     float viewZ,
							 float upVectorX, float upVectorY, float upVectorZ)
{
	CVectorPontos vPosition	= CVectorPontos(positionX, positionY, positionZ);
	CVectorPontos vView		= CVectorPontos(viewX, viewY, viewZ);
	CVectorPontos vUpVector	= CVectorPontos(upVectorX, upVectorY, upVectorZ);


	m_vPosition = vPosition;					// Atribuir a posi��o
	m_vView     = vView;						// Atribuir a vista
	m_vUpVector = vUpVector;					// Atribuir o vector up
}


///////////////////////////////// Define a vista com o rato \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	Permite movimentar a camara com o rato
/////
///////////////////////////////// Define a vista com o rato \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CCamera::SetViewByMouse()
{
	POINT mousePos;									// Estrutura que guarda o X e Y
	int middleX = SCREEN_WIDTH  >> 1;				// Desloca��o binaria para obter metade da largura
	int middleY = SCREEN_HEIGHT >> 1;				// Desloca��o binaria para obter metade da altura
	float angleY = 0.0f;							// Dire��o para olhar para cima e para baixo
	float angleZ = 0.0f;							// Este ser� o valor que precisamos para girar em torno do eixo Y (Esquerda e Direita)
	static float currentRotX = 0.0f;
	
	// Obter a posi��o X e Y atual do rato
	GetCursorPos(&mousePos);						
	
	// Se o nosso cursor ainda est� no meio, n�o houve movimento logo n�o iremos actualizar o ecr�
	if( (mousePos.x == middleX) && (mousePos.y == middleY) ) return;

	// Defina a posi��o do rato para o meio da nossa janela
	SetCursorPos(middleX, middleY);							

	// Obtemos a dire��o para a qual o rato se moveu mas incrementamos o numero de uma forma razoavel
	angleY = (float)( (middleX - mousePos.x) ) / 500.0f;		
	angleZ = (float)( (middleY - mousePos.y) ) / 500.0f;		

	static float lastRotX = 0.0f; 
 	lastRotX = currentRotX; // Guardamos o valor actual da rota��o actual no eixo do X para utilizarmos quando o angulo for capturado
	
	// Aqui iremos controlar a rota��o actual (para cima e para baixo) para que
	// possamos impedir a camara de fazer um loop de 360 graus.
	currentRotX += angleZ;
 
	// Se a rota��o em radianos � maior do que 1.0, entao devemos manter sempre a 1.0 (valor max.).
	if(currentRotX > 1.0f)     
	{
		currentRotX = 1.0f;
		
		// Se existir, rodar o angulo remanescente
		if(lastRotX != 1.0f) 
		{
		
			//Para encontrar o eixo precisamos girar para cima e para baixo
			// precisamos obter um vector perpendicular a partir da
			// do vector da camara e o vector para cima. Este ser� o nosso eixo.
			// Antes de utilizar o eixo, devemos normaliza-lo primeiro.
			CVectorPontos vAxis = Cross(m_vView - m_vPosition, m_vUpVector);
			vAxis = Normalize(vAxis);
				
			// Rodar a c�mera pelo �ngulo restante (1.0f - lastRotX)
			RotateView( 1.0f - lastRotX, vAxis.x, vAxis.y, vAxis.z);
		}
	}
	// Verifique se a rota��o for inferior a -1,0, em caso afirmativo queremos ter certeza de que n�o continue
	else if(currentRotX < -1.0f)
	{
		currentRotX = -1.0f;
		
		// Girar pelo �ngulo remanescente se houver algum
		if(lastRotX != -1.0f)
		{
    		//Para encontrar o eixo precisamos girar para cima e para baixo
			// precisamos obter um vector perpendicular a partir da
			// do vector da camara e o vector para cima. Este ser� o nosso eixo.
			// Antes de utilizar o eixo, devemos normaliza-lo primeiro.
			CVectorPontos vAxis = Cross(m_vView - m_vPosition, m_vUpVector);
			vAxis = Normalize(vAxis);
			
			// girar a camara ( -1.0f - lastRotX)
			RotateView( -1.0f - lastRotX, vAxis.x, vAxis.y, vAxis.z);
		}
	}
	// Caso contr�rio, podemos girar a vista em torno de nossa posi��o
	else 
	{	
		//Para encontrar o eixo precisamos girar para cima e para baixo
		// precisamos obter um vector perpendicular a partir da
		// do vector da camara e o vector para cima. Este ser� o nosso eixo.
		// Antes de utilizar o eixo, devemos normaliza-lo primeiro.
		CVectorPontos vAxis = Cross(m_vView - m_vPosition, m_vUpVector);
		vAxis = Normalize(vAxis);
	
		// Girar em torno de nosso eixo perpendicular
		RotateView(angleZ, vAxis.x, vAxis.y, vAxis.z);
	}

	// Sempre girar a c�mera � volta do eixo y
	RotateView(angleY, 0, 1, 0);
}


///////////////////////////////// Girar Camara \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	Rodar o ponto de vista em torno da posi��o usando um �ngulo de rota��o do eixo
/////
///////////////////////////////// Girar Camara \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CCamera::RotateView(float angle, float x, float y, float z)
{
	CVectorPontos vNewView;

	// Obter o vetor vista (A dire��o que est� de frente para n�s)
	CVectorPontos vView = m_vView - m_vPosition;		

	// Calcula o sin e cos do anglo uma vez
	float cosTheta = (float)cos(angle);
	float sinTheta = (float)sin(angle);

	// Encontrar a nova posi��o X para o novo ponto de rota��o
	vNewView.x  = (cosTheta + (1 - cosTheta) * x * x)		* vView.x;
	vNewView.x += ((1 - cosTheta) * x * y - z * sinTheta)	* vView.y;
	vNewView.x += ((1 - cosTheta) * x * z + y * sinTheta)	* vView.z;

	// Encontrar a nova posi��o Y para o novo ponto de rota��o
	vNewView.y  = ((1 - cosTheta) * x * y + z * sinTheta)	* vView.x;
	vNewView.y += (cosTheta + (1 - cosTheta) * y * y)		* vView.y;
	vNewView.y += ((1 - cosTheta) * y * z - x * sinTheta)	* vView.z;

    //Encontrar a nova posi��o Z para o novo ponto de rota��o
	vNewView.z  = ((1 - cosTheta) * x * z - y * sinTheta)	* vView.x;
	vNewView.z += ((1 - cosTheta) * y * z + x * sinTheta)	* vView.y;
	vNewView.z += (cosTheta + (1 - cosTheta) * z * z)		* vView.z;

    
	//define a nova posi�ao da camara
	m_vView = m_vPosition + vNewView;
}


///////////////////////////////// STRAFE CAMERA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	Move a camera para a esquerda e para a direita sefundo a velocidade (+/-)
/////
///////////////////////////////// STRAFE CAMERA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CCamera::StrafeCamera(float speed)
{	
	// Adiciona ao vector strafe a nossa posi�ao
	m_vPosition.x += m_vStrafe.x * speed;
	m_vPosition.z += m_vStrafe.z * speed;

	// Adiciona ao vector strafe � nossa vis�o/camera
	m_vView.x += m_vStrafe.x * speed;
	m_vView.z += m_vStrafe.z * speed;
}


///////////////////////////////// Mover a Camera \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	Iremos mover a camera para a frente e para tras dependendo da velocidade
/////
///////////////////////////////// Mover a Camera \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CCamera::MoveCamera(float speed)
{
	// Vamos buscar a dire��o para onde estamos a olhar
	CVectorPontos vVector = m_vView - m_vPosition;
	vVector = Normalize(vVector);

	m_vPosition.x += vVector.x * speed;		// Adiciona a nossa acelera��o � nossa posi��o X
	m_vPosition.y += vVector.y * speed;		// Adiciona a nossa acelera��o � nossa posi��o Y
	m_vPosition.z += vVector.z * speed;		// Adiciona a nossa acelera��o � nossa posi��o Z
	m_vView.x += vVector.x * speed;			// Adiciona a nossa acelera��o � nossa camera X
	m_vView.y += vVector.y * speed;			// Adiciona a nossa acelera��o � nossa camera Y
	m_vView.z += vVector.z * speed;			// Adiciona a nossa acelera��o � nossa camera Z
}


//////////////////////////// Procura movimentos \\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	Esta fun��o trata o input com mais rapidez do que o WinProc()
/////
//////////////////////////// Procura movimentos \\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CCamera::CheckForMovement()
{	
	// Assim que tenhamos o intervalo das frames, podemos encontrar a velocidade actual
	float speed = kSpeed * g_FrameInterval;

	// Verifica se foi primida a tecla "UP" ou "W"
	if(GetKeyState(VK_UP) & 0x80 || GetKeyState('W') & 0x80) {				

		// Move a nossa camera para a frente com uma velocidade positiva
		MoveCamera(speed);				
	}

	// Verifica se foi primida a tecla "Down" ou "S"
	if(GetKeyState(VK_DOWN) & 0x80 || GetKeyState('S') & 0x80) {			

		// Move a nossa camera para a frente com uma velocidade negativa
		MoveCamera(-speed);				
	}

	// Verifica se foi primida a tecla "Esquerda" ou "A"
	if(GetKeyState(VK_LEFT) & 0x80 || GetKeyState('A') & 0x80) {			

		// Move a camera para a esquerda
		StrafeCamera(-speed);
	}

	// Verifica se foi primida a tecla "Direita" ou "D"
	if(GetKeyState(VK_RIGHT) & 0x80 || GetKeyState('D') & 0x80) {			

		// Move a camera para a direita
		StrafeCamera(speed);
	}	
}


///////////////////////////////// Actualiza��o \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////   Actualiza a vista da camara e o vector strafe
/////
///////////////////////////////// UPDATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CCamera::Update() 
{
	// Inicializa a variavel para o resultado do produto vectorial
	CVectorPontos vCross = Cross(m_vView - m_vPosition, m_vUpVector);

	// Normaliza��o do vector strafe
	m_vStrafe = Normalize(vCross);

	// Move a camara segundo o rato
	SetViewByMouse();
	MoveCamera(1);	

	// Verifica se o teclado foi pressionado
	CheckForMovement();
	
	// Calcula a frame rate e define o intervalo segundo o movimento
	CalculateFrameRate();
}


///////////////////////////////// LOOK \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	Actualiza a camara de acordo com a posi�ao
/////
///////////////////////////////// LOOK \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CCamera::Look()
{
	// Damos ao opengl a posi�ao da camara, a visao da camara e o vector up da camara
	gluLookAt(m_vPosition.x, m_vPosition.y, m_vPosition.z,	
			  m_vView.x,	 m_vView.y,     m_vView.z,	
			  m_vUpVector.x, m_vUpVector.y, m_vUpVector.z);
}

