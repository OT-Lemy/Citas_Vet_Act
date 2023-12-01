#include <iostream>
#include <windows.h>
#include <string>
#include <stdio.h>
#include <commctrl.h>
#include <fstream>
#include <map>
#include "DiseñoWinapi.h"

HINSTANCE hInst;

using namespace std;

////

struct VETERINARIO
{
	string nombreVeterinario;
	string cedula;
	HBITMAP foto;
	string usuario;
	string contrasena;
};



struct USUARIO {
	std::string cedula;
};

std::map<std::string, USUARIO> sesionesActivas;



struct NODOVET {
	VETERINARIO* veterinario; //datos de veterinario
	NODOVET* anterior; //nodo datos de veterinario anterior
	NODOVET* siguiente; //nodo datos de veterinario despues
};

struct VETERINARIOS {
	NODOVET* origen; //inicio del nodo(lista)
	NODOVET* fin; //final del nodo(lista)
}LISTAVETERINARIOS;

////

struct CITA {
	string fechaHora;
	string nombreCliente;
	string telefonoCita;
	string nombreMascota;
	string motivo;
	float costo;
	string especie;
	string status;
	string cedula;
};

struct NODOCITA {
	CITA* cita;
	NODOCITA* anterior;
	NODOCITA* siguiente;
};

struct CITAS {
	NODOCITA* origen;
	NODOCITA* fin;
}LISTACITAS;

CITA* crearCita(const string& nombreCliente, const string& fechaHora, const string telefonoCita, const string& nombreMascota,
	const string& motivo, float costo, const string& especie, const string& status, const string& cedula);

void agregarCitaFinal(CITA* cita);

void guardarCitasEnArchivo(const char* nombreArchivo);
void cargarCitasDesdeArchivo(const char* nombreArchivo);
void EliminarListaCita();

NODOCITA* nuevoNodo(CITA* cita);

NODOCITA* buscarNombre(const string& buscarNomCliente, const string& cedulaUsuario);

CITA* EliminarCitaMedio(const string& nombre, const string& cedulaUsuario);
CITA* EliminarCitaFinal();
CITA* EliminarCitaInicio();

VETERINARIO* crearUsuario(const string& nombreVeterinario, const string& cedula, const string& usuario, const string& contrasena);
void agregarVeterinarioFinal(VETERINARIO* veterinario);
void guardarUsuariosEnArchivo(const string& nombreArchivo);
NODOVET* verificarUsuario(const string& usuario, const string& contrasena);
void cargarUsuariosDesdeArchivo(const string& nombreArchivo);
string cedulaUsuarioActivo();

HBITMAP CargarImagen(const wchar_t* rutaImagen) {
	return (HBITMAP)LoadImage(NULL, rutaImagen, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, PSTR cmdLine, int cShow) {

	cargarCitasDesdeArchivo("DatosMiCat.bin");
	cargarUsuariosDesdeArchivo("DatosVeterinariosMiCat.bin");

	HWND hVentana1 = CreateDialog(hInst, MAKEINTRESOURCE(ID_DIA_Login), NULL, VentanaPrincipal);

	if (!hVentana1)
	{
		return FALSE;
	}

	// Obtener las dimensiones de la pantalla
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Obtener las dimensiones de la ventana
	RECT windowRect;
	GetWindowRect(hVentana1, &windowRect);
	int windowWidth = windowRect.right - windowRect.left;
	int windowHeight = windowRect.bottom - windowRect.top;

	// Calcular las coordenadas para centrar la ventana
	int x = (screenWidth - windowWidth) / 2;
	int y = (screenHeight - windowHeight) / 2;

	// Mover la ventana a las coordenadas calculadas
	MoveWindow(hVentana1, x, y, windowWidth, windowHeight, TRUE);

	ShowWindow(hVentana1, cShow);
	UpdateWindow(hVentana1);

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while (GetMessage(&msg, NULL, NULL, NULL)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return TRUE;
}

LRESULT CALLBACK VentanaRegistroUsser(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	static HBITMAP hBitmap = NULL;

	switch (msg) {
	case WM_COMMAND: {
		int wmId = LOWORD(wParam);
		switch (wmId)
		{
		case ID_BTN_RegExaminar: {

		}

		case ID_BTN_RegUssOK: {

			// Leemos los datos desde el UI
			wchar_t nombreVeterinario[255];
			GetDlgItemText(hWnd, ID_EC_RegNombreDoctor, nombreVeterinario, 255);

			wchar_t cedula[255];
			GetDlgItemText(hWnd, ID_EC_RegCedula, cedula, 255);

			wchar_t usuario[255];
			GetDlgItemText(hWnd, ID_EC_RegClaveUss, usuario, 255);

			wchar_t contrasena[255];
			GetDlgItemText(hWnd, ID_EC_RegContrasena, contrasena, 255);

			///
			OPENFILENAME ofn;
			wchar_t szFileName[MAX_PATH] = L"";
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hWnd;
			ofn.lpstrFilter = L"Imágenes (*.bmp;*.jpg;*.jpeg;*.png)\0*.BMP;*.JPG;*.JPEG;*.PNG\0Todos los archivos\0*.*\0";
			ofn.lpstrFile = szFileName;
			ofn.nMaxFile = MAX_PATH;
			ofn.Flags = OFN_FILEMUSTEXIST;

			if (GetOpenFileName(&ofn)) {
				// Cargar la imagen y almacenar el handle del bitmap
				hBitmap = CargarImagen(szFileName);
				if (hBitmap == NULL) {
					MessageBox(hWnd, L"Error al cargar la imagen.", L"Error", MB_OK | MB_ICONERROR);
				}
				else {
					// Mostrar la imagen (por ejemplo, en un control de imagen)
					// Asegúrate de tener un control de imagen en tu ventana (ID_IMG_Doctor) y ajusta según sea necesario
					SendMessage(GetDlgItem(hWnd, ID_FOTO_RegUss), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
				}
			}

			///

			//wchar_t -> wstring
			wstring nombreVeterinarioStr(nombreVeterinario);
			wstring cedulaStr(cedula);
			wstring usuarioStr(usuario);
			wstring contrasenaStr(contrasena);

			//wstring -> string
			string nombreVeterinarioA(nombreVeterinarioStr.begin(), nombreVeterinarioStr.end());
			string cedulaA(cedulaStr.begin(), cedulaStr.end());
			string usuarioA(usuarioStr.begin(), usuarioStr.end());
			string contrasenaA(contrasenaStr.begin(), contrasenaStr.end());

			// Crear la estructura VETERINARIO
			VETERINARIO* nuevo = crearUsuario(nombreVeterinarioA, cedulaA, usuarioA, contrasenaA);

			agregarVeterinarioFinal(nuevo);

			// Mostrar el nombre del Vet agregado en un messageBox
			wstring message = L"El usuario" + wstring(nombreVeterinario) + L" se registró exitosamente!";
			int respuesta = MessageBox(hWnd, message.c_str(), L"Registrado", MB_OK);

			guardarUsuariosEnArchivo("DatosVeterinariosMiCat.bin");

			if (respuesta == IDOK) {
				// Limpiar el contenido del cuadro de texto
				SetWindowText(GetDlgItem(hWnd, ID_EC_RegNombreDoctor), L"");
				SetWindowText(GetDlgItem(hWnd, ID_EC_RegCedula), L"");
				SetWindowText(GetDlgItem(hWnd, ID_EC_RegClaveUss), L"");
				SetWindowText(GetDlgItem(hWnd, ID_EC_RegContrasena), L"");
				DeleteObject(hBitmap);
				hBitmap = NULL;
				//crear una opcion para limpiar todo y no colocar todo el codigo aqui
			}

		}

		}
	}
	}
	return FALSE;
}

LRESULT CALLBACK VentanaPrincipal(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	switch (msg) {
	case WM_COMMAND: {
		int wmId = LOWORD(wParam);
		switch (wmId) {
		case ID_BTN_Acceder: {

			wchar_t nombreUsuario[255];
			wchar_t nombrePsw[255];

			GetDlgItemText(hWnd, ID_EC_LoginUss, nombreUsuario, 255);
			GetDlgItemText(hWnd, ID_EC_LoginPassword, nombrePsw, 255);

			wstring nombreUsuarioStr(nombreUsuario);
			wstring nombrePswStr(nombrePsw);

			string nombreUsuarioA(nombreUsuarioStr.begin(), nombreUsuarioStr.end());
			string nombrePswA(nombrePswStr.begin(), nombrePswStr.end());

			NODOVET* usuarioEncontrado = verificarUsuario(nombreUsuarioA, nombrePswA);

			if (usuarioEncontrado) {

				//wstring cedulaUsuario = L"Cedula: " + wstring(usuarioEncontrado->veterinario->cedula.begin(), usuarioEncontrado->veterinario->cedula.end());
				//int respuesta = MessageBox(hWnd, cedulaUsuario.c_str(), L"MSG", MB_OK);

				USUARIO nuevoUsuario;
				nuevoUsuario.cedula = usuarioEncontrado->veterinario->cedula;
				sesionesActivas["sesionUsuario"] = nuevoUsuario;


				HWND ventanaUsuario = CreateDialog(
					hInst,
					MAKEINTRESOURCE(ID_DIA_Menu),
					NULL,
					VentanaUsuario);
				ShowWindow(ventanaUsuario, SW_SHOW);

			}

		} break;

		case ID_BTN_RegistrarUss: {
			HWND ventanaRegistroUsser = CreateDialog(
				hInst,
				MAKEINTRESOURCE(ID_DIA_RegistroUss),
				NULL,
				VentanaRegistroUsser);
			ShowWindow(ventanaRegistroUsser, SW_SHOW);
		} break;
		}
	}break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(117);
		break;
	}
	return FALSE;
}

//////

LRESULT CALLBACK VentanaUsuario(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	switch (msg) {

	case WM_INITDIALOG: {

		string cedulaUsuario = cedulaUsuarioActivo();

		NODOCITA* cita = buscarNombre("Nom22", cedulaUsuario);

		if (cita == NULL) {
			return FALSE;
		}

		//Obtén el control de la ListBox
		HWND hListBox = GetDlgItem(hWnd, ID_LBOX_AgendaMenu);

		wstring nombreMascotaStr = L"Nombre de la Mascota: " + wstring(cita->cita->nombreMascota.begin(), cita->cita->nombreMascota.end());
		SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)nombreMascotaStr.c_str());

		wstring fechayhoraStr = L"Fecha y Hora: " + wstring(cita->cita->fechaHora.begin(), cita->cita->fechaHora.end());
		SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)fechayhoraStr.c_str());

		wstring statusCitaStr = L"Estatus: " + wstring(cita->cita->status.begin(), cita->cita->status.end());
		SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)statusCitaStr.c_str());

	}break;

	case WM_COMMAND: {
		int wmId = LOWORD(wParam);

		if (MenuUsuario(wmId, hWnd)) {
			return FALSE;
		}

	}break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(117);
		break;
	}

	return FALSE;
}

LRESULT CALLBACK VentanaRegistro(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	//Aqui va el codigo de la ventana

	switch (msg)
	{
	case WM_INITDIALOG: {

		SendMessage(GetDlgItem(hWnd, ID_COMB_RegCitaEspecie), CB_ADDSTRING, 0, (LPARAM)L"Perro");
		SendMessage(GetDlgItem(hWnd, ID_COMB_RegCitaEspecie), CB_ADDSTRING, 0, (LPARAM)L"Gato");
		SendMessage(GetDlgItem(hWnd, ID_COMB_RegCitaEspecie), CB_ADDSTRING, 0, (LPARAM)L"Ave");
		SendMessage(GetDlgItem(hWnd, ID_COMB_RegCitaEspecie), CB_ADDSTRING, 0, (LPARAM)L"Conejo");

		SendMessage(GetDlgItem(hWnd, ID_COMB_RegCitaStatus), CB_ADDSTRING, 0, (LPARAM)L"Activa");
		SendMessage(GetDlgItem(hWnd, ID_COMB_RegCitaStatus), CB_ADDSTRING, 0, (LPARAM)L"Urgente");

	}break;



	case WM_COMMAND: {
		int wmId = LOWORD(wParam);
		switch (wmId)
		{
		case ID_BTN_RegCitaOK: {

			// Leemos los datos desde el UI
			wchar_t fechayHora[255];
			GetDlgItemText(hWnd, ID_DATE_RegistroFecha, fechayHora, 255);

			wchar_t nombreCliente[255];
			GetDlgItemText(hWnd, ID_EC_RegCitaNombre, nombreCliente, 255);

			wchar_t telefonoCita[255];
			GetDlgItemText(hWnd, ID_EC_RegCitaTel, telefonoCita, 255);

			wchar_t nombreMascota[255];
			GetDlgItemText(hWnd, ID_EC_RegCitaMascota, nombreMascota, 255);

			wchar_t costoCita[255];
			GetDlgItemText(hWnd, ID_EC_RegCitaCosto, costoCita, 255);

			wchar_t motivoCita[255];
			GetDlgItemText(hWnd, ID_EC_RegCitaMotivo, motivoCita, 255);

			wchar_t especie[255];
			GetDlgItemText(hWnd, ID_COMB_RegCitaEspecie, especie, 255);

			wchar_t statusCita[255];
			GetDlgItemText(hWnd, ID_COMB_RegCitaStatus, statusCita, 255);

			// Convertir de wchar_t a wstring
			wstring fechayhoraStr(fechayHora);
			wstring nombreClienteStr(nombreCliente);
			wstring telefonoCitaStr(telefonoCita);
			wstring nombreMascotaStr(nombreMascota);
			wstring costoCitaeStr(costoCita);
			wstring motivoCitaStr(motivoCita);
			wstring especieStr(especie);
			wstring statusCitaStr(statusCita);

			// Convertir de wstring a string
			string fechayHoraA(fechayhoraStr.begin(), fechayhoraStr.end());
			string nombreClienteA(nombreClienteStr.begin(), nombreClienteStr.end());
			string telefonoCitaA(telefonoCitaStr.begin(), telefonoCitaStr.end());
			string nombreMascotaA(nombreMascotaStr.begin(), nombreMascotaStr.end());
			string costoCitaeA(costoCitaeStr.begin(), costoCitaeStr.end());
			string motivoCitaA(motivoCitaStr.begin(), motivoCitaStr.end());
			string especieA(especieStr.begin(), especieStr.end());
			string statusCitaA(statusCitaStr.begin(), statusCitaStr.end());

			// Obtener el costo como float 
			float costoCitaFloat = 0.0f;
			if (costoCita[0] != L'\0') {
				costoCitaFloat = stof(costoCita);
			}

			// Crear la estructura CITA

			string cedulaUsuario = cedulaUsuarioActivo();
			CITA* nuevo = crearCita(nombreClienteA, fechayHoraA, telefonoCitaA, nombreMascotaA, motivoCitaA, costoCitaFloat, especieA, statusCitaA, cedulaUsuario);

			agregarCitaFinal(nuevo);

			// Mostrar el nombre agregado en un messageBox
			wstring message = L"La cita de " + wstring(nombreCliente) + L" se agrego exitosamente!";
			int respuesta = MessageBox(hWnd, message.c_str(), L"Agregado", MB_OK);

			guardarCitasEnArchivo("DatosMiCat.bin");

			if (respuesta == IDOK) {
				// Limpiar el contenido del cuadro de texto
				SetWindowText(GetDlgItem(hWnd, ID_DATE_RegistroFecha), L"");
				SetWindowText(GetDlgItem(hWnd, ID_EC_RegCitaNombre), L"");
				SetWindowText(GetDlgItem(hWnd, ID_EC_RegCitaTel), L"");
				SetWindowText(GetDlgItem(hWnd, ID_EC_RegCitaMascota), L"");
				SetWindowText(GetDlgItem(hWnd, ID_EC_RegCitaCosto), L"");
				SetWindowText(GetDlgItem(hWnd, ID_EC_RegCitaMotivo), L"");
				SetWindowText(GetDlgItem(hWnd, ID_COMB_RegCitaStatus), L"");
				SetWindowText(GetDlgItem(hWnd, ID_COMB_RegCitaEspecie), L"");
				//crear una opcion para limpiar todo y no colocar todo el codigo aqui

			}

		}
							 break;
		}

	}
				   break;

	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(117);
		break;
	}


	return FALSE;
}

LRESULT CALLBACK VentanaEditar(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	switch (msg)
	{
	case WM_COMMAND: {
		int wmId = LOWORD(wParam);
		switch (wmId) {
		case ID_BTN_Edit_BuscarCita: {
			wchar_t nombreCliente[255];
			GetDlgItemText(hWnd, ID_EC_Edit_IngCita, nombreCliente, 255);
			wstring nombreClienteStr(nombreCliente);
			string nombreClienteA(nombreClienteStr.begin(), nombreClienteStr.end());

			string cedulaUsuario = cedulaUsuarioActivo();

			NODOCITA* cita = buscarNombre(nombreClienteA, cedulaUsuario);

			if (cita == NULL) {
				wstring message = L"La cita de " + wstring(nombreCliente) + L" no se encontró";
				int respuesta = MessageBox(hWnd, message.c_str(), L"ERROR", MB_OK);
				return FALSE;
			}

			//Obtén el control de la ListBox
			HWND hListBox = GetDlgItem(hWnd, ID_LBOX_Editar);
			wstring fechayhoraStr = L"Fecha y Hora: " + wstring(cita->cita->fechaHora.begin(), cita->cita->fechaHora.end());
			SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)fechayhoraStr.c_str());

			wstring telefonoCitaStr = L"Telefono: " + wstring(cita->cita->telefonoCita.begin(), cita->cita->telefonoCita.end());
			SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)telefonoCitaStr.c_str());

			wstring nombreMascotaStr = L"Nombre de la Mascota: " + wstring(cita->cita->nombreMascota.begin(), cita->cita->nombreMascota.end());
			SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)nombreMascotaStr.c_str());

			wstring motivoCitaStr = L"Motivo: " + wstring(cita->cita->motivo.begin(), cita->cita->motivo.end());
			SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)motivoCitaStr.c_str());

			wstring statusCitaStr = L"Estatus: " + wstring(cita->cita->status.begin(), cita->cita->status.end());
			SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)statusCitaStr.c_str());

			wstring especieStr = L"Especie: " + wstring(cita->cita->especie.begin(), cita->cita->especie.end());
			SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)especieStr.c_str());

			wstring costoCitaStr = L"Costo: " + to_wstring(cita->cita->costo);
			SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)costoCitaStr.c_str());
		}
								   break;

		case ID_BTN_EditarOK:
		{
			wchar_t nombreCliente[255];
			GetDlgItemText(hWnd, ID_EC_Edit_IngCita, nombreCliente, 255);
			wstring nombreClienteStr(nombreCliente);
			string nombreClienteA(nombreClienteStr.begin(), nombreClienteStr.end());

			string cedulaUsuario = cedulaUsuarioActivo();

			NODOCITA* cita = buscarNombre(nombreClienteA, cedulaUsuario);

			if (cita == NULL) {
				wstring message = L"La cita de " + wstring(nombreCliente) + L" no se encontró";
				int respuesta = MessageBox(hWnd, message.c_str(), L"ERROR", MB_OK);
				return FALSE;

			}

			wchar_t costoCita[255];
			GetDlgItemText(hWnd, ID_EC_EDIT_Costo, costoCita, 255);
			wchar_t motivoCita[255];
			GetDlgItemText(hWnd, ID_EC_EDIT_Motivo, motivoCita, 255);
			wchar_t statusCita[255];
			GetDlgItemText(hWnd, ID_COMB_EDIT_Status, statusCita, 255);

			wstring motivoCitaStr(motivoCita);
			wstring statusCitaStr(statusCita);

			// Convertir de wstring a string
			string motivoCitaA(motivoCitaStr.begin(), motivoCitaStr.end());
			string statusCitaA(statusCitaStr.begin(), statusCitaStr.end());

			// Obtener el costo como float 
			float costoCitaFloat = 0.0f;
			if (costoCita[0] != L'\0') {
				costoCitaFloat = stof(costoCita);
			}

			cita->cita->costo = costoCitaFloat;
			cita->cita->motivo = motivoCitaA;
			cita->cita->status = statusCitaA;

			guardarCitasEnArchivo("DatosMiCat.bin");

			// Mostrar el nombre agregado en un messageBox
			wstring message = L"La cita de " + wstring(nombreCliente) + L" se edito exitosamente!";
			int respuesta = MessageBox(hWnd, message.c_str(), L"Editado", MB_OK);
			HWND hListBox = GetDlgItem(hWnd, ID_LBOX_Editar);

			if (respuesta == IDOK) {
				//	// Limpiar el contenido del cuadro de texto
				//	SetWindowText(GetDlgItem(hWnd, IDC_EDT_NOMBRE), L"");
				//	SetWindowText(GetDlgItem(hWnd, IDC_EDT_COSTO), L"");
				//	SetWindowText(GetDlgItem(hWnd, IDC_EDT_MOTIVO), L"");
				//	SetWindowText(GetDlgItem(hWnd, IDC_EDT_STATUS), L"");
				//	SetWindowText(GetDlgItem(hWnd, IDC_LIST_EDITAR), L"");
				SendMessage(hListBox, LB_RESETCONTENT, 0, 0);
			}

		}
		break;

		case ID_BTN_EditarCANCEL:
		{
		}
		break;
		}

	}
	default:
		break;
	}

	return FALSE;

}

LRESULT CALLBACK VentanaEliminar(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	switch (msg) {

	case WM_COMMAND: {
		int wmId = LOWORD(wParam);

		switch (wmId) {
		case ID_BTN_BuscarCitaEliminar: {

			// Leemos los datos desde el UI
			wchar_t nombreCliente[255];
			GetDlgItemText(hWnd, ID_EC_SelCitaAEliminar, nombreCliente, 255);

			// Convertir de wchar_t a wstring
			wstring nombreClienteStr(nombreCliente);

			// Convertir de wstring a string
			string nombreClienteA(nombreClienteStr.begin(), nombreClienteStr.end());

			string cedulaUsuario = cedulaUsuarioActivo();

			NODOCITA* cita = buscarNombre(nombreClienteA, cedulaUsuario);

			if (cita == nullptr) {
				MessageBox(hWnd, L"No se encontro ninguna cita con ese nombre", L"Busqueda", MB_OK);
				return FALSE;
			}

			// Obtén el control de la ListBox
			HWND hListBox = GetDlgItem(hWnd, ID_LBOX_CitaEliminar); // Reemplaza  con el ID de tu ListBox

			// Convertir datos de la cita a wstring
			//wstring nombreClienteStr = L"Nombre del Cliente: " + wstring(cita->cita->nombreCliente.begin(), cita->cita->nombreCliente.end());
			wstring nombreMascotaStr = L"Nombre de la Mascota: " + wstring(cita->cita->nombreMascota.begin(), cita->cita->nombreMascota.end());

			// Agregar las cadenas a la ListBox
			SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)nombreMascotaStr.c_str());

		}break;
		case ID_BTN_EliminarOK: {
			// Leemos los datos desde el UI
			wchar_t nombreCliente[255];
			GetDlgItemText(hWnd, ID_EC_SelCitaAEliminar, nombreCliente, 255);

			// Convertir de wchar_t a wstring
			wstring nombreClienteStr(nombreCliente);

			// Convertir de wstring a string
			string nombreClienteA(nombreClienteStr.begin(), nombreClienteStr.end());

			string cedulaUsuario = cedulaUsuarioActivo();

			CITA* citaEliminada = EliminarCitaMedio(nombreClienteA, cedulaUsuario);
			if (citaEliminada != nullptr) {
				MessageBox(hWnd, L"Cita eliminada", L"Exito", MB_OK);
				delete citaEliminada;
			}
			else {
				MessageBox(hWnd, L"No se encontro ninguna cita con ese nombre", L"Error", MB_OK);
			}

			guardarCitasEnArchivo("DatosMiCat.bin");

		}break;
		}

	}break;
	default: return FALSE;

	}
	return FALSE;
}

bool MenuUsuario(int opcion, HWND ventana) {

	switch (opcion)
	{

	case ID_MEN_Agenda: {
		HWND ventanaUsuario = CreateDialog(
			hInst,
			MAKEINTRESOURCE(ID_DIA_Agenda),
			NULL,
			VentanaUsuario);
		ShowWindow(ventanaUsuario, SW_SHOW);
	} break;

	case ID_MEN_Anadir: {
		HWND ventanaRegistro = CreateDialog(
			hInst,
			MAKEINTRESOURCE(ID_DIA_RegistrarCita),
			NULL,
			VentanaRegistro);
		ShowWindow(ventanaRegistro, SW_SHOW);
	} break;

	case ID_MEN_Editar: {
		HWND ventanaEditar = CreateDialog(
			hInst,
			MAKEINTRESOURCE(ID_DIA_EditarCita),
			NULL,
			VentanaEditar);
		ShowWindow(ventanaEditar, SW_SHOW);
	} break;

	case ID_MEN_Eliminar: {
		HWND ventanaEliminar = CreateDialog(
			hInst,
			MAKEINTRESOURCE(ID_DIA_EliminarCita),
			NULL,
			VentanaEliminar);
		ShowWindow(ventanaEliminar, SW_SHOW);
	} break;

	case ID_MEN_CerrarSesion: {
		HWND ventanaUsuario = CreateDialog(
			hInst,
			MAKEINTRESOURCE(ID_DIA_CerrarSesion),
			NULL,
			VentanaUsuario);
		ShowWindow(ventanaUsuario, SW_SHOW);
	} break;

	case ID_MEN_Salir: {
		PostQuitMessage(0);
	} break;


	default:
		return FALSE;
		break;
	}
	return true;
}

CITA* crearCita(const string& nombreCliente, const string& fechaHora, const string telefonoCita, const string& nombreMascota,
	const string& motivo, float costo, const string& especie, const string& status, const string& cedula) {
	CITA* nuevo = new CITA;
	nuevo->costo = costo;
	nuevo->fechaHora = fechaHora;
	nuevo->motivo = motivo;
	nuevo->nombreCliente = nombreCliente;
	nuevo->nombreMascota = nombreMascota;
	nuevo->status = status;
	nuevo->cedula = cedula;
	return nuevo;
}

void agregarCitaFinal(CITA* cita) {
	NODOCITA* nodo = nuevoNodo(cita);
	if (LISTACITAS.origen == NULL) {
		LISTACITAS.origen = nodo;
		LISTACITAS.fin = nodo;
		nodo->siguiente = NULL;
	}
	else {
		LISTACITAS.fin->siguiente = nodo;
		LISTACITAS.fin = nodo;
		nodo->siguiente = NULL;
	}
}

NODOCITA* buscarNombre(const string& buscarNomCliente, const string& cedulaUsuario) {
	if (LISTACITAS.origen == NULL)
		return NULL;
	NODOCITA* indice = LISTACITAS.origen;
	while (indice != NULL) {
		if (indice->cita->nombreCliente == buscarNomCliente && indice->cita->cedula == cedulaUsuario)
			break;
		indice = indice->siguiente;
	}
	return indice;
}

NODOCITA* nuevoNodo(CITA* cita) {
	NODOCITA* nodo = new NODOCITA;
	nodo->cita = cita;
	nodo->siguiente = NULL;
	return nodo;
}

CITA* EliminarCitaMedio(const string& nombre, const string& cedulaUsuario) {
	NODOCITA* busqueda = buscarNombre(nombre, cedulaUsuario);
	if (busqueda == nullptr)
		return nullptr;
	if (busqueda == LISTACITAS.origen)
		return EliminarCitaInicio();
	else if (busqueda == LISTACITAS.fin)
		return EliminarCitaFinal();
	NODOCITA* indice = LISTACITAS.origen;
	while (indice != nullptr) {
		if (indice->siguiente == busqueda)
			break;
		indice = indice->siguiente;
	}
	CITA* cita = busqueda->cita;
	if (indice != nullptr)
		indice->siguiente = busqueda->siguiente;
	delete busqueda;
	return cita;
}

CITA* EliminarCitaInicio() {
	if (LISTACITAS.origen == nullptr)
		return nullptr;
	if (LISTACITAS.origen == LISTACITAS.fin)
		LISTACITAS.fin = nullptr;
	NODOCITA* anterior = LISTACITAS.origen;
	LISTACITAS.origen = LISTACITAS.origen->siguiente;
	CITA* cita = anterior->cita;
	delete anterior;
	return cita;
}

CITA* EliminarCitaFinal() {
	if (LISTACITAS.origen == nullptr)
		return nullptr;
	if (LISTACITAS.origen == LISTACITAS.fin)
		return EliminarCitaInicio();
	NODOCITA* indice = LISTACITAS.origen;
	while (indice != nullptr) {
		if (indice->siguiente == LISTACITAS.fin) {
			break;
		}
		indice = indice->siguiente;
	}
	CITA* cita = LISTACITAS.fin->cita;
	delete LISTACITAS.fin;
	LISTACITAS.fin = indice;
	if (indice != nullptr)
		indice->siguiente = nullptr;
	return cita;
}

///////////////////

NODOVET* nuevoNodoVeterinario(VETERINARIO* veterinario) {
	NODOVET* nodo = new NODOVET;
	nodo->veterinario = veterinario;
	nodo->siguiente = nullptr;
	return nodo;
}

VETERINARIO* crearUsuario(const string& nombreVeterinario, const string& cedula, const string& usuario, const string& contrasena) {
	VETERINARIO* nuevo = new VETERINARIO;
	nuevo->nombreVeterinario = nombreVeterinario;
	nuevo->cedula = cedula;
	nuevo->usuario = usuario;
	nuevo->contrasena = contrasena;
	return nuevo;
}

void agregarVeterinarioFinal(VETERINARIO* veterinario) {
	NODOVET* nodo = nuevoNodoVeterinario(veterinario);
	if (LISTAVETERINARIOS.origen == nullptr) {
		LISTAVETERINARIOS.origen = nodo;
		LISTAVETERINARIOS.fin = nodo;
		nodo->siguiente = nullptr;
	}
	else {
		LISTAVETERINARIOS.fin->siguiente = nodo;
		LISTAVETERINARIOS.fin = nodo;
		nodo->siguiente = nullptr;
	}
}

NODOVET* verificarUsuario(const string& usuario, const string& contrasena) {

	if (LISTAVETERINARIOS.origen == nullptr)
		return nullptr;
	NODOVET* indice = LISTAVETERINARIOS.origen;
	while (indice != nullptr) {
		if ((indice->veterinario->usuario == usuario) && (indice->veterinario->contrasena == contrasena)) {
			return indice;
		}
		indice = indice->siguiente;
	}
	return nullptr;
}

void EliminarListaVeterinarios() {
	while (LISTAVETERINARIOS.origen != NULL) {
		NODOVET* temporal = LISTAVETERINARIOS.origen;
		LISTAVETERINARIOS.origen = LISTAVETERINARIOS.origen->siguiente;
		if (temporal->veterinario != NULL)
			delete temporal->veterinario;
		delete temporal;
	}
}

//Archivo de Veterinarios

void guardarUsuariosEnArchivo(const string& nombreArchivo) {
	// Intentar abrir el archivo para escritura binaria
	std::ofstream archivo(nombreArchivo, std::ios::binary);

	if (!archivo.is_open()) {
		// Manejar errores al abrir el archivo
		MessageBox(nullptr, L"No se pudo abrir el archivo para escritura.", L"Error", MB_OK);
		return; // Salir de la función si no se pudo abrir el archivo
	}

	NODOVET* nodo = LISTAVETERINARIOS.origen;

	while (nodo != nullptr) {
		// Comprobar si el nodo o la cita son nulos antes de intentar escribirlos
		if (nodo->veterinario != nullptr) {
			// Intentar escribir el nodo->cita en el archivo
			archivo.write(reinterpret_cast<const char*>(nodo->veterinario), sizeof(VETERINARIO));
		}
		nodo = nodo->siguiente;
	}

	archivo.close(); // Cerrar el archivo después de escribir

	if (archivo.fail()) {
		// Manejar errores al cerrar el archivo
		MessageBox(nullptr, L"No se pudo cerrar el archivo correctamente.", L"Error", MB_OK);
	}
}

void cargarUsuariosDesdeArchivo(const string& nombreArchivo) {
	std::ifstream archivo(nombreArchivo, std::ios::binary);

	if (archivo.is_open()) {
		// Primero, elimina todas las citas existentes en la lista para evitar duplicados
		EliminarListaVeterinarios();

		// Busca el tamaño del archivo para asignar memoria
		archivo.seekg(0, std::ios::end);
		std::streampos fileSize = archivo.tellg();
		archivo.seekg(0, std::ios::beg);

		if (fileSize <= 0) {
			// El archivo está vacío, no hay datos que leer.
			archivo.close();
			return;
		}

		// Crea un buffer para leer los datos
		char* buffer = new char[fileSize];

		// Lee los datos en el buffer
		archivo.read(buffer, fileSize);

		// Cerrar el archivo
		archivo.close();

		// Procesar los datos en el buffer
		const VETERINARIO* vetData = reinterpret_cast<const VETERINARIO*>(buffer);
		size_t numVets = fileSize / sizeof(VETERINARIO);

		for (size_t i = 0; i < numVets; ++i) {
			VETERINARIO* nuevoVet = crearUsuario(vetData[i].nombreVeterinario, vetData[i].cedula, vetData[i].usuario, vetData[i].contrasena);
			agregarVeterinarioFinal(nuevoVet);
		}
		// Liberar la memoria del buffer
		delete[] buffer;
	}
	else {
		MessageBox(nullptr, L"No se pudo abrir el archivo para lectura.", L"Error", MB_OK);
	}
}

string cedulaUsuarioActivo() {

	string cedulaUsuario;
	auto it = sesionesActivas.find("sesionUsuario");
	if (it == sesionesActivas.end())
		cedulaUsuario = "";

	USUARIO& usuario = it->second;
	cedulaUsuario = usuario.cedula;
	return cedulaUsuario;

}

///////////////////

void EliminarListaCita() {
	while (LISTACITAS.origen != NULL) {
		NODOCITA* temporal = LISTACITAS.origen;
		LISTACITAS.origen = LISTACITAS.origen->siguiente;
		if (temporal->cita != NULL)
			delete temporal->cita;
		delete temporal;
	}
}

void guardarCitasEnArchivo(const char* nombreArchivo) {
	// Intentar abrir el archivo para escritura binaria
	std::ofstream archivo(nombreArchivo, std::ios::binary);

	if (!archivo.is_open()) {
		// Manejar errores al abrir el archivo
		MessageBox(nullptr, L"No se pudo abrir el archivo para escritura.", L"Error", MB_OK);
		return; // Salir de la función si no se pudo abrir el archivo
	}

	NODOCITA* nodo = LISTACITAS.origen;

	while (nodo != nullptr) {
		// Comprobar si el nodo o la cita son nulos antes de intentar escribirlos
		if (nodo->cita != nullptr) {
			// Intentar escribir el nodo->cita en el archivo
			archivo.write(reinterpret_cast<const char*>(nodo->cita), sizeof(CITA));
		}
		nodo = nodo->siguiente;
	}

	archivo.close(); // Cerrar el archivo después de escribir

	if (archivo.fail()) {
		// Manejar errores al cerrar el archivo
		MessageBox(nullptr, L"No se pudo cerrar el archivo correctamente.", L"Error", MB_OK);
	}
}

void cargarCitasDesdeArchivo(const char* nombreArchivo) {
	std::ifstream archivo(nombreArchivo, std::ios::binary);

	if (archivo.is_open()) {
		// Primero, elimina todas las citas existentes en la lista para evitar duplicados
		EliminarListaCita();

		// Busca el tamaño del archivo para asignar memoria
		archivo.seekg(0, std::ios::end);
		std::streampos fileSize = archivo.tellg();
		archivo.seekg(0, std::ios::beg);

		if (fileSize <= 0) {
			// El archivo está vacío, no hay datos que leer.
			archivo.close();
			return;
		}

		// Crea un buffer para leer los datos
		char* buffer = new char[fileSize];

		// Lee los datos en el buffer
		archivo.read(buffer, fileSize);

		// Cerrar el archivo
		archivo.close();

		// Procesar los datos en el buffer
		const CITA* citaData = reinterpret_cast<const CITA*>(buffer);
		size_t numCitas = fileSize / sizeof(CITA);

		for (size_t i = 0; i < numCitas; ++i) {
			CITA* nuevaCita = crearCita(citaData[i].nombreCliente, citaData[i].fechaHora, citaData[i].telefonoCita, citaData[i].nombreMascota,
				citaData[i].motivo, citaData[i].costo, citaData[i].especie, citaData[i].status, citaData[i].cedula);
			agregarCitaFinal(nuevaCita);
		}
		// Liberar la memoria del buffer
		delete[] buffer;
	}
	else {
		MessageBox(nullptr, L"No se pudo abrir el archivo para lectura.", L"Error", MB_OK);
	}
}



