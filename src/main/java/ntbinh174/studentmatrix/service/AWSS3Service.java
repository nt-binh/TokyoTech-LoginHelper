package ntbinh174.studentmatrix.service;

import java.io.InputStream;

import org.springframework.web.multipart.MultipartFile;

public interface AWSS3Service {
    
    public void uploadFile(MultipartFile multipartFile);

    public InputStream getFile(String keyName);

    public boolean checkFile(String keyName);
    
}
