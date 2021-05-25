package ntbinh174.studentmatrix.service;

import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

import com.amazonaws.AmazonServiceException;
import com.amazonaws.services.s3.AmazonS3;
import com.amazonaws.services.s3.model.CannedAccessControlList;
import com.amazonaws.services.s3.model.PutObjectRequest;
import com.amazonaws.services.s3.model.S3Object;
import com.amazonaws.services.s3.model.S3ObjectInputStream;
import com.amazonaws.util.IOUtils;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.scheduling.annotation.Async;
import org.springframework.stereotype.Service;
import org.springframework.web.multipart.MultipartFile;

@Service
public class AWSS3ServiceImpl implements AWSS3Service {
    
    private static final Logger LOGGER = LoggerFactory.getLogger(AWSS3ServiceImpl.class);

    @Autowired
    private AmazonS3 amazonS3;
    @Value("${aws.s3.bucket}")
    private String bucketName;

    @Override
    @Async
    public void uploadFile(MultipartFile multipartFile) {
        LOGGER.info("Uploading file...");
        try {
            File file = convertMultipartFileToFile(multipartFile);
            uploadFileToS3Bucket(bucketName, file);
            LOGGER.info("Upload completed");
            file.delete();
        } catch (AmazonServiceException e) {
            LOGGER.info("Uploading file has failed!");
            LOGGER.error("Error = {} while uploading file.", e.getMessage());
        }
    }

    @Override
    @Async
    public InputStream getFile(String keyName) {
        byte[] content = null;
        LOGGER.info("Downloading file with key " + keyName);
        S3Object s3object = amazonS3.getObject(bucketName, keyName);
        S3ObjectInputStream stream = s3object.getObjectContent();
        try {
            content = IOUtils.toByteArray(stream);
            LOGGER.info("Download completed");
            s3object.close();
        } catch (IOException ex) {
            LOGGER.error("IO Error Message = " + ex.getMessage());
        }
        return new ByteArrayInputStream(content);
    }

    @Override
    @Async
    public boolean checkFile(String keyName) {
        return amazonS3.doesObjectExist(bucketName, keyName);
    }

    private File convertMultipartFileToFile(MultipartFile multipartFile) {
        File file = new File(multipartFile.getOriginalFilename());
        try (FileOutputStream outputstream = new FileOutputStream(file)) {
            outputstream.write(multipartFile.getBytes());
        } catch (IOException ex) {
            LOGGER.error("Error converting multipartFile to file: " + ex.getMessage());
        }
        return file;
    }
    
    private void uploadFileToS3Bucket(String bucketName, File file) {
        String fileName = file.getName();
        LOGGER.info("Uploading file " + fileName + " ...");
        PutObjectRequest putObjectRequest = new PutObjectRequest(bucketName, fileName, file);
        amazonS3.putObject(putObjectRequest.withCannedAcl(CannedAccessControlList.PublicRead));
    }

}
