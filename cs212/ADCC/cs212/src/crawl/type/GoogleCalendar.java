package crawl.type;

import java.util.ArrayList;
import java.util.Date;

public class GoogleCalendar {
    ArrayList<Date> startTime;
    ArrayList<Date> endTime;
    ArrayList<String> title;
    ArrayList<String> location;
    ArrayList<String> description;
    ArrayList<String> actionLink;
    
    public GoogleCalendar(){
        startTime = new ArrayList<Date>();
        endTime = new ArrayList<Date>();
        title = new ArrayList<String>();
        location = new ArrayList<String>();
        description = new ArrayList<String>();
        actionLink = new ArrayList<String>();
    }
    
    public void add(Date startTime, Date endTime, String title, String location, String description, String actionLink){
        this.startTime.add(startTime);
        this.endTime.add(endTime);
        this.title.add(title);
        this.location.add(location);
        this.description.add(description);
        this.actionLink.add(actionLink);
    }
    
    public void sort(){
        int min = 0;
        Date tmpStartTime = new Date();
        Date tmpEndTime = new Date();
        String tmpTitle = "";
        String tmpLocation = "";
        String tmpDescription = "";
        String tmpActionLink = "";
        for(int i = 0; i < this.size() - 1; i++){
            min = i;
            for(int j = i+1; j < this.size(); j++){
                if(startTime.get(min).compareTo(startTime.get(j)) > 0){
                    min = j;
                }
            }
            //swap
            tmpStartTime = startTime.get(min);
            tmpEndTime = endTime.get(min);
            tmpTitle = title.get(min);
            tmpLocation = location.get(min);
            tmpDescription = description.get(min);
            tmpActionLink = actionLink.get(min);
            startTime.set(min, startTime.get(i));
            endTime.set(min, endTime.get(i));
            title.set(min, title.get(i));
            location.set(min, location.get(i));
            description.set(min, description.get(i));
            actionLink.set(min, actionLink.get(i));
            startTime.set(i, tmpStartTime);
            endTime.set(i, tmpEndTime);
            title.set(i, tmpTitle);
            location.set(i, tmpLocation);
            description.set(i, tmpDescription);
            actionLink.set(i, tmpActionLink);
        }
    }

    public int size(){
        return startTime.size();
    }
    
    public boolean remove(int index){
        if(index > this.size() - 1)
            return false;
        
        startTime.remove(index);
        endTime.remove(index);
        title.remove(index);
        location.remove(index);
        description.remove(index);
        actionLink.remove(index);
        return true;
    }
    
    public String toString(){
        String out = "";
        for(int i = 0; i < this.size(); i++){
            out += "   " + title.get(i).toString();
            out += "   " + startTime.get(i).toString();
            out += "   " + endTime.get(i).toString();
            out += "   " + location.get(i).toString();
            out += "   " + description.get(i).toString();
            out += "   " + actionLink.get(i).toString();
        }
        return out;
    }
    
    public String toHTML(){
        String out = "";
        for(int i = 0; i < this.size(); i++){
            out += "<b>[GC] <a href=" + actionLink.get(i).toString() + ">"+ title.get(i).toString() + "</a></b>";
            out += "  from " + startTime.get(i).toString().substring(11, startTime.get(this.size() - 1 - i).toString().indexOf("PDT"));
            out += "  to " + endTime.get(i).toString().substring(11, endTime.get(this.size() - 1 - i).toString().indexOf("PDT"));
            if(location.get(i).toString().equals(""))
            	out += "<br>";
            else
            	out += "  at " + location.get(i).toString() + "<br>";
        }
        return out;
    }
    
    public String getTitles(){
        String out = "";
        for(int i = 0; i < this.size(); i++){
            out += "[GC]" + title.get(i).toString() + "\n";
        }
        return out;
    }
    
    public ArrayList<Date> getStartTime() {
        return startTime;
    }

    public void setStartTime(ArrayList<Date> startTime) {
        this.startTime = startTime;
    }

    public ArrayList<Date> getEndTime() {
        return endTime;
    }

    public void setEndTime(ArrayList<Date> endTime) {
        this.endTime = endTime;
    }

    public ArrayList<String> getTitle() {
        return title;
    }

    public void setTitle(ArrayList<String> title) {
        this.title = title;
    }

    public ArrayList<String> getLocation() {
        return location;
    }

    public void setLocation(ArrayList<String> location) {
        this.location = location;
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
