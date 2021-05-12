import java.io.*;
import gnu.getopt.Getopt;
import java.net.* ;
import java.util.* ;
import java.lang.* ;



class client {
	
	/********************* TYPES **********************/
	
	/**
	 * @brief Return codes for the protocol methods
	 */
	private static enum RC {
		OK,
		ERROR,
		USER_ERROR	
	};
	
	/******************* ATTRIBUTES *******************/
	
	private static String _server   = null;
	private static int _port = -1;
    private static Thread _readMessage = null;
	
	/********************* METHODS ********************/
	
	/**
	 * @param user - User name to register in the system
	 * 
	 * @return OK if successful
	 * @return USER_ERROR if the user is already registered
	 * @return ERROR if another error occurred
	 */
	static RC register(String user) 
	{
		// Write your code here
        try{
            Socket sc = new Socket(_server,_port);
            
            DataOutputStream out = new DataOutputStream(sc.getOutputStream());
            DataInputStream in = new DataInputStream(sc.getInputStream());
            
            //System.out.println("REGISTER in progress");
            String message = new String("REGISTER");
            byte[] aux = null;
            aux = new byte[256];
            
            out.writeBytes(message);
            out.write('\0'); // insert the ASCII 0 at the end
            message = user;
            out.writeBytes(message);
            out.write('\0');
            
            in.read(aux);
            String s = new String(aux);
            System.out.println(s);
            sc.close();
            
            if ("REGISTER OK".equals(s.trim())){
                return RC.OK; //CHANGE!
            }
            else{
                return RC.ERROR;
            }
        }
        catch (Exception e)
        {
            System.err.println("Exception " + e.toString() );
            e.printStackTrace() ;
            return RC.ERROR;
        }
	}
	
	/**
	 * @param user - User name to unregister from the system
	 * 
	 * @return OK if successful
	 * @return USER_ERROR if the user does not exist
	 * @return ERROR if another error occurred
	 */
	static RC unregister(String user) 
	{
		// Write your code here
        try{
            Socket sc = new Socket(_server,_port);
            
            DataOutputStream out = new DataOutputStream(sc.getOutputStream());
            DataInputStream in = new DataInputStream(sc.getInputStream());

            
            //System.out.println("UNREGISTER in progress");
            String message = new String("UNREGISTER");
            byte[] aux = null;
            aux = new byte[256];
            
            out.writeBytes(message);
            out.write('\0'); // insert the ASCII 0 at the end
            message = user;
            out.writeBytes(message);
            out.write('\0');
            
            in.read(aux);
            String s = new String(aux);
            System.out.println(s);
            sc.close();
            
            if ("UNREGISTER OK".equals(s.trim())){
                return RC.OK; //CHANGE!
            }
            else{
                return RC.ERROR;
            }
        }
        catch (Exception e)
        {
            System.err.println("Exception " + e.toString() );
            e.printStackTrace() ;
            return RC.ERROR;
        }
	}
	
    /**
	 * @param user - User name to connect to the system
	 * 
	 * @return OK if successful
	 * @return USER_ERROR if the user does not exist or if it is already connected
	 * @return ERROR if another error occurred
	 */
	static RC connect(String user) 
	{
		// Write your code here
        try{
            Socket sc = new Socket(_server,_port); //New port number!!!!!
            
            DataOutputStream out = new DataOutputStream(sc.getOutputStream());
            DataInputStream in = new DataInputStream(sc.getInputStream());
            
            //System.out.println("CONNECT in progress");
            String message = new String("CONNECT");
            byte[] aux = null;
            aux = new byte[256];
            
            out.writeBytes(message);
            out.write('\0'); // insert the ASCII 0 at the end
            message = user;
            out.writeBytes(message);
            out.write('\0');
            
            in.read(aux);
            String s = new String(aux);
            System.out.println(s);
            
            int port_number = aux[0];
            
            in.read(aux);
            s = new String(aux);
            System.out.println(s);
            
            sc.close();
            
            //AFTER CONNECTING THE THREAD NEEDS TO START RUNNING TO ACCEPT MESSAGES
            if ("CONNECT OK".equals(s.trim())){
                Socket sc_rec = new Socket(_server,port_number); //I am trying to connect to 42005
                
                _readMessage = new Thread(new Runnable()
                                          {
                    @Override
                    public void run() {
                        
                        while (true) {
                            try {
                                DataInputStream stream = new DataInputStream(sc_rec.getInputStream());
                                byte[] mess = null;
                                mess = new byte[256];
                                stream.read(mess);
                                String msg= new String(mess);
                                System.out.println(msg);
                                System.out.flush();
                            } catch (IOException e) {
                                e.printStackTrace();
                            }
                        }
                    }
                });
                
                
                _readMessage.start();
                System.out.println("read message started");
            }
            else if ("CONNECT FAIL".equals(s.trim())){
                return RC.ERROR; //CHANGE!
            }
            
            else if ("USER ALREADY CONNECTED".equals(s.trim())){
                return RC.ERROR; //CHANGE!
            }
            else if ("CONNECT FAIL, USER DOES NOT EXIST".equals(s.trim())){
                return RC.ERROR; //CHANGE!
            }

            return RC.OK;
        }
        catch (Exception e)
        {
            System.err.println("Exception " + e.toString() );
            e.printStackTrace() ;
            return RC.ERROR;
        }
	}
	
	 /**
	 * @param user - User name to disconnect from the system
	 * 
	 * @return OK if successful
	 * @return USER_ERROR if the user does not exist
	 * @return ERROR if another error occurred
	 */
	static RC disconnect(String user) //THIS NEEDS TO TERMINATE THE THREAD SOMEHOW
	{
		// Write your code here
        try{
            Socket sc = new Socket(_server,_port); //New port number!!!!!
            
            DataOutputStream out = new DataOutputStream(sc.getOutputStream());
            DataInputStream in = new DataInputStream(sc.getInputStream());
            
            
            //System.out.println("DISCONNECT in progress");
            String message = new String("DISCONNECT");
            byte[] aux = null;
            aux = new byte[256];
            
            out.writeBytes(message);
            out.write('\0'); // insert the ASCII 0 at the end
            message = user;
            out.writeBytes(message);
            out.write('\0');
            
            in.read(aux);
            String s = new String(aux);
            System.out.println(s);
            sc.close();
            
            if ("DISCONNECT OK".equals(s.trim())){
                _readMessage.stop();
                return RC.ERROR; //CHANGE!
            }
            else if ("DISCONNECT FAIL".equals(s.trim())){
                return RC.ERROR; //CHANGE!
            }
            else if ("DISCONNECT FAIL/ USER NOT CONNECTED".equals(s.trim())){
                return RC.ERROR; //CHANGE!
            }
            else if ("DISCONNECT FAIL/ USER DOES NOT EXIST".equals(s.trim())){
                return RC.ERROR; //CHANGE!
            }
            else{
                 return RC.ERROR; //CHANGE!
            }
        }
        catch (Exception e)
        {
            System.err.println("Exception " + e.toString() );
            e.printStackTrace() ;
            return RC.ERROR;
        }
	}

	 /**
	 * @param user    - Receiver user name
	 * @param message - Message to be sent
	 * 
	 * @return OK if the server had successfully delivered the message
	 * @return USER_ERROR if the user is not connected (the message is queued for delivery)
	 * @return ERROR the user does not exist or another error occurred
	 */
	static RC send(String user, String message) 
	{
		// Write your code here
		return RC.ERROR;
	}
	
	/**
	 * @brief Command interpreter for the client. It calls the protocol functions.
	 */
	static void shell() 
	{
		boolean exit = false;
		String input;
		String [] line;
		BufferedReader in = new BufferedReader(new InputStreamReader(System.in));
		while (!exit) {
			try {
				System.out.print("c> ");
				input = in.readLine();
				line = input.split("\\s");

				if (line.length > 0) {
					/*********** REGISTER *************/
					if (line[0].equals("REGISTER")) {
						if  (line.length == 2) {
							register(line[1]); // userName = line[1]
						} else {
							System.out.println("Syntax error. Usage: REGISTER <userName>");
						}
					} 
					
					/********** UNREGISTER ************/
					else if (line[0].equals("UNREGISTER")) {
						if  (line.length == 2) {
							unregister(line[1]); // userName = line[1]
						} else {
							System.out.println("Syntax error. Usage: UNREGISTER <userName>");
						}
                    } 
                    
                    /************ CONNECT *************/
                    else if (line[0].equals("CONNECT")) {
						if  (line.length == 2) {
							connect(line[1]); // userName = line[1]
                            
                            
						} else {
							System.out.println("Syntax error. Usage: CONNECT <userName>");
                    	}
                    } 
                    
                    /********** DISCONNECT ************/
                    else if (line[0].equals("DISCONNECT")) {
						if  (line.length == 2) {
							disconnect(line[1]); // userName = line[1]
						} else {
							System.out.println("Syntax error. Usage: DISCONNECT <userName>");
                    	}
                    } 
                    
                    /************** SEND **************/
                    else if (line[0].equals("SEND")) {
						if  (line.length >= 3) {
							// Remove first two words
							String message = input.substring(input.indexOf(' ')+1).substring(input.indexOf(' ')+1);
							send(line[1], message); // userName = line[1]
						} else {
							System.out.println("Syntax error. Usage: SEND <userName> <message>");
                    	}
                    } 
                    
                    /************** QUIT **************/
                    else if (line[0].equals("QUIT")){
						if (line.length == 1) {
							exit = true;
						} else {
							System.out.println("Syntax error. Use: QUIT");
						}
					} 
					
					/************* UNKNOWN ************/
					else {						
						System.out.println("Error: command '" + line[0] + "' not valid.");
					}
				}				
			} catch (java.io.IOException e) {
				System.out.println("Exception: " + e);
				e.printStackTrace();
			}
		}
	}
	
	/**
	 * @brief Prints program usage
	 */
	static void usage() 
	{
		System.out.println("Usage: java -cp . client -s <server> -p <port>");
	}
	
	/**
	 * @brief Parses program execution arguments 
	 */ 
	static boolean parseArguments(String [] argv) 
	{
		Getopt g = new Getopt("client", argv, "ds:p:");

		int c;
		String arg;

		while ((c = g.getopt()) != -1) {
			switch(c) {
				//case 'd':
				//	_debug = true;
				//	break;
				case 's':
					_server = g.getOptarg();
					break;
				case 'p':
					arg = g.getOptarg();
					_port = Integer.parseInt(arg);
					break;
				case '?':
					System.out.print("getopt() returned " + c + "\n");
					break; // getopt() already printed an error
				default:
					System.out.print("getopt() returned " + c + "\n");
			}
		}
		
		if (_server == null)
			return false;
		
		if ((_port < 1024) || (_port > 65535)) {
			System.out.println("Error: Port must be in the range 1024 <= port <= 65535");
			return false;
		}

		return true;
	}
	
	
	
	/********************* MAIN **********************/
	
	public static void main(String[] argv) 
	{
		if(!parseArguments(argv)) {
			usage();
			return;
		}
		
		// Write code here
		shell();
		System.out.println("+++ FINISHED +++");
	}
}
