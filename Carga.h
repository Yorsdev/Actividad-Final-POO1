#ifndef CARGA_H
#define CARGA_H

#include <string>
#include "Config.h"

using namespace std;

class Carga
{
protected:
    string nombre;
    float potenciaKW;
    bool encendida;
    int horasUso;

public:
    Carga(string n, float p) : nombre(n), potenciaKW(p), encendida(false), horasUso(0) {}
    virtual ~Carga() {}

    virtual bool debeEncender(int hora, float precio, float embalses) const = 0;

    void encender()
    {
        encendida = true;
        horasUso++;
    }

    void apagar()
    {
        encendida = false;
    }

    void reiniciarHoras()
    {
        horasUso = 0;
        encendida = false;
    }

    float costoHora(float precio) const
    {
        return encendida ? (potenciaKW * precio) : 0.0f;
    }

    float consumoKWh() const
    {
        return horasUso * potenciaKW;
    }

    string getNombre() const { return nombre; }
    float getPotenciaKW() const { return potenciaKW; }
    bool isEncendida() const { return encendida; }
    int getHorasUso() const { return horasUso; }
};

class CargaCritica : public Carga
{
public:
    CargaCritica(string n, float p) : Carga(n, p) {}

    bool debeEncender(int hora, float precio, float embalses) const override
    {
        return true;
    }
};

class CargaAplazable : public Carga
{
public:
    CargaAplazable(string n, float p) : Carga(n, p) {}

    bool debeEncender(int hora, float precio, float embalses) const override
    {
        bool esHoraPico = (hora >= PICO_INICIO && hora <= PICO_FIN);
        bool precioElevado = (precio > PRECIO_CARO);
        bool embalsesBajos = (embalses < EMBALSES_ALERTA);

        return !(esHoraPico || precioElevado || embalsesBajos);
    }
};

#endif