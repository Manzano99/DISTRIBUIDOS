package cliente;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;

// Imports necesarios para RabbitMQ
import com.rabbitmq.client.Channel;
import com.rabbitmq.client.Connection;
import com.rabbitmq.client.ConnectionFactory;

// Imports necesarios para RMI
import java.io.IOException;
import java.rmi.Naming;
import java.io.File;

public class Cliente {
    private final static String NOMBRE_COLA = "cola_pet_ftp"; // A RELLENAR (cambiar nombre a cola)

    static int id; // Identificador del cliente

    public static boolean directoryExists(String path) {
        if (path == null || path.isEmpty()) {
            return false;
        }

        File file = new File(path);
        return file.exists() && file.isDirectory();
    }

    public static boolean fileExists(String path) {
        if (path == null || path.isEmpty()) {
            return false;
        }

        File file = new File(path);
        return file.exists() && file.isFile();
    }

    public static void main(String[] argv) throws Exception {

        // =================================================
        // Obtener argumentos de línea de comandos
        if (argv.length < 3) {
            System.out.println("Uso: cliente <id_cliente> <fichero_descargas> <directorio_descargas>");
            System.exit(1);
        }

        try {
            id = Integer.parseInt(argv[0]);
        } catch (NumberFormatException e) {
            System.out.println("El id del cliente debe ser entero");
            System.exit(2);
        }

        if (!fileExists(argv[1])) {
            System.out.println("Cliente " + id + ": El fichero de descargas no existe");
            System.exit(3);
        }

        // comprobamos que el directorio de descargas existe
        if (!directoryExists(argv[2])) {
            System.out.println("Cliente " + id + ": El directorio de descargas no existe");
            System.exit(4);
        }

        // =================================================
        // Parte principal, toda dentro de un try para capturar cualquier excepción
        try {

            // Arrancar el la parte servidor RMI del cliente y registrarlo
            // ante rmiregistry con el nombre adecuado a este cliente
            // A RELLENAR:
            ClienteImpl cli = new ClienteImpl();
            Naming.rebind("Cliente_" + id, cli);

            // Conectar con Rabbit para poder enviar peticiones a la cola
            // A RELLENAR:
            ConnectionFactory factory = new ConnectionFactory();
            factory.setHost("localhost");
            Connection connection = factory.newConnection();
            Channel channel = connection.createChannel();
            channel.queueDeclare(NOMBRE_COLA, false, false, false, null);

            // Procesa la descargas de los ficheros
            procesarDescargas(channel, cli, argv[1], argv[2]);

            // Terminar
            System.out.println("Cliente finalizado");
            channel.close();
            connection.close();
            System.exit(0);
        } catch (Exception e) {
            // Cualquier excepción simplemente se imprime
            System.out.println("Error en Cliente" + e.getMessage());
            e.printStackTrace();
        }
    }

    // =========================================================================
    // La función procesarDescargas lee del fichero de descargas y envia
    // mensajes a través de la cola de mensajes para comprobar si
    // el fichero a descargar existe en el servidor, si tiene permiso de lectura
    // cuántas líneas tiene y a partir de ahí descrgarlo línea a línea
    // Requiere como parámetros:
    //
    // - El canal de comunicación con RabbitMQ para enviar los mensajes
    // - La referencia al objeto RMI exportado por el cliente
    // - El nombre del fichero con los ficheros a descargar del servidor
    // - El nombre de la carpeta donde se almacenarán los ficheros descargados
    //
    // Una vez finaliza de descarga todos los ficheros del fichero de entrada,
    // la función retorna
    static void procesarDescargas(Channel channel, ClienteImpl cli, String fich_peticiones, String dir_descarga)
            throws IOException, InterruptedException {

        BufferedReader br = new BufferedReader(new FileReader(fich_peticiones));
        String msg; // Para componer los mensajes a enviar por Rabbit
        String respuesta; // Respuesta a la petición realizada
        String linea;

        try {
            String query = br.readLine();

            while (query != null) {
                // Aqui es donde hay que enviar las peticiones y esperar las respuestas
                // por RMI. Se recuerda que para descargar un fichero serán necesarias
                // varias peticiones sucesivas al servidor.

                // Cada peticion consiste primero en el nombre del cliente registrado en RMI
                // seguido del comando que corresponda y el nombre del fichero, separados ambos por
                // espacios en blanco. Este mensaje se envía a la cola Rabbit.
                // Después se espera a recibir la respuesta del servidor a través de RMI para ese
                // comando, antes de proceder con el siguiente comando. Si alguno falla, se ignora
                // el resto de comandos y se pasa a la siguiente línea del fichero de peticiones.
                // El cliente debe también emitir por la salida estándar (para depuración) uno
                // de los siguientes mensajes, según corresponda:
                //
                //  "Cliente_X: El fichero <fichero> existe en el servidor"
                //  "Cliente_X: El fichero <fichero> es legible"
                //  "Cliente_X: El fichero <fichero> tiene <num_lineas> líneas"
                //  "Cliente_X: El fichero <fichero> está vacío"
                //  "Cliente_X: Descargando línea <num_linea> del fichero <fichero>"
                //  "Cliente_X: El fichero <fichero> no es legible"
                //  "Cliente_X: El fichero <fichero> no existe en el servidor"

                // A RELLENAR
                String[] partes = query.trim().split(" ");
                if (partes.length < 1) {
                    query = br.readLine();
                    continue;
                }

                String nombreFichero = partes[0];
                String nombreCliente = "Cliente_" + id;

                msg = nombreCliente + " ISFILE " + nombreFichero;
                channel.basicPublish("", NOMBRE_COLA, null, msg.getBytes());
                respuesta = cli.getIsFile() ? "SI" : "NO";
                if (!respuesta.equals("SI")) {
                    System.out.println("Cliente_" + id + ": El fichero " + nombreFichero + " no existe en el servidor");
                    query = br.readLine();
                    continue;
                }
                System.out.println("Cliente_" + id + ": El fichero " + nombreFichero + " existe en el servidor");

                msg = nombreCliente + " ISREADABLE " + nombreFichero;
                channel.basicPublish("", NOMBRE_COLA, null, msg.getBytes());
                respuesta = cli.getIsReadable() ? "SI" : "NO";
                if (!respuesta.equals("SI")) {
                    System.out.println("Cliente_" + id + ": El fichero " + nombreFichero + " no es legible");
                    query = br.readLine();
                    continue;
                }
                System.out.println("Cliente_" + id + ": El fichero " + nombreFichero + " es legible");

                msg = nombreCliente + " GETNUMLINES " + nombreFichero;
                channel.basicPublish("", NOMBRE_COLA, null, msg.getBytes());
                int numLineas = cli.getNumLines();
                if (numLineas == 0) {
                    System.out.println("Cliente_" + id + ": El fichero " + nombreFichero + " está vacío");
                    query = br.readLine();
                    continue;
                }
                System.out.println("Cliente_" + id + ": El fichero " + nombreFichero + " tiene " + numLineas + " líneas");

                File f = new File(dir_descarga + "/" + nombreFichero);
                BufferedWriter bw = new BufferedWriter(new FileWriter(f));

                for (int i = 1; i <= numLineas; i++) {
                    msg = nombreCliente + " GETLINE " + nombreFichero + " " + i;
                    channel.basicPublish("", NOMBRE_COLA, null, msg.getBytes());
                    respuesta = cli.getLine();
                    System.out.println("Cliente_" + id + ": Descargando línea " + i + " del fichero " + nombreFichero);
                    bw.write(respuesta);
                }

                bw.close();
                // Leer la siguiente consulta del fichero
                query = br.readLine();
            }
        } catch (Exception e) {
            // Cualquier excepción simplemente se imprime
            System.out.println("Error en Cliente" + e.getMessage());
            e.printStackTrace();
        } finally {
            // Cerrar fichero de entrada y de salida
            br.close();
        }

    }
}
