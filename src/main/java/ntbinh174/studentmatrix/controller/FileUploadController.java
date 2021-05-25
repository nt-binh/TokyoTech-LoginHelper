package ntbinh174.studentmatrix.controller;

import java.io.IOException;
import java.util.List;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.multipart.MultipartFile;
import org.springframework.web.servlet.mvc.support.RedirectAttributes;

import ntbinh174.studentmatrix.service.AWSS3Service;
import ntbinh174.studentmatrix.service.CrawlerService;
import ntbinh174.studentmatrix.service.OCRService;

// import com.example.uploadingfiles.storage.StorageFileNotFoundException;

@Controller
@RequestMapping("/upload")
public class FileUploadController {

    private AWSS3Service awsService;
    private OCRService ocrService;
    private CrawlerService crawlerService;
    private char[][] matrix;
    private char[][] codes;

    @Value("${studentNumber}")
    private String studentNumber;

    @Autowired
    public FileUploadController(AWSS3Service awsService, OCRService ocrService, CrawlerService crawlerService) {
        this.awsService = awsService;
        this.ocrService = ocrService;
        this.crawlerService = crawlerService;
    }

    @GetMapping
    public String index(Model model) {
        String keyName = studentNumber + ".jpg";
        String subject = (awsService.checkFile(keyName))? "Send the current code" : "Upload your card";
        int mode = (awsService.checkFile(keyName))? 1 : 0;
        model.addAttribute("subject", subject);
        model.addAttribute("mode", mode);
        return "uploadForm";
    }

    @PostMapping("/card")
    public String uploadFile(@RequestParam("file") MultipartFile file, RedirectAttributes redirectAttributes) {
        awsService.uploadFile(file);
        String message = "Uploaded file successfully: " + file.getOriginalFilename();
        redirectAttributes.addFlashAttribute("message", message);
        return "redirect:/upload";
    }

    @PostMapping("/code")
    public String uploadCode(Model model) throws IOException {
        if (matrix == null) {
            matrix = ocrService.detectCard(awsService.getFile(studentNumber + ".jpg"));
        }
        codes = convertListStringToCharArray(crawlerService.getCode());
        char[] keys = process(matrix, codes);
        String message = "Querying completed";
        model.addAttribute("keys", keys);
        model.addAttribute("message", message);
        return "showResult";
    }

    private char[][] convertListStringToCharArray(List<String> codes) {
        char[][] codeArr = new char[3][2];
        int row = 0, col = 0;
        for (String code : codes) {
            col = 0;
            char[] temp = code.toCharArray();
            for (int i = 0; i < temp.length; i++) {
                if ((temp[i] >= 48 && temp[i] <=55) || (temp[i] >= 65 && temp[i] <= 90)) {
                    codeArr[row][col++] = temp[i];
                }
            }
            row++;
        }
        return codeArr;
    }

    private char[] process(char[][] matrix, char[][] code) {
        char[] res = new char[3];
        int count = 0;
        while (count < 3) {
            int x = (code[count][1] - '0') - 1;
            int y = (code[count][0] - 'A');
            res[count++] = matrix[x][y];
        }
        return res;
    }
}
