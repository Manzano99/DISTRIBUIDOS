package cliente;

import java.rmi.Remote;
import java.rmi.RemoteException;

/*
Interfaz remoto del Cliente que ha de hacer público los métodos setXXXXXXX() para que el servidor FTP pueda invocarlos vía RMI
*/
public interface ClienteInterface extends Remote {
    // A RELLENAR:
    public void setIsFile(Boolean answer) throws RemoteException;
    public void setIsReadable(Boolean answer) throws RemoteException;
    public void setNumLines(Integer answer) throws RemoteException;
    public void setLine(String line) throws RemoteException;
}
