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
#include <stdio.h>
#include <Printers.hpp>
#include "Acecade.h"
#include "Ayuda.h"
#include "VentanaCirculo.h"
#include "Sincronizaciones.hpp"
#include "NumeroRotacion.hpp"
#include "ExponenteLyapunov.hpp"
#include "LMensaje.h"


#include "VentanaToro.h"
#pragma hdrstop

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
extern  char                  *COMPANIA;
extern  char                  *E_MAIL;

// Al cerrar la forma ...
void __fastcall TFormaCirculo::FormClose(TObject *Sender,TCloseAction &Action)
{
   Ventana_activa = NULL;
   Free();        
}

// Al solicitar cerrar la ventana
void __fastcall TFormaCirculo::FormCloseQuery(TObject *Sender, bool &CanClose)
{
   if (MessageBox(Handle,"Do you wish to close this window?","Circle scenery",MB_YESNO + MB_ICONQUESTION)  == IDYES) CanClose = true;
    else CanClose = false;
}

// Al cambiar de tama�o
void __fastcall TFormaCirculo::FormResize(TObject *Sender)
{
   // Calcula la escala de la ventana de visualizacion
   Escala.X = (VTCirculo->Width -1) /  (Dim_Vtn.Xf - Dim_Vtn.Xi);
   Escala.Y = (VTCirculo->Height -1) / (Dim_Vtn.Yf - Dim_Vtn.Yi);
   Grafica();        
}


///////////////////////////////////////////////////////////////////////////////
//  Definici�n de Comportamientos del Menu 
///////////////////////////////////////////////////////////////////////////////

// Menu->Archivo->Cerrar
void __fastcall TFormaCirculo::MenuArchivoCerrarClick(TObject *Sender)
{
   Close();        
}

// Menu->Archivo->Grabar BMP
void __fastcall TFormaCirculo::MenuArchivoGrabarBMPClick(TObject *Sender)
{
   TRect   xRect = Rect(0,0,VTCirculo->Width,VTCirculo->Height);
   Graphics::TBitmap *Bitmap = new Graphics::TBitmap;
   Bitmap->Width = VTCirculo->Width;
   Bitmap->Height = VTCirculo->Height;
   Bitmap->Canvas->CopyRect(xRect, VTCirculo->Canvas, xRect);
   SaveDialog->Title = "Save as ...";
   if (SaveDialog->Execute()) {
      Bitmap->SaveToFile(SaveDialog->FileName);           
   }
   VTCirculo->Canvas->Draw(0,0,Bitmap);
   delete Bitmap;
}

// Menu->Archivo->Imprimir
void __fastcall TFormaCirculo::MenuArchivoImprimirClick(TObject *Sender)
{
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



// Menu->Calcular->Iteraciones en el Circulo
void __fastcall TFormaCirculo::MenuCalcularIteracinesenCirculoClick(TObject *Sender)
{
   Calcula_iteraciones();
}

void __fastcall TFormaCirculo::Atractors1Click(TObject *Sender)
{
   Detener_calculo = false;
   // Cambia el cursor del mouse
   TCursor antcursor = Cursor;
   Cursor = crHourGlass;
   
   long double x, t, xt, xtmp, ytmp;
   int n_i, i, px, py;

   Grafica();
   // Color del punto peri�dico
   TColor color = Color_grafica[3];
   for (x = 0.0; x < 1.0; x += 1e-2) {
       t = x;
      // Transitorios
      for(n_i = 0; n_i < 10000; n_i++) {
         t = fun_disp.Calcula(t,Tipo_integracion);
         if (fun_disp.Error_calculo) break;
         t = modfl(t,&aux);
      }   
      xt = t;
//////////////////////////////////////////////////////////////////////////////////////////////////////      
// Al encontrar la �rbitas peri�dicas almacene el per�odo y la envolvencia de las �rbitas encontradas
// al igual que el n�mero de �stas.
// Para calcular la cuenca de atracci�n tomar una partici�n de condiciones iniciales e iterar un 
// transitorio dado, despues ver a que atractor converge y segun el numero de �ste en el indice, dibujar
// el punto de condiciones iniciales con el color del indice
//////////////////////////////////////////////////////////////////////////////////////////////////////      
      // Busca orbitas peri�dicas
      for(n_i = 0; n_i < 1000; n_i++) {
         t = fun_disp.Calcula(t,Tipo_integracion);
         if (fun_disp.Error_calculo) break;
         t = modfl(t,&aux);
         if (fabsl(t-xt) < 1e-5) {
            for(i = 0; i < 1000; i++) {
               t = fun_disp.Calcula(t,Tipo_integracion);
               if (fun_disp.Error_calculo) break;
               t = modfl(t,&aux);
               xtmp = cosl(2.0 * M_PI*t);
               ytmp = sinl(2.0 * M_PI*t);
               pix[1].Asigna_valor(xtmp,ytmp,true);
               px = (xtmp - Dim_Vtn.Xi) * Escala.X;
               py = VTCirculo->Height - (((ytmp - Dim_Vtn.Yi) * Escala.Y) + 1.0);
               Visualiza_marca(px, py, color);
            }
            break;
         }
      }   
   }
   // Cambia el cursor del mouse
   Cursor = antcursor;
   Detener_calculo = false;
}
//---------------------------------------------------------------------------


// Menu->Calcular->Numero de Rotaci�n
void __fastcall TFormaCirculo::MenuCalcularNumRotacionClick(TObject *Sender)
{
   TCursor antcursor = Cursor;
   // Cambia el cursor del mouse
   Cursor = crHourGlass;
   long double p[NUM_MAX_PARAMETROS];
   BarraDeEstadosCirculo->SimpleText = (AnsiString) "Calculating rotation number (this process may last several seconds).";
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
   BarraDeEstadosCirculo->SimpleText = (AnsiString) " ";
}

// Menu->Calcular->Sincronizaci�n
void __fastcall TFormaCirculo::MenuCalcularSincronizacionClick(TObject *Sender)
{
   TCursor antcursor = Cursor;
   // Cambia el cursor del mouse
   Cursor = crHourGlass;
   long double p[NUM_MAX_PARAMETROS];
   BarraDeEstadosCirculo->SimpleText = (AnsiString) "Calculating synchronization (this process may last several seconds).";
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
   BarraDeEstadosCirculo->SimpleText = (AnsiString) " ";
   // Cambia el cursor del mouse
   Cursor = antcursor;
}

// Menu->Calcular->Exponente de Lyapunov
void __fastcall TFormaCirculo::MenuCalcularExpLyapunovClick(TObject *Sender)
{
   TCursor antcursor = Cursor;
   // Cambia el cursor del mouse
   Cursor = crHourGlass;
   long double p[NUM_MAX_PARAMETROS], rot;
   BarraDeEstadosCirculo->SimpleText = (AnsiString) "Calculating Lyapunov exponent (this process may last several seconds).";
   // Recupera los par�metros por omisi�n
   fun_disp.RetornaParametros(p);
   // Cambiar par�metros
   exp_lyap.AsignaParametros(p);
   rot = exp_lyap.Calcula(Tipo_integracion);
   char xcad[200];
   sprintf(xcad,"Lyapunov exponent: %Lf",rot);
   MessageBox(NULL,xcad,"Circle scenery",MB_OK+MB_ICONINFORMATION);
   BarraDeEstadosCirculo->SimpleText = (AnsiString) " ";
   // Cambia el cursor del mouse
   Cursor = antcursor;
}

//Menu->Ventana->Limpiar
void __fastcall TFormaCirculo::MenuVentanaLimpiarClick(TObject *Sender)
{
   Limpia_matriz_pixeles();
   Grafica();
}


// Menu->Configurar->Parametros
void __fastcall TFormaCirculo::MenuConfigurarParametrosClick(TObject *Sender)
{
   configura = new TVCap_Circunferencia(this);
   if (configura) {
      PasarValoresConfiguracion();
      configura->TabbedNotebook1->PageIndex = 0;
      configura->ShowModal();
      RetornarValoresConfiguracion();
      delete configura;
   }
}

// Menu->Configurar->Visualizar
void __fastcall TFormaCirculo::MenuConfigurarVisualizarClick(TObject *Sender)
{
   configura = new TVCap_Circunferencia(this);
   if (configura) {
      PasarValoresConfiguracion();
      configura->TabbedNotebook1->PageIndex = 1;
      configura->ShowModal();
      RetornarValoresConfiguracion();
      delete configura;
   }
}


// Menu->Configurar->Calculo
void __fastcall TFormaCirculo::MenuConfigurarCalculoClick(TObject *Sender)
{
   configura = new TVCap_Circunferencia(this);
   if (configura) {
      PasarValoresConfiguracion();
      configura->TabbedNotebook1->PageIndex = 2;
      configura->ShowModal();
      RetornarValoresConfiguracion();
      delete configura;
   }
}

// Menu->Ayuda->Acerca de ...
void __fastcall TFormaCirculo::MenuAyudaAcercadeClick(TObject *Sender)
{
   TVAcercaDe *Acercade = new TVAcercaDe(this);
   if (Acercade) {
      Acercade->ShowModal();
      delete Acercade;
   }
}

// Menu->Ayuda->Circulo
void __fastcall TFormaCirculo::MenuAyudaCirculoClick(TObject *Sender)
{
   TAyudaForm *Ayuda = new TAyudaForm(this);
   if (Ayuda) {
      Ayuda->Abrir_archivo("Circle scenery","Circulo.hlp");
      Ayuda->Show();
   }
}


///////////////////////////////////////////////////////////////////////////////
//  Definici�n de Comportamientos de Circulo 
///////////////////////////////////////////////////////////////////////////////
// Constructor de la clase
__fastcall TFormaCirculo::TFormaCirculo(TComponent* Owner) : TForm(Owner)
{
   MousePresionado = false;
   Detener_calculo = false;
   // Dimenciones de la ventana
   Dim_Vtn.Xi = -1.5;
   Dim_Vtn.Xf = 1.5;
   Dim_Vtn.Yi = -1.5;
   Dim_Vtn.Yf = 1.5;

   // Color de los graficos
   Color_grafica[0] = clWhite;    // Referencia
   Color_grafica[1] = clBlue;     // Gr�fico
   Color_grafica[2] = clYellow;   // Condici�n inicial
   Color_grafica[3] = clRed;      // Primer disparo
   Color_fondo = clSilver;
   // Visualiaci�n
   Sw_Visualiza_grafico = true;
   Sw_Visualiza_referencia = true;

   Iteraciones = 10;

   Tipo_integracion = ADELANTE;

   
   Condicion_inicial.X = 0.0; 
   xCondicion_inicial_visualizada = 0.0;
   
   Pix_x = 1024; 
   Pix_y = 768;
   
   pix[0].Inicializa(Pix_x,Pix_y, false, Dim_Vtn);
   pix[1].Inicializa(Pix_x,Pix_y, false, Dim_Vtn);

   // Calcula la referencia
   Calcula_referencia();
   Grafica_condicion_inicial();
}


// Calcula las iteraciones en la TFormaCirculo
void TFormaCirculo::Calcula_iteraciones(void) 
{
   TCursor antcursor = Cursor;
   // Cambia el cursor del mouse
   Cursor = crHourGlass;
   unsigned int n_i;
   int px, py;
   long double xtmp, ytmp, t = Condicion_inicial.X;

   Detener_calculo = false;
   Grafica();
   TColor color = Color_grafica[3];

   // Visualiza la actual condici�n inicial
   xtmp = cosl(2.0 * M_PI * t);
   ytmp = sinl(2.0 * M_PI * t);
   pix[1].Asigna_valor(xtmp,ytmp,true);
   px = (xtmp - Dim_Vtn.Xi) * Escala.X;
   py = VTCirculo->Height - (((ytmp - Dim_Vtn.Yi) * Escala.Y) + 1.0);
   Visualiza_marca(px, py, color);
   color = Color_grafica[1];
   // Visualiza las iteraciones en el circulo
   for(n_i = 0; n_i < Iteraciones; n_i++) {
      if (Detener_calculo) break;
      t = fun_disp.Calcula(t,Tipo_integracion);
      if (fun_disp.Error_calculo) break;
      t = modfl(t,&aux);
      xtmp = cosl(2.0 * M_PI*t);
      ytmp = sinl(2.0 * M_PI*t);
      pix[1].Asigna_valor(xtmp,ytmp,true);
      px = (xtmp - Dim_Vtn.Xi) * Escala.X;
      py = VTCirculo->Height - (((ytmp - Dim_Vtn.Yi) * Escala.Y) + 1.0);
      Visualiza_marca(px, py, color);
      // Procesa los mensajes pendientes
      Application->ProcessMessages();
      // Visualiza valores de la actual iteraci�n
      sprintf(Msg,"Initial condition: %3.8Lf Iteration No.: %d", t, (n_i+1));
      BarraDeEstadosCirculo->SimpleText = (AnsiString) Msg;
   }   
   // Grafica la nueva condici�n inicial
   Condicion_inicial.X = t;
   Grafica_condicion_inicial();
   // Cambia el cursor del mouse
   Cursor = antcursor;
   Detener_calculo = false;
}


// Grafica la referencia de la ventana
void TFormaCirculo::Calcula_referencia(void)
{
   long double y ,x;
   // Calcula la TFormaCirculo
   for(x = -1.0; x <= 1.0; x += 0.00001) {
      y = sqrtl(1.0 - ((x) * (x)));
      pix[0].Asigna_valor(x,y,true);
      pix[0].Asigna_valor(x,-y,true); 
   }
}

// Grafica las curvas calculadas
void TFormaCirculo::Grafica(bool tp)
{
//   if (Sw_cerrar_ventana) return;
   if (!pix[0].Ventana_activa()) return;

   unsigned int x, y, i;
   C_2D escala;

   if (tp) Limpiar_vantana();
   // Calcula la escala de la ventana de visualizacion con respecto a la ventana de pixeles
   escala.X = VTCirculo->Width  / (long double) Pix_x;
   escala.Y = VTCirculo->Height / (long double) Pix_y;

   VTCirculo->Canvas->Font->Color = Color_grafica[1];
   for(i = 0; i < 2; i++) {
      if(i == 0 && !Sw_Visualiza_grafico) i++;
      if(i == 1 && !Sw_Visualiza_referencia) break;
      for(y = 0; y < Pix_y; y++) {
          for(x = 0; x < Pix_x; x++) {
              if(pix[i].Retorna_valor(x, y)) {
                 VTCirculo->Canvas->Pixels[x * escala.X][y * escala.Y] = Color_grafica[i];
                 if (i == 1) Visualiza_marca(x * escala.X, y * escala.Y,Color_grafica[i]);
              }
           }
       }
   }
   Grafica_condicion_inicial();
}

// Limpia la ventana de graficacion
void TFormaCirculo::Limpiar_vantana(void)
{
   VTCirculo->Canvas->Pen->Color = Color_fondo;
   VTCirculo->Canvas->Brush->Color = Color_fondo;
   VTCirculo->Canvas->Rectangle(0, 0, VTCirculo->Width, VTCirculo->Height);
}

// Visualiza la marca de la condici�n inicial
void TFormaCirculo::Visualiza_marca(const int x, const int y, const TColor color)
{
   VTCirculo->Canvas->Pixels[x][y] = color;
   for (int i = 0; i < 5; i++) {
      VTCirculo->Canvas->Pixels[x+i][y+i] = color;
      VTCirculo->Canvas->Pixels[x+i][y-i] = color;
      VTCirculo->Canvas->Pixels[x-i][y+i] = color;
      VTCirculo->Canvas->Pixels[x-i][y-i] = color;
      VTCirculo->Canvas->Pixels[x+i][y] = color;
      VTCirculo->Canvas->Pixels[x-i][y] = color;
      VTCirculo->Canvas->Pixels[x][y+i] = color;
      VTCirculo->Canvas->Pixels[x][y-i] = color;
   }
}




// Pasa los valores de configuraci�n de la ventana de captura
void TFormaCirculo::PasarValoresConfiguracion(void)
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
   ////////////////////////////////////////////////////////
   // Visualizar
   ////////////////////////////////////////////////////////
   // Check Box
   configura->CheckBox1->Checked = Sw_Visualiza_grafico;
   configura->CheckBox2->Checked = Sw_Visualiza_referencia;
   // Colores
   configura->Panel5->Color = Color_grafica[1];
   configura->Panel6->Color = Color_grafica[0];
   configura->Panel7->Color = Color_grafica[2];
   configura->Panel8->Color = Color_grafica[3];
   configura->Panel9->Color = Color_fondo;
   ////////////////////////////////////////////////////////
   // Escenario
   ////////////////////////////////////////////////////////
   sprintf(xcad,"%Lf",Condicion_inicial.X);
   configura->Edit8->Text = (AnsiString) xcad;
   sprintf(xcad,"%u",Iteraciones);
   configura->Edit9->Text = (AnsiString) xcad;
   configura->RadioButton1->Checked = Tipo_integracion;   
   configura->RadioButton2->Checked = !Tipo_integracion;   
}

// Retorna los valores de configuraci�n de la ventana de captura
void TFormaCirculo::RetornarValoresConfiguracion(void)
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
   ////////////////////////////////////////////////////////
   // Visualizar
   ////////////////////////////////////////////////////////
   // Check Box
   Sw_Visualiza_grafico    = configura->CheckBox1->Checked;
   Sw_Visualiza_referencia = configura->CheckBox2->Checked;
   // Colores
   Color_grafica[1] = configura->Panel5->Color;
   Color_grafica[0] = configura->Panel6->Color;
   Color_grafica[2] = configura->Panel7->Color;
   Color_grafica[3] = configura->Panel8->Color;
   Color_fondo      = configura->Panel9->Color;
   ////////////////////////////////////////////////////////
   // Escenario
   ////////////////////////////////////////////////////////
   AnsiString txt1(configura->Edit8->Text);
   Condicion_inicial.X = atof(txt1.c_str());       
   AnsiString txt2(configura->Edit9->Text);
   Iteraciones         = atoi(txt2.c_str());       
   Tipo_integracion    = configura->RadioButton1->Checked;   
   /////////////////////////////////////////////////////////
   // Al terminar
   /////////////////////////////////////////////////////////
   Grafica();
}






void __fastcall TFormaCirculo::Timer1Timer(TObject *Sender)
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
          MenuCalcularExpLyapunovClick(this);
      }
      // Configura el escenario
      if (Configura_escenario) {
          Configura_escenario = false;
          MenuConfigurarParametrosClick(this);
      }
      // Calcula
      if (Calcula_escenario) {
          Calcula_escenario = false;
          MenuCalcularIteracinesenCirculoClick(this);
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
}
//---------------------------------------------------------------------------

void __fastcall TFormaCirculo::MenuCalcularDetenerelcalculoClick(TObject *Sender)
{
   Detener_calculo = true;        
}

// Cuando el mouse es movido
void __fastcall TFormaCirculo::VTCirculoMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
   Vs.Xf = X, Vs.Yf = Y;
   Vs.Xi = Vs.Xf / Escala.X + Dim_Vtn.Xi;
   Vs.Yi = (-(Vs.Yf - VTCirculo->Height) / Escala.Y) + Dim_Vtn.Yi;


   if (MousePresionado) {
      if (Vs.Xi < Dim_Vtn.Xi || Vs.Xi > Dim_Vtn.Xf) return;
      if (Vs.Xi < 0 ) Vs.Xi *= -1.0;
      long double aux;
      xCondicion_inicial = modfl(Vs.Xi,&aux);

      tmppxa = (cosl(2.0 * M_PI * xCondicion_inicial_visualizada)* 0.9 - Dim_Vtn.Xi) * Escala.X;
      tmppya = VTCirculo->Height - ((sinl(2.0 * M_PI * xCondicion_inicial_visualizada)* 0.9 - Dim_Vtn.Yi) * Escala.Y);
      Visualiza_marca(tmppxa, tmppya, Color_fondo);

      tmppxa = (cosl(2.0 * M_PI * xCondicion_inicial)* 0.9 - Dim_Vtn.Xi) * Escala.X;
      tmppya = VTCirculo->Height - ((sinl(2.0 * M_PI * xCondicion_inicial) * 0.9 - Dim_Vtn.Yi) * Escala.Y);
      Visualiza_marca(tmppxa, tmppya, Color_grafica[2]);
      xCondicion_inicial_visualizada = xCondicion_inicial;
      
      sprintf(Msg,"Initial condition: %3.8Lf", xCondicion_inicial);
      BarraDeEstadosCirculo->SimpleText = (AnsiString) Msg;
    } else {
      sprintf(Msg,"Mouse position: (%3.8Lf, %3.8Lf)", Vs.Xi, Vs.Yi);
      BarraDeEstadosCirculo->SimpleText = (AnsiString) Msg;
   }
}

// Al cer presionado el mouse
void __fastcall TFormaCirculo::VTCirculoMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
   MousePresionado = true;
}

// Al ser soltado el mouse
void __fastcall TFormaCirculo::VTCirculoMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
   MousePresionado = false;
   Condicion_inicial.X = xCondicion_inicial; 
   Limpia_matriz_pixeles();
   Grafica_condicion_inicial();
   Grafica();
}

// Fija la condici�n inicial indicada por el usuario
void __fastcall TFormaCirculo::FijarCondicIninicialClick(TObject *Sender)
{
   Condicion_inicial.X = xCondicion_inicial; 
   Grafica();
}

// Grafico el indicador de la condici�n inicial
void  TFormaCirculo::Grafica_condicion_inicial(void)
{
   tmppx = (cosl(2.0 * M_PI * Condicion_inicial.X) - Dim_Vtn.Xi) * Escala.X;
   tmppy = VTCirculo->Height - (((sinl(2.0 * M_PI * Condicion_inicial.X) - Dim_Vtn.Yi) * Escala.Y) + 1.0);
   Visualiza_marca(tmppx, tmppy, Color_grafica[2]);
}


//#error Que la regi�n de visualizaci�n sea siempre un cuadrado (evitando la deformaci�n del circulo)

void __fastcall TFormaCirculo::VTCirculoPaint(TObject *Sender)
{
   Grafica();        
   Grafica_condicion_inicial();
}

// OnHint
void  __fastcall TFormaCirculo::OnHint(TObject *Sender)
{
   BarraDeEstadosCirculo->SimpleText = Application->Hint;
}

void __fastcall TFormaCirculo::FormActivate(TObject *Sender)
{
   Application->OnHint = OnHint;
}

void __fastcall TFormaCirculo::FormDeactivate(TObject *Sender)
{
   Application->OnHint = NULL;
}



// Lanza una ventana del escenario Toro
void __fastcall TFormaCirculo::Torus1Click(TObject *Sender)
{
   TFormaToro *vent_tor = new TFormaToro(this);
   if(vent_tor) {
      for( int i = 0; i < fun_disp.Numero_parametros; i++) vent_tor->fun_disp.P[i] = fun_disp.P[i];
      vent_tor->Calcula_toro();
      vent_tor->Show();
   }
}



