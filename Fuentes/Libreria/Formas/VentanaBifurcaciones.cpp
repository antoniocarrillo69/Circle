//////////////////////////////////////////////////////////////////////////////////////////////
// CIRCULO WINDOWS C++ 1.0                                                                  //
//                                                                                          //
// An�lisis y Dise�o:                                                                       //
//                                                                                          //
// Humberto Andres Carrillo Calvet                                                          //
// Antonio Carrillo Ledesma                                                                 //
// Oscar Garc�a Regis                                                                       //
// Miguel Angel Mendoza Reyes                                                               //
// Fernando Alberto Ongay Larios                                                            //
//                                                                                          //
// Programaci�n:                                                                            //
//                                                                                          //
// Humberto Andres Carrillo Calvet                                                          //
// Antonio Carrillo Ledesma                                                                 //
//                                                                                          //
//////////////////////////////////////////////////////////////////////////////////////////////


#include <vcl.h>
#include <Printers.hpp>
#include <stdio.h>
#include <stdlib.h>
#include "Acecade.h"
#include "Ayuda.h"
#include "Sincronizaciones.hpp"
#include "NumeroRotacion.hpp"
#include "ExponenteLyapunov.hpp"
#include "LMensaje.h"

#include "VentanaCirculo.h"
#include "VentanaToro.h"
#pragma hdrstop

#include "VentanaBifurcaciones.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
// Objetosglobales del sistema IyD
extern  Sincronizaciones       sincro;
extern  NumeroRotacion         num_rot;
extern  ExponenteLyapunov      exp_lyap;

// Variables globales de control de escenarios
extern  bool                   Limpiar_ventana;
extern  void                  *Ventana_activa;
extern  bool                   Calcular_sincronizaciones;
extern  bool                   Calcular_numero_rotacion;
extern  bool                   Calcular_exponente_lyapunov;  
extern  bool                   Configura_escenario;
extern  bool                   Calcula_escenario;
extern  bool                   Detener_Calculo;
extern  bool                   Zoom_manteniendo_aspecto;
extern  char                  *COMPANIA;
extern  char                  *E_MAIL;


const char *VG_TXT01   = "Arrastre el mouse para seleccionar el �rea";

// Constructor de la clase
__fastcall TFormaBifurcaciones::TFormaBifurcaciones(TComponent* Owner) : TForm(Owner)
{
   // Controla si esta activo algun calculo
   Sw_Proceso_calculo = false;
   Sw_cerrar_ventana = false;
   // Inicializa el indicador de recuadro
   Sw_Dibuja_rectangulo = false;
   Ventana_seleccionada = false;
   Sw_Recuadro_activo = true;

   // Dimenciones de la ventana
   Dim_Vtn.Xi = 0.0;
   Dim_Vtn.Xf = 1.0;
   Dim_Vtn.Yi = 0.0;
   Dim_Vtn.Yf = 1.0;

   Dim_Vtn2.Xi = 0.0;
   Dim_Vtn2.Xf = 1.0;
   Dim_Vtn2.Yi = -1.0;
   Dim_Vtn2.Yf = 1.0;

   // Dimensiones originales
   Dim_orig = Dim_Vtn;
   // Indice de zooms
   Ind_zoom = 0;
   
   Pix_x = 1024;
   Pix_y = 768; 

   // Color de los graficos
   Color_grafica[0] = clBlack;    // Grafico
   Color_grafica[1] = clYellow;   // Linea del Grafico
   Color_grafica[2] = clWhite;  // Referencia
   Color_fondo = clSilver;

   // Controla la visualizaci�n de ...
   Sw_Visualiza_bifurcaciones = true;
   Sw_Visualiza_exponente = true;
   Sw_Visualiza_referencia = true;

   // Puntos a trazar en la curva
   Puntos_calcular = 5000;  
   Puntos_trazar = 3000;   
   Condiciones_iniciales = 1;
   Continuar_ultima_cond_inicial = false;

   // Condicion inicial
   Condicion_inicial = 0.0;
   Sw_Cond_inicial_aleatoria = true;
 

   Tipo_integracion = ADELANTE;

   Parametro_bifurcar = 0;

   
   pix[0].Inicializa(Pix_x,Pix_y, false, Dim_Vtn);
   pix[1].Inicializa(Pix_x,Pix_y, false, Dim_Vtn2);
}

// Revisa cada segundo si se ha solicitado el cierre de la ventana
void __fastcall TFormaBifurcaciones::Timer1Timer(TObject *Sender)
{
   // Almacena la ventana activa
   if (Focused()) {
      Ventana_activa = this;
   }   
   // Revisa si es la ventana activa 
   if (Ventana_activa == this) {
      // Calcula el n�mero de rotacion
      if (Calcular_numero_rotacion) {
          Calcular_numero_rotacion = false;
          MenuCalcularNumRotacionClick(this);
      }
      // Calcula la sincronizaci�n
      if (Calcular_sincronizaciones) {
          Calcular_sincronizaciones = false;
          MenuCalcularSincronizacionClick(this);
      }
      // Calcula el exponente de Luapunov
      if (Calcular_exponente_lyapunov) {
          Calcular_exponente_lyapunov = false;
          MenuCalculaeExpLyapunovClick(this);
      }
      // Configura el escenario
      if (Configura_escenario) {
          Configura_escenario = false;
          MenuConfigurarParametrosClick(this);
      }
      // Calcula
      if (Calcula_escenario) {
          Calcula_escenario = false;
          MenuCalculaBifurcacionesClick(this);
      }
      // Detener c�lculo
      if (Detener_Calculo) {
          Detener_Calculo = false;
          MenuCalcularDetenerelcalculoClick(this);
      }
      // Limpia la ventana
      if (Limpiar_ventana) {
          Limpiar_ventana = false;
          MenuVentanaLimpiarClick(this);
      }
   }
   // Controla que no este calculando para cerrar la ventana
   if (Sw_cerrar_ventana && !Sw_Proceso_calculo) {
      Ventana_activa = NULL;
      Free();
   }
}


// Al cerrar la forma ...// Al solicitar cerrar la ventana ...
void __fastcall TFormaBifurcaciones::FormCloseQuery(TObject *Sender, bool &CanClose)
{
   CanClose = false;
   if (MessageBox(Handle,"Do you wish to close this window?","Bifurcations scenery",MB_YESNO + MB_ICONQUESTION)  == IDYES) {
       Sw_cancela_calculo = true;
       Sw_cerrar_ventana = true;
   }
}
// Al cambiar de tama�o la forma
void __fastcall TFormaBifurcaciones::FormResize(TObject *Sender)
{
   // Calcula la escala de la ventana de visualizacion
   Escala1.X = (VTBifurcaciones->Width -1)/  (Dim_Vtn.Xf - Dim_Vtn.Xi);
   Escala1.Y = (VTBifurcaciones->Height -1) /  (Dim_Vtn.Yf - Dim_Vtn.Yi);
   Escala2.X = (VTExponenteLyapunov->Width -1)/  (Dim_Vtn2.Xf - Dim_Vtn2.Xi);
   Escala2.Y = (VTExponenteLyapunov->Height -1) / (Dim_Vtn2.Yf - Dim_Vtn2.Yi);
   // Grafica el contenido de la ventana
   Grafica();        
}


///////////////////////////////////////////////////////////////////////////////
//  Definici�n de Comportamientos del Menu 
///////////////////////////////////////////////////////////////////////////////

// Menu->Archivo->Cerrar
void __fastcall TFormaBifurcaciones::MenuArchivoCerrarClick(TObject *Sender)
{
   Close();     
}

// Menu->Archivo->Grabar BMP
void __fastcall TFormaBifurcaciones::MenuArchivoGrabarBMPClick(TObject *Sender)
{
   TRect   xRect = Rect(0,0,ClientWidth,ClientHeight-20);
   Graphics::TBitmap *Bitmap = new Graphics::TBitmap;
   Bitmap->Width = ClientWidth;
   Bitmap->Height = ClientHeight-20;
   Bitmap->Canvas->CopyRect(xRect, Canvas, xRect);
   SaveDialog->Title = "Save as ...";
   if (SaveDialog->Execute()) {
      Bitmap->SaveToFile(SaveDialog->FileName);           
   }
   delete Bitmap;
}

// Menu->Archivo->Imprimir
void __fastcall TFormaBifurcaciones::MenuArchivoImprimirClick(TObject *Sender)
{
   Sw_cancela_calculo  = true;
   TRect   xRect = Rect(0,0,ClientWidth,ClientHeight);
   Graphics::TBitmap *Bitmap = new Graphics::TBitmap;
   Bitmap->Width = ClientWidth;
   Bitmap->Height = ClientHeight;
   Bitmap->Canvas->CopyRect(xRect, Canvas, xRect);
   Bitmap->SaveToFile("Prn_img.bmp");
   delete Bitmap;
   Bitmap = new Graphics::TBitmap;
   Bitmap->LoadFromFile("Prn_img.bmp");
   Printer()->BeginDoc();
   Printer()->Canvas->TextOut(100,10,Application->Title);
   Printer()->Canvas->TextOut(100,100,Caption);
   Printer()->Canvas->StretchDraw(Rect(100,300,Printer()->PageWidth-100,Printer()->PageWidth-200),Bitmap);
   Printer()->Canvas->TextOut(100,(Printer()->PageWidth - 100),COMPANIA);
   Printer()->Canvas->TextOut(100,(Printer()->PageWidth - 10),E_MAIL);
   Printer()->EndDoc();
   delete Bitmap;
   unlink("Prn_img.bmp");
}




// Menu->Calcular->Bifurcaciones
void __fastcall TFormaBifurcaciones::MenuCalculaBifurcacionesClick(TObject *Sender)
{
   Calcula_bifurcaciones();
}

// Menu->Calcular->Numero de Rotaci�n
void __fastcall TFormaBifurcaciones::MenuCalcularNumRotacionClick(TObject *Sender)
{
   TCursor antcursor = Cursor;
   // Cambia el cursor del mouse
   Cursor = crHourGlass;
   long double p[NUM_MAX_PARAMETROS];
   BarraDeEstadosBifurcaciones->SimpleText = (AnsiString) "Calculating rotation number (this process may last several seconds).";
   // Recupera los par�metros por omisi�n
   fun_disp.RetornaParametros(p);
   // Cambiar par�metros
   num_rot.AsignaParametros(p);
   num_rot.Calcula(Tipo_integracion);
   TLMensajeForm *Ayuda = new TLMensajeForm(this);
   if(!Ayuda) return;
    Ayuda->Abrir_archivo("Rotational","C:\\REPTMP.TMP");
    Ayuda->ShowModal();

   Cursor = antcursor;
   BarraDeEstadosBifurcaciones->SimpleText = (AnsiString) " ";
}

// Menu->Calcular->Sincronizaci�n
void __fastcall TFormaBifurcaciones::MenuCalcularSincronizacionClick(TObject *Sender)
{
   TCursor antcursor = Cursor;
   // Cambia el cursor del mouse
   Cursor = crHourGlass;
   long double p[NUM_MAX_PARAMETROS];
   BarraDeEstadosBifurcaciones->SimpleText = (AnsiString) "Calculating synchronization (this process may last several seconds).";
   // Recupera los par�metros por omisi�n
   fun_disp.RetornaParametros(p);
   // Cambiar par�metros
   sincro.AsignaParametros(p);
   if (sincro.Calcula(Tipo_integracion)) {
      TLMensajeForm *Ayuda = new TLMensajeForm(this);
      if(!Ayuda) return;
      Ayuda->Abrir_archivo("Sincronizaciones","C:\\REPTMP.TMP");
      Ayuda->ShowModal();
   } else ShowMessage("No se encontro sincronizaciones");
   Cursor = antcursor;
   BarraDeEstadosBifurcaciones->SimpleText = (AnsiString) " ";
}

// Menu->Calcular->Exponente de Lyapunov
void __fastcall TFormaBifurcaciones::MenuCalculaeExpLyapunovClick(TObject *Sender)
{
   TCursor antcursor = Cursor;
   // Cambia el cursor del mouse
   Cursor = crHourGlass;
   long double p[NUM_MAX_PARAMETROS], rot;
   BarraDeEstadosBifurcaciones->SimpleText = (AnsiString) "Calculating Lyapunov exponent (this process may last several seconds).";
   // Recupera los par�metros por omisi�n
   fun_disp.RetornaParametros(p);
   // Cambiar par�metros
   exp_lyap.AsignaParametros(p);
   rot = exp_lyap.Calcula(Tipo_integracion);
   char xcad[200];
   sprintf(xcad,"Lyapunov exponent: %Lf",rot);
   MessageBox(NULL,xcad,"Bifurcations scenery",MB_OK+MB_ICONINFORMATION);
   BarraDeEstadosBifurcaciones->SimpleText = (AnsiString) " ";
   // Cambia el cursor del mouse
   Cursor = antcursor;
}

// Menu->Ventana->Detener el calculo
void __fastcall TFormaBifurcaciones::MenuCalcularDetenerelcalculoClick(TObject *Sender)
{
   Sw_cancela_calculo  = true;
}


// Menu->Ventana->Limpiar
void __fastcall TFormaBifurcaciones::MenuVentanaLimpiarClick(TObject *Sender)
{
   Limpia_matriz_pixeles();
   Grafica();
}

// Menu->Ventana->Zoom Out
void __fastcall TFormaBifurcaciones::MenuVentanaZoomOutClick(TObject *Sender)
{
   // Almacena las dimensiones actuales
   if (Ind_zoom > 0) {
      Ind_zoom --;
      Dim_Vtn = Dim_zoom[Ind_zoom];
      // Actualiza las dimensiones de las ventanas de trabajo
      Dim_Vtn2.Xi = Dim_Vtn.Xi;
      Dim_Vtn2.Xf = Dim_Vtn.Xf;
      pix[0].Cambia_dimension(Dim_Vtn);
      pix[1].Cambia_dimension(Dim_Vtn2);
      FormResize(this);
      Calcula_bifurcaciones();
      if (Ind_zoom < 1) MenuVentanaZoomOut->Enabled = false;
   }
}

// Menu->Ventana->Dimensiones originales
void __fastcall TFormaBifurcaciones::MenuVentanaDimensionesOriginalesClick(TObject *Sender)
{
   Dim_Vtn = Dim_orig;
   Ind_zoom = 0;
   MenuVentanaZoomOut->Enabled = false;
   // Actualiza las dimensiones de las ventanas de trabajo
   Dim_Vtn2.Xi = Dim_Vtn.Xi;
   Dim_Vtn2.Xf = Dim_Vtn.Xf;
   pix[0].Cambia_dimension(Dim_Vtn);
   pix[1].Cambia_dimension(Dim_Vtn2);
   FormResize(this);
   Calcula_bifurcaciones();
}


// Menu->Configura->Parametros
void __fastcall TFormaBifurcaciones::MenuConfigurarParametrosClick(TObject *Sender)
{
   configura = new TVCap_Bifurcaciones(this);
   if (configura) {
      PasarValoresConfiguracion();
      configura->TabbedNotebook1->PageIndex = 0;   
      configura->ShowModal();
      RetornarValoresConfiguracion();
      delete configura;
   }
}

// Menu->Configura->Dimensiones 
void __fastcall TFormaBifurcaciones::MenuConfigurarDimencionesBifClick(TObject *Sender)
{
   configura = new TVCap_Bifurcaciones(this);
   if (configura) {
      PasarValoresConfiguracion();
      configura->TabbedNotebook1->PageIndex = 1;   
      configura->ShowModal();
      RetornarValoresConfiguracion();
      delete configura;
   }
}

void __fastcall TFormaBifurcaciones::MenuConfiguraDimensionesExpClick(
      TObject *Sender)
{
   configura = new TVCap_Bifurcaciones(this);
   if (configura) {
      PasarValoresConfiguracion();
      configura->TabbedNotebook1->PageIndex = 2;   
      configura->ShowModal();
      RetornarValoresConfiguracion();
      delete configura;
   }
}


// Menu->Configura->Visualizar
void __fastcall TFormaBifurcaciones::MenuConfigurarVisualizarClick(TObject *Sender)
{
   configura = new TVCap_Bifurcaciones(this);
   if (configura) {
      PasarValoresConfiguracion();
      configura->TabbedNotebook1->PageIndex = 3;   
      configura->ShowModal();
      RetornarValoresConfiguracion();
      delete configura;
   }
}

// Menu->Configura->Calculo
void __fastcall TFormaBifurcaciones::MenuConfigurarCalculoClick(TObject *Sender)
{
   configura = new TVCap_Bifurcaciones(this);
   if (configura) {
      PasarValoresConfiguracion();
      configura->TabbedNotebook1->PageIndex = 4;   
      configura->ShowModal();
      RetornarValoresConfiguracion();
      delete configura;
   }
}


// Menu->Ayuda->Acerca de ...
void __fastcall TFormaBifurcaciones::MenuAyudaAcercadeClick(TObject *Sender)
{
   TVAcercaDe *Acercade = new TVAcercaDe(this);
   if (Acercade) {
      Acercade->ShowModal();
      delete Acercade;
   }
}

// Menu->Ayuda->Bifurcaciones
void __fastcall TFormaBifurcaciones::MenuAyudaBifurcacionesClick(TObject *Sender)
{
   TAyudaForm *Ayuda = new TAyudaForm(this);
   if (Ayuda) {
      Ayuda->Abrir_archivo("Bifurcations scenery","Bifurcaciones.hlp");
      Ayuda->Show();
   }
}

///////////////////////////////////////////////////////////////////////////////
//  Definici�n de Comportamientos de bifurcaciones
///////////////////////////////////////////////////////////////////////////////



void TFormaBifurcaciones::Calcula_bifurcaciones(void)
{
   Sw_Proceso_calculo = true;
   TCursor antcursor = Cursor;
   long double p_org[NUM_MAX_PARAMETROS];
   long double p[NUM_MAX_PARAMETROS];
   char xcad[200];
   long double Inc, T, ret2, Iaux;  
   unsigned int  I; 
   int px, py; 
   unsigned int n_c_i;

   // Cambia el cursor del mouse
   Cursor = crHourGlass;
   
   // Objeto para calcular el exponente de Lyapunov
   ExponenteLyapunov  exp_lyap;
   // Recupera los parametros por omisi�n
   fun_disp.RetornaParametros(p_org);
   // Recupera los parametros por omisi�n
   fun_disp.RetornaParametros(p);

   
   Inc = (Dim_Vtn.Xf - Dim_Vtn.Xi) / 5000.0;
   p[Parametro_bifurcar] = Dim_Vtn.Xi + Inc;

   // Inicializa la variable de cancelacion del calculo
   Sw_cancela_calculo = false;
   while (p[Parametro_bifurcar] < Dim_Vtn.Xf) {
       sprintf(xcad,"Calculating bifurcations of the parameter %s = %Lf (this process may last several minutes).", fun_disp.Nombre_parametros[Parametro_bifurcar],p[Parametro_bifurcar]);
       BarraDeEstadosBifurcaciones->SimpleText=(AnsiString) xcad;

       ////////////////////////////////////////////////////////////////////////
       // C�lculo del exponente de Lyapunov
       ////////////////////////////////////////////////////////////////////////
       if (Sw_Visualiza_exponente) {
          // Cambiar par�metros
          exp_lyap.AsignaParametros(p);
          // C�lcula el exponente de Lyapunov
          ret2 = exp_lyap.Calcula(Tipo_integracion);
          pix[1].Asigna_valor(p[Parametro_bifurcar],ret2,true);
          px = (p[Parametro_bifurcar] - Dim_Vtn2.Xi) * Escala2.X;
          py = VTExponenteLyapunov->Height - (((ret2 - Dim_Vtn2.Yi) * Escala2.Y) + 1.0);
          if (px >= 0 && px < VTExponenteLyapunov->Width && py >= 0 && py < VTExponenteLyapunov->Height) VTExponenteLyapunov->Canvas->Pixels[px][py] = Color_grafica[1];
       }
       Application->ProcessMessages();

       ////////////////////////////////////////////////////////////////////////
       // C�lculo del diagrama de bifurcaciones
       ////////////////////////////////////////////////////////////////////////
       n_c_i = 0;
       while (n_c_i < Condiciones_iniciales) {
           // Asigna la condici�n inicial segun configuraci�n del usuario
           if (Sw_Cond_inicial_aleatoria) T = (long double) random(1000) / 1000.0; 
            else {
               if (Continuar_ultima_cond_inicial) T =  Ultima_condicion_inicial;
                else T = Condicion_inicial;
           }
           I = 1;
           while (I <= Puntos_calcular) {
              if (Sw_Visualiza_bifurcaciones) {
                 // Cambiar parametros
                 fun_disp.AsignaParametros(p);
                 T = fun_disp.Calcula(T,Tipo_integracion);
                 if(fun_disp.Error_calculo) break;
                 if(T >= 0) T = modfl(T,&Iaux);
                  else T = modfl(T,&Iaux) + 1.0;
                 if (I > (Puntos_calcular - Puntos_trazar)) {
                    pix[0].Asigna_valor(p[Parametro_bifurcar],T,true);
                    px = (p[Parametro_bifurcar] - Dim_Vtn.Xi) * Escala1.X;
                    py = VTBifurcaciones->Height - (((T - Dim_Vtn.Yi) * Escala1.Y) + 1.0);
                    if (px >= 0 && px < VTBifurcaciones->Width && py >= 0 && py < VTBifurcaciones->Height) VTBifurcaciones->Canvas->Pixels[px][py] = Color_grafica[0];
                 }   
              }
              I++;
           }
           n_c_i++;
       }
       Ultima_condicion_inicial = T;
       // Controla la cancelacion del calculo
       Application->ProcessMessages();
       if (Sw_cancela_calculo) break;
       p[Parametro_bifurcar] += Inc;
//     }   
   }
   // Cambiar parametros
   fun_disp.AsignaParametros(p_org);
   BarraDeEstadosBifurcaciones->SimpleText=(AnsiString) " ";
   // Cambia el cursor del mouse
   Cursor = antcursor;
   Sw_Proceso_calculo = false;
}


// Grafica las curvas calculadas
void TFormaBifurcaciones::Grafica(void)
{
   if (Sw_cerrar_ventana) return;
   if (!pix[0].Ventana_activa()) return;

   Sw_Proceso_calculo = true;
   unsigned int x, y, py;
   C_2D escala;

   Limpiar_vantana();
   if(Sw_Visualiza_bifurcaciones) {
      // Calcula la escala de la ventana de visualizacion con respecto a la ventana de pixeles
      escala.X = VTBifurcaciones->Width  / (long double) Pix_x;
      escala.Y = VTBifurcaciones->Height / (long double) Pix_y;

      // Visualiza el diagramna de bifurcaciones
      for(y = 0; y < Pix_y; y++) {
          for(x = 0; x < Pix_x; x++) {
              if(pix[0].Retorna_valor(x, y)) VTBifurcaciones->Canvas->Pixels[x * escala.X][y * escala.Y] = Color_grafica[0];
           }
       }
   }
   
   if(Sw_Visualiza_referencia) {
      // Asigna el color a la grafica
      VTExponenteLyapunov->Canvas->Pen->Color = Color_grafica[2];
      // Visualiza referencias
      py = VTExponenteLyapunov->Height * 0.5;
      VTExponenteLyapunov->Canvas->MoveTo(0,py);
      VTExponenteLyapunov->Canvas->LineTo(VTExponenteLyapunov->Width,py);
   }

   if(Sw_Visualiza_exponente) {
      // Calcula la escala de la ventana de visualizacion con respecto a la ventana de pixeles
      escala.X = VTExponenteLyapunov->Width  / (long double) Pix_x;
      escala.Y = VTExponenteLyapunov->Height / (long double) Pix_y;
      // Grafica el exponente de Lyapunov
      for(y = 0; y < Pix_y; y++) {
          for(x = 0; x < Pix_x; x++) {
              if(pix[1].Retorna_valor(x, y)) VTExponenteLyapunov->Canvas->Pixels[x * escala.X][y * escala.Y] = Color_grafica[1];
          }
      }
   }
   Sw_Proceso_calculo = false;
}

// Limpia la ventana de graficacion
void TFormaBifurcaciones::Limpiar_vantana(void)
{
   VTBifurcaciones->Canvas->Pen->Color = Color_fondo;
   VTBifurcaciones->Canvas->Brush->Color = Color_fondo;
   VTBifurcaciones->Canvas->Rectangle(0, 0, VTBifurcaciones->Width, VTBifurcaciones->Height);
   VTExponenteLyapunov->Canvas->Pen->Color = Color_fondo;
   VTExponenteLyapunov->Canvas->Brush->Color = Color_fondo;
   VTExponenteLyapunov->Canvas->Rectangle(0, 0, VTExponenteLyapunov->Width, VTExponenteLyapunov->Height);
}

///////////////////////////////////////////////////////////////////////////////
// Controla el movimiento del mouse y genereaci�n del recuadro para el zoom del
// la ventana de bifurcaciones
///////////////////////////////////////////////////////////////////////////////

// Muestra la posicion del mouse dentro de la ventana de bifurcaciones
void __fastcall TFormaBifurcaciones::VTBifurcacionesMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
   if (Sw_Recuadro_activo) {
      if (Sw_Dibuja_rectangulo) {
         TPenMode mode;

         mode = VTBifurcaciones->Canvas->Pen->Mode;
         VTBifurcaciones->Canvas->Pen->Mode = pmNot;
      
         if (Sw_Dibuja_rectangulo_ant) {
            VTBifurcaciones->Canvas->MoveTo(Vt2.Xi,Vt2.Yi);
            VTBifurcaciones->Canvas->LineTo(Vt2.Xf,Vt2.Yi);
            VTBifurcaciones->Canvas->MoveTo(Vt2.Xi,Vt2.Yi);
            VTBifurcaciones->Canvas->LineTo(Vt2.Xi,Vt2.Yf);
            VTBifurcaciones->Canvas->MoveTo(Vt2.Xf,Vt2.Yf);
            VTBifurcaciones->Canvas->LineTo(Vt2.Xi,Vt2.Yf);
            VTBifurcaciones->Canvas->MoveTo(Vt2.Xf,Vt2.Yf);
            VTBifurcaciones->Canvas->LineTo(Vt2.Xf,Vt2.Yi);
         }
         VTBifurcaciones->Canvas->MoveTo(Vt1.Xi,Vt1.Yi);
         VTBifurcaciones->Canvas->LineTo(X,Vt1.Yi);
         VTBifurcaciones->Canvas->MoveTo(Vt1.Xi,Vt1.Yi);
         VTBifurcaciones->Canvas->LineTo(Vt1.Xi,Y);
         VTBifurcaciones->Canvas->MoveTo(X,Y);
         VTBifurcaciones->Canvas->LineTo(Vt1.Xi,Y);
         VTBifurcaciones->Canvas->MoveTo(X,Y);
         VTBifurcaciones->Canvas->LineTo(X,Vt1.Yi);
      
         Vt2.Xf = X;
         Vt2.Yf = Y;
         VTBifurcaciones->Canvas->Pen->Mode = mode;
         Sw_Dibuja_rectangulo_ant = true;

         // Ajusta a que xVt tenga el inicio y el final de la ventana
         xVt.Xi = Vt1.Xi, xVt.Xf = Vt2.Xf, xVt.Yi = Vt1.Yi, xVt.Yf = Vt2.Yf;
         int  xtmp;
         if (xVt.Xi > xVt.Xf) xtmp = xVt.Xi, xVt.Xi = xVt.Xf, xVt.Xf = xtmp;
         if (xVt.Yi > xVt.Yf) xtmp = xVt.Yi, xVt.Yi = xVt.Yf, xVt.Yf = xtmp;
         // Ajusta los valores a la longitud de la ventana
         if (xVt.Xi < 0) xVt.Xi = 0;
         if (xVt.Yi < 0) xVt.Yi = 0;
         if (xVt.Xf > VTBifurcaciones->Width) xVt.Xf = VTBifurcaciones->Width;
         if (xVt.Yf > VTBifurcaciones->Height) xVt.Yf = VTBifurcaciones->Height;
         // Visualiza los valores de la region seleccionada segun la dimenci�n de la ventana
         Vs.Xi = xVt.Xi / Escala1.X + Dim_Vtn.Xi;
         Vs.Yi = (-(xVt.Yi - VTBifurcaciones->Height) / Escala1.Y) + Dim_Vtn.Yi;
         Vs.Xf = xVt.Xf / Escala1.X + Dim_Vtn.Xi;
         Vs.Yf = (-(xVt.Yf - VTBifurcaciones->Height) / Escala1.Y) + Dim_Vtn.Yi;
         sprintf(xcad,"%s: (%3.5Lf, %3.5Lf, %3.5Lf, %3.5Lf)",VG_TXT01,Vs.Xi,Vs.Yf,Vs.Xf,Vs.Yi);
         BarraDeEstadosBifurcaciones->SimpleText = (AnsiString) xcad;
      } else {
         Vs.Xf = X, Vs.Yf = Y;
         Vs.Xi = Vs.Xf / Escala1.X + Dim_Vtn.Xi;
         Vs.Yi = (-(Vs.Yf - VTBifurcaciones->Height) / Escala1.Y) + Dim_Vtn.Yi;

         sprintf(Msg,"%s=%3.12Lf, %3.12Lf", fun_disp.Nombre_parametros[Parametro_bifurcar], Vs.Xi, Vs.Yi);
         BarraDeEstadosBifurcaciones->SimpleText = (AnsiString) Msg;
      }    
   }
}

// Cuando el mouse es presionado
void __fastcall TFormaBifurcaciones::VTBifurcacionesMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
   if (Sw_Recuadro_activo) {
      if (Button == mbLeft) {
         Vt2.Xf = Vt1.Xf = Vt2.Xi = Vt1.Xi = X;
         Vt2.Yf = Vt1.Yf = Vt2.Yi = Vt1.Yi = Y;
         Sw_Dibuja_rectangulo = true;
         Sw_Dibuja_rectangulo_ant = false;
         Ventana_seleccionada = false;
      }
   }
}

// Cuando el mouse es soltado
void __fastcall TFormaBifurcaciones::VTBifurcacionesMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X,int Y)
{
   if (Sw_Recuadro_activo) {
      if (Button == mbLeft) {
         TPenMode mode;
         mode = VTBifurcaciones->Canvas->Pen->Mode;
         VTBifurcaciones->Canvas->Pen->Mode = pmNot;
         VTBifurcaciones->Canvas->MoveTo(Vt2.Xi,Vt2.Yi);
         VTBifurcaciones->Canvas->LineTo(Vt2.Xf,Vt2.Yi);
         VTBifurcaciones->Canvas->MoveTo(Vt2.Xi,Vt2.Yi);
         VTBifurcaciones->Canvas->LineTo(Vt2.Xi,Vt2.Yf);
         VTBifurcaciones->Canvas->MoveTo(Vt2.Xf,Vt2.Yf);
         VTBifurcaciones->Canvas->LineTo(Vt2.Xi,Vt2.Yf);
         VTBifurcaciones->Canvas->MoveTo(Vt2.Xf,Vt2.Yf);
         VTBifurcaciones->Canvas->LineTo(Vt2.Xf,Vt2.Yi);
         VTBifurcaciones->Canvas->Pen->Mode = mode;
         Vt1.Xf = X;
         Vt1.Xf = Y;
         Sw_Dibuja_rectangulo = false;
         Sw_Dibuja_rectangulo_ant = false;
         BarraDeEstadosBifurcaciones->SimpleText = (AnsiString) " ";
         Ventana_seleccionada = true;
         // Revisa si realmente se solicito el zoom
         if (abs(Vt2.Xf - Vt2.Xi) > 5 && abs(Vt2.Yf - Vt2.Yi) > 5) {
            // Pregunta si se desea hacer el zoom
            if (MessageBox(Handle,"Do you wish to zoom?","Bifurcations scenery",MB_YESNO + MB_ICONQUESTION)  == IDYES) {
               // Almacena las dimensiones actuales
               Dim_zoom[Ind_zoom] = Dim_Vtn;
               if ((Ind_zoom + 1) < NUM_MAX_DIM_ZOOM) Ind_zoom ++;
               MenuVentanaZoomOut->Enabled = true;
               long double incx = fabsl(Vs.Xf - Vs.Xi);
               long double incy = fabsl(Vs.Yf - Vs.Yi);
               long double inc  = (incx > incy ? incx : incy);
               // Actualiza las dimensiones de las ventanas de trabajo
               Dim_Vtn.Xi = Vs.Xi;
               Dim_Vtn.Yi = Vs.Yf;
               
               Dim_Vtn2.Xi = Vs.Xi;

               if (Zoom_manteniendo_aspecto) {
                  Dim_Vtn.Xf = Vs.Xi + inc;
                  Dim_Vtn.Yf = Vs.Yf + inc;
                  Dim_Vtn2.Xf = Vs.Xi + inc;
                } else {
                  Dim_Vtn.Xf = Vs.Xf;
                  Dim_Vtn.Yf = Vs.Yi;
                  Dim_Vtn2.Xf = Vs.Xf;
               }
               pix[0].Cambia_dimension(Dim_Vtn);
               pix[1].Cambia_dimension(Dim_Vtn2);
               FormResize(this);
               Calcula_bifurcaciones();
            }
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
// Controla el movimiento del mouse en la ventana del exponente de Lyapunov
///////////////////////////////////////////////////////////////////////////////

// Muestra la posicion del mouse dentro de la ventana del exponente de Lyapunov
void __fastcall TFormaBifurcaciones::VTExponenteLyapunovMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
   Vs.Xf = X, Vs.Yf = Y;
   Vs.Xi = Vs.Xf / Escala2.X + Dim_Vtn2.Xi;
   Vs.Yi = (-(Vs.Yf - VTExponenteLyapunov->Height) / Escala2.Y) + Dim_Vtn2.Yi;

   sprintf(Msg,"%s=%3.12Lf, %3.12Lf", fun_disp.Nombre_parametros[Parametro_bifurcar], Vs.Xi, Vs.Yi);
   BarraDeEstadosBifurcaciones->SimpleText = (AnsiString) Msg;
        
}

///////////////////////////////////////////////////////////////////////////////
// Control de edici�n del par�metros
///////////////////////////////////////////////////////////////////////////////

// Pasa los valores de configuraci�n de la ventana de captura
void TFormaBifurcaciones::PasarValoresConfiguracion(void)
{
   ////////////////////////////////////////////////////////
   // Par�metros
   ////////////////////////////////////////////////////////
   // Par�metros
   configura->ListBoxParametros->Items->Clear();
   for( int i = 0; i < fun_disp.Numero_parametros; i++) {
      sprintf(xcad,"%-6s = %1.9Lf",fun_disp.Nombre_parametros[i],fun_disp.P[i] );
      configura->ListBoxParametros->Items->Add(xcad);
   }
   // Par�metro a bifurcar
   configura->ComboBoxParamertoBifurcar->Items->Clear();
   for( int i = 0; i < fun_disp.Numero_parametros_bifurcar; i++) {
      configura->ComboBoxParamertoBifurcar->Items->Add(fun_disp.Nombre_parametros[i]);
   }
   configura->ComboBoxParamertoBifurcar->ItemIndex = Parametro_bifurcar;
   ////////////////////////////////////////////////////////
   // Dimensiones
   ////////////////////////////////////////////////////////
   sprintf(xcad,"%Lf",Dim_Vtn.Xi);
   configura->EditHMin->Text = (AnsiString) xcad;
   sprintf(xcad,"%Lf",Dim_Vtn.Xf);
   configura->EditHMax->Text = (AnsiString) xcad;
   sprintf(xcad,"%Lf",Dim_Vtn.Yi);
   configura->EditVMin->Text = (AnsiString) xcad;
   sprintf(xcad,"%Lf",Dim_Vtn.Yf);
   configura->EditVMax->Text = (AnsiString) xcad;
   sprintf(xcad,"%Lf",Dim_Vtn2.Xi);
   configura->EditHMin2->Text = (AnsiString) xcad;
   sprintf(xcad,"%Lf",Dim_Vtn2.Xf);
   configura->EditHMax2->Text = (AnsiString) xcad;
   sprintf(xcad,"%Lf",Dim_Vtn2.Yi);
   configura->EditVMin2->Text = (AnsiString) xcad;
   sprintf(xcad,"%Lf",Dim_Vtn2.Yf);
   configura->EditVMax2->Text = (AnsiString) xcad;
   ////////////////////////////////////////////////////////
   // Visualizar
   ////////////////////////////////////////////////////////
   // Check Box
   configura->CheckBox1->Checked = Sw_Visualiza_bifurcaciones;
   configura->CheckBox2->Checked = Sw_Visualiza_exponente;
   configura->CheckBox3->Checked = Sw_Visualiza_referencia;
   // Colores
   configura->Panel5->Color = Color_grafica[0];
   configura->Panel6->Color = Color_grafica[1];
   configura->Panel7->Color = Color_grafica[2];
   configura->Panel8->Color = Color_fondo;
   ////////////////////////////////////////////////////////
   // Escenario
   ////////////////////////////////////////////////////////
   sprintf(xcad,"%u",Puntos_calcular);
   configura->Edit8->Text = (AnsiString) xcad;
   sprintf(xcad,"%u",Puntos_trazar);
   configura->Edit9->Text = (AnsiString) xcad;
   configura->RadioButton1->Checked = Tipo_integracion;   
   configura->RadioButton2->Checked = !Tipo_integracion;   
   sprintf(xcad,"%Lf",Condicion_inicial);
   configura->Edit14->Text = (AnsiString) xcad;
   configura->CheckBox5->Checked = Sw_Cond_inicial_aleatoria;      
}

// Retorna los valores de configuraci�n de la ventana de captura
void TFormaBifurcaciones::RetornarValoresConfiguracion(void)
{
   if (!configura->Aceptar) return;
   ////////////////////////////////////////////////////////
   // Par�metros
   ////////////////////////////////////////////////////////
   for (int i = 0; i < fun_disp.Numero_parametros; i++) {
       unsigned int i1, i2;
       char xcad[100], xcad1[100];
       AnsiString txt1(configura->ListBoxParametros->Items->Strings[i]);
       strcpy(xcad1,txt1.c_str());
       // Valor del par�metro
       for (i2 = 0, i1 = 9; i1 < strlen(xcad1); i2++, i1++) xcad[i2] = xcad1[i1];
       xcad[i2] = 0;
       fun_disp.P[i] = atof(xcad);
   }
   // Par�metro a bifurcar
   Parametro_bifurcar = configura->ComboBoxParamertoBifurcar->ItemIndex;
   ////////////////////////////////////////////////////////
   // Dimensiones
   ////////////////////////////////////////////////////////
   AnsiString txt1(configura->EditHMin->Text);
   Dim_Vtn.Xi = atof(txt1.c_str());       
   AnsiString txt2(configura->EditHMax->Text);
   Dim_Vtn.Xf = atof(txt2.c_str());       
   AnsiString txt3(configura->EditVMin->Text);
   Dim_Vtn.Yi = atof(txt3.c_str());       
   AnsiString txt4(configura->EditVMax->Text);
   Dim_Vtn.Yf = atof(txt4.c_str());
// Se tomara solamente la dimension horizontal de la ventana de bifurcaciones         
   AnsiString txt5(configura->EditHMin->Text);
   Dim_Vtn2.Xi = atof(txt5.c_str());       
   AnsiString txt6(configura->EditHMax->Text);
   Dim_Vtn2.Xf = atof(txt6.c_str());       
//   Dim_Vtn2.Xi = atof(configura->EditHMin2->Text.c_str());       
//   Dim_Vtn2.Xf = atof(configura->EditHMax2->Text.c_str());       
   AnsiString txt7(configura->EditVMin2->Text);
   Dim_Vtn2.Yi = atof(txt7.c_str());       
   AnsiString txt8(configura->EditVMax2->Text);
   Dim_Vtn2.Yf = atof(txt8.c_str());       
   ////////////////////////////////////////////////////////
   // Visualizar
   ////////////////////////////////////////////////////////
   // Check Box
   Sw_Visualiza_bifurcaciones = configura->CheckBox1->Checked;
   Sw_Visualiza_exponente = configura->CheckBox2->Checked;
   Sw_Visualiza_referencia = configura->CheckBox3->Checked;
   // Colores
   Color_grafica[0] = configura->Panel5->Color;
   Color_grafica[1] = configura->Panel6->Color;
   Color_grafica[2] = configura->Panel7->Color;
   Color_fondo      = configura->Panel8->Color;
   ////////////////////////////////////////////////////////
   // Escenario
   ////////////////////////////////////////////////////////
   AnsiString txt9(configura->Edit8->Text);
   Puntos_calcular  = atoi(txt9.c_str());       
   AnsiString txt10(configura->Edit9->Text);
   Puntos_trazar    = atoi(txt10.c_str());       
   Tipo_integracion = configura->RadioButton1->Checked;   
   AnsiString txt11(configura->Edit14->Text);
   Condicion_inicial = atof(txt11.c_str());       
   Sw_Cond_inicial_aleatoria = configura->CheckBox5->Checked;   
   /////////////////////////////////////////////////////////
   // Al terminar
   /////////////////////////////////////////////////////////
   pix[0].Cambia_dimension(Dim_Vtn);
   pix[1].Cambia_dimension(Dim_Vtn2);
   FormResize(this);
}



void __fastcall TFormaBifurcaciones::VTExponenteLyapunovPaint(TObject *Sender)
{
   // Grafica el contenido de la ventana
   Grafica();        
}

void __fastcall TFormaBifurcaciones::VTBifurcacionesPaint(TObject *Sender)
{
   // Grafica el contenido de la ventana
   Grafica();        
}

// OnHint
void  __fastcall TFormaBifurcaciones::OnHint(TObject *Sender)
{
   BarraDeEstadosBifurcaciones->SimpleText = Application->Hint;
}

void __fastcall TFormaBifurcaciones::FormActivate(TObject *Sender)
{
   Application->OnHint = OnHint;
}

void __fastcall TFormaBifurcaciones::FormDeactivate(TObject *Sender)
{
   Application->OnHint = NULL;
}


// Lanza una ventana del escenario Toro
void __fastcall TFormaBifurcaciones::Torus1Click(TObject *Sender)
{
   TFormaToro *vent_tor = new TFormaToro(this);
   if(vent_tor) {
      for( int i = 0; i < fun_disp.Numero_parametros; i++) vent_tor->fun_disp.P[i] = fun_disp.P[i];
      vent_tor->fun_disp.P[Parametro_bifurcar] = Vs.Xi;
      vent_tor->Calcula_toro();
      vent_tor->Show();
   }
}

// Lanza una ventana del escenario C�rculo
void __fastcall TFormaBifurcaciones::Circle1Click(TObject *Sender)
{
   TFormaCirculo *vent_cir = new TFormaCirculo(this);
   if(vent_cir) {
      for( int i = 0; i < fun_disp.Numero_parametros; i++) vent_cir->fun_disp.P[i] = fun_disp.P[i];
      vent_cir->fun_disp.P[Parametro_bifurcar] = Vs.Xi;
      vent_cir->Show();
   }
}

