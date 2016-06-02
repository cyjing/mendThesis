package edu.umass.cs.msocket.apps;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import edu.umass.cs.msocket.MServerSocket;
import edu.umass.cs.msocket.MSocket;
import edu.umass.cs.msocket.mobility.MobilityManagerServer;

public class MSocketServer 
{
	public static void main(String[] args) throws IOException
	{
		String serverName = args[0];
		//String strGUID = args[1];
		String strGUID = "10001";
		byte[] myGUID = new byte[20];
		System.arraycopy(strGUID.getBytes(), 0, myGUID, 20-strGUID.length(), strGUID.length());
		MServerSocket mserv = new MServerSocket(serverName);
				
		//while(true)
		{
			MSocket msocket = mserv.accept();
			OutputStream outstream = msocket.getOutputStream();
			InputStream inpstream = msocket.getInputStream();
			
			int i=0;
			
			try
			{
				Thread.sleep(500);
			} catch (InterruptedException e)
			{
				e.printStackTrace();
			}
			
			while(i<10)
			{
				byte[] largeChunk = new byte[1000000];
				outstream.write( largeChunk );
				//int numRead = inpstream.read(byteArray);
				//System.out.println(new String(byteArray));
				
				try
				{
					Thread.sleep(500);
				} catch (InterruptedException e)
				{
					e.printStackTrace();
				}
				i++;
			}
			msocket.close();
		}
		mserv.close();
		MobilityManagerServer.shutdownMobilityManager();	
	}
}