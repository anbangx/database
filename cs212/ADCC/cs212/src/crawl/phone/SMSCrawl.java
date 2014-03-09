package crawl.phone;

import java.io.FileReader;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.Locale;

import au.com.bytecode.opencsv.CSVReader;

import crawl.type.SMS;

public class SMSCrawl {
    public static SMS start(String csvFilename, Date startTime, Date endTime) throws Exception{
        ArrayList<Date> textTime = new ArrayList<Date>();
        ArrayList<String> number = new ArrayList<String>();
        ArrayList<String> content = new ArrayList<String>();
        ArrayList<String> type = new ArrayList<String>();
        
        CSVReader csvReader = new CSVReader(new FileReader(csvFilename));
        String[] row = null;
        csvReader.readNext();
        csvReader.readNext();
        Date tmp = new Date();
        while((row = csvReader.readNext()) != null) {
            tmp = new SimpleDateFormat("EEE MMM dd HH:mm:ss zzz yyyy", Locale.ENGLISH).parse(row[4]);
            if(tmp.compareTo(startTime) >= 0 && tmp.compareTo(endTime) <= 0){
                textTime.add(tmp);
                number.add(row[2]);
                content.add(row[12]);
                if(row[9].equals("1"))
                    type.add("incoming");
                else
                    type.add("outgoing");
                System.out.println(row[4]
                        + " # " + row[2]
                        + " #  " + row[12]
                                + " #  " + row[9]);
            }
        }
        //...
        csvReader.close();
        
        SMS sms = new SMS();
        sms.setTextTime(textTime);
        sms.setNumber(number);
        sms.setContent(content);
        sms.setType(type);
        return sms;
    }
    
    public static void main(String[] args) throws Exception {
        Date startTime = new SimpleDateFormat("yyyy-MM-dd").parse("2013-06-07");
        Date endTime = new SimpleDateFormat("yyyy-MM-dd").parse("2013-06-09");
        SMSCrawl.start( "src/Data/SMS.csv", startTime, endTime);
    }
}
