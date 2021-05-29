# TokyoTech-LoginHelper

Package/Framework used
------------
<li>Spring Boot</li>
<li>Spring Data JPA, H2 Database</li>
<li>Selenium</li>
<li>OpenCV (C++)</li>
<li>Cloud Vision API</li>
<li>AWS Service API</li>
<li>Docker</li>

Goal
------------
The goal of this project is to help students saving time logging in Tokyo Tech Portal by offering student IC card detection feature and automatically displaying one-time password on the UI.

Which problem does TokyoTech LoginHelper address?
------------
As a student at Tokyo Institute of Technology, whenever I need to log into Tokyo Tech Portal to register the course, check the student's mailbox, receive the assignment, I have to type in not only my student ID and password but also the one-time password based on the matrix code written on my student card.
To type in the one-time password, I need to take out my student card to check the matrix code or open the folder where I store the student card as an image file. Therefore, by utilizing my working experience as an intern in the last two companies, I created this web application to save me and other potential users 10-20 seconds each time they log into the Portal.

What's included?
------------
<code>TokyoTech-LoginHelper/</code><br>
<code>├── src/</code><br>
<code>│   ├── main/</code><br>
<code>│   │   ├── java/ntbinh174/studentmatrix/</code><br>
<code>|   |   |   ├── config/</code><br>
<code>|   |   |   |   └── AWSS3Config.java</code><br>
<code>|   |   |   ├── controller/</code><br>
<code>|   |   |   |   └── FileUploadController.java</code><br>
<code>|   |   |   |   └── LoginController.java</code><br>
<code>|   |   |   ├── entity/</code><br>
<code>|   |   |   |   └── Student.java</code><br>
<code>|   |   |   ├── repository/</code><br>
<code>|   |   |   |   └── StudentRepository.java</code><br>
<code>|   |   |   ├── service/</code><br>
<code>|   |   |   |   └── AWSS3Service.java</code><br>
<code>|   |   |   |   └── AWSS3ServiceImpl.java</code><br>
<code>|   |   |   |   └── CrawlerService.java</code><br>
<code>|   |   |   |   └── CrawlerServiceImpl.java</code><br>
<code>|   |   |   |   └── OCRService.java</code><br>
<code>|   |   |   |   └── OCRServiceImpl.java</code><br>
<code>|   |   |   ├── StudentMatrixApplication.java</code><br><br>
<code>│   │   ├── resources</code><br>
<code>│   ├── test/</code><br>
<code>│   └── Dockerfile</code><br>
<code>│   └── pom.xml</code><br>
<code>│   └── claheTransform.cpp</code><br>
<code>│   └── claheTransform.exe</code><br>
<code>└── README.md</code><br>

Further Improvements
--------------
<li>Add Rest API version</li>
<li>Add Spring Security</li>


