package crawl.type;

import java.util.ArrayList;
import java.util.Date;

public class Photo {
    ArrayList<Date> createTime; //Date?
    ArrayList<String> type;
    ArrayList<String> title;
    ArrayList<String> link;

    public Photo() {
        createTime = new ArrayList<Date>();
        type = new ArrayList<String>();
        title = new ArrayList<String>();
        link = new ArrayList<String>();
    }
    
    public void sort(){
        int min = 0;
        Date tmpTime = new Date();
        String tmpType = "";
        String tmpTitle = "";
        String tmpLink = "";
        for(int i = 0; i < this.size() - 1; i++){
            min = i;
            for(int j = i+1; j < this.size(); j++){
                if(createTime.get(min).compareTo(createTime.get(j)) > 0){
                    min = j;
                }
            }
            //swap
            tmpTime = createTime.get(min);
            tmpType = type.get(min);
            tmpTitle = title.get(min);
            tmpLink = link.get(min);
            createTime.set(min, createTime.get(i));
            type.set(min, type.get(i));
            title.set(min, title.get(i));
            link.set(min, link.get(i));
            createTime.set(i, tmpTime);
            type.set(i, tmpType);
            title.set(i, tmpTitle);
            link.set(i, tmpLink);
        }
    }
    
    public int size(){
        return createTime.size();
    }
    
    public String toString(){
        String out = "";
        for(int i = 0; i < this.size(); i++){
            out += "   " + createTime.get(i).toString();
            out += "   " + title.get(i).toString();
            out += "   " + type.get(i).toString();
            if(link.get(i) != null)
                out += "   " + link.get(i).toString();
            out += "\n";
        }
        return out;
    }
    
    public String toHTML(){   	
        String out = "";
        if(this.size() > 0)
        	out += "<b>[Pic]</b><table border=\"0\" cellspacing=\"20\" cellpadding=\"20\">";
        
        for(int i = 0; i < this.size(); i++){
        	if(i%4 == 0){
        		out += "<tr>";
        	}
        	
        	out += "<td>At " +createTime.get(i).toString().toString().substring(11, createTime.get(this.size() - 1 - i).toString().indexOf("PDT")) + "<br>";
            if(title.get(i) != null)
                out += "<a alt=\"" + title.get(i).toString() + "\" href=\"https://students.ics.uci.edu/~taewok2/" + title.get(i).toString() + 
                "\" target=\"_blank\"><img width=\"80\" height=\"80\" border=\"0\" alt=\"" + title.get(i).toString() + "\" src=\"https://students.ics.uci.edu/~taewok2/" + title.get(i).toString() + "\" />&nbsp;&nbsp;&nbsp;</a>  ";
            out += "</td>";
            
            if(i%3 == 3){
        		out += "</tr>";
        	}            
        }
        
        out += "</table>";
        
        return out;
    }

    public ArrayList<Date> getCreateTime() {
        return createTime;
    }

    public void setCreateTime(ArrayList<Date> createTime) {
        this.createTime = createTime;
    }

    public ArrayList<String> getType() {
        return type;
    }

    public void setType(ArrayList<String> type) {
        this.type = type;
    }

    public ArrayList<String> getTitle() {
        return title;
    }

    public void setTitle(ArrayList<String> title) {
        this.title = title;
    }

    public ArrayList<String> getLink() {
        return link;
    }

    public void setLink(ArrayList<String> link) {
        this.link = link;
    }
}