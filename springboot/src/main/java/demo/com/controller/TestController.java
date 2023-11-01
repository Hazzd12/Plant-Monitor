package demo.com.controller;

import demo.com.config.MqttConsumerConfig;
import demo.com.service.MainService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.web.bind.annotation.*;

@RestController
public class TestController {
    @Autowired
    private MqttConsumerConfig client;

    private final MainService mainService;
    @Value("${spring.mqtt.client.id}")
    private String clientId;
    public TestController(MainService mainService) {
        this.mainService = mainService;
    }
    @GetMapping("/test")
    public String test()
    {
        client.connect();
        return clientId + "connect to the MQTT server";
    }
    @GetMapping("/test4")
    public float[] test4() {
        return mainService.getRecentMaxAndMinVue();
    }


}
