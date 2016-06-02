package edu.umass.cs.msocket.apps;


import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetAddress;

import org.apache.commons.lang.StringUtils;

import edu.umass.cs.msocket.MServerSocket;
import edu.umass.cs.msocket.MSocket;
import edu.umass.cs.msocket.common.policies.NoProxyPolicy;
import edu.umass.cs.msocket.mobility.MobilityManagerServer;


public class MSocketServerGSTAR {
	public static void main(String[] args) throws IOException
	{
		String serverName = args[0];
		//String strGUID = args[1];
		String strGUID = "101";
		
		byte[] myGUID = new byte[20];
		System.arraycopy(strGUID.getBytes(), 0, myGUID, 20-strGUID.length(), strGUID.length());

		MServerSocket mserv = new MServerSocket(serverName);
		
		//while(true)
		{
			MSocket msocket = mserv.accept();
			
			try {
				Thread.sleep(5000);
			} catch (InterruptedException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}
			
			byte[] byteArray = new byte[400];
			int i=0;
			/*
			while(i<1)
			{
				System.out.println("sending info to client");
				String mss = StringUtils.leftPad("hello world from client", 100, 'x');
				
				msocket.writeToOutputGSTAR(mss);
				
				try
				{
					Thread.sleep(5000);
				} catch (InterruptedException e)
				{
					e.printStackTrace();
				}
				i++;
			}*/
			msocket.close();
		}
		mserv.close();
		MobilityManagerServer.shutdownMobilityManager();	
	}
}
