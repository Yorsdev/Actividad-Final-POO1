#include <iostream>
#include <string>
#include <limits>
#include "Config.h"
#include "Carga.h"
#include "Medidor.h"
#include "Mercado.h"
#include "Negocio.h"

using namespace std;

string leerTextoValido(const string &etiqueta)
{
    string texto;
    bool valido = false;

    do
    {
        cout << etiqueta;
        getline(cin, texto);

        if (texto.empty())
        {
            cout << "  [!] El texto no puede estar vacio." << endl;
        }
        else if (texto.length() < 3)
        {
            cout << "  [!] Debe tener minimo 3 caracteres." << endl;
        }
        else if (texto.at(0) < 'A' || texto.at(0) > 'Z')
        {
            cout << "  [!] La primera letra debe ser mayuscula (A-Z)." << endl;
        }
        else
        {
            valido = true;
        }
    } while (!valido);

    return texto;
}

int main()
{
    cout << "======================================================" << endl;
    cout << "       WATTCONTROL v3 - GESTOR INTELIGENTE DE ENERGIA " << endl;
    cout << "======================================================" << endl;

    string nombreNegocio = leerTextoValido("Ingrese el nombre de la panaderia: ");
    string nombreResponsable = leerTextoValido("Ingrese el nombre del responsable: ");

    Mercado mercado(78.0f);

    CargaCritica refrigerador("Refrigerador", POTENCIA_REFRIGERADOR);
    CargaAplazable horno("Horno Industrial", POTENCIA_HORNO);
    CargaAplazable amasadora("Amasadora", POTENCIA_AMASADORA);

    Negocio panaderia(nombreNegocio, nombreResponsable, &mercado);
    panaderia.agregarCarga(&refrigerador);
    panaderia.agregarCarga(&horno);
    panaderia.agregarCarga(&amasadora);

    int horaManual = 12;
    float precioManual = 500.0f;
    float embalsesManual = 78.0f;
    int opcion = -1;

    do
    {
        cout << "\n================ MENU PRINCIPAL ================" << endl;
        cout << "1. Ingresar lectura manual (hora, precio, embalses)" << endl;
        cout << "2. Evaluar decision puntual de cada carga" << endl;
        cout << "3. Simular un dia real (24 h)" << endl;
        cout << "4. Ver resumen del dia simulado" << endl;
        cout << "5. Simular la semana real (21 al 27 Sep 2026)" << endl;
        cout << "6. Ver reporte ejecutivo semanal" << endl;
        cout << "7. Cambiar nivel de embalses del mercado (Actual: "
             << mercado.nivelEmbalses() << "%)" << endl;
        cout << "0. Salir" << endl;
        cout << "Seleccione una opcion: ";

        if (!(cin >> opcion))
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "\n[!] Entrada invalida. Ingrese un numero del menu." << endl;
            opcion = -1;
            continue;
        }

        switch (opcion)
        {
        case 1:
        {
            cout << "\n--- INGRESO DE LECTURA MANUAL ---" << endl;
            do
            {
                cout << "Hora (0 - 23): ";
                cin >> horaManual;
            } while (horaManual < 0 || horaManual > 23);

            do
            {
                cout << "Precio energia (COP/kWh > 0): ";
                cin >> precioManual;
            } while (precioManual <= 0);

            do
            {
                cout << "Nivel de embalses (0 - 100%): ";
                cin >> embalsesManual;
            } while (embalsesManual < 0 || embalsesManual > 100);

            cout << "-> Lectura manual guardada." << endl;
            break;
        }
        case 2:
            panaderia.evaluar(horaManual, precioManual, embalsesManual);
            break;

        case 3:
        {
            int dia = 0;
            do
            {
                cout << "Seleccione dia (0=Lun, 1=Mar, 2=Mie, 3=Jue, 4=Vie, 5=Sab, 6=Dom): ";
                cin >> dia;
            } while (dia < 0 || dia >= DIAS_SEMANA);

            panaderia.simularDia(dia);
            break;
        }
        case 4:
            panaderia.resumenDia();
            break;

        case 5:
            panaderia.simularSemana();
            break;

        case 6:
            panaderia.reporteSemanal();
            break;

        case 7:
        {
            float nuevoNivel = 78.0f;
            do
            {
                cout << "Nuevo nivel de embalses (0 - 100%): ";
                cin >> nuevoNivel;
            } while (nuevoNivel < 0.0f || nuevoNivel > 100.0f);

            mercado.setNivelEmbalses(nuevoNivel);
            cout << "-> Embalses actualizados a " << mercado.nivelEmbalses() << "%." << endl;
            break;
        }
        case 0:
            cout << "\nCerrando WattControl. Hasta pronto!" << endl;
            break;

        default:
            cout << "\n[!] Opcion no valida." << endl;
            break;
        }
    } while (opcion != 0);

    return 0;
}