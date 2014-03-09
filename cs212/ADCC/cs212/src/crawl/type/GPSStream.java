package crawl.type;

import java.util.ArrayList;
import java.util.Date;

public class GPSStream {
    ArrayList<Integer> basePoints;
    ArrayList<Date> recordDate;
    ArrayList<Date> recordEndDate;
    ArrayList<Long> duration;
    ArrayList<Double> clusterLatitude;
    ArrayList<Double> clusterLongitude;
    ArrayList<Double> latitude;
    ArrayList<Double> longitude;
    
    public GPSStream(){
        basePoints = new ArrayList<Integer>();
        recordDate = new ArrayList<Date>();
        duration = new ArrayList<Long>();
        recordEndDate = new ArrayList<Date>();
        clusterLatitude = new ArrayList<Double>();
        clusterLongitude = new ArrayList<Double>();
        latitude = new ArrayList<Double>();
        longitude = new ArrayList<Double>();
    }

    public ArrayList<Integer> getBasePoints() {
        return basePoints;
    }

    public void setBasePoints(ArrayList<Integer> basePoints) {
        this.basePoints = basePoints;
    }

    public ArrayList<Date> getRecordDate() {
        return recordDate;
    }

    public void setRecordDate(ArrayList<Date> recordDate) {
        this.recordDate = recordDate;
    }

        public ArrayList<Long> getDuration() {
        return duration;
    }

    public void setDuration(ArrayList<Long> duration) {
        this.duration = duration;
    }

    public ArrayList<Date> getRecordEndDate() {
        return recordEndDate;
    }

    public void setRecordEndDate(ArrayList<Date> recordEndDate) {
        this.recordEndDate = recordEndDate;
    }
    
    public ArrayList<Double> getClusterLatitude() {
        return clusterLatitude;
    }
    
    public void setClusterLatitude(ArrayList<Double> clusterLatitude) {
        this.clusterLatitude = clusterLatitude;
    }
    
    public ArrayList<Double> getClusterLongitude() {
        return clusterLongitude;
    }
    
    public void setClusterLongitude(ArrayList<Double> clusterLongitude) {
        this.clusterLongitude = clusterLongitude;
    }

    public ArrayList<Double> getLongitude() {
        return longitude;
    }
    
    public void setLongitude(ArrayList<Double> longitude) {
        this.longitude = longitude;
    }
    
    public ArrayList<Double> getLatitude() {
        return latitude;
    }
    
    public void setLatitude(ArrayList<Double> latitude) {
        this.latitude = latitude;
    }


    
    public int size(){
        return basePoints.size();
    }
}
