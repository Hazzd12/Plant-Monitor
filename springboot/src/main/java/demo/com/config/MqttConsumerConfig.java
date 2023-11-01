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

    /**
     * 在bean初始化后连接到服务器
     */
    @PostConstruct
    public void init(){
        connect();
    }

    /**
     * 客户端连接服务端
     */

    public boolean test(){
        return client.isConnected();
    }
    public void connect(){
        try {
            //create MQTT Client Object
            client = new MqttClient(hostUrl,clientId,new MemoryPersistence());
            MqttConnectOptions options = new MqttConnectOptions();
            //Every time you connect to the server, it's a new identity
            options.setCleanSession(true);
            //Set the connection user name
            options.setUserName(username);
            //Set the connection password
            options.setPassword(password.toCharArray());
            //设置超时时间，单位为秒
            options.setConnectionTimeout(100);
            //If the heartbeat interval is set to second, the server sends the heartbeat interval
            // to the client every 1.5 x 20 seconds to check whether the client is online
            options.setKeepAliveInterval(20);
            //set callback function
            client.setCallback(new MqttConsumerCallBack(mainService));
            client.connect(options);
            // subscribe topics
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


    /**
     * 订阅主题
     */
    public void subscribe(String topic,int qos){
        try {
            client.subscribe(topic,qos);
        } catch (MqttException e) {
            e.printStackTrace();
        }
    }
}