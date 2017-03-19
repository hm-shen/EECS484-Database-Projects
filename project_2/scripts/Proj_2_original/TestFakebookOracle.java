package project2;

import java.io.IOException;
import java.io.OutputStreamWriter;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;

public class TestFakebookOracle {

    static String dataType = "PUBLIC";
    static String oracleUserName = "username"; //replace with your Oracle account name
    static String password = "password"; //replace with your Oracle password

    public static void main(String[] args) {

        try (Connection conn = getConnection()) {
            FakebookOracle fbwz = new MyFakebookOracle(dataType, conn);
            OutputStreamWriter out = new OutputStreamWriter(System.out);

            // Query 0
            out.write(">>>>>>>>>Query0>>>>>>>>>>\n");
            fbwz.findMonthOfBirthInfo();
            fbwz.printMonthOfBirthInfo(out);

            // Query 1
            out.write(">>>>>>>>>Query1>>>>>>>>>>\n");
            fbwz.findNameInfo();
            fbwz.printNameInfo(out);

            // Query 2
            out.write(">>>>>>>>>Query2>>>>>>>>>>\n");
            fbwz.lonelyUsers();
            fbwz.printLonelyFriends(out);

            // Query 3
            out.write(">>>>>>>>>Query3>>>>>>>>>>\n");
            fbwz.liveAwayFromHome();
            fbwz.printLiveAwayFromHome(out);

            // Query 4
            out.write(">>>>>>>>>Query4>>>>>>>>>>\n");
            fbwz.findPhotosWithMostTags(5);
            fbwz.printPhotosWithMostTags(out);

            // Query 5
            out.write(">>>>>>>>>Query5>>>>>>>>>>\n");
            fbwz.matchMaker(5, 2);
            fbwz.printBestMatches(out);

            // Query 6
            out.write(">>>>>>>>>Query6>>>>>>>>>>\n");
            fbwz.suggestFriendsByMutualFriends(5);
            fbwz.printMutualFriendsInfo(out);

            // Query 7
            out.write(">>>>>>>>>Query7>>>>>>>>>>\n");
            fbwz.findEventStates();
            fbwz.printStateNames(out);

            // Query 8
            out.write(">>>>>>>>>Query8>>>>>>>>>>\n");
            fbwz.findAgeInfo(215L);
            fbwz.printAgeInfo(out);

            // Query 9
            out.write(">>>>>>>>>Query9>>>>>>>>>>\n");
            fbwz.findPotentialSiblings();
            fbwz.printPotentialSiblings(out);


            conn.close();
        } catch (SQLException | IOException e) {
            e.printStackTrace();
        }
    }

    public static Connection getConnection() throws SQLException {
        try {
            Class.forName("oracle.jdbc.driver.OracleDriver").newInstance();
        } catch (InstantiationException | IllegalAccessException e1) {
            e1.printStackTrace();
        } catch (ClassNotFoundException e1) {
            e1.printStackTrace();
        }
        return DriverManager.getConnection("jdbc:oracle:thin:@forktail.dsc.umich.edu:1521:COURSEDB", oracleUserName, password);
    }
}