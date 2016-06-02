package edu.umass.cs.msocket.apps;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.UnknownHostException;
import java.time.Duration;
import java.util.concurrent.*;

import edu.umass.cs.msocket.InBufferOutOrder;
import edu.umass.cs.msocket.MSocket;
import edu.umass.cs.msocket.MWrappedInputStream;
import edu.umass.cs.msocket.mobility.MobilityManagerClient;


public class MSocketMovingClient {
	public static void main(String[] args) throws IOException, InterruptedException, ExecutionException
	{

		String serverName = args[0];
		int timeStamp = Integer.parseInt(args[1]);
		
		
		String fileName = "102.txt"; 
		File f = new File(fileName);
		FileReader fileReader = new FileReader(f);
		StringBuffer sb = new StringBuffer();
		int n=0;
		String nextNode = "";
		int timeMove = 0; //in milliseconds
		
		try (BufferedReader br = new BufferedReader(new FileReader(f))) {
		    String line;
		    while ((line = br.readLine()) != null) {
		       if (n == timeStamp){
		    	   nextNode = line.split(" ")[0];
		    	   timeMove = Integer.parseInt(line.split(" ")[1]);
		    	   break;
		       }
		       n+=1;
		    }
		}
		
		
		//String strGUID = args[1];
		String mGUID = "102";
		String oGUID = "101";
		
		final byte[] myGUID = new byte[20];
		System.arraycopy(mGUID.getBytes(), 0, myGUID, 20-mGUID.length(), mGUID.length());
	  	  
		final byte[] otherGUID = new byte[20];
		System.arraycopy(oGUID.getBytes(), 0, otherGUID, 20-oGUID.length(), oGUID.length());
	  	  
		final MSocket msock = new MSocket(serverName, 0, myGUID, otherGUID);
		
		byte[] byteArray = new byte[1000];

		int i=0;
		
		OutputStream outstream = msock.getOutputStream();
		InputStream inpstream = msock.getInputStream();
		final String sshCmd = "ssh root@" + nextNode + " 'cd MSocketEverything/msocketSource/msocket1/dist/jars/ ";


		new java.util.Timer().schedule( 
		        new java.util.TimerTask() {
		            @Override
		            public void run() {
		            	final String command = sshCmd + " && java -cp GNRS.jar:GNS-CLI.jar:. edu.umass.cs.msocket.apps.MSocketClientMobility "
		                                       + msock.GNRSclose() + "'";
						
						MobilityManagerClient.shutdownMobilityManager();
						//move to other node
						Process p;
						try {
						    p = Runtime.getRuntime().exec(command);
						}catch (Exception e){
							e.printStackTrace();
						}
		        		
		            }
		        }, 
		        timeMove 
		);
		
		//some arbitrary size of the total file
		int size = 10;
		int recv = 0;
		while(recv < size)
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
