package ntbinh174.studentmatrix.entity;

import javax.persistence.Entity;
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;
import javax.persistence.Id;
import javax.persistence.Transient;
import javax.validation.constraints.Email;
import javax.validation.constraints.NotBlank;
import javax.validation.constraints.NotEmpty;

import lombok.Data;
import lombok.NoArgsConstructor;

@Data
@Entity
@NoArgsConstructor
public class StudentInfo {
    
    @Id
    @GeneratedValue(strategy = GenerationType.AUTO)
    private String usrId;

    @NotBlank
    private String usrName;

    @Email
    @NotEmpty
    private String email;

    @Transient
    private String usrPassword;

}
