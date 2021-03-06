// Autor:     Antonio Carrillo Ledesma.
// R.F.C.:    CAAN-691229-TV7
// Direcci�n: Amsterdam 312 col. Hip�dromo Condesa
// Tel�fono:  5-74-43-53

// Propiedad intelectual, todos los derechos reservados conforme a la ley, registro en tr�mite 1999-2000
// Revisi�n  1.1-A


#include "DefinicionEcuacion.hpp"

// Constructor de la clase
DefinicionEcuacion::DefinicionEcuacion(void)
{
   // Nombre del sistema   
#ifdef TETRAESTABLE   
   Nombre_sistema       = "Tetraestable";
   // N�mero de par�metros
   Numero_parametros          = 3;
   Numero_parametros_bifurcar = 3;
   // Nombre de los par�metros (si el nombre de el par�metro excede de 6 letras
   // hay que indicar el tama�o maximo de este en captura de par�metros en cada escenario)
   Nombre_parametros[0] = "a";
   Nombre_parametros[1] = "b";
   Nombre_parametros[2] = "c";
   
   // Valores por omisi�n de los parametros
   P[0] = 0.5;
   P[1] = 0.45;
   P[2] = 0.5;
   // Ecuaci�n en formato texto
   Ecuacion_texto = "x = x + a + b * sin(2.0 * M_PI * x) + c * sin(4.0 * M_PI * x)";
#endif
#ifdef ANA
   Nombre_sistema       = "ANA";
   // N�mero de par�metros
   Numero_parametros          = 3;
   Numero_parametros_bifurcar = 3;
   // Nombre de los par�metros (si el nombre de el par�metro excede de 6 letras
   // hay que indicar el tama�o maximo de este en captura de par�metros en cada escenario)
   Nombre_parametros[0] = "a";
   Nombre_parametros[1] = "b";
   Nombre_parametros[2] = "c";
   
   // Valores por omisi�n de los parametros
   P[0] = 0.5;
   P[1] = 0.45;
   P[2] = 0.45;
   // Ecuaci�n en formato texto
   Ecuacion_texto = "x = x + a + b * sin(2.0 * M_PI * x)";
#endif
#ifdef ARNOLD
   Nombre_sistema       = "Arnold's Classical Maps of Circunference";
   // N�mero de par�metros
   Numero_parametros          = 2;
   Numero_parametros_bifurcar = 2;
   // Nombre de los par�metros (si el nombre de el par�metro excede de 6 letras
   // hay que indicar el tama�o maximo de este en captura de par�metros en cada escenario)
   Nombre_parametros[0] = "a";
   Nombre_parametros[1] = "b";
   
   // Valores por omisi�n de los parametros
   P[0] = 0.5;
   P[1] = 0.45;
   // Ecuaci�n en formato texto
   Ecuacion_texto = "x = x + a + b * sin(2.0 * M_PI * x)";
#endif
#ifdef NM_FISICA
   Nombre_sistema       = "Neurona Mec�nica F�sica";
   // N�mero de par�metros
   Numero_parametros          = 8;
   Numero_parametros_bifurcar = 2;
   // Nombre de los par�metros (si el nombre de el par�metro excede de 6 letras
   // hay que indicar el tama�o maximo de este en captura de par�metros en cada escenario)
   Nombre_parametros[0] = "M";
   Nombre_parametros[1] = "A";
   Nombre_parametros[2] = "l";
   Nombre_parametros[3] = "h";
   Nombre_parametros[4] = "I";
   Nombre_parametros[5] = "v";
   Nombre_parametros[6] = "alfa";
   Nombre_parametros[7] = "d";
   
   // Valores por omisi�n de los parametros
   P[0] = 9.0;
   P[1] = 18.0;
   P[2] = 6.0;
   P[3] = 0.5;
   P[4] = 0.75;
   P[5] = 2.0 * M_PI;
   P[6] = 0.96;
   P[7] = 85.0;
   // Ecuaci�n en formato texto
   Ecuacion_texto = "x = ...";
#endif
}



// Asigna valor a los par�metros
void DefinicionEcuacion::AsignaParametros(const long double parm[NUM_MAX_PARAMETROS])
{
   for (int i = 0; i < Numero_parametros; i++) P[i] = parm[i];
}

// Retorna el valor de los par�metros
void DefinicionEcuacion::RetornaParametros(long double *parm)
{
   for (int i = 0; i < Numero_parametros; i++) parm[i] = P[i];
#ifdef BOYLAND   
   Preparada=-1;
#endif   
}


