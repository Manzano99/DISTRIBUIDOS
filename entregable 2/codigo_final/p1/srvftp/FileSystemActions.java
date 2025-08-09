package srvftp;

// Import necesario para verificar la existencia de un fichero
import java.io.IOException;
import java.io.File;
import java.io.BufferedReader;
import java.io.FileReader;

class FileSystemActions {

    public boolean directoryExists(String path) {
        if (path == null || path.isEmpty()) {
            return false;
        }
        File file = new File(path);
        return file.exists() && file.isDirectory();
    }

    public boolean fileExists(String path) {
        if (path == null || path.isEmpty()) {
            return false;
        }
        File file = new File(path);
        return file.exists() && file.isFile();
    }

    public boolean fileIsReadable(String path) {
        if (path == null || path.isEmpty()) {
            return false;
        }
        File file = new File(path);
        return file.canRead();
    }

    public int getNumLines(String path) {
        if (path == null || path.isEmpty()) {
            return 0;
        }
        int numLines = 0;
        try {
            BufferedReader reader = new BufferedReader(new FileReader(path));
            while (reader.readLine() != null)
                numLines++;
            reader.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return numLines;
    }

    public String getLine(String path, int lineNumber) {
        if (path == null || path.isEmpty()) {
            return null;
        }
        String line = null;
        try {
            BufferedReader reader = new BufferedReader(new FileReader(path));
            for (int i = 1; i <= lineNumber; i++) {
                line = reader.readLine();
            }
            reader.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
        if (line != null)
            line = line + "\n";
        return line;
    }
}
