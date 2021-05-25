package ntbinh174.studentmatrix.service;

import java.io.IOException;
import java.io.InputStream;

public interface OCRService {
    
    public char[][] detectCard(InputStream inputStream) throws IOException;

}
