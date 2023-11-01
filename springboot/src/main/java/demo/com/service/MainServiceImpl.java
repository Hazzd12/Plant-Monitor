package demo.com.service;

import demo.com.utils.GlovalVar;
import demo.com.utils.MailContent;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.mail.javamail.JavaMailSender;
import org.springframework.mail.javamail.MimeMessageHelper;
import org.springframework.stereotype.Service;

import javax.mail.MessagingException;
import javax.mail.internet.MimeMessage;
import java.time.Duration;
import java.time.Instant;



@Service
public class MainServiceImpl implements MainService {
    @Value("${mail.username}")
    private String mailUserName;
    @Value(("${mail.useremail}"))
    private String consumerEmail;
    private Instant previousTime = Instant.now();
    private static int cot = 0;
    @Autowired
    private JavaMailSender javaMailSender;
    boolean flag = false;

    @Override
    public void MailSender(float[] content) {
        float humidity = content[1];
        float temperature = content[0];
        float moisture = content[2];
        StringBuilder sb = new StringBuilder();
        GlovalVar.temperatures[cot] = temperature;
        GlovalVar.moistures[cot] = moisture;
        GlovalVar.humiditys[cot] = humidity;
        cot++;
        cot %=10;
        if(flag){
            long timeDifference = calculateTimeDifferenceInSeconds();
            if(timeDifference >= 3 * 60 * 60) {
                flag = false;
            }
        }
        else {
            if (humidity < 40) {
                sb.append("the HUMIDITY value is too low which is ");
                sb.append("<span style=\"color: red; font-size: 25px; \">"
                        + humidity + "</span><br/>");
                flag = true;
            } else if (humidity > 60) {
                sb.append("the HUMIDITY value is too high which is ");
                sb.append("<span style=\"color: red;  font-size: 25px; \">"
                        + humidity + "</span><br/>");
                flag = true;
            }
            if (temperature > 24) {
                sb.append("the TEMPERATURE value is too high which is ");
                sb.append("<span style=\"color: red;  font-size: 25px; \">"
                        + temperature + "</span><br/>");
                flag = true;
            } else if (temperature < 15) {
                sb.append("the TEMPERATURE value is too low which is ");
                sb.append("<span style=\"color: red; font-size: 25px; \">"
                        + humidity + "</span><br/>");
                flag = true;
            }

            if (moisture > 75) {
                sb.append("the MOISTURE value is too high which is ");
                sb.append("<span style=\"color: red;  font-size: 25px;\">"
                        + moisture + "</span><br/>");
                flag = true;
            } else if (humidity < 20) {
                sb.append("the MOISTURE value is too low which is ");
                sb.append("<span style=\"color: red; font-size: 25px; \">"
                        + moisture + "</span><br/>");
                flag = true;
            }
            previousTime = Instant.now();
            SendProblemEmail(consumerEmail, sb.toString());
        }
    }
    @Override
    public float[] getRecentMaxAndMinVue() {
        float[] result =  new float[6];
        float[] temp =  new float[2];
        temp = getMaxValue(GlovalVar.humiditys);
        result[0] = temp[0];
        result[1] = temp[1];
        temp = getMaxValue(GlovalVar.temperatures);
        result[2] = temp[0];
        result[3] = temp[1];
        temp = getMaxValue(GlovalVar.moistures);
        result[4] = temp[0];
        result[5] = temp[1];
        return result;
    }
    public float[] getMaxValue(float[] arr){
        float[] val = new float[2];
        val[0] = arr[0];//min value
        val[1] = arr[1];//max value
        for(int i=1; i<arr.length; i++){
            if(arr[i]<val[0]){
                val[0] = arr[i];
            }
            if(arr[i]>val[1]){
                val[1] = arr[i];
            }
        }
        return val;
    }
    public void SendProblemEmail(String emailAddr, String problem) {
        String text = MailContent.mailContent(problem);
        MimeMessage mimeMessage = javaMailSender.createMimeMessage();
        try {
            MimeMessageHelper mimeMessageHelper = new MimeMessageHelper(mimeMessage, true);
            mimeMessageHelper.setFrom(mailUserName);
            mimeMessageHelper.setTo(emailAddr);
            mimeMessage.setSubject("Plant Monitor");
            mimeMessageHelper.setText(text, true);
            javaMailSender.send(mimeMessage);
        } catch (MessagingException e) {
            e.printStackTrace();
        }
    }
    public long calculateTimeDifferenceInSeconds() {
        Instant currentTime = Instant.now();
        Duration duration = Duration.between(previousTime, currentTime);
        long seconds = duration.getSeconds();
        return seconds;
    }
}
