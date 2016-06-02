package edu.umass.cs.msocket.apps;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.concurrent.ExecutionException;


import edu.umass.cs.msocket.MSocket;
import edu.umass.cs.msocket.MWrappedOutputStream;
import edu.umass.cs.msocket.mobility.MobilityManagerClient;

public class MSocketClientMoving {
	public static void main(String[] args) throws IOException, InterruptedException, ExecutionException
	{

		String serverName = args[0];
		int moveNumber = Integer.parseInt(args[1]);
		int myNA = Integer.parseInt(args[2]);
		int nextNA = 0;
		
		
		String fileName = "10003.txt"; 
		File f = new File(fileName);
		FileReader fileReader = new FileReader(f);
		StringBuffer sb = new StringBuffer();
		int n=0;
		String nextNode = "";
		int timeMove = 0; //in milliseconds
		

		
		try (BufferedReader br = new BufferedReader(new FileReader(f))) {
		    String line;
		    while ((line = br.readLine()) != null) {
		       if (n == moveNumber){
		    	   nextNode = line.split(" ")[0];
		    	   timeMove = Integer.parseInt(line.split(" ")[1]);
		    	   nextNA = Integer.parseInt(line.split(" ")[2]);
		    	   n+=1;
		    	   break;
		       }
		       n+=1;
		    }
		}
		
		String nVal = "" + n;
		String nextNAVal = "" + nextNA;

		//String strGUID = args[1];
		String mGUID = args[3];
		String oGUID = "10001";
		
		final byte[] myGUID = new byte[20];
		System.arraycopy(mGUID.getBytes(), 0, myGUID, 20-mGUID.length(), mGUID.length());
	  	  
		final byte[] otherGUID = new byte[20];
		System.arraycopy(oGUID.getBytes(), 0, otherGUID, 20-oGUID.length(), oGUID.length());
	  	
		
		final MSocket msock;
		if (moveNumber == 0){
			msock = new MSocket(serverName, 0, myGUID, otherGUID, myNA, 0);
		}else{
			System.out.println(myNA);
			msock =  new MSocket(serverName, 0, myGUID, otherGUID,   
					 Long.parseLong(args[4]),  Integer.parseInt(args[5]), Integer.parseInt(args[6]),
					 Integer.parseInt(args[7]),  Integer.parseInt(args[8]), Integer.parseInt(args[9]),  
					 Integer.parseInt(args[10]), Integer.parseInt(args[11]),Integer.parseInt(args[12]),
					 Integer.parseInt(args[13]), Integer.parseInt(args[14]), args[15], myNA, 1);
		}
		
		
		

		int i=0;
		
		OutputStream outstream = msock.getOutputStream();
		InputStream inpstream = msock.getInputStream();
		
		
		final String sshCmd = "ssh root@" + nextNode + " 'cd MSocketEverything/msocketSource/msocket1/dist/jars/ ";


		new java.util.Timer().schedule( 
		        new java.util.TimerTask() {
		            @Override
		            public void run() {
		            	final String command = sshCmd + " && export PATH=~/jdk1.8.0_20/bin/:$PATH "
		            			               + " && java -Dlog4j.configuration=file:./log4j.properties -cp GNS.jar:GNS-CLI.jar:. edu.umass.cs.msocket.apps.MSocketClientMoving "
		                                       + serverName +" "+ nVal +" "+ nextNAVal + " "+ msock.GNRSclose() + "'";
					    System.out.println(command);

						MobilityManagerClient.shutdownMobilityManager();
						msock.closeSockets();
						//move to other node
						Process p;
						Process p2;
						try {
						    //p2 = Runtime.getRuntime().exec("killall mfstack");
						    p = Runtime.getRuntime().exec(command);
						    //msock.close();
						    System.exit(0);
						}catch (Exception e){
							e.printStackTrace();
						}
		        		
		            }
		        }, 
		        timeMove 
		);
		
		//some arbitrary size of the total file
		int size = 1000000000;
		byte[] byteArray = new byte[MWrappedOutputStream.WRITE_CHUNK_SIZE];

		int recv = 0;
		while(recv < size*2)
		{	
			int recvOnce = inpstream.read(byteArray);
			recv += recvOnce;
			try
			{
				Thread.sleep(200);
			} catch (InterruptedException e)
			{
				e.printStackTrace();
			}
		}
		

	}
}
