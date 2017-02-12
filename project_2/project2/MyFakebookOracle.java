package project2;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;

public class MyFakebookOracle extends FakebookOracle {

    static String prefix = "syzhao.";

    // You must use the following variable as the JDBC connection
    Connection oracleConnection = null;

    // You must refer to the following variables for the corresponding tables in your database
    String cityTableName = null;
    String userTableName = null;
    String friendsTableName = null;
    String currentCityTableName = null;
    String hometownCityTableName = null;
    String programTableName = null;
    String educationTableName = null;
    String eventTableName = null;
    String participantTableName = null;
    String albumTableName = null;
    String photoTableName = null;
    String coverPhotoTableName = null;
    String tagTableName = null;


    // DO NOT modify this constructor
    public MyFakebookOracle(String dataType, Connection c) {
        super();
        oracleConnection = c;
        // You will use the following tables in your Java code
        cityTableName = prefix + dataType + "_CITIES";
        userTableName = prefix + dataType + "_USERS";
        friendsTableName = prefix + dataType + "_FRIENDS";
        currentCityTableName = prefix + dataType + "_USER_CURRENT_CITY";
        hometownCityTableName = prefix + dataType + "_USER_HOMETOWN_CITY";
        programTableName = prefix + dataType + "_PROGRAMS";
        educationTableName = prefix + dataType + "_EDUCATION";
        eventTableName = prefix + dataType + "_USER_EVENTS";
        albumTableName = prefix + dataType + "_ALBUMS";
        photoTableName = prefix + dataType + "_PHOTOS";
        tagTableName = prefix + dataType + "_TAGS";
    }


    @Override
    // ***** Query 0 *****
    // This query is given to your for free;
    // You can use it as an example to help you write your own code
    //
    public void findMonthOfBirthInfo() {

        // Scrollable result set allows us to read forward (using next())
        // and also backward.
        // This is needed here to support the user of isFirst() and isLast() methods,
        // but in many cases you will not need it.
        // To create a "normal" (unscrollable) statement, you would simply call
        // Statement stmt = oracleConnection.createStatement();
        //
        try (Statement stmt =
                     oracleConnection.createStatement(ResultSet.TYPE_SCROLL_INSENSITIVE,
                             ResultSet.CONCUR_READ_ONLY)) {

            // For each month, find the number of users born that month
            // Sort them in descending order of count
            ResultSet rst = stmt.executeQuery("select count(*), month_of_birth from " +
                    userTableName +
                    " where month_of_birth is not null group by month_of_birth order by 1 desc");

            this.monthOfMostUsers = 0;
            this.monthOfLeastUsers = 0;
            this.totalUsersWithMonthOfBirth = 0;

            // Get the month with most users, and the month with least users.
            // (Notice that this only considers months for which the number of users is > 0)
            // Also, count how many total users have listed month of birth (i.e., month_of_birth not null)
            //
            while (rst.next()) {
                int count = rst.getInt(1);
                int month = rst.getInt(2);
                if (rst.isFirst())
                    this.monthOfMostUsers = month;
                if (rst.isLast())
                    this.monthOfLeastUsers = month;
                this.totalUsersWithMonthOfBirth += count;
            }

            // Get the names of users born in the "most" month
            rst = stmt.executeQuery("select user_id, first_name, last_name from " +
                    userTableName + " where month_of_birth=" + this.monthOfMostUsers);
            while (rst.next()) {
                Long uid = rst.getLong(1);
                String firstName = rst.getString(2);
                String lastName = rst.getString(3);
                this.usersInMonthOfMost.add(new UserInfo(uid, firstName, lastName));
            }

            // Get the names of users born in the "least" month
            rst = stmt.executeQuery("select first_name, last_name, user_id from " +
                    userTableName + " where month_of_birth=" + this.monthOfLeastUsers);
            while (rst.next()) {
                String firstName = rst.getString(1);
                String lastName = rst.getString(2);
                Long uid = rst.getLong(3);
                this.usersInMonthOfLeast.add(new UserInfo(uid, firstName, lastName));
            }

            // Close statement and result set
            rst.close();
            stmt.close();
        } catch (SQLException err) {
            System.err.println(err.getMessage());
        }
    }

    @Override
    // ***** Query 1 *****
    // Find information about users' names:
    // (1) The longest first name (if there is a tie, include all in result)
    // (2) The shortest first name (if there is a tie, include all in result)
    // (3) The most common first name, and the number of times it appears (if there
    //      is a tie, include all in result)
    //
    public void findNameInfo() 
    {   // Query1
        // Find the following information from your database and store the information as shown
        
        // Build connection
        try (Statement stmt = oracleConnection.createStatement()) 
        {
            // Find the maximum number of appearance of a first name 
            ResultSet rst = stmt.executeQuery(
            " SELECT DISTINCT U.first_name " + 
            " FROM " + userTableName + " U " + 
            " WHERE length(U.first_name) = (SELECT MAX(length(U1.first_name)) " + 
                                          " FROM " + userTableName + " U1) " + 
            " ORDER BY U.first_name ");

            while (rst.next()) 
            {
                this.longestFirstNames.add(rst.getString(1));
            }

            rst = stmt.executeQuery(
            " SELECT DISTINCT U.first_name " + 
            " FROM " + userTableName + " U " + 
            " WHERE length(U.first_name) = (SELECT MIN(length(U1.first_name)) " + 
                                          " FROM " + userTableName + " U1) " + 
            " ORDER BY U.first_name ");

            while (rst.next()) 
            {
                this.shortestFirstNames.add(rst.getString(1));
            }

            String queryView = " CREATE VIEW common_name AS " +
                               " SELECT U.first_name AS name, COUNT(*) AS cont " + 
                               " From " + userTableName + " U " + 
                               " GROUP BY U.first_name " + 
                               " ORDER BY COUNT(U.first_name) DESC ";

            String queryAns = " SELECT cn.name, cn.cont" + 
                              " FROM common_name cn " + 
                              " WHERE cn.cont = (SELECT MAX(cn1.cont) FROM common_name cn1) " + 
                              " ORDER BY cn.name ASC ";

            String queryDrop = " DROP VIEW common_name ";
            
            stmt.executeQuery(queryView);
            rst = stmt.executeQuery(queryAns);
            while(rst.next())
            {
                this.mostCommonFirstNames.add(rst.getString(1));
                this.mostCommonFirstNamesCount = rst.getInt(2);
            }
            stmt.executeQuery(queryDrop);
                
            // Close statement and result set
            rst.close();
            stmt.close();
        } catch (SQLException err) 
        {
            System.err.println(err.getMessage());
        }
    }

    @Override
    // ***** Query 2 *****
    // Find the user(s) who have no friends in the network
    //
    // Be careful on this query!
    // Remember that if two users are friends, the friends table
    // only contains the pair of user ids once, subject to
    // the constraint that user1_id < user2_id
    //
    public void lonelyUsers() {
        // Find the following information from your database and store the information as shown
//         this.lonelyUsers.add(new UserInfo(10L, "Billy", "SmellsFunny"));
//         this.lonelyUsers.add(new UserInfo(11L, "Jenny", "BadBreath"));
        try (Statement stmt = oracleConnection.createStatement()) 
        {
            ResultSet rst = stmt.executeQuery(
                            " SELECT DISTINCT U.user_id, U.first_name, U.last_name " + 
                            " FROM " + userTableName + " U " +
                            " WHERE U.user_id NOT IN ( SELECT F1.user1_id " +
                                                     " FROM " + friendsTableName + " F1 " + 
                                                     " UNION " + 
                                                     " SELECT F2.user2_id " +
                                                     " FROM " + friendsTableName + " F2) ");
            while(rst.next())
            { 
                Long user_id = rst.getLong(1);
                String first_name = rst.getString(2);
                String last_name = rst.getString(3); 
                this.lonelyUsers.add(new UserInfo(user_id, first_name, last_name));
            }
            rst.close();
            stmt.close(); 
        }
        catch (SQLException err)
        {
            System.err.println(err.getMessage());
        }
    }

    @Override
    // ***** Query 3 *****
    // Find the users who do not live in their hometowns
    // (I.e., current_city != hometown_city)
    //
    public void liveAwayFromHome() throws SQLException {
//         this.liveAwayFromHome.add(new UserInfo(11L, "Heather", "Movalot"));
          try (Statement stmt = oracleConnection.createStatement()) 
          {
              ResultSet rst = stmt.executeQuery(" SELECT DISTINCT U.user_id, U.first_name, U.last_name " + 
                                                " FROM " + userTableName + " U, " + currentCityTableName + " C, " + hometownCityTableName + " H " + 
                                                " WHERE U.user_id = C.user_id " + 
                                                " AND U.user_id = H.user_id " + 
                                                " AND C.current_city_id <> H.hometown_city_id " + 
                                                " AND C.current_city_id IS NOT NULL " + 
                                                " AND H.hometown_city_id IS NOT NULL " +
                                                " ORDER BY U.user_id ") ;
                                                
              while(rst.next())
              { 
                  Long user_id = rst.getLong(1);
                  String first_name = rst.getString(2);
                  String last_name = rst.getString(3); 
                  this.liveAwayFromHome.add(new UserInfo(user_id, first_name, last_name));
              }
              rst.close();
              stmt.close(); 
          }
          catch (SQLException err)
          {
              System.err.println(err.getMessage());
          }
    }

    @Override
    // **** Query 4 ****
    // Find the top-n photos based on the number of tagged users
    // If there are ties, choose the photo with the smaller numeric PhotoID first
    //
    public void findPhotosWithMostTags(int n) 
    {
	       try (Statement stmt = oracleConnection.createStatement()) 
         {
	    	     stmt.executeUpdate("create view tag_info as select T.tag_photo_id,count(T.tag_photo_id) as cont" + " from "
	    				+ tagTableName + " T group by T.tag_photo_id order by cont DESC, T.tag_photo_id ASC");

	    		   String q = "select U.user_id, U.first_name, U.last_name,P.photo_id, P.album_id, A.album_name, P.photo_caption, P.photo_link"
	    				+ " from " + userTableName + " U, " + photoTableName + " P, " + albumTableName + " A, tag_info TI, "
	    				+ tagTableName + " T "
	    				+ " where TI.tag_photo_id = T.tag_photo_id and T.tag_photo_id = P.photo_id and P.album_id = A.album_id and T.tag_subject_id = U.user_id "
	    				+ " and TI.tag_photo_id in (select T1.tag_photo_id from tag_info T1 where rownum <= " + n + " )"
	    				+ " order by TI.cont DESC, T.tag_photo_id ASC";

	    		   ResultSet rst = stmt.executeQuery(q);

	    		   String photoId_old = "";
	    		   PhotoInfo p = new PhotoInfo("", "", "", "", "");
	    		   TaggedPhotoInfo tp = new TaggedPhotoInfo(p);

	    		   while (rst.next()) 
             {
	    		       String photoId = rst.getString(4);
	    		       String albumId = rst.getString(5);
	    		       String albumName = rst.getString(6);
	    		       String photoCaption = rst.getString(7);
	    		       String photoLink = rst.getString(8);
	    		       if (!photoId_old.equals(photoId)) 
                 {
	    		          if (photoId_old != "") 
                    {
	    		       		    this.photosWithMostTags.add(tp);
	    		       	  }
	    		       	  p = new PhotoInfo(photoId, albumId, albumName, photoCaption, photoLink);
	    		       	  tp = new TaggedPhotoInfo(p);
	    		       	  photoId_old = photoId;
	    		       }
	    		       long userId = rst.getLong(1);
	    		       String fn = rst.getString(2);
	    		       String ln = rst.getString(3);
	    		       tp.addTaggedUser(new UserInfo(userId, fn, ln));
	    		   }

	    		   this.photosWithMostTags.add(tp);
	    		   stmt.executeUpdate("drop view tag_info");

	    		   rst.close();
	    		   stmt.close();
	    	} catch (SQLException err) 
        {
	    		System.err.println(err.getMessage());
	    	}
    }

    @Override
    // **** Query 5 ****
    // Find suggested "match pairs" of users, using the following criteria:
    // (1) Both users should be of the same gender
    // (2) They should be tagged together in at least one photo (They do not have to be friends of the same person)
    // (3) Their age difference is <= yearDiff (just compare the years of birth for this)
    // (4) They are not friends with one another
    //
    // You should return up to n "match pairs"
    // If there are more than n match pairs, you should break ties as follows:
    // (i) First choose the pairs with the largest number of shared photos
    // (ii) If there are still ties, choose the pair with the smaller user1_id
    // (iii) If there are still ties, choose the pair with the smaller user2_id
    //
    public void matchMaker(int n, int yearDiff) 
    {
		    String q1 = "create view share_photo as select U1.user_id as u1id, U2.user_id as u2id ,count(*) as cont"
		    		+ " from " + userTableName + " U1, " + userTableName + " U2, " + tagTableName + " T1, " + tagTableName + " T2 "
		    		+ " where U1.user_id = T1.tag_subject_id and U2.user_id = T2.tag_subject_id and T1.tag_photo_id = T2.tag_photo_id "
		    		+ " and U1.user_id < U2.user_id and U1.gender = U2.gender "
		    		+ " and U1.year_of_birth is not null and U2.year_of_birth is not null and abs(U1.year_of_birth - U2.year_of_birth) < = " + yearDiff
		    		+ " group by U1.user_id,U2.user_id order by cont DESC, u1id ASC, u2id ASC";
		    
		    String q2 = "select U1.user_id, U1.first_name, U1.last_name, U1.year_of_birth, U2.user_id, U2.first_name, U2.last_name, U2.year_of_birth, P.photo_id, P.album_id, A.album_name, P.photo_caption, P.photo_link"
		    		+ " from " + userTableName + " U1, " + userTableName + " U2, " + " share_photo s, " + photoTableName + " P, " + albumTableName + " A, " + tagTableName + " T1, " + tagTableName + " T2 " 
		    		+ " where (U1.user_id, U2.user_id )in (select s.u1id, s.u2id from share_photo s) "
		    		+ " and s.u1id = U1.user_id and s.u2id = U2.user_id "
		    		+ " and U1.user_id = T1.tag_subject_id and U2.user_id = T2.tag_subject_id and T1.tag_photo_id = T2.tag_photo_id "
		    		+ " and not exists( select F.user1_id ,F.user2_id from " + friendsTableName + " F where U1.user_id = F.user1_id and U2.user_id = F.user2_id) "
		    		+ " and T1.tag_photo_id = P.photo_id and P.album_id = A.album_id and rownum <= " + n 
		    		+ " order by s.cont DESC ,U1.user_id ASC,  U2.user_id ASC" ;
		    
		    String q3 = "drop view share_photo";
		    
		    try (Statement stmt = oracleConnection.createStatement()) 
        {
		        stmt.executeUpdate(q1);
		    	  ResultSet rst = stmt.executeQuery(q2);
		    	
		    	  Long oldid1 = -1L;
		    	  Long oldid2 = -1L;
		    	  MatchPair mp = new MatchPair(oldid1, "", "", 0, oldid2, "", "",0);			
		    	  while(rst.next())
		    	  {
		    	    	long u1UserId = rst.getLong(1);
		    	    	long u2UserId= rst.getLong(5);
		    	    	if ( u1UserId!=oldid1 || u2UserId!= oldid2)
		    	    	{
		    	    		  if(oldid1!=-1L)
		    	    		  {this.bestMatches.add(mp);}
		    	    		  String u1FirstName = rst.getString(2);
		    	    		  String u1LastName = rst.getString(3);
		    	    		  int u1Year = rst.getInt(4);
		    	    		  String u2FirstName = rst.getString(6);
		    	    		  String u2LastName = rst.getString(7);
		    	    		  int u2Year = rst.getInt(8);
		    	    		  mp = new MatchPair(u1UserId, u1FirstName, u1LastName, u1Year, u2UserId, u2FirstName, u2LastName,u2Year);
		    	    		  oldid1 = u1UserId; oldid2= u2UserId;
		    	    	}
		    	    	String sharedPhotoId = rst.getString(9);
		    	    	String sharedPhotoAlbumId = rst.getString(10);
		    	    	String sharedPhotoAlbumName = rst.getString(11);
		    	    	String sharedPhotoCaption =rst.getString(12);
		    	    	String sharedPhotoLink = rst.getString(13);
		    	    	mp.addSharedPhoto(new PhotoInfo(sharedPhotoId, sharedPhotoAlbumId, sharedPhotoAlbumName, sharedPhotoCaption,
		    	  			sharedPhotoLink));			
		    	  }
		    	  this.bestMatches.add(mp);
		    	  stmt.executeUpdate(q3);		
		    	  rst.close();
		    	  stmt.close();
		    } catch (SQLException err) {
		    	System.err.println(err.getMessage());
		    }
    }

    // **** Query 6 ****
    // Suggest users based on mutual friends
    //
    // Find the top n pairs of users in the database who have the most
    // common friends, but are not friends themselves.
    //
    // Your output will consist of a set of pairs (user1_id, user2_id)
    // No pair should appear in the result twice; you should always order the pairs so that
    // user1_id < user2_id
    //
    // If there are ties, you should give priority to the pair with the smaller user1_id.
    // If there are still ties, give priority to the pair with the smaller user2_id.
    //
    @Override
    public void suggestFriendsByMutualFriends(int n) 
    {
         try (Statement stmt = oracleConnection.createStatement()) 
         {
             Statement stmt2 = oracleConnection.createStatement();
 
             stmt.executeQuery(" CREATE OR REPLACE VIEW common_friends AS " + 
                               " SELECT temp.user1_id AS user1_id, temp.user2_id AS user2_id, COUNT(*) AS cont" + 
                               " FROM " +  
                               " (SELECT F1.user1_id AS user1_id, F2.user2_id AS user2_id " + 
                               " FROM " + friendsTableName + " F1, " + friendsTableName + " F2 " + 
                               " WHERE F1.user2_id = F2.user1_id " + 
                               " AND (F1.user1_id, F2.user2_id) NOT IN (SELECT DISTINCT * FROM " + friendsTableName + " F3) " +
                               " UNION ALL " + 
                               " SELECT F1.user1_id AS user1_id, F2.user1_id AS user2_id " + 
                               " FROM " + friendsTableName + " F1, " + friendsTableName + " F2 " + 
                               " WHERE F1.user2_id = F2.user2_id AND F1.user1_id < F2.user1_id" + 
                               " AND (F1.user1_id, F2.user1_id) NOT IN (SELECT DISTINCT * FROM " + friendsTableName + " F3) " +
                               " UNION ALL " + 
                               " SELECT F1.user2_id AS user1_id, F2.user2_id AS user2_id " + 
                               " FROM " + friendsTableName + " F1, " + friendsTableName + " F2 " + 
                               " WHERE F1.user1_id = F2.user1_id AND F1.user2_id < F2.user2_id" + 
                               " AND (F1.user2_id, F2.user2_id) NOT IN (SELECT DISTINCT * FROM " + friendsTableName + " F3) " +
                               " ) temp " +
                               " GROUP BY temp.user1_id, temp.user2_id " +
                               " ORDER BY COUNT(*) DESC, temp.user1_id ASC, temp.user2_id ASC ");
             
             ResultSet rst = stmt.executeQuery(" SELECT CF.user1_id, CF.user2_id, U1.first_name, U1.last_name, U2.first_name, U2.last_name " + 
                                               " FROM " +
                                               " (SELECT comf.user1_id, comf.user2_id, comf.cont " + 
                                               " FROM common_friends comf " + 
                                               " WHERE ROWNUM <= " + Integer.toString(n) + " ) CF, " +
                                               userTableName + " U1, " + userTableName + " U2 " + 
                                               " WHERE U1.user_id = CF.user1_id AND U2.user_id = CF.user2_id " + 
                                               " ORDER BY CF.cont DESC, CF.user1_id, CF.user2_id "); 
 
             while(rst.next())
             { 
                 Long user1_id = rst.getLong(1);
                 Long user2_id = rst.getLong(2);
                 String user1FirstName = rst.getString(3);
                 String user1LastName = rst.getString(4);
                 String user2FirstName = rst.getString(5);
                 String user2LastName = rst.getString(6); 
                 UsersPair p = new UsersPair(user1_id, user1FirstName, user1LastName, user2_id, user2FirstName, user2LastName);
               
                 ResultSet rst_frd = stmt2.executeQuery(" SELECT U.user_id, U.first_name, U.last_name FROM " + userTableName + " U, " +
                                                        " ( " + 
                                                        " SELECT F1.user2_id AS user_id" + 
                                                        " FROM " + friendsTableName + " F1, " + friendsTableName + " F2 " +
                                                        " WHERE (F1.user1_id = " + Long.toString(user1_id) + " AND F2.user2_id = " + Long.toString(user2_id) + 
                                                        " AND F1.user2_id = F2.user1_id) " + 
                                                        " UNION ALL " + 
                                                        " SELECT F1.user1_id AS user_id" + 
                                                        " FROM " + friendsTableName + " F1, " + friendsTableName + " F2 " +
                                                        " WHERE (F1.user2_id = " + Long.toString(user1_id) + " AND F2.user2_id = " + Long.toString(user2_id) + 
                                                        " AND F1.user1_id = F2.user1_id) " +
                                                        " UNION ALL " + 
                                                        " SELECT F1.user2_id AS user_id" + 
                                                        " FROM " + friendsTableName + " F1, " + friendsTableName + " F2 " +
                                                        " WHERE (F1.user1_id = " + Long.toString(user1_id) + " AND F2.user1_id = " + Long.toString(user2_id) + 
                                                        " AND F1.user2_id = F2.user2_id) " + 
                                                        " ) FRD " +
                                                        " WHERE U.user_id = FRD.user_id " + 
                                                        " ORDER BY U.user_id " ); 
                
                while(rst_frd.next())
                {
                    p.addSharedFriend(rst_frd.getLong(1), rst_frd.getString(2), rst_frd.getString(3));
                }
                rst_frd.close();
                this.suggestedUsersPairs.add(p);
             }

             stmt.executeQuery(" DROP VIEW common_friends ");

             rst.close();
             stmt.close(); 
         }
         catch (SQLException err)
         {
             System.err.println(err.getMessage());
         }
    }

    @Override
    // ***** Query 7 *****
    //
    // Find the name of the state with the most events, as well as the number of
    // events in that state.  If there is a tie, return the names of all of the (tied) states.
    //
    public void findEventStates() 
    {
          try (Statement stmt = oracleConnection.createStatement()) 
          {

              stmt.executeQuery(" CREATE VIEW state_count AS " + 
                                " SELECT C.state_name AS state_name, COUNT(*) AS cont " + 
                                " FROM " +  eventTableName + " E, " + cityTableName + " C " + 
                                " WHERE C.city_id = E.event_city_id " + 
                                " GROUP BY C.state_name " +
                                " ORDER BY COUNT(*) DESC ");
              
              ResultSet rst = stmt.executeQuery(" SELECT S.state_name, S.cont " + 
                                                " FROM state_count S " + 
                                                " WHERE S.cont = (SELECT MAX(S1.cont) " + 
                                                                 " FROM state_count S1) ") ;

              while(rst.next())
              { 
                  this.popularStateNames.add(rst.getString(1));
                  this.eventCount = rst.getInt(2);
              }

              stmt.executeQuery(" DROP VIEW state_count ");

              rst.close();
              stmt.close(); 
          }
          catch (SQLException err)
          {
              System.err.println(err.getMessage());
          }
    }

    //@Override
    // ***** Query 8 *****
    // Given the ID of a user, find information about that
    // user's oldest friend and youngest friend
    //
    // If two users have exactly the same age, meaning that they were born
    // on the same day, then assume that the one with the larger user_id is older
    //
    public void findAgeInfo(Long user_id) 
    {
		    try (Statement stmt = oracleConnection.createStatement(ResultSet.TYPE_SCROLL_INSENSITIVE,ResultSet.CONCUR_READ_ONLY)) 
        {
            String queryView = " CREATE VIEW all_friends AS" +
                               " SELECT U1.user_id AS user_id " + 
                               " FROM " + userTableName + " U1, " + userTableName + " U, " + friendsTableName + " F1 " +
                               " WHERE U.user_id = " + Long.toString(user_id) + 
                               " AND U1.year_of_birth IS NOT NULL " + 
                               " AND ((U1.user_id = F1.user2_id AND U.user_id = F1.user1_id) " +
                               " OR (U1.user_id = F1.user1_id AND U.user_id = F1.user2_id)) " +
                               " ORDER BY U1.year_of_birth ASC, U1.month_of_birth ASC, U1.day_of_birth ASC, U1.user_id DESC ";

            String queryAns = " SELECT U.user_id, U.first_name, U.last_name " + 
                              " FROM all_friends af, " + userTableName + " U " + 
                              " WHERE U.user_id = af.user_id ";

            stmt.executeUpdate(queryView);
            ResultSet rst = stmt.executeQuery(queryAns);
            
            while (rst.next()) 
            {
                if (rst.isFirst())
                {
                    long oldid = rst.getLong(1);
                    String oldF = rst.getString(2);
                    String oldL = rst.getString(3);
		                this.oldestFriend = new UserInfo(oldid, oldF, oldL);
                }

                if (rst.isLast())
                {
                    long youngid = rst.getLong(1);
                    String youngF = rst.getString(2);
                    String youngL = rst.getString(3);
		                this.youngestFriend = new UserInfo(youngid, youngF, youngL);
                }
            }

		        stmt.executeUpdate(" DROP VIEW all_friends ");
		        rst.close();
		        stmt.close();
		    } catch (SQLException err) 
        {
		        System.err.println(err.getMessage());
		    }

    }
		    //try (Statement stmt = oracleConnection.createStatement(ResultSet.TYPE_SCROLL_INSENSITIVE,ResultSet.CONCUR_READ_ONLY)) {
		   	// 
		    //String q1 = "create view his_friend as select U.user_id as whos_friend_id, U1.user_id as friends_id "
		   	//	 + " from " + userTableName + " U, " + userTableName + " U1, " + friendsTableName + " F "
		   	//	 + " where U.user_id = " + user_id + " and ((U.user_id = F.user1_id and U1.user_id = F.user2_id) or (U.user_id = F.user2_id and U1.user_id = F.user1_id))";
		    //
		    //stmt.executeUpdate(q1);
		    //
		    //String q2 = "select U.user_id, U.first_name,U.last_name "
		   	//	 + " from " + userTableName + " U, " + " his_friend hf where U.year_of_birth = "
		   	//	 + " (select min(U2.year_of_birth) from " + userTableName + " U2,his_friend hf1 where U2.year_of_birth is not null and U2.user_id = hf1.friends_id) "
		   	//	 + "and U.user_id = hf.friends_id and rownum = 1 order by U.month_of_birth ASC, U.day_of_birth ASC, U.user_id DESC";		 
		    //ResultSet rst = stmt.executeQuery(q2);		 
		    //if(rst.first())
		    //{
		    //    long oldid = rst.getLong(1);
		    //    String oldF = rst.getString(2);
		    //    String oldL = rst.getString(3);
		    //    this.oldestFriend = new UserInfo(oldid, oldF, oldL);
		    //}
		    //
		    //String q3 = "select U.user_id, U.first_name,U.last_name "
		   	//	 + " from " + userTableName + " U, " + " his_friend hf where U.year_of_birth = "
		   	//	 + " (select max(U2.year_of_birth) from " + userTableName + " U2,his_friend hf1 where U2.year_of_birth is not null and U2.user_id = hf1.friends_id) "
		   	//	 + "and U.user_id = hf.friends_id and rownum = 1 order by U.month_of_birth DESC, U.day_of_birth DESC, U.user_id ASC";
		    //if(rst.first())
		    //{
		    //    long youngid = rst.getLong(1);
		    //    String youngF = rst.getString(2);
		    //    String youngL = rst.getString(3);
		    //    this.youngestFriend = new UserInfo(youngid, youngF, youngL);
		    //}
		    //stmt.executeUpdate("drop view his_friend");
		    //rst.close();
		    //stmt.close();
		    //} catch (SQLException err) 
        //{
		    //    System.err.println(err.getMessage());
		    //}

    @Override
    //	 ***** Query 9 *****
    //
    // Find pairs of potential siblings.
    //
    // A pair of users are potential siblings if they have the same last name and hometown, if they are friends, and
    // if they are less than 10 years apart in age.  Pairs of siblings are returned with the lower user_id user first
    // on the line.  They are ordered based on the first user_id and in the event of a tie, the second user_id.
    //
    //
    public void findPotentialSiblings() 
    {
		    try (Statement stmt = oracleConnection.createStatement()) 
        {
		    	  String q = "select U1.user_id,U1.first_name,U1.last_name,U2.user_id,U2.first_name,U2.last_name " + " from "
		    			+ userTableName + " U1, " + userTableName + " U2, " + friendsTableName + " F, "
		    			+ hometownCityTableName + " UH1, " + hometownCityTableName + " UH2 "
		    			+ " where U1.user_id < U2.user_id and U1.last_name = U2.last_name and ((U1.user_id = F.user1_id and U2.user_id = F.user2_id) or (U1.user_id = F.user2_id and U2.user_id = F.user1_id))"
		    			+ " and U1.user_id = UH1.user_id and U2.user_id = UH2.user_id and UH1.hometown_city_id is not null and UH1.hometown_city_id = UH2.hometown_city_id "
		    			+ " and U1.year_of_birth is not null and U2.year_of_birth is not null "
		    			+ " and (abs(U1.year_of_birth-U2.year_of_birth) < 10) order by U1.user_id asc , U2.user_id asc";
		    	  ResultSet rst = stmt.executeQuery(q);
		    	  while (rst.next()) 
            {
		    	  	  Long user1_id = rst.getLong(1);
		    	  	  String user1FirstName = rst.getString(2);
		    	  	  String user1LastName = rst.getString(3);
		    	  	  Long user2_id = rst.getLong(4);
		    	  	  String user2FirstName = rst.getString(5);
		    	  	  String user2LastName = rst.getString(6);
		    	  	  SiblingInfo s = new SiblingInfo(user1_id, user1FirstName, user1LastName, user2_id, user2FirstName,
		    	  	  		user2LastName);
		    	  	  this.siblings.add(s);
		    	  }
		    	  rst.close();
		    	  stmt.close();
		    } catch (SQLException err) 
        {
		    	System.err.println(err.getMessage());
		    }

    }

}
