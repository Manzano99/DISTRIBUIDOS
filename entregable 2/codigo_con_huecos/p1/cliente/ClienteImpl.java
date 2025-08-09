package cliente;

import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;
import java.util.concurrent.ArrayBlockingQueue;

/*
La clase ClienteImpl expone vía RMI los metodos:

setXXXXXXXXXX(): que seran invocados desde el servidor FTP para responder a cadea mensaje del cliente

Además proporciona los metodos getXXXXXXXXXX() para que pueda ser llamado
desde el método procesarDescargas() del cliente, para así obtener la respuesta al ultimo mensaje enviado por la cola Rabbit.

Estos métodos se ejecutan en diferentes hilos (ej.: setXXXXXXXXX() se
ejecuta en el hilo que atiende peticiones RMI, mientras que getXXXXXXXXX() se
ejecuta en el hilo del cliente. Se comunican entre sí mediante
una cola bloqueante. getXXXXXXXX() intenta leer de esa cola, y si está vacía
se bloquea. setXXXXXXXXX() pone en esa cola la respuesta al ultimo mensaje
enviado a la cola Rabbit, desbloqueando así al hilo que estaba esperando en 
getXXXXXXXXX())
*/

public class ClienteImpl extends UnicastRemoteObject implements ClienteInterface {
    private ArrayBlockingQueue<Object> cola; // Cola bloqueante para comunicar los hilos

    public ClienteImpl() throws RemoteException {
        super();
        // La cola a través de la cual se comunican los hilos que ejecutan
        // los métodos de esta clase
        cola = new ArrayBlockingQueue<Object>(1); // Basta que tenga tamaño 1
    }

    @Override
    public void setIsFile(Boolean answer) throws RemoteException {
        // Este método es invocado vía RMI desde el servidor FTP
        // Recibe la respuesta a la consulta y se limita a guardarla en la cola interna
        try {
            // A RELLENAR:
            |
            |
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void setIsReadable(Boolean answer) throws RemoteException {
        // Este método es invocado vía RMI desde el servidor FTP
        // Recibe la respuesta a la consulta y se limita a guardarla en la cola interna
        try {
            // A RELLENAR:
            |
            |            
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void setNumLines(Integer answer) throws RemoteException {
        // Este método es invocado vía RMI desde el servidor FTP
        // Recibe la respuesta a la consulta y se limita a guardarla en la cola interna
        try {
            // A RELLENAR:
            |
            |
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void setLine(String line) throws RemoteException {
        // Este método es invocado vía RMI desde el servidor FTP
        // Recibe la respuesta a la consulta y se limita a guardarla en la cola interna
        try {
            // A RELLENAR:
            |
            |
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    public Boolean getIsFile() throws InterruptedException {
        // Este método es invocado desde el cliente (método procesarDescargas)
        // Espera a que aparezca la respuesta a la última consulta
        // y retorna su valor
        // A RELLENAR:
        |
        |
    }

    public Boolean getIsReadable() throws InterruptedException {
        // Este método es invocado desde el cliente (método procesarDescargas)
        // Espera a que aparezca la respuesta a la última consulta
        // y retorna su valor
        // A RELLENAR:
        |
        |
    }

    public Integer getNumLines() throws InterruptedException {
        // Este método es invocado desde el cliente (método procesarDescargas)
        // Espera a que aparezca la respuesta a la última consulta
        // y retorna su valor
        // A RELLENAR:
        |
        |
    }

    public String getLine() throws InterruptedException {
        // Este método es invocado desde el cliente (método procesarDescargas)
        // Espera a que aparezca la respuesta a la última consulta
        // y retorna su valor
        // A RELLENAR:
        |
        |
    }

}
