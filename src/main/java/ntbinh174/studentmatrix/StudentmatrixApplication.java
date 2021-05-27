package ntbinh174.studentmatrix;

import java.io.FileInputStream;
import java.io.IOException;

import com.google.api.gax.paging.Page;
import com.google.auth.oauth2.GoogleCredentials;
import com.google.cloud.storage.Bucket;
import com.google.cloud.storage.Storage;
import com.google.cloud.storage.StorageOptions;
import com.google.common.collect.Lists;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;

@SpringBootApplication
public class StudentmatrixApplication {

	public static void main(String[] args) throws IOException {
		SpringApplication.run(StudentmatrixApplication.class, args);
	}
}
