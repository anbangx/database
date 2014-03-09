package crawl.type;

import java.util.ArrayList;
import java.util.Date;

public class Call {
    ArrayList<Date> callTime;
    ArrayList<String> name;
    ArrayList<String> number;
    ArrayList<String> type;
    
    public Call(){
        callTime = new ArrayList<Date>();
        name = new ArrayList<String>();
        number = new ArrayList<String>();
        type = new ArrayList<String>();
    }
    
    public void sort(){
        int min = 0;
        Date tmpTime = new Date();
        String tmpName = "";
        String tmpNumber = "";
        String tmpType = "";
        for(int i = 0; i < this.size() - 1; i++){
            min = i;
            for(int j = i+1; j < this.size(); j++){
                if(callTime.get(min).compareTo(callTime.get(j)) > 0){
                    min = j;
                }
            }
            //swap
            tmpTime = callTime.get(min);
            tmpName = name.get(min);
            tmpNumber = number.get(min);
            tmpType = type.get(min);
            callTime.set(min, callTime.get(i));
            name.set(min, name.get(i));
            number.set(min, number.get(i));
            type.set(min, type.get(i));
            callTime.set(i, tmpTime);
            name.set(i, tmpName);
            number.set(i, tmpNumber);
            type.set(i, tmpType);
        }
    }
    
    public int size(){
        return callTime.size();
    }  
    
    public String toString(){
        String out = "";
        for(int i = 0; i < this.size(); i++){
            out += "   " + callTime.get(i).toString();
            out += "   " + name.get(i);
            out += "   " + number.get(i);
            out += "   " + type.get(i);
            out += "\n";
        }
        return out;
    }
/*    
    public String toHTML(){
        String out = "";
        for(int i = 0; i < this.size(); i++){
            out += "<b>[Call]</b> " + callTime.get(i).toString().substring(11, callTime.get(this.size() - 1 - i).toString().indexOf("PDT"));
            out += " name: " + name.get(i);
            out += "(" + number.get(i) +")<br>";
            out += " " + type.get(i);
            out += "<br>";
        }
        return out;
    }
*/
    public String toHTML(){
        String out = "";
        for(int i = 0; i < this.size(); i++){
        	out += "<b>[Call]</b> At " + callTime.get(i).toString().substring(11, callTime.get(this.size() - 1 - i).toString().indexOf("PDT")) + ", I ";
        	if (type.get(i).toString().equals("outgoing") ){
        		out += "called " + name.get(i) + "(" + number.get(i) + ")";
        	}
        	else{
        		out += "got a call from " + name.get(i) + "(" + number.get(i) + ")";
        	}

            out += "<br>";
        }
        return out;
    }
    
    public ArrayList<Date> getCallTime() {
        return callTime;
    }

    public void setCallTime(ArrayList<Date> callTime) {
        this.callTime = callTime;
    }

    public ArrayList<String> getName() {
        return name;
    }

    public void setName(ArrayList<String> name) {
        this.name = name;
    }

    public ArrayList<String> getNumber() {
        return number;
    }

    public void setNumber(ArrayList<String> number) {
        this.number = number;
    }

    public ArrayList<String> getType() {
        return type;
    }

    public void setType(ArrayList<String> type) {
        this.type = type;
    }
}
