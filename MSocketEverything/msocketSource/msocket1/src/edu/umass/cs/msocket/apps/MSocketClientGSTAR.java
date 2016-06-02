package edu.umass.cs.msocket.apps;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetAddress;

import org.apache.commons.lang.StringUtils;

import edu.umass.cs.msocket.MSocket;
import edu.umass.cs.msocket.mobility.MobilityManagerClient;


public class MSocketClientGSTAR {
	public static void main(String[] args) throws IOException
	{
		String serverName = args[0];
		//String strGUID = args[1];
		String mGUID = "102";
		String oGUID = "101";
		
		final byte[] myGUID = new byte[20];
		System.arraycopy(mGUID.getBytes(), 0, myGUID, 20-mGUID.length(), mGUID.length());
	  	  
		final byte[] otherGUID = new byte[20];
		System.arraycopy(oGUID.getBytes(), 0, otherGUID, 20-oGUID.length(), oGUID.length());
	  	  
		MSocket msock = new MSocket(serverName, 0, myGUID, otherGUID, 0);
		
		byte[] byteArray = new byte[1000];

		int i=0;
		
		OutputStream outstream = msock.getOutputStream();
		InputStream inpstream = msock.getInputStream();
		
		while(i < 6)
		{
			System.out.println("reading output from server");
			inpstream.read(byteArray);
			System.out.println(new String(byteArray));
			
			try
			{
				Thread.sleep(2000);
			} catch (InterruptedException e)
			{
				e.printStackTrace();
			}
			
			i++;
		}
		msock.close();
		
		MobilityManagerClient.shutdownMobilityManager();
	}
}
