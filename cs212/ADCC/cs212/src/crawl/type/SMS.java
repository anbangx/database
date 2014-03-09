package crawl.type;

import java.util.ArrayList;
import java.util.Date;

public class SMS {
    ArrayList<Date> textTime;
    ArrayList<String> number;
    ArrayList<String> content;
    ArrayList<String> type;
    
    public SMS(){
        textTime = new ArrayList<Date>();
        number = new ArrayList<String>();
        content = new ArrayList<String>();
        type = new ArrayList<String>();
    }
    
    public void sort(){
        int min = 0;
        Date tmpTime = new Date();
        String tmpNumber = "";
        String tmpContent = "";
        String tmpType = "";
        for(int i = 0; i < this.size() - 1; i++){
            min = i;
            for(int j = i+1; j < this.size(); j++){
                if(textTime.get(min).compareTo(textTime.get(j)) > 0){
                    min = j;
                }
            }
            //swap
            tmpTime = textTime.get(min);
            tmpNumber = number.get(min);
            tmpContent = content.get(min);
            tmpType = type.get(min);
            textTime.set(min, textTime.get(i));
            content.set(min, content.get(i));
            number.set(min, number.get(i));
            type.set(min, type.get(i));
            textTime.set(i, tmpTime);
            content.set(i, tmpContent);
            number.set(i, tmpNumber);
            type.set(i, tmpType);
        }
    }
    
    public int size(){
        return textTime.size();
    }
    
    public String toString(){
        String out = "";
        for(int i = 0; i < this.size(); i++){
            out += "   " + textTime.get(i).toString();
            out += "   " + number.get(i).toString();
            out += "   " + content.get(i).toString();
            out += "   " + type.get(i).toString();
            out += "\n";
        }
        return out;
    }
/*    
    public String toHTML(){
        String out = "";
        for(int i = 0; i < this.size(); i++){
            out += "<b>[SMS]</b> " + textTime.get(i).toString().substring(11, textTime.get(this.size() - 1 - i).toString().indexOf("PDT"));
            out += " " + type.get(i).toString() +"<br>";
            out += " " + number.get(i).toString();
            out += " " + content.get(i).toString();
            out += "<br>";
        }
        return out;
    }
*/    
    public String toHTML(){
        String out = "";
        for(int i = 0; i < this.size(); i++){
            out += "<b>[SMS]</b> At " + textTime.get(i).toString().substring(11, textTime.get(this.size() - 1 - i).toString().indexOf("PDT")) + ",";
            
//            System.out.println("###########" + type.get(i).toCharArray().);
            
            if(type.get(i).equals("incoming")){
            	out += "I got a text from [" + number.get(i).toString() +"]: <br>";
            	out += "\"" + content.get(i).toString() + "\"";
            }
            else{
            	out += "I sent a text to [" + number.get(i).toString() +"]: <br>";
            	out += "\"" + content.get(i).toString() + "\"";
            }
            out += "<br>";
        }
        return out;
    }

    public ArrayList<Date> getTextTime() {
        return textTime;
    }

    public void setTextTime(ArrayList<Date> textTime) {
        this.textTime = textTime;
    }

    public ArrayList<String> getNumber() {
        return number;
    }

    public void setNumber(ArrayList<String> number) {
        this.number = number;
    }

    public ArrayList<String> getContent() {
        return content;
    }

    public void setContent(ArrayList<String> content) {
        this.content = content;
    }

    public ArrayList<String> getType() {
        return type;
    }

    public void setType(ArrayList<String> type) {
        this.type = type;
    }
}
