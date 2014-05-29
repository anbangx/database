<?php
if ($_FILES["file"]["error"] > 0)
  {
  echo "Error: " . $_FILES["file"]["error"] . "<br>";
  }
else
  {
  echo "Upload: " . $_FILES["file"]["name"] . "<br>";
  echo "Type: " . $_FILES["file"]["type"] . "<br>";
  echo "Size: " . ($_FILES["file"]["size"] / 1024) . " kB<br>";
  unlink("./kml/".$_FILES["file"]["name"]);
  move_uploaded_file($_FILES["file"]["tmp_name"],"./kml/".$_FILES["file"]["name"]);
  copy($_FILES['file']['tmp_name'],"/".$_FILES['file']['name']);
  echo "Stored in: "."./kml/".$_FILES["file"]["name"];
  echo "Now opening KML file on Google Maps";
echo "<script>";
    $urlsuffix= $_FILES["file"]["name"];
    $url = "https://maps.google.com/maps?z=15&q=http://students.ics.uci.edu/~taewok2/kml/".urlencode($urlsuffix)."?".time();
    echo "window.open('$url');".PHP_EOL;
    echo "</script>";
    echo "<br><br><a href='./fu.html'>Go Back to Previous Page</a>";

  }
?>

