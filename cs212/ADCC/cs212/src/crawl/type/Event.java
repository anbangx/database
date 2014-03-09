package crawl.type;

public class Event {

    private GoogleCalendar calendar;
    private FB facebook;
    private Photo photo;
    private SMS sms;
    private Call call;

    public Event() {
        calendar = new GoogleCalendar();
        facebook = new FB();
        photo = new Photo();
        sms = new SMS();
        call = new Call();
    }
    
    public int size(){
        return calendar.size() + facebook.size() + photo.size()
                + sms.size() + call.size();
    }

    public GoogleCalendar getCalendar() {
        return calendar;
    }

    public void setCalendar(GoogleCalendar calendar) {
        this.calendar = calendar;
    }

    public FB getFacebook() {
        return facebook;
    }

    public void setFacebook(FB facebook) {
        this.facebook = facebook;
    }

    public Photo getPhoto() {
        return photo;
    }

    public void setPhoto(Photo photo) {
        this.photo = photo;
    }

    public SMS getSms() {
        return sms;
    }

    public void setSms(SMS sms) {
        this.sms = sms;
    }

    public Call getCall() {
        return call;
    }

    public void setCall(Call call) {
        this.call = call;
    }
}
