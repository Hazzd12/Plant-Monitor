package demo.com.utils;


import demo.com.service.MainService;
import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttMessage;

public class  MqttConsumerCallBack implements MqttCallback{
    private  final MainService mainService;
    int count = 0;
    float[] par = new float[3];
    public MqttConsumerCallBack(MainService mainService) {
        this.mainService = mainService;
    }

    @Override
    public void connectionLost(Throwable throwable) {
        System.out.println("Disconnect");
    }
    @Override
    public void messageArrived(String topic, MqttMessage message) throws Exception {
        System.out.println(topic);
        System.out.println(message);
        try {
            par[count] = Float.valueOf(String.valueOf(message));
            count++;
            if(count==3){
                count=0;
                mainService.MailSender(par);
            }
        }
        catch (Exception e){
            System.out.println(count);
            System.out.println("fail");
            e.printStackTrace();
        }
    }
    @Override
    public void deliveryComplete(IMqttDeliveryToken iMqttDeliveryToken) {
    }

}