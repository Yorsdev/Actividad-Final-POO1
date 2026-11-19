#ifndef NEGOCIO_H
#define NEGOCIO_H

#include <iostream>
#include <iomanip>
#include <string>
#include "Config.h"
#include "Carga.h"
#include "Medidor.h"
#include "Mercado.h"

using namespace std;

class Negocio
{
private:
    string nombre;
    string responsable;
    Medidor medidor;
    Carga *cargas[MAX_CARGAS];
    int numCargas;
    Mercado *mercado;

    // Variables para simulacion de 1 dia (EA2)
    float costoConDia;
    float costoSinDia;
    int horasSimuladasDia;
    int fallasLecturaDia;

    // Estructuras para simulacion semanal real (EA3)
    float costos[DIAS_SEMANA][HORAS_DIA];
    float costosSinGestor[DIAS_SEMANA][HORAS_DIA];
    int horasUsoDiario[MAX_CARGAS][DIAS_SEMANA];
    bool semanaSimulada;
    int totalFallasSemana;

public:
    Negocio(string nomNegocio, string nomResponsable, Mercado *m)
        : nombre(nomNegocio), responsable(nomResponsable), mercado(m),
          numCargas(0), costoConDia(0.0f), costoSinDia(0.0f),
          horasSimuladasDia(0), fallasLecturaDia(0),
          semanaSimulada(false), totalFallasSemana(0)
    {
        reiniciarMatricesSemana();
    }

    void reiniciarMatricesSemana()
    {
        for (int d = 0; d < DIAS_SEMANA; d++)
        {
            for (int h = 0; h < HORAS_DIA; h++)
            {
                costos[d][h] = 0.0f;
                costosSinGestor[d][h] = 0.0f;
            }
            for (int c = 0; c < MAX_CARGAS; c++)
            {
                horasUsoDiario[c][d] = 0;
            }
        }
    }

    void agregarCarga(Carga *c)
    {
        if (numCargas < MAX_CARGAS)
        {
            cargas[numCargas] = c;
            numCargas++;
        }
    }

    // Evaluacion puntual de una hora especifica
    void evaluar(int hora, float precio, float embalses)
    {
        float potenciaTotal = 0.0f;

        for (int i = 0; i < numCargas; i++)
        {
            if (cargas[i]->debeEncender(hora, precio, embalses))
            {
                cargas[i]->encender();
                potenciaTotal += cargas[i]->getPotenciaKW();
            }
            else
            {
                cargas[i]->apagar();
            }
        }

        if (potenciaTotal > POTENCIA_CONTRATADA)
        {
            cout << "\n [ALERTA!]: Potencia encendida (" << potenciaTotal
                 << " kW) supera el limite contratado (" << POTENCIA_CONTRATADA << " kW)." << endl;
            cout << " -> Apagando la carga aplazable de mayor consumo (Horno Industrial)..." << endl;

            for (int i = 0; i < numCargas; i++)
            {
                if (cargas[i]->getNombre() == "Horno Industrial" || cargas[i]->getNombre() == "Horno")
                {
                    if (cargas[i]->isEncendida())
                    {
                        cargas[i]->apagar();
                        potenciaTotal -= cargas[i]->getPotenciaKW();
                    }
                    break;
                }
            }
        }

        cout << "\n--- Evaluacion puntual (Hora: " << hora << "h | Precio: $"
             << precio << " | Embalses: " << embalses << "%) ---" << endl;

        for (int i = 0; i < numCargas; i++)
        {
            cout << "  " << setw(18) << left << cargas[i]->getNombre() << right
                 << (cargas[i]->isEncendida() ? "ENCENDER (ON)" : "APAGAR / OFF") << endl;
        }
    }

    // Simulacion de 1 dia usando datos reales del dia seleccionado (0 = Lunes ... 6 = Domingo)
    void simularDia(int diaSeleccionado = 0)
    {
        medidor.reiniciar();
        costoConDia = 0.0f;
        costoSinDia = 0.0f;
        horasSimuladasDia = 0;
        fallasLecturaDia = 0;

        for (int i = 0; i < numCargas; i++)
        {
            cargas[i]->reiniciarHoras();
        }

        float embalses = mercado->nivelEmbalses();
        float potenciaTotalInstalada = POTENCIA_REFRIGERADOR + POTENCIA_HORNO + POTENCIA_AMASADORA;

        cout << "\n============= SIMULACION DIARIA: " << NOMBRES_DIAS[diaSeleccionado] << " =============" << endl;
        cout << setw(6) << "Hora" << setw(10) << "Precio" << setw(10) << "Embalse"
             << setw(14) << "Refri" << setw(10) << "Horno" << setw(12) << "Amasadora"
             << setw(14) << "Costo Hora" << endl;
        cout << "--------------------------------------------------------------------------" << endl;

        for (int h = 0; h < HORAS_DIA; h++)
        {
            float precio = mercado->precio(diaSeleccionado, h);

            if (h > 0 && h % FALLA_CADA == 0)
            {
                fallasLecturaDia++;
                cout << setw(6) << h << "  [FALLA DE LECTURA] - Solo se mantiene Carga Critica" << endl;
                cargas[0]->encender();
                cargas[1]->apagar();
                cargas[2]->apagar();

                costoConDia += cargas[0]->costoHora(precio);
                costoSinDia += potenciaTotalInstalada * precio;
                horasSimuladasDia++;
                continue;
            }

            if (precio > PRECIO_ESCASEZ && embalses < EMBALSES_ALERTA)
            {
                cout << "\n!!! EMERGENCIA EN HORA " << h << " !!!" << endl;
                cout << "Precio critico ($" << precio << ") y Embalses en alerta (" << embalses << "%)." << endl;
                break;
            }

            float potenciaEncendida = 0.0f;
            for (int i = 0; i < numCargas; i++)
            {
                if (cargas[i]->debeEncender(h, precio, embalses))
                {
                    cargas[i]->encender();
                    potenciaEncendida += cargas[i]->getPotenciaKW();
                }
                else
                {
                    cargas[i]->apagar();
                }
            }

            if (potenciaEncendida > POTENCIA_CONTRATADA && cargas[1]->isEncendida())
            {
                cargas[1]->apagar();
                cout << "  -> [ALERTA POTENCIA HORA " << h << "]: Limite superado ("
                     << potenciaEncendida << " kW). Horno Apagado." << endl;
            }

            float costoHoraActual = 0.0f;
            float potenciaHoraCon = 0.0f;

            for (int i = 0; i < numCargas; i++)
            {
                if (cargas[i]->isEncendida())
                {
                    costoHoraActual += cargas[i]->costoHora(precio);
                    potenciaHoraCon += cargas[i]->getPotenciaKW();
                }
            }

            medidor.registrarConsumo(potenciaHoraCon, precio);
            costoConDia += costoHoraActual;
            costoSinDia += (potenciaTotalInstalada * precio);
            horasSimuladasDia++;

            cout << setw(6) << h << setw(10) << fixed << setprecision(0) << precio
                 << setw(9) << setprecision(1) << embalses << "%"
                 << setw(14) << (cargas[0]->isEncendida() ? "ON" : "OFF")
                 << setw(10) << (cargas[1]->isEncendida() ? "ON" : "OFF")
                 << setw(12) << (cargas[2]->isEncendida() ? "ON" : "OFF")
                 << setw(14) << setprecision(0) << costoHoraActual << endl;
        }
        cout << "======================= SIMULACION DIARIA FINALIZADA =====================\n"
             << endl;
    }

    // Resumen de la ultima simulacion de 1 dia
    void resumenDia() const
    {
        if (horasSimuladasDia == 0)
        {
            cout << "\n[!] No hay simulacion diaria registrada aun." << endl;
            return;
        }

        cout << fixed << setprecision(0);
        cout << "\n=======================================================" << endl;
        cout << "         RESUMEN DIARIO DE OPERACION Y AHORRO          " << endl;
        cout << "=======================================================" << endl;
        cout << "Negocio: " << nombre << " | Resp: " << responsable << endl;
        cout << "Horas Simuladas: " << horasSimuladasDia << " / " << HORAS_DIA
             << " | Fallas de Lectura: " << fallasLecturaDia << endl;
        cout << "-------------------------------------------------------" << endl;

        for (int i = 0; i < numCargas; i++)
        {
            cout << "  * " << setw(18) << left << cargas[i]->getNombre()
                 << right << cargas[i]->getHorasUso() << " h usadas ("
                 << setprecision(1) << cargas[i]->consumoKWh() << " kWh)" << endl;
        }

        cout << "-------------------------------------------------------" << endl;
        cout << setprecision(0);
        cout << "Costo TOTAL Sin Gestor:  $" << costoSinDia << " COP" << endl;
        cout << "Costo TOTAL Con Gestor:  $" << costoConDia << " COP" << endl;

        float ahorro = costoSinDia - costoConDia;
        float pct = (costoSinDia > 0) ? (ahorro / costoSinDia) * 100.0f : 0.0f;
        cout << "AHORRO ALCANZADO:        $" << ahorro << " COP ("
             << setprecision(1) << pct << " %)" << endl;
        cout << "=======================================================\n"
             << endl;
    }

    // Simulacion completa de la semana real (7 dias x 24 horas)
    void simularSemana()
    {
        medidor.reiniciar();
        reiniciarMatricesSemana();
        totalFallasSemana = 0;

        for (int i = 0; i < numCargas; i++)
        {
            cargas[i]->reiniciarHoras();
        }

        float embalses = mercado->nivelEmbalses();
        float potenciaTotalInstalada = POTENCIA_REFRIGERADOR + POTENCIA_HORNO + POTENCIA_AMASADORA;

        for (int d = 0; d < DIAS_SEMANA; d++)
        {
            for (int h = 0; h < HORAS_DIA; h++)
            {
                float precio = mercado->precio(d, h);

                // Falla periodica de lectura cada 20 horas
                if (h > 0 && h % FALLA_CADA == 0)
                {
                    totalFallasSemana++;
                    cargas[0]->encender();
                    cargas[1]->apagar();
                    cargas[2]->apagar();

                    horasUsoDiario[0][d]++;
                    costos[d][h] = cargas[0]->costoHora(precio);
                    costosSinGestor[d][h] = medidor.calcularCosto(potenciaTotalInstalada, precio);
                    medidor.registrarConsumo(cargas[0]->getPotenciaKW(), precio);
                    continue;
                }

                // Evaluacion polimorfica de cada carga (Reglas 1 a 5)
                float potenciaEncendida = 0.0f;
                for (int i = 0; i < numCargas; i++)
                {
                    if (cargas[i]->debeEncender(h, precio, embalses))
                    {
                        cargas[i]->encender();
                        potenciaEncendida += cargas[i]->getPotenciaKW();
                    }
                    else
                    {
                        cargas[i]->apagar();
                    }
                }

                // Regla 6: Control de potencia contratada (9.0 kW)
                if (potenciaEncendida > POTENCIA_CONTRATADA && cargas[1]->isEncendida())
                {
                    cargas[1]->apagar();
                }

                // Registro de costos y horas de operacion del dia
                float costoHora = 0.0f;
                float potenciaCon = 0.0f;
                for (int i = 0; i < numCargas; i++)
                {
                    if (cargas[i]->isEncendida())
                    {
                        costoHora += cargas[i]->costoHora(precio);
                        potenciaCon += cargas[i]->getPotenciaKW();
                        horasUsoDiario[i][d]++;
                    }
                }

                costos[d][h] = costoHora;
                costosSinGestor[d][h] = medidor.calcularCosto(potenciaTotalInstalada, precio);
                medidor.registrarConsumo(potenciaCon, precio);
            }
        }

        semanaSimulada = true;
        cout << "\n[OK] Simulacion de la semana real (21 al 27 Sep 2026 - 168 horas) completada." << endl;
    }

    // Usa la sobrecarga de Medidor para sumar el arreglo de 24 horas de un dia
    float costoDia(int dia) const
    {
        if (dia < 0 || dia >= DIAS_SEMANA)
            return 0.0f;
        return medidor.calcularCosto(costos[dia], HORAS_DIA);
    }

    float costoDiaSinGestor(int dia) const
    {
        if (dia < 0 || dia >= DIAS_SEMANA)
            return 0.0f;
        return medidor.calcularCosto(costosSinGestor[dia], HORAS_DIA);
    }

    float costoSemana() const
    {
        float total = 0.0f;
        for (int d = 0; d < DIAS_SEMANA; d++)
        {
            total += costoDia(d);
        }
        return total;
    }

    float costoSemanaSinGestor() const
    {
        float total = 0.0f;
        for (int d = 0; d < DIAS_SEMANA; d++)
        {
            total += costoDiaSinGestor(d);
        }
        return total;
    }

    int diaMasCaroOperacion() const
    {
        int diaMayor = 0;
        float maxCosto = costoDia(0);
        for (int d = 1; d < DIAS_SEMANA; d++)
        {
            float actual = costoDia(d);
            if (actual > maxCosto)
            {
                maxCosto = actual;
                diaMayor = d;
            }
        }
        return diaMayor;
    }

    void horaMasCaraSemana(int &diaPico, int &horaPico, float &maxPrecio) const
    {
        diaPico = 0;
        horaPico = 0;
        maxPrecio = mercado->precio(0, 0);

        for (int d = 0; d < DIAS_SEMANA; d++)
        {
            for (int h = 0; h < HORAS_DIA; h++)
            {
                float p = mercado->precio(d, h);
                if (p > maxPrecio)
                {
                    maxPrecio = p;
                    diaPico = d;
                    horaPico = h;
                }
            }
        }
    }

    void reporteSemanal() const
    {
        if (!semanaSimulada)
        {
            cout << "\n[!] Primero ejecute la simulacion de la semana real en el menu." << endl;
            return;
        }

        int diaPicoBolsa = 0, horaPicoBolsa = 0;
        float maxPrecioBolsa = 0.0f;
        horaMasCaraSemana(diaPicoBolsa, horaPicoBolsa, maxPrecioBolsa);

        int diaMayorGasto = diaMasCaroOperacion();
        int diasMetaHorno = 0;
        int diasMetaAmasadora = 0;

        cout << fixed << setprecision(0);
        cout << "\n====================================================================================" << endl;
        cout << "                REPORTE EJECUTIVO SEMANAL - WATTCONTROL (21-27 SEP 2026)            " << endl;
        cout << "====================================================================================" << endl;
        cout << "Negocio: " << nombre << " | Responsable: " << responsable << endl;
        cout << "Nivel de Embalses: " << setprecision(1) << mercado->nivelEmbalses()
             << "% | Fallas de lectura gestionadas: " << totalFallasSemana << endl;
        cout << "------------------------------------------------------------------------------------" << endl;
        cout << setw(14) << left << "Dia"
             << setw(16) << right << "Sin Gestor"
             << setw(16) << "Con Gestor"
             << setw(15) << "Ahorro COP"
             << setw(11) << "Horno"
             << setw(12) << "Amasadora" << endl;
        cout << "------------------------------------------------------------------------------------" << endl;

        for (int d = 0; d < DIAS_SEMANA; d++)
        {
            float sinG = costoDiaSinGestor(d);
            float conG = costoDia(d);
            float ahorroD = sinG - conG;
            int hHorno = horasUsoDiario[1][d];
            int hAmasa = horasUsoDiario[2][d];

            if (hHorno >= HORAS_MIN_HORNO)
                diasMetaHorno++;
            if (hAmasa >= HORAS_MIN_AMASADORA)
                diasMetaAmasadora++;

            cout << setprecision(0)
                 << setw(14) << left << NOMBRES_DIAS[d]
                 << setw(15) << right << "$" << sinG
                 << setw(15) << "$" << conG
                 << setw(14) << "$" << ahorroD
                 << setw(8) << hHorno << "h/" << HORAS_MIN_HORNO << "h"
                 << setw(9) << hAmasa << "h/" << HORAS_MIN_AMASADORA << "h" << endl;
        }

        float totalSin = costoSemanaSinGestor();
        float totalCon = costoSemana();
        float ahorroSemanal = totalSin - totalCon;
        float ahorroMensual = ahorroSemanal * 4.0f;
        float pctAhorro = (totalSin > 0) ? (ahorroSemanal / totalSin) * 100.0f : 0.0f;

        cout << "------------------------------------------------------------------------------------" << endl;
        cout << "ANALISIS DE PICOS DE MERCADO Y OPERACION:" << endl;
        cout << "  * Dia mas caro para la panaderia:  " << NOMBRES_DIAS[diaMayorGasto]
             << " ($" << costoDia(diaMayorGasto) << " COP)" << endl;
        cout << "  * Hora mas cara de la semana (XM): " << NOMBRES_DIAS[diaPicoBolsa]
             << " a las " << horaPicoBolsa << ":00h ($" << maxPrecioBolsa << " COP/kWh)" << endl;

        cout << "------------------------------------------------------------------------------------" << endl;
        cout << "CUMPLIMIENTO DE HORAS MINIMAS DE PRODUCCION:" << endl;
        cout << "  * Horno Industrial (min " << HORAS_MIN_HORNO << "h/dia): "
             << diasMetaHorno << " de " << DIAS_SEMANA << " dias cumplidos." << endl;
        cout << "  * Amasadora        (min " << HORAS_MIN_AMASADORA << "h/dia): "
             << diasMetaAmasadora << " de " << DIAS_SEMANA << " dias cumplidos." << endl;

        cout << "------------------------------------------------------------------------------------" << endl;
        cout << "BALANCE FINANCIERO SEMANAL Y PROYECCION:" << endl;
        cout << "  * Gasto Total SIN Gestor:        $" << totalSin << " COP" << endl;
        cout << "  * Gasto Total CON Gestor:        $" << totalCon << " COP" << endl;
        cout << "  * AHORRO SEMANAL ALCANZADO:      $" << ahorroSemanal << " COP ("
             << setprecision(1) << pctAhorro << "%)" << endl;
        cout << setprecision(0);
        cout << "  * PROYECCION AHORRO MENSUAL (x4): $" << ahorroMensual << " COP" << endl;
        cout << "====================================================================================\n"
             << endl;
    }
};

#endif