
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
    static float lastTime			= 0.0f;		// Guarda a diferença entre a frame actual e a anterior
//	static char strFrameRate[50] = {0};			// We will store the string here for the window title

	static float frameTime = 0.0f;				// O ultimo tempo da frame

	// O tempo actual em segundos
    float currentTime = timeGetTime() * 0.001f;				

	// Guarda a diferença entre a frame actual e a anterior.
 	g_FrameInterval = currentTime - frameTime;
	frameTime = currentTime;

	// Aumenta o contador das frames
    ++framesPerSecond;
}


/////////////////////////////////////// Produto Vectorial \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////   Retorna o vetor perpendicular entre dois vectores, para tal deveremos realizar o produto vectorial
/////	
/////   O produto vetorial pode também ser utilizado para calcular a normal de um triângulo 
/////   ou outro polígono, o que é importante no ramo da computação gráfica e do desenvolvimento de jogos eletrônicos
/////   para permitir efeitos que simulam iluminação, dentre outros.
/////////////////////////////////////// Produto Vectorial \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
												
CVectorPontos Cross(CVectorPontos vVector1, CVectorPontos vVector2)
{
	CVectorPontos vNormal;	

	// Calcula o produto vectorial com a equaçao nao comutativa
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
	// Equação para calcular o tamanho do vector = raiz (V.x^2 + V.y^2 + V.z^2) : V é o vector
	return (float)sqrt( (vNormal.x * vNormal.x) + 
						(vNormal.y * vNormal.y) + 
						(vNormal.z * vNormal.z) );
}


/////////////////////////////////////// Vetor unitário \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	retornamos um vector unitário com exactamente uma unidade
/////
/////////////////////////////////////// Vetor unitário \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

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
	CVectorPontos vZero = CVectorPontos(0.0, 0.0, 0.0);		// Inicia um vector com 0 0 0 para a nossa posição inicial
	CVectorPontos vView = CVectorPontos(0.0, 1.0, 0.5);		// Cria o vector para a camera (para olhar para cima e para fora do ecra)
	CVectorPontos vUp   = CVectorPontos(0.0, 0.0, 1.0);		// Inicia um vector para cima (nunca se altera o valor)

	m_vPosition	= vZero;					// Inciar a posição inicial = 0
	m_vView		= vView;					// Inciar o Campo de visão inicial
	m_vUpVector	= vUp;						// Iniciar o vector
}


///////////////////////////////// Posição da Camara\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////	
/////   Esta funcao define a posição da camara, a vista e o vector up
/////
///////////////////////////////// Posição da Camara \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CCamera::PositionCamera(float positionX, float positionY, float positionZ,
				  		     float viewX,     float viewY,     float viewZ,
							 float upVectorX, float upVectorY, float upVectorZ)
{
	CVectorPontos vPosition	= CVectorPontos(positionX, positionY, positionZ);
	CVectorPontos vView		= CVectorPontos(viewX, viewY, viewZ);
	CVectorPontos vUpVector	= CVectorPontos(upVectorX, upVectorY, upVectorZ);


	m_vPosition = vPosition;					// Atribuir a posição
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
	int middleX = SCREEN_WIDTH  >> 1;				// Deslocação binaria para obter metade da largura
	int middleY = SCREEN_HEIGHT >> 1;				// Deslocação binaria para obter metade da altura
	float angleY = 0.0f;							// Direção para olhar para cima e para baixo
	float angleZ = 0.0f;							// Este será o valor que precisamos para girar em torno do eixo Y (Esquerda e Direita)
	static float currentRotX = 0.0f;
	
	// Obter a posição X e Y atual do rato
	GetCursorPos(&mousePos);						
	
	// Se o nosso cursor ainda está no meio, não houve movimento logo não iremos actualizar o ecrã
	if( (mousePos.x == middleX) && (mousePos.y == middleY) ) return;

	// Defina a posição do rato para o meio da nossa janela
	SetCursorPos(middleX, middleY);							

	// Obtemos a direção para a qual o rato se moveu mas incrementamos o numero de uma forma razoavel
	angleY = (float)( (middleX - mousePos.x) ) / 500.0f;		
	angleZ = (float)( (middleY - mousePos.y) ) / 500.0f;		

	static float lastRotX = 0.0f; 
 	lastRotX = currentRotX; // Guardamos o valor actual da rotação actual no eixo do X para utilizarmos quando o angulo for capturado
	
	// Aqui iremos controlar a rotação actual (para cima e para baixo) para que
	// possamos impedir a camara de fazer um loop de 360 graus.
	currentRotX += angleZ;
 
	// Se a rotação em radianos é maior do que 1.0, entao devemos manter sempre a 1.0 (valor max.).
	if(currentRotX > 1.0f)     
	{
		currentRotX = 1.0f;
		
		// Se existir, rodar o angulo remanescente
		if(lastRotX != 1.0f) 
		{
		
			//Para encontrar o eixo precisamos girar para cima e para baixo
			// precisamos obter um vector perpendicular a partir da
			// do vector da camara e o vector para cima. Este será o nosso eixo.
			// Antes de utilizar o eixo, devemos normaliza-lo primeiro.
			CVectorPontos vAxis = Cross(m_vView - m_vPosition, m_vUpVector);
			vAxis = Normalize(vAxis);
				
			// Rodar a câmera pelo ângulo restante (1.0f - lastRotX)
			RotateView( 1.0f - lastRotX, vAxis.x, vAxis.y, vAxis.z);
		}
	}
	// Verifique se a rotação for inferior a -1,0, em caso afirmativo queremos ter certeza de que não continue
	else if(currentRotX < -1.0f)
	{
		currentRotX = -1.0f;
		
		// Girar pelo ângulo remanescente se houver algum
		if(lastRotX != -1.0f)
		{
    		//Para encontrar o eixo precisamos girar para cima e para baixo
			// precisamos obter um vector perpendicular a partir da
			// do vector da camara e o vector para cima. Este será o nosso eixo.
			// Antes de utilizar o eixo, devemos normaliza-lo primeiro.
			CVectorPontos vAxis = Cross(m_vView - m_vPosition, m_vUpVector);
			vAxis = Normalize(vAxis);
			
			// girar a camara ( -1.0f - lastRotX)
			RotateView( -1.0f - lastRotX, vAxis.x, vAxis.y, vAxis.z);
		}
	}
	// Caso contrário, podemos girar a vista em torno de nossa posição
	else 
	{	
		//Para encontrar o eixo precisamos girar para cima e para baixo
		// precisamos obter um vector perpendicular a partir da
		// do vector da camara e o vector para cima. Este será o nosso eixo.
		// Antes de utilizar o eixo, devemos normaliza-lo primeiro.
		CVectorPontos vAxis = Cross(m_vView - m_vPosition, m_vUpVector);
		vAxis = Normalize(vAxis);
	
		// Girar em torno de nosso eixo perpendicular
		RotateView(angleZ, vAxis.x, vAxis.y, vAxis.z);
	}

	// Sempre girar a câmera à volta do eixo y
	RotateView(angleY, 0, 1, 0);
}


///////////////////////////////// Girar Camara \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	Rodar o ponto de vista em torno da posição usando um ângulo de rotação do eixo
/////
///////////////////////////////// Girar Camara \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CCamera::RotateView(float angle, float x, float y, float z)
{
	CVectorPontos vNewView;

	// Obter o vetor vista (A direção que está de frente para nós)
	CVectorPontos vView = m_vView - m_vPosition;		

	// Calcula o sin e cos do anglo uma vez
	float cosTheta = (float)cos(angle);
	float sinTheta = (float)sin(angle);

	// Encontrar a nova posição X para o novo ponto de rotação
	vNewView.x  = (cosTheta + (1 - cosTheta) * x * x)		* vView.x;
	vNewView.x += ((1 - cosTheta) * x * y - z * sinTheta)	* vView.y;
	vNewView.x += ((1 - cosTheta) * x * z + y * sinTheta)	* vView.z;

	// Encontrar a nova posição Y para o novo ponto de rotação
	vNewView.y  = ((1 - cosTheta) * x * y + z * sinTheta)	* vView.x;
	vNewView.y += (cosTheta + (1 - cosTheta) * y * y)		* vView.y;
	vNewView.y += ((1 - cosTheta) * y * z - x * sinTheta)	* vView.z;

    //Encontrar a nova posição Z para o novo ponto de rotação
	vNewView.z  = ((1 - cosTheta) * x * z - y * sinTheta)	* vView.x;
	vNewView.z += ((1 - cosTheta) * y * z + x * sinTheta)	* vView.y;
	vNewView.z += (cosTheta + (1 - cosTheta) * z * z)		* vView.z;

    
	//define a nova posiçao da camara
	m_vView = m_vPosition + vNewView;
}


///////////////////////////////// STRAFE CAMERA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	Move a camera para a esquerda e para a direita sefundo a velocidade (+/-)
/////
///////////////////////////////// STRAFE CAMERA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CCamera::StrafeCamera(float speed)
{	
	// Adiciona ao vector strafe a nossa posiçao
	m_vPosition.x += m_vStrafe.x * speed;
	m_vPosition.z += m_vStrafe.z * speed;

	// Adiciona ao vector strafe à nossa visão/camera
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
	// Vamos buscar a direção para onde estamos a olhar
	CVectorPontos vVector = m_vView - m_vPosition;
	vVector = Normalize(vVector);

	m_vPosition.x += vVector.x * speed;		// Adiciona a nossa aceleração à nossa posição X
	m_vPosition.y += vVector.y * speed;		// Adiciona a nossa aceleração à nossa posição Y
	m_vPosition.z += vVector.z * speed;		// Adiciona a nossa aceleração à nossa posição Z
	m_vView.x += vVector.x * speed;			// Adiciona a nossa aceleração à nossa camera X
	m_vView.y += vVector.y * speed;			// Adiciona a nossa aceleração à nossa camera Y
	m_vView.z += vVector.z * speed;			// Adiciona a nossa aceleração à nossa camera Z
}


//////////////////////////// Procura movimentos \\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	Esta função trata o input com mais rapidez do que o WinProc()
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


///////////////////////////////// Actualização \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////   Actualiza a vista da camara e o vector strafe
/////
///////////////////////////////// UPDATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CCamera::Update() 
{
	// Inicializa a variavel para o resultado do produto vectorial
	CVectorPontos vCross = Cross(m_vView - m_vPosition, m_vUpVector);

	// Normalização do vector strafe
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
/////	Actualiza a camara de acordo com a posiçao
/////
///////////////////////////////// LOOK \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CCamera::Look()
{
	// Damos ao opengl a posiçao da camara, a visao da camara e o vector up da camara
	gluLookAt(m_vPosition.x, m_vPosition.y, m_vPosition.z,	
			  m_vView.x,	 m_vView.y,     m_vView.z,	
			  m_vUpVector.x, m_vUpVector.y, m_vUpVector.z);
}

