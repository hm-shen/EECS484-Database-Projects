package project2;

import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.OutputStream;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.util.concurrent.*;
import java.util.Vector;

public class TestFakebookOracleFull {
	
	//static boolean PRINT_TIME = false;
	static boolean PRINT_TIME = true;
	static final long TIMEOUT = 65;
	static final TimeUnit TO_UNIT = TimeUnit.SECONDS;
	
	static String dataType = "PUBLIC"; 
	static String oracleUserName = "<uniqname>"; //replace with your Oracle account name
	static String password = "<password>"; //replace with your Oracle password

	static FakebookOracle fbwz;
	static OutputStreamWriter out;

	public static void main(String[] args) {
		
		try (Connection conn = getConnection()) {

			OutputStream nullOutputStream = new OutputStream() { @Override public void write(int b) { } };
			
			long startTime, duration=0;
			String curChar;

			Vector<Integer> queriesToDo = new Vector<Integer>();
			
			for(String s : args)
				for(int a = 0; a < s.length(); a++)
					try {
						curChar = String.valueOf(s.charAt(a));
						
						if(curChar.equals("H") || curChar.equals("h"))
							dataType = "PRIVATE";
						if(curChar.equals("T") || curChar.equals("t"))
							PRINT_TIME = true;
						else
							queriesToDo.add(new Integer(curChar));
					} catch(NumberFormatException e) {
						// skip what we can't parse
					}
			
			fbwz = new MyFakebookOracle(dataType, conn);
			out = new OutputStreamWriter(System.out);

			if (PRINT_TIME)
				out = new OutputStreamWriter(nullOutputStream);
			
			ExecutorService executor = Executors.newCachedThreadPool();
			
			Callable<Object> query0 = new Callable<Object>() {
			   public Object call() {
				   try {
					   fbwz.findMonthOfBirthInfo();
					   fbwz.printMonthOfBirthInfo(out);
				   } catch (SQLException e) {
						e.printStackTrace();
					} catch (IOException e) {
						e.printStackTrace();
					}
			   
				   return null;
			   }
			};
			
			Callable<Object> query1 = new Callable<Object>() {
				   public Object call() {
					   try {
						   fbwz.findNameInfo();
						   fbwz.printNameInfo(out);
					   } catch (SQLException e) {
							e.printStackTrace();
						} catch (IOException e) {
							e.printStackTrace();
						}
				   
					   return null;
				   }
			};
			
			Callable<Object> query2 = new Callable<Object>() {
				   public Object call() {
					   try {
						   fbwz.lonelyUsers();
						   fbwz.printLonelyFriends(out);
					   } catch (SQLException e) {
							e.printStackTrace();
						} catch (IOException e) {
							e.printStackTrace();
						}
				   
					   return null;
				   }
			};
			
			Callable<Object> query3 = new Callable<Object>() {
				   public Object call() {
					   try {
						   fbwz.liveAwayFromHome();
						   fbwz.printLiveAwayFromHome(out);
					   } catch (SQLException e) {
							e.printStackTrace();
						} catch (IOException e) {
							e.printStackTrace();
						}
				   
					   return null;
				   }
			};
			
			Callable<Object> query4 = new Callable<Object>() {
				   public Object call() {
					   try {
						   fbwz.findPhotosWithMostTags(5);
						   fbwz.printPhotosWithMostTags(out);
					   } catch (SQLException e) {
							e.printStackTrace();
						} catch (IOException e) {
							e.printStackTrace();
						}
				   
					   return null;
				   }
			};
			
			Callable<Object> query5 = new Callable<Object>() {
				   public Object call() {
					   try {
						   fbwz.matchMaker(5,2);
						   fbwz.printBestMatches(out);
					   } catch (SQLException e) {
							e.printStackTrace();
						} catch (IOException e) {
							e.printStackTrace();
						}
				   
					   return null;
				   }
			};
			
			Callable<Object> query6 = new Callable<Object>() {
				   public Object call() {
					   try {
						   fbwz.suggestFriendsByMutualFriends(5);
						   fbwz.printMutualFriendsInfo(out);
					   } catch (SQLException e) {
							e.printStackTrace();
						} catch (IOException e) {
							e.printStackTrace();
						}
				   
					   return null;
				   }
			};
			
			Callable<Object> query7 = new Callable<Object>() {
				   public Object call() {
					   try {
						   fbwz.findEventStates();
						   fbwz.printStateNames(out);
					   } catch (SQLException e) {
							e.printStackTrace();
						} catch (IOException e) {
							e.printStackTrace();
						}
				   
					   return null;
				   }
			};
			
			Callable<Object> query8 = new Callable<Object>() {
				   public Object call() {
					   try {
						   fbwz.findAgeInfo(215L);
						   fbwz.printAgeInfo(out);
					   } catch (SQLException e) {
							e.printStackTrace();
						} catch (IOException e) {
							e.printStackTrace();
						}
				   
					   return null;
				   }
			};
			
			Callable<Object> query9 = new Callable<Object>() {
				   public Object call() {
					   try {
						   fbwz.findPotentialSiblings();
						   fbwz.printPotentialSiblings(out);
					   } catch (SQLException e) {
							e.printStackTrace();
						} catch (IOException e) {
							e.printStackTrace();
						}
				   
					   return null;
				   }
			};
			
			Vector<Callable<Object>> query = new Vector<Callable<Object>>();
			
			query.add(query0);
			query.add(query1);
			query.add(query2);
			query.add(query3);
			query.add(query4);
			query.add(query5);
			query.add(query6);
			query.add(query7);
			query.add(query8);
			query.add(query9);
			
			if (queriesToDo.size() == 0)
				for(int a = 0; a < query.size(); a++)
					queriesToDo.add(new Integer(a));
			
			for(int i = 0; i < queriesToDo.size(); i++)
			{
				startTime = System.currentTimeMillis();
				Future<Object> future = executor.submit(query.get(queriesToDo.get(i).intValue()));
				try {
					
				   future.get(TIMEOUT, TO_UNIT);
				   duration = System.currentTimeMillis() - startTime;
				   if(PRINT_TIME)
					   System.err.println("Query " + i + " took: " + duration/1000f + " sec");
				   
				} catch (Exception e) {
					duration = System.currentTimeMillis() - startTime;
					future.cancel(true);
					System.err.println("Error: Query " + i + " timed out after " + duration/1000f + " sec");
				}
			}

			executor.shutdownNow();

			conn.close();
		} catch (SQLException e) {
			e.printStackTrace();
		}
	}
	
	public static Connection getConnection() throws SQLException{
		try {
			Class.forName("oracle.jdbc.driver.OracleDriver").newInstance();
		} catch (InstantiationException e1) {
			e1.printStackTrace();
		} catch (IllegalAccessException e1) {
			e1.printStackTrace();
		} catch (ClassNotFoundException e1) {
			e1.printStackTrace();
		}
		return DriverManager.getConnection("jdbc:oracle:thin:@forktail.dsc.umich.edu:1521:COURSEDB", oracleUserName, password);
	}
}
