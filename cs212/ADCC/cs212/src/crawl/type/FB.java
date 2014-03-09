package crawl.type;

import java.util.ArrayList;
import java.util.Date;

public class FB {
    ArrayList<Date> postTime;
    ArrayList<String> type;
    ArrayList<String> title;
    ArrayList<String> link;
    ArrayList<String> description;
    ArrayList<String> actionLink;
    
    public FB(){
        postTime = new ArrayList<Date>();
        type = new ArrayList<String>();
        title = new ArrayList<String>();
        link = new ArrayList<String>();
        description = new ArrayList<String>();
        actionLink = new ArrayList<String>();
    }
    
    public void sort(){
        int min = 0;
        Date tmpTime = new Date();
        String tmpType = "";
        String tmpTitle = "";
        String tmpLink = "";
        String tmpDescription = "";
        String tmpActionLink = "";
        for(int i = 0; i < this.size() - 1; i++){
            min = i;
            for(int j = i+1; j < this.size(); j++){
                if(postTime.get(min).compareTo(postTime.get(j)) > 0){
                    min = j;
                }
            }
            //swap
            tmpTime = postTime.get(min);
            tmpType = type.get(min);
            tmpTitle = title.get(min);
            tmpLink = link.get(min);
            tmpDescription = description.get(min);
            tmpActionLink = actionLink.get(min);
            postTime.set(min, postTime.get(i));
            type.set(min, type.get(i));
            title.set(min, title.get(i));
            link.set(min, link.get(i));
            description.set(min, description.get(i));
            actionLink.set(min, actionLink.get(i));
            postTime.set(i, tmpTime);
            type.set(i, tmpType);
            title.set(i, tmpTitle);
            link.set(i, tmpLink);
            description.set(i, tmpDescription);
            actionLink.set(i, tmpActionLink);
        }
    }
    
    public int size(){
        return postTime.size();
    }
    
    public String toString(){
        String out = "";
        for(int i = 0; i < this.size(); i++){
            out += "   " + postTime.get(i).toString();
            out += "   " + actionLink.get(i);
            out += "   " + title.get(i);
            out += "   " + type.get(i);
            if(link.get(i) != null)
                out += "   " + link.get(i);
            if(description.get(i) != null)
                out += "   " + description.get(i);
            out += "\n";
        }
        return out;
    }
    
    public String toHTML(){
        String out = "";
        for(int i = 0; i < this.size(); i++){
        	out += "<b>[FB] <a href=" + actionLink.get(i) + ">" + title.get(i).toString() + "</a> " + "</b>";
        	
            out += " Time: " + postTime.get(i).toString().substring(11, postTime.get(i).toString().indexOf("PDT"));
            out += " Type: " + type.get(i).toString();
            if(link.get(i) != null)
                out += "<br>" + link.get(i).toString();
            if(description.get(i) != null)
                out += "<br>" + description.get(i).toString();
            out += "<br>";
        }
        return out;
    }
    
    public String getTitles(){
        String out = "";
        for(int i = 0; i < this.size(); i++){
        	out += "[FB]" + title.get(this.size() - 1 - i).toString() + "\n";
        }
        return out;
    }
    
    public ArrayList<Date> getPostTime() {
        return postTime;
    }

    public void setPostTime(ArrayList<Date> postTime) {
        this.postTime = postTime;
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

    public ArrayList<String> getdescription() {
        return description;
    }

    public void setdescription(ArrayList<String> description) {
        this.description = description;
    }

    public ArrayList<String> getDescription() {
        return description;
    }

    public void setDescription(ArrayList<String> description) {
        this.description = description;
    }

    public ArrayList<String> getActionLink() {
        return actionLink;
    }

    public void setActionLink(ArrayList<String> actionLink) {
        this.actionLink = actionLink;
    }
    
}
