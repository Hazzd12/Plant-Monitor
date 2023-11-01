package demo.com.config;
import javax.annotation.PostConstruct;

import demo.com.service.MainService;
import demo.com.utils.MqttConsumerCallBack;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.annotation.Configuration;

@Configuration
public class MqttConsumerConfig {
    @Value("${spring.mqtt.username}")
    private String username;
    @Value("${spring.mqtt.password}")
    private String password;
    @Value("${spring.mqtt.url}")
    private String hostUrl;
    @Value("${spring.mqtt.client.id}")
    private String clientId;
    @Value("${spring.mqtt.default.topic}")
    private String defaultTopic;
    private final MainService mainService;
    private MqttClient client;

    public MqttConsumerConfig(MainService mainService) {
        this.mainService = mainService;
    }
    @PostConstruct
    public void init(){
        connect();
    }
    public boolean test(){
        return client.isConnected();
    }
    public void connect(){
        try {
            client = new MqttClient(hostUrl,clientId,new MemoryPersistence());
            MqttConnectOptions options = new MqttConnectOptions();
            options.setCleanSession(true);
            options.setUserName(username);
            options.setPassword(password.toCharArray());
            options.setConnectionTimeout(100);
            options.setKeepAliveInterval(20);
            client.setCallback(new MqttConsumerCallBack(mainService));
            client.connect(options);
            String[] topics = {defaultTopic+"/temperature", defaultTopic+"/moisture", defaultTopic+"/humidity"};
            client.subscribe(topics);
        } catch (MqttException e) {
            e.printStackTrace();
        }
    }
    public void disConnect(){
        try {
            client.disconnect();
        } catch (MqttException e) {
            e.printStackTrace();
        }
    }

}