package ntbinh174.studentmatrix.service;


import com.google.cloud.vision.v1.AnnotateImageRequest;
import com.google.cloud.vision.v1.AnnotateImageResponse;
import com.google.cloud.vision.v1.BatchAnnotateImagesResponse;
import com.google.cloud.vision.v1.EntityAnnotation;
import com.google.cloud.vision.v1.Feature;
import com.google.cloud.vision.v1.Image;
import com.google.cloud.vision.v1.ImageAnnotatorClient;
import com.google.protobuf.ByteString;

import org.springframework.stereotype.Service;

import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.List;

@Service
public class OCRServiceImpl implements OCRService {
    
    public char[][] detectCard(InputStream inputStream) throws IOException {
        List<AnnotateImageRequest> requests = new ArrayList<>();
        char[][] matrix = new char[7][10];

        ByteString imgBytes = ByteString.readFrom(inputStream);
        Image img = Image.newBuilder().setContent(imgBytes).build();
        Feature feat = Feature.newBuilder().setType(Feature.Type.TEXT_DETECTION).build();
        AnnotateImageRequest request =
        AnnotateImageRequest.newBuilder().addFeatures(feat).setImage(img).build();
        requests.add(request);


        // Initialize client that will be used to send requests. This client only needs to be created
        // once, and can be reused for multiple requests. After completing all of your requests, call
        // the "close" method on the client to safely clean up any remaining background resources.
        try (ImageAnnotatorClient client = ImageAnnotatorClient.create()) {
            BatchAnnotateImagesResponse response = client.batchAnnotateImages(requests);
            List<AnnotateImageResponse> responses = response.getResponsesList();
        
            String data = "";
            for (AnnotateImageResponse res : responses) {
                if (res.hasError()) {
                    System.out.format("Error: %s%n", res.getError().getMessage());
                    return null;
                }
          
              // For full list of available annotations, see http://g.co/cloud/vision/docs
                for (EntityAnnotation annotation : res.getTextAnnotationsList()) {
                    data = annotation.getDescription();  
                    // System.out.format("Text: %s%n", annotation.getDescription());
                    // System.out.format("Position : %s%n", annotation.getBoundingPoly());
                    break;
                }
            }
            String[] dataList = data.split("\n");
            for (int i = 0; i < dataList.length; i++) {
                dataList[i] = removeAllSpaces(dataList[i]);
                if (i >= 2) {
                    matrix[i-2] = dataList[i].substring(1).toCharArray();
                }
            }
            return matrix;
        }
    }

    private static String removeAllSpaces(String str) {
        char[] charArr = str.toCharArray();
        StringBuilder builder = new StringBuilder("");
        for (int i = 0; i < charArr.length; i++) {
            if (charArr[i] != ' ') {
                builder.append(charArr[i]);
            }
        }
        return builder.toString();
    }
}
