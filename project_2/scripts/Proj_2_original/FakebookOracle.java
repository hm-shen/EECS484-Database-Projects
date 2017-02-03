package project2;

import java.io.IOException;
import java.io.Writer;
import java.sql.SQLException;
import java.util.Objects;
import java.util.TreeSet;


public abstract class FakebookOracle {

	//
	// Utility classes for storing result data
	//

	class UserInfo implements Comparable<UserInfo>{
		Long userId;
		String firstName;
		String lastName;
		public UserInfo(Long uid, String fname, String lname){
			userId = uid;
			firstName = fname;
			lastName = lname;
		}
		public String toString(){
			return firstName+" "+lastName+"("+userId+")";
		}

		public int compareTo(UserInfo arg0) {
			return userId.compareTo(arg0.userId);
		}
	}

	class PhotoInfo  implements Comparable<PhotoInfo>{
		String photoId;
		String photoCaption;
		String photoLink;
		String albumId;
		String albumName;
		public PhotoInfo(String pid, String aid, String aName, String caption, String link){
			photoId = pid;
			albumId = aid;
			albumName = aName;
			photoLink = link;
			photoCaption = caption;
		}
		public String toString(){
			return "(ID "+photoId+") from album "+albumName+" url("+photoLink+")";
		}
		public int compareTo(PhotoInfo arg0) {
			return photoId.compareTo(arg0.photoId);
		}
	}

	class TaggedPhotoInfo implements Comparable<TaggedPhotoInfo>{
		PhotoInfo photoInfo;
		TreeSet<UserInfo> tags;
		public TaggedPhotoInfo(PhotoInfo p){
			photoInfo = p;
			tags = new TreeSet<UserInfo>();
		}
		public void addTaggedUser(UserInfo u){
			tags.add(u);
		}
		public String toString(){
			return photoInfo+"\n"+tags.size()+" Tags\n"+this.tags;
		}

		public int compareTo(TaggedPhotoInfo t) {
			if(this.tags.size() == t.tags.size()){
				return this.photoInfo.compareTo(t.photoInfo);
			}
			return t.tags.size() - this.tags.size();
		}

	}

	class MatchPair implements Comparable<MatchPair>{
		Long u1Id;
		String u1FirstName;
		String u1LastName;
		int u1Year;
		Long u2Id;
		String u2FirstName;
		String u2LastName;
		int u2Year;
		TreeSet<PhotoInfo> sharedPhotos;
		public MatchPair(Long id1,String fn1, String ln1, int year1,Long id2,String fn2, String ln2, int year2){
			u1Id = id1;
			u1FirstName = fn1;
			u1LastName = ln1;
			u1Year = year1;
			u2Id = id2;
			u2FirstName = fn2;
			u2LastName = ln2;
			u2Year = year2;
			sharedPhotos = new TreeSet<PhotoInfo>();
		}
		public void addSharedPhoto(PhotoInfo p){
			sharedPhotos.add(p);
		}
		public String toString(){
			return u1FirstName+" "+u1LastName+"("+u1Id+") born in "+u1Year+", "+u2FirstName+" "+u2LastName+"("+u2Id+") born in "+u2Year+
			"\nThey are not Fakebook friends and share "+this.sharedPhotos.size()+" photos\n"+this.sharedPhotos;
		}

		public int compareTo(MatchPair arg0) {
			if(this.sharedPhotos.size() != arg0.sharedPhotos.size()){
				return arg0.sharedPhotos.size()-this.sharedPhotos.size();
			}
			if(u1Id.equals(arg0.u1Id)){
				return u2Id.compareTo(arg0.u2Id);
			}
			return u1Id.compareTo(arg0.u1Id);
		}
	}


	class UsersPair implements Comparable<UsersPair>{
		Long user1Id;
		String user1FirstName;
		String user1LastName;
		Long user2Id;
		String user2FirstName;
		String user2LastName;
		TreeSet<UserInfo> sharedFriends;
		public UsersPair(Long id1, String fn1, String ln1, Long id2, String fn2, String ln2){
			if(id1 < id2){
				user1Id = id1;
				user1FirstName = fn1;
				user1LastName = ln1;
				user2Id = id2;
				user2FirstName = fn2;
				user2LastName = ln2;
			}
			else{
				user1Id = id2;
				user1FirstName = fn2;
				user1LastName = ln2;
				user2Id = id1;
				user2FirstName = fn1;
				user2LastName = ln1;
			}
			sharedFriends = new TreeSet<UserInfo>();
		}
		public void addSharedFriend(Long uid, String firstName, String lastName){
			sharedFriends.add(new UserInfo(uid, firstName, lastName));
		}
		public String toString(){
			return user1FirstName+" "+user1LastName+"("+user1Id+") "+user2FirstName+" "+user2LastName+"("+user2Id+") are not Fakebook friends and they share "+this.sharedFriends.size()+" friends\n"+
			this.sharedFriends;
		}

		public int compareTo(UsersPair arg0) {
			if(this.sharedFriends.size() != arg0.sharedFriends.size()){
				return arg0.sharedFriends.size() - this.sharedFriends.size();
			}
			if(user1Id == arg0.user1Id){
				return user2Id.compareTo(arg0.user2Id);
			}
			return user1Id.compareTo(arg0.user1Id);
		}
	}


	class SiblingInfo implements Comparable<SiblingInfo>{
		Long userId1, userId2;
		String firstName1, firstName2;
		String lastName1, lastName2;

		public SiblingInfo(Long uid1, String fname1, String lname1,
				   Long uid2, String fname2, String lname2){
		    userId1 = uid1;
		    firstName1 = fname1;
		    lastName1 = lname1;
		    userId2 = uid2;
		    firstName2 = fname2;
		    lastName2 = lname2;
		}
		public String toString(){
		    return firstName1+" "+lastName1+"("+userId1+") and "+
			firstName2+" "+lastName2+"("+userId2+")";
		}
		public int compareTo(SiblingInfo s) {
		    if(userId1.compareTo(s.userId1)==0){
			return userId2.compareTo(s.userId2);
		    }
		    return userId1.compareTo(s.userId1);
		}
    }

	//
	// Result set data structures
	//

	/*Query 0 data structures*/
	protected int totalUsersWithMonthOfBirth;
	protected int monthOfMostUsers;
	protected TreeSet<UserInfo> usersInMonthOfMost;
	protected int monthOfLeastUsers;
	protected TreeSet<UserInfo> usersInMonthOfLeast;

	public final void printMonthOfBirthInfo(Writer writer) throws IOException{
		writer.write(this.totalUsersWithMonthOfBirth + " users with month of birth information"+"\n");
		writer.write(this.usersInMonthOfMost.size()+ " users in month of " + monthOfMostUsers +"(most)\n");
		writer.write(this.usersInMonthOfMost +"\n");
		writer.write(this.usersInMonthOfLeast.size()+ " users in month of " + monthOfLeastUsers +"(least)\n");
		writer.write(this.usersInMonthOfLeast +"\n");
		writer.flush();
	}


	/*Query 1 data structures*/
	protected TreeSet<String> longestFirstNames;
	protected TreeSet<String> shortestFirstNames;
	protected TreeSet<String> mostCommonFirstNames;
	protected int mostCommonFirstNamesCount;

	public final void printNameInfo(Writer writer) throws IOException{
		writer.write("Longest first names are "+this.longestFirstNames+"\n");
		writer.write("Shortest first names are "+this.shortestFirstNames+"\n");
		writer.write("Most common first names are "+this.mostCommonFirstNames+" - having "+this.mostCommonFirstNamesCount+" users for each\n");
		writer.flush();
	}


	/*Query 2 data structures*/
	protected TreeSet<UserInfo> lonelyUsers;

	public final void printLonelyFriends(Writer writer) throws IOException {
		writer.write("Number of lonely users: " + this.lonelyUsers.size() + "\n");
		writer.write("Lonely users are " + this.lonelyUsers + "\n");
		writer.flush();
	}


	/* Query 3 data structures */
	protected TreeSet<UserInfo> liveAwayFromHome;

	public final void printLiveAwayFromHome(Writer writer) throws IOException {
		writer.write("Number of users who live away from home: " + this.liveAwayFromHome.size() + "\n");
		writer.write("These users still live away from home " + this.liveAwayFromHome + "\n");
		writer.flush();
	}


	/*Query 4 data structures */
	protected TreeSet<TaggedPhotoInfo> photosWithMostTags;

	public final void printPhotosWithMostTags(Writer writer) throws IOException{
		writer.write("The following are top "+this.photosWithMostTags.size()+" photos with the most tags\n");
		for(TaggedPhotoInfo t:this.photosWithMostTags){
			writer.write(t+"\n");
		}
		writer.flush();
	}


	/*Query 5 data structures*/
	protected TreeSet<MatchPair> bestMatches;

	public final void printBestMatches(Writer writer) throws IOException{
		writer.write("Top "+this.bestMatches.size()+" matches\n");
		for(MatchPair match:this.bestMatches){
			writer.write(match+"\n");
		}
		writer.flush();
	}


	/*Query 6 data structures*/
	protected TreeSet<UsersPair> suggestedUsersPairs;

	public final void printMutualFriendsInfo(Writer writer) throws IOException {
		for (UsersPair p : this.suggestedUsersPairs) {
			writer.write(p + "\n");
		}
		writer.flush();
	}

	/*Query 7 data structures*/
	protected TreeSet<String> popularStateNames;
	protected int eventCount;

	public final void printStateNames(Writer writer) throws IOException {
		writer.write("State/States with most events: " + popularStateNames + "\n");
		writer.write("Total events in each state: " + eventCount + "\n");
		writer.flush();
	}

	/*Query 8 data structures*/
	protected UserInfo oldestFriend;
	protected UserInfo youngestFriend;

	public final void printAgeInfo(Writer writer) throws IOException {
		writer.write("Oldest friend: " + oldestFriend + "\n");
		writer.write("Youngest friend: " + youngestFriend + "\n");
		writer.flush();
	}


	/*Query 9 data structures*/
	protected TreeSet<SiblingInfo> siblings;
	public final void printPotentialSiblings(Writer writer) throws IOException {
		writer.write(this.siblings.size()+" pairs of siblings\n");
		for (SiblingInfo s : this.siblings) {
		    writer.write(s + "\n");
			}
		writer.flush();
    }

	// Constructor
	protected FakebookOracle(){
		// Query 0
		usersInMonthOfMost = new TreeSet<UserInfo>();
		usersInMonthOfLeast = new TreeSet<UserInfo>();

		// Query 1
		longestFirstNames = new TreeSet<String>();
		shortestFirstNames = new TreeSet<String>();
		mostCommonFirstNames = new TreeSet<String>();

		// Query 2
		lonelyUsers = new TreeSet<UserInfo>();

		// Query 3
		liveAwayFromHome = new TreeSet<UserInfo>();

		// Query 4
		photosWithMostTags = new TreeSet<TaggedPhotoInfo>();

		// Query 5
		bestMatches = new TreeSet<MatchPair>();

		// Query 6
		suggestedUsersPairs = new TreeSet<UsersPair>();

		// Query 7
		popularStateNames = new TreeSet<String>();
		eventCount = 0;

		// Query 8
		oldestFriend = new UserInfo(0L, "","");
		youngestFriend = new UserInfo(0L, "", "");

		// Query 9
		siblings = new TreeSet<SiblingInfo>();
	}


	public abstract void findMonthOfBirthInfo() throws SQLException;
	public abstract void findNameInfo() throws SQLException;
	public abstract void lonelyUsers() throws SQLException;
	public abstract void liveAwayFromHome() throws SQLException;
	public abstract void findPhotosWithMostTags(int n) throws SQLException;
	public abstract void matchMaker(int n, int yearDiff) throws SQLException;
	public abstract void suggestFriendsByMutualFriends(int n) throws SQLException;
	public abstract void findEventStates() throws SQLException;
	public abstract void findAgeInfo(Long user_id) throws SQLException;
	public abstract void findPotentialSiblings() throws SQLException;


}
