package demo.com.utils;

public class MailContent {
        public static String mailContent(String problem)
        {
            StringBuilder stringBuilder = new StringBuilder();
            stringBuilder.append("<html><head><title></title></head><body>");
            stringBuilder.append("<h1 style=\"color: red;\">Warning</h1><br/>");
            stringBuilder.append("<span style=\"font-size:30px\">Your plant is meeting the problems：</span><br/>");
            stringBuilder.append(problem);
            return stringBuilder.toString();
        }
}
