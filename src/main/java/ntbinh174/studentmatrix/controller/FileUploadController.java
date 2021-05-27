package ntbinh174.studentmatrix.controller;

import java.io.IOException;
import java.util.List;
import java.util.Map;

import javax.servlet.http.HttpServletRequest;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.SessionAttributes;
import org.springframework.web.multipart.MultipartFile;
import org.springframework.web.servlet.mvc.support.RedirectAttributes;
import org.springframework.web.servlet.support.RequestContextUtils;

import ntbinh174.studentmatrix.entity.Student;
import ntbinh174.studentmatrix.repository.StudentRepository;
import ntbinh174.studentmatrix.service.AWSS3Service;
import ntbinh174.studentmatrix.service.CrawlerService;
import ntbinh174.studentmatrix.service.OCRService;

// import com.example.uploadingfiles.storage.StorageFileNotFoundException;

@Controller
@RequestMapping("/upload")
@SessionAttributes("student")
public class FileUploadController {

    private StudentRepository studentRepo;

    private AWSS3Service awsService;
    private OCRService ocrService;
    private CrawlerService crawlerService;
    private char[][] matrix;
    private char[][] codes;

    // @Value("${studentNumber}")
    // private String studentNumber;
    private String username;
    private String password;

    @Autowired
    public FileUploadController(AWSS3Service awsService, OCRService ocrService, CrawlerService crawlerService, StudentRepository studentRepo) {
        this.awsService = awsService;
        this.ocrService = ocrService;
        this.crawlerService = crawlerService;
        this.studentRepo = studentRepo;
    }

    @GetMapping
    public String uploadFile(Model model, HttpServletRequest request) {
        Map<String,?> inputFlashMap = RequestContextUtils.getInputFlashMap(request);
        Student student;
        if (inputFlashMap != null) {
            student = (Student) inputFlashMap.get("student");
        } else {
            return "redirect:/";
        }
        username = student.getUsrName();
        password = student.getUsrPassword();
        String keyName = username + ".jpg";
        String subject = (awsService.checkFile(keyName))? "Get the keys from card" : "Upload Student IC card";
        int mode = (awsService.checkFile(keyName))? 1 : 0;
        model.addAttribute("subject", subject);
        model.addAttribute("mode", mode);
        return "uploadForm";
    }

    @GetMapping("/code")
    public String generateKey(Model model) {
        String subject = "Get the keys from card";
        int mode = 1;
        model.addAttribute("subject", subject);
        model.addAttribute("mode", mode);
        return "uploadForm";
    }

    @PostMapping("/card")
    public String processUploadCard(@RequestParam("file") MultipartFile file, RedirectAttributes redirectAttributes) {
        awsService.uploadFile(file);
        // String message = "Uploaded file successfully: " + file.getOriginalFilename();
        // redirectAttributes.addFlashAttribute("message", message);
        return "redirect:/upload/code";
    }

    @PostMapping("/key")
    public String showKeys(HttpServletRequest request) throws IOException {
        if (matrix == null) {
            matrix = ocrService.detectCard(awsService.getFile(username + ".jpg"));
        }
        codes = convertListStringToCharArray(crawlerService.getCode(username, password));
        char[] keys = process(matrix, codes);
        request.setAttribute("keys", keys);
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
