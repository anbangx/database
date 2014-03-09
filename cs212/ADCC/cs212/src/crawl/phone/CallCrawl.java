package crawl.phone;

import java.io.FileReader;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.Locale;

import au.com.bytecode.opencsv.CSVReader;

import crawl.type.Call;

public class CallCrawl {
    
    public static Call start(String csvFilename, Date startTime, Date endTime) throws Exception{
        ArrayList<Date> callTime = new ArrayList<Date>();
        ArrayList<String> name = new ArrayList<String>();
        ArrayList<String> number = new ArrayList<String>();
        ArrayList<String> type = new ArrayList<String>();
        
        CSVReader csvReader = new CSVReader(new FileReader(csvFilename));
        String[] row = null;
        csvReader.readNext();
        csvReader.readNext();
        Date tmp = new Date();
        while((row = csvReader.readNext()) != null) {
            String s = row[0] + " " + row[1];
            tmp = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss aa", Locale.ENGLISH).parse(s);
            if(tmp.compareTo(startTime) >= 0 && tmp.compareTo(endTime) <= 0){
                callTime.add(tmp);
                name.add(row[2]);
                number.add(row[3]);
                type.add(row[6]);
                System.out.println(s
                        + " # " + row[2]
                        + " #  " + row[3]
                        + " #  " + row[6]);
            }
        }
        //...
        csvReader.close();
        
        Call call = new Call();
        call.setCallTime(callTime);
        call.setName(name);
        call.setNumber(number);
        call.setType(type);
        return call;
    }
    public static void main(String[] args) throws Exception {
        Date startTime = new SimpleDateFormat("yyyy-MM-dd").parse("2013-06-07");
        Date endTime = new SimpleDateFormat("yyyy-MM-dd").parse("2013-06-09");
        CallCrawl.start( "src/Data/call.csv", startTime, endTime);
    }
}
