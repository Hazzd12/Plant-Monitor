package demo.com.utils;

import org.springframework.stereotype.Component;

@Component
public class GlovalVar {
    public static float[] humiditys = new float[10];
    public static float[] temperatures = new float[10];
    public static float[] moistures  = new float[10];

    static {
        // 初始化数组
        for (int i = 0; i < humiditys.length; i++) {
            humiditys[i] = i;
            temperatures[i] = i;
            moistures[i] = i;
        }
    }
}
