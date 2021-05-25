package ntbinh174.studentmatrix.service;

import java.util.ArrayList;
import java.util.List;

import org.openqa.selenium.By;
import org.openqa.selenium.WebDriver;
import org.openqa.selenium.WebElement;
import org.openqa.selenium.chrome.ChromeDriver;
import org.openqa.selenium.chrome.ChromeOptions;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.scheduling.annotation.Async;
import org.springframework.stereotype.Service;

@Service
public class CrawlerServiceImpl implements CrawlerService {
    
    private static final Logger LOGGER = LoggerFactory.getLogger(CrawlerServiceImpl.class);

    @Value("${studentNumber}")
    private String usrName;

    @Value("${studentPassword}")
    private String usrPassword;

    @Value("${loginPage}")
    private String loginPage;

    @Override
    @Async
    public List<String> getCode() {
        List<String> res = new ArrayList<String>();
        ChromeOptions options = new ChromeOptions();
		options.addArguments("--headless");
		WebDriver driver = new ChromeDriver(options);
		String baseUrl = loginPage;					
        LOGGER.info("Opening browser...");
        driver.get(baseUrl);					
	
        driver.findElement(By.name("usr_name")).sendKeys(usrName);							
        driver.findElement(By.name("usr_password")).sendKeys(usrPassword);							
		driver.findElement(By.name("OK")).click();
        
        for (int i = 4; i < 7; i++) {
            String xpath = "//*[@id=\"authentication\"]/tbody/tr["+ i + "]/th[1]";
            WebElement code = driver.findElement(By.xpath(xpath));
            res.add(code.getText());
        }
        driver.close();
        return res;
    }

}
