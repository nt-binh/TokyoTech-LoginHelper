package ntbinh174.studentmatrix.repository;

import org.springframework.data.jpa.repository.JpaRepository;

import ntbinh174.studentmatrix.entity.Student;

public interface StudentRepository extends JpaRepository<Student, Long> {
    
}
