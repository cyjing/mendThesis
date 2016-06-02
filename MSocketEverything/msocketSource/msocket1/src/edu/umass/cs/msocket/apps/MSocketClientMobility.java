package edu.umass.cs.msocket.apps;

import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import edu.umass.cs.msocket.MSocket;
import edu.umass.cs.msocket.mobility.MobilityManagerClient;


public class MSocketClientMobility {
	public static void main(String[] args) throws IOException
	{
		String serverName = args[0];
		//String fileName = args[1];
		String fileName = "102.txt"; 
		File f = new File(fileName);
		FileReader fileReader = new FileReader(f);
		StringBuffer sb = new StringBuffer();
		int n;
		char[] charArray = new char[1024];
		while ((n = fileReader.read(charArray)) > 0){
			sb.append(charArray,0,n);
		}
		
		String[] arguments = sb.toString().split(" ");
		
		
		String mGUID = "102";
		String oGUID = "101";
		
		final byte[] myGUID = new byte[20];
		System.arraycopy(mGUID.getBytes(), 0, mGUID, 20-mGUID.length(), mGUID.length());
	  	  
		final byte[] otherGUID = new byte[20];
		System.arraycopy(oGUID.getBytes(), 0, oGUID, 20-oGUID.length(), oGUID.length());
	  	  
		MSocket msock = new MSocket(serverName, 0, myGUID, otherGUID, Long.parseLong(arguments[0]),  
				 Integer.parseInt(arguments[1]),  Integer.parseInt(arguments[2]), Integer.parseInt(arguments[3]),
				 Integer.parseInt(arguments[4]),  Integer.parseInt(arguments[5]), Integer.parseInt(arguments[6]),  
				 Integer.parseInt(arguments[7]), Integer.parseInt(arguments[8]),Integer.parseInt(arguments[9]),
				 Integer.parseInt(arguments[10]));
		
		OutputStream outstream = msock.getOutputStream();
		InputStream inpstream = msock.getInputStream();
		
		byte[] byteArray = new byte[1000];
		
		int i=0;
		System.out.println("a____________________________________");

		while(i < 5)
		{
			//outstream.write( new String("hello world from client").getBytes() );
			inpstream.read(byteArray);
			System.out.println("trying to read");
			System.out.println(new String(byteArray) + "__B");
			
			try
			{
				Thread.sleep(2000);
			} catch (InterruptedException e)
			{
				e.printStackTrace();
			}
			
			i++;
		}
		//msock.close();
		
		MobilityManagerClient.shutdownMobilityManager();
	}
}