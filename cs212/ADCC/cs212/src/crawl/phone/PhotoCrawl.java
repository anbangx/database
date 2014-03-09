package crawl.phone;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Date;

import com.drew.imaging.ImageMetadataReader;
import com.drew.imaging.ImageProcessingException;
import com.drew.metadata.Metadata;
import com.drew.metadata.exif.ExifSubIFDDirectory;

import crawl.type.Photo;

public class PhotoCrawl {
	
	public static Photo start(File dir, Date startTime, Date endTime) throws IOException {
	    ArrayList<Date> createTime = new ArrayList<Date>();
	    ArrayList<String> type = new ArrayList<String>();
	    ArrayList<String> title = new ArrayList<String>();
	    ArrayList<String> link = new ArrayList<String>();
	    
		File[] fs = dir.listFiles(); //picture/video
		for(int i = 0; i < fs.length; i++){
//           Path path = fs[i].toPath(); // the path to the file
//            BasicFileAttributes attributes = 
//                Files.readAttributes(path, BasicFileAttributes.class);
//            FileTime creationTime = attributes.creationTime();
//            //make sure creationTime is in the range [startTime, endTime]
//            createTime.add(creationTime.toString());
		    
		 // There are multiple ways to get a Metadata object for a file

	        //
	        // SCENARIO 1: UNKNOWN FILE TYPE
	        //
	        // This is the most generic approach.  It will transparently determine the file type and invoke the appropriate
	        // readers.  In most cases, this is the most appropriate usage.  This will handle JPEG, TIFF, GIF, BMP and RAW
	        // (CRW/CR2/NEF/RW2/ORF) files and extract whatever metadata is available and understood.
	        //
	        try {
	            Metadata metadata = ImageMetadataReader.readMetadata(fs[i]);
	            
	         // obtain the Exif directory
	            ExifSubIFDDirectory directory = metadata.getDirectory(ExifSubIFDDirectory.class);
	         // query the tag's value
	            Date date = directory.getDate(ExifSubIFDDirectory.TAG_DATETIME_ORIGINAL);
	            if(date.compareTo(startTime) >= 0 && date.compareTo(endTime) <= 0){
	                createTime.add(date);
	                type.add("picture");
	                title.add(fs[i].getName());
	                link.add(fs[i].getAbsolutePath());
	            }

	        } catch (ImageProcessingException e) {
	            // handle exception
	        } catch (IOException e) {
	            // handle exception
	        }
		}
		
		Photo photo = new Photo();
		photo.setCreateTime(createTime);
		photo.setType(type);
		photo.setTitle(title);
		photo.setLink(link);
		
		return photo;
	}
	
	public static void main(String[] args) throws Exception {
        //crawl phone
        File dir = new File("src/Data"); //transfer data from android phone to PC in this directory
        Photo phone = PhotoCrawl.start(dir, null, null);
        System.out.println("Photo(Image/Video): ");
        System.out.println(phone.toString());
	}
}
