##
## ICStunes:  A Music Manager
##
## Original version ("InfxTunes") in Scheme by Alex Thornton,
##      modified 2007 and 2008 by David G. Kay
## Python version by David G. Kay, 2012

from pcollections import pnamedtuple
#######################################
# Album, Song
#######################################

Album = pnamedtuple('Album', 'id artist title year songs')
# id is a unique ID number; artist and title are strings; year is a number,
#   the year the song was released; songs is a list of Songs

Song = pnamedtuple('Song', 'track title length play_count')
# track is the track number; title is a string; length is the number of
#   seconds long the song is; play_count is the number of times the user
#   has listened to the song

MUSIC = [
    Album(1, "Peter Gabriel", "Up", 2002,
        [Song(1, "Darkness", 411, 5),
         Song(2, "Growing Up", 453, 5),
         Song(3, "Sky Blue", 397, 2),
         Song(4, "No Way Out", 473, 2),
         Song(5, "I Grieve", 444, 2),
         Song(6, "The Barry Williams Show", 735, 1),
         Song(7, "My Head Sounds Like That", 389, 1),
         Song(8, "More Than This", 362, 1),
         Song(9, "Signal to Noise", 456, 2),
         Song(10, "The Drop", 179, 1)]),
    Album(2, "Simple Minds", "Once Upon a Time", 1985,
        [Song(1, "Once Upon a Time", 345, 9),
         Song(2, "All the Things She Said", 256, 10),
         Song(3, "Ghost Dancing", 285, 7),
         Song(4, "Alive and Kicking", 326, 26),
         Song(5, "Oh Jungleland", 314, 13),
         Song(6, "I Wish You Were Here", 282, 12),
         Song(7, "Sanctify Yourself", 297, 7),
         Song(8, "Come a Long Way", 307, 5)]),
    Album(3, "The Postal Service", "Give Up", 2003,
        [Song(1, "The District Sleeps Alone", 284, 13),
         Song(2, "Such Great Heights", 266, 13),
         Song(3, "Sleeping In", 261, 12),
         Song(4, "Nothing Better", 226, 18),
         Song(5, "Recycled Air", 269, 13),
         Song(6, "Clark Gable", 294, 12),
         Song(7, "We Will Become Silhouettes", 300, 11),
         Song(8, "This Place is a Prison", 234, 9),
         Song(9, "Brand New Colony", 252, 9),
         Song(10, "Natural Anthem", 307, 7)]),
    Album(4, "Midnight Oil", "Blue Sky Mining", 1989,
        [Song(1, "Blue Sky Mine", 258, 12),
         Song(2, "Stars of Warburton", 294, 11),
         Song(3, "Bedlam Bridge", 266, 11),
         Song(4, "Forgotten Years", 266, 8),
         Song(5, "Mountains of Burma", 296, 9),
         Song(6, "King of the Mountain", 231, 8),
         Song(7, "River Runs Red", 322, 9),
         Song(8, "Shakers and Movers", 268, 9),
         Song(9, "One Country", 353, 7),
         Song(10, "Antarctica", 258, 6)]),
    Album(5, "The Rolling Stones", "Let It Bleed", 1969,
        [Song(1, "Gimme Shelter", 272, 3),
         Song(2, "Love In Vain", 259, 2),
         Song(3, "Country Honk", 187, 0),
         Song(4, "Live With Me", 213, 2),
         Song(5, "Let It Bleed", 327, 2),
         Song(6, "Midnight Rambler", 412, 1),
         Song(7, "You Got the Silver", 170, 0),
         Song(8, "Monkey Man", 251, 13),
         Song(9, "You Can't Always Get What You Want", 448, 10)])
]
#######################################
# Sorting the collection
#######################################

# Sort the collection into chronological order
# The 'key=' argument of sort() takes a function---that function
#   takes an album and produces the value that will be used for
#   comparisons in the sort.
# So first we define that function

#pnamedtuple implements this functionality with Album._get_year
#def Album_year(A: Album) -> int:
#    ''' Return the album's year
#    '''
#    return A.year

MUSIC.sort(key=Album._get_year) # Oldest to newest
assert(MUSIC[0].title == "Let It Bleed") # Kind of a half-hearted test
assert(MUSIC[-1].title == "Give Up")

MUSIC.sort(key=Album._get_year, reverse=True) # Newest to oldest
assert(MUSIC[0].title == "Give Up") # Kind of a half-hearted test
assert(MUSIC[-1].title == "Let It Bleed")

#pnamedtuple implements this functionality with Album._get_title
# Sort the collection by Album title
#def Album_title(A: Album) -> str:
#    ''' Return the album's title
#    '''
#    return A.title

MUSIC.sort(key=Album._get_title)
assert(MUSIC[0].title == "Blue Sky Mining") # Kind of a half-hearted test
assert(MUSIC[-1].title == "Up")

# Sort the collection by length (playing time) of album
def Album_length(a: Album) -> int:
    ''' Return the total length of all the songs in the album
    '''
    total_length = 0
    for s in a.songs:
        total_length += s.length
    return total_length

MUSIC.sort(key=Album_length)
assert(MUSIC[0].title == "Once Upon a Time") # Kind of a half-hearted test
assert(MUSIC[-1].title == "Up")

# Sort the collection by Album id (as above)
def Album_id(A: Album) -> str:
    ''' Return the album's number
    '''
    return A.id

MUSIC.sort(key=Album_id)

## We can also write a conventional function to sort a collection, so
## we could say collection_sort(MUSIC, Album_length) instead of using
## the method notation MUSIC.sort(key=Album_length).  We do this by
## PASSING A FUNCTION AS A PARAMETER (like the interchangeable
## attachment on a robot arm).

def collection_sort(C: 'list of Album', keyfunction: 'Function on Albums') -> None:
    ''' Sort collection according to specified key function
        Note that this function, like the sort() method, sorts the collection
        IN PLACE (by reference), so it changes the argument it was called with.
        That's why it doesn't RETURN anything.
    '''
    C.sort(key=keyfunction)
    return

collection_sort(MUSIC, Album._get_title)
assert(MUSIC[0].title == "Blue Sky Mining") # Kind of a half-hearted test
assert(MUSIC[-1].title == "Up")

collection_sort(MUSIC, Album_id) # Just to put it back in the original order


#######################################
# Top 10 most frequently played songs
#######################################

# Collect all the songs out of all the albums.
# To find the MOST frequent, just use the find-largest (king-of-the-hill) algorithm
# To find the top N is hard to code that way.
# Better: Take the list of songs, sort by play_count, take first 10 -- songlist[:10]

def Song_play_count(s: Song) -> int:
    ''' Return the number of times this song has been played
    '''
    return s.play_count

def all_songs(MC: 'list of Album') -> 'list of Song':
    ''' Return a list of all the Songs in a music collection (list of Album)
    '''
    result = [ ]
    for a in MC:
        result.extend(a.songs)
    return result

Songlist = all_songs(MUSIC)
assert(Songlist[0] == Song(1, "Darkness", 411, 5))
assert(Songlist[1] == Song(2, "Growing Up", 453, 5))
assert(Songlist[-1] == Song(9, "You Can't Always Get What You Want", 448, 10))

def top_n_played_songs(MC: 'list of Album', n: int) -> 'list of Song':
    ''' Return a list of the n most frequently played songs in MC
    '''
    Songlist = all_songs(MC)
    Songlist.sort(key=Song_play_count, reverse=True)
    return Songlist[:n]

assert(top_n_played_songs(MUSIC, 5) ==
       [Song(4, "Alive and Kicking", 326, 26),
        Song(4, "Nothing Better", 226, 18),
        Song(5, "Oh Jungleland", 314, 13),
        Song(1, "The District Sleeps Alone", 284, 13),
        Song(2, "Such Great Heights", 266, 13)])


###################################
# Song-displays
###################################
# But these songs don't have their album information!  We removed it when we created
# the list of all songs.  If we want to display selected songs on our iPod screen,
# we'd want to have the album information along with the song information.

# We could flatten out our data structure, storing a copy of the album
# information with each song:
#       1   Up  Peter Gabriel  2002  1  Darkness   411   5
#       1   Up  Peter Gabriel  2002  2  Growing Up   453  8
#       1   Up  Peter Gabriel  2002  3  Sky Blue    397  2
#            ...
# This would work, but there's a lot of duplicate data---it would be wasteful of storage
# and error-prone to store our music data this way permanently.

# Instead, let's just get the album info that goes with a song WHEN WE NEED IT,
# during the computation.  To do this, we define a structure that contains the
# info we need to display a song (on our iPod screen, e.g.)---song details plus
# the info we need from that song's album:

Songdisplay = pnamedtuple('Songdisplay', 'artist a_title year track s_title length play_count')

# We'll create these structures as we need them during the computation,
# discarding them as we're done; this doesn't affect the main, permanent
# list of albums (like the one we defined as MUSIC above).

def all_Songdisplays(MC: 'list of Album') -> 'list of Songdisplay':
    ''' Return a list of all the songs in the collection MC, in Songdisplay form
    '''
    result = [ ]
    for a in MC:
        result.extend(Album_to_Songdisplays(a))
    return result

def Album_to_Songdisplays(a: Album) -> 'list of Songdisplay':
    ''' Return a list of Songdisplays, one for each song in the album
    '''
    result = [ ]
    for s in a.songs:
        result.append(Songdisplay(a.artist, a.title, a.year,
            s.track, s.title, s.length, s.play_count))
    return result

def play_count_from_songdisplay(sd: Songdisplay) -> int:
    ''' Return the play_count from a Songdisplay
    '''
    return sd.play_count

def top_n_played(MC: 'list of Album', n: int) -> 'list of Songdisplay':
    ''' Return the top n most frequently played songs in MC
    '''
    list_of_Songdisplays = all_Songdisplays(MC)
    list_of_Songdisplays.sort(key=play_count_from_songdisplay, reverse=True)
    return list_of_Songdisplays[:n]

test_list = top_n_played(MUSIC, 3)
assert(test_list[0].s_title == "Alive and Kicking")
assert(test_list[0].a_title == "Once Upon a Time")
assert(test_list[-1].s_title == "Oh Jungleland")
assert(test_list[-1].a_title == "Once Upon a Time")