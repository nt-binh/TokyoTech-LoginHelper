package ntbinh174.studentmatrix.controller;

import javax.validation.Valid;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.validation.Errors;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.ModelAttribute;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.servlet.mvc.support.RedirectAttributes;

import ntbinh174.studentmatrix.entity.Student;
import ntbinh174.studentmatrix.repository.StudentRepository;

@Controller
@RequestMapping("/")
public class LoginController {
  
    private static final Logger LOGGER = LoggerFactory.getLogger(LoginController.class);

    private StudentRepository studentRepo;

    @Autowired
    public LoginController(StudentRepository studentRepo) {
        this.studentRepo = studentRepo;
    }

    @ModelAttribute(name="student")
    public Student student() {
        return new Student();
    }

    @GetMapping
    public String showLogin(Model model) {
        model.addAttribute("subject", "Enter student ID & password");
        return "loginForm";
    }

    @PostMapping
    public String processLoginForm(@Valid Student student, Errors errors, RedirectAttributes redirectAttributes) {
        if (errors.hasErrors()) {
            LOGGER.info("Error in student " + student);
            return "/";
        }
        LOGGER.info("Processing student info " + student);
        studentRepo.save(student);
        redirectAttributes.addFlashAttribute("student", student);
        return "redirect:/upload";
    }

}
