#ifndef MEDIDOR_H
#define MEDIDOR_H

class Medidor
{
private:
    float kWhAcumulados;
    float costoAcumulado;

public:
    Medidor() : kWhAcumulados(0.0f), costoAcumulado(0.0f) {}

    void registrarConsumo(float kw, float precio)
    {
        kWhAcumulados += kw;
        costoAcumulado += calcularCosto(kw, precio);
    }

    void reiniciar()
    {
        kWhAcumulados = 0.0f;
        costoAcumulado = 0.0f;
    }

    // Sobrecarga 1: Calcula el costo de una hora especifica
    float calcularCosto(float kw, float precio) const
    {
        return kw * precio;
    }

    // Sobrecarga 2: Suma el costo acumulado de un arreglo de N horas
    float calcularCosto(const float costosHoras[], int cantidadHoras) const
    {
        float total = 0.0f;
        for (int i = 0; i < cantidadHoras; i++)
        {
            total += costosHoras[i];
        }
        return total;
    }

    float getKWh() const { return kWhAcumulados; }
    float getCosto() const { return costoAcumulado; }
};

#endif