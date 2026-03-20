#include <iostream>
#include <string>

using namespace std;

// --- ESTRUCTURAS ---
struct Torre {
    int id;
    string nombre;
    int posicion;
    int danio;
    int rango;
};

struct Enemigo {
    int id;
    int vida;
    int velocidad;
    int posicion;
    Enemigo *sig, *ant;
};

struct Oleada {
    int idOleada;
    int cantidadEnemigos;
    int vidaBase;
    int velocidadBase;
    Oleada* sig;
};

// --- 1. LISTA SECUENCIAL (TORRES) ---
class ListaTorres {
    Torre* torres;
    int capacidad;
    int cantidad;
public:
    ListaTorres(int cap = 10) {
        capacidad = cap;
        torres = new Torre[capacidad];
        cantidad = 0;
    }
    void insertar(Torre t) { if (cantidad < capacidad) torres[cantidad++] = t; }
    int getCantidad() { return cantidad; }
    Torre getTorre(int i) { return torres[i]; }
    void mostrar() {
        cout << "  [ Torres: ";
        for(int i=0; i<cantidad; i++) cout << torres[i].nombre << "(Posicion:" << torres[i].posicion << ") ";
        cout << "]" << endl;
    }
};

// --- 2. LISTA DOBLE (ENEMIGOS) ---
class ListaEnemigos {
    Enemigo *primero, *ultimo;
public:
    ListaEnemigos() { primero = NULL; ultimo = NULL; }

    void insertarAlFinal(int id, int vida, int vel) {
        Enemigo* nuevo = new Enemigo;
        nuevo->id = id; nuevo->vida = vida; nuevo->velocidad = vel;
        nuevo->posicion = 0; nuevo->sig = NULL; nuevo->ant = ultimo;
        if (ultimo != NULL) ultimo->sig = nuevo;
        else primero = nuevo;
        ultimo = nuevo;
    }

    void eliminarEnemigo(Enemigo* nodo) {
        if (nodo == NULL) return;
        if (nodo->ant != NULL) nodo->ant->sig = nodo->sig;
        else primero = nodo->sig;
        if (nodo->sig != NULL) nodo->sig->ant = nodo->ant;
        else ultimo = nodo->ant;
        delete nodo;
    }

    void moverEnemigos(int &vidas) {
        Enemigo* actual = primero;
        while (actual != NULL) {
            actual->posicion += actual->velocidad;
            Enemigo* aux = actual;
            actual = actual->sig;
            if (aux->posicion >= 20) { 
                vidas--;
                eliminarEnemigo(aux);
                cout << "  >> !ALERTA! Un enemigo cruzo la base. Vidas -1" << endl;
            }
        }
    }

    void atacar(int posT, int rangoT, int danioT) {
        Enemigo* actual = primero;
        while (actual != NULL) {
            if (actual->posicion >= (posT - rangoT) && actual->posicion <= (posT + rangoT)) {
                actual->vida -= danioT;
                if (actual->vida <= 0) {
                    Enemigo* morir = actual;
                    actual = actual->sig;
                    eliminarEnemigo(morir);
                    cout << "  >> !Enemigo destruido por torre en posicion " << posT << "!" << endl;
                    continue;
                }
            }
            actual = actual->sig;
        }
    }

    void mostrarEstatus() {
        if (primero == NULL) {
            cout << "  [ Campo despejado ]" << endl;
            return;
        }
        Enemigo* actual = primero;
        cout << "  [ Enemigos: ";
        while (actual != NULL) {
            cout << "Enemigo" << actual->id << "(Posicion:" << actual->posicion << " HP:" << actual->vida << ") ";
            actual = actual->sig;
        }
        cout << "]" << endl;
    }
};

// --- 3. LISTA CIRCULAR (OLEADAS) ---
class ListaOleadas {
    Oleada* ultimo;
public:
    ListaOleadas() { ultimo = NULL; }
    void registrar(int id, int cant, int v, int vel) {
        Oleada* nuevo = new Oleada;
        nuevo->idOleada = id; nuevo->cantidadEnemigos = cant;
        nuevo->vidaBase = v; nuevo->velocidadBase = vel;
        if (ultimo == NULL) { nuevo->sig = nuevo; ultimo = nuevo; }
        else { nuevo->sig = ultimo->sig; ultimo->sig = nuevo; ultimo = nuevo; }
    }
    Oleada* siguiente(Oleada* actual) {
        if (ultimo == NULL) return NULL;
        return (actual == NULL) ? ultimo->sig : actual->sig;
    }
};

// --- FUNCION DE INTERFAZ (CORREGIDA SIN TO_STRING) ---
void mostrarHUD(int vidas, int oleadaID, ListaTorres &t, ListaEnemigos &e) {
    cout << "\n====================================================" << endl;
    cout << "   VIDAS JUGADOR: " << vidas << " | OLEADA ACTUAL: ";
    if (oleadaID == 0) cout << "Ninguna"; else cout << oleadaID;
    cout << "\n----------------------------------------------------" << endl;
    t.mostrar();
    e.mostrarEstatus();
    cout << "====================================================" << endl;
}

int main() {
    ListaTorres misTorres;
    ListaEnemigos misEnemigos;
    ListaOleadas misOleadas;
    Oleada* oleadaActual = NULL;
    int vidas = 3; 
    int opcion, numEnemigosID = 0;

    misOleadas.registrar(1, 3, 50, 1);
    misOleadas.registrar(2, 2, 40, 2);

    do {
        mostrarHUD(vidas, (oleadaActual ? oleadaActual->idOleada : 0), misTorres, misEnemigos);
        
        cout << "\n1. Torre Arquero (Posicion:3)\n2. Torre Canion (Posicion:8)\n3. Siguiente Oleada\n4. AVANZAR TURNO\n5. Salir\nOpcion: ";
        cin >> opcion;

        if (opcion == 1) misTorres.insertar({1, "Arquero", 3, 20, 2});
        else if (opcion == 2) misTorres.insertar({2, "Canion", 8, 35, 3});
        else if (opcion == 3) {
            oleadaActual = misOleadas.siguiente(oleadaActual);
            if (oleadaActual != NULL) {
                for (int i = 0; i < oleadaActual->cantidadEnemigos; i++) {
                    numEnemigosID++;
                    misEnemigos.insertarAlFinal(numEnemigosID, oleadaActual->vidaBase, oleadaActual->velocidadBase);
                }
            }
        } else if (opcion == 4) {
            misEnemigos.moverEnemigos(vidas);
            for (int i = 0; i < misTorres.getCantidad(); i++) {
                Torre t = misTorres.getTorre(i);
                misEnemigos.atacar(t.posicion, t.rango, t.danio);
            }
        }
    } while (opcion != 5 && vidas > 0);

    if (vidas <= 0) cout << "\n!!! GAME OVER - BASE DESTRUIDA !!!\n" << endl;
    else cout << "\nJuego finalizado.\n" << endl;

    return 0;
}
